//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <ostream>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/ir2c_ctx.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/ir2c_handler.h"
#include "air/core/opcode.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;

class IR2C {
public:
  IR2C(GLOB_SCOPE* scope, std::ostream& os)
      : _scope(scope), _ctx(os), _os(os) {}

  void Run(FUNC_SCOPE* func) { Emit_function(func); }

  void Run() {
    Emit_user_types();
    Emit_global_vars();
    Emit_functions();
  }

private:
  void Emit_constants() { _ctx.Emit_global_constants(_scope, false); }

  void Emit_user_types() {
    // TODO: no interface
  }

  void Emit_global_vars() {
    // TODO: no interface
  }

  void Emit_functions() {
    // TODO: no interface
  }

  void Emit_function(FUNC_SCOPE* func) {
    Emit_constants();
    // generate function signature
    _ctx.Emit_func_sig(func->Owning_func());
    NODE_PTR body = func->Container().Stmt_list().Block_node();
    air::base::VISITOR<IR2C_CTX, air::core::HANDLER<air::core::IR2C_HANDLER> >
        trav(_ctx);
    _ctx.Begin_func_body(body);
    _ctx.Emit_local_var(func);
    trav.Visit<void>(body);
    _ctx.End_func_body(body);
  }

private:
  GLOB_SCOPE*   _scope;
  IR2C_CTX      _ctx;
  std::ostream& _os;
};

int main(int argc, char* argv[]) {
  // copied from ut_container.cxx to generate a simple function
  bool ret = Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  GLOB_SCOPE* glob = GLOB_SCOPE::Get();
  SPOS        spos = glob->Unknown_simple_spos();
  // name of main function
  STR_PTR name_str = glob->New_str("main");
  // main function
  FUNC_PTR main_func = glob->New_func(name_str, spos);
  main_func->Set_parent(glob->Comp_env_id());
  // signature of main function
  SIGNATURE_TYPE_PTR sig = glob->New_sig_type();
  // return type of main function
  TYPE_PTR main_rtype = glob->Prim_type(PRIMITIVE_TYPE::VOID);
  glob->New_ret_param(main_rtype, sig);
  // parameter argc of function main
  TYPE_PTR argc_type = glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  STR_PTR  argc_str  = glob->New_str("argc");
  glob->New_param(argc_str, argc_type, sig, spos);
  // parameter argv of function main, no pointer type yet
  TYPE_PTR argv_type = glob->Prim_type(PRIMITIVE_TYPE::INT_S8);
  STR_PTR  argv_str  = glob->New_str("argv");
  glob->New_param(argv_str, argv_type, sig, spos);
  sig->Set_complete();
  // global entry for main
  ENTRY_PTR entry =
      glob->New_global_entry_point(sig, main_func, name_str, spos);
  FUNC_SCOPE* main_scope = &glob->New_func_scope(main_func);
  CONTAINER*  cntr       = &main_scope->Container();
  STMT_PTR    ent_stmt   = cntr->New_func_entry(spos);
  // int x, y, z;
  STR_PTR        x_str = glob->New_str("x");
  ADDR_DATUM_PTR var_x = main_scope->New_var(argc_type, x_str, spos);

  STR_PTR        y_str = glob->New_str("y");
  ADDR_DATUM_PTR var_y = main_scope->New_var(argc_type, y_str, spos);

  STR_PTR        z_str = glob->New_str("z");
  ADDR_DATUM_PTR var_z = main_scope->New_var(argc_type, z_str, spos);
  // x + y
  NODE_PTR x_node = cntr->New_ld(var_x, spos);
  NODE_PTR y_node = cntr->New_ld(var_y, spos);
  NODE_PTR add_node =
      cntr->New_bin_arith(air::core::OPC_ADD, x_node, y_node, spos);
  // z = x + y;
  STMT_PTR  stmt = cntr->New_st(add_node, var_z, spos);
  STMT_LIST sl   = cntr->Stmt_list();
  sl.Append(stmt);
  // x = 1;
  NODE_PTR one   = cntr->New_intconst(argc_type, 1, spos);
  STMT_PTR set_x = cntr->New_st(one, var_x, spos);
  sl.Append(set_x);
  // float data[4] = {1.1, 2.2, 3.3, 4.4};
  STR_PTR      type_name = glob->New_str("tensor_array");
  TYPE_PTR     etype     = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  ARB_PTR      arb       = glob->New_arb(1, 0, 4, 1);
  TYPE_PTR     var_type  = glob->New_arr_type(type_name, etype, arb, spos);
  float        data[]    = {1.1, 2.2, 3.3, 4.4};
  CONSTANT_PTR cst =
      glob->New_const(CONSTANT_KIND::ARRAY, var_type, data, sizeof(data));
  STR_PTR        data_str = glob->New_str("data");
  ADDR_DATUM_PTR var_data = main_scope->New_var(var_type, data_str, spos);
  NODE_PTR       ldc      = cntr->New_ldc(cst, spos);
  STMT_PTR       set_data = cntr->New_st(ldc, var_data, spos);
  sl.Append(set_data);
  // double data2[2][3][4] = {...}
  type_name     = glob->New_str("array_2_3_4");
  etype         = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_64);
  ARB_PTR arb_c = glob->New_arb(1, 0, 2, 1);
  ARB_PTR arb_h = glob->New_arb(2, 0, 3, 1);
  ARB_PTR arb_w = glob->New_arb(3, 0, 4, 1);
  arb_c->Set_next(arb_h->Id());
  arb_h->Set_next(arb_w->Id());
  var_type = glob->New_arr_type(type_name, etype, arb_c, spos);
  double data2[2 * 3 * 4];
  for (uint32_t i = 0; i < 2 * 3 * 4; ++i) {
    data2[i] = (i + 1) + (double)(i + 1) * 0.1;
  }
  cst = glob->New_const(CONSTANT_KIND::ARRAY, var_type, data2, sizeof(data2));
  data_str = glob->New_str("data2");
  var_data = main_scope->New_var(var_type, data_str, spos);
  ldc      = cntr->New_ldc(cst, spos);
  set_data = cntr->New_st(ldc, var_data, spos);
  sl.Append(set_data);
  // return z;
  NODE_PTR z_node   = cntr->New_ld(var_z, spos);
  STMT_PTR ret_stmt = cntr->New_retv(z_node, spos);
  sl.Append(ret_stmt);

  // IR2C
  IR2C ir2c(glob, std::cout);
  ir2c.Run(main_scope);

  return 0;
}
