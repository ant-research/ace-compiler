//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/ptr_util.h"

#include "gtest/gtest.h"

using namespace air::util;

namespace {

struct A {
  int8_t               Foo() { return 'a'; }
  static constexpr int TAG_VALUE = 1;
  uintptr_t            _v;
};  // struct A

struct B {
  int8_t               Foo() { return 'b'; }
  static constexpr int TAG_VALUE = 2;
  uintptr_t            _v;
};  // struct B

enum TAG {
  TAG_A = 1,
  TAG_B = 2,
};  // enum TAG

TEST(TaggedPointerTest, Basic) {
  struct A       a;
  struct B       b;
  TAGGED_POINTER tga(&a, TAG_A);
  TAGGED_POINTER tgb(&b, TAG_B);
  EXPECT_EQ(tga.Tag<TAG>(), TAG_A);
  EXPECT_EQ(tgb.Tag<TAG>(), TAG_B);
  EXPECT_EQ(tga.Ptr<A>(), &a);
  EXPECT_EQ(tgb.Ptr<B>(), &b);
}  // TaggedPointerTest.Basic

TEST(TaggedPointerTest, Operator) {
  struct A       a;
  struct B       b;
  TAGGED_POINTER tga(&a, TAG_A);
  TAGGED_POINTER tgb(&b, TAG_B);
  EXPECT_EQ(tga.Ptr<A>()->Foo(), 'a');
  EXPECT_EQ(tga.Ptr<B>()->Foo(), 'b');
  EXPECT_EQ(((A*)tga)->Foo(), 'a');
  EXPECT_EQ(((B*)tgb)->Foo(), 'b');
  EXPECT_EQ((uintptr_t)tga, (uintptr_t)&a | TAG_A);
  EXPECT_EQ((uintptr_t)tgb, (uintptr_t)&b | TAG_B);
  EXPECT_TRUE((bool)tga);
  EXPECT_FALSE(!(bool)tga);

  TAGGED_POINTER np;
  EXPECT_EQ((uintptr_t)np, 0);
  EXPECT_FALSE((bool)np);
}  // TaggedPointerTest.Operator

}  // namespace
