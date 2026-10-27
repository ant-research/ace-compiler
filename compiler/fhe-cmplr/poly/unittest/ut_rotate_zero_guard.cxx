//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
//! @brief Unit tests for the rot-0 fast path in CKKS2HPOLY rotate lowering.
//!
//! Background: the non-inline (base) pipeline emits a runtime
//! `if (rot_idx == 0) return copy;` fast path into the generated Rotate
//! helper (POLY2C_DRIVER::Emit_rotate_zero_fast_path). The inline (HPOLY)
//! pipeline used by -POLY:hmup/-POLY:smd previously expanded every rotation
//! unconditionally into PRECOMP + 2xDOT_PROD + 2xMOD_DOWN, so identity
//! rotations paid a full keyswitch.
//! These tests pin down the fixed behavior for literal-0 indices:
//!   - rotate(x, 0)          -> plain copy, 0 _precomp, 0 dotprod, 0 moddown
//!   - rotate(x, 5)          -> normal keyswitch (1 _precomp, 2 dotprod, 2 moddown)
//!   - rotate(x,0) + rot(x,3)-> only the non-zero rotation pays a keyswitch
//!   - full pipeline smoke   -> lowers through LPOLY without corruption

#include "../include/poly_ir_gen.h"
#include "air/base/opcode.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/opcode.h"
#include "fhe/test/gen_ckks_ir.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace fhe::core;

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

GLOB_SCOPE* Run_pipeline(GLOB_SCOPE* glob, LOWER_CTX& lower_ctx,
                         air::driver::DRIVER_CTX* drv_ctx, bool to_lpoly) {
  POLY_CONFIG config;
  config._inline_rotate  = true;
  config._inline_relin   = true;
  config._lower_to_hpoly = true;
  config._lower_to_lpoly = to_lpoly;
  config._hoist_mdup     = true;
  fhe::poly::POLY_DRIVER poly_driver;
  return poly_driver.Run(config, glob, lower_ctx, drv_ctx);
}

void Setup_ctx(LOWER_CTX& lower_ctx) {
  lower_ctx.Get_ctx_param().Set_poly_degree(16, false);
  lower_ctx.Get_ctx_param().Set_mul_level(3, true);
  lower_ctx.Get_ctx_param().Set_q_part_num(2);
}

void Set_rotate_level(STMT_PTR rotate_stmt, uint32_t lvl) {
  NODE_PTR rhs = rotate_stmt->Node()->Child(0);
  rhs->Set_attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL, &lvl, 1);
}

}  // namespace

//! rotate(x, 0) with constant index: identity copy, no keyswitch ops
TEST(TEST_ROTATE_ZERO_GUARD, literal_zero_no_keyswitch) {
  LOWER_CTX lower_ctx;
  Setup_ctx(lower_ctx);
  CKKS_IR_GEN ir_gen(lower_ctx);
  CONTAINER*  cntr = ir_gen.Main_container();

  ADDR_DATUM_PTR var_in   = ir_gen.Input_var();
  ADDR_DATUM_PTR var_rot0 = ir_gen.Gen_ciph_var("ciph_rot0");
  ir_gen.Gen_rotate(cntr, var_rot0, var_in, 0);
  ir_gen.Gen_add(cntr, ir_gen.Output_var(), var_rot0, var_in);

  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE*             glob =
      Run_pipeline(cntr->Glob_scope(), lower_ctx, &driver_ctx, false);

  OP_COUNT cnt = Count_in_glob(glob);
  EXPECT_EQ(cnt._precomp, 0u);
  EXPECT_EQ(cnt._dot_prod, 0u);
  EXPECT_EQ(cnt._mod_down, 0u);
  EXPECT_EQ(cnt._poly_rotate, 0u);
}

//! rotate(x, 5) with constant index: normal keyswitch ops emitted
TEST(TEST_ROTATE_ZERO_GUARD, literal_nonzero_unchanged) {
  LOWER_CTX lower_ctx;
  Setup_ctx(lower_ctx);
  CKKS_IR_GEN ir_gen(lower_ctx);
  CONTAINER*  cntr = ir_gen.Main_container();

  ADDR_DATUM_PTR var_in   = ir_gen.Input_var();
  ADDR_DATUM_PTR var_rot5 = ir_gen.Gen_ciph_var("ciph_rot5");
  STMT_PTR s_rot5 = ir_gen.Gen_rotate(cntr, var_rot5, var_in, 5);
  Set_rotate_level(s_rot5, 3);
  ir_gen.Gen_add(cntr, ir_gen.Output_var(), var_rot5, var_in);

  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE*             glob =
      Run_pipeline(cntr->Glob_scope(), lower_ctx, &driver_ctx, false);

  OP_COUNT cnt = Count_in_glob(glob);
  EXPECT_EQ(cnt._precomp, 1u);
  EXPECT_EQ(cnt._dot_prod, 2u);
  EXPECT_EQ(cnt._mod_down, 2u);
  EXPECT_EQ(cnt._poly_rotate, 2u);
}

//! add(rotate(x,0), rotate(x,3)): only the non-zero rotation pays a keyswitch
TEST(TEST_ROTATE_ZERO_GUARD, mixed_literal_sum) {
  LOWER_CTX lower_ctx;
  Setup_ctx(lower_ctx);
  CKKS_IR_GEN ir_gen(lower_ctx);
  CONTAINER*  cntr = ir_gen.Main_container();

  ADDR_DATUM_PTR var_in   = ir_gen.Input_var();
  ADDR_DATUM_PTR var_rot0 = ir_gen.Gen_ciph_var("ciph_rot0");
  ADDR_DATUM_PTR var_rot3 = ir_gen.Gen_ciph_var("ciph_rot3");
  ir_gen.Gen_rotate(cntr, var_rot0, var_in, 0);
  STMT_PTR s_rot3 = ir_gen.Gen_rotate(cntr, var_rot3, var_in, 3);
  Set_rotate_level(s_rot3, 3);
  ir_gen.Gen_add(cntr, ir_gen.Output_var(), var_rot0, var_rot3);

  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE*             glob =
      Run_pipeline(cntr->Glob_scope(), lower_ctx, &driver_ctx, false);

  OP_COUNT cnt = Count_in_glob(glob);
  EXPECT_EQ(cnt._precomp, 1u);  // only rotate(x, 3) pays
  EXPECT_EQ(cnt._dot_prod, 2u);
  EXPECT_EQ(cnt._mod_down, 2u);
  EXPECT_EQ(cnt._poly_rotate, 2u);
}

//! Full-pipeline smoke test (through LPOLY): lowers end to end without error
TEST(TEST_ROTATE_ZERO_GUARD, full_pipeline_lpoly_smoke) {
  LOWER_CTX lower_ctx;
  Setup_ctx(lower_ctx);
  CKKS_IR_GEN ir_gen(lower_ctx);
  CONTAINER*  cntr = ir_gen.Main_container();

  ADDR_DATUM_PTR var_in   = ir_gen.Input_var();
  ADDR_DATUM_PTR var_rot0 = ir_gen.Gen_ciph_var("ciph_rot0");
  ADDR_DATUM_PTR var_rot3 = ir_gen.Gen_ciph_var("ciph_rot3");
  ir_gen.Gen_rotate(cntr, var_rot0, var_in, 0);        // identity (literal 0)
  ir_gen.Gen_rotate(cntr, var_rot3, var_in, 3);        // normal keyswitch
  ir_gen.Gen_add(cntr, ir_gen.Output_var(), var_rot0, var_rot3);

  air::driver::DRIVER_CTX driver_ctx;
  GLOB_SCOPE*             glob =
      Run_pipeline(cntr->Glob_scope(), lower_ctx, &driver_ctx, true);
  EXPECT_NE(glob, nullptr);
}

}  // namespace test
}  // namespace poly
}  // namespace fhe
