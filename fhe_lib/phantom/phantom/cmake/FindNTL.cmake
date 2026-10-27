# FindNTL.cmake

find_path(NTL_INCLUDE_DIR
    NAMES NTL/ZZ.h
    PATHS
    /usr/include
    /usr/local/include
)

find_library(NTL_LIBRARY
    NAMES ntl
    PATHS
    /usr/lib
    /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NTL DEFAULT_MSG NTL_LIBRARY NTL_INCLUDE_DIR)

if(NTL_FOUND)
    set(NTL_LIBRARIES ${NTL_LIBRARY})
    set(NTL_INCLUDE_DIRS ${NTL_INCLUDE_DIR})
endif()

mark_as_advanced(NTL_INCLUDE_DIR NTL_LIBRARY)