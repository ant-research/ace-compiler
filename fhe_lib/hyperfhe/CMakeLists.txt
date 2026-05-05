#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.28)
    project(ace-lib-hyperfhe LANGUAGES CXX CUDA)

    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_ARCHITECTURES native)

    include(${CMAKE_CURRENT_SOURCE_DIR}/../../config.cmake)

    message(STATUS "Building ACE-LIB-HYPERFHE Independence")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/fetch_source.cmake)

add_library(FHErt_hyperfhe SHARED src/hyperfhe_lib.cu)

target_compile_definitions(FHErt_hyperfhe PRIVATE
    GPU_BACKEND
    HYPER_BTS_MACRO
    LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE
)

set_property(TARGET FHErt_hyperfhe PROPERTY POSITION_INDEPENDENT_CODE 1)
# set_target_properties(FHErt_hyperfhe PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(FHErt_hyperfhe
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${RMM_SOURCE_DIR}/cpp/include
      ${RMM_BINARY_DIR}/include
      ${ckks_infra_SOURCE_DIR}/ckks-app/src
      ${ckks_infra_SOURCE_DIR}/ckks-app/src/common
      ${ckks_infra_SOURCE_DIR}/ckks-gpu/include/public
      ${ckks_infra_BINARY_DIR}/include/public  
      $<TARGET_PROPERTY:compiler_shared,INTERFACE_INCLUDE_DIRECTORIES>
)

find_library(NTL_LIBRARY ntl)
find_library(GMP_LIBRARY gmp)
find_library(GMPXX_LIBRARY gmpxx)

target_link_libraries(FHErt_hyperfhe PRIVATE ckks_bootstrapper_lib gpulib rmm ${GMPXX_LIBRARY} ${NTL_LIBRARY} FHErt_common) # ${HYPERFHE_LIBS}) # hyperfhe)

set_target_properties(FHErt_hyperfhe PROPERTIES
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
)

install(DIRECTORY include/
        DESTINATION ace/include/rt_hyperfhe
        FILES_MATCHING PATTERN "*.h"
)

install(TARGETS FHErt_hyperfhe
        LIBRARY DESTINATION ace/lib
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/CMakeLists.txt")
    add_subdirectory(example)
endif()
