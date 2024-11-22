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

  CONTAINER* cntr = ir_gen.Container();

  fhe::poly::POLY_DRIVER poly_driver;
  fhe::poly::POLY_CONFIG poly_config;
  // inline rotate IR
  poly_config.Set_inline_rotate(true);

  GLOB_SCOPE* glob = poly_driver.Run(poly_config, cntr->Glob_scope(), fhe_ctx);

  std::ofstream            of("test_rotate_01.inc");
  fhe::poly::POLY2C_CONFIG p2c_config;
  fhe::poly::POLY2C_DRIVER poly2c(of, fhe_ctx, p2c_config);
  poly2c.Run(glob);

  Gen_expected(of);
  std::cout << "Output: test_rotate_01.inc" << std::endl;
  return 0;
}

// output = rotate(input, 2)
void Create_ckks_ir(CKKS_IR_GEN& ir_gen) {
  CONTAINER* cntr = ir_gen.Container();
  STMT_LIST  sl   = cntr->Stmt_list();
  SPOS       spos = ir_gen.Spos();

  NODE_PTR n_input = cntr->New_ld(ir_gen.Input_var(), spos);
  NODE_PTR int2    = cntr->New_intconst(
      ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_S32), Rot_idx, spos);
  NODE_PTR rotate_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ROTATE),
                          ir_gen.Ciph_ty(), spos);
  rotate_node->Set_child(0, n_input);
  rotate_node->Set_child(1, int2);
  rotate_node->Set_attr("nums", &Rot_idx, 1);
  STMT_PTR rotate_stmt = cntr->New_st(rotate_node, ir_gen.Output_var(), spos);
  sl.Append(rotate_stmt);

  ir_gen.Append_output();
}

void Gen_expected(std::ofstream& of) {
  Gen_msg_rotate(of);
  std::string emit_str =
      "\
double *Get_exp_data(double *input, size_t input_len) { \n\
  double *rot = Msg_rotate(input, input_len, Rot_idx); \n\
  return rot;\n\
}";
  of << "int32_t Rot_idx = " << Rot_idx << ";" << std::endl;
  of << emit_str << std::endl;
}
