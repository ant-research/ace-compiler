//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"

namespace air {

namespace cg {

namespace demo {

using namespace air::base;
using namespace air::core;

static air::base::CONTAINER* Init_air_cont() {
  // TEST PROGRAM:
  //   int padd() {
  //     long op0[16], op1[16], res[16];
  //     int i;
  //     for (i = 0; i < 16; ++i) {
  //       res[i] = op0[i] + op1[i];
  //     }
  //     return i;
  //   }
  air::core::Register_core();
  GLOB_SCOPE* glob = GLOB_SCOPE::Get();
  SPOS        spos = glob->Unknown_simple_spos();

  // type long array[16], struct poly
  TYPE_PTR           sint32  = glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR           sint64  = glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  std::vector<int64_t> dim{16};
  TYPE_PTR sarr  = glob->New_arr_type("s64_16", sint64, dim, spos);

  // function int padd() {}
  STR_PTR  padd_str  = glob->New_str("padd");
  FUNC_PTR padd_func = glob->New_func(padd_str, spos);
  padd_func->Set_parent(glob->Comp_env_id());
  SIGNATURE_TYPE_PTR padd_sig = glob->New_sig_type();
  glob->New_ret_param(sint32, padd_sig);
  padd_sig->Set_complete();
  ENTRY_PTR padd_entry = glob->New_entry_point(padd_sig, padd_func, padd_str, spos);
  FUNC_SCOPE*    padd_scope = &glob->New_func_scope(padd_func);
  STR_PTR res_str = glob->New_str("res");
  ADDR_DATUM_PTR res_var   = padd_scope->New_var(sarr, res_str, spos);
  STR_PTR op0_str = glob->New_str("op0");
  ADDR_DATUM_PTR op0_var = padd_scope->New_var(sarr, op0_str, spos);
  STR_PTR op1_str = glob->New_str("op1");
  ADDR_DATUM_PTR op1_var = padd_scope->New_var(sarr, op1_str, spos);

  STR_PTR        i_str     = glob->New_str("i");
  ADDR_DATUM_PTR i_var     = padd_scope->New_var(sint32, i_str, spos);
  CONTAINER*     cntr      = &padd_scope->Container();
  STMT_PTR       estmt     = cntr->New_func_entry(spos);
  STMT_LIST      sl        = cntr->Stmt_list();

  // for(i=0; i < 16; ++i) {}
  NODE_PTR ld_i0 = cntr->New_ld(i_var, spos);
  NODE_PTR ld_i1 = cntr->New_ld(i_var, spos);
  NODE_PTR cst_16 = cntr->New_intconst(sint32, 16, spos);
  NODE_PTR cst_1 = cntr->New_intconst(sint32, 1, spos);
  NODE_PTR body = cntr->New_stmt_block(spos);
  STMT_PTR loop = cntr->New_do_loop(i_var, cntr->New_intconst(sint32, 0, spos),
	  cntr->New_bin_arith(air::core::OPC_LT, sint32, ld_i0, cst_16, spos),
	  cntr->New_bin_arith(air::core::OPC_ADD, sint32, ld_i1, cst_1, spos),
	  body, spos);
  sl.Append(loop);

  // res[i] = op0[i] + op1[i];
  NODE_PTR op0_arr = cntr->New_array(cntr->New_lda(op0_var, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr->Set_array_idx(op0_arr, 0, cntr->New_ld(i_var, spos));
  NODE_PTR op1_arr = cntr->New_array(cntr->New_lda(op1_var, POINTER_KIND::FLAT32,spos), 1, spos);
  cntr->Set_array_idx(op1_arr, 0, cntr->New_ld(i_var, spos));
  NODE_PTR ist_val = cntr->New_bin_arith(air::core::OPC_ADD, sint64,
	  cntr->New_ild(op0_arr, sint64, spos),
	  cntr->New_ild(op1_arr, sint64, spos),
	  spos);
  NODE_PTR res_arr = cntr->New_array(cntr->New_lda(res_var, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr->Set_array_idx(res_arr, 0, cntr->New_ld(i_var, spos));
  STMT_PTR ist = cntr->New_ist(res_arr, ist_val, spos);
  STMT_LIST body_list(body);
  body_list.Append(ist);

  // return res
  STMT_PTR ret = cntr->New_retv(cntr->New_ld(i_var, spos), spos);
  sl.Append(ret);

  return cntr;
}  // Init_air_cont

}  // namespace demo

}  // namespace cg

}  // namespace air
