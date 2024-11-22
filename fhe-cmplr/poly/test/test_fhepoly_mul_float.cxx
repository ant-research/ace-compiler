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

float Float_data = 3.0;
void  Create_ckks_ir(CKKS_IR_GEN& ir_gen);

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

  std::ofstream            of("test_mul_float.inc");
  fhe::poly::POLY2C_CONFIG p2c_config;
  fhe::poly::POLY2C_DRIVER poly2c(of, fhe_ctx, p2c_config);
  poly2c.Run(glob);
  Gen_expected(of);
  std::cout << "Output: test_mul_float.inc" << std::endl;
  return 0;
}

// output = mul(input, 3.0)
void Create_ckks_ir(CKKS_IR_GEN& ir_gen) {
  CONTAINER* cntr = ir_gen.Container();
  STMT_LIST  sl   = cntr->Stmt_list();
  SPOS       spos = ir_gen.Spos();

  NODE_PTR     n_input   = cntr->New_ld(ir_gen.Input_var(), spos);
  CONSTANT_PTR cst_float = cntr->Glob_scope()->New_const(
      CONSTANT_KIND::FLOAT,
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_32),
      (long double)Float_data);
  NODE_PTR f_node = cntr->New_ldc(cst_float, spos);
  NODE_PTR mul_node =
      cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::MUL),
                          n_input, f_node, spos);
  STMT_PTR mul_stmt = cntr->New_st(mul_node, ir_gen.Output_var(), spos);
  sl.Append(mul_stmt);

  ir_gen.Append_output();
}

void Gen_expected(std::ofstream& of) {
  Gen_msg_mul_float(of);
  std::string emit_str =
      "\
double *Get_exp_data(double *input, size_t input_len) { \n\
  double *rot = Msg_mul_float(input, input_len, f); \n\
  return rot;\n\
}";
  of << "float f = " << Float_data << ";" << std::endl;
  of << emit_str << std::endl;
}
