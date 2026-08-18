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
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/ir2c_handler.h"
#include "nn/core/handler.h"
#include "nn/core/ir2c_ctx.h"
#include "nn/core/ir2c_handler.h"
#include "nn/core/opcode.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;

class IR2C {
public:
  IR2C(GLOB_SCOPE* scope, std::ostream& os)
      : _scope(scope), _ctx(os), _os(os) {}

  void Run(FUNC_SCOPE* func) { Emit_function(func); }

  void Run() {
    Emit_types();
    Emit_global_vars();
    Emit_functions();
  }

private:
  void Emit_types() {
    // TODO: no interface
  }

  void Emit_global_vars() {
    // TODO: no interface
  }

  void Emit_functions() {
    // TODO: no interface
  }

  void Emit_function(FUNC_SCOPE* func) {
    // generate function signature
    _ctx.Emit_func_sig(func->Owning_func());

    NODE_PTR body = func->Container().Entry_node();
    air::base::VISITOR<nn::core::IR2C_CTX,
                       air::core::HANDLER<air::core::IR2C_HANDLER>,
                       nn::core::HANDLER<nn::core::IR2C_HANDLER> >
        trav(_ctx);
    trav.Visit<void>(body);
  }

private:
  GLOB_SCOPE*        _scope;
  nn::core::IR2C_CTX _ctx;
  std::ostream&      _os;
};

int main(int argc, char* argv[]) {
  // copied from ut_container.cxx to generate a simple function
  bool ret = air::core::Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  ret = nn::core::Register_nn();
  AIR_ASSERT_MSG(ret, "Register nn domain failed");
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
  // set define before create a new scope
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
  NODE_PTR x_node   = cntr->New_ld(var_x, spos);
  NODE_PTR y_node   = cntr->New_ld(var_y, spos);
  NODE_PTR add_node = cntr->New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::ADD), x_node,
      y_node, spos);
  // z = x + y;
  STMT_PTR  stmt = cntr->New_st(add_node, var_z, spos);
  STMT_LIST sl   = cntr->Stmt_list();
  sl.Append(stmt);
  // z = gemm(x, y)
  NODE_PTR nnx_node   = cntr->New_ld(var_x, spos);
  NODE_PTR nny_node   = cntr->New_ld(var_y, spos);
  NODE_PTR nnadd_node = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD), nnx_node,
      nny_node, spos);
  STMT_PTR nnstmt = cntr->New_st(nnadd_node, var_z, spos);
  sl.Append(nnstmt);
  // return z;
  NODE_PTR z_node   = cntr->New_ld(var_z, spos);
  STMT_PTR ret_stmt = cntr->New_retv(z_node, spos);
  sl.Append(ret_stmt);

  // IR2C
  IR2C ir2c(glob, std::cout);
  ir2c.Run(main_scope);

  return 0;
}
