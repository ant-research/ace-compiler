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

  void Append_output();

  air::base::GLOB_SCOPE*    Glob() { return _glob; }
  air::base::TYPE_PTR       Ciph_ty() { return _ciph_ty; }
  air::base::TYPE_PTR       Ciph3_ty() { return _ciph3_ty; }
  air::base::TYPE_PTR       Plain_ty() { return _plain_ty; }
  air::base::ADDR_DATUM_PTR Input_var() { return _v_input; }
  air::base::ADDR_DATUM_PTR Output_var() { return _v_output; }

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
  // name of main function
  air::base::STR_PTR name_str = Glob()->New_str("Main_graph");
  // main function
  air::base::FUNC_PTR main_func = Glob()->New_func(name_str, Spos());
  main_func->Set_parent(Glob()->Comp_env_id());
  // signature of main function
  air::base::SIGNATURE_TYPE_PTR sig = Glob()->New_sig_type();
  // return type of main function
  air::base::TYPE_PTR main_rtype =
      Glob()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
  Glob()->New_ret_param(main_rtype, sig);
  // parameter argc of function main
  air::base::TYPE_PTR argc_type =
      Glob()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
  air::base::STR_PTR argc_str = Glob()->New_str("input");
  Glob()->New_param(argc_str, _ciph_ty, sig, Spos());
  sig->Set_complete();
  // global entry for main
  air::base::ENTRY_PTR entry =
      Glob()->New_global_entry_point(sig, main_func, name_str, Spos());
  // set define before create a new scope
  air::base::FUNC_SCOPE* main_scope = &(Glob()->New_func_scope(main_func));
  air::base::STMT_PTR ent_stmt = main_scope->Container().New_func_entry(Spos());
  _v_input                     = main_scope->Formal(0);
  _v_output = main_scope->New_var(_ciph_ty, "output", Spos());

  main_scope->Owning_func()->Entry_point()->Set_program_entry();

  return main_scope;
}

void CKKS_IR_GEN::Append_output() {
  air::base::CONTAINER* cntr = &_main_graph->Container();
  air::base::STMT_LIST  sl   = cntr->Stmt_list();

  // return
  air::base::NODE_PTR ld_output_var = cntr->New_ld(Output_var(), Spos());
  air::base::STMT_PTR ret_stmt      = cntr->New_retv(ld_output_var, Spos());
  sl.Append(ret_stmt);

  // analyze CKKS parameters
  air::driver::DRIVER_CTX driver_ctx;
  fhe::ckks::CKKS_CONFIG  config;
  _lower_ctx.Get_ctx_param().Set_poly_degree(16);
  fhe::core::CTX_PARAM_ANA ctx_param_ana(Container()->Parent_func_scope(),
                                         &_lower_ctx, &driver_ctx, &config);
  ctx_param_ana.Run();
}

}  // namespace test
}  // namespace poly
}  // namespace fhe

#endif
