#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.28)

find_package(pybind11 CONFIG REQUIRED)

find_library(TORCH_PYTHON_LIBRARY
    NAMES torch_python
    PATHS ${TORCH_INSTALL_PREFIX}/lib
    NO_DEFAULT_PATH
)

if(NOT TORCH_PYTHON_LIBRARY)
    message(FATAL_ERROR "torch_python library not found in ${TORCH_INSTALL_PREFIX}/lib")
endif()

# Fetch spdlog with bundled fmt — shared by all extension modules
# Try to use internal URL first, fallback to external GitHub
if(DEFINED DEP_SPDLOG_URL)
    set(SPDLOG_URL ${DEP_SPDLOG_URL})
    set(SPDLOG_REF ${DEP_SPDLOG_REF})
else()
    set(SPDLOG_URL "https://github.com/gabime/spdlog.git")
    set(SPDLOG_REF "v1.15.1")
endif()

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY ${SPDLOG_URL}
    GIT_TAG        ${SPDLOG_REF}
)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

add_subdirectory(common)
add_subdirectory(frontend)
add_subdirectory(runtime)
