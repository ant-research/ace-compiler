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

#include "air/util/mem_block.h"

using namespace air::util;

namespace {

typedef MEM_BLOCK<512>  MEM_SMALL_BLOCK;
typedef MEM_BLOCK<4096> MEM_NORMAL_BLOCK;

template <typename T>
void Test_mem_block(uint32_t large_size) {
  T*    blk = T::Create();
  char* s0  = blk->Allocate(0);
  char* s1  = blk->Allocate(1);
  EXPECT_TRUE(Is_aligned(s1, 1));
  *s1      = 'a';
  char* s2 = blk->Allocate(2);
  EXPECT_TRUE(Is_aligned(s2, 2));
  *(uint16_t*)s2 = 0x1234;
  char* s3       = blk->Allocate(8);
  EXPECT_TRUE(Is_aligned(s3, 8));
  *(uint64_t*)s3 = 0x1234567890abcdef;
  EXPECT_EQ(s2 - s1, s1 - s0 + sizeof(uintptr_t));
  EXPECT_EQ(s3 - s2, s2 - s1);
  blk->Deallocate(s0, 0);
  blk->Deallocate(s1, 0);
  blk->Deallocate(s2, 0);
  blk->Deallocate(s3, 0);
  char* s4 = blk->Allocate(large_size);
  EXPECT_EQ(s4, nullptr);
  char* s5 = blk->Allocate(32);
  EXPECT_TRUE(Is_aligned(s5, 8));
  memset(s5, 0, 32);
  char* s6 = blk->Allocate(32);
  EXPECT_TRUE(Is_aligned(s6, 8));
  memset(s6, 0, 32);
  char* s7 = blk->Allocate(32);
  EXPECT_TRUE(Is_aligned(s7, 8));
  memset(s7, 0, 32);
  char* s8 = blk->Allocate(32);
  EXPECT_TRUE(Is_aligned(s8, 8));
  memset(s8, 0, 32);
  EXPECT_GT(s5, s3);
  EXPECT_EQ(s7 - s6, s6 - s5);
  EXPECT_EQ(s8 - s7, s7 - s6);
  blk->Deallocate(s5, 0);
  blk->Deallocate(s6, 0);
  blk->Deallocate(s7, 0);
  blk->Deallocate(s8, 0);
  T::Destroy(blk);
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_mem_block

TEST(MemBlockTest, Block) {
  Test_mem_block<MEM_SMALL_BLOCK>(500);
  Test_mem_block<MEM_NORMAL_BLOCK>(4000);
}  // MemBlockTest.Block

TEST(MemBlockTest, LargeBlock) {
  MEM_LARGE_BLOCK* blk0 = MEM_LARGE_BLOCK::Create(1024);
  char*            addr = blk0->Address();
  memset(addr, 0, 1024);
  MEM_LARGE_BLOCK::Destroy(blk0);
  MEM_LARGE_BLOCK* blk1 = MEM_LARGE_BLOCK::Create(512);
  addr                  = blk1->Address();
  memset(addr, 0, 512);
  MEM_LARGE_BLOCK::Destroy(blk1);
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // MemBlockTest.LargeBlock

}  // namespace
