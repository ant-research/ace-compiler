# SEAL-compatible package configuration backed by the clib SDK
# (/app/clib-sdk: include/clib.h + lib/libclib.a).
#
# FHE-MP-CNN's cnn_ckks does `find_package(SEAL 3.6 REQUIRED)` and links
# `SEAL::seal`. This config makes that resolve to the clib-backed SEAL shim
# (include/seal/seal.h -> seal_api.h) plus the clib static library, instead of
# Microsoft SEAL.
#
# The prebuilt libclib.a contains only the clib:: backend; the seal::
# compatibility layer (src/seal_api.cxx) is compiled here as a static library
# so consumers link both transparently through SEAL::seal.

set(SEAL_VERSION_MAJOR 3)
set(SEAL_VERSION_MINOR 6)
set(SEAL_VERSION_PATCH 6)
set(SEAL_VERSION 3.6.6)

set(SEAL_FOUND TRUE)
set(SEAL_STATIC_FOUND TRUE)
set(SEAL_SHARED_FOUND FALSE)
set(SEAL_C_FOUND FALSE)
set(SEAL_USE_CXX17 TRUE)

# <prefix>/lib/cmake/SEAL-3.6/ -> <prefix>
get_filename_component(SEAL_PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

set(SEAL_INCLUDE_DIRS "${SEAL_PACKAGE_PREFIX_DIR}/include")
set(SEAL_LIBRARIES "${SEAL_PACKAGE_PREFIX_DIR}/lib/libclib.a")

if(NOT TARGET SEAL::seal)
    # Compile the SEAL-compatible shim (seal:: API over clib) as part of the
    # consuming build; only libclib.a is prebuilt.
    add_library(seal_clib_shim STATIC "${SEAL_PACKAGE_PREFIX_DIR}/src/seal_api.cxx")
    target_include_directories(seal_clib_shim PUBLIC "${SEAL_PACKAGE_PREFIX_DIR}/include")
    target_compile_features(seal_clib_shim PUBLIC cxx_std_17)
    set_target_properties(seal_clib_shim PROPERTIES POSITION_INDEPENDENT_CODE ON)
    # libclib.a uses OpenMP kernels; propagate the flags so consumers link
    # cleanly even without their own OpenMP setup.
    find_package(OpenMP QUIET)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(seal_clib_shim PUBLIC OpenMP::OpenMP_CXX)
    endif()
    target_link_libraries(seal_clib_shim PUBLIC "${SEAL_PACKAGE_PREFIX_DIR}/lib/libclib.a" pthread m)
    add_library(SEAL::seal ALIAS seal_clib_shim)
endif()

if(NOT TARGET SEAL::seal_shared)
    # cnn_ckks only uses SEAL::seal, but expose the shared alias too so the
    # package is a drop-in for either spelling.
    add_library(SEAL::seal_shared ALIAS seal_clib_shim)
endif()
