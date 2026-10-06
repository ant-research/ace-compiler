//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_UNITTEST_UT_AIR2CGIR_UTIL_H
#define AIR_CG_UNITTEST_UT_AIR2CGIR_UTIL_H

#include "air/base/container.h"
#include "air/base/st.h"

using namespace air::base;
using namespace air::core;

namespace {

class TEST_AIR2CGIR_UTIL {
public:
  TEST_AIR2CGIR_UTIL() { Create_scope_func(); }
  ~TEST_AIR2CGIR_UTIL() { Destroy_scope_func(); }

  void Create_scope_func() {
    _glob           = new GLOB_SCOPE(0, true);
    _si32           = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
    SPOS     spos   = _glob->Unknown_simple_spos();
    STR_PTR  fn_str = _glob->New_str("foo");
    FUNC_PTR fn_ptr = _glob->New_func(fn_str, spos);
    fn_ptr->Set_parent(_glob->Comp_env_id());
    SIGNATURE_TYPE_PTR fn_sig = _glob->New_sig_type();
    _glob->New_ret_param(_si32, fn_sig);
    fn_sig->Set_complete();
    STR_PTR   i_str    = _glob->New_str("i");
    STR_PTR   x_str    = _glob->New_str("x");
    ENTRY_PTR fn_entry = _glob->New_entry_point(fn_sig, fn_ptr, fn_str, spos);
    _func              = &_glob->New_func_scope(fn_ptr);
    _i_var             = _func->New_var(_si32, i_str, spos);
    _x_var             = _func->New_var(_si32, x_str, spos);
    _cntr              = &_func->Container();
    _cntr->New_func_entry(spos);
  }

  STMT_PTR Create_retv(int64_t val) {
    SPOS     spos = _glob->Unknown_simple_spos();
    NODE_PTR retv = _cntr->New_intconst(_si32, val, spos);
    STMT_PTR rets = _cntr->New_retv(retv, spos);
    return rets;
  }

  STMT_PTR Create_if(const std::vector<int64_t>& vals) {
    // if (i < vals[0]) {
    //   x = vals[1];
    // } else {
    //   x = vals[2];
    // }
    SPOS     spos  = _glob->Unknown_simple_spos();
    NODE_PTR lod_i = _cntr->New_ld(_i_var, spos);
    NODE_PTR one   = _cntr->New_intconst(_si32, vals[0], spos);
    NODE_PTR two   = _cntr->New_intconst(_si32, vals[1], spos);
    NODE_PTR three = _cntr->New_intconst(_si32, vals[2], spos);
    NODE_PTR if_cond =
        _cntr->New_bin_arith(air::core::OPC_LT, _si32, lod_i, one, spos);
    NODE_PTR then_blk = _cntr->New_stmt_block(spos);
    STMT_PTR x_one    = _cntr->New_st(two, _x_var, spos);
    STMT_LIST(then_blk).Append(x_one);
    NODE_PTR else_blk = _cntr->New_stmt_block(spos);
    STMT_PTR x_three  = _cntr->New_st(three, _x_var, spos);
    STMT_LIST(else_blk).Append(x_three);
    STMT_PTR if_stmt =
        _cntr->New_if_then_else(if_cond, then_blk, else_blk, spos);
    return if_stmt;
  }

  STMT_PTR Create_do_loop(const std::vector<int64_t>& vals) {
    // for (i = vals[0]; i < vals[1]; i+=vals[2]) {
    //   x = vals[3];
    // }
    SPOS     spos  = _glob->Unknown_simple_spos();
    NODE_PTR lod_i = _cntr->New_ld(_i_var, spos);
    NODE_PTR one   = _cntr->New_intconst(_si32, vals[0], spos);
    NODE_PTR two   = _cntr->New_intconst(_si32, vals[1], spos);
    NODE_PTR three = _cntr->New_intconst(_si32, vals[2], spos);
    NODE_PTR four  = _cntr->New_intconst(_si32, vals[3], spos);
    NODE_PTR cmp =
        _cntr->New_bin_arith(air::core::OPC_LT, _si32, lod_i, two, spos);
    NODE_PTR incr = _cntr->New_bin_arith(
        air::core::OPC_ADD, _si32, _cntr->New_ld(_i_var, spos), four, spos);
    NODE_PTR body_blk = _cntr->New_stmt_block(spos);
    STMT_PTR x_three  = _cntr->New_st(three, _x_var, spos);
    STMT_LIST(body_blk).Append(x_three);
    STMT_PTR do_loop =
        _cntr->New_do_loop(_i_var, one, cmp, incr, body_blk, spos);
    return do_loop;
  }

  void Destroy_scope_func() {
    delete _glob;
    _func = nullptr;
    _cntr = nullptr;
  }

  void Convert_to_cgir(UT_AIR2CGIR_CTX& ctx) {
    air::base::VISITOR<UT_AIR2CGIR_CTX, air::core::HANDLER<UT_CORE_HANDLER>>
             trav(ctx);
    NODE_PTR body = _cntr->Entry_node();
    trav.template Visit<air::cg::OPND_PTR>(body);
  }

  STMT_LIST   Stmt_list() { return _cntr->Stmt_list(); }
  FUNC_SCOPE* Func_scope() const { return _func; }

  GLOB_SCOPE*    _glob;
  FUNC_SCOPE*    _func;
  CONTAINER*     _cntr;
  ADDR_DATUM_PTR _i_var;
  ADDR_DATUM_PTR _x_var;
  TYPE_PTR       _si32;
};

}  // namespace

#endif  // AIR_CG_UNITTEST_UT_AIR2CGIR_UTIL_H
