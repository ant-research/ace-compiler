//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks2poly.h"
#include "fhe/test/lower_ckks.h"

using namespace air::base;
using namespace fhe::core;
using namespace fhe::poly;

namespace fhe {
namespace poly {
extern const POLY_FUNC_INFO* Poly_func_info(FHE_FUNC func_id);
}  // namespace poly
}  // namespace fhe

namespace fhe {
namespace poly {
namespace test {
class TEST_CKKS2POLY : public TEST_LOWER_CKKS<TEST_CONFIG> {};

TEST_P(TEST_CKKS2POLY, Handle_add_ciph) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_y());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_add_plain) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_p());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_mul_plain) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_mul_ciph) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_mul_ciph_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph3_ty());
  STMT_PTR stmt = Ckks_ir_gen().Gen_mul(Container(), preg_z, Var_x(), Var_y());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_mul_float) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_relin_inline) {
  Config()._inline_relin = true;
  STMT_PTR stmt = Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_relin_func) {
  Config()._inline_relin = false;
  STMT_PTR stmt = Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_relin_func_preg) {
  Config()._inline_relin = false;
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Ckks_ir_gen().Gen_relin(Container(), preg_z, Var_ciph3());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_rotate_inline) {
  Config()._inline_rotate = true;
  STMT_PTR stmt = Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_rotate_func) {
  Config()._inline_rotate = false;
  STMT_PTR stmt = Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_rotate_func_preg) {
  Config()._inline_rotate = false;
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  PREG_PTR preg_x = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Ckks_ir_gen().Gen_rotate(Container(), preg_z, preg_x, 5);
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_rescale) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_rescale(Container(), Var_z(), Var_x());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_bts) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_bootstrap(Container(), Var_x());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_encode) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_encode(Container(), Var_p());
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_stp) {
  CONTAINER* cntr = Container();
  STMT_LIST  sl   = cntr->Stmt_list();
  NODE_PTR   n_x  = cntr->New_ld(Var_x(), Spos());
  NODE_PTR   rescale_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::RESCALE),
                          Ciph_ty(), Spos());
  rescale_node->Set_child(0, n_x);
  PREG_PTR tmp          = cntr->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR rescale_stmt = cntr->New_stp(rescale_node, tmp, Spos());
  sl.Append(rescale_stmt);
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_call) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_call(Container(), Container()->Parent_func_scope(), 1);

  NODE_PTR add_node = Ckks_ir_gen().Gen_add_node(Container(), Var_x(), Var_y());
  stmt->Node()->Set_child(0, add_node);
  Lower();
}

TEST_P(TEST_CKKS2POLY, Handle_all) {
  Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_y());
  Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_p());
  Ckks_ir_gen().Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  Ckks_ir_gen().Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  Ckks_ir_gen().Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Ckks_ir_gen().Gen_rescale(Container(), Var_z(), Var_x());
  Ckks_ir_gen().Gen_bootstrap(Container(), Var_x());
  Ckks_ir_gen().Gen_encode(Container(), Var_p());
  Ckks_ir_gen().Gen_ret(Container(), Var_z());

  Lower();
}

TEST(POLY_IR_GEN, Builtin_func) {
  const POLY_FUNC_INFO* rotate_func = Poly_func_info(FHE_FUNC::ROTATE);
  EXPECT_TRUE(rotate_func != nullptr);
  EXPECT_EQ(rotate_func->_func_id, FHE_FUNC::ROTATE);
  EXPECT_EQ(rotate_func->_retv_type, CIPH);
  EXPECT_EQ(rotate_func->_param_types[0], CIPH);
  EXPECT_EQ(rotate_func->_param_types[1], INT32);
  EXPECT_EQ(sizeof(POLY_FUNC_INFO), 80);
  EXPECT_EQ(OFFSETOF(POLY_FUNC_INFO, _fname), 72);
}

// testing same tests with different config
INSTANTIATE_TEST_SUITE_P(CKKS2POLY_TEST, TEST_CKKS2POLY,
                         ::testing::Values(TEST_CONFIG(".spoly.t", SPOLY)));
}  // namespace test
}  // namespace poly
}  // namespace fhe
