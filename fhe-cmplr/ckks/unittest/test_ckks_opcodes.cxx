//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/meta_info.h"
#include "fhe/ckks/ckks_opcode.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace fhe::ckks;

TEST(CKKS, ckks_opcode) {
  // check register process
  bool reg_res = Register_ckks_domain();
  ASSERT_TRUE(reg_res);

  // check domain id
  bool is_valid_domain_id = META_INFO::Valid_domain(CKKS_DOMAIN::ID);
  ASSERT_TRUE(is_valid_domain_id);

  // check opcodes
  bool op_is_valid =
      META_INFO::Valid_operator(CKKS_DOMAIN::ID, CKKS_OPERATOR::ADD);
  ASSERT_TRUE(op_is_valid);
  op_is_valid = META_INFO::Valid_operator(CKKS_DOMAIN::ID, CKKS_OPERATOR::MUL);
  ASSERT_TRUE(op_is_valid);

#define CKKS_OPCODE(NAME, name, category, kid_num, fld_num, property)  \
  op_is_valid =                                                        \
      META_INFO::Valid_operator(CKKS_DOMAIN::ID, CKKS_OPERATOR::NAME); \
  ASSERT_TRUE(op_is_valid);
#include "fhe/ckks/opcode_def.inc"
}