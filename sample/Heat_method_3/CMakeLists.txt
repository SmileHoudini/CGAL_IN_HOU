cmake_minimum_required( VERSION 3.6 )

project( Heat_method )

# CMAKE_PREFIX_PATH must contain the path to the toolkit/cmake subdirectory of
# the Houdini installation. See the "Compiling with CMake" section of the HDK
# documentation for more details, which describes several options for
# specifying this path.
list( APPEND CMAKE_PREFIX_PATH "C:/Program Files/Side Effects Software/Houdini 18.0.287/toolkit/cmake" )

# Locate Houdini's libraries and header files.
# Registers an imported library target named 'Houdini'.
find_package( Houdini REQUIRED )

set( library_name Heat_method_3 )

# Code generation for the embedded DS file in Heat_method_3.C.
houdini_generate_proto_headers( FILES Heat_method_3.C )

# Add a library and its source files.
add_library( ${library_name} SHARED
    Heat_method_3.C
    Heat_method_3.h
)
#add ThirdPartyInclude
#EIGENDIR
list( APPEND ThirdPartIncludePath "$ENV{BOOST_INCLUDEDIR}")
list( APPEND ThirdPartIncludePath "$ENV{CGAL_DIR}/include")
list( APPEND ThirdPartIncludePath "$ENV{CGAL_DIR}/auxiliary/gmp/include")
list( APPEND ThirdPartIncludePath "C:/dev/Eigen3.37")
target_include_directories( ${library_name} PRIVATE
    ${ThirdPartIncludePath}
)
#add ThirdPartylib
list( APPEND ThirdPartLibPath "$ENV{CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.lib")
list( APPEND ThirdPartLibPath "$ENV{CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.lib")
target_link_libraries( ${library_name} ${ThirdPartLibPath})
# Link against the Houdini libraries, and add required include directories and
# compile definitions.
target_link_libraries( ${library_name} Houdini )

# Include ${CMAKE_CURRENT_BINARY_DIR} for the generated header.
target_include_directories( ${library_name} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Sets several common target properties, such as the library's output directory.
houdini_configure_target( ${library_name} )
