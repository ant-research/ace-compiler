//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_TEST_GEN_CKKS_IR_H
#define FHE_POLY_TEST_GEN_CKKS_IR_H

#include <limits.h>

#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/node.h"
#include "air/base/st.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/config.h"
#include "fhe/poly/opcode.h"
#include "fhe/poly/poly2c_driver.h"
#include "fhe/poly/poly_driver.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/util/util.h"

namespace fhe {
namespace poly {
namespace test {

class CKKS_IR_GEN {
public:
  CKKS_IR_GEN(fhe::core::LOWER_CTX& lower_ctx) : _lower_ctx(lower_ctx) {
    bool ret = air::core::Register_core();
    CMPLR_ASSERT(ret, "core register failed");
    ret = fhe::ckks::Register_ckks_domain();
    CMPLR_ASSERT(ret, "ckks register failed");
    ret = fhe::poly::Register_polynomial();
    CMPLR_ASSERT(ret, "polynomial register failed");

    _glob = air::base::GLOB_SCOPE::Get();
    fhe::sihe::SIHE_GEN(Glob(), &_lower_ctx).Register_sihe_types();
    fhe::ckks::CKKS_GEN(Glob(), &_lower_ctx).Register_ckks_types();
    _ciph_ty    = Glob()->Type(_lower_ctx.Get_cipher_type_id());
    _ciph3_ty   = Glob()->Type(_lower_ctx.Get_cipher3_type_id());
    _plain_ty   = Glob()->Type(_lower_ctx.Get_plain_type_id());
    _spos       = Glob()->Unknown_simple_spos();
    _main_graph = Gen_main_graph();
  }

  air::base::SPOS Spos() { return _spos; }

  air::base::CONTAINER* Container() { return &_main_graph->Container(); }

  air::base::FUNC_SCOPE* Gen_main_graph();
  air::base::FUNC_SCOPE* Gen_func(
      const char* fname, air::base::TYPE_PTR ret_type,
      std::vector<air::base::TYPE_PTR>& param_types);

  void Append_output();

  void Analyze_ckks_params(air::base::FUNC_SCOPE* fs);

  air::base::GLOB_SCOPE*    Glob() { return _glob; }
  air::base::TYPE_PTR       Ciph_ty() { return _ciph_ty; }
  air::base::TYPE_PTR       Ciph3_ty() { return _ciph3_ty; }
  air::base::TYPE_PTR       Plain_ty() { return _plain_ty; }
  air::base::ADDR_DATUM_PTR Input_var() { return _v_input; }
  air::base::ADDR_DATUM_PTR Output_var() { return _v_output; }
  fhe::core::LOWER_CTX&     Lower_ctx() { return _lower_ctx; }

private:
  fhe::core::LOWER_CTX&     _lower_ctx;
  air::base::FUNC_SCOPE*    _main_graph;
  air::base::GLOB_SCOPE*    _glob;
  air::base::TYPE_PTR       _ciph_ty;
  air::base::TYPE_PTR       _ciph3_ty;
  air::base::TYPE_PTR       _plain_ty;
  air::base::ADDR_DATUM_PTR _v_input;
  air::base::ADDR_DATUM_PTR _v_output;
  air::base::SPOS           _spos;
};

air::base::FUNC_SCOPE* CKKS_IR_GEN::Gen_main_graph() {
  air::base::TYPE_PTR main_rtype =
      Glob()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
  std::vector<air::base::TYPE_PTR> param_types;
  param_types.push_back(_ciph_ty);

  air::base::FUNC_SCOPE* main_scope =
      Gen_func("Main_graph", main_rtype, param_types);

  _v_input  = main_scope->Formal(0);
  _v_output = main_scope->New_var(_ciph_ty, "output", Spos());

  main_scope->Owning_func()->Entry_point()->Set_program_entry();
  return main_scope;
}

air::base::FUNC_SCOPE* CKKS_IR_GEN::Gen_func(
    const char* fname, air::base::TYPE_PTR ret_type,
    std::vector<air::base::TYPE_PTR>& param_types) {
  // name of function
  air::base::STR_PTR name_str = Glob()->New_str(fname);
  // new function
  air::base::FUNC_PTR func = Glob()->New_func(name_str, Spos());
  func->Set_parent(Glob()->Comp_env_id());
  // signature of function
  air::base::SIGNATURE_TYPE_PTR sig = Glob()->New_sig_type();
  // return type of function
  if (ret_type != air::base::Null_ptr) {
    Glob()->New_ret_param(ret_type, sig);
  }
  // parameter of function
  uint32_t param_idx = 0;
  for (auto param_type : param_types) {
    std::string param_name = "input";
    if (param_idx > 0) param_name += std::to_string(param_idx);
    air::base::STR_PTR param_str = Glob()->New_str(param_name.c_str());
    Glob()->New_param(param_str, param_type, sig, Spos());
    param_idx++;
  }
  sig->Set_complete();
  // global entry for main
  air::base::ENTRY_PTR entry =
      Glob()->New_global_entry_point(sig, func, name_str, Spos());
  // set define before create a new scope
  air::base::FUNC_SCOPE* func_scope = &(Glob()->New_func_scope(func));
  air::base::STMT_PTR ent_stmt = func_scope->Container().New_func_entry(Spos());
  return func_scope;
}

void CKKS_IR_GEN::Append_output() {
  air::base::CONTAINER* cntr = &_main_graph->Container();
  air::base::STMT_LIST  sl   = cntr->Stmt_list();

  // return
  air::base::NODE_PTR ld_output_var = cntr->New_ld(Output_var(), Spos());
  air::base::STMT_PTR ret_stmt      = cntr->New_retv(ld_output_var, Spos());
  sl.Append(ret_stmt);

  Analyze_ckks_params(Container()->Parent_func_scope());
}

void CKKS_IR_GEN::Analyze_ckks_params(air::base::FUNC_SCOPE* fs) {
  air::driver::DRIVER_CTX driver_ctx;
  fhe::ckks::CKKS_CONFIG  config;
  _lower_ctx.Get_ctx_param().Set_poly_degree(16);
  fhe::core::CTX_PARAM_ANA ctx_param_ana(fs, &_lower_ctx, &driver_ctx, &config);
  ctx_param_ana.Run();
}

}  // namespace test
}  // namespace poly
}  // namespace fhe

#endif
