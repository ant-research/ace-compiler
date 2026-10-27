//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks2py_driver.h"

#include <iostream>

#include "air/base/flatten_ctx.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "fhe/poly/opcode.h"
#include "nn/core/data_scheme.h"

using namespace air::base;

namespace fhe {

namespace ckks {

// Reused verbatim from POLY2C_DRIVER::Flatten (poly/src/poly2c_driver.cxx:26-67).
// It is independent of the CTX/provider: it only rewrites the glob scope by
// flattening CORE / OPC_SLICE / poly-HW nodes. The non-ant path (poly lowering
// disabled) requires this before emission, so the Python backend reuses it.
GLOB_SCOPE* CKKS2PY_DRIVER::Flatten(GLOB_SCOPE* glob) {
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
      // Do not flatten HW_* ops that are SET_COEFFS value; IR2C emits
      // Coeffs(...) as first arg only when value is direct HW_*.
      if (node->Domain() == fhe::poly::POLYNOMIAL_DID) {
        air::base::OPCODE opc = node->Opcode();
        if (opc == fhe::poly::OPC_HW_MODADD ||
            opc == fhe::poly::OPC_HW_MODSUB ||
            opc == fhe::poly::OPC_HW_MODMUL ||
            opc == fhe::poly::OPC_HW_ROTATE) {
          return false;
        }
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

// Emit `def get_context_params(): return { ... }` -- a Python dict mirroring
// the C `CKKS_PARAMS Get_context_params()` fields (poly2c_driver.cxx:69-122).
// Only the context-parameter face is emitted in the MVP; the full
// runtime-helper face (Get_encode_scheme / Get_decode_scheme / Get_rt_data_info)
// is Phase 2.
void CKKS2PY_DRIVER::Emit_get_context_params() {
  const fhe::core::CTX_PARAM&   param     = _ctx.Lower_ctx().Get_ctx_param();
  const std::set<int32_t>&      rot_keys  = param.Get_rotate_index();
  uint32_t                      mul_level = param.Get_mul_level();
  // mul_depth is 1 less than mul_level (mirrors the C driver); guard against
  // mul_level == 0 instead of asserting, so trivial/early test IR compiles.
  uint32_t mul_depth = (mul_level > 0) ? (mul_level - 1) : 0;
  _ctx << "def get_context_params():\n";
  _ctx << "  return {\n";
  _ctx << "    'provider': '" << _ctx.Provider_tag() << "',\n";
  _ctx << "    'poly_degree': " << param.Get_poly_degree() << ",\n";
  _ctx << "    'security_level': " << param.Get_security_level() << ",\n";
  _ctx << "    'mul_depth': " << mul_depth << ",\n";
  _ctx << "    'input_level': " << param.Get_input_level() << ",\n";
  _ctx << "    'first_prime_bit_num': " << param.Get_first_prime_bit_num()
       << ",\n";
  _ctx << "    'scaling_factor_bit_num': "
       << param.Get_scaling_factor_bit_num() << ",\n";
  _ctx << "    'q_part_num': " << param.Get_q_part_num() << ",\n";
  _ctx << "    'hamming_weight': " << param.Get_hamming_weight() << ",\n";
  _ctx << "    'rotate_keys': [";
  bool first = true;
  for (auto it = rot_keys.begin(); it != rot_keys.end(); ++it) {
    if (!first) {
      _ctx << ", ";
    }
    _ctx << (*it);
    first = false;
  }
  _ctx << "],\n";
  // Phase 3 wiring: 'need_bts' is True when Emit_bootstrap_op set _need_bts
  // (i.e. ReSBM inserted OPC_BOOTSTRAP). rt_openfhe gates _build_bootstrap_context
  // on ctx.get('need_bts').
  _ctx << "    'need_bts': " << (_ctx._need_bts ? "True" : "False") << ",\n";
  _ctx << "  }\n\n";
}

// Emit `def get_rt_data_info()` returning a dict {file, uuid, entry_type}
// mirroring C Get_rt_data_info (poly2c_driver.cxx:108-121), or None when no
// data file (`-CKKS2PY:df=` not given -- mirrors C returning NULL). rt_ref reads
// the `file` path to locate the C binary side data file for offline encode
// (load_data_entry); the uuid/entry_type are parity metadata (Phase 2.3 (a)).
void CKKS2PY_DRIVER::Emit_rt_data_info() {
  _ctx << "def get_rt_data_info():\n";
  if (_ctx.Emit_data_file()) {
    _ctx << "  return {\n";
    _ctx << "    'file': '" << _ctx.Data_file() << "',\n";
    _ctx << "    'uuid': '" << _ctx.Data_file_uuid() << "',\n";
    _ctx << "    'entry_type': '"
         << fhe::core::Data_entry_name(_ctx.Data_entry_type()) << "',\n";
    _ctx << "  }\n\n";
  } else {
    _ctx << "  return None\n\n";
  }
}

// Emit `def get_input_count()` / `def get_output_count()` + `get_encode_scheme
// (idx)` / `get_decode_scheme(idx)` for the program entry, mirroring C
// Emit_helper_function (poly2c_driver.cxx:157-220). The encode/decode scheme
// dicts mirror the C DATA_SCHEME structs (name/shape[4]/num_chunk/desc[]).
// rt_ref does not consume them (offline encode reads by index via
// load_data_entry) -- they are parity metadata for the C rt-data face (plan
// section 2.3 (a)).
void CKKS2PY_DRIVER::Emit_helper_function(air::base::FUNC_SCOPE* func_scope) {
  air::base::NODE_PTR entry      = func_scope->Container().Entry_stmt()->Node();
  uint32_t            parm_count = entry->Num_child() - 1;  // input formals
  _ctx << "def get_input_count():\n";
  _ctx << "  return " << parm_count << "\n\n";
  _ctx << "def get_output_count():\n";
  _ctx << "  return 1\n\n";

  // get_encode_scheme(idx): list of per-input scheme dicts; returns schemes[idx]
  // (mirrors C Get_encode_scheme returning scheme[idx]).
  _ctx << "def get_encode_scheme(idx):\n";
  _ctx << "  schemes = [\n";
  for (uint32_t i = 0; i < parm_count; ++i) {
    air::base::NODE_PTR       formal = entry->Child(i);
    air::base::ADDR_DATUM_PTR parm   = func_scope->Formal(i);
    _ctx << "    ";
    Emit_data_scheme_py(parm->Name()->Char_str(), formal);
    _ctx << ",\n";
  }
  _ctx << "  ]\n";
  _ctx << "  return schemes[idx]\n\n";

  // get_decode_scheme(idx): the single output scheme dict (idx ignored; one
  // output -- mirrors C Get_decode_scheme returning &scheme).
  air::base::STMT_LIST sl(entry->Last_child());
  air::base::NODE_PTR  retv = sl.Last_stmt()->Node();
  AIR_ASSERT(retv->Opcode() == air::core::OPC_RETV);
  _ctx << "def get_decode_scheme(idx):\n";
  _ctx << "  return ";
  Emit_data_scheme_py(_ctx.Output_name(), retv);
  _ctx << "\n\n";
}

// Emit a Python 4-int shape list `[n, c, h, w]` mirroring C Emit_data_shape
// (poly2c_driver.cxx:140-155): dim==4 -> {s0,s1,s2,s3}; dim==2 -> {s0,s1,1,1};
// no shape attr -> [0, 0, 0, 0].
void CKKS2PY_DRIVER::Emit_data_shape_py(air::base::NODE_PTR node) {
  uint32_t       dim   = 0;
  const int64_t* shape = nn::core::Data_shape_attr(node, &dim);
  _ctx << "[";
  if (shape != nullptr) {
    if (dim == 4) {
      _ctx << shape[0] << ", " << shape[1] << ", " << shape[2] << ", "
           << shape[3];
    } else if (dim == 2) {
      _ctx << shape[0] << ", " << shape[1] << ", 1, 1";
    } else {
      AIR_ASSERT(false);
    }
  } else {
    _ctx << "0, 0, 0, 0";
  }
  _ctx << "]";
}

// Emit one Python DATA_SCHEME dict `{'name': ..., 'shape': [4], 'num_chunk':
// N, 'desc': [...]}` mirroring C Get_encode_scheme/Get_decode_scheme's per-
// scheme struct (poly2c_driver.cxx:172-183,210-217) + DATA_CHUNK::To_str
// (data_scheme.cxx:21-34), which prints `{ NORMAL, count, start, pad, stride }`
// (kind always NORMAL). `node` is an input formal or the retv.
void CKKS2PY_DRIVER::Emit_data_scheme_py(const std::string& name,
                                         air::base::NODE_PTR node) {
  uint32_t                     num_chunk = 1;
  const nn::core::DATA_CHUNK* chunk =
      nn::core::Data_scheme_attr(node, &num_chunk);
  _ctx << "{'name': '" << name << "', 'shape': ";
  Emit_data_shape_py(node);
  _ctx << ", 'num_chunk': " << num_chunk << ", 'desc': [";
  if (chunk != nullptr) {
    for (uint32_t i = 0; i < num_chunk; ++i) {
      if (i > 0) _ctx << ", ";
      _ctx << "{'kind': 'NORMAL', 'count': " << chunk[i].Count()
           << ", 'start': " << chunk[i].Start() << ", 'pad': " << chunk[i].Pad()
           << ", 'stride': " << chunk[i].Stride() << "}";
    }
  } else {
    _ctx << "{'kind': 'NORMAL', 'count': 0, 'start': 0, 'pad': 0, 'stride': 0}";
  }
  _ctx << "]}";
}

}  // namespace ckks

}  // namespace fhe