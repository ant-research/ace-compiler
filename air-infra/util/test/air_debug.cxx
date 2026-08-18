//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/debug.h"

using namespace air::util;

/**
 * @brief test macro AIR_DEBUG
 *
 */
static void Test_air_debug(void) {
  AIR_DEBUG("API: AIR_DEBUG testing .... \n");
}

int main() {
  Test_air_debug();

  return 0;
}
