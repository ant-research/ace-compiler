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
    # In standalone builds default to common archs; when built as sub-project
    # CMAKE_CUDA_ARCHITECTURES is inherited from the parent.
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES OR CMAKE_CUDA_ARCHITECTURES STREQUAL "native")
        set(CMAKE_CUDA_ARCHITECTURES 80 86 89 90)
    endif()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/fetch_source.cmake)

add_library(FHErt_phantom SHARED
    src/context.cu
    src/vendor_traits.cu
    src/api.cu
    src/kms/key_serializer.cu
    src/kms/key_mgr.cu
    # src/data/enc_pipeline.cu  # DISABLED: offline msg→enc tool, pending
                                  # reimplementation without building an
                                  # encode-only FheContext (EncPolicy::BYPASS removed).
    src/data/enc_mgr.cu
    src/data/enc_mgr_api.cu
)

target_include_directories(FHErt_phantom
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${phantom_SOURCE_DIR}/include
)

target_link_libraries(FHErt_phantom PUBLIC
    phantom
    FHErt_common_shared
    FHErt_io
    ace_compile_settings
    compiler_shared
)

# Install phantom headers from Phantom dependency (include all subdirectories)
install(DIRECTORY ${phantom_SOURCE_DIR}/include/
        DESTINATION ace/include/lib_phantom
        COMPONENT library
        PATTERN "*"
)

set_target_properties(FHErt_phantom PROPERTIES
    BUILD_RPATH "${CMAKE_BINARY_DIR}/rtlib/build/common;${CMAKE_BINARY_DIR}/fhe_lib/common"
    INSTALL_RPATH "$ORIGIN"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS FHErt_phantom
        LIBRARY DESTINATION ace/lib
        COMPONENT library
)

# DISABLED: the phantom example (fhe_lib/phantom/example/) is an old-style
# demo built on the legacy rtlib C contract + codegen-inc. Its add.inc emits
# `#include "rt_phantom/rt_phantom.h"`, which resolves to the legacy rtlib
# rt_phantom.h (guard-collision with fhe_lib's identical guard name means the
# fhe_lib 2-hop variant is skipped). That legacy header inlines to
# Phantom_* symbols, which were deleted when the dispatch layer moved to
# FheContext<VendorTraits>. Re-enabling this example requires either keeping
# the Phantom_* backend exports alive (dead-code regression) or migrating the
# example onto the kernel-runner dlsym path. Pending that, the example is
# excluded from the build. The fhe-cmplr <-> add.inc relationship is
# unaffected (the file remains on disk).
# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/CMakeLists.txt")
#     add_subdirectory(example)
# endif()
