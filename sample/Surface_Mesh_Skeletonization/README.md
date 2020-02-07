# CGAL_Heat_method
## Use plugin in Houdini
* copy dso to C:\Users\Smile\Documents\houdini18.0
* SOP/CGAL_Heat_method is this node type name  

## Geting Start With Source
* Using CGAL on Windows (with Visual C++)：https://doc.cgal.org/latest/Manual/windows.html
* Add some necessary defines options in houdiniconfig.cmake.
set( _houdini_defines VERSION="18.0.287";CGAL_EIGEN3_ENABLED;BOOST_ALL_DYN_LINK=1;AMD64;SIZEOF_VOID_P=8;FBX_ENABLED=1;OPENCL_ENABLED=1;OPENVDB_ENABLED=1;SESI_LITTLE_ENDIAN;$<$<CONFIG:Release>:UT_ASSERT_LEVEL=0>;$<$<CONFIG:RelWithDebInfo>:UT_ASSERT_LEVEL=1>;$<$<CONFIG:Debug>:UT_ASSERT_LEVEL=2>;I386;WIN32;SWAP_BITFIELDS;_WIN32_WINNT=0x0600;NOMINMAX;STRICT;WIN32_LEAN_AND_MEAN;_USE_MATH_DEFINES;_CRT_SECURE_NO_DEPRECATE;_CRT_NONSTDC_NO_DEPRECATE;_SCL_SECURE_NO_WARNINGS;HBOOST_ALL_NO_LIB;EIGEN_MALLOC_ALREADY_ALIGNED=0;$<$<CONFIG:Debug>:_HAS_ITERATOR_DEBUGGING=0> )
* Add necessary environment variables： BOOST_INCLUDEDIR and CGAL_DIR

## Third Party Dependencies
* boost_1_72
* CGAL-5.0
* Eigen3.37

