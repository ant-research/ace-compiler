# SEAL 3.6 API subset implemented by clib; not Microsoft SEAL itself.
get_filename_component(_clib_prefix "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
include(CMakeFindDependencyMacro)
find_dependency(OpenMP COMPONENTS CXX)
find_dependency(Threads)
set(SEAL_VERSION 3.6.6)
set(SEAL_FOUND TRUE)
set(SEAL_STATIC_FOUND TRUE)
set(SEAL_SHARED_FOUND FALSE)
set(SEAL_C_FOUND FALSE)
set(SEAL_USE_CXX17 TRUE)
set(SEAL_INCLUDE_DIRS "${_clib_prefix}/include")
set(SEAL_LIBRARIES SEAL::seal)
if(NOT TARGET SEAL::seal)
    add_library(SEAL::seal STATIC IMPORTED)
    set_target_properties(SEAL::seal PROPERTIES
        IMPORTED_LOCATION "${_clib_prefix}/lib/libclib_seal.a"
        INTERFACE_INCLUDE_DIRECTORIES "${_clib_prefix}/include"
        INTERFACE_COMPILE_FEATURES cxx_std_17
        INTERFACE_LINK_LIBRARIES "${_clib_prefix}/lib/libclib.a;OpenMP::OpenMP_CXX;Threads::Threads;m")
endif()
