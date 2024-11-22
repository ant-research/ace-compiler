//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/meta_info.h"
#include "gtest/gtest.h"
#include "nn/vector/vector_opcode.h"

using namespace air::base;
using namespace nn::vector;

TEST(VECTOR, vector_opcode) {
  // check register process
  bool reg_res = Register_vector_domain();
  ASSERT_TRUE(reg_res);

  // check domain id
  bool is_valid_domain_id = META_INFO::Valid_domain(VECTOR_DOMAIN::ID);
  ASSERT_TRUE(is_valid_domain_id);

  // check opcodes
  bool op_is_valid =
      META_INFO::Valid_operator(VECTOR_DOMAIN::ID, VECTOR_OPCODE::ADD);
  ASSERT_TRUE(op_is_valid);
}
