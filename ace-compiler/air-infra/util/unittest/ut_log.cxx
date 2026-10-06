//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"

// #include "air/util/debug.h"

namespace {

TEST(util, MESSAGE) {
  //    AIR_ASSERT(( (-1 == 0)), "%s should be %d", "assert part", 999);
  //    EXPECT_EQ(0, AIR_ASSERT((-1 != 0)));
  EXPECT_EQ(0, 0);
}

}  // namespace