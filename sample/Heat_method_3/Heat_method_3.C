
#include "Heat_method_3.h"

// This is an automatically generated header file based on theDsFile, below,
// to provide Heat_method_3Parms, an easy way to access parameter values from
// Heat_method_3Verb::cook with the correct type.
#include "Heat_method_3.proto.h"

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


#include <CGAL/draw_surface_mesh.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Heat_method_3/Surface_mesh_geodesic_distances_3.h>
#include <iostream>
#include <fstream>
typedef CGAL::Simple_cartesian<double>                       Kernel;
typedef Kernel::Point_3                                      Point_3;
typedef CGAL::Surface_mesh<Point_3>                          Triangle_mesh;
typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor vertex_descriptor;
typedef Triangle_mesh::Property_map<vertex_descriptor, double> Vertex_distance_map;
typedef Triangle_mesh::Face_index face_descriptor;

using namespace HDK_Heat_method_3;
//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder Heat_method_3::theSOPTypeName("CGAL_Heat_method"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        Heat_method_3::theSOPTypeName,   // Internal name
        "CGAL_Heat_method",                     // UI name
        Heat_method_3::myConstructor,    // How to build the SOP
        Heat_method_3::buildTemplates(), // My parameters
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
			name    "shourcept"      // Internal parameter name
			label   "Shourcept" // Descriptive parameter name for user interface
			type    integer
			default { "0" }     // Default for this parameter on new nodes
			range   { 0! 100000 }   // The value is prevented from going below 2 at all.
								// The UI slider goes up to 50, but the value can go higher.
			export  all         // This makes the parameter show up in the toolbox
								// above the viewport when it's in the node's state.
		}
}
)THEDSFILE";

PRM_Template*
Heat_method_3::buildTemplates()
{
    static PRM_TemplateBuilder templ("Heat_method_3.C"_sh, theDsFile);
    return templ.templates();
}

class Heat_method_3Verb : public SOP_NodeVerb
{
public:
	Heat_method_3Verb() {}
    virtual ~Heat_method_3Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new Heat_method_3Parms(); }
    virtual UT_StringHolder name() const { return Heat_method_3::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<Heat_method_3Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<Heat_method_3Verb> Heat_method_3Verb::theVerb;

const SOP_NodeVerb *
Heat_method_3::cookVerb() const 
{ 
    return Heat_method_3Verb::theVerb.get();
}

OP_ERROR HDK_Heat_method_3::Heat_method_3::cookMySop(OP_Context &context)
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
Heat_method_3Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<Heat_method_3Parms>();
    GU_Detail *detail = cookparms.gdh().gdpNC();

    // We need two points per division
   
	int32 sourcePtnum = sopparms.getShourcept();

	Triangle_mesh tm;
	GA_Offset ptoff;
	std::vector<Point_3> points;
	
	GA_RWHandleV3 Phandle(detail->findAttribute(GA_ATTRIB_POINT, "P"));

	detail->addFloatTuple(GA_ATTRIB_POINT, "distance", 1);
	GA_RWHandleF DistanceHandl(detail, GA_ATTRIB_POINT, "distance");


	std::vector<vertex_descriptor> indexmap;
	GEO_Primitive *prim;
	
	GA_FOR_ALL_PTOFF(detail, ptoff)
	{
		UT_Vector3 Pvalue = Phandle.get(ptoff);
		Point_3 ptTemp = Point_3(Pvalue.x(), Pvalue.y(), Pvalue.z());
		vertex_descriptor u = tm.add_vertex(ptTemp);
		indexmap.push_back(u);
	}
	std::vector<int32> ptoffsetmap;

	GA_FOR_ALL_PRIMITIVES(detail, prim)
	{
		GA_Range ptRange = prim->getPointRange(true);
		if (ptRange.getEntries() !=3 )
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
	for (size_t i =0;i< ptoffsetmap.size()/3;i++)
	{
		tm.add_face(indexmap[ptoffsetmap[i*3]], indexmap[ptoffsetmap[i*3+1]], indexmap[ptoffsetmap[i*3+2]]);
	}

	Vertex_distance_map vertex_distance = tm.add_property_map<vertex_descriptor, double>("v:distance", 0).first;
	vertex_descriptor source = (vertices(tm).first)[sourcePtnum];
	CGAL::Heat_method_3::estimate_geodesic_distances(tm, vertex_distance, source);
	
	int32 indexpt = -1;
	for (vertex_descriptor vd : vertices(tm))
	{
		indexpt += 1;
		;
		fpreal32 distance = get(vertex_distance, vd);
		if (indexpt < indexmap.size())
		{
			GA_Offset ptOff =detail->pointOffset(indexpt);
			DistanceHandl.set(ptOff, distance);
		}
		else
		{
			std::cout << vd << " (" << tm.point(vd) << ")"
				<< " is at distance " << get(vertex_distance, vd) << std::endl;
		}
	}

}
