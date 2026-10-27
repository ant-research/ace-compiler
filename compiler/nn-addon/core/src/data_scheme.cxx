//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/core/data_scheme.h"

#include <sstream>

#include "air/base/st.h"
#include "air/core/opcode.h"
#include "nn/core/attr.h"

namespace nn {

namespace core {

void DATA_CHUNK::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << "{ NORMAL, " << _val[2] << ", " << _val[3] << ", " << _val[4] << ", "
     << _val[5] << "}";
}

void DATA_CHUNK::Print() const { Print(std::cout, 0); }

std::string DATA_CHUNK::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void Set_scheme_attr(air::base::NODE_PTR node, air::base::TYPE_PTR type,
                     uint32_t n_n, uint32_t n_c, uint32_t n_h, uint32_t n_w,
                     uint32_t o_h, uint32_t o_w) {
  AIR_ASSERT(node->Opcode() == air::core::OPC_IDNAME ||
             node->Opcode() == air::core::OPC_RETV);
  if (node->Has_rtype()) {
    AIR_ASSERT(node->Rtype()->Is_array());
  }
  AIR_ASSERT(type->Is_array());
  AIR_ASSERT(type->Cast_to_arr()->Dim() == 4 ||
             type->Cast_to_arr()->Dim() == 2);
  AIR_ASSERT_MSG(n_w == 1 && o_w == 0, "TODO: n_w != 1");
  std::vector<int64_t> shape     = type->Cast_to_arr()->Shape();
  uint32_t             tot_count = 0;
  int64_t              iw        = 0;
  if (shape.size() == 4) {
    tot_count = shape[0] * shape[1] * shape[2] * shape[3];
    iw        = shape[3];
  } else if (shape.size() == 2) {
    tot_count = shape[0] * shape[1];
    iw        = shape[1];
  } else {
    AIR_ASSERT(false);
  }
  uint32_t num_chunk = n_n * n_c * n_h * n_w;
  AIR_ASSERT(tot_count % num_chunk == 0);
  DATA_CHUNK chunk[num_chunk];
  uint32_t   count = tot_count / num_chunk;
  for (int i = 0; i < num_chunk; ++i) {
    uint32_t start = i * count;
    uint32_t pad   = o_h * iw;
    chunk[i].Init(DATA_CHUNK_KIND::BLOCK, i, count, start, pad, 1);
  }
  node->Set_attr(ATTR::SHAPE, shape.data(), shape.size());
  node->Set_attr(ATTR::NUM_CHUNK, &num_chunk, 1);
  node->Set_attr(ATTR::DATA_SCHEME, chunk[0].Data(),
                 chunk[0].Size() * num_chunk);
}

uint32_t Num_data_chunk(air::base::NODE_PTR node) {
  AIR_ASSERT(node->Opcode() == air::core::OPC_IDNAME ||
             node->Opcode() == air::core::OPC_RETV);
  uint32_t        elem_count = 0;
  const uint32_t* num_ptr = node->Attr<uint32_t>(ATTR::NUM_CHUNK, &elem_count);
  if (num_ptr == nullptr) {
    return 0;
  }
  AIR_ASSERT(elem_count == 1);
  return *num_ptr;
}

const DATA_CHUNK* Data_scheme_attr(air::base::NODE_PTR node, uint32_t* count) {
  AIR_ASSERT(node->Opcode() == air::core::OPC_IDNAME ||
             node->Opcode() == air::core::OPC_RETV);
  uint32_t        elem_count = 0;
  const uint32_t* num_ptr = node->Attr<uint32_t>(ATTR::NUM_CHUNK, &elem_count);
  if (num_ptr == nullptr) {
    return nullptr;
  }
  AIR_ASSERT(elem_count == 1);
  const uint32_t* data_ptr =
      node->Attr<uint32_t>(ATTR::DATA_SCHEME, &elem_count);
  AIR_ASSERT(data_ptr != nullptr);
  AIR_ASSERT(elem_count == (*num_ptr) * sizeof(DATA_CHUNK) / sizeof(uint32_t));
  if (count) {
    *count = *num_ptr;
  }

  return (const DATA_CHUNK*)data_ptr;
}

const int64_t* Data_shape_attr(air::base::NODE_PTR node, uint32_t* dim) {
  AIR_ASSERT(node->Opcode() == air::core::OPC_IDNAME ||
             node->Opcode() == air::core::OPC_RETV);
  return node->Attr<int64_t>(ATTR::SHAPE, dim);
}

}  // namespace core

}  // namespace nn
