//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/align.h"

#include "gtest/gtest.h"

using namespace air::util;

namespace {

TEST(AlignTest, Basic) {
  EXPECT_EQ(Align(1, 1), 1);
  EXPECT_EQ(Align(3, 1), 3);
  EXPECT_EQ(Align(5, 1), 5);
  EXPECT_EQ(Align(7, 1), 7);
  EXPECT_EQ(Align(1, 2), 2);
  EXPECT_EQ(Align(2, 2), 2);
  EXPECT_EQ(Align(5, 2), 6);
  EXPECT_EQ(Align(6, 2), 6);
  EXPECT_EQ(Align(1, 4), 4);
  EXPECT_EQ(Align(1, 4), 4);
  EXPECT_EQ(Align(5, 4), 8);
  EXPECT_EQ(Align(8, 4), 8);
  EXPECT_EQ(Align(1, 8), 8);
  EXPECT_EQ(Align(8, 8), 8);
  EXPECT_EQ(Align(9, 8), 16);
  EXPECT_EQ(Align(16, 8), 16);
}  // AlignTest.Basic

TEST(AlignTest, Pointer) {
  int8_t buf[16];
  EXPECT_EQ(Align((int8_t*)(buf + 1), 1), (int8_t*)(buf + 1));
  EXPECT_EQ(Align((int16_t*)(buf + 3), 1), (int16_t*)(buf + 3));
  EXPECT_EQ(Align((int32_t*)(buf + 5), 1), (int32_t*)(buf + 5));
  EXPECT_EQ(Align((int64_t*)(buf + 7), 1), (int64_t*)(buf + 7));
  EXPECT_EQ(Align((int8_t*)(buf + 1), 2), (int8_t*)(buf + 2));
  EXPECT_EQ(Align((int16_t*)(buf + 2), 2), (int16_t*)(buf + 2));
  EXPECT_EQ(Align((int32_t*)(buf + 5), 2), (int32_t*)(buf + 6));
  EXPECT_EQ(Align((int64_t*)(buf + 6), 2), (int64_t*)(buf + 6));
  EXPECT_EQ(Align((int8_t*)(buf + 1), 4), (int8_t*)(buf + 4));
  EXPECT_EQ(Align((int16_t*)(buf + 1), 4), (int16_t*)(buf + 4));
  EXPECT_EQ(Align((int32_t*)(buf + 5), 4), (int32_t*)(buf + 8));
  EXPECT_EQ(Align((int64_t*)(buf + 8), 4), (int64_t*)(buf + 8));
  EXPECT_EQ(Align((int8_t*)(buf + 1), 8), (int8_t*)(buf + 8));
  EXPECT_EQ(Align((int16_t*)(buf + 8), 8), (int16_t*)(buf + 8));
  EXPECT_EQ(Align((int32_t*)(buf + 9), 8), (int32_t*)(buf + 16));
  EXPECT_EQ(Align((int64_t*)(buf + 16), 8), (int64_t*)(buf + 16));
}  // AlignTest.Pointer

TEST(AlignTest, Is_aligned) {
  int8_t i8_buf[16];
  EXPECT_EQ(Is_aligned(i8_buf + 0, 1), true);
  EXPECT_EQ(Is_aligned(i8_buf + 1, 1), true);
  EXPECT_EQ(Is_aligned(i8_buf + 2, 1), true);
  EXPECT_EQ(Is_aligned(i8_buf + 3, 1), true);
  EXPECT_EQ(Is_aligned(i8_buf + 4, 2), true);
  EXPECT_EQ(Is_aligned(i8_buf + 5, 2), false);
  EXPECT_EQ(Is_aligned(i8_buf + 6, 2), true);
  EXPECT_EQ(Is_aligned(i8_buf + 7, 2), false);
  EXPECT_EQ(Is_aligned(i8_buf + 8, 4), true);
  EXPECT_EQ(Is_aligned(i8_buf + 9, 4), false);
  EXPECT_EQ(Is_aligned(i8_buf + 10, 4), false);
  EXPECT_EQ(Is_aligned(i8_buf + 11, 4), false);
  EXPECT_EQ(Is_aligned(i8_buf + 12, 8), false);
  EXPECT_EQ(Is_aligned(i8_buf + 13, 8), false);
  EXPECT_EQ(Is_aligned(i8_buf + 14, 8), false);
  EXPECT_EQ(Is_aligned(i8_buf + 15, 8), false);
  EXPECT_EQ(Is_aligned(i8_buf + 16, 8), true);
  int16_t i16_buf[16];
  EXPECT_EQ(Is_aligned(i16_buf + 0, 4), true);
  EXPECT_EQ(Is_aligned(i16_buf + 1, 4), false);
  EXPECT_EQ(Is_aligned(i16_buf + 2, 4), true);
  EXPECT_EQ(Is_aligned(i16_buf + 3, 4), false);
  EXPECT_EQ(Is_aligned(i16_buf + 4, 8), true);
  EXPECT_EQ(Is_aligned(i16_buf + 5, 8), false);
  EXPECT_EQ(Is_aligned(i16_buf + 6, 8), false);
  EXPECT_EQ(Is_aligned(i16_buf + 7, 8), false);
  EXPECT_EQ(Is_aligned(i16_buf + 8, 8), true);
  int32_t i32_buf[16];
  EXPECT_EQ(Is_aligned(i32_buf + 0, 8), true);
  EXPECT_EQ(Is_aligned(i32_buf + 1, 8), false);
  EXPECT_EQ(Is_aligned(i32_buf + 2, 8), true);
  EXPECT_EQ(Is_aligned(i32_buf + 3, 8), false);
  EXPECT_EQ(Is_aligned(i32_buf + 4, 8), true);
}  // AlignTest.Is_aligned

}  // namespace
