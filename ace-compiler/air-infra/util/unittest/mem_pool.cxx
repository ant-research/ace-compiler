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

#include "air/util/mem_pool.h"

using namespace air::util;

namespace {

typedef MEM_POOL<512>  SMALL_MEM_POOL;
typedef MEM_POOL<4096> MEDIUM_MEM_POOL;

template <typename T>
void Test_mem_pool(uint32_t large_size) {
  T*    mp = new T;
  char* s0 = mp->Allocate(0);
  char* s1 = mp->Allocate(1);
  EXPECT_TRUE(Is_aligned(s1, 1));
  *s1      = 'a';
  char* s2 = mp->Allocate(2);
  EXPECT_TRUE(Is_aligned(s2, 2));
  *(uint16_t*)s2 = 0x1234;
  char* s3       = mp->Allocate(8);
  EXPECT_TRUE(Is_aligned(s3, 8));
  *(uint64_t*)s3 = 0x1234567890abcdef;
  EXPECT_EQ(s2 - s1, s1 - s0 + sizeof(uintptr_t));
  EXPECT_EQ(s3 - s2, s2 - s1);
  mp->Deallocate(s0, 0);
  mp->Deallocate(s1, 0);
  mp->Deallocate(s2, 0);
  mp->Deallocate(s3, 0);
  char* s4 = mp->Allocate(large_size);
  ASSERT_NE(s4, nullptr);
  memset(s4, 0, large_size);
  mp->Deallocate(s4, large_size);
  char* s5 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s5, 8));
  memset(s5, 0, 32);
  char* s6 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s6, 8));
  memset(s6, 0, 32);
  char* s7 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s7, 8));
  memset(s7, 0, 32);
  char* s8 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s8, 8));
  memset(s8, 0, 32);
  EXPECT_GT(s5, s3);
  EXPECT_EQ(s7 - s6, s6 - s5);
  EXPECT_EQ(s8 - s7, s7 - s6);
  mp->Deallocate(s5, 0);
  mp->Deallocate(s6, 0);
  mp->Deallocate(s7, 0);
  mp->Deallocate(s8, 0);
  delete mp;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_mem_pool

TEST(MemPoolTest, Basic) {
  Test_mem_pool<SMALL_MEM_POOL>(512 - 16 + 1);
  Test_mem_pool<MEDIUM_MEM_POOL>(4096 - 16 + 1);
}  // MemPoolTest.Basic

typedef STACKED_MEM_POOL<512>  SMALL_STACKED_MEM_POOL;
typedef STACKED_MEM_POOL<4096> MEDIUM_STACKED_MEM_POOL;

template <typename T>
void Test_stacked_mem_pool(uint32_t large_size) {
  constexpr int N = 512;
  char*         blk0[N];
  char*         blk1[N];
  T*            mp0 = new T;
  for (int i = 0; i < N; ++i) {
    blk0[i] = mp0->Allocate(i * 2);
    memset(blk0[i], 1, i * 2);
    mp0->Push();
    blk1[i] = mp0->Allocate(i * 2);
    memset(blk1[i], 2, i * 2);
  }
  for (int i = N - 1; i >= 0; --i) {
    mp0->Deallocate(blk1[i], i * 2);
    mp0->Pop();
    mp0->Deallocate(blk0[i], i * 2);
  }
  // still remain something in mem pool
  EXPECT_FALSE(MEM_POOL_MANAGER::All_freed());
  delete mp0;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());

  // a more typical use case
  T* mp1 = new T;
  for (int i = 0; i < N; ++i) {
    mp1->Push();
    blk1[i] = mp1->Allocate(32 + i * 2);
    memset(blk1[i], 3, 32 + i * 2);
  }
  for (int i = 0; i < N; ++i) {
    mp1->Pop();
  }
  // last pop clears anything
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
  delete mp1;
}  // Test_stacked_mem_pool

TEST(MemPoolTest, Stacked) {
  Test_stacked_mem_pool<SMALL_STACKED_MEM_POOL>(512 - 16 + 1);
  Test_stacked_mem_pool<MEDIUM_STACKED_MEM_POOL>(4096 - 16 + 1);
}  // MemPoolTest.Stacked

}  // namespace
