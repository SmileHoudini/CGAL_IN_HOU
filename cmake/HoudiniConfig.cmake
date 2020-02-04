# Package configuration file for the HDK.
#
# Defines the following imported library targets:
# - Houdini
#
# Defines the following variables:
# - Houdini_FOUND
# - Houdini_VERSION (from HoudiniConfigVersion.cmake)
# - Houdini_VERSION_MAJOR
# - Houdini_VERSION_MINOR
# - Houdini_VERSION_PATCH

include( CMakeParseArguments ) # For compatibility with CMake < 3.4.

set( _houdini_shared_libs custom/houdini/dsolib/libHOMUI.lib;custom/houdini/dsolib/libFUSE.lib;custom/houdini/dsolib/libMT.lib;custom/houdini/dsolib/libMDS.lib;custom/houdini/dsolib/libJEDI.lib;custom/houdini/dsolib/libOP3D.lib;custom/houdini/dsolib/libDM.lib;custom/houdini/dsolib/libVISF.lib;custom/houdini/dsolib/libGUI.lib;custom/houdini/dsolib/libGR.lib;custom/houdini/dsolib/libSHOP.lib;custom/houdini/dsolib/libVOP.lib;custom/houdini/dsolib/libVCC.lib;custom/houdini/dsolib/libPI.lib;custom/houdini/dsolib/libOP.lib;custom/houdini/dsolib/libLM.lib;custom/houdini/dsolib/libFS.lib;custom/houdini/dsolib/libUT.lib;custom/houdini/dsolib/libVM.lib;custom/houdini/dsolib/libSYS.lib;custom/houdini/dsolib/libtools.lib;custom/houdini/dsolib/hboost_system-mt.lib;custom/houdini/dsolib/tbb.lib;custom/houdini/dsolib/tbbmalloc.lib;custom/houdini/dsolib/libcurlwrap.lib;custom/houdini/dsolib/libPRM.lib;custom/houdini/dsolib/libCMD.lib;custom/houdini/dsolib/libCH.lib;custom/houdini/dsolib/libDEP.lib;custom/houdini/dsolib/libEXPR.lib;custom/houdini/dsolib/libHOM.lib;custom/houdini/dsolib/libPY.lib;custom/houdini/dsolib/libPXL.lib;custom/houdini/dsolib/libCL.lib;custom/houdini/dsolib/libARR.lib;custom/houdini/dsolib/libTAKE.lib;custom/houdini/dsolib/libIMG.lib;custom/houdini/dsolib/libTBF.lib;custom/houdini/dsolib/libDD.lib;custom/houdini/dsolib/libCVEX.lib;custom/houdini/dsolib/libPBR.lib;custom/houdini/dsolib/libVEX.lib;custom/houdini/dsolib/libTIL.lib;custom/houdini/dsolib/libhptex.lib;custom/houdini/dsolib/libIMG3D.lib;custom/houdini/dsolib/libGVEX.lib;custom/houdini/dsolib/libGT.lib;custom/houdini/dsolib/libGU.lib;custom/houdini/dsolib/libGSTY.lib;custom/houdini/dsolib/libSTY.lib;custom/houdini/dsolib/libGOP.lib;custom/houdini/dsolib/libGEO.lib;custom/houdini/dsolib/libGP.lib;custom/houdini/dsolib/libGD.lib;custom/houdini/dsolib/libGA.lib;custom/houdini/dsolib/libCE.lib;custom/houdini/dsolib/libTS.lib;custom/houdini/dsolib/libBV.lib;custom/houdini/dsolib/libKIN.lib;custom/houdini/dsolib/libGQ.lib;custom/houdini/dsolib/libUI.lib;custom/houdini/dsolib/libAU.lib;custom/houdini/dsolib/libRE.lib;custom/houdini/dsolib/libFONT.lib;custom/houdini/dsolib/libHARD.lib;custom/houdini/dsolib/libHAPIL.lib;custom/houdini/dsolib/libPYP.lib;custom/houdini/dsolib/libBM.lib;custom/houdini/dsolib/libFUI.lib;custom/houdini/dsolib/libSTORUI.lib;custom/houdini/dsolib/libIPR.lib;custom/houdini/dsolib/libOPUI.lib;custom/houdini/dsolib/libPSI2.lib;custom/houdini/dsolib/libSI.lib;custom/houdini/dsolib/libBR.lib;custom/houdini/dsolib/libSS.lib;custom/houdini/dsolib/libCHOP.lib;custom/houdini/dsolib/libOH.lib;custom/houdini/dsolib/libMOT.lib;custom/houdini/dsolib/libMGR.lib;custom/houdini/dsolib/libDOPZ.lib;custom/houdini/dsolib/libSIMZ.lib;custom/houdini/dsolib/libOBJ.lib;custom/houdini/dsolib/libSOP.lib;custom/houdini/dsolib/libSOPTG.lib;custom/houdini/dsolib/libDOP.lib;custom/houdini/dsolib/libWIRE.lib;custom/houdini/dsolib/libCLO.lib;custom/houdini/dsolib/libSIM.lib;custom/houdini/dsolib/libGAS.lib;custom/houdini/dsolib/libRBD.lib;custom/houdini/dsolib/libGDT.lib;custom/houdini/dsolib/libPDG.lib;custom/houdini/dsolib/libPDGT.lib;custom/houdini/dsolib/libSOPZ.lib;custom/houdini/dsolib/libFBX.lib;custom/houdini/dsolib/libDAE.lib;custom/houdini/dsolib/libCOP2.lib;custom/houdini/dsolib/libRU.lib;custom/houdini/dsolib/libTOP.lib;custom/houdini/dsolib/libROP.lib;custom/houdini/dsolib/libSOHO.lib;custom/houdini/dsolib/libLOP.lib;custom/houdini/dsolib/libHUSD.lib;custom/houdini/dsolib/libgusd.lib;custom/houdini/dsolib/libPDGD.lib;custom/houdini/dsolib/libCOPZ.lib;custom/houdini/dsolib/libCHOPZ.lib;custom/houdini/dsolib/libCHOPNET.lib;custom/houdini/dsolib/libCOPNET.lib;custom/houdini/dsolib/libVOPNET.lib;custom/houdini/dsolib/libTOPNET.lib;custom/houdini/dsolib/libGABC.lib;custom/houdini/dsolib/libIMGUI.lib;custom/houdini/dsolib/libGLTF.lib;custom/houdini/dsolib/libSHLFUI.lib;custom/houdini/dsolib/libSHLF.lib;custom/houdini/dsolib/libVIS.lib;custom/houdini/dsolib/libCV.lib;custom/houdini/dsolib/libSTOR.lib;custom/houdini/dsolib/libMH.lib;custom/houdini/dsolib/libMCS.lib;custom/houdini/dsolib/libMWS.lib;custom/houdini/dsolib/libMPI.lib;custom/houdini/dsolib/libMSS.lib;custom/houdini/dsolib/libMLS.lib;custom/houdini/dsolib/libIMS.lib;custom/houdini/dsolib/libIMP.lib;custom/houdini/dsolib/libIMH.lib;custom/houdini/dsolib/libIM.lib;custom/houdini/dsolib/libMATUI.lib;custom/houdini/dsolib/libJIVE.lib;custom/houdini/dsolib/libCHUI.lib;custom/houdini/dsolib/libTHOR.lib;custom/houdini/dsolib/libMIDI.lib;custom/houdini/dsolib/libDTUI.lib;custom/houdini/dsolib/libHOMF.lib;custom/houdini/dsolib/libRAY.lib;custom/houdini/dsolib/libVPRM.lib;custom/houdini/dsolib/libVGEO.lib;custom/houdini/dsolib/libBRAY.lib )
set( _houdini_shared_lib_targets HOMUI;FUSE;MT;MDS;JEDI;OP3D;DM;VISF;GUI;GR;SHOP;VOP;VCC;PI;OP;LM;FS;UT;VM;SYS;tools;hboost_system;tbb;tbbmalloc;curlwrap;PRM;CMD;CH;DEP;EXPR;HOM;PY;PXL;CL;ARR;TAKE;IMG;TBF;DD;CVEX;PBR;VEX;TIL;hptex;IMG3D;GVEX;GT;GU;GSTY;STY;GOP;GEO;GP;GD;GA;CE;TS;BV;KIN;GQ;UI;AU;RE;FONT;HARD;HAPIL;PYP;BM;FUI;STORUI;IPR;OPUI;PSI2;SI;BR;SS;CHOP;OH;MOT;MGR;DOPZ;SIMZ;OBJ;SOP;SOPTG;DOP;WIRE;CLO;SIM;GAS;RBD;GDT;PDG;PDGT;SOPZ;FBX;DAE;COP2;RU;TOP;ROP;SOHO;LOP;HUSD;gusd;PDGD;COPZ;CHOPZ;CHOPNET;COPNET;VOPNET;TOPNET;GABC;IMGUI;GLTF;SHLFUI;SHLF;VIS;CV;STOR;MH;MCS;MWS;MPI;MSS;MLS;IMS;IMP;IMH;IM;MATUI;JIVE;CHUI;THOR;MIDI;DTUI;HOMF;RAY;VPRM;VGEO;BRAY )
set( _houdini_static_libs  )
set( _houdini_static_lib_targets  )

set( _houdini_compile_options /MD;/bigobj;/GR;/EHsc;/nologo;/W1;/w14996;/wd4355;$<$<COMPILE_LANGUAGE:CXX>:/std:c++14>;/Zc:forScope;/Zc:rvalueCast;/Zc:strictStrings;/Zc:referenceBinding;/Zc:ternary;/Zc:throwingNew;/Zc:__cplusplus;/permissive- )
set( _houdini_defines VERSION="18.0.287";CGAL_EIGEN3_ENABLED;BOOST_ALL_DYN_LINK=1;AMD64;SIZEOF_VOID_P=8;FBX_ENABLED=1;OPENCL_ENABLED=1;OPENVDB_ENABLED=1;SESI_LITTLE_ENDIAN;$<$<CONFIG:Release>:UT_ASSERT_LEVEL=0>;$<$<CONFIG:RelWithDebInfo>:UT_ASSERT_LEVEL=1>;$<$<CONFIG:Debug>:UT_ASSERT_LEVEL=2>;I386;WIN32;SWAP_BITFIELDS;_WIN32_WINNT=0x0600;NOMINMAX;STRICT;WIN32_LEAN_AND_MEAN;_USE_MATH_DEFINES;_CRT_SECURE_NO_DEPRECATE;_CRT_NONSTDC_NO_DEPRECATE;_SCL_SECURE_NO_WARNINGS;HBOOST_ALL_NO_LIB;EIGEN_MALLOC_ALREADY_ALIGNED=0;$<$<CONFIG:Debug>:_HAS_ITERATOR_DEBUGGING=0> )

set( _houdini_release_version 18.0 )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    set( _houdini_platform_linux TRUE )
elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    set( _houdini_platform_win TRUE )
elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
    set( _houdini_platform_osx TRUE )
endif ()

if ( _houdini_platform_osx )
    get_filename_component( _houdini_toolkit_realpath ${CMAKE_CURRENT_LIST_DIR} REALPATH )

    # Locate the root of the Houdini installation.
    set( _houdini_install_root "${_houdini_toolkit_realpath}/../../../../../../.." )
    get_filename_component( _houdini_install_root ${_houdini_install_root} REALPATH )

    # Locate the root of $HFS.
    set( _houdini_hfs_root "${CMAKE_CURRENT_LIST_DIR}/../.." )
    get_filename_component( _houdini_hfs_root ${_houdini_hfs_root} REALPATH )
else ()
    set( _houdini_install_root "${CMAKE_CURRENT_LIST_DIR}/../.." )
    get_filename_component( _houdini_install_root ${_houdini_install_root} ABSOLUTE )
    set( _houdini_hfs_root ${_houdini_install_root} )
endif ()

set( _houdini_include_dir "${_houdini_hfs_root}/toolkit/include" )
set( _houdini_bin_dir "${_houdini_hfs_root}/bin" )

# Add interface target.
add_library( Houdini INTERFACE )
target_include_directories( Houdini SYSTEM INTERFACE
	${_houdini_include_dir}
	${_houdini_include_dir}/python2.7
)
target_compile_options( Houdini INTERFACE ${_houdini_compile_options} )
target_compile_definitions( Houdini INTERFACE ${_houdini_defines} -DMAKING_DSO )

function( _houdini_create_libraries )
    cmake_parse_arguments( H_LIB
        ""
        "TYPE"
        "PATHS;TARGET_NAMES"
        ${ARGN}
    )

    list( LENGTH H_LIB_PATHS num_libs )
    if ( NOT num_libs )
        return ()
    endif ()
    math( EXPR num_libs "${num_libs} - 1" )
    foreach ( idx RANGE ${num_libs} )
        list( GET H_LIB_PATHS ${idx} lib_path )
        list( GET H_LIB_TARGET_NAMES ${idx} base_target_name )
        set( target_name Houdini::${base_target_name} )

        add_library( ${target_name} ${H_LIB_TYPE} IMPORTED )

        if ( ${H_LIB_TYPE} STREQUAL "STATIC" OR NOT _houdini_platform_win )
            set( import_property IMPORTED_LOCATION )
        else ()
            # IMPORTED_IMPLIB is used on Windows.
            set( import_property IMPORTED_IMPLIB )
        endif ()

        set_target_properties(
            ${target_name}
            PROPERTIES
                ${import_property} ${_houdini_install_root}/${lib_path}
        )

        target_link_libraries( Houdini INTERFACE ${target_name} )
    endforeach ()

endfunction ()

_houdini_create_libraries(
    PATHS ${_houdini_shared_libs}
    TARGET_NAMES ${_houdini_shared_lib_targets}
    TYPE SHARED
)

_houdini_create_libraries(
    PATHS ${_houdini_static_libs}
    TARGET_NAMES ${_houdini_static_lib_targets}
    TYPE STATIC
)

# Returns the default installation directory for the platform. For example, on
# Linux this is $HOME/houdiniX.Y.
# This can be used for installing additional files such as help or HDAs.

# Usage: houdini_get_default_install_dir( <VAR> )
function ( houdini_get_default_install_dir output_var )

    set( hython_path "${_houdini_bin_dir}/hython${CMAKE_EXECUTABLE_SUFFIX}" )

    # Run hython to retrieve the correct value of $HOUDINI_USER_PREF_DIR.
    execute_process(
        COMMAND ${hython_path} -c "from __future__ import print_function\nprint(hou.homeHoudiniDirectory())"
        OUTPUT_VARIABLE install_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
	RESULT_VARIABLE status_code
	ERROR_VARIABLE status_str
    )

    if ( NOT status_code EQUAL 0 )
	message( FATAL_ERROR "Error running ${hython_path}: ${status_str}" )
    endif ()

    set( ${output_var} ${install_dir} PARENT_SCOPE )
endfunction ()

# - Sets the output directory for the target.
# - Sets the prefix for the library name.
#
# Usage: houdini_configure_target( target_name [INSTDIR dir] )
#
# Arguments:
# - INSTDIR: Output directory for the library / executable.
#            If not specified, the dso folder under the
#            houdini_get_default_install_dir() path (e.g. $HOME/houdiniX.Y/dso)
#            is used for libraries.
#            For executables, the default is ${CMAKE_CURRENT_BINARY_DIR}.
# - LIB_PREFIX: prefix for the library name. If not specified, the empty string
#               is used (e.g. "SOP_Star.so")
function ( houdini_configure_target target_name )
    cmake_parse_arguments( H_OUTPUT
        ""
        "INSTDIR;LIB_PREFIX"
        ""
        ${ARGN}
    )

    # Set the library prefix.
    set( prefix "" )
    if ( H_OUTPUT_LIB_PREFIX )
        set( prefix ${H_OUTPUT_LIB_PREFIX} )
    endif ()

    set_target_properties( ${target_name} PROPERTIES
        PREFIX "${prefix}"
    )

    # Configure the output directory for the library / executable.
    if ( NOT H_OUTPUT_INSTDIR )
        get_target_property( target_type ${target_name} TYPE )
        if ( ${target_type} STREQUAL "EXECUTABLE" )
            set( H_OUTPUT_INSTDIR ${CMAKE_CURRENT_BINARY_DIR} )
        else ()
            houdini_get_default_install_dir( H_OUTPUT_INSTDIR )
            string( APPEND H_OUTPUT_INSTDIR "/dso" )
        endif ()
    endif ()

    set( output_dir_properties
        LIBRARY_OUTPUT_DIRECTORY
        LIBRARY_OUTPUT_DIRECTORY_DEBUG
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO
        LIBRARY_OUTPUT_DIRECTORY_RELEASE
        RUNTIME_OUTPUT_DIRECTORY
        RUNTIME_OUTPUT_DIRECTORY_DEBUG
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
        RUNTIME_OUTPUT_DIRECTORY_RELEASE
        ARCHIVE_OUTPUT_DIRECTORY
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    )
    foreach ( output_dir_property ${output_dir_properties} )
        set_target_properties( ${target_name}
            PROPERTIES ${output_dir_property} "${H_OUTPUT_INSTDIR}" )
    endforeach ()

endfunction ()


set( _houdini_python_version 2.7 )
set( _houdini_python_full_version 2.7.15-8 )
set( _houdini_python_dotless_version 27 )

if ( _houdini_platform_win )
    set( _python_binary "${_houdini_install_root}/python${_houdini_python_dotless_version}/python${_houdini_python_version}" )
elseif ( _houdini_platform_linux )
    set( _python_binary "${_houdini_install_root}/python/bin/python" )
elseif ( _houdini_platform_osx )
    set( _python_binary "${_houdini_install_root}/Frameworks/Python.framework/Versions/${_houdini_python_version}/bin/python" )
else ()
    message( FATAL_ERROR "Not implemented." )
endif ()

set( H_GEN_PROTO_SCRIPT ${_houdini_hfs_root}/houdini/python${_houdini_python_version}libs/generate_proto.py )

# Usage: houdini_generate_proto_headers(
#           OUTPUT_VAR generated_headers
#           FILES src.C...
#       )
#
#   - OUTPUT_VAR: Name of a variable to contain the list of generated headers.
#   - API: API name for the generated class (e.g. SOP_API)
#   - FILES: A list of .C files.
function ( houdini_generate_proto_headers )
    cmake_parse_arguments( H_PROTO "" "OUTPUT_VAR;API" "FILES" ${ARGN} )

    set( api_arg )
    if ( H_PROTO_API )
        set( api_arg "--api=${H_PROTO_API}" )
    endif ()

    set( generated_headers )
    foreach ( proto_file ${H_PROTO_FILES} )
        get_filename_component( fullname ${proto_file} NAME )
        string( REGEX REPLACE "\\.[^.]*$" "" basename ${fullname} )
        set( dst_header ${CMAKE_CURRENT_BINARY_DIR}/${basename}.proto.h )
        list( APPEND generated_headers ${dst_header} )

        add_custom_command(
            OUTPUT ${dst_header}
            COMMAND ${_python_binary} ${H_GEN_PROTO_SCRIPT} ${api_arg} ${proto_file} ${dst_header}
            DEPENDS ${proto_file} ${H_GEN_PROTO_SCRIPT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        # Ensure the generated header is built before the corresponding .C file.
        set_source_files_properties(
            ${proto_file}
            PROPERTIES
            OBJECT_DEPENDS ${dst_header}
        )
    endforeach ()

    if ( H_PROTO_OUTPUT_VAR )
        set( ${H_PROTO_OUTPUT_VAR} ${generated_headers} PARENT_SCOPE )
    endif ()

endfunction ()
