
#include "Ridges_Umbilics.h"

// This is an automatically generated header file based on theDsFile, below,
// to provide Heat_method_3Parms, an easy way to access parameter values from
// Heat_method_3Verb::cook with the correct type.
#include "Ridges_Umbilics.proto.h"

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Director.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include "PolyhedralSurf_rings.h"
#include "compute_normals.h"
#include <CGAL/Ridges.h>
#include <CGAL/Umbilics.h>
#include <CGAL/Monge_via_jet_fitting.h>
#include <fstream>
#include <cassert>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace HDK_Ridges_Umbilics;

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::FT                      FT;
typedef Kernel::Point_3                 Point_3;
typedef Kernel::Vector_3                Vector_3;

typedef CGAL::Surface_mesh<Point_3> PolyhedralSurf;

typedef boost::graph_traits<PolyhedralSurf>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<PolyhedralSurf>::vertex_iterator   vertex_iterator;
typedef boost::graph_traits<PolyhedralSurf>::face_descriptor   face_descriptor;

typedef T_PolyhedralSurf_rings<PolyhedralSurf> Poly_rings;
typedef CGAL::Monge_via_jet_fitting<Kernel>    Monge_via_jet_fitting;
typedef Monge_via_jet_fitting::Monge_form      Monge_form;

typedef PolyhedralSurf::Property_map<vertex_descriptor, FT> VertexFT_property_map;
typedef PolyhedralSurf::Property_map<vertex_descriptor, Vector_3> VertexVector_property_map;
//RIDGES
typedef CGAL::Ridge_line<PolyhedralSurf> Ridge_line;
typedef CGAL::Ridge_approximation < PolyhedralSurf,
	VertexFT_property_map,
	VertexVector_property_map > Ridge_approximation;
//UMBILICS
typedef CGAL::Umbilic<PolyhedralSurf> Umbilic;
typedef CGAL::Umbilic_approximation < PolyhedralSurf,
	VertexFT_property_map,
	VertexVector_property_map > Umbilic_approximation;

//create property maps

PolyhedralSurf::Property_map<vertex_descriptor, FT>
vertex_k1_pm, vertex_k2_pm,
vertex_b0_pm, vertex_b3_pm,
vertex_P1_pm, vertex_P2_pm;

PolyhedralSurf::Property_map<vertex_descriptor, Vector_3> vertex_d1_pm, vertex_d2_pm;

PolyhedralSurf::Property_map<face_descriptor, Vector_3> face2normal_pm;

// default fct parameter values and global variables
unsigned int d_fitting = 3;
unsigned int d_monge = 3;
unsigned int nb_rings = 0;//seek min # of rings to get the required #pts
unsigned int nb_points_to_use = 0;//
CGAL::Ridge_order tag_order = CGAL::Ridge_order_3;
double umb_size = 2;
bool verbose = false;
unsigned int min_nb_points = (d_fitting + 1) * (d_fitting + 2) / 2;

template <typename VertexPointMap>
void gather_fitting_points(vertex_descriptor v,
	std::vector<Point_3> &in_points,
	Poly_rings& poly_rings,
	VertexPointMap vpm)
{
	//container to collect vertices of v on the PolyhedralSurf
	std::vector<vertex_descriptor> gathered;
	//initialize
	in_points.clear();

	//OPTION -p nb_points_to_use, with nb_points_to_use != 0. Collect
	//enough rings and discard some points of the last collected ring to
	//get the exact "nb_points_to_use"
	if (nb_points_to_use != 0) {
		poly_rings.collect_enough_rings(v, nb_points_to_use, gathered);
		if (gathered.size() > nb_points_to_use) gathered.resize(nb_points_to_use);
	}
	else { // nb_points_to_use=0, this is the default and the option -p is not considered;
	  // then option -a nb_rings is checked. If nb_rings=0, collect
	  // enough rings to get the min_nb_points required for the fitting
	  // else collect the nb_rings required
		if (nb_rings == 0)
			poly_rings.collect_enough_rings(v, min_nb_points, gathered);
		else poly_rings.collect_i_rings(v, nb_rings, gathered);
	}

	//store the gathered points
	std::vector<vertex_descriptor>::const_iterator
		itb = gathered.begin(), ite = gathered.end();
	CGAL_For_all(itb, ite) in_points.push_back(get(vpm, *itb));
}

void compute_differential_quantities(PolyhedralSurf& P, Poly_rings& poly_rings)
{
	//container for approximation points
	std::vector<Point_3> in_points;

	typedef boost::property_map<PolyhedralSurf, CGAL::vertex_point_t>::type VPM;
	VPM vpm = get(CGAL::vertex_point, P);

	//MAIN LOOP
	vertex_iterator vitb = P.vertices_begin(), vite = P.vertices_end();
	for (; vitb != vite; vitb++) {
		//initialize
		vertex_descriptor v = *vitb;
		in_points.clear();
		Monge_form monge_form;
		Monge_via_jet_fitting monge_fit;

		//gather points around the vertex using rings
		gather_fitting_points(v, in_points, poly_rings, vpm);

		//exit if the nb of points is too small
		if (in_points.size() < min_nb_points)
		{
			std::cerr << "Too few points to perform the fitting" << std::endl; exit(1);
		}

		//For Ridges we need at least 3rd order info
		assert(d_monge >= 3);
		// run the main fct : perform the fitting
		monge_form = monge_fit(in_points.begin(), in_points.end(),
			d_fitting, d_monge);

		//switch min-max ppal curv/dir wrt the mesh orientation
		const Vector_3 normal_mesh = computeFacetsAverageUnitNormal(P, v, face2normal_pm, Kernel());
		monge_form.comply_wrt_given_normal(normal_mesh);

		//Store monge data needed for ridge computations in property maps
		vertex_d1_pm[v] = monge_form.maximal_principal_direction();
		vertex_d2_pm[v] = monge_form.minimal_principal_direction();
		vertex_k1_pm[v] = monge_form.coefficients()[0];
		vertex_k2_pm[v] = monge_form.coefficients()[1];
		vertex_b0_pm[v] = monge_form.coefficients()[2];
		vertex_b3_pm[v] = monge_form.coefficients()[5];
		if (d_monge >= 4) {
			//= 3*b1^2+(k1-k2)(c0-3k1^3)
			vertex_P1_pm[v] =
				3 * monge_form.coefficients()[3] * monge_form.coefficients()[3]
				+ (monge_form.coefficients()[0] - monge_form.coefficients()[1])
				*(monge_form.coefficients()[6]
					- 3 * monge_form.coefficients()[0] * monge_form.coefficients()[0]
					* monge_form.coefficients()[0]);
			//= 3*b2^2+(k2-k1)(c4-3k2^3)
			vertex_P2_pm[v] =
				3 * monge_form.coefficients()[4] * monge_form.coefficients()[4]
				+ (-monge_form.coefficients()[0] + monge_form.coefficients()[1])
				*(monge_form.coefficients()[10]
					- 3 * monge_form.coefficients()[1] * monge_form.coefficients()[1]
					* monge_form.coefficients()[1]);
		}
	} //END FOR LOOP
}
//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder Ridges_Umbilics::theSOPTypeName("Ridges_Umbilics"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        Ridges_Umbilics::theSOPTypeName,   // Internal name
        "CGAL_Ridges_Umbilics",                     // UI name
        Ridges_Umbilics::myConstructor,    // How to build the SOP
        Ridges_Umbilics::buildTemplates(), // My parameters
        0,                          // Min # of sources
        1,                          // Max # of sources
        nullptr,                    // Custom local variables (none)
        OP_FLAG_GENERATOR));        // Flag it as generator
}

/// This is a multi-line raw string specifying the parameter interface
/// for this SOP.
//


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
	parm {
			name    "shourcept"      // Internal parameter name
			label   "Shourcept" // Descriptive parameter name for user interface
			type    integer
			default { "0" }     // Default for this parameter on new nodes
			range   { 0! 100000 }   // The value is prevented from going below 2 at all.
								// The UI slider goes up to 50, but the value can go higher.
			export  all         // This makes the parameter show up in the toolbox
								// above the viewport when it's in the node's state.
		}
	parm {
				name    "umb_size"      // Internal parameter name
				label   "umb_size" // Descriptive parameter name for user interface
				type    integer
				default { "2" }     // Default for this parameter on new nodes
				range   { 0! 100 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
	parm {
				name    "nb_points_to_use"      // Internal parameter name
				label   "nb_points_to_use" // Descriptive parameter name for user interface
				type    integer
				default { "0" }     // Default for this parameter on new nodes
				range   { 0! 10 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
	parm {
				name    "nb_rings"      // Internal parameter name
				label   "nb_rings" // Descriptive parameter name for user interface
				type    integer
				default { "0" }     // Default for this parameter on new nodes
				range   { 0! 10 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
parm {
				name    "d_monge"      // Internal parameter name
				label   "d_monge" // Descriptive parameter name for user interface
				type    integer
				default { "3" }     // Default for this parameter on new nodes
				range   { 0! 10 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
	parm {
				name    "d_fitting"      // Internal parameter name
				label   "d_fitting" // Descriptive parameter name for user interface
				type    integer
				default { "3" }     // Default for this parameter on new nodes
				range   { 0! 10 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
	parm {
				name    "tag_order"      // Internal parameter name
				label   "tag_order" // Descriptive parameter name for user interface
				type    integer
				default { "3" }     // Default for this parameter on new nodes
				range   { 3! 4 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
}
)THEDSFILE";

PRM_Template*
Ridges_Umbilics::buildTemplates()
{
    static PRM_TemplateBuilder templ("Ridges_Umbilics.C"_sh, theDsFile);
    return templ.templates();
}

class Ridges_UmbilicsVerb : public SOP_NodeVerb
{
public:
	Ridges_UmbilicsVerb() {}
    virtual ~Ridges_UmbilicsVerb() {}

    virtual SOP_NodeParms *allocParms() const { return new Ridges_UmbilicsParms(); }
    virtual UT_StringHolder name() const { return Ridges_Umbilics::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<Ridges_UmbilicsVerb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<Ridges_UmbilicsVerb> Ridges_UmbilicsVerb::theVerb;

const SOP_NodeVerb *
Ridges_Umbilics::cookVerb() const 
{ 
    return Ridges_UmbilicsVerb::theVerb.get();
}

OP_ERROR HDK_Ridges_Umbilics::Ridges_Umbilics::cookMySop(OP_Context &context)
{
	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();

	// Duplicate input geometry
	duplicateSource(0, context);

	// Flag the SOP as being time dependent (i.e. cook on time changes)
	flags().setTimeDep(true);
	return cookMyselfAsVerb(context);
}


/// This is the function that does the actual work.
void
Ridges_UmbilicsVerb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<Ridges_UmbilicsParms>();
    GU_Detail *detail = cookparms.gdh().gdpNC();

    // We need two points per division
   
	int32 sourcePtnum = sopparms.getShourcept();
	////////////////////////////////////////////////////////////////////////
	d_fitting = sopparms.getD_fitting();
	d_monge = sopparms.getD_monge();
	nb_rings = sopparms.getNb_rings();//seek min # of rings to get the required #pts
	nb_points_to_use = sopparms.getNb_points_to_use();//
	//CGAL::Ridge_order tag_order = CGAL::Ridge_order_3;
	umb_size = sopparms.getUmb_size();
	int64 tempTagOd = sopparms.getTag_order();
	if (tempTagOd == 3)
	{
		tag_order = CGAL::Ridge_order_3;
	}
	else
	{
		tag_order = CGAL::Ridge_order_4;
	}
	/////////////////////////////////////////////////////////////////////////
	PolyhedralSurf P;
	//Triangle_mesh tm;
	GA_Offset ptoff;
	std::vector<Point_3> points;
	
	GA_RWHandleV3 Phandle(detail->findAttribute(GA_ATTRIB_POINT, "P"));
	std::vector<vertex_descriptor> indexmap;
	GEO_Primitive *prim;

	GA_FOR_ALL_PTOFF(detail, ptoff)
	{
		UT_Vector3 Pvalue = Phandle.get(ptoff);
		Point_3 ptTemp = Point_3(Pvalue.x(), Pvalue.y(), Pvalue.z());
		vertex_descriptor u = P.add_vertex(ptTemp);
		indexmap.push_back(u);
	}
	std::vector<int32> ptoffsetmap;

	GA_FOR_ALL_PRIMITIVES(detail, prim)
	{
		GA_Range ptRange = prim->getPointRange(false);
		if (ptRange.getEntries() != 3)
		{
			cookparms.sopAddWarning(SOP_MESSAGE, "Input mesh must be Triangle_mesh!");
			return;
		}
		int32 size = 0;
		for (GA_Iterator it(ptRange); !it.atEnd(); ++it)
		{
			GA_Offset    ptoffset = (*it);
			ptoffsetmap.push_back((int32)ptoffset);
		}
	}
	for (size_t i = 0; i < ptoffsetmap.size() / 3; i++)
	{
		P.add_face(indexmap[ptoffsetmap[i * 3]], indexmap[ptoffsetmap[i * 3 + 1]], indexmap[ptoffsetmap[i * 3 + 2]]);
	}
	detail->clearAndDestroy();
	detail->addFloatTuple(GA_ATTRIB_POINT, "line_type", 1);
	GA_RWHandleF line_typeHandl(detail, GA_ATTRIB_POINT, "line_type");
	detail->addFloatTuple(GA_ATTRIB_POINT, "sharpness", 1);
	GA_RWHandleF sharpnessHandl(detail, GA_ATTRIB_POINT, "sharpness");
	detail->addFloatTuple(GA_ATTRIB_POINT, "strength", 1);
	GA_RWHandleF strengthHandl(detail, GA_ATTRIB_POINT, "strength");
	detail->addFloatTuple(GA_ATTRIB_POINT, "isUmbilics", 1);
	GA_RWHandleF isUmbilicsHandl(detail, GA_ATTRIB_POINT, "isUmbilics");

	min_nb_points = (d_fitting + 1) * (d_fitting + 2) / 2;
	
	std::string of_name = "temp.off";
	std::ostringstream str_4ogl;
	str_4ogl << "data/"
		<< of_name << "RIDGES"
		<< "-d" << d_fitting
		<< "-m" << d_monge
		<< "-t" << tag_order
		<< "-a" << nb_rings
		<< "-p" << nb_points_to_use
		<< ".4ogl.txt";
	std::cout << str_4ogl.str() << std::endl;
	std::ofstream out_4ogl(str_4ogl.str().c_str(), std::ios::out);
	//if verbose only...
	std::ostringstream str_verb;
	str_verb << "data/"
		<< of_name << "RIDGES"
		<< "-d" << d_fitting
		<< "-m" << d_monge
		<< "-t" << tag_order
		<< "-a" << nb_rings
		<< "-p" << nb_points_to_use
		<< ".verb.txt";
	std::cout << str_verb.str() << std::endl;
	std::ofstream out_verb(str_verb.str().c_str(), std::ios::out);
	
	std::ifstream stream(of_name.c_str());
	
	fprintf(stderr, "loadMesh %d Ves %d Facets\n",
		(int)num_vertices(P), (int)num_faces(P));
	if (verbose)
		out_verb << "Polysurf with " << num_vertices(P)
		<< " vertices and " << num_faces(P)
		<< " facets. " << std::endl;

	
	vertex_k1_pm = P.add_property_map<vertex_descriptor, FT>("v:k1", 0).first;
	vertex_k2_pm = P.add_property_map<vertex_descriptor, FT>("v:k2", 0).first;
	vertex_b0_pm = P.add_property_map<vertex_descriptor, FT>("v:b0", 0).first;
	vertex_b3_pm = P.add_property_map<vertex_descriptor, FT>("v:b3", 0).first;
	vertex_P1_pm = P.add_property_map<vertex_descriptor, FT>("v:P1", 0).first;
	vertex_P2_pm = P.add_property_map<vertex_descriptor, FT>("v:P2", 0).first;
	vertex_d1_pm = P.add_property_map<vertex_descriptor, Vector_3>("v:d1", Vector_3(0, 0, 0)).first;
	vertex_d2_pm = P.add_property_map<vertex_descriptor, Vector_3>("v:d2", Vector_3(0, 0, 0)).first;

	face2normal_pm = P.add_property_map<face_descriptor, Vector_3>("f:n", Vector_3(0, 0, 0)).first;
	//exit if not enough points in the model
	if (min_nb_points > num_vertices(P))
	{
		std::cerr << "not enough points in the model" << std::endl;   exit(1);
	}

	//initialize Polyhedral data : normal of facets
	compute_facets_normals(P, face2normal_pm, Kernel());
	//create a Poly_rings object
	Poly_rings poly_rings(P);

	std::cout << "Compute differential quantities via jet fitting..." << std::endl;
	//initialize the diff quantities property maps
	compute_differential_quantities(P, poly_rings);
	std::cout << "Compute ridges..." << std::endl;
	Ridge_approximation ridge_approximation(P,
		vertex_k1_pm, vertex_k2_pm,
		vertex_b0_pm, vertex_b3_pm,
		vertex_d1_pm, vertex_d2_pm,
		vertex_P1_pm, vertex_P2_pm);
	std::vector<Ridge_line*> ridge_lines;
	std::back_insert_iterator<std::vector<Ridge_line*> > ii(ridge_lines);

	//Find MAX_RIDGE, MIN_RIDGE, CREST_RIDGES
	//   ridge_approximation.compute_max_ridges(ii, tag_order);
	//   ridge_approximation.compute_min_ridges(ii, tag_order);
	ridge_approximation.compute_crest_ridges(ii, tag_order);
	// or with the global function
	CGAL::compute_max_ridges(P,
		vertex_k1_pm, vertex_k2_pm,
		vertex_b0_pm, vertex_b3_pm,
		vertex_d1_pm, vertex_d2_pm,
		vertex_P1_pm, vertex_P2_pm,
		ii, tag_order);
	std::vector<Ridge_line*>::iterator iter_lines = ridge_lines.begin(),iter_end = ridge_lines.end();
	typedef boost::property_map<PolyhedralSurf, CGAL::vertex_point_t>::type VPM;
	VPM vpm = get(CGAL::vertex_point, P);

	typedef typename Kernel::FT         FT;
	typedef typename boost::graph_traits<PolyhedralSurf>::halfedge_descriptor halfedge_descriptor;
	typedef std::pair< halfedge_descriptor, FT> Ridge_halfedge;
	typedef std::pair< halfedge_descriptor, FT> Umbilic_halfedge;
	
	for (; iter_lines != iter_end; iter_lines++)
	{
		Ridge_line* line = (*iter_lines);
		std::list<Ridge_halfedge>* hafline = line->line();
		GEO_PrimPoly *poly = (GEO_PrimPoly *)detail->appendPrimitive(GA_PRIMPOLY);
		for (auto it = hafline->begin(); it != hafline->end(); it++)
		{
			const Ridge_halfedge& i = *it;
			Point_3 p = get(vpm, target(opposite(i.first, P), P)), q = get(vpm, target(i.first, P));
			Point_3 r = CGAL::barycenter(p, i.second, q);
			int ptNumber1 = detail->appendPointOffset();
			UT_Vector3 PvalueNew1(r.hx(), r.hy(), r.hz());
			Phandle.set(ptNumber1, PvalueNew1);
			
			strengthHandl.set(ptNumber1, line->strength());
			line_typeHandl.set(ptNumber1, (int32)(line->line_type()));
			sharpnessHandl.set(ptNumber1, line->sharpness());
			isUmbilicsHandl.set(ptNumber1, 0);
			poly->appendVertex(ptNumber1);
		}
		detail->bumpDataIdsForAddOrRemove(true, true, true);
	} 

	////---------------------------------------------------------------------------
 // // UMBILICS
 // //--------------------------------------------------------------------------
	std::cout << "Compute umbilics..." << std::endl;
	std::vector<Umbilic*> umbilics;
	std::back_insert_iterator<std::vector<Umbilic*> > umb_it(umbilics);

	CGAL::compute_umbilics(P,
		vertex_k1_pm, vertex_k2_pm,
		vertex_d1_pm, vertex_d2_pm,
		umb_it, umb_size);

	std::vector<Umbilic*>::iterator iter_umb = umbilics.begin(),
		iter_umb_end = umbilics.end();
	// output
	for (iter_umb = umbilics.begin(); iter_umb != iter_umb_end; iter_umb++) {
		if (verbose) {
			out_verb << **iter_umb;
		}
		Umbilic* UmbilicEdge = *iter_umb;
		const std::list<halfedge_descriptor>* hafline = &(UmbilicEdge->contour_list());
		GEO_PrimPoly *poly = (GEO_PrimPoly *)detail->appendPrimitive(GA_PRIMPOLY);
		int32 umbilic_type = (int32)UmbilicEdge->umbilic_type();
		for (auto it = hafline->begin(); it != hafline->end(); it++)
		{
			const halfedge_descriptor& i = *it;
			
			Point_3 p = get(vpm, target(opposite(i, P), P)), q = get(vpm, target(i, P));
			int ptNumber1 = detail->appendPointOffset();
			UT_Vector3 PvalueNew1(p.hx(), p.hy(), p.hz());
			Phandle.set(ptNumber1, PvalueNew1);
			//poly->appendVertex(ptNumber1);
			line_typeHandl.set(ptNumber1, umbilic_type);
			isUmbilicsHandl.set(ptNumber1, 1);
			poly->appendVertex(ptNumber1);
		}
		detail->bumpDataIdsForAddOrRemove(true, true, true);

		delete *iter_umb;
	}

}
