#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

set (FHE_TESTAPP_INCLUDE_DIRS "")
list (APPEND FHE_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${FHE_TESTAPP_INCLUDE_DIRS})
set (FHE_TESTAPP_INCLUDE_DIRS "${FHE_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

build_app_test (FHE_INSTALL_APP "${FHE_LIBS}")

add_test( NAME test_fhe_cmplr COMMAND ${PROJECT_SOURCE_DIR}/test/test_fhe_cmplr.sh "${fhe-cmplr_BINARY_DIR}")
add_test( NAME test_bts_rescale COMMAND ${PROJECT_SOURCE_DIR}/test/test_bts_rescale.sh "${fhe-cmplr_BINARY_DIR}")
