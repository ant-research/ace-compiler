//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <limits.h>

#include "fhe/core/ctx_param_ana.h"
#include "fhe/poly/poly2c_driver.h"
#include "fhe/test/gen_ckks_ir.h"
#include "gen_expect_data.h"
#include "nn/core/attr.h"

using namespace air::base;
using namespace air::util;
using namespace fhe::core;
using namespace fhe::ckks;
using namespace fhe::sihe;
using namespace fhe::poly;
using namespace fhe::poly::test;

void Create_ckks_ir(CKKS_IR_GEN& ir_gen);

// TEST PURPOSE: rotate called in multiple function
int main(int argc, char** argv) {
  fhe::core::LOWER_CTX fhe_ctx;
  CKKS_IR_GEN          ir_gen(fhe_ctx);
  Create_ckks_ir(ir_gen);

  CONTAINER*              cntr = ir_gen.Main_container();
  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;

  poly_config.Pre_process_options();
  // do not inline rotate IR, generate into a new function
  poly_config._inline_rotate = false;

  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), fhe_ctx, &driver_ctx);

  std::ofstream            of("test_rotate_04.inc");
  fhe::poly::POLY2C_CONFIG p2c_config;
  fhe::poly::POLY2C_DRIVER poly2c(of, fhe_ctx, p2c_config);
  POLY2C_VISITOR           visitor(poly2c.Ctx());
  poly2c.Run(glob, visitor);
  Gen_expected(of);
  std::cout << "Output: test_rotate_04.inc" << std::endl;
  return 0;
}

PREG_PTR Gen_call_rotate(CKKS_IR_GEN& ir_gen, const char* fname,
                         ADDR_DATUM_PTR var, int32_t rot_idx) {
  SPOS                  spos = ir_gen.Spos();
  std::vector<TYPE_PTR> param_types;
  param_types.push_back(ir_gen.Ciph_ty());
  param_types.push_back(ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_S32));

  FUNC_SCOPE* fs   = ir_gen.Gen_func(fname, ir_gen.Ciph_ty(), param_types);
  CONTAINER*  cntr = &(fs->Container());

  // Gen function body
  NODE_PTR n_input   = cntr->New_ld(fs->Formal(0), spos);
  NODE_PTR n_rot_idx = cntr->New_ld(fs->Formal(1), spos);
  NODE_PTR n_rotate =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ROTATE),
                          ir_gen.Ciph_ty(), spos);
  n_rotate->Set_child(0, n_input);
  n_rotate->Set_child(1, n_rot_idx);
  n_rotate->Set_attr(nn::core::ATTR::RNUM, &rot_idx, 1);

  ADDR_DATUM_PTR output   = fs->New_var(ir_gen.Ciph_ty(), "output", spos);
  STMT_PTR       s_rotate = cntr->New_st(n_rotate, output, spos);

  NODE_PTR n_retv = cntr->New_ld(output, spos);
  STMT_PTR s_retv = cntr->New_retv(n_retv, spos);
  cntr->Stmt_list().Append(s_rotate);
  cntr->Stmt_list().Append(s_retv);
  // analyze ckks parameters for rotate function
  ir_gen.Analyze_ckks_params(fs, 16);

  // back to main_graph
  CONTAINER*  main_cntr   = ir_gen.Main_container();
  FUNC_SCOPE* main_fscope = main_cntr->Parent_func_scope();
  PREG_PTR    retv        = main_fscope->New_preg(ir_gen.Ciph_ty());
  STMT_PTR    s_call =
      main_cntr->New_call(fs->Owning_func()->Entry_point(), retv, 2, spos);
  const uint32_t depth = 1;
  s_call->Node()->Set_attr(FHE_ATTR_KIND::MUL_DEPTH, &depth, 1);
  main_cntr->New_arg(s_call, 0, main_cntr->New_ld(var, spos));
  main_cntr->New_arg(
      s_call, 1,
      main_cntr->New_intconst(ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_S32),
                              rot_idx, spos));

  STMT_LIST sl = main_cntr->Stmt_list();
  sl.Append(s_call);
  return retv;
}

// output = roate1(input, 2) + rotate2(input, 3)
void Create_ckks_ir(CKKS_IR_GEN& ir_gen) {
  CONTAINER* cntr = ir_gen.Main_container();
  STMT_LIST  sl   = cntr->Stmt_list();
  SPOS       spos = ir_gen.Spos();

  PREG_PTR retv1 = Gen_call_rotate(ir_gen, "rotate1", ir_gen.Input_var(), 2);
  PREG_PTR retv2 = Gen_call_rotate(ir_gen, "rotate2", ir_gen.Input_var(), 3);

  NODE_PTR n_add =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ADD),
                          ir_gen.Ciph_ty(), spos);

  n_add->Set_child(0, cntr->New_ldp(retv1, spos));
  n_add->Set_child(1, cntr->New_ldp(retv2, spos));

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
