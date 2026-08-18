//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/core/data_scheme.h"

#include "air/base/st.h"
#include "air/core/opcode.h"
#include "nn/core/attr.h"

namespace nn {

namespace core {

void Set_input_scheme_attr(air::base::NODE_PTR input, uint32_t n_n,
                           uint32_t n_c, uint32_t n_h, uint32_t n_w) {
  AIR_ASSERT(input->Opcode() == air::core::OPC_IDNAME);
  AIR_ASSERT(input->Rtype()->Is_array());
  air::base::ARRAY_TYPE_PTR type = input->Rtype()->Cast_to_arr();
  AIR_ASSERT(type->Dim() == 4);
  std::vector<int64_t> shape     = type->Shape();
  uint32_t             tot_count = shape[0] * shape[1] * shape[2] * shape[3];
  uint32_t             num_chunk = n_n * n_c * n_h * n_w;
  AIR_ASSERT(tot_count % num_chunk == 0);
  DATA_CHUNK chunk[num_chunk];
  uint32_t   count = tot_count / num_chunk;
  for (int i = 0; i < num_chunk; ++i) {
    chunk[i].Init(DATA_CHUNK_KIND::BLOCK, i, count, i * count,
                  i * count + count - 1, 1);
  }
  input->Set_attr(ATTR::SHAPE, shape.data(), shape.size());
  input->Set_attr(ATTR::NUM_CHUNK, &num_chunk, 1);
  input->Set_attr(ATTR::DATA_SCHEME, chunk[0].Data(),
                  chunk[0].Size() * num_chunk);
}

void Set_output_scheme_attr(air::base::NODE_PTR output, uint32_t n_h,
                            uint32_t n_w) {
  AIR_ASSERT(output->Opcode() == air::core::OPC_RETV);
  AIR_ASSERT(output->Child(0)->Rtype()->Is_array());
  air::base::ARRAY_TYPE_PTR type = output->Child(0)->Rtype()->Cast_to_arr();
  AIR_ASSERT(type->Dim() == 2);
  std::vector<int64_t> shape     = type->Shape();
  uint32_t             tot_count = shape[0] * shape[1];
  uint32_t             num_chunk = n_h * n_w;
  AIR_ASSERT(tot_count % num_chunk == 0);
  DATA_CHUNK chunk[num_chunk];
  uint32_t   count = tot_count / num_chunk;
  for (int i = 0; i < num_chunk; ++i) {
    chunk[i].Init(DATA_CHUNK_KIND::BLOCK, i, count, i * count,
                  i * count + count - 1, 1);
  }
  output->Set_attr(ATTR::SHAPE, shape.data(), shape.size());
  output->Set_attr(ATTR::NUM_CHUNK, &num_chunk, 1);
  output->Set_attr(ATTR::DATA_SCHEME, chunk[0].Data(),
                   chunk[0].Size() * num_chunk);
}

}  // namespace core

}  // namespace nn
