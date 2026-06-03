#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.28)
project(ant-ace LANGUAGES C CXX)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(Torch REQUIRED)

# Auto-detect CXX11 ABI from PyTorch (must match for Python extension compatibility)
execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import torch; print(int(torch._C._GLIBCXX_USE_CXX11_ABI))"
    OUTPUT_VARIABLE USE_CXX11_ABI
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT USE_CXX11_ABI)
    set(USE_CXX11_ABI 0)
endif()
message(STATUS "USE_CXX11_ABI (from PyTorch)       : ${USE_CXX11_ABI}")

if(CMAKE_CUDA_COMPILER)
    message(STATUS "Building with CUDA support")
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_ARCHITECTURES native)
else()
    message(STATUS "Building CPU-only extension")
endif()

option(BUILD_EXTENSION "Build Python extension"   ON)
option(BUILD_TESTS     "Build tests"             OFF)

set(COMPILE_MODULE "fhe-cmplr" CACHE STRING "Compiler module (air-infra, nn-addon, fhe-cmplr, hpu-cg)")
set(ENABLE_LIB     "phantom"   CACHE STRING "FHE backend (phantom, acelib, seal, openfhe)")

include("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")

# Default build type
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

message(STATUS "")
message(STATUS "=========================================================")
message(STATUS "ANT-ACE Build Configuration")
message(STATUS "=========================================================")
message(STATUS "Version                             : ${PROJECT_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE                    : ${CMAKE_BUILD_TYPE}")
message(STATUS "COMPILE_MODULE                      : ${COMPILE_MODULE}")
message(STATUS "ENABLE_LIB                          : ${ENABLE_LIB}")
message(STATUS "BUILD_EXTENSION                     : ${BUILD_EXTENSION}")
message(STATUS "BUILD_TESTS                         : ${BUILD_TESTS}")
message(STATUS "ACE_DEP_SOURCE                      : ${ACE_DEP_SOURCE}")
message(STATUS "CMAKE_INSTALL_PREFIX                : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=========================================================")
message(STATUS "")

add_subdirectory(compiler)
add_subdirectory(fhe_dsl/csrc)

# Install Python source and data files from fhe_dsl/python
install(DIRECTORY fhe_dsl/python/
        DESTINATION ace
        COMPONENT frontend
        PATTERN "*.pyc" EXCLUDE
        PATTERN "__pycache__" EXCLUDE
        PATTERN "*.so" EXCLUDE
        PATTERN "*.pyd" EXCLUDE)

if(CMAKE_CUDA_COMPILER)
    add_subdirectory(fhe_lib)
else()
    message(WARNING "Compilation of Library was not selected")
endif()