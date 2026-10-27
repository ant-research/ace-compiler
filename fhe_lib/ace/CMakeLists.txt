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
    # In standalone builds default to common archs; when built as sub-project
    # CMAKE_CUDA_ARCHITECTURES is inherited from the parent.
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES OR CMAKE_CUDA_ARCHITECTURES STREQUAL "native")
        set(CMAKE_CUDA_ARCHITECTURES 80 86 89 90)
    endif()

    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/AceDependencies.cmake)

    message(STATUS "Building ACE-LIB Independence")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/fetch_source.cmake)

add_library(FHErt_ace SHARED
    src/ace_config.cu
    src/bts_mgr.cu
    src/api.cu
    src/vendor_traits.cu
    src/context.cu
    src/kms/key_serializer.cu
    src/kms/key_mgr.cu
    src/data/enc_mgr.cu
    src/data/enc_mgr_api.cu
    # src/data/enc_pipeline.cu  # DISABLED: offline msg→enc tool, pending
                                  # reimplementation without building an
                                  # encode-only FheContext (EncPolicy::BYPASS removed).
)

target_compile_definitions(FHErt_ace PRIVATE
    ACE_LIBRARY_ENABLE_BTS=1
    ACE_BTS_IMPL
    LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE
    __DATA_WORD_SIZE_64__
    __NTT_4STEP_MONT__
    __ACE_NTT_HAS_MONT__
    CCCL_DISABLE_PDL
    CUB_DISABLE_NAMESPACE_MAGIC
    CUB_IGNORE_NAMESPACE_MAGIC_ERROR
    THRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CUDA
    THRUST_DISABLE_ABI_NAMESPACE
    THRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_CPP
    THRUST_IGNORE_ABI_NAMESPACE_ERROR
)

set_property(TARGET FHErt_ace PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(FHErt_ace PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(FHErt_ace
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../../compiler/fhe-cmplr/rtlib/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../../compiler/fhe-cmplr/include
      ${ACE_LIBRARY_SOURCE_DIR}/he-engine/include/ace/he_engine
      ${CMAKE_CURRENT_BINARY_DIR}/ace_library_build/he-engine/include/ace/he_engine
      ${ACE_LIBRARY_SOURCE_DIR}/he-modules/src
)

# Link ace-library static libs (gpulib, ckks_bootstrapper_lib, rmm, etc.)
# and system dependencies. CUDA libs are propagated through acelib INTERFACE.
target_link_libraries(FHErt_ace PUBLIC FHErt_common_shared FHErt_io)
target_link_libraries(FHErt_ace PRIVATE acelib)

set_target_properties(FHErt_ace PROPERTIES
    BUILD_RPATH "$ORIGIN;${CMAKE_BINARY_DIR}/fhe_lib/common"
    INSTALL_RPATH "$ORIGIN"
)

# ---------------------------------------------------------------------------
# Install shared library
# ---------------------------------------------------------------------------
install(TARGETS FHErt_ace
        LIBRARY DESTINATION ace/lib
        COMPONENT library
)

# Install the ace-library he_engine headers so runtime JIT compilation of
# user .cu kernels can resolve the vendor shim's #include "ace/he_engine/Ciphertext.h"
# (etc.). The acelib backend's nvcc command adds -I <ace>/include/lib_ace/public,
# so the headers must land under .../lib_ace/public/ace/he_engine/. Mirrors how
# fhe_lib/phantom installs ${phantom_SOURCE_DIR}/include for the phantom shim.
install(DIRECTORY ${ACE_LIBRARY_SOURCE_DIR}/he-engine/include/ace/he_engine/
        DESTINATION ace/include/lib_ace/public/ace/he_engine
        COMPONENT library
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*.cuh"
)

# Generated/configured he_engine headers (e.g. host/util/config.h) produced in
# the binary tree during the acelib build — overlay them on the installed set.
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/ace_library_build/he-engine/include/ace/he_engine")
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ace_library_build/he-engine/include/ace/he_engine/
            DESTINATION ace/include/lib_ace/public/ace/he_engine
            COMPONENT library
            FILES_MATCHING PATTERN "*.h"
            PATTERN "*.cuh"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/CMakeLists.txt")
    add_subdirectory(example)
endif()
