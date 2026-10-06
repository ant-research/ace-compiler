//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/test/lower_ckks.h"

using namespace air::base;
using namespace air::util;
using namespace fhe::ckks;
using namespace fhe::poly;

namespace fhe {
namespace poly {
namespace test {

class TEST_HP1TOHP2 : public TEST_LOWER_CKKS<TEST_CONFIG> {};

TEST_P(TEST_HP1TOHP2, Handle_relin_inline) {
  CKKS_IR_GEN& irgen     = Ckks_ir_gen();
  Config()._inline_relin = true;
  STMT_PTR stmt = irgen.Gen_mul(Container(), Var_ciph3(), irgen.Input_var(),
                                irgen.Input_var());
  STMT_PTR stmt2 =
      irgen.Gen_relin(Container(), irgen.Output_var(), Var_ciph3());
  Ckks_ir_gen().Append_output();
  Lower();
}

TEST_P(TEST_HP1TOHP2, Handle_rotate_inline) {
  Config()._inline_rotate = true;
  CKKS_IR_GEN& irgen      = Ckks_ir_gen();
  STMT_PTR     stmt = Ckks_ir_gen().Gen_rotate(Container(), irgen.Output_var(),
                                               irgen.Input_var(), 5);
  Set_rot_idx_attr(stmt->Node()->Child(0), {5});
  Ckks_ir_gen().Append_output();
  Lower();
}

TEST_P(TEST_HP1TOHP2, Handle_rescale) {
  CKKS_IR_GEN& irgen = Ckks_ir_gen();
  STMT_PTR     stmt = Ckks_ir_gen().Gen_rescale(Container(), irgen.Output_var(),
                                                irgen.Input_var());
  Ckks_ir_gen().Append_output();
  Lower();
}

TEST_P(TEST_HP1TOHP2, Handle_raise_mod) {
  const_cast<TEST_CONFIG&>(this->GetParam()).Set_input_level(1);
  STMT_PTR stmt = Ckks_ir_gen().Gen_raise_mod(
      Container(), Ckks_ir_gen().Output_var(), Ckks_ir_gen().Input_var(), 0);
  Set_level_attr(stmt->Node()->Child(0)->Child(0), 1);
  Set_sf_degree(stmt->Node()->Child(0)->Child(0), 1);
  Lower();
}

// testing same tests with different config
INSTANTIATE_TEST_SUITE_P(HP1TOHP2_TEST, TEST_HP1TOHP2,
                         ::testing::Values(TEST_CONFIG(".hpoly2.t", HPOLY_P2)));
}  // namespace test
}  // namespace poly
}  // namespace fhe
