#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# fhe_lib/common - Shared I/O infrastructure for FHE GPU backends.
#
# Contains:
#   - GDS (cuFile) support (gds/)
#   - I/O transport abstraction layer (io/)
#
# Both are used by GPU backends (phantom, ace, etc.).
#
# cuFile is loaded at RUNTIME via dlopen — there is no compile-time
# dependency on libcufile.so. The FindcuFile.cmake module locates
# the cufile.h header only (needed for type definitions if present),
# but linking is against libdl, not libcufile.
#
# CUDA language and CUDAToolkit are already configured by the parent
# project (fhe_lib/CMakeLists.txt → root CMakeLists.txt).

# Logging infrastructure (spdlog facade — must come before io/kms that use it)
add_subdirectory(logging)

# Add our cmake/ directory for FindcuFile.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# cuFile headers are optional — only needed for type checking at compile
# time. Runtime loading is via dlopen, so this is soft dependency.
find_package(cuFile)

# nlohmann/json for GaloisKeyIndex JSON serialization (key_index.cpp).
find_package(nlohmann_json REQUIRED)

# ---------------------------------------------------------------------------
# GDS I/O library (static - linked into each backend that needs it)
# ---------------------------------------------------------------------------

add_library(FHErt_gds_io STATIC
    io/gds_io.cu
    io/cufile_api.cpp
)

target_include_directories(FHErt_gds_io
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
      $<INSTALL_INTERFACE:include>
)

# cuFile headers (optional — for type definitions only)
# cuFile::cufile is an INTERFACE library that only adds include dirs.
# If cufile.h is not found, compilation still works because
# cufile_api.h defines all needed types locally.
if(cuFile_FOUND)
    target_link_libraries(FHErt_gds_io PRIVATE cuFile::cufile)
    message(STATUS "GDS: cuFile headers found — cufile_api type-checking enabled")
else()
    message(STATUS "GDS: cuFile headers not found — using local type definitions (cufile_api.h)")
endif()

# dlopen/dlsym for runtime cuFile loading
target_link_libraries(FHErt_gds_io PRIVATE ${CMAKE_DL_LIBS})

# Logging (spdlog facade)
target_link_libraries(FHErt_gds_io PUBLIC ace_logging)

# Unified ABI / compile settings
target_link_libraries(FHErt_gds_io PRIVATE ace_compile_settings)

# ---------------------------------------------------------------------------
# I/O transport library (shared - provides IoTransport abstraction)
# ---------------------------------------------------------------------------
#
# Built as a SHARED library so that the global IoConfig singleton, transport
# factory, and all I/O code exist in exactly one .so at runtime.  When this
# was a STATIC library, its object code was copied into every downstream .so
# (FHErt_phantom, FHErt_ace, runtime), creating separate copies of global
# state — most critically the IoConfig singleton.  A shared library ensures
# a single source of truth and eliminates the dual-singleton problem.

add_library(FHErt_io SHARED
    io/transport.cpp
    io/file_transport.cpp
    io/staged_transport.cpp
    io/mmap_transport.cpp
    io/gds_transport.cpp
    io/io_bulk_reader.cpp
    io/crc32.cu
    io/data_file.cpp
    io/io_policy.cpp
    kms/key_io.cpp
    kms/key_index.cpp
)

target_include_directories(FHErt_io
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
      $<INSTALL_INTERFACE:include>
)

# FHErt_io needs CUDA runtime for cudaMemcpy in StagedTransport,
# MappedTransport, and Crc32.
# It also needs the rtlib type definitions (fhe/core/rt_data_def.h) for
# DataFileReader/Writer.
target_link_libraries(FHErt_io
    PUBLIC
      FHErt_gds_io
    PRIVATE
      CUDA::cudart
      compiler_shared
      nlohmann_json::nlohmann_json
)

# Unified ABI / compile settings
target_link_libraries(FHErt_io PRIVATE ace_compile_settings)

# Export all symbols by default (matching project convention for SHARED libs).
set_target_properties(FHErt_io PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
    BUILD_RPATH "${CMAKE_BINARY_DIR}/rtlib/build/common"
    INSTALL_RPATH "$ORIGIN"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------

# New layered include structure (fhe/io/, fhe/kms/, fhe/enc/, fhe/vendor/, fhe/util/)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/fhe/
    DESTINATION ace/include/fhe
    COMPONENT library
    FILES_MATCHING PATTERN "*.h"
)

# JIT kernel include shims — compiler-generated kernel.cu emits
# #include "rt_ace/rt_ace.h" and #include "rt_phantom/rt_phantom.h"
# These forwarding headers redirect to the canonical fhe/vendor/ headers.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/rt_ace/
    DESTINATION ace/include/rt_ace
    COMPONENT library
    FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/rt_phantom/
    DESTINATION ace/include/rt_phantom
    COMPONENT library
    FILES_MATCHING PATTERN "*.h"
)

# Install spdlog headers so JIT-compiled kernels can #include <spdlog/spdlog.h>.
# spdlog is fetched via FetchContent; its source tree is at ${spdlog_SOURCE_DIR}.
# The bundled fmt headers live under spdlog/fmt/bundled/ and are included
# automatically by spdlog when SPDLOG_COMPILED_LIB is not defined.
install(DIRECTORY ${spdlog_SOURCE_DIR}/include/spdlog/
    DESTINATION ace/include/spdlog
    COMPONENT library
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Only install the shared library — FHErt_gds_io is an internal implementation
# detail absorbed into libFHErt_io.so and is not needed at install time.
install(TARGETS FHErt_io
    LIBRARY DESTINATION ace/lib
    COMPONENT library
)
