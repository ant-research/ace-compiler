//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"

#ifndef MPOOL_DEBUG
#define MPOOL_DEBUG
#endif

#include "air/util/strtab.h"

using namespace air::util;

namespace {

TEST(StrTabTest, Basic) {
  STRTAB*  strtab = new STRTAB;
  uint32_t s0     = strtab->Save_str("hello");
  uint32_t s1     = strtab->Save_str("hello");
  EXPECT_EQ(s0, s1);
  EXPECT_EQ(strcmp(strtab->Id_to_addr(s0), "hello"), 0);
  char buf[8];
  memcpy(buf, "hello", 6);
  uint32_t s3 = strtab->Save_str(buf);
  EXPECT_EQ(s0, s3);
  uint32_t s4 = strtab->Save_str(buf, 6);
  EXPECT_EQ(s0, s4);
  std::string str("hello");
  uint32_t    s5 = strtab->Save_str(str);
  EXPECT_EQ(s0, s5);

  uint32_t t0 = strtab->Save_str("world");
  EXPECT_GT(t0, s0);
  EXPECT_EQ(strcmp(strtab->Id_to_addr(t0), "world"), 0);
  memcpy(buf, "world\0\1\2", 8);
  uint32_t t1 = strtab->Save_str(buf);
  EXPECT_EQ(t0, t1);

  uint32_t x0 = strtab->Save_str(buf, 7);
  EXPECT_GT(x0, t0);
  uint32_t x1 = strtab->Save_str(buf, 8);
  EXPECT_GT(x1, x0);

  delete strtab;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // StrTabTest.Basic

TEST(StrTabTest, Extra) {
  STRTAB*  strtab = new STRTAB;
  uint32_t s0     = strtab->Save_str("hello0");
  uint32_t s1     = strtab->Save_stri("hello", 0);
  EXPECT_EQ(s0, s1);
  uint32_t s2 = strtab->Save_str("hello123456789");
  uint32_t s3 = strtab->Save_stri("hello", 123456789);
  EXPECT_EQ(s2, s3);

  uint32_t s4 = strtab->Save_str("helloworld");
  uint32_t s5 = strtab->Save_str2("hello", "world");
  EXPECT_EQ(s4, s5);
  uint32_t s6 = strtab->Save_str("hello, world");
  uint32_t s7 = strtab->Save_str2("hello", ", world");
  uint32_t s8 = strtab->Save_str2("hello,", " world");
  uint32_t s9 = strtab->Save_str2("hello, ", "world");
  EXPECT_EQ(s6, s7);
  EXPECT_EQ(s7, s8);
  EXPECT_EQ(s8, s9);

  uint32_t s10 = strtab->Save_str("helloworld0");
  uint32_t s11 = strtab->Save_str2i("hello", "world", 0);
  EXPECT_EQ(s10, s11);
  uint32_t s12 = strtab->Save_str("hello, world. 123456789");
  uint32_t s13 = strtab->Save_str2i("hello", ", world. ", 123456789);
  uint32_t s14 = strtab->Save_str2i("hello, ", "world. ", 123456789);
  uint32_t s15 = strtab->Save_str2i("hello, ", "world. 123", 456789);
  uint32_t s16 = strtab->Save_str2i("hello, ", "world. 123456", 789);
  EXPECT_EQ(s12, s13);
  EXPECT_EQ(s13, s14);
  EXPECT_EQ(s14, s15);
  EXPECT_EQ(s15, s16);

  delete strtab;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // StrTabTest.Extra

}  // namespace
