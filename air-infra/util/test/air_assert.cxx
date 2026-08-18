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
 * @brief test macro AIR_ASSERT
 *
 */
static void Test_air_assert() {
  // AIR_ASSERT(0);

  AIR_ASSERT(1);
}

/**
 * @brief test macro AIR_ASSERT_MSG
 *
 */
static void Test_air_assert_msg() {
  // AIR_ASSERT_MSG(0, "API: AIR_ASSERT_MSG(%d, msg) testing ... \n", 0);

  AIR_ASSERT_MSG(1, "API: AIR_ASSERT_MSG(%d, msg) testing .... \n", 1);
}

int main() {
  Test_air_assert();

  Test_air_assert_msg();

  return 0;
}
