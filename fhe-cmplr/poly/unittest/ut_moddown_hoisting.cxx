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

TEST(TEST_MODDOWN_HOISTING, no_loop_hoisting) {
  LOWER_CTX lower_ctx;
  lower_ctx.Get_ctx_param().Set_poly_degree(16, false);
  lower_ctx.Get_ctx_param().Set_mul_level(3, true);
  lower_ctx.Get_ctx_param().Set_q_part_num(2);
  CKKS_IR_GEN ir_gen(lower_ctx);
  CONTAINER*  cntr = ir_gen.Main_container();
  STMT_LIST   sl   = cntr->Stmt_list();

  ADDR_DATUM_PTR var_1   = ir_gen.Input_var();
  ADDR_DATUM_PTR var_2   = ir_gen.Gen_ciph_var("ciph_2");
  ADDR_DATUM_PTR var_3   = ir_gen.Gen_ciph_var("ciph_3");
  ADDR_DATUM_PTR var_4   = ir_gen.Gen_ciph_var("ciph_4");
  STMT_PTR       sr_rot1 = ir_gen.Gen_rotate(cntr, var_2, var_1, 2);
  STMT_PTR       sr_rot2 = ir_gen.Gen_rotate(cntr, var_3, var_1, 3);
  STMT_PTR       sr_add  = ir_gen.Gen_add(cntr, var_4, var_2, var_3);
  sl.Append(sr_rot1);
  sl.Append(sr_rot2);
  sl.Append(sr_add);

  cntr->Print();
  // hoisting_opt.perform(cntr);

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
}

// Sum(rotate(ciph_i, i))
TEST(TEST_MODDOWN_HOISTING, loop_hoisting1) {
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

  // ciph_2 = rotate(ciph_1, i)
  // sum = sum + ciph2
  std::vector<ADDR_DATUM_PTR> var_list;
  for (uint32_t i = 0; i < ub_value; i++) {
    std::string name_i = "ciph_";
    name_i.append(std::to_string(i));
    var_list.push_back(ckks_ir_gen.Gen_ciph_var(name_i.c_str()));
  }
  ADDR_DATUM_PTR var_i   = ckks_ir_gen.Gen_ciph_var("ciph_i");
  ADDR_DATUM_PTR var_rot = ckks_ir_gen.Gen_ciph_var("ciph_rot");
  STMT_PTR       sr_rot =
      ckks_ir_gen.Gen_rotate(cntr, var_rot, var_i, ind_var, &sl_body);
  STMT_PTR sr_add = ckks_ir_gen.Gen_add(cntr, sum, sum, var_rot, &sl_body);

  sl.Append(s_zero);
  sl.Append(s_do_loop);
  cntr->Print();
  // hoisting_opt.perform(cntr);

  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;

  poly_config._inline_rotate  = true;
  poly_config._inline_relin   = true;
  poly_config._lower_to_hpoly = true;
  poly_config._lower_to_lpoly = true;
  poly_config._hoist_mdup     = false;
  poly_config._hoist_mdown    = true;

  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), lower_ctx, &driver_ctx);
}

// Sum(rotate(ciph_i, i) * plain_i)
TEST(TEST_MODDOWN_HOISTING, DISABLED_loop_hoisting2) {
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

  // ciph_rot = rotate(ciph_i, i)
  // ciph_mul = ciph_rot * plain_i
  // sum = sum + ciph_mul
  std::vector<ADDR_DATUM_PTR> var_list;
  for (uint32_t i = 0; i < ub_value; i++) {
    std::string name_i = "ciph_";
    name_i.append(std::to_string(i));
    var_list.push_back(ckks_ir_gen.Gen_ciph_var(name_i.c_str()));
  }
  ADDR_DATUM_PTR var_i       = ckks_ir_gen.Gen_ciph_var("ciph_i");
  ADDR_DATUM_PTR var_rot     = ckks_ir_gen.Gen_ciph_var("ciph_rot");
  ADDR_DATUM_PTR var_plain_i = ckks_ir_gen.Gen_plain_var("plain_i");
  ADDR_DATUM_PTR var_mul     = ckks_ir_gen.Gen_ciph_var("ciph_mul");
  STMT_PTR       sr_rot =
      ckks_ir_gen.Gen_rotate(cntr, var_rot, var_i, ind_var, &sl_body);
  STMT_PTR sr_mul =
      ckks_ir_gen.Gen_mul(cntr, var_mul, var_rot, var_plain_i, &sl_body);
  STMT_PTR sr_add = ckks_ir_gen.Gen_add(cntr, sum, sum, var_mul, &sl_body);

  sl.Append(s_zero);
  sl.Append(s_do_loop);
  cntr->Print();
  // hoisting_opt.perform(cntr);

  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;

  poly_config._inline_rotate  = true;
  poly_config._inline_relin   = true;
  poly_config._lower_to_hpoly = true;
  poly_config._lower_to_lpoly = true;
  poly_config._hoist_mdown    = true;

  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), lower_ctx, &driver_ctx);
}

}  // namespace test
}  // namespace poly
}  // namespace fhe