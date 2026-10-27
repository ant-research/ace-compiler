#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.28)

find_package(pybind11 CONFIG REQUIRED)

# TORCH_PYTHON_LIBRARY is found by cmake/AceDetect.cmake
if(NOT TORCH_PYTHON_LIBRARY)
    message(FATAL_ERROR "TORCH_PYTHON_LIBRARY not set — check cmake/AceDetect.cmake")
endif()

# spdlog is provided by ace_logging (fhe_lib/common/logging/) — no separate
# FetchContent needed here.  ace_common links ace_logging which provides
# spdlog::spdlog transitively.

add_subdirectory(common)
add_subdirectory(frontend)
add_subdirectory(runtime)
