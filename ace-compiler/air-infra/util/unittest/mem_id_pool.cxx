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

#include "air/util/mem_id_pool.h"

using namespace air::util;

namespace {

typedef MEM_ID_POOL<512, 4>  SMALL_ID_POOL;
typedef MEM_ID_POOL<4096, 4> MEDIUM_ID_POOL;

template <typename T>
void Test_mem_pool(uint32_t large_size) {
  T*     mp = new T;
  MEM_ID s0 = mp->Allocate(0);
  EXPECT_EQ(s0.Addr(), mp->Id_to_addr(s0.Id()));
  MEM_ID s1 = mp->Allocate(1);
  EXPECT_TRUE(Is_aligned(s1.Addr(), 1));
  *s1.Addr() = 'a';
  EXPECT_EQ(s1.Addr(), mp->Id_to_addr(s1.Id()));
  MEM_ID s2 = mp->Allocate(2);
  EXPECT_TRUE(Is_aligned(s2.Addr(), 2));
  *(uint16_t*)s2.Addr() = 0x1234;
  EXPECT_EQ(s2.Addr(), mp->Id_to_addr(s2.Id()));
  MEM_ID s3 = mp->Allocate(8);
  EXPECT_TRUE(Is_aligned(s3.Addr(), 8));
  *(uint64_t*)s3.Addr() = 0x1234567890abcdef;
  EXPECT_EQ(s3.Addr(), mp->Id_to_addr(s3.Id()));
  EXPECT_EQ(s2.Addr() - s1.Addr(), s1.Addr() - s0.Addr() + sizeof(uintptr_t));
  EXPECT_EQ(s3.Addr() - s2.Addr(), s2.Addr() - s1.Addr());
  mp->Deallocate(s0.Addr(), 0);
  mp->Deallocate(s1.Id(), 0);
  mp->Deallocate(s2.Addr(), 0);
  mp->Deallocate(s3.Id(), 0);

  MEM_ID s4 = mp->Allocate(large_size);
  ASSERT_NE(s4.Addr(), nullptr);
  memset(s4.Addr(), 0, large_size);
  EXPECT_EQ(s4.Addr(), mp->Id_to_addr(s4.Id()));
  mp->Deallocate(s4.Addr(), large_size);
  MEM_ID s5 = mp->Allocate(large_size);
  ASSERT_NE(s5.Addr(), nullptr);
  memset(s5.Addr(), 0, large_size);
  EXPECT_EQ(s5.Addr(), mp->Id_to_addr(s5.Id()));
  mp->Deallocate(s5.Id(), large_size);

  MEM_ID s6 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s6.Addr(), 8));
  memset(s6.Addr(), 0, 32);
  EXPECT_EQ(s6.Addr(), mp->Id_to_addr(s6.Id()));
  MEM_ID s7 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s7.Addr(), 8));
  memset(s7.Addr(), 0, 32);
  EXPECT_EQ(s7.Addr(), mp->Id_to_addr(s7.Id()));
  MEM_ID s8 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s8.Addr(), 8));
  memset(s8.Addr(), 0, 32);
  EXPECT_EQ(s8.Addr(), mp->Id_to_addr(s8.Id()));
  MEM_ID s9 = mp->Allocate(32);
  EXPECT_TRUE(Is_aligned(s9.Addr(), 8));
  memset(s9.Addr(), 0, 32);
  EXPECT_EQ(s9.Addr(), mp->Id_to_addr(s9.Id()));
  EXPECT_GT(s6.Addr(), s3.Addr());
  EXPECT_EQ(s8.Addr() - s7.Addr(), s7.Addr() - s6.Addr());
  EXPECT_EQ(s9.Addr() - s8.Addr(), s8.Addr() - s7.Addr());
  mp->Deallocate(s6.Addr(), 0);
  mp->Deallocate(s7.Id(), 0);
  mp->Deallocate(s8.Addr(), 0);
  mp->Deallocate(s9.Id(), 0);
  delete mp;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_mem_pool

TEST(MemIdPoolTest, Basic) {
  Test_mem_pool<SMALL_ID_POOL>(512 - 16 + 1);
  Test_mem_pool<MEDIUM_ID_POOL>(4096 - 16 + 1);
}  // MemIdPoolTest.Basic

template <typename T>
void Test_mem_id_pool_state(uint32_t large_size) {
  constexpr int N = 512;
  MEM_ID        blk0[N];
  MEM_ID        blk1[N];
  T*            mp0 = new T;
  mp0->Bookmark();
  for (int i = 0; i < N; ++i) {
    blk0[i] = mp0->Allocate(i * 2);
    memset(blk0[i].Addr(), 1, i * 2);
    EXPECT_EQ(blk0[i].Addr(), mp0->Id_to_addr(blk0[i].Id()));
  }
  mp0->Restore();
  mp0->Bookmark();
  for (int i = 0; i < N; ++i) {
    blk1[i] = mp0->Allocate(i * 2);
    memset(blk1[i].Addr(), 1, i * 2);
    EXPECT_EQ(blk1[i].Addr(), mp0->Id_to_addr(blk1[i].Id()));
  }
  for (int i = 0; i < N; ++i) {
    EXPECT_EQ(blk0[i].Id(), blk1[i].Id());
  }
  mp0->Restore();
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
  delete mp0;

  T* mp1 = new T;
  mp1->Bookmark();
  for (int i = 0; i < N; ++i) {
    blk0[i] = mp1->Allocate(i * 2);
    memset(blk0[i].Addr(), 1, i * 2);
    EXPECT_EQ(blk0[i].Addr(), mp1->Id_to_addr(blk0[i].Id()));
    mp1->Restore();
    mp1->Bookmark();
    blk1[i] = mp1->Allocate(i * 2);
    memset(blk1[i].Addr(), 1, i * 2);
    EXPECT_EQ(blk1[i].Addr(), mp1->Id_to_addr(blk1[i].Id()));
    EXPECT_EQ(blk0[i].Id(), blk1[i].Id());
    mp1->Restore();
    mp1->Bookmark();
  }
  mp1->Restore();
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
  delete mp1;

  T* mp2 = new T;
  for (int i = 0; i < 16; ++i) {
    mp2->Bookmark();
    for (int j = 0; j < N / 16; ++j) {
      blk0[j] = mp2->Allocate(i * N / 16 + j * 2);
      memset(blk0[j].Addr(), 1, i * N / 16 + j * 2);
      EXPECT_EQ(blk0[j].Addr(), mp2->Id_to_addr(blk0[j].Id()));
    }
    mp2->Restore();
    mp2->Bookmark();
    for (int j = 0; j < N / 16; ++j) {
      blk1[j] = mp2->Allocate(i * N / 16 + j * 2);
      memset(blk1[j].Addr(), 1, i * N / 16 + j * 2);
      EXPECT_EQ(blk1[j].Addr(), mp2->Id_to_addr(blk1[j].Id()));
    }
    mp2->Restore();
    for (int j = 0; j < N / 16; ++j) {
      EXPECT_EQ(blk0[j].Id(), blk1[j].Id());
    }
  }
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
  delete mp2;
}

TEST(MemIdPoolTest, State) {
  Test_mem_id_pool_state<SMALL_ID_POOL>(512 - 16 + 1);
  Test_mem_id_pool_state<MEDIUM_ID_POOL>(4096 - 16 + 1);
}

}  // namespace
