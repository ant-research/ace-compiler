//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/opt.h"
#include "gtest/gtest.h"

using namespace air::opt;

namespace {

TEST(OPT, MESSAGE) {
  AIR_OPT opt;
  opt.Opt_test();

  EXPECT_EQ(0, 0);
}

}  // namespace