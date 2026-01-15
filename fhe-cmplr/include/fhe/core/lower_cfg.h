//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_LOWER_CFG_H
#define FHE_CORE_LOWER_CFG_H

#include <sys/types.h>

#include <cstdint>
#include <functional>
#include <nlohmann/json.hpp>
#include <set>
#include <unordered_map>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/opcode.h"
#include "fhe/core/rt_context.h"
#include "fhe/core/scheme_info.h"
#include "nn/core/data_scheme.h"

using json = nlohmann::json;

namespace fhe {
namespace core {

//! Configure file
class LOWER_CFG {
public:
  LOWER_CFG() {}

  // Add or update a JSON section
  void Update_section(const std::string& key, const json& value) {
    _data[key] = value;
  }

  // Get the entire JSON data as a string
  std::string Get_str() const {
    return _data.dump(4);  // Pretty print with 4 spaces indent
  }

  // Dump Configure to JSON file
  void Dump(std::ostream& ofile) const { ofile << _data.dump(4); }

  void Emit_cfg(air::base::FUNC_SCOPE*      func_scope,
                const fhe::core::CTX_PARAM& param) {
    air::base::NODE_PTR entry = func_scope->Container().Entry_stmt()->Node();
    uint32_t            parm_count = entry->Num_child() - 1;

    Update_section("input_count", parm_count);
    Update_section("output_count", 1);
    Update_section("context_params", Emit_context_params(param));
    Update_section("rt_data_info", Emit_rt_data_info());
    Update_section("encode_schemes", Emit_encode_schemes(func_scope));
    Update_section("decode_schemes", Emit_decode_schemes(func_scope));
  }

private:
  json _data;

  json Emit_context_params(const fhe::core::CTX_PARAM& param) {
    const std::set<int32_t>& rot_keys = param.Get_rotate_index();

    uint32_t mul_level = param.Get_mul_level();
    AIR_ASSERT_MSG(mul_level >= 1, "mul_level must be at least 1.");
    uint32_t mul_depth = mul_level - 1;

    // _provider : fhe::core::Provider_name(Provider())}
    return {
        {"_provider",         0                                 },
        {"_poly_degree",      param.Get_poly_degree()           },
        {"_sec_level",        param.Get_security_level()        },
        {"_mul_depth",        mul_depth                         },
        {"_input_level",      param.Get_input_level()           },
        {"_first_mod_size",   param.Get_first_prime_bit_num()   },
        {"_scaling_mod_size", param.Get_scaling_factor_bit_num()},
        {"_num_q_parts",      param.Get_q_part_num()            },
        {"_hamming_weight",   param.Get_hamming_weight()        },
        {"_num_rot_idx",      rot_keys.size()                   },
        {"_rot_idxs",         json::array()                     }
    };
  }

  //! TODO: The weight file needs to be generated when the assembly is
  //! generated, But the current driver implementation cannot support it.
  //! This will require a change to Driver's implementation and will be
  //! implemented later.
  //! _file_name : _ctx.Data_file() _file_uuid :
  //! _ctx.Data_file_uuid() _entry_type :
  //! Data_entry_name(_ctx.Data_entry_type())}
  json Emit_rt_data_info() {
    return {
        {"_file_name",  "data.weight"                         },
        {"_file_uuid",  "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"},
        {"_entry_type", 0                                     }  // DE_MSG_F32
    };
  }

  //! @brief data shape
  json Emit_data_shape(air::base::NODE_PTR node) {
    std::vector<int> vec(4);
    uint32_t         dim   = 0;
    const int64_t*   shape = nn::core::Data_shape_attr(node, &dim);
    if (shape != nullptr) {
      if (dim == 4) {
        vec[0] = shape[0];
        vec[1] = shape[1];
        vec[2] = shape[2];
        vec[3] = shape[3];
      } else if (dim == 2) {
        vec[0] = shape[0];
        vec[1] = shape[1];
        vec[2] = 0;
        vec[3] = 0;
      } else {
        AIR_ASSERT(false);
      }
    } else {
      vec[0] = 0;
      vec[1] = 0;
      vec[2] = 0;
      vec[3] = 0;
    }

    return vec;
  }

  //! @brief scheme chunk info
  json Emit_chunk_info(air::base::NODE_PTR node, uint32_t idx) {
    json                        desc;
    uint32_t                    num_chunk = 1;
    const nn::core::DATA_CHUNK* chunk =
        nn::core::Data_scheme_attr(node, &num_chunk);
    if (chunk != nullptr) {
      for (uint32_t i = 0; i < num_chunk; ++i) {
        desc[chunk[i].To_str()] = 0;
      }
    } else {
      desc["_kind"]   = 0;
      desc["_count"]  = 0;
      desc["_start"]  = 0;
      desc["_pad"]    = 0;
      desc["_stride"] = 0;
    }
    return json::array({desc});
  }

  //! @brief input data encode scheme
  json Emit_encode_schemes(air::base::FUNC_SCOPE* func_scope) {
    std::vector<json>   scheme;
    air::base::NODE_PTR entry = func_scope->Container().Entry_stmt()->Node();
    uint32_t            parm_count = entry->Num_child() - 1;

    for (uint32_t i = 0; i < parm_count; ++i) {
      air::base::ADDR_DATUM_PTR parm   = func_scope->Formal(i);
      air::base::NODE_PTR       formal = entry->Child(i);
      // chunk info
      uint32_t num_chunk = 1;
      nn::core::Data_scheme_attr(formal, &num_chunk);

      json obj = {
          {"_name", parm->Name()->Char_str()},
          {"_shape", Emit_data_shape(formal)},
          {"_count", num_chunk},
          {"_desc", Emit_chunk_info(formal, i)}
      };
      scheme.push_back(obj);
    }

    return scheme;
  }

  //! @brief output data decode scheme
  json Emit_decode_schemes(air::base::FUNC_SCOPE* func_scope) {
    air::base::NODE_PTR  entry = func_scope->Container().Entry_stmt()->Node();
    air::base::STMT_LIST sl(entry->Last_child());
    air::base::NODE_PTR  retv = sl.Last_stmt()->Node();
    AIR_ASSERT(retv->Opcode() == air::core::OPC_RETV);
    // chunk info
    uint32_t num_chunk = 1;
    nn::core::Data_scheme_attr(retv, &num_chunk);

    return json::array({
        {{"_name", "output"},
         {"_shape", Emit_data_shape(retv)},
         {"_count", num_chunk},
         {"_desc", Emit_chunk_info(retv, 0)}}
    });
  }

};  // namespace fhe

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_LOWER_CFG_H
