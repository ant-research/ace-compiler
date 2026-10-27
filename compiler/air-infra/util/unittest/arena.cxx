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

#include "air/util/arena.h"

using namespace air::util;

namespace {

int Ctor_a, Ctor_a_sum;
int Dtor_a, Dtor_a_sum;

struct NODE {
  NODE(int a) : _a(a) {
    Ctor_a_sum += a;
    Ctor_a++;
    for (int i = 0; i < a; ++i) {
      _kid[i] = 0x1234567890abcdef;
    }
    EXPECT_EQ(_kid[-1], a);
  }
  ~NODE() {
    Dtor_a_sum += _a;
    Dtor_a++;
    for (int i = 0; i < (int)_a; ++i) {
      EXPECT_EQ(_kid[i], 0x1234567890abcdef);
    }
    EXPECT_EQ(_kid[-1], _a);
  }
  char      _data[16];
  uintptr_t _a;
  uintptr_t _kid[];
};  // struct NODE

int Ctor_b, Ctor_b_sum;
int Dtor_b, Dtor_b_sum;

struct SYMBOL {
  SYMBOL(int a, int b) : _a(a), _b(b) {
    Ctor_b_sum += a + b;
    Ctor_b++;
  }
  ~SYMBOL() {
    Dtor_b_sum += _a + _b;
    Dtor_b++;
  }
  int _a;
  int _b;
};  // struct SYMBOL

template <uint32_t BLK_SIZE>
void Test_node_arena() {
  MEM_ID_POOL<BLK_SIZE, 8>* mp = new MEM_ID_POOL<BLK_SIZE, 8>();
  ARENA<BLK_SIZE>           alloc(mp);
  constexpr int             N = 32;
  OBJ_ID<NODE>              n[N];
  Ctor_a     = 0;
  Ctor_a_sum = 0;
  Dtor_a     = 0;
  Dtor_a_sum = 0;
  for (int i = 0; i < N; ++i) {
    n[i] = alloc.template Allocate_extra<NODE>(i * 8, i);
    EXPECT_EQ(alloc.template Id_to_ptr<NODE>(n[i].Id()), n[i].Ptr());
    if (i > 0) {
      EXPECT_GT(n[i].Id(), n[i - 1].Id());
    }
  }
  EXPECT_EQ(Ctor_a, N);
  EXPECT_EQ(Ctor_a_sum, N * (N - 1) / 2);
  for (int i = 0; i < N; ++i) {
    alloc.template Deallocate_extra<NODE>(n[i].Ptr(), i * 8);
  }
  EXPECT_EQ(Dtor_a, N);
  EXPECT_EQ(Dtor_a_sum, Ctor_a_sum);
  // for (int i = 0; i < N; ++i) {
  //   NODE* n0 = alloc.template Allocate_extra<NODE>(i*8, i);
  //   EXPECT_EQ(n0, n[i]);
  // }
  // EXPECT_EQ(Ctor_a, 2*N);
  // EXPECT_EQ(Ctor_a_sum, N*(N-1));
  delete mp;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_node_arena

TEST(AirArenaTest, Node) {
  Test_node_arena<512>();
  Test_node_arena<4096>();
}  // AirArenaTest.Node

template <uint32_t BLK_SIZE>
void Test_symbol_arena() {
  {
    ARENA<BLK_SIZE> alloc;
    constexpr int   N = 32;
    OBJ_ID<SYMBOL>  s[N];
    Ctor_b     = 0;
    Ctor_b_sum = 0;
    Dtor_b     = 0;
    Dtor_b_sum = 0;
    for (int i = 0; i < N; ++i) {
      s[i] = alloc.template Allocate<SYMBOL>(1, 1);
      EXPECT_EQ(alloc.template Id_to_ptr<SYMBOL>(s[i].Id()), s[i].Ptr());
      if (i > 0) {
        EXPECT_GT(s[i].Id(), s[i - 1].Id());
      }
    }
    EXPECT_EQ(Ctor_b, N);
    EXPECT_EQ(Ctor_b_sum, 2 * N);
    for (int i = 0; i < N; ++i) {
      alloc.template Deallocate<SYMBOL>(s[i].Ptr());
    }
    EXPECT_EQ(Dtor_b, N);
    EXPECT_EQ(Dtor_b_sum, 2 * N);
    // for (int i = N-1; i >= 0; --i) {
    //   SYMBOL* s0 = alloc.template Allocate<SYMBOL>(1, 1);
    //   EXPECT_EQ(s0, s[i]);
    // }
    // EXPECT_EQ(Ctor_b, 2*N);
    // EXPECT_EQ(Ctor_b_sum, 4*N);
    // SYMBOL* s1 = alloc.template Allocate<SYMBOL>(1, 1);
    // for (int i = 0; i < N; ++i) {
    //   alloc.template Deallocate<SYMBOL>(s1);
    //   SYMBOL* s0 = alloc.template Allocate<SYMBOL>(1, 1);
    //   EXPECT_EQ(s0, s1);
    //   s1 = s0;
    // }
  }
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_symbol_arena

TEST(AirArenaTest, Symbol) {
  Test_symbol_arena<512>();
  Test_symbol_arena<4096>();
}  // AirArenaTest.Symbol

template <uint32_t BLK_SIZE>
void Test_raw_arena() {
  {
    ARENA<BLK_SIZE> alloc;
    constexpr int   N = 32;
    MEM_ID          s[N];
    for (int i = 0; i < N; ++i) {
      s[i] = alloc.Allocate_raw(i * 512 + 1);
      EXPECT_EQ(alloc.Id_to_raw(s[i].Id()), s[i].Addr());
      if (i > 0) {
        EXPECT_GT(s[i].Id(), s[i - 1].Id());
      }
    }
    for (int i = 0; i < N; ++i) {
      alloc.Deallocate_raw(s[i].Addr(), i * 512 + 1);
    }
  }
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_symbol_arena

TEST(AirArenaTest, Raw) {
  Test_raw_arena<512>();
  Test_raw_arena<4096>();
}  // AirArenaTest.Raw

}  // namespace
