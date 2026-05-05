//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/debug.h"

using namespace air::util;

static void Test_air_debug(TFILE& tfile) {
  AIR_DEBUG("This is AIR_DEBUG testing .... \n");

  AIR_TRACE(tfile, "This is AIR_TRACE testing .... \n");

  AIR_ASSERT_MSG(1, "This is AIR_ASSERT 0 testing .... \n");
}

int main() {
  TFILE trace("air_trace.t");
  Test_air_debug(trace);

  return 0;
}
