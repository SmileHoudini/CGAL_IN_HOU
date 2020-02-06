
#include "Triangulated_Mesh_Approximation.h"

// This is an automatically generated header file based on theDsFile, below,
// to provide Heat_method_3Parms, an easy way to access parameter values from
// Heat_method_3Verb::cook with the correct type.
#include "Triangulated_Mesh_Approximation.proto.h"

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
#include <iostream>
#include <fstream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_approximation/approximate_triangle_mesh.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
typedef CGAL::Simple_cartesian<double>   Kernel;
typedef Kernel::Point_3										Point;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef boost::graph_traits<Mesh>::vertex_descriptor vertex_descriptor;

namespace VSA = CGAL::Surface_mesh_approximation;
namespace PMP = CGAL::Polygon_mesh_processing;



using namespace HDK_Triangulated_Mesh_Approximation;
//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder Triangulated_Mesh_Approximation::theSOPTypeName("CGAL_Triangulated_Mesh_Approximation"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        Triangulated_Mesh_Approximation::theSOPTypeName,   // Internal name
        "CGAL_Triangulated_Mesh_Approximation",                     // UI name
        Triangulated_Mesh_Approximation::myConstructor,    // How to build the SOP
        Triangulated_Mesh_Approximation::buildTemplates(), // My parameters
        0,                          // Min # of sources
        1,                          // Max # of sources
        nullptr,                    // Custom local variables (none)
        OP_FLAG_GENERATOR));        // Flag it as generator
}

/// This is a multi-line raw string specifying the parameter interface
/// for this SOP.
static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
	parm {
			name    "inscribedType"
			label   "InscribedType"
			type    ordinal
			default { "0" }     // Default to first entry in menu, "xy"
			menu    {
				"inscribed_4_gon"    "inscribed_4_gon"
				"inscribed_k_gon"    "inscribed_k_gon"
			}
		}	
	parm {
				name    "iterations"      // Internal parameter name
				label   "IterationsNum" // Descriptive parameter name for user interface
				type    integer
				default { "30" }     // Default for this parameter on new nodes
				range   { 1 50 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
		}
	parm {
				name    "clusterNum"      // Internal parameter name
				label   "ClusterNum" // Descriptive parameter name for user interface
				type    integer
				default { "200" }     // Default for this parameter on new nodes
				range   { 50 300 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
		}
}
)THEDSFILE";

PRM_Template*
Triangulated_Mesh_Approximation::buildTemplates()
{
    static PRM_TemplateBuilder templ("Triangulated_Mesh_Approximation.C"_sh, theDsFile);
    return templ.templates();
}

class Triangulated_Mesh_ApproximationVerb : public SOP_NodeVerb
{
public:
	Triangulated_Mesh_ApproximationVerb() {}
    virtual ~Triangulated_Mesh_ApproximationVerb() {}

    virtual SOP_NodeParms *allocParms() const { return new Triangulated_Mesh_ApproximationParms(); }
    virtual UT_StringHolder name() const { return Triangulated_Mesh_Approximation::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<Triangulated_Mesh_ApproximationVerb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<Triangulated_Mesh_ApproximationVerb> Triangulated_Mesh_ApproximationVerb::theVerb;

const SOP_NodeVerb *
Triangulated_Mesh_Approximation::cookVerb() const 
{ 
    return Triangulated_Mesh_ApproximationVerb::theVerb.get();
}

OP_ERROR HDK_Triangulated_Mesh_Approximation::Triangulated_Mesh_Approximation::cookMySop(OP_Context &context)
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
Triangulated_Mesh_ApproximationVerb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<Triangulated_Mesh_ApproximationParms>();
    GU_Detail *detail = cookparms.gdh().gdpNC();
	GA_RWHandleV3 Phandle(detail->findAttribute(GA_ATTRIB_POINT, "P"));
	// We need two points per division
	int32 Iterations = sopparms.getIterations();
	if (Iterations < 1)
	{
		Iterations = 1;
	}
	int32 ClusterNum = sopparms.getClusternum();
	if (ClusterNum < 1)
	{
		ClusterNum = 1;
	}

	Mesh mesh;

	GA_Offset ptoff;
	std::vector<Point> points;
	std::vector<vertex_descriptor> indexmap;
	GEO_Primitive *prim;

	GA_FOR_ALL_PTOFF(detail, ptoff)
	{
		UT_Vector3 Pvalue = Phandle.get(ptoff);
		Point ptTemp = Point(Pvalue.x(), Pvalue.y(), Pvalue.z());
		vertex_descriptor u = mesh.add_vertex(ptTemp);
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
		Mesh::Face_index error=mesh.add_face(indexmap[ptoffsetmap[i * 3]], indexmap[ptoffsetmap[i * 3 + 1]], indexmap[ptoffsetmap[i * 3 + 2]]);
		//if (error == Mesh::null_face())
		//{
		//	mesh.add_face(indexmap[ptoffsetmap[i * 3]], indexmap[ptoffsetmap[i * 3 + 2]], indexmap[ptoffsetmap[i * 3 + 1]]);
		//	//std::cout<<"erroe"<<std::endl;
		//}
	}
	/*std::ofstream out("dumptest.off");
	out << mesh;
	out.close();*/

	detail->clearAndDestroy();
	// output indexed triangles
	std::vector<Point> anchors;
	std::vector<std::array<std::size_t, 3> > triangles; // triplets of indices

	 // free function interface with named parameters
	bool is_manifold = VSA::approximate_triangle_mesh(mesh,
		CGAL::parameters::seeding_method(VSA::HIERARCHICAL). // hierarchical seeding
		max_number_of_proxies(ClusterNum). // seeding with maximum number of proxies
		number_of_iterations(Iterations). // number of clustering iterations after seeding
		anchors(std::back_inserter(anchors)). // anchor vertices
		triangles(std::back_inserter(triangles))); // indexed triangles
	std::cout << "#anchor vertices: " << anchors.size() << std::endl;
	std::cout << "#triangles: " << triangles.size() << std::endl;

	if (1)
	{
		std::vector<GA_Offset> currentMeshOffsetArray;
		for (int32 i=0;i<anchors.size();i++)
		{
			GA_Offset ptNumber = detail->appendPointOffset();
			UT_Vector3 PvalueNew(anchors[i].hx(), anchors[i].hy(), anchors[i].hz());
			Phandle.set(ptNumber, PvalueNew);
			currentMeshOffsetArray.push_back(ptNumber);
		}
		detail->bumpDataIdsForAddOrRemove(true, true, true);

		for (int32 i = 0; i < triangles.size(); i++)
		{
			GEO_PrimPoly *poly = (GEO_PrimPoly *)detail->appendPrimitive(GA_PRIMPOLY);
			poly->appendVertex(currentMeshOffsetArray[triangles[i][0]]);
			poly->appendVertex(currentMeshOffsetArray[triangles[i][1]]);
			poly->appendVertex(currentMeshOffsetArray[triangles[i][2]]);
			poly->close();
		}
		detail->bumpDataIdsForAddOrRemove(true, true, true);

	}

	//if (is_manifold) {
	//	std::cout << "oriented, 2-manifold output." << std::endl;
	//	// convert from soup to surface mesh
	//	PMP::orient_polygon_soup(anchors, triangles);
	//	Mesh output;
	//	PMP::polygon_soup_to_polygon_mesh(anchors, triangles, output);
	//	if (CGAL::is_closed(output) && (!PMP::is_outward_oriented(output)))
	//		PMP::reverse_face_orientations(output);
	//	std::ofstream out("dump.off");
	//	out << output;
	//	out.close();

	
}
