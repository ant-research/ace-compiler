#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Checks for independent builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.28)
    project(ace-compiler LANGUAGES C CXX)

    set(COMPILE_MODULE "fhe-cmplr" CACHE STRING "")

    # Default ABI for standalone compiler build
    if(NOT DEFINED USE_CXX11_ABI)
        set(USE_CXX11_ABI 0)
    endif()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/fetch_source.cmake)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ABI compatibility: must match across all modules and PyTorch
add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=${USE_CXX11_ABI})

add_library(compiler_obj OBJECT "")
target_sources(compiler_obj PRIVATE
    $<TARGET_OBJECTS:airutil_obj>
    $<TARGET_OBJECTS:airbase_obj>
    $<TARGET_OBJECTS:aircore_obj>
    $<TARGET_OBJECTS:airopt_obj>
    $<TARGET_OBJECTS:airdriver_obj>

    $<TARGET_OBJECTS:nnutil_obj>
    $<TARGET_OBJECTS:nncore_obj>
    $<TARGET_OBJECTS:nnopt_obj>
    $<TARGET_OBJECTS:nnvector_obj>
    $<TARGET_OBJECTS:nndriver_obj>

    $<TARGET_OBJECTS:fheutil_obj>
    $<TARGET_OBJECTS:fhecore_obj>
    $<TARGET_OBJECTS:fheopt_obj>
    $<TARGET_OBJECTS:fhesihe_obj>
    $<TARGET_OBJECTS:fheckks_obj>
    $<TARGET_OBJECTS:fhepoly_obj>
    $<TARGET_OBJECTS:fhedriver_obj>
)

add_library(compiler_shared SHARED wrapper.cxx $<TARGET_OBJECTS:compiler_obj>)

target_include_directories(compiler_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/air-infra/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/nn-addon/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fhe-cmplr/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fhe-cmplr/rtlib/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fhe-cmplr/rtlib/ant/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/rtlib/build/_deps/uthash-src/src>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:rtlib/include>
    $<INSTALL_INTERFACE:rtlib/include/ant>
)

# Use default visibility for ABI compatibility with Python extension
set_target_properties(compiler_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

target_compile_options(compiler_shared PRIVATE "-fvisibility=default")
target_link_options(compiler_shared PRIVATE
    "-Wl,--export-dynamic"
    "-Wl,--no-as-needed"
)

# Install shared libraries to ace/lib (COMPONENT core)
install(FILES ${CMAKE_BINARY_DIR}/rtlib/build/common/libFHErt_common.so DESTINATION ace/lib COMPONENT core)
install(FILES ${CMAKE_BINARY_DIR}/rtlib/build/rtlib/lib/libFHErt_ant.so DESTINATION ace/lib COMPONENT core)
install(TARGETS compiler_shared LIBRARY DESTINATION ace/lib COMPONENT core)

# Fix RPATH for installed shared libraries (convert from build path to $ORIGIN)
install(CODE "
  file(RPATH_CHANGE
    FILE \"\${CMAKE_INSTALL_PREFIX}/ace/lib/libFHErt_ant.so\"
    OLD_RPATH \"${CMAKE_BINARY_DIR}/rtlib/build/common:\"
    NEW_RPATH \"\$ORIGIN\")
" COMPONENT core)

# Install headers to ace/include (COMPONENT core)
install(DIRECTORY ${fhe-cmplr_SOURCE_DIR}/rtlib/include/ DESTINATION ace/include COMPONENT core)
install(DIRECTORY ${fhe-cmplr_SOURCE_DIR}/rtlib/ant/include/ DESTINATION ace/include COMPONENT core)
install(FILES ${CMAKE_BINARY_DIR}/rtlib/build/_deps/uthash-src/src/uthash.h DESTINATION ace/include COMPONENT core)

# Install fhe_cmplr driver to ace/bin (COMPONENT core)
install(FILES ${CMAKE_BINARY_DIR}/_deps/fhe-cmplr-build/driver/fhe_cmplr
        DESTINATION ace/bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        COMPONENT core)