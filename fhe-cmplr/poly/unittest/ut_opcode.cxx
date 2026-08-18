//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/meta_info.h"
#include "fhe/poly/opcode.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace fhe::poly;
namespace {

TEST(POLYNOMIAL, register_domain) {
  bool ret = Register_polynomial();
  EXPECT_EQ(ret, true);
  EXPECT_EQ(META_INFO::Num_domain(), POLYNOMIAL_DID + 1);
  EXPECT_TRUE(META_INFO::Valid_domain(POLYNOMIAL_DID));
  META_INFO::Remove_all();
}

TEST(POLYNOMIAL, operators) {
  Register_polynomial();

  EXPECT_EQ(META_INFO::Num_domain_op(POLYNOMIAL_DID), fhe::poly::LAST_VALID);

  for (int i = 0; i < fhe::poly::LAST_VALID; i++) {
    EXPECT_TRUE(META_INFO::Valid_operator(POLYNOMIAL_DID, i));
  }
  META_INFO::Remove_all();
}

}  // namespace
