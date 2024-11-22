//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly2c_driver.h"

#include <iostream>

#include "air/base/container.h"
#include "air/base/flatten_ctx.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ir2c_handler.h"
#include "fhe/poly/handler.h"
#include "fhe/poly/ir2c_core.h"
#include "fhe/poly/ir2c_handler.h"
#include "fhe/poly/poly2c_mfree.h"
#include "fhe/sihe/ir2c_handler.h"
#include "fhe/sihe/sihe_handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/ir2c_vector.h"

using namespace air::base;

namespace fhe {

namespace poly {

GLOB_SCOPE* POLY2C_DRIVER::Flatten(GLOB_SCOPE* glob) {
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    auto flatten_func = [](NODE_PTR node) {
      if (node->Domain() == air::core::CORE ||
          node->Opcode() == nn::vector::OPC_SLICE) {
        return false;
      }
      return true;
    };
    FLATTEN_CTX          trav_ctx(&cntr, std::move(flatten_func));
    VISITOR<FLATTEN_CTX> trav(trav_ctx);
    NODE_PTR             entry = func->Container().Entry_node();
    NODE_PTR             retv  = trav.Visit<NODE_PTR>(entry);
    AIR_ASSERT(retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }

  // delete old glob
  delete glob;
  return new_glob;
}

void POLY2C_DRIVER::Run(GLOB_SCOPE* glob) {
  _ctx.Emit_global_include();
  _ctx.Emit_global_constants(glob, true);

  // emit function prototype
  for (FUNC_ITER it = glob->Begin_func(); it != glob->End_func(); ++it) {
    if ((*it)->Entry_point()->Is_program_entry()) {
      continue;
    }
    _ctx.Emit_func_sig((*it));
    _ctx << ";\n";
  }
  _ctx << "\n";

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    NODE_PTR    body = func->Container().Stmt_list().Block_node();

    if (_ctx.Free_poly()) {
      // insert free before emit C code
      fhe::poly::MFREE_PASS mfree(_ctx.Lower_ctx());
      mfree.Perform(body);
    }

    // emit C code
    air::base::VISITOR<fhe::poly::IR2C_CTX,
                       air::core::HANDLER<fhe::poly::IR2C_CORE>,
                       nn::vector::HANDLER<nn::vector::IR2C_VECTOR>,
                       fhe::sihe::HANDLER<fhe::sihe::IR2C_HANDLER>,
                       fhe::ckks::HANDLER<fhe::ckks::IR2C_HANDLER>,
                       fhe::poly::HANDLER<fhe::poly::IR2C_HANDLER> >
        visitor(_ctx);

    _ctx.Emit_func_def(func);
    _ctx.Begin_func_body(body);
    _ctx.Emit_local_var(func);
    visitor.template Visit<void>(body);
    _ctx.End_func_body(body);
    _ctx << "\n";

    if (func->Owning_func()->Entry_point()->Is_program_entry()) {
      Emit_helper_function(func);
    }
  }

  Emit_get_context_params();

  _ctx.Emit_global_constants(glob, false);
}

void POLY2C_DRIVER::Emit_get_context_params() {
  const core::CTX_PARAM&   param    = _ctx.Lower_ctx().Get_ctx_param();
  const std::set<int32_t>& rot_keys = param.Get_rotate_index();
  // CKKS_PARAMS Get_context_params()
  _ctx << "CKKS_PARAMS* Get_context_params() {\n";
  _ctx << "  static CKKS_PARAMS parm = {\n";
  _ctx << "    ";
  _ctx << fhe::core::Provider_name(_ctx.Provider());
  _ctx << ", " << param.Get_poly_degree() << ", ";
  _ctx << param.Get_security_level() << ", ";
  _ctx << param.Get_mul_level() << ", ";
  _ctx << param.Get_first_prime_bit_num() << ", ";
  _ctx << param.Get_scaling_factor_bit_num() << ", ";
  _ctx << param.Get_q_part_num() << ", ";
  _ctx << param.Get_hamming_weight() << ", ";
  _ctx << rot_keys.size() << ", \n";
  _ctx << "    { ";
  int i = 0;
  for (auto it = rot_keys.begin(); it != rot_keys.end(); ++it) {
    if (i > 0) {
      if ((i % 8) == 0) {
        _ctx << ",\n      ";
      } else {
        _ctx << ", ";
      }
    }
    _ctx << (*it);
    ++i;
  }
  _ctx << " }\n";
  _ctx << "  };\n";
  _ctx << "  return &parm;\n";
  _ctx << "}\n\n";

  _ctx << "RT_DATA_INFO* Get_rt_data_info() {\n";
  if (_ctx.Emit_data_file()) {
    _ctx << "  static RT_DATA_INFO info = {\n";
    _ctx << "    \"" << _ctx.Data_file() << "\",\n";
    _ctx << "    \"" << _ctx.Data_file_uuid() << "\",\n";
    _ctx << "    " << Data_entry_name(_ctx.Data_entry_type()) << "\n";
    _ctx << "  };\n";
    _ctx << "  return &info;\n";
  } else {
    _ctx << "  return NULL;\n";
  }
  _ctx << "}\n\n";
}

void POLY2C_DRIVER::Emit_helper_function(FUNC_SCOPE* func_scope) {
  // int Get_input_count()
  uint32_t parm_count =
      func_scope->Container().Entry_stmt()->Node()->Num_child() - 1;
  _ctx << "int Get_input_count() {\n";
  _ctx << "  return " << parm_count << ";\n";
  _ctx << "}\n\n";

  // DATA_SCHEME Get_encode_scheme()
  _ctx << "DATA_SCHEME* Get_encode_scheme(int idx) {\n";
  for (uint32_t i = 0; i < parm_count; ++i) {
    _ctx << "  static DATA_SCHEME scheme_" << i << " = {\n";
    // input data name
    ADDR_DATUM_PTR parm = func_scope->Formal(i);
    _ctx << "    \"" << parm->Name()->Char_str() << "\", ";
    // input data shape
    _ctx << "{0, 0, 0, 0}, ";
    // input data encode scheme
    _ctx << "1, {NORMAL, 0, 0, 0, 0}\n";
    _ctx << "  };\n";
  }
  _ctx << "  static DATA_SCHEME* scheme[] = { ";
  for (uint32_t i = 0; i < parm_count; ++i) {
    if (i > 0) {
      _ctx << ", ";
    }
    _ctx << "&scheme_" << i;
  }
  _ctx << " };\n";
  _ctx << "  return scheme[idx];\n";
  _ctx << "}\n\n";

  // int Get_output_count()
  _ctx << "int Get_output_count() {\n";
  _ctx << "  return 1;\n";
  _ctx << "}\n\n";

  // DATA_SCHEME Get_decode_scheme()
  _ctx << "DATA_SCHEME* Get_decode_scheme(int idx) {\n";
  _ctx << "  static DATA_SCHEME scheme = {\n";
  // output data name
  _ctx << "    \"" << _ctx.Output_name() << "\", ";
  // output data shape
  _ctx << "{0, 0, 0, 0}, ";
  // output data decode scheme
  _ctx << "1, {NORMAL, 0, 0, 0, 0}\n";
  _ctx << "  };\n";
  _ctx << "  return &scheme;\n";
  _ctx << "}\n\n";
}

}  // namespace poly

}  // namespace fhe
