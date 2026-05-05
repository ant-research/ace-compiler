#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

find_package(pybind11 CONFIG REQUIRED)

find_library(TORCH_PYTHON_LIBRARY
    NAMES torch_python
    PATHS ${TORCH_INSTALL_PREFIX}/lib
    NO_DEFAULT_PATH
)

if(NOT TORCH_PYTHON_LIBRARY)
    message(FATAL_ERROR "torch_python library not found in ${TORCH_INSTALL_PREFIX}/lib")
endif()

add_subdirectory(frontend)
add_subdirectory(runtime)
