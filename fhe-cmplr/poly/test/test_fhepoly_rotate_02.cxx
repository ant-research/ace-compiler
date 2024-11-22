//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <limits.h>

#include "fhe/core/ctx_param_ana.h"
#include "gen_ckks_ir.h"
#include "gen_expect_data.h"

using namespace air::base;
using namespace air::util;
using namespace fhe::core;
using namespace fhe::ckks;
using namespace fhe::sihe;
using namespace fhe::poly;
using namespace fhe::poly::test;

int32_t Rot_idx = 2;
void    Create_ckks_ir(CKKS_IR_GEN& ir_gen);

int main(int argc, char** argv) {
  fhe::core::LOWER_CTX fhe_ctx;
  CKKS_IR_GEN          ir_gen(fhe_ctx);
  Create_ckks_ir(ir_gen);

  CONTAINER*             cntr = ir_gen.Container();
  fhe::poly::POLY_DRIVER poly_driver;
  fhe::poly::POLY_CONFIG poly_config;

  // do not inline rotate IR, generate into a new function
  poly_config.Set_inline_rotate(false);

  GLOB_SCOPE* glob = poly_driver.Run(poly_config, cntr->Glob_scope(), fhe_ctx);

  std::ofstream            of("test_rotate_02.inc");
  fhe::poly::POLY2C_CONFIG p2c_config;
  fhe::poly::POLY2C_DRIVER poly2c(of, fhe_ctx, p2c_config);
  poly2c.Run(glob);
  Gen_expected(of);
  std::cout << "Output: test_rotate_02.inc" << std::endl;
  return 0;
}

// output = roate(input, 2) + rotate(input, 3)
void Create_ckks_ir(CKKS_IR_GEN& ir_gen) {
  CONTAINER* cntr = ir_gen.Container();
  STMT_LIST  sl   = cntr->Stmt_list();
  SPOS       spos = ir_gen.Spos();

  // rotate(input, 2)
  int32_t  rot_idx = 2;
  NODE_PTR n_input = cntr->New_ld(ir_gen.Input_var(), spos);
  NODE_PTR int2    = cntr->New_intconst(
      ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_S32), rot_idx, spos);
  NODE_PTR rotate_node1 =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ROTATE),
                          ir_gen.Ciph_ty(), spos);
  rotate_node1->Set_child(0, n_input);
  rotate_node1->Set_child(1, int2);
  rotate_node1->Set_attr("nums", &rot_idx, 1);

  // rotate(input, 3)
  n_input       = cntr->New_ld(ir_gen.Input_var(), spos);
  rot_idx       = 3;
  NODE_PTR int3 = cntr->New_intconst(
      ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_S32), rot_idx, spos);
  NODE_PTR rotate_node2 =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ROTATE),
                          ir_gen.Ciph_ty(), spos);
  rotate_node2->Set_child(0, n_input);
  rotate_node2->Set_child(1, int3);
  rotate_node2->Set_attr("nums", &rot_idx, 1);

  NODE_PTR n_add =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ADD),
                          ir_gen.Ciph_ty(), spos);

  n_add->Set_child(0, rotate_node1);
  n_add->Set_child(1, rotate_node2);

  STMT_PTR rotate_stmt = cntr->New_st(n_add, ir_gen.Output_var(), spos);
  sl.Append(rotate_stmt);

  ir_gen.Append_output();
}

void Gen_expected(std::ofstream& of) {
  Gen_msg_rotate(of);
  Gen_msg_add_ciph(of);
  std::string emit_str =
      "\
double *Get_exp_data(double *input, size_t input_len) { \n\
  double *rot_2 = Msg_rotate(input, input_len, 2); \n\
  double *rot_3 = Msg_rotate(input, input_len, 3); \n\
  double *add = Msg_add(rot_2, rot_3, input_len);\n\
  return add;\n\
}";
  of << emit_str << std::endl;
}
