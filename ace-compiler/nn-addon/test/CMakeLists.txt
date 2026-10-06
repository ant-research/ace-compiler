#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB NN_TESTAPP_SRC_FILES CONFIGURE_DEPENDS *.cxx)

set (NN_TESTAPP_INCLUDE_DIRS "")
list (APPEND NN_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${NN_TESTAPP_INCLUDE_DIRS})
set (NN_TESTAPP_INCLUDE_DIRS "${NN_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

build_app_test (NN_INSTALL_APP "${NN_LIBS}")

add_test( NAME test_onnx_cmplr COMMAND ${PROJECT_SOURCE_DIR}/test/test_onnx_cmplr.sh "${CMAKE_BINARY_DIR}")
add_test( NAME test_mask_fusion COMMAND ${PROJECT_SOURCE_DIR}/test/test_mask_fusion.sh "${CMAKE_BINARY_DIR}")
add_test( NAME test_strided_slice_fusion COMMAND ${PROJECT_SOURCE_DIR}/test/test_strided_slice_fusion.sh "${CMAKE_BINARY_DIR}")
