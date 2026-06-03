//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/debug.h"

using namespace air::util;

// TODO: replace later
int main() {
  TFILE trace("test_air.t");
  AIR_TRACE(trace, "Test AIR_TRACE to file .... \n");

  return 0;
}