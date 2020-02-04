# Set the variables that CMake expects our version test to set.
# For now, we'll ignore doing compatibility checks based on the version number.
set( PACKAGE_VERSION 18.0.287 )
set( PACKAGE_VERSION_EXACT FALSE )
set( PACKAGE_VERSION_COMPATIBLE TRUE )
set( PACKAGE_VERSION_UNSUITABLE FALSE )

set( version_major 18 )
set( version_minor 0 )
set( version_build 287 )

if ( "${PACKAGE_FIND_VERSION_MAJOR}" EQUAL "${version_major}" )
    if ( "${PACKAGE_FIND_VERSION_MINOR}" EQUAL "${version_minor}" )
        if ( "${PACKAGE_FIND_VERSION_PATCH}" EQUAL "${version_build}" )
            set( PACKAGE_VERSION_EXACT TRUE )
        endif ()
    endif ()
endif ()
