#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

set (AIR_TESTAPP_INCLUDE_DIRS "")
list (APPEND AIR_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${AIR_TESTAPP_INCLUDE_DIRS})
set (AIR_TESTAPP_INCLUDE_DIRS "${AIR_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")