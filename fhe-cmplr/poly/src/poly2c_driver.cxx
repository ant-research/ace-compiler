//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly2c_driver.h"

#include <iostream>

#include "air/base/flatten_ctx.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "nn/core/data_scheme.h"

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
    FLATTEN_CTX<TRANSFORM_UTIL> trav_ctx(&cntr, std::move(flatten_func));
    VISITOR<FLATTEN_CTX<TRANSFORM_UTIL>> trav(trav_ctx);
    NODE_PTR                             entry = func->Container().Entry_node();
    NODE_PTR                             retv  = trav.Visit<NODE_PTR>(entry);
    AIR_ASSERT(retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }

  // delete old glob
  delete glob;
  return new_glob;
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
  // mul_depth is 1 less than mul_level.
  uint32_t mul_level = param.Get_mul_level();
  AIR_ASSERT_MSG(mul_level >= 1, "mul_level must be at least 1.");
  uint32_t mul_depth = mul_level - 1;
  _ctx << mul_depth << ", ";
  _ctx << param.Get_input_level() << ", ";
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

  _ctx << "// In offline encoding scenery, data file type will be changed. "
          "Below impl need to be changed also? \n";
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

uint32_t POLY2C_DRIVER::Emit_chunk_info(NODE_PTR node, uint32_t idx) {
  uint32_t                    num_chunk = 1;
  const nn::core::DATA_CHUNK* chunk =
      nn::core::Data_scheme_attr(node, &num_chunk);
  _ctx << "  static MAP_DESC desc_" << idx << "[] = {\n";
  if (chunk != nullptr) {
    for (uint32_t i = 0; i < num_chunk; ++i) {
      _ctx << "    " << chunk[i].To_str() << ",\n";
    }
  } else {
    _ctx << "    {NORMAL, 0, 0, 0, 0}\n";
  }
  _ctx << "  };\n";
  return num_chunk;
}

void POLY2C_DRIVER::Emit_data_shape(air::base::NODE_PTR node) {
  uint32_t       dim   = 0;
  const int64_t* shape = nn::core::Data_shape_attr(node, &dim);
  if (shape != nullptr) {
    if (dim == 4) {
      _ctx << "{" << shape[0] << ", " << shape[1] << ", " << shape[2] << ", "
           << shape[3] << "}, ";
    } else if (dim == 2) {
      _ctx << "{" << shape[0] << ", " << shape[1] << ", 0, 0},";
    } else {
      AIR_ASSERT(false);
    }
  } else {
    _ctx << "{0, 0, 0, 0}, ";
  }
}

void POLY2C_DRIVER::Emit_helper_function(FUNC_SCOPE* func_scope) {
  NODE_PTR entry = func_scope->Container().Entry_stmt()->Node();
  // int Get_input_count()
  uint32_t parm_count = entry->Num_child() - 1;
  _ctx << "int Get_input_count() {\n";
  _ctx << "  return " << parm_count << ";\n";
  _ctx << "}\n\n";

  // DATA_SCHEME Get_encode_scheme()
  _ctx << "DATA_SCHEME* Get_encode_scheme(int idx) {\n";
  for (uint32_t i = 0; i < parm_count; ++i) {
    NODE_PTR formal = entry->Child(i);
    // chunk info
    uint32_t num_chunk = Emit_chunk_info(formal, i);

    // scheme
    _ctx << "  static DATA_SCHEME scheme_" << i << " = {\n";
    // input data name
    ADDR_DATUM_PTR parm = func_scope->Formal(i);
    _ctx << "    \"" << parm->Name()->Char_str() << "\", ";

    // input data shape
    Emit_data_shape(formal);

    // input data encode scheme
    _ctx << num_chunk << ", desc_" << i << "\n";
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
  STMT_LIST sl(entry->Last_child());
  NODE_PTR  retv = sl.Last_stmt()->Node();
  AIR_ASSERT(retv->Opcode() == air::core::OPC_RETV);
  // chunk info
  uint32_t num_chunk = Emit_chunk_info(retv, 0);

  // scheme
  _ctx << "  static DATA_SCHEME scheme = {\n";
  // output data name
  _ctx << "    \"" << _ctx.Output_name() << "\", ";
  // output data shape
  Emit_data_shape(retv);

  // output data decode scheme
  _ctx << num_chunk << ", desc_0\n";
  _ctx << "  };\n";
  _ctx << "  return &scheme;\n";
  _ctx << "}\n\n";
}

}  // namespace poly

}  // namespace fhe
