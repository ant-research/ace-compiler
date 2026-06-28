//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_LOWER_CONF_H
#define FHE_CORE_LOWER_CONF_H

#include <sys/types.h>

#include <cstdint>
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <unordered_map>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/opcode.h"
#include "fhe/core/rt_context.h"
#include "fhe/core/scheme_info.h"
#include "nn/core/data_scheme.h"

#define JSON_INDENT (4)

using json = nlohmann::json;

namespace fhe {
namespace core {

//! @brief Configure file
class LOWER_CONF {
public:
  LOWER_CONF() = default;

  void Set_provider(uint32_t p) { _provider = p; }
  uint32_t Get_provider() const { return _provider; }

  //! @brief Generate config as JSON string
  std::string To_string(air::base::FUNC_SCOPE*      func_scope,
                        const fhe::core::CTX_PARAM& param,
                        const std::string&          data_file,
                        uint32_t                    provider = 0) const {
    _provider  = provider;
    _data_file = data_file;
    return Generate_conf(func_scope, param).dump(JSON_INDENT);
  }

  //! @brief Write config to file
  void To_file(const std::string& name, air::base::FUNC_SCOPE* func_scope,
               const fhe::core::CTX_PARAM& param,
               const std::string&          data_file,
               uint32_t                    provider = 0) const {
    _provider  = provider;
    _data_file = data_file;
    std::ofstream ofile(name);
    if (!ofile.is_open()) {
      CMPLR_USR_MSG(U_CODE::Output_File_Open_Err_Fatal, name);
    }
    ofile << Generate_conf(func_scope, param).dump(JSON_INDENT);
    ofile.close();
  }

private:
  mutable uint32_t  _provider  = 0;
  mutable std::string _data_file;

  json Emit_context_params(const fhe::core::CTX_PARAM& param) const {
    const std::set<int32_t>& rot_keys  = param.Get_rotate_index();
    uint32_t                 mul_level = param.Get_mul_level();
    AIR_ASSERT_MSG(mul_level >= 1, "mul_level must be at least 1.");
    uint32_t mul_depth = mul_level - 1;

    return {
        {"_provider",         _provider                          },
        {"_poly_degree",      param.Get_poly_degree()           },
        {"_sec_level",        param.Get_security_level()        },
        {"_mul_depth",        mul_depth                         },
        {"_input_level",      param.Get_input_level()           },
        {"_first_mod_size",   param.Get_first_prime_bit_num()   },
        {"_scaling_mod_size", param.Get_scaling_factor_bit_num()},
        {"_num_q_parts",      param.Get_q_part_num()            },
        {"_hamming_weight",   param.Get_hamming_weight()        },
        {"_num_rot_idx",      rot_keys.size()                   },
        {"_rot_idxs",         rot_keys                          }
    };
  }

  //! @brief Emit runtime data info
  json Emit_rt_data_info() const {
    return {
        {"_file_name",  _data_file                              },
        {"_file_uuid",  "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"},
        {"_entry_type", 0                                       }  // DE_MSG_F32
    };
  }

  //! @brief data shape
  json Emit_data_shape(air::base::NODE_PTR node) const {
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
        vec[2] = 1;
        vec[3] = 1;
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
  json Emit_chunk_info(air::base::NODE_PTR node, uint32_t idx) const {
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
  json Emit_encode_schemes(air::base::FUNC_SCOPE* func_scope) const {
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
  json Emit_decode_schemes(air::base::FUNC_SCOPE* func_scope) const {
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

  json Generate_conf(air::base::FUNC_SCOPE*      func_scope,
                    const fhe::core::CTX_PARAM& param) const {
    air::base::NODE_PTR entry = func_scope->Container().Entry_stmt()->Node();
    uint32_t            parm_count = entry->Num_child() - 1;

    json data;
    data["input_count"]    = parm_count;
    data["output_count"]   = 1;
    data["context_params"] = Emit_context_params(param);
    data["rt_data_info"]   = Emit_rt_data_info();
    data["encode_schemes"] = Emit_encode_schemes(func_scope);
    data["decode_schemes"] = Emit_decode_schemes(func_scope);
    return data;
  }
};

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_LOWER_CONF_H
