//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <thread>

#include "air/util/timing.h"
#include "gtest/gtest.h"

namespace {

void Delay_func(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

TEST(util, TIME_START) {
  double start = TIME_START();

  EXPECT_GT(start, 0);
}

TEST(util, TIME_TAKEN) {
  TIME_START();

  Delay_func(100);

  double taken = TIME_TAKEN("TIME_TAKEN");

  EXPECT_GT(taken, 0);
}

TEST(util, TIME_TAKEN_NULL) {
  TIME_START();

  Delay_func(100);

  double taken = TIME_TAKEN(NULL);

  EXPECT_GT(taken, 0);
}

TEST(util, TIME_TAKEN_MULTI) {
  TIME_START();

  Delay_func(100);

  double taken1 = TIME_TAKEN(NULL);

  Delay_func(100);

  double taken2 = TIME_TAKEN(NULL);

  EXPECT_GT(taken2, taken1);
}

TEST(util, TIME_START_EX) {
  air::util::TIMING tim;
  double            start = TIME_START_EX(tim);

  EXPECT_GT(start, 0);
}

TEST(util, TIME_TAKEN_EX) {
  air::util::TIMING tim;
  TIME_START_EX(tim);

  Delay_func(100);

  double taken = TIME_TAKEN_EX(tim, "TIME_TAKEN");

  EXPECT_GT(taken, 0);
}

TEST(util, TIME_TAKEN_EX_MULTI) {
  air::util::TIMING tim;
  TIME_START_EX(tim);

  Delay_func(100);

  double taken1 = TIME_TAKEN_EX(tim, NULL);

  Delay_func(100);

  double taken2 = TIME_TAKEN_EX(tim, NULL);

  EXPECT_GT(taken2, taken1);
}
}  // namespace