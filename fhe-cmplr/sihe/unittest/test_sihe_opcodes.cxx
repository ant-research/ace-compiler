//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/meta_info.h"
#include "fhe/sihe/sihe_opcode.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace fhe::sihe;

TEST(SIHE, sihe_opcode) {
  // check register process
  bool reg_res = Register_sihe_domain();
  ASSERT_TRUE(reg_res);

  // check domain id
  bool is_valid_domain_id = META_INFO::Valid_domain(SIHE_DOMAIN::ID);
  ASSERT_TRUE(is_valid_domain_id);

  // check opcodes
  bool op_is_valid =
      META_INFO::Valid_operator(SIHE_DOMAIN::ID, SIHE_OPERATOR::ADD);
  ASSERT_TRUE(op_is_valid);
  op_is_valid = META_INFO::Valid_operator(SIHE_DOMAIN::ID, SIHE_OPERATOR::MUL);
  ASSERT_TRUE(op_is_valid);

#define SIHE_OPCODE(NAME, name, category, kid_num, fld_num, property)  \
  op_is_valid =                                                        \
      META_INFO::Valid_operator(SIHE_DOMAIN::ID, SIHE_OPERATOR::NAME); \
  ASSERT_TRUE(op_is_valid);
#include "fhe/sihe/opcode_def.inc"
}