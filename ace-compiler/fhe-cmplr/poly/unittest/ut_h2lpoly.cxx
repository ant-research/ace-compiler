//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/test/lower_ckks.h"

using namespace air::base;
using namespace fhe::core;
using namespace fhe::poly;

namespace fhe {
namespace poly {
namespace test {
class TEST_H2LPOLY : public TEST_LOWER_CKKS<TEST_CONFIG> {};

TEST_P(TEST_H2LPOLY, Handle_add_ciph) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_y());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_add_plain) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_p());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_mul_plain) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_mul_ciph) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_mul_ciph_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph3_ty());
  STMT_PTR stmt = Ckks_ir_gen().Gen_mul(Container(), preg_z, Var_x(), Var_y());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_mul_float) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_relin_inline) {
  Config()._inline_relin = true;
  STMT_PTR stmt = Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_relin_func) {
  GTEST_SKIP() << "relin call is not supported yet";
  Config()._inline_relin = false;
  STMT_PTR stmt = Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_relin_func_preg) {
  GTEST_SKIP() << "relin call is not supported yet";
  Config()._inline_relin = false;
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Ckks_ir_gen().Gen_relin(Container(), preg_z, Var_ciph3());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_rotate_inline) {
  Config()._inline_rotate = true;
  STMT_PTR stmt = Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_rotate_func) {
  GTEST_SKIP() << "rotate call is not supported yet";
  Config()._inline_rotate = false;
  STMT_PTR stmt = Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_rotate_func_preg) {
  GTEST_SKIP() << "rotate call is not supported yet";
  Config()._inline_rotate = false;
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  PREG_PTR preg_x = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Ckks_ir_gen().Gen_rotate(Container(), preg_z, preg_x, 5);
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_rescale) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_rescale(Container(), Var_z(), Var_x());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_bts) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_bootstrap(Container(), Var_x());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_encode) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_encode(Container(), Var_p());
  Lower();
}

TEST_P(TEST_H2LPOLY, Handle_all) {
  Config()._inline_relin  = true;
  Config()._inline_rotate = true;
  Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_y());
  Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_p());
  Ckks_ir_gen().Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  Ckks_ir_gen().Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  Ckks_ir_gen().Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Ckks_ir_gen().Gen_rescale(Container(), Var_z(), Var_x());
  Ckks_ir_gen().Gen_bootstrap(Container(), Var_x());
  Ckks_ir_gen().Gen_encode(Container(), Var_p());
  Ckks_ir_gen().Gen_ret(Container(), Var_z());

  Lower();
}

// testing same tests with different config
INSTANTIATE_TEST_SUITE_P(H2LPOLY_TEST, TEST_H2LPOLY,
                         ::testing::Values(TEST_CONFIG(".lpoly.t", LPOLY)));

}  // namespace test
}  // namespace poly
}  // namespace fhe
