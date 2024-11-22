//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_RT_DATA_MGR_H
#define FHE_CORE_RT_DATA_MGR_H

#include "fhe/core/rt_data_writer.h"
#include "fhe/core/rt_encode_api.h"

namespace fhe {

namespace core {

//! @brief External plaintext data manager
class RT_DATA_MGR {
public:
  //! @brief Construct a plaintext data manager
  RT_DATA_MGR(const fhe::core::LOWER_CTX&     lower_ctx,
              const fhe::poly::POLY2C_CONFIG& cfg)
      : _rt_data_writer(nullptr) {
    if (cfg.Emit_data_file()) {
      // prepare encode context
      const core::CTX_PARAM& param = lower_ctx.Get_ctx_param();
      Prepare_encode_context(param.Get_poly_degree(),
                             param.Get_security_level(), param.Get_mul_level(),
                             param.Get_first_prime_bit_num(),
                             param.Get_scaling_factor_bit_num());

      // create rt_data_writer
      _data_file_uuid = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
      _data_entry_type =
          cfg.Ct_encode() ? fhe::core::DE_PLAINTEXT : fhe::core::DE_MSG_F32;
      _rt_data_writer =
          new fhe::core::RT_DATA_WRITER(cfg.Data_file(), _data_entry_type,
                                        cfg.Ifile(), _data_file_uuid.c_str());
    }
  }

  //! @brief Destruct ir2c ctx object
  ~RT_DATA_MGR() {
    if (_rt_data_writer != nullptr) {
      delete _rt_data_writer;
      Finalize_encode_context();
    }
  }

  uint64_t Append(air::base::CONSTANT_PTR cst, int64_t start, int64_t len,
                  int scale = 0, int level = 0) {
    AIR_ASSERT(_rt_data_writer != nullptr);
    AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
    AIR_ASSERT(cst->Type()->Is_array());
    AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_type()->Is_prim());
    AIR_ASSERT(
        cst->Type()->Cast_to_arr()->Elem_type()->Cast_to_prim()->Encoding() ==
        air::base::PRIMITIVE_TYPE::FLOAT_32);
    char name[32];
    snprintf(name, 32, "cst_%d_%d", cst->Id().Value(), (int)start);
    const float* data = (const float*)cst->Array_buffer();
    AIR_ASSERT(start >= 0 && len > 0 &&
               start + len <= cst->Array_byte_len() / sizeof(float));
    uint64_t idx;
    if (_data_entry_type == fhe::core::DE_PLAINTEXT) {
      PLAINTEXT_BUFFER* buf =
          Encode_plain_buffer(data + start, len, scale, level);
      idx = _rt_data_writer->Append_pt(name, (const char*)buf,
                                       Plain_buffer_length(buf));
      Free_plain_buffer(buf);
    } else {
      idx = _rt_data_writer->Append(name, data + start, len);
    }
    return idx;
  }

  const char* Data_file_uuid() const { return _data_file_uuid.c_str(); }

  core::DATA_ENTRY_TYPE Data_entry_type() const { return _data_entry_type; }

private:
  fhe::core::RT_DATA_WRITER* _rt_data_writer;
  std::string                _data_file_uuid;
  fhe::core::DATA_ENTRY_TYPE _data_entry_type;
};  // RT_DATA_MGR

}  // namespace core

}  // namespace fhe

#endif  // FHE_CORE_RT_DATA_MGR_H
