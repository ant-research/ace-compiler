#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.28)
    project(ace-lib LANGUAGES CXX CUDA)

    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_ARCHITECTURES native)

    include(${CMAKE_CURRENT_SOURCE_DIR}/../../config.cmake)

    message(STATUS "Building ACE-LIB Independence")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/fetch_source.cmake)

add_library(FHErt_ace SHARED src/ace_library_lib.cu)

target_compile_definitions(FHErt_ace PRIVATE
    GPU_BACKEND
    ACE_LIBRARY_ENABLE_BTS=1
    LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE
    __DATA_WORD_SIZE_64__
    __NTT_4STEP_MONT__
)

set_property(TARGET FHErt_ace PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(FHErt_ace PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(FHErt_ace
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../../compiler/fhe-cmplr/rtlib/include
)

# Link ace-library static libs (gpulib, ckks_bootstrapper_lib, rmm, etc.)
# and system dependencies. CUDA libs are propagated through acelib INTERFACE.
target_link_libraries(FHErt_ace PUBLIC FHErt_common_shared)
target_link_libraries(FHErt_ace PRIVATE acelib)

set_target_properties(FHErt_ace PROPERTIES
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
)

# ---------------------------------------------------------------------------
# Install runtime headers (ace_library_api.h, rt_def.h, rt_ace_library/)
# ---------------------------------------------------------------------------
install(DIRECTORY include/
        DESTINATION ace/include/lib_ace
        COMPONENT library
        FILES_MATCHING PATTERN "*.h"
)

# ---------------------------------------------------------------------------
# Install shared library
# ---------------------------------------------------------------------------
install(TARGETS FHErt_ace
        LIBRARY DESTINATION ace/lib
        COMPONENT library
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/CMakeLists.txt")
    add_subdirectory(example)
endif()