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
 * @brief test macro CMPLR_ASSERT
 *
 */
static void Test_cmplr_assert() {
  CMPLR_ASSERT(0, "This is CMPLR_ASSERT 0 testing .... \n");

  CMPLR_ASSERT(1, "This is CMPLR_ASSERT 1 testing .... \n");
}

int main() {
  Test_cmplr_assert();

  return 0;
}
