//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks_py_airgen.h"

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

namespace fhe {
namespace ckks {

// Note: Guard is used to manage the lifecycle of python interpreter in C++
// application, put it to file scope to avoid reinvoke of python interpreter.
// And if we put the guard in each New_py_xx method, there will be
// issue for multiple module invoke.
// static py::scoped_interpreter Guard{};

#define UPDATE_BODY(body)                                               \
  for (STMT_PTR stmt = body->Begin_stmt(); stmt != body->End_stmt();) { \
    STMT_PTR next_stmt = stmt->Next();                                  \
    _ctx.Prepend(stmt);                                                 \
    stmt = next_stmt;                                                   \
  }

NODE_PTR CKKS_PY_IMPL::New_py_add(NODE_PTR node, NODE_PTR new_ld0,
                                  NODE_PTR new_ld1, const SPOS& spos) {
  CONTAINER*     cntr    = _ctx.Ckks_gen().Cntr();
  FUNC_SCOPE*    fs      = cntr->Parent_func_scope();
  NODE_PTR       body    = fs->Container().New_stmt_block(node->Spos());
  ADDR_DATUM_PTR new_var = fs->New_var(node->Rtype(), "ckks_dsl_out", spos);

  try {
    py::module air_dsl  = py::module::import("air_dsl");
    py::module ckks_dsl = py::module::import("ckks_dsl");

    py::object py_fs      = py::cast(fs);
    py::object py_body    = py::cast(body);
    py::object py_node    = py::cast(node);
    py::object py_out     = py::cast(new_var);
    py::object pyctx_cls  = air_dsl.attr("PyContext");
    py::object pyctx      = pyctx_cls(py_fs, py_body, py_node, py_out);
    py::object py_new_ld0 = py::cast(new_ld0);
    py::object py_new_ld1 = py::cast(new_ld1);
    py::object add        = ckks_dsl.attr("add");
    add(pyctx, py_new_ld0, py_new_ld1);

    UPDATE_BODY(body);

    NODE_PTR new_load = fs->Container().New_ld(new_var, node->Spos());
    return new_load;
  } catch (py::error_already_set& e) {
    e.restore();
    PyErr_Print();
    AIR_ASSERT_MSG(false, "Call Python dsl failed");
    return air::base::Null_ptr;
  }
}

FUNC_SCOPE* CKKS_PY_IMPL::New_py_bts(NODE_PTR node, const SPOS& spos) {
  CONTAINER*      cntr = _ctx.Ckks_gen().Cntr();
  FUNC_SCOPE*     fs   = cntr->Parent_func_scope();
  const uint32_t* mul_lev =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
  // Use default level 0 (full level) if not set
  uint32_t level = mul_lev ? *mul_lev : 0;

  // Check if bootstrap function for this level already exists
  air::base::FUNC_ID cached_func_id = _ctx.Get_bts_func(level);
  if (cached_func_id != air::base::FUNC_ID()) {
    // Function already generated, return existing scope
    GLOB_SCOPE* glob = cntr->Glob_scope();
    return &glob->Open_func_scope(cached_func_id);
  }

  FUNC_SCOPE*    f_bts   = New_bts_func(node, spos);
  ADDR_DATUM_PTR new_var = f_bts->New_var(node->Rtype(), "ckks_bts_out", spos);
  NODE_PTR       body    = f_bts->Container().New_stmt_block(node->Spos());

  try {
    py::module m_dsl     = py::module::import("air_dsl");
    py::module ckks_dsl  = py::module::import("ckks_dsl");
    py::object py_fs     = py::cast(f_bts);
    py::object py_body   = py::cast(body);
    py::object py_node   = py::cast(node);
    py::object py_out    = py::cast(new_var);
    py::object pyctx_cls = m_dsl.attr("PyContext");
    py::object pyctx     = pyctx_cls(py_fs, py_body, py_node, py_out);
    py::object py_formal0 =
        py::cast(f_bts->Container().New_ld(f_bts->Formal(0), spos));
    py::object bts = ckks_dsl.attr("air_bootstrap");
    bts(pyctx, py_formal0, level);
    f_bts->Container().Stmt_list().Append(STMT_LIST(body));
    return f_bts;
  } catch (py::error_already_set& e) {
    e.restore();
    PyErr_Print();
    AIR_ASSERT_MSG(false, "Call Python dsl failed");
    return nullptr;
  }
}

FUNC_SCOPE* CKKS_PY_IMPL::New_bts_func(NODE_PTR node, const SPOS& spos) {
  CONTAINER*      cntr = _ctx.Ckks_gen().Cntr();
  GLOB_SCOPE*     glob = cntr->Glob_scope();
  const uint32_t* mul_lev =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
  // Use default level 0 (full level) if not set
  uint32_t level = mul_lev ? *mul_lev : 0;

  // Check if bootstrap function for this level already exists
  air::base::FUNC_ID cached_func_id = _ctx.Get_bts_func(level);
  if (cached_func_id != air::base::FUNC_ID()) {
    return &glob->Open_func_scope(cached_func_id);
  }

  std::string func_name = "Bootstrap_";
  func_name += std::to_string(level);
  STR_PTR  name_str = glob->New_str(func_name.c_str());
  FUNC_PTR bts_func = glob->New_func(name_str, spos);
  bts_func->Set_parent(glob->Comp_env_id());

  // setup return
  SIGNATURE_TYPE_PTR sig   = glob->New_sig_type();
  TYPE_PTR           rtype = _ctx.Lower_ctx().Get_cipher_type(glob);
  glob->New_ret_param(rtype, sig);

  // setup parameter
  TYPE_PTR param1_ty  = _ctx.Lower_ctx().Get_cipher_type(glob);
  STR_PTR  param1_str = glob->New_str("input");
  glob->New_param(param1_str, param1_ty, sig, spos);
  sig->Set_complete();

  // setup entry
  ENTRY_PTR entry = glob->New_global_entry_point(sig, bts_func, name_str, spos);
  // set define before create a new scope
  FUNC_SCOPE*         bts_scope  = &(glob->New_func_scope(bts_func));
  air::base::STMT_PTR entry_stmt = bts_scope->Container().New_func_entry(spos);

  // Cache the function ID for this level
  _ctx.Set_bts_func(level, bts_func->Id());

  return bts_scope;
}

}  // namespace ckks
}  // namespace fhe
