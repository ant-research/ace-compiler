//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <iostream>
#include <memory>
#include <ostream>

#include "air/base/container.h"
#include "air/base/node.h"
#include "air/driver/driver_ctx.h"
#include "air/util/error.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/vector2sihe_lower.h"
#include "gtest/gtest.h"
#include "min_cut_region.h"

using namespace air::base;
using namespace fhe::sihe;
using namespace fhe::ckks;
using namespace fhe::core;
class REGION_CNTR_TEST : public testing::Test {
public:
  void Test_region_cntr();
  void Test_min_cut();

protected:
  void SetUp() override {
    _glob_scope = new GLOB_SCOPE(0, true);
    Register_domains();
    Register_types();
  }

  void TearDown() override {
    META_INFO::Remove_all();
    delete _glob_scope;
  }

  void Register_domains() {
    META_INFO::Remove_all();
    air::core::Register_core();
    bool reg_sihe = Register_sihe_domain();
    ASSERT_TRUE(reg_sihe) << "failed register sihe domain";
    bool reg_ckks = Register_ckks_domain();
    ASSERT_TRUE(reg_ckks) << "failed register ckks domain";
  }
  void Register_types() {
    SIHE_GEN(_glob_scope, &_lower_ctx).Register_sihe_types();
    CKKS_GEN(_glob_scope, &_lower_ctx).Register_ckks_types();
  };

  FUNC_SCOPE* Gen_uni_func(const char* func_name);
  FUNC_SCOPE* Gen_called_func(const char* func_name);
  FUNC_SCOPE* Gen_entry_func(const char* func_name);
  LOWER_CTX*  Lower_ctx() { return &_lower_ctx; }

  GLOB_SCOPE* _glob_scope;
  LOWER_CTX   _lower_ctx;
};

FUNC_SCOPE* REGION_CNTR_TEST::Gen_uni_func(const char* func_name) {
  SPOS     spos = _glob_scope->Unknown_simple_spos();
  TYPE_PTR ciph = Lower_ctx()->Get_cipher_type(_glob_scope);
  // gen function
  FUNC_PTR    func       = _glob_scope->New_func(func_name, spos);
  FUNC_SCOPE* func_scope = &_glob_scope->New_func_scope(func);

  func->Set_parent(_glob_scope->Comp_env_id());
  // signature of function
  SIGNATURE_TYPE_PTR sig = _glob_scope->New_sig_type();
  // return type of function
  _glob_scope->New_ret_param(ciph, sig);
  _glob_scope->New_param("", ciph, sig, spos);
  sig->Set_complete();

  _glob_scope->New_entry_point(sig, func, func_name, spos);
  return func_scope;
}

//! @brief gen functions
//! CIPHER Main_graph(CIPHER input) {
//!   for (int i = 0; i < 10; ++i) x += input;
//!   y = Approx_relu(x);
//!   z = Approx_relu(y);
//!   return z;
//! }
//! CIPHER Approx_relu(CIPHER x) {
//!   y = x * cst1;
//!   m = y * x;
//!   n = x * cst2;
//!   z = m + n;
//!   return z;
//! }
FUNC_SCOPE* REGION_CNTR_TEST::Gen_entry_func(const char* func_name) {
  FUNC_SCOPE* func_scope = Gen_uni_func(func_name);
  func_scope->Owning_func()->Entry_point()->Set_program_entry();

  SPOS           spos       = _glob_scope->Unknown_simple_spos();
  TYPE_PTR       ciph       = Lower_ctx()->Get_cipher_type(_glob_scope);
  ADDR_DATUM_PTR formal     = func_scope->New_formal(ciph->Id(), "input", spos);
  CONTAINER*     cntr       = &func_scope->Container();
  STMT_PTR       entry_stmt = cntr->New_func_entry(spos, 1);
  NODE_PTR       idname     = cntr->New_idname(formal, spos);
  entry_stmt->Node()->Set_child(0, idname);

  // x = 0
  NODE_PTR       zero   = cntr->New_zero(ciph, spos);
  ADDR_DATUM_PTR x      = func_scope->New_var(ciph, "x", spos);
  STMT_PTR       init_x = cntr->New_st(zero, x, spos);
  STMT_LIST      sl     = cntr->Stmt_list();
  sl.Append(init_x);

  // for (int i = 0; i < 10; ++i) x = x + input;
  TYPE_PTR       u32   = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_U32);
  ADDR_DATUM_PTR iv    = func_scope->New_var(u32, "i", spos);
  NODE_PTR       init  = cntr->New_intconst(u32, 0, spos);
  NODE_PTR       ld_iv = cntr->New_ld(iv, spos);
  NODE_PTR       upper = cntr->New_intconst(u32, 10, spos);
  NODE_PTR       comp =
      cntr->New_bin_arith(air::core::OPC_LT, u32, ld_iv, upper, spos);
  ld_iv         = cntr->New_ld(iv, spos);
  NODE_PTR step = cntr->New_intconst(u32, 1, spos);
  NODE_PTR incr =
      cntr->New_bin_arith(air::core::OPC_ADD, u32, ld_iv, step, spos);
  NODE_PTR  block = cntr->New_stmt_block(spos);
  STMT_LIST loop_body(block);
  NODE_PTR  ld_x      = cntr->New_ld(x, spos);
  NODE_PTR  ld_formal = cntr->New_ld(formal, spos);
  NODE_PTR  add =
      cntr->New_bin_arith(air::core::OPC_ADD, ciph, ld_x, ld_formal, spos);
  STMT_PTR st_sum = cntr->New_st(add, x, spos);
  loop_body.Append(st_sum);
  STMT_PTR do_loop = cntr->New_do_loop(iv, init, comp, incr, block, spos);
  sl.Append(do_loop);

  // y = Approx_relu(x);
  FUNC_SCOPE* called_func = Gen_called_func("Approx_relu");
  ENTRY_PTR   entry       = called_func->Owning_func()->Entry_point();
  PREG_PTR    preg_y      = func_scope->New_preg(ciph);
  STMT_PTR    call_a      = cntr->New_call(entry, preg_y, 1, spos);
  call_a->Node()->Set_child(0, cntr->New_ld(x, spos));
  sl.Append(call_a);

  // z = Approx_relu(y);
  PREG_PTR preg_z = func_scope->New_preg(ciph);
  STMT_PTR call_b = cntr->New_call(entry, preg_z, 1, spos);
  call_b->Node()->Set_child(0, cntr->New_ldp(preg_y, spos));
  sl.Append(call_b);

  // retv z
  STMT_PTR retv = cntr->New_retv(cntr->New_ldp(preg_z, spos), spos);
  sl.Append(retv);
  return func_scope;
}

FUNC_SCOPE* REGION_CNTR_TEST::Gen_called_func(const char* func_name) {
  // gen function scope
  FUNC_SCOPE* func_scope = Gen_uni_func(func_name);

  SPOS           spos       = _glob_scope->Unknown_simple_spos();
  TYPE_PTR       ciph       = Lower_ctx()->Get_cipher_type(_glob_scope);
  TYPE_PTR       ciph3      = Lower_ctx()->Get_cipher3_type(_glob_scope);
  ADDR_DATUM_PTR formal     = func_scope->New_formal(ciph->Id(), "x", spos);
  CONTAINER*     cntr       = &func_scope->Container();
  STMT_PTR       entry_stmt = cntr->New_func_entry(spos, 1);
  NODE_PTR       idname     = cntr->New_idname(formal, spos);
  entry_stmt->Node()->Set_child(0, idname);

  // y = 0.5 * x
  TYPE_PTR     f64 = _glob_scope->Prim_type(PRIMITIVE_TYPE::FLOAT_64);
  CONSTANT_PTR cst1 =
      _glob_scope->New_const(CONSTANT_KIND::FLOAT, f64, (long double)0.5);
  NODE_PTR ldc1 = cntr->New_ldc(cst1, spos);
  NODE_PTR ld_x = cntr->New_ld(formal, spos);
  NODE_PTR mul =
      cntr->New_bin_arith(fhe::ckks::OPC_MUL, ciph, ld_x, ldc1, spos);
  PREG_PTR  preg_y = func_scope->New_preg(ciph);
  STMT_PTR  stp_y  = cntr->New_stp(mul, preg_y, spos);
  STMT_LIST sl     = cntr->Stmt_list();
  sl.Append(stp_y);

  // m = y * x
  ld_x          = cntr->New_ld(formal, spos);
  NODE_PTR ld_y = cntr->New_ldp(preg_y, spos);
  mul = cntr->New_bin_arith(fhe::ckks::OPC_MUL, ciph3, ld_y, ld_x, spos);
  NODE_PTR relin  = cntr->New_una_arith(OPC_RELIN, ciph, mul, spos);
  PREG_PTR preg_m = func_scope->New_preg(ciph);
  STMT_PTR stp_m  = cntr->New_stp(relin, preg_m, spos);
  sl.Append(stp_m);

  // n = 0.4 * x
  CONSTANT_PTR cst2 =
      _glob_scope->New_const(CONSTANT_KIND::FLOAT, f64, (long double)0.4);
  NODE_PTR ldc2 = cntr->New_ldc(cst2, spos);
  ld_x          = cntr->New_ld(formal, spos);
  mul = cntr->New_bin_arith(fhe::ckks::OPC_MUL, ciph, ld_x, ldc2, spos);
  PREG_PTR preg_n = func_scope->New_preg(ciph);
  STMT_PTR stp_n  = cntr->New_stp(mul, preg_n, spos);
  sl.Append(stp_n);

  // z = m + n
  NODE_PTR ld_m = cntr->New_ldp(preg_m, spos);
  NODE_PTR ld_n = cntr->New_ldp(preg_n, spos);
  NODE_PTR add =
      cntr->New_bin_arith(fhe::ckks::OPC_ADD, ciph, ld_m, ld_n, spos);
  PREG_PTR preg_z = func_scope->New_preg(ciph);
  STMT_PTR stp_z  = cntr->New_stp(add, preg_z, spos);
  sl.Append(stp_z);

  // return z
  STMT_PTR retv = cntr->New_retv(cntr->New_ldp(preg_z, spos), spos);
  sl.Append(retv);
  return func_scope;
}

void REGION_CNTR_TEST::Test_region_cntr() {
  FUNC_SCOPE*             entry_func = Gen_entry_func("Main_graph");
  air::driver::DRIVER_CTX driver_ctx;
  REGION_CONTAINER        region_cntr(_glob_scope, Lower_ctx());
  REGION_BUILDER          region_builder(&region_cntr, &driver_ctx);
  R_CODE                  ret = region_builder.Perform();
  ASSERT_EQ(ret, R_CODE::NORMAL);
  ASSERT_EQ(region_cntr.Region_cnt(), 6);
}

void REGION_CNTR_TEST::Test_min_cut() {
  FUNC_SCOPE*             entry_func = Gen_entry_func("Main_graph");
  air::driver::DRIVER_CTX driver_ctx;
  driver_ctx.Tfile().Open("min_cut_region3.t");
  CKKS_CONFIG config;
  config._trace_detail |= TD_RESBM_MIN_CUT;
  REGION_CONTAINER region_cntr(_glob_scope, Lower_ctx());
  REGION_BUILDER   region_builder(&region_cntr, &driver_ctx);
  R_CODE           ret = region_builder.Perform();
  ASSERT_EQ(ret, R_CODE::NORMAL);
  ASSERT_EQ(region_cntr.Region_cnt(), 6);
  MIN_CUT_REGION min_cut1(&config, &driver_ctx, &region_cntr, REGION_ID(3), 1,
                          fhe::ckks::MIN_CUT_RESCALE);
  min_cut1.Perform();
  ASSERT_EQ(min_cut1.Min_cut().Size(), 2);
  for (REGION_ELEM_ID id : min_cut1.Min_cut().Cut_elem()) {
    REGION_ELEM_PTR   elem = region_cntr.Region_elem(id);
    air::base::OPCODE opc  = elem->Dfg_node()->Node()->Opcode();
    ASSERT_EQ(opc, fhe::ckks::OPC_MUL);
  }

  MIN_CUT_REGION min_cut2(&config, &driver_ctx, &region_cntr, REGION_ID(3), 2,
                          fhe::ckks::MIN_CUT_RESCALE);
  min_cut2.Perform();
  ASSERT_EQ(min_cut2.Min_cut().Size(), 1);
  for (REGION_ELEM_ID id : min_cut2.Min_cut().Cut_elem()) {
    REGION_ELEM_PTR   elem = region_cntr.Region_elem(id);
    air::base::OPCODE opc  = elem->Dfg_node()->Node()->Opcode();
    ASSERT_EQ(opc, fhe::ckks::OPC_ADD);
  }
}

TEST_F(REGION_CNTR_TEST, test_region_cntr) { Test_region_cntr(); }
TEST_F(REGION_CNTR_TEST, test_min_cut) { Test_min_cut(); }