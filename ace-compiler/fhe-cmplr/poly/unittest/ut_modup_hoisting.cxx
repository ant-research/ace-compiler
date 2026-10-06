//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "../include/poly_ir_gen.h"
#include "air/base/opcode.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/opcode.h"
#include "fhe/test/gen_ckks_ir.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace fhe::core;
using namespace fhe::poly;
namespace fhe {
namespace poly {
namespace test {

namespace {

struct OP_COUNT {
  uint32_t _precomp     = 0;
  uint32_t _dot_prod    = 0;
  uint32_t _mod_down    = 0;
  uint32_t _poly_rotate = 0;
};

void Count_ops(NODE_PTR node, OP_COUNT& cnt) {
  if (node == Null_ptr) return;
  if (node->Is_block()) {
    for (STMT_PTR s = node->Begin_stmt(); s != node->End_stmt(); s = s->Next()) {
      Count_ops(s->Node(), cnt);
    }
    return;
  }
  air::base::OPCODE opc = node->Opcode();
  if (opc == fhe::poly::OPC_PRECOMP) ++cnt._precomp;
  if (opc == fhe::poly::OPC_DOT_PROD) ++cnt._dot_prod;
  if (opc == fhe::poly::OPC_MOD_DOWN) ++cnt._mod_down;
  if (opc == fhe::poly::OPC_ROTATE) ++cnt._poly_rotate;
  for (uint32_t i = 0; i < node->Num_child(); ++i) {
    Count_ops(node->Child(i), cnt);
  }
}

OP_COUNT Count_in_glob(GLOB_SCOPE* glob) {
  OP_COUNT cnt;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    Count_ops(func->Container().Entry_node(), cnt);
  }
  return cnt;
}

void Set_rotate_level(STMT_PTR rotate_stmt, uint32_t lvl) {
  NODE_PTR rhs = rotate_stmt->Node()->Child(0);
  rhs->Set_attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL, &lvl, 1);
}

}  // namespace

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

TEST(TEST_MODUP_HOISTING, loop_hoisting1) {
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

TEST(TEST_MODUP_HOISTING, loop_hoisting2) {
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

//! Test that SSAPRE / modup-hoisting shares PRECOMP across two consecutive
//! loops that both rotate the same input ciphertext (the exact topology
//! produced by roll_simp Rule 3 loop splitting: loop1 over [0,4), literal-0
//! rot at index 4, loop2 over [5,9)).
//!
//! Pinning this in a unit test guarantees that future loop-splitting /
//! rotation-optimization changes do not silently duplicate PRECOMP work.
TEST(TEST_MODUP_HOISTING, split_loops_share_precomp) {
  air::util::STACKED_MEM_POOL<4096> pool;
  LOWER_CTX                         lower_ctx;
  lower_ctx.Get_ctx_param().Set_poly_degree(16, false);
  lower_ctx.Get_ctx_param().Set_mul_level(3, true);
  lower_ctx.Get_ctx_param().Set_q_part_num(2);
  CKKS_IR_GEN ckks_ir_gen(lower_ctx);
  CONTAINER*  cntr = ckks_ir_gen.Main_container();
  POLY_IR_GEN poly_ir_gen(cntr, &lower_ctx, &pool);
  STMT_LIST   sl = cntr->Stmt_list();

  ADDR_DATUM_PTR var_in = ckks_ir_gen.Input_var();
  ADDR_DATUM_PTR sum    = ckks_ir_gen.Gen_ciph_var("sum");
  NODE_PTR       n_zero =
      cntr->New_intconst(ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32),
                         0, ckks_ir_gen.Spos());
  STMT_PTR s_zero = cntr->New_st(n_zero, sum, ckks_ir_gen.Spos());
  sl.Append(s_zero);

  // Loop 1: for (i = 0; i < 4; i++) { sum += rotate(var_in, i); }
  ADDR_DATUM_PTR iv1  = ckks_ir_gen.Gen_int_var("iv1");
  NODE_PTR       ub1  = cntr->New_intconst(
      ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32), 4,
      ckks_ir_gen.Spos());
  STMT_PTR  loop1     = poly_ir_gen.New_loop(
      VAR(cntr->Parent_func_scope(), iv1), ub1, 0, 1, ckks_ir_gen.Spos());
  NODE_PTR  body1     = loop1->Node()->Child(3);
  STMT_LIST sl_body1  = STMT_LIST::Enclosing_list(body1->End_stmt());
  ADDR_DATUM_PTR rot_tmp1 = ckks_ir_gen.Gen_ciph_var("rot_tmp1");
  STMT_PTR s_rot1 =
      ckks_ir_gen.Gen_rotate(cntr, rot_tmp1, var_in, iv1, &sl_body1);
  Set_rotate_level(s_rot1, 3);
  ckks_ir_gen.Gen_add(cntr, sum, sum, rot_tmp1, &sl_body1);
  sl.Append(loop1);

  // Center tap: sum += rotate(var_in, 0)
  ADDR_DATUM_PTR rot_zero = ckks_ir_gen.Gen_ciph_var("rot_zero");
  ckks_ir_gen.Gen_rotate(cntr, rot_zero, var_in, 0, &sl);
  ckks_ir_gen.Gen_add(cntr, sum, sum, rot_zero, &sl);

  // Loop 2: for (i = 5; i < 9; i++) { sum += rotate(var_in, i); }
  ADDR_DATUM_PTR iv2  = ckks_ir_gen.Gen_int_var("iv2");
  NODE_PTR       ub2  = cntr->New_intconst(
      ckks_ir_gen.Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32), 9,
      ckks_ir_gen.Spos());
  STMT_PTR  loop2     = poly_ir_gen.New_loop(
      VAR(cntr->Parent_func_scope(), iv2), ub2, 5, 1, ckks_ir_gen.Spos());
  NODE_PTR  body2     = loop2->Node()->Child(3);
  STMT_LIST sl_body2  = STMT_LIST::Enclosing_list(body2->End_stmt());
  ADDR_DATUM_PTR rot_tmp2 = ckks_ir_gen.Gen_ciph_var("rot_tmp2");
  STMT_PTR s_rot2 =
      ckks_ir_gen.Gen_rotate(cntr, rot_tmp2, var_in, iv2, &sl_body2);
  Set_rotate_level(s_rot2, 3);
  ckks_ir_gen.Gen_add(cntr, sum, sum, rot_tmp2, &sl_body2);
  sl.Append(loop2);

  air::driver::DRIVER_CTX driver_ctx;
  fhe::poly::POLY_DRIVER  poly_driver;
  fhe::poly::POLY_CONFIG  poly_config;
  poly_config._inline_rotate  = true;
  poly_config._inline_relin   = true;
  poly_config._lower_to_hpoly = true;
  poly_config._lower_to_lpoly = false;
  poly_config._hoist_mdup     = true;

  GLOB_SCOPE* glob =
      poly_driver.Run(poly_config, cntr->Glob_scope(), lower_ctx, &driver_ctx);

  // PRECOMP on var_in should be hoisted above BOTH loops into a single
  // shared computation (exactly 1 PRECOMP node across the whole graph,
  // not 2).
  OP_COUNT cnt = Count_in_glob(glob);
  EXPECT_EQ(cnt._precomp, 1u);
  // Center tap rotate(x, 0) pays 0 dot-prods; each loop body has 1 rotate ->
  // 2 dot_prods per loop body. Total _dot_prod = 4.
  EXPECT_EQ(cnt._dot_prod, 4u);
  EXPECT_EQ(cnt._mod_down, 4u);
}

}  // namespace test
}  // namespace poly
}  // namespace fhe