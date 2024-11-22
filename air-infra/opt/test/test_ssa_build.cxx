//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"

using namespace air::base;
using namespace air::opt;

void Test_build_ssa() {
  GLOB_SCOPE* glob = GLOB_SCOPE::Get();
  SPOS        spos = glob->Unknown_simple_spos();

  // foo()
  STR_PTR  foo_str  = glob->New_str("foo");
  FUNC_PTR foo_func = glob->New_func(foo_str, spos);
  foo_func->Set_parent(glob->Comp_env_id());
  SIGNATURE_TYPE_PTR foo_sig = glob->New_sig_type();
  TYPE_PTR           sint32  = glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  glob->New_ret_param(sint32, foo_sig);
  STR_PTR a_str = glob->New_str("a");
  glob->New_param(a_str, sint32, foo_sig, spos);
  STR_PTR b_str = glob->New_str("b");
  glob->New_param(b_str, sint32, foo_sig, spos);
  STR_PTR c_str = glob->New_str("c");
  glob->New_param(c_str, sint32, foo_sig, spos);
  foo_sig->Set_complete();
  ENTRY_PTR foo_entry = glob->New_entry_point(foo_sig, foo_func, foo_str, spos);
  FUNC_SCOPE*    foo_scope = &glob->New_func_scope(foo_func);
  CONTAINER*     cntr      = &foo_scope->Container();
  STMT_PTR       estmt     = cntr->New_func_entry(spos);
  STMT_LIST      sl        = cntr->Stmt_list();
  ADDR_DATUM_PTR a_var     = foo_scope->Formal(0);
  ADDR_DATUM_PTR b_var     = foo_scope->Formal(1);
  ADDR_DATUM_PTR c_var     = foo_scope->Formal(2);
  STR_PTR        i_str     = glob->New_str("i");
  ADDR_DATUM_PTR i_var     = foo_scope->New_var(sint32, i_str, spos);
  STR_PTR        x_str     = glob->New_str("x");
  ADDR_DATUM_PTR x_var     = foo_scope->New_var(sint32, x_str, spos);
  // x = 0
  NODE_PTR zero;
  zero            = cntr->New_intconst(sint32, 0, spos);
  STMT_PTR init_x = cntr->New_st(zero, x_var, spos);
  sl.Append(init_x);
  // i < c
  NODE_PTR lod_i = cntr->New_ld(i_var, spos);
  NODE_PTR lod_c = cntr->New_ld(c_var, spos);
  NODE_PTR comp  = cntr->New_bin_arith(air::core::OPC_LT, lod_i, lod_c, spos);
  // i+1
  NODE_PTR one  = cntr->New_intconst(sint32, 1, spos);
  NODE_PTR incr = cntr->New_bin_arith(air::core::OPC_ADD, lod_i, one, spos);
  // body
  NODE_PTR  body = cntr->New_stmt_block(spos);
  STMT_LIST body_list(body);
  NODE_PTR  lod_x;

  // x < 10
  lod_x             = cntr->New_ld(x_var, spos);
  NODE_PTR ten      = cntr->New_intconst(sint32, 10, spos);
  NODE_PTR if_cond  = cntr->New_bin_arith(air::core::OPC_LT, lod_x, ten, spos);
  NODE_PTR then_blk = cntr->New_stmt_block(spos);
  NODE_PTR else_blk = cntr->New_stmt_block(spos);
  STMT_PTR if_stmt  = cntr->New_if_then_else(if_cond, then_blk, else_blk, spos);
  // a = 1
  STMT_PTR  st_a = cntr->New_st(one, a_var, spos);
  STMT_LIST then_list(then_blk);
  then_list.Append(st_a);
  body_list.Append(if_stmt);

  // a+b
  NODE_PTR lod_a = cntr->New_ld(a_var, spos);
  NODE_PTR lod_b = cntr->New_ld(b_var, spos);
  NODE_PTR add_a_b =
      cntr->New_bin_arith(air::core::OPC_ADD, lod_a, lod_b, spos);
  STMT_PTR st_x = cntr->New_st(add_a_b, x_var, spos);
  body_list.Append(st_x);
  // for (i=0; i < c; ++i)
  zero             = cntr->New_intconst(sint32, 0, spos);
  STMT_PTR do_loop = cntr->New_do_loop(i_var, zero, comp, incr, body, spos);
  sl.Append(do_loop);
  lod_x          = cntr->New_ld(x_var, spos);
  STMT_PTR ret_x = cntr->New_retv(lod_x, spos);
  sl.Append(ret_x);

  // print IR for testing
  foo_scope->Print();

  // Build SSA
  air::driver::DRIVER_CTX driver_ctx;
  air::opt::SSA_CONTAINER ssa_container(&foo_scope->Container());
  air::opt::SSA_BUILDER   bldr(foo_scope, &ssa_container, &driver_ctx);
  bldr.Perform();
}

int main() {
  air::core::Register_core();
  Test_build_ssa();
  return 0;
}
