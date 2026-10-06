//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "pycontext.h"

#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "nn/core/attr.h"
#include "nn/vector/vector_utils.h"

namespace air::dsl {

PYCONTEXT::PYCONTEXT(NODE_PTR node) : _cur_node(node) {}

// -------- Type relevant APIs --------
std::vector<int64_t> PYCONTEXT::Rtype(NODE_PTR node) {
  AIR_ASSERT_MSG(node->Rtype()->Is_array(),
                 "conv new_input is not an array type");
  return node->Rtype()->Cast_to_arr()->Shape();
}

TYPE_PTR PYCONTEXT::Get_elem_type(NODE_PTR node) {
  AIR_ASSERT_MSG(node->Rtype()->Is_array(),
                 "conv new_input is not an array type");
  return node->Rtype()->Cast_to_arr()->Elem_type();
}

CONST_TYPE_PTR PYCONTEXT::Get_elem_const_type(NODE_PTR node) {
  AIR_ASSERT_MSG(node->Rtype()->Is_array(),
                 "conv new_input is not an array type");
  return node->Rtype()->Cast_to_arr()->Elem_type();
}

PYCONTEXT::NCHW PYCONTEXT::Get_array_nchw(TYPE_PTR ty) {
  AIR_ASSERT_MSG(ty->Is_array(), "conv new_input is not an array type");
  int64_t n = 0, c = 0, h = 0, w = 0;
  nn::vector::Get_array_nchw(ty, n, c, h, w);
  return std::make_tuple(n, c, h, w);
}

// -------- Constant relevant APIs --------

std::vector<float> PYCONTEXT::Get_float_const_array(NODE_PTR node) {
  AIR_ASSERT_MSG(node->Opcode() == core::OPC_LDC, "node is not a constant");
  AIR_ASSERT_MSG(node->Rtype()->Is_array(), "node is not an array constant");
  CONSTANT_PTR cst = node->Const();
  // cst->Print();
  AIR_ASSERT(cst->Kind() == base::CONSTANT_KIND::ARRAY);
  AIR_ASSERT(cst->Type()->Is_array());

  ARRAY_TYPE_PTR cst_type   = cst->Type()->Cast_to_arr();
  size_t         array_size = cst->Array_byte_len() / sizeof(float);

  // printf("%d\n", cst_type->Dim());
  // printf("array size: %lu\n", array_size);
  // printf("elem cnt: %lu\n", cst_type->Elem_count());

  std::vector<float> res;
  const float*       arr = cst->Array_ptr<float>();
  for (size_t i = 0; i < array_size; i++) {
    res.push_back(arr[i]);
  }

  return res;
}

// -------- Attribute relevant APIs --------

std::vector<int> PYCONTEXT::Strides(NODE_PTR node) {
  return nn::vector::Get_attr_data<int>(node, nn::core::ATTR::STRIDE);
}

std::vector<int> PYCONTEXT::Pads(NODE_PTR node) {
  return nn::vector::Get_attr_data<int>(node, nn::core::ATTR::PAD);
}

std::vector<int> PYCONTEXT::Group(NODE_PTR node) {
  return nn::vector::Get_attr_data<int>(node, nn::core::ATTR::GROUP);
}

NODE_PTR PYCONTEXT::Get_cur_node() { return _cur_node; }

}  // namespace air::dsl
