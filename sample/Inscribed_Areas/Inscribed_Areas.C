
#include "Inscribed_Areas.h"

// This is an automatically generated header file based on theDsFile, below,
// to provide Heat_method_3Parms, an easy way to access parameter values from
// Heat_method_3Verb::cook with the correct type.
#include "Inscribed_Areas.proto.h"

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
#include <CGAL/Iso_rectangle_2.h>
#include <CGAL/Largest_empty_iso_rectangle_2.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_convex_set_2.h>
#include <CGAL/extremal_polygon_2.h>
#include <iostream>
#include <vector>

#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kconvex;
typedef Kconvex::Point_2 Point_convex;


typedef double                                 Number_Type;
typedef CGAL::Simple_cartesian<Number_Type>    K;
typedef CGAL::Largest_empty_iso_rectangle_2<K> Largest_empty_iso_rect_2;
typedef K::Iso_rectangle_2                     Iso_rectangle_2;
typedef K::Point_2                             Point_2;

typedef double                                    FT;

typedef CGAL::Simple_cartesian<FT>                Kernel;

typedef Kernel::Point_2                           Point;
typedef std::vector<int>                          Index_cont;
typedef CGAL::Polygon_2<Kernel>                   Polygon_2;
typedef CGAL::Random_points_in_square_2<Point>    Generator;

using namespace HDK_Inscribed_Areas;
//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder Inscribed_Areas::theSOPTypeName("CGAL_Inscribed_Areas"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        Inscribed_Areas::theSOPTypeName,   // Internal name
        "CGAL_Inscribed_Areas",                     // UI name
        Inscribed_Areas::myConstructor,    // How to build the SOP
        Inscribed_Areas::buildTemplates(), // My parameters
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
				name    "k"      // Internal parameter name
				label   "k" // Descriptive parameter name for user interface
				type    integer
				default { "3" }     // Default for this parameter on new nodes
				range   { 3! 10 }   // The value is prevented from going below 2 at all.
									// The UI slider goes up to 50, but the value can go higher.
				export  all         // This makes the parameter show up in the toolbox
									// above the viewport when it's in the node's state.
			}
}
)THEDSFILE";

PRM_Template*
Inscribed_Areas::buildTemplates()
{
    static PRM_TemplateBuilder templ("Inscribed_Areas.C"_sh, theDsFile);
    return templ.templates();
}

class Inscribed_AreasVerb : public SOP_NodeVerb
{
public:
	Inscribed_AreasVerb() {}
    virtual ~Inscribed_AreasVerb() {}

    virtual SOP_NodeParms *allocParms() const { return new Inscribed_AreasParms(); }
    virtual UT_StringHolder name() const { return Inscribed_Areas::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<Inscribed_AreasVerb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<Inscribed_AreasVerb> Inscribed_AreasVerb::theVerb;

const SOP_NodeVerb *
Inscribed_Areas::cookVerb() const 
{ 
    return Inscribed_AreasVerb::theVerb.get();
}

OP_ERROR HDK_Inscribed_Areas::Inscribed_Areas::cookMySop(OP_Context &context)
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
Inscribed_AreasVerb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{

	/*CGAL::maximum_area_inscribed_k_gon_2
		CGAL::maximum_perimeter_inscribed_k_gon_2
		CGAL::extremal_polygon_2
		CGAL::Largest_empty_iso_rectangle_2<T>
		CGAL::Extremal_polygon_area_traits_2<K>
		CGAL::Extremal_polygon_perimeter_traits_2<K>
		ExtremalPolygonTraits_2
		LargestEmptyIsoRectangleTraits_2*/

    auto &&sopparms = cookparms.parms<Inscribed_AreasParms>();
    GU_Detail *detail = cookparms.gdh().gdpNC();
	GA_RWHandleV3 Phandle(detail->findAttribute(GA_ATTRIB_POINT, "P"));
	// We need two points per division
	int32 k = sopparms.getK();
	if (k < 3)
	{
		k = 3;
	}
	int32 type = (int32)(sopparms.getInscribedtype());

	if (type == 0)
	{
		GA_Offset ptoff;
		UT_BoundingBox bbox;
		detail->computeQuickBounds(bbox);
		Point_2 pt0(bbox.xmin(), bbox.zmin());
		Point_2 pt1(bbox.xmax(), bbox.zmax());
		Iso_rectangle_2 bounding_box(pt0, pt1);
		Largest_empty_iso_rect_2 leir(bounding_box);
		GA_FOR_ALL_PTOFF(detail, ptoff)
		{
			UT_Vector3 Pvalue = Phandle.get(ptoff);
			leir.push_back(Point_2(Pvalue.x(), Pvalue.z()));
		}
		Iso_rectangle_2 b = leir.get_largest_empty_iso_rectangle();
		detail->clearAndDestroy();
		
		GEO_PrimPoly *poly = (GEO_PrimPoly *)detail->appendPrimitive(GA_PRIMPOLY);
		for (int32 i=0;i< 4;i++ )
		{
			Point_2 pt22 = b.vertex(i);
			int ptNumber1 = detail->appendPointOffset();
			UT_Vector3 PvalueNew1(pt22.hx(), 0, pt22.hy());
			Phandle.set(ptNumber1, PvalueNew1);
			poly->appendVertex(ptNumber1);
		}
		poly->close();
	}
	else
	{
		Polygon_2 SourcePoly;
		GA_Offset ptoff;
		GA_FOR_ALL_PTOFF(detail, ptoff)
		{
			UT_Vector3 Pvalue = Phandle.get(ptoff);
			SourcePoly.push_back(Point_2(Pvalue.x(), Pvalue.z()));
		}
		detail->clearAndDestroy();
		Polygon_2 k_gon;
		CGAL::maximum_area_inscribed_k_gon_2(
			SourcePoly.vertices_begin(), SourcePoly.vertices_end(), k, std::back_inserter(k_gon));
		
		GEO_PrimPoly *poly = (GEO_PrimPoly *)detail->appendPrimitive(GA_PRIMPOLY);
		for (int32 i = k_gon.size()-1; i >=0 ; i--)
		{
			Point currentPt = k_gon[i];
			int ptNumber1 = detail->appendPointOffset();
			UT_Vector3 PvalueNew1(currentPt.hx(), 0, currentPt.hy());
			Phandle.set(ptNumber1, PvalueNew1);
			poly->appendVertex(ptNumber1);
		}
		poly->close();

		detail->bumpDataIdsForAddOrRemove(true, true, true);
	}
}
