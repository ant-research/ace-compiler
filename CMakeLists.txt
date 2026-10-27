#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.28)

# ---------------------------------------------------------------------------
# Project declaration — CUDA language is enabled conditionally by
# cmake/AceDetect.cmake, NOT in project() LANGUAGES.  This avoids CMake
# auto-detecting CUDA when it's not wanted and gives us full control over
# the enable_language(CUDA) order.
# ---------------------------------------------------------------------------
# Read version from fhe_dsl/python/_version.py (single source of truth)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/fhe_dsl/python/_version.py" _ver_content)
string(REGEX MATCH "__version__ *= *[\"']v?([0-9]+\\.[0-9]+\\.[0-9]+)[\"']" _ver_match "${_ver_content}")
if(NOT _ver_match)
    message(FATAL_ERROR "Cannot extract version from fhe_dsl/python/_version.py")
endif()
set(ACE_VERSION "${CMAKE_MATCH_1}")

project(ant-ace VERSION ${ACE_VERSION} LANGUAGES C CXX)

# Export compile_commands.json for clangd / clang-tidy (make lint-tidy).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# 1. Python interpreter (needed for pybind11 and ABI detection)
# ---------------------------------------------------------------------------
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# ---------------------------------------------------------------------------
# 2. CUDA detection, architecture selection, ABI, compile settings
#    See cmake/AceDetect.cmake for details.
#    IMPORTANT: This sets TORCH_CUDA_ARCH_LIST and CMAKE_CUDA_ARCHITECTURES
#    BEFORE find_package(Torch), which is required because PyTorch's cmake
#    config reads TORCH_CUDA_ARCH_LIST at find_package time to generate
#    -gencode flags and forcibly overrides CMAKE_CUDA_ARCHITECTURES=OFF.
# ---------------------------------------------------------------------------
include(cmake/AceDetect.cmake)

# ---------------------------------------------------------------------------
# 3. Find PyTorch (MUST come after AceDetect.cmake)
# ---------------------------------------------------------------------------
find_package(Torch REQUIRED)

# ---------------------------------------------------------------------------
# 4. Post-Torch cleanup
#    a) Retry torch_python lookup (TORCH_INSTALL_PREFIX now available)
#    b) Restore CMAKE_CUDA_ARCHITECTURES (PyTorch set it to OFF)
# ---------------------------------------------------------------------------
if(_ACE_TORCH_PYTHON_PENDING OR NOT TORCH_PYTHON_LIBRARY)
    find_library(TORCH_PYTHON_LIBRARY
        NAMES torch_python
        PATHS "${TORCH_INSTALL_PREFIX}/lib"
        NO_DEFAULT_PATH
    )
    if(TORCH_PYTHON_LIBRARY)
        message(STATUS "torch_python: ${TORCH_PYTHON_LIBRARY}")
    else()
        message(WARNING "torch_python library not found in ${TORCH_INSTALL_PREFIX}/lib — "
            "Python extension build may fail")
    endif()
    unset(_ACE_TORCH_PYTHON_PENDING)
endif()

# PyTorch's cmake/cuda.cmake forcibly sets CMAKE_CUDA_ARCHITECTURES=OFF
# (as a normal variable that shadows the CACHE entry).  Restore it from
# ACE_CUDA_ARCHITECTURES so that our non-PyTorch CUDA targets (phantom,
# ace_library, etc.) generate code for the correct architectures.
if(ACE_CUDA_AVAILABLE)
    set(CMAKE_CUDA_ARCHITECTURES ${ACE_CUDA_ARCHITECTURES} CACHE STRING
        "CUDA architectures (derived from ACE_CUDA_ARCHITECTURES)" FORCE)
endif()

# ---------------------------------------------------------------------------
# 5. Options
# ---------------------------------------------------------------------------
option(BUILD_UNITTEST       "Build C++ unit tests"          OFF)
option(BUILD_BENCH          "Build C++ benchmarks"          OFF)
option(BUILD_EXAMPLE        "Build examples"                OFF)

# GPU library backends (require CUDA).
# WITH_PHANTOM is ON by default; the others are opt-in.
# WITH_ACELIB (which fetches/builds fhe_lib/ace/ace-library) defaults OFF:
# enable explicitly with -DWITH_ACELIB=ON when the ACE GPU backend is needed.
option(WITH_PHANTOM        "Build Phantom GPU backend"      ON)
option(WITH_ACELIB         "Build ACE GPU backend (fetches ace-library)" OFF)

# CPU library backends (future — uncomment when fhe_lib/ directories are ready).
# option(WITH_SEAL         "Build Seal CPU backend"         OFF)
# option(WITH_OPENFHE      "Build OpenFHE CPU backend"      OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/AceDependencies.cmake")

# Build shared libraries for Python extension support (moved from config.cmake)
option(BUILD_SHARED "Build shared libraries for rtlib" ON)

# ---------------------------------------------------------------------------
# 6. Build type & install prefix
# ---------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Development build defaults (verbose output, fetch content options)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(FETCHCONTENT_QUIET OFF)
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()

# Install prefix: use scikit-build staging dir for pip, otherwise allow override
if(DEFINED SKBUILD_PLATLIB_DIR)
    set(CMAKE_INSTALL_PREFIX "${SKBUILD_PLATLIB_DIR}" CACHE PATH "Install prefix" FORCE)
endif()

set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_SKIP_RPATH FALSE)

# ---------------------------------------------------------------------------
# 7. Configuration summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "=========================================================")
message(STATUS "ANT-ACE Build Configuration")
message(STATUS "=========================================================")
message(STATUS "Version                             : ${PROJECT_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE                    : ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_UNITTEST                      : ${BUILD_UNITTEST}")
message(STATUS "BUILD_BENCH                         : ${BUILD_BENCH}")
message(STATUS "BUILD_EXAMPLE                       : ${BUILD_EXAMPLE}")
message(STATUS "WITH_PHANTOM                        : ${WITH_PHANTOM}")
message(STATUS "WITH_ACELIB                         : ${WITH_ACELIB}")
message(STATUS "ACE_DEP_SOURCE                      : ${ACE_DEP_SOURCE}")
message(STATUS "ACE_CUDA_AVAILABLE                  : ${ACE_CUDA_AVAILABLE}")
message(STATUS "ACE_CUDA_ARCHITECTURES              : ${ACE_CUDA_ARCHITECTURES}")
message(STATUS "CMAKE_CUDA_ARCHITECTURES            : ${CMAKE_CUDA_ARCHITECTURES}")
message(STATUS "TORCH_CUDA_ARCH_LIST                : ${TORCH_CUDA_ARCH_LIST}")
message(STATUS "USE_CXX11_ABI                       : ${USE_CXX11_ABI}")
message(STATUS "CMAKE_INSTALL_PREFIX                : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=========================================================")
message(STATUS "")

# ---------------------------------------------------------------------------
# 8. Sub-projects
# ---------------------------------------------------------------------------
# Build order matters: fhe_lib depends on compiler_shared (from compiler/),
# and fhe_dsl/csrc depends on ace_logging (from fhe_lib/).
#   compiler → fhe_lib → fhe_dsl/csrc
# ---------------------------------------------------------------------------

add_subdirectory(compiler)

if(ACE_CUDA_AVAILABLE)
    add_subdirectory(fhe_lib)
else()
    message(WARNING "Compilation of Library was not selected — no CUDA")
endif()

add_subdirectory(fhe_dsl/csrc)

# Install Python source and data files from fhe_dsl/python
install(DIRECTORY fhe_dsl/python/
        DESTINATION ace
        COMPONENT frontend
        PATTERN "*.pyc" EXCLUDE
        PATTERN "__pycache__" EXCLUDE
        PATTERN "*.so" EXCLUDE
        PATTERN "*.pyd" EXCLUDE)
