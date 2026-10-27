//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cg.h"
#include "gtest/gtest.h"

using namespace air::cg;

namespace {

TEST(CG, MESSAGE) {
  AIR_CG cg;
  cg.Cg_test();

  EXPECT_EQ(0, 0);
}

}  // namespace