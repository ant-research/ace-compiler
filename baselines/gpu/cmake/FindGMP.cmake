# FindGMP.cmake

find_path(GMP_INCLUDE_DIR
    NAMES gmp.h
    PATHS
    /usr/include
    /usr/local/include
)

find_library(GMP_LIBRARY
    NAMES gmp
    PATHS
    /usr/lib
    /usr/local/lib
)

find_library(GMPXX_LIBRARY
    NAMES gmpxx
    PATHS
    /usr/lib
    /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARY GMPXX_LIBRARY GMP_INCLUDE_DIR)

if(GMP_FOUND)
    set(GMP_LIBRARIES ${GMP_LIBRARY} ${GMPXX_LIBRARY})
    set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})
endif()

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY GMPXX_LIBRARY)