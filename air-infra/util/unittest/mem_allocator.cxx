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

#include "air/util/mem_allocator.h"

using namespace air::util;

namespace {

int Ctor_a, Ctor_a_sum;
int Dtor_a, Dtor_a_sum;

struct A {
  A(int a) : _a(a) {
    Ctor_a_sum += a;
    Ctor_a++;
  }
  ~A() {
    Dtor_a_sum += _a;
    Dtor_a++;
  }
  int _a;
};  // struct A

int Ctor_b, Ctor_b_sum;
int Dtor_b, Dtor_b_sum;

struct B {
  B(int a, int b) : _a(a), _b(b) {
    Ctor_b_sum += a + b;
    Ctor_b++;
  }
  ~B() {
    Dtor_b_sum += _a + _b;
    Dtor_b++;
  }
  int _a;
  int _b;
};  // struct B

struct C {
  C(int v) : _v(v) { memset(_data, _v, sizeof(_data)); }
  ~C() {
    for (int i = 0; i < sizeof(_data); ++i) {
      EXPECT_EQ(_data[i], _v);
    }
  }
  uint8_t _v;
  uint8_t _data[8192];
};  // struct C

typedef MEM_POOL<512>  SMALL_MEM_POOL;
typedef MEM_POOL<4096> MEDIUM_MEM_POOL;

template <typename T>
void Test_mem_allocator(uint32_t large_size) {
  T* mp = new T;
  {
    Ctor_a     = 0;
    Ctor_a_sum = 0;
    Dtor_a     = 0;
    Dtor_a_sum = 0;
    MEM_ALLOCATOR<T> alloc(mp);
    A*               a = alloc.template Allocate<A>(1);
    EXPECT_EQ(Ctor_a, 1);
    EXPECT_EQ(Ctor_a_sum, 1);
    alloc.template Deallocate<A>(a);
    EXPECT_EQ(Dtor_a, 1);
    EXPECT_EQ(Dtor_a_sum, 1);
    A* b = alloc.template Allocate_array<A>(10, 1);
    EXPECT_EQ(Ctor_a, 11);
    EXPECT_EQ(Ctor_a_sum, 11);
    alloc.template Deallocate_array<A>(b, 10);
    EXPECT_EQ(Dtor_a, 11);
    EXPECT_EQ(Dtor_a_sum, 11);
  }
  {
    Ctor_b     = 0;
    Ctor_b_sum = 0;
    Dtor_b     = 0;
    Dtor_b_sum = 0;
    MEM_ALLOCATOR<T> alloc(mp);
    B*               b0 = alloc.template Allocate<B>(1, 1);
    EXPECT_EQ(Ctor_b, 1);
    EXPECT_EQ(Ctor_b_sum, 2);
    B* b1 = alloc.template Allocate<B>(2, 3);
    EXPECT_EQ(Ctor_b, 2);
    EXPECT_EQ(Ctor_b_sum, 7);
    alloc.template Deallocate<B>(b1);
    EXPECT_EQ(Dtor_b, 1);
    EXPECT_EQ(Dtor_b_sum, 5);
    alloc.template Deallocate<B>(b0);
    EXPECT_EQ(Dtor_b, 2);
    EXPECT_EQ(Dtor_b_sum, 7);
    B* b3 = alloc.template Allocate_array<B>(10, 1, 2);
    EXPECT_EQ(Ctor_b, 12);
    EXPECT_EQ(Ctor_b_sum, 37);
    alloc.template Deallocate_array<B>(b3, 10);
    EXPECT_EQ(Dtor_b, 12);
    EXPECT_EQ(Dtor_b_sum, 37);
  }
  {
    MEM_ALLOCATOR<T> alloc(mp);
    C*               c0 = alloc.template Allocate<C>(1);
    C*               c1 = alloc.template Allocate<C>(2);
    C*               c2 = alloc.template Allocate<C>(3);
    alloc.template Deallocate<C>(c0);
    alloc.template Deallocate<C>(c1);
    alloc.template Deallocate<C>(c2);
  }
  delete mp;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
}  // Test_mem_allocator

TEST(MemAllocTest, Basic) {
  Test_mem_allocator<SMALL_MEM_POOL>(512 - 16 + 1);
  Test_mem_allocator<MEDIUM_MEM_POOL>(4096 - 16 + 1);
}  // MemAllocTest.Basic

typedef STACKED_MEM_POOL<512>  SMALL_STACKED_MEM_POOL;
typedef STACKED_MEM_POOL<4096> MEDIUM_STACKED_MEM_POOL;

template <typename T>
void Test_stacked_mem_alloc(uint32_t large_size) {
  constexpr int    N = 512;
  A*               a[N];
  B*               b[N];
  C*               c[N];
  T*               mp0 = new T;
  MEM_ALLOCATOR<T> alloc0(mp0);
  for (int i = 0; i < N; ++i) {
    a[i] = alloc0.template Allocate<A>(i);
    mp0->Push();
    b[i] = alloc0.template Allocate<B>(i, i * 2);
    c[i] = alloc0.template Allocate<C>(i);
  }
  for (int i = N - 1; i >= 0; --i) {
    alloc0.template Deallocate<C>(c[i]);
    alloc0.template Deallocate<B>(b[i]);
    mp0->Pop();
    alloc0.template Deallocate<A>(a[i]);
  }
  // still remain something in mem pool
  EXPECT_FALSE(MEM_POOL_MANAGER::All_freed());
  delete mp0;
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());

  // a more typical use case
  T*               mp1 = new T;
  MEM_ALLOCATOR<T> alloc1(mp1);
  for (int i = 0; i < N; ++i) {
    mp1->Push();
    a[i] = alloc1.template Allocate<A>(i);
    b[i] = alloc1.template Allocate<B>(i, i * 2);
    c[i] = alloc1.template Allocate<C>(i);
  }
  for (int i = 0; i < N; ++i) {
    mp1->Pop();
  }
  // last pop clears anything
  EXPECT_TRUE(MEM_POOL_MANAGER::All_freed());
  delete mp1;
}  // Test_stacked_mem_alloc

TEST(MemAllocTest, Stacked) {
  Test_stacked_mem_alloc<SMALL_STACKED_MEM_POOL>(512 - 16 + 1);
  Test_stacked_mem_alloc<MEDIUM_STACKED_MEM_POOL>(4096 - 16 + 1);
}  // MemAllocTest.Stacked

}  // namespace
