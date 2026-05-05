#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.28)
    project(ace-lib-phantom LANGUAGES CXX CUDA)

    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_ARCHITECTURES native)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/fetch_source.cmake)

add_library(FHErt_phantom SHARED src/phantom_lib.cu)

target_include_directories(FHErt_phantom
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${phantom_SOURCE_DIR}/include
      $<TARGET_PROPERTY:compiler_shared,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(FHErt_phantom PUBLIC phantom FHErt_common_shared)

install(DIRECTORY include/
        DESTINATION ace/include/rt_phantom
        COMPONENT library
        FILES_MATCHING PATTERN "*.h"
)

# Install phantom headers from Phantom dependency (include all subdirectories)
install(DIRECTORY ${phantom_SOURCE_DIR}/include/
        DESTINATION ace/include/lib_phantom
        COMPONENT library
        PATTERN "*"
)

set_target_properties(FHErt_phantom PROPERTIES
    BUILD_RPATH "${CMAKE_BINARY_DIR}/rtlib/build/common"
    INSTALL_RPATH "$ORIGIN"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS FHErt_phantom
        LIBRARY DESTINATION ace/lib
        COMPONENT library
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/CMakeLists.txt")
    add_subdirectory(example)
endif()

