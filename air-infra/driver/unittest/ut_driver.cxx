//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <limits.h>

#include "air/util/debug.h"
#include "gtest/gtest.h"

namespace {

TEST(UTIL, MESSAGE) {
  AIR_ASSERT_MSG(((0 == 0)), "%s should be %d", "assert part", 999);
  //    EXPECT_EQ(0, AIR_ASSERT((-1 != 0)));
  EXPECT_EQ(0, 0);
}

}  // namespace
