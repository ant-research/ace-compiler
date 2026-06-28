//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "../include/poly_ir_gen.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/test/gen_ckks_ir.h"
#include "gtest/gtest.h"

using namespace fhe::core;
using namespace fhe::poly;
namespace fhe {
namespace poly {
namespace test {

TEST(TEST_MODUP_HOISTING, no_loop_hoisting) {
  LOWER_CTX lower_ctx;
  lower_ctx.Get_ctx_param().Set_poly_degree(16, false);
  lower_ctx.Get_ctx_param().Set_mul_level(3, true);
  lower_ctx.Get_ctx_param().Set_q_part_num(2);
  CKKS_IR_GEN ir_gen(lower_ctx);
  CONTAINER*  cntr = ir_gen.Main_container();
  STMT_LIST   sl   = cntr->Stmt_list();

  ADDR_DATUM_PTR var_1   = ir_gen.Input_var();
  ADDR_DATUM_PTR var_2   = ir_gen.Gen_ciph_var("ciph_rot1");
  ADDR_DATUM_PTR var_3   = ir_gen.Gen_ciph_var("ciph_rot2");
  STMT_PTR       sr_rot1 = ir_gen.Gen_rotate(cntr, var_2, var_1, 2);
  STMT_PTR       sr_rot2 = ir_gen.Gen_rotate(cntr, var_3, var_1, 3);
  STMT_PTR sr_add = ir_gen.Gen_add(cntr, ir_gen.Output_var(), var_2, var_3);
  sl.Append(sr_rot1);
  sl.Append(sr_rot2);
  sl.Append(sr_add);

  // ir_gen.Append_output();
  cntr->Print();

  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;
  poly_config._inline_rotate  = true;
  poly_config._inline_relin   = true;
  poly_config._lower_to_hpoly = true;
  poly_config._lower_to_lpoly = true;
  poly_config._hoist_mdup     = true;
  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), lower_ctx, &driver_ctx);
  // glob->Print();

  // hoisting_opt.perform(cntr);
}

TEST(TEST_MODUP_HOISTING, DISABLED_loop_hoisting1) {
  air::util::STACKED_MEM_POOL<4096> pool;
  uint32_t                          ub_value = 10;
  LOWER_CTX                         lower_ctx;
  lower_ctx.Get_ctx_param().Set_poly_degree(16, false);
  lower_ctx.Get_ctx_param().Set_mul_level(3, true);
  lower_ctx.Get_ctx_param().Set_q_part_num(2);
  CKKS_IR_GEN ckks_ir_gen(lower_ctx);
  CONTAINER*  cntr = ckks_ir_gen.Main_container();
  POLY_IR_GEN poly_ir_gen(cntr, &lower_ctx, &pool);
  STMT_LIST   sl = cntr->Stmt_list();

  // sum = 0
  ADDR_DATUM_PTR sum = ckks_ir_gen.Gen_ciph_var("sum");
  NODE_PTR       n_zero =
      cntr->New_intconst(ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32),
                         0, ckks_ir_gen.Spos());
  STMT_PTR s_zero = cntr->New_st(n_zero, sum, ckks_ir_gen.Spos());

  // for(i = 0; i < ub_value, i++) {}
  ADDR_DATUM_PTR ind_var = ckks_ir_gen.Gen_int_var("i");
  NODE_PTR       n_ub =
      cntr->New_intconst(ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32),
                         ub_value, ckks_ir_gen.Spos());
  STMT_PTR s_do_loop = poly_ir_gen.New_loop(
      VAR(cntr->Parent_func_scope(), ind_var), n_ub, 0, 1, ckks_ir_gen.Spos());
  NODE_PTR n_loop_body = s_do_loop->Node()->Child(3);
  CMPLR_ASSERT(n_loop_body->Is_block(), "not a block node");
  STMT_LIST sl_body = STMT_LIST::Enclosing_list(n_loop_body->End_stmt());

  // ciph_2 = rotate(input, i)
  // sum = sum + ciph2
  ADDR_DATUM_PTR var_1 = ckks_ir_gen.Input_var();
  ADDR_DATUM_PTR var_2 = ckks_ir_gen.Gen_ciph_var("ciph_2");
  STMT_PTR       sr_rot =
      ckks_ir_gen.Gen_rotate(cntr, var_2, var_1, ind_var, &sl_body);
  STMT_PTR sr_add = ckks_ir_gen.Gen_add(cntr, sum, sum, var_2, &sl_body);

  sl.Append(s_zero);
  sl.Append(s_do_loop);
  cntr->Print();

  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;

  poly_config._inline_rotate  = true;
  poly_config._inline_relin   = true;
  poly_config._lower_to_hpoly = true;
  poly_config._lower_to_lpoly = true;
  poly_config._hoist_mdup     = true;

  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), lower_ctx, &driver_ctx);
  // hoisting_opt.perform(cntr);
}

TEST(TEST_MODUP_HOISTING, DISABLED_loop_hoisting2) {
  air::util::STACKED_MEM_POOL<4096> pool;
  uint32_t                          ub_value = 10;
  LOWER_CTX                         lower_ctx;
  lower_ctx.Get_ctx_param().Set_poly_degree(16, false);
  lower_ctx.Get_ctx_param().Set_mul_level(3, true);
  lower_ctx.Get_ctx_param().Set_q_part_num(2);
  CKKS_IR_GEN ckks_ir_gen(lower_ctx);
  CONTAINER*  cntr = ckks_ir_gen.Main_container();
  POLY_IR_GEN poly_ir_gen(cntr, &lower_ctx, &pool);
  STMT_LIST   sl = cntr->Stmt_list();

  // sum = 0
  ADDR_DATUM_PTR sum = ckks_ir_gen.Gen_ciph_var("sum");
  NODE_PTR       n_zero =
      cntr->New_intconst(ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32),
                         0, ckks_ir_gen.Spos());
  STMT_PTR s_zero = cntr->New_st(n_zero, sum, ckks_ir_gen.Spos());

  // for(i = 0; i < ub_value, i++) {}
  ADDR_DATUM_PTR ind_var = ckks_ir_gen.Gen_int_var("i");
  NODE_PTR       n_ub =
      cntr->New_intconst(ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32),
                         ub_value, ckks_ir_gen.Spos());
  STMT_PTR s_do_loop = poly_ir_gen.New_loop(
      VAR(cntr->Parent_func_scope(), ind_var), n_ub, 0, 1, ckks_ir_gen.Spos());
  NODE_PTR n_loop_body = s_do_loop->Node()->Child(3);
  CMPLR_ASSERT(n_loop_body->Is_block(), "not a block node");
  STMT_LIST sl_body = STMT_LIST::Enclosing_list(n_loop_body->End_stmt());

  // ciph_2 = input * plain
  // ciph_3 = rotate(ciph_2, idx)
  // sum = sum + ciph_3
  ADDR_DATUM_PTR var_1     = ckks_ir_gen.Input_var();
  ADDR_DATUM_PTR var_2     = ckks_ir_gen.Gen_ciph_var("ciph_2");
  ADDR_DATUM_PTR var_3     = ckks_ir_gen.Gen_ciph_var("ciph_3");
  ADDR_DATUM_PTR var_plain = ckks_ir_gen.Gen_plain_var("plain");
  STMT_PTR       sr_encode = ckks_ir_gen.Gen_encode(cntr, var_plain);

  STMT_PTR sr_mul =
      ckks_ir_gen.Gen_mul(cntr, var_2, var_1, var_plain, &sl_body);
  STMT_PTR sr_rot =
      ckks_ir_gen.Gen_rotate(cntr, var_3, var_2, ind_var, &sl_body);
  STMT_PTR sr_add = ckks_ir_gen.Gen_add(cntr, sum, sum, var_3, &sl_body);

  sl.Append(s_zero);
  sl.Append(s_do_loop);
  cntr->Print();

  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;

  poly_config._inline_rotate  = true;
  poly_config._inline_relin   = true;
  poly_config._lower_to_hpoly = true;
  poly_config._lower_to_lpoly = true;
  poly_config._hoist_mdup     = true;

  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), lower_ctx, &driver_ctx);
  // hoisting_opt.perform(cntr);
}

}  // namespace test
}  // namespace poly
}  // namespace fhe