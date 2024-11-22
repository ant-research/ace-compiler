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

void Create_ckks_ir(CKKS_IR_GEN& ir_gen);

int main(int argc, char** argv) {
  fhe::core::LOWER_CTX fhe_ctx;
  CKKS_IR_GEN          ir_gen(fhe_ctx);
  Create_ckks_ir(ir_gen);

  CONTAINER* cntr = ir_gen.Container();

  fhe::poly::POLY_DRIVER poly_driver;
  fhe::poly::POLY_CONFIG poly_config;
  // inline relin IR
  poly_config.Set_inline_relin(true);

  GLOB_SCOPE* glob = poly_driver.Run(poly_config, cntr->Glob_scope(), fhe_ctx);

  std::ofstream            of("test_relin_01.inc");
  fhe::poly::POLY2C_CONFIG p2c_config;
  fhe::poly::POLY2C_DRIVER poly2c(of, fhe_ctx, p2c_config);
  poly2c.Run(glob);
  Gen_expected(of);
  std::cout << "Output: test_relin_01.inc" << std::endl;
  return 0;
}

// output = relin(mul(input, input))
void Create_ckks_ir(CKKS_IR_GEN& ir_gen) {
  CONTAINER* cntr = ir_gen.Container();
  STMT_LIST  sl   = cntr->Stmt_list();
  SPOS       spos = ir_gen.Spos();

  NODE_PTR n_input = cntr->New_ld(ir_gen.Input_var(), spos);
  NODE_PTR n_mul =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::MUL),
                          ir_gen.Ciph3_ty(), spos);
  n_mul->Set_child(0, n_input);
  n_mul->Set_child(1, n_input);

  NODE_PTR n_relin =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::RELIN),
                          ir_gen.Ciph_ty(), spos);
  n_relin->Set_child(0, n_mul);

  STMT_PTR s_relin = cntr->New_st(n_relin, ir_gen.Output_var(), spos);
  sl.Append(s_relin);

  ir_gen.Append_output();
}

void Gen_expected(std::ofstream& of) {
  Gen_msg_mul_ciph(of);
  std::string emit_str =
      "\
double *Get_exp_data(double *input, size_t input_len) { \n\
  double *res = Msg_mul_ciph(input, input, input_len); \n\
  return res;\n\
}";
  of << emit_str << std::endl;
}
