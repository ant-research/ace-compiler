//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"
#include "gtest/gtest.h"
#include "nn/core/attr.h"
#include "nn/core/default_handler.h"
#include "nn/vector/config.h"
#include "nn/vector/handler.h"
#include "nn/vector/roll_simp.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"

using namespace air::base;
using namespace nn::vector;

namespace {

// Domains can only be registered once per process; guard so every test is
// self-contained regardless of execution order or gtest filter.
void Ensure_domains_registered() {
  static bool registered = false;
  if (registered) return;
  ASSERT_TRUE(air::core::Register_core());
  ASSERT_TRUE(nn::core::Register_nn());
  ASSERT_TRUE(Register_vector_domain());
  registered = true;
}

struct OP_STAT {
  uint32_t _num_roll = 0;
  uint32_t _num_loop = 0;
  uint32_t _num_ist  = 0;
};

void Count_ops(NODE_PTR node, OP_STAT& stat) {
  if (node == Null_ptr) return;
  air::base::OPCODE opc = node->Opcode();
  if (opc == OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL)) {
    stat._num_roll++;
  } else if (opc == air::core::OPC_DO_LOOP) {
    stat._num_loop++;
  } else if (opc == air::core::OPC_IST) {
    stat._num_ist++;
  }
  // Statements live in a block's stmt list, not in its child list
  // (BLOCK has kid_num == 0), so blocks must be traversed explicitly.
  if (node->Is_block()) {
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      Count_ops(stmt->Node(), stat);
    }
  }
  for (uint32_t i = 0; i < node->Num_child(); ++i) {
    Count_ops(node->Child(i), stat);
  }
}

// Count ops only in the function this test created. GLOB_SCOPE::Get() is a
// process-wide singleton, so functions from earlier tests are still present
// in the optimized glob and must not pollute the counts.
OP_STAT Count_in_func(GLOB_SCOPE* glob, FUNC_ID fid) {
  OP_STAT     stat;
  FUNC_SCOPE* fscope = &glob->Open_func_scope(fid);
  Count_ops(fscope->Container().Entry_node(), stat);
  return stat;
}

}  // namespace

TEST(TEST_ROLL_SIMP, LiteralZeroRoll) {
  Ensure_domains_registered();

  GLOB_SCOPE* gscope = GLOB_SCOPE::Get();
  SPOS        spos   = gscope->Unknown_simple_spos();

  STR_PTR  name_str  = gscope->New_str("Test_LiteralZeroRoll");
  FUNC_PTR main_func = gscope->New_func(name_str, spos);
  main_func->Set_parent(gscope->Comp_env_id());
  SIGNATURE_TYPE_PTR sig       = gscope->New_sig_type();
  TYPE_PTR           elem_type = gscope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  gscope->New_ret_param(elem_type, sig);
  sig->Set_complete();
  gscope->New_global_entry_point(sig, main_func, name_str, spos);

  FUNC_SCOPE* fscope = &gscope->New_func_scope(main_func);
  CONTAINER&  cntr   = fscope->Container();
  cntr.New_func_entry(spos);

  std::vector<int64_t> shape{16};
  ARRAY_TYPE_PTR arr_type =
      gscope->New_arr_type(gscope->New_str("arr_type"), elem_type, shape, spos);

  ADDR_DATUM_PTR var_in  = fscope->New_var(arr_type, "var_in", spos);
  ADDR_DATUM_PTR var_out = fscope->New_var(arr_type, "var_out", spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // var_out = roll(var_in, 0)
  NODE_PTR ld_in = cntr.New_ld(var_in, spos);
  NODE_PTR shift = cntr.New_intconst(s32_type, 0, spos);
  NODE_PTR roll_node = cntr.New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL), arr_type,
      spos);
  roll_node->Set_child(0, ld_in);
  roll_node->Set_child(1, shift);

  STMT_PTR st = cntr.New_st(roll_node, var_out, spos);
  cntr.Stmt_list().Append(st);

  VECTOR_CTX              ctx;
  VECTOR_CONFIG           cfg;
  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE* opt_glob = Roll_simp_opt(gscope, ctx, &driver_ctx, cfg);

  OP_STAT stat = Count_in_func(opt_glob, fscope->Id());
  EXPECT_EQ(stat._num_roll, 0u);
}

TEST(TEST_ROLL_SIMP, NonZeroRollPreserved) {
  Ensure_domains_registered();

  GLOB_SCOPE* gscope = GLOB_SCOPE::Get();
  SPOS        spos   = gscope->Unknown_simple_spos();

  STR_PTR  name_str  = gscope->New_str("Test_NonZeroRoll");
  FUNC_PTR main_func = gscope->New_func(name_str, spos);
  main_func->Set_parent(gscope->Comp_env_id());
  SIGNATURE_TYPE_PTR sig       = gscope->New_sig_type();
  TYPE_PTR           elem_type = gscope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  gscope->New_ret_param(elem_type, sig);
  sig->Set_complete();
  gscope->New_global_entry_point(sig, main_func, name_str, spos);

  FUNC_SCOPE* fscope = &gscope->New_func_scope(main_func);
  CONTAINER&  cntr   = fscope->Container();
  cntr.New_func_entry(spos);

  std::vector<int64_t> shape{16};
  ARRAY_TYPE_PTR arr_type = gscope->New_arr_type(
      gscope->New_str("arr_type_nz"), elem_type, shape, spos);

  ADDR_DATUM_PTR var_in  = fscope->New_var(arr_type, "var_in_nz", spos);
  ADDR_DATUM_PTR var_out = fscope->New_var(arr_type, "var_out_nz", spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // var_out = roll(var_in, 5) with RNUM {5}
  int roll_num = 5;
  NODE_PTR roll_node = cntr.New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL), arr_type,
      spos);
  roll_node->Set_child(0, cntr.New_ld(var_in, spos));
  roll_node->Set_child(1, cntr.New_intconst(s32_type, 5, spos));
  roll_node->Set_attr(nn::core::ATTR::RNUM, &roll_num, 1);

  STMT_PTR st = cntr.New_st(roll_node, var_out, spos);
  cntr.Stmt_list().Append(st);

  VECTOR_CTX              ctx;
  VECTOR_CONFIG           cfg;
  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE* opt_glob = Roll_simp_opt(gscope, ctx, &driver_ctx, cfg);

  OP_STAT stat = Count_in_func(opt_glob, fscope->Id());
  EXPECT_EQ(stat._num_roll, 1u);
}

TEST(TEST_ROLL_SIMP, AllZero1x1ConvLoop) {
  Ensure_domains_registered();

  GLOB_SCOPE* gscope = GLOB_SCOPE::Get();
  SPOS        spos   = gscope->Unknown_simple_spos();

  STR_PTR  name_str  = gscope->New_str("Test_1x1Conv");
  FUNC_PTR main_func = gscope->New_func(name_str, spos);
  main_func->Set_parent(gscope->Comp_env_id());
  SIGNATURE_TYPE_PTR sig       = gscope->New_sig_type();
  TYPE_PTR           elem_type = gscope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  gscope->New_ret_param(elem_type, sig);
  sig->Set_complete();
  gscope->New_global_entry_point(sig, main_func, name_str, spos);

  FUNC_SCOPE* fscope = &gscope->New_func_scope(main_func);
  CONTAINER&  cntr   = fscope->Container();
  cntr.New_func_entry(spos);

  std::vector<int64_t> elem_shape{16};
  ARRAY_TYPE_PTR elem_arr_type = gscope->New_arr_type(
      gscope->New_str("elem_arr"), elem_type, elem_shape, spos);

  std::vector<int64_t> grid_shape{1};
  ARRAY_TYPE_PTR grid_arr_type = gscope->New_arr_type(
      gscope->New_str("grid_arr"), elem_arr_type, grid_shape, spos);

  ADDR_DATUM_PTR input_dup_var  = fscope->New_var(elem_arr_type, "input_dup", spos);
  ADDR_DATUM_PTR input_grid_var = fscope->New_var(grid_arr_type, "input_grid", spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  ADDR_DATUM_PTR iv_var   = fscope->New_var(s32_type, "bs_input", spos);

  // for (bs_input = 0; bs_input < 1; bs_input++)
  NODE_PTR init_node = cntr.New_intconst(s32_type, 0, spos);
  NODE_PTR comp_node = cntr.New_bin_arith(air::core::OPC_LT, s32_type,
                                          cntr.New_ld(iv_var, spos),
                                          cntr.New_intconst(s32_type, 1, spos), spos);
  NODE_PTR incr_node = cntr.New_bin_arith(air::core::OPC_ADD, s32_type,
                                          cntr.New_ld(iv_var, spos),
                                          cntr.New_intconst(s32_type, 1, spos), spos);
  NODE_PTR body_blk = cntr.New_stmt_block(spos);
  STMT_PTR loop_stmt = cntr.New_do_loop(iv_var, init_node, comp_node, incr_node, body_blk, spos);

  // Body: input_grid[bs_input] = roll(input_dup, ra[bs_input]) where ra={0}
  std::vector<int> ra{0};
  std::vector<int64_t> ra_shape{1};
  ARRAY_TYPE_PTR ra_type = gscope->New_arr_type(
      gscope->New_str("ra_type1"), s32_type, ra_shape, spos);
  CONSTANT_PTR ra_const = gscope->New_const(CONSTANT_KIND::ARRAY, ra_type, (void*)ra.data(), 4);
  NODE_PTR ra_array = cntr.New_array(cntr.New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr.Set_array_idx(ra_array, 0, cntr.New_ld(iv_var, spos));
  NODE_PTR ild_ra = cntr.New_ild(ra_array, spos);

  NODE_PTR roll_node = cntr.New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL),
      elem_arr_type, spos);
  roll_node->Set_child(0, cntr.New_ld(input_dup_var, spos));
  roll_node->Set_child(1, ild_ra);
  roll_node->Set_attr(nn::core::ATTR::RNUM, ra.data(), 1);

  NODE_PTR grid_i = cntr.New_array(cntr.New_lda(input_grid_var, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr.Set_array_idx(grid_i, 0, cntr.New_ld(iv_var, spos));
  STMT_PTR ist = cntr.New_ist(grid_i, roll_node, spos);

  STMT_LIST body_sl = STMT_LIST::Enclosing_list(body_blk->End_stmt());
  body_sl.Append(ist);

  cntr.Stmt_list().Append(loop_stmt);

  VECTOR_CTX              ctx;
  VECTOR_CONFIG           cfg;
  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE* opt_glob = Roll_simp_opt(gscope, ctx, &driver_ctx, cfg);

  OP_STAT stat = Count_in_func(opt_glob, fscope->Id());
  // The loop is kept (downstream scale/level analysis depends on loop
  // structure); the roll's shift is rewritten to a literal 0 so the
  // keyswitch is eliminated later at the POLY stage (Rotate_identity).
  EXPECT_EQ(stat._num_loop, 1u);
  EXPECT_EQ(stat._num_roll, 1u);
  EXPECT_EQ(stat._num_ist, 1u);
}

TEST(TEST_ROLL_SIMP, CenterTap3x3ConvLoop) {
  Ensure_domains_registered();

  GLOB_SCOPE* gscope = GLOB_SCOPE::Get();
  SPOS        spos   = gscope->Unknown_simple_spos();

  STR_PTR  name_str  = gscope->New_str("Test_3x3Conv");
  FUNC_PTR main_func = gscope->New_func(name_str, spos);
  main_func->Set_parent(gscope->Comp_env_id());
  SIGNATURE_TYPE_PTR sig       = gscope->New_sig_type();
  TYPE_PTR           elem_type = gscope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  gscope->New_ret_param(elem_type, sig);
  sig->Set_complete();
  gscope->New_global_entry_point(sig, main_func, name_str, spos);

  FUNC_SCOPE* fscope = &gscope->New_func_scope(main_func);
  CONTAINER&  cntr   = fscope->Container();
  cntr.New_func_entry(spos);

  std::vector<int64_t> elem_shape{16};
  ARRAY_TYPE_PTR elem_arr_type = gscope->New_arr_type(
      gscope->New_str("elem_arr3"), elem_type, elem_shape, spos);

  std::vector<int64_t> grid_shape{9};
  ARRAY_TYPE_PTR grid_arr_type = gscope->New_arr_type(
      gscope->New_str("grid_arr3"), elem_arr_type, grid_shape, spos);

  ADDR_DATUM_PTR input_dup_var  = fscope->New_var(elem_arr_type, "input_dup", spos);
  ADDR_DATUM_PTR input_grid_var = fscope->New_var(grid_arr_type, "input_grid", spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  ADDR_DATUM_PTR iv_var   = fscope->New_var(s32_type, "bs_input", spos);

  // for (bs_input = 0; bs_input < 9; bs_input++)
  NODE_PTR init_node = cntr.New_intconst(s32_type, 0, spos);
  NODE_PTR comp_node = cntr.New_bin_arith(air::core::OPC_LT, s32_type,
                                          cntr.New_ld(iv_var, spos),
                                          cntr.New_intconst(s32_type, 9, spos), spos);
  NODE_PTR incr_node = cntr.New_bin_arith(air::core::OPC_ADD, s32_type,
                                          cntr.New_ld(iv_var, spos),
                                          cntr.New_intconst(s32_type, 1, spos), spos);
  NODE_PTR body_blk = cntr.New_stmt_block(spos);
  STMT_PTR loop_stmt = cntr.New_do_loop(iv_var, init_node, comp_node, incr_node, body_blk, spos);

  // Body: ra = {-33, -32, -31, -1, 0, 1, 31, 32, 33}
  std::vector<int> ra{-33, -32, -31, -1, 0, 1, 31, 32, 33};
  std::vector<int64_t> ra_shape{9};
  ARRAY_TYPE_PTR ra_type = gscope->New_arr_type(
      gscope->New_str("ra_type9"), s32_type, ra_shape, spos);
  CONSTANT_PTR ra_const = gscope->New_const(CONSTANT_KIND::ARRAY, ra_type, (void*)ra.data(), 4 * ra.size());
  NODE_PTR ra_array = cntr.New_array(cntr.New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr.Set_array_idx(ra_array, 0, cntr.New_ld(iv_var, spos));
  NODE_PTR ild_ra = cntr.New_ild(ra_array, spos);

  NODE_PTR roll_node = cntr.New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL),
      elem_arr_type, spos);
  roll_node->Set_child(0, cntr.New_ld(input_dup_var, spos));
  roll_node->Set_child(1, ild_ra);
  roll_node->Set_attr(nn::core::ATTR::RNUM, ra.data(), ra.size());

  NODE_PTR grid_i = cntr.New_array(cntr.New_lda(input_grid_var, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr.Set_array_idx(grid_i, 0, cntr.New_ld(iv_var, spos));
  STMT_PTR ist = cntr.New_ist(grid_i, roll_node, spos);

  STMT_LIST body_sl = STMT_LIST::Enclosing_list(body_blk->End_stmt());
  body_sl.Append(ist);

  cntr.Stmt_list().Append(loop_stmt);

  VECTOR_CTX              ctx;
  VECTOR_CONFIG           cfg;
  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE* opt_glob = Roll_simp_opt(gscope, ctx, &driver_ctx, cfg);

  OP_STAT stat = Count_in_func(opt_glob, fscope->Id());
  // Loop split at the center tap: [0,4) and [5,9) keep rolling (2 loops,
  // 1 roll each), index 4 (offset 0) becomes a literal-zero roll (1 roll,
  // 1 ist) whose keyswitch is eliminated later at the POLY stage.
  EXPECT_EQ(stat._num_loop, 2u);
  EXPECT_EQ(stat._num_roll, 3u);
  EXPECT_EQ(stat._num_ist, 3u);
}
