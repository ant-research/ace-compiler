//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/air_utils.h"

#include "nn/onnx2air/air_gen.h"
#include "nn/onnx2air/air_sym.h"

namespace nn {
namespace onnx2air {

TYPE_PTR
Create_array_type(TYPE_PTR elem_ty, std::vector<int>& data_dim,
                  GLOB_SCOPE* glob) {
  STR_PTR type_name = glob->Undefined_name();
  SPOS    spos      = glob->Unknown_simple_spos();

  ARB_PTR arb_tail, arb_head;
  for (int i = 0; i < data_dim.size(); ++i) {
    ARB_PTR arb = glob->New_arb(i + 1, 0, data_dim[i], 1);
    if (i != 0) {
      arb_tail->Set_next(arb->Id());
    } else {
      arb_head = arb;
    }
    arb_tail = arb;
  }

  TYPE_PTR a_type = glob->New_arr_type(type_name, elem_ty, arb_head, spos);

  return a_type;
}

TYPE_PTR
Create_tensor_type(TYPE_PTR base_type_ptr, std::vector<int>& data_dim,
                   GLOB_SCOPE* glob) {
  TYPE_PTR array_type_ptr = Create_array_type(base_type_ptr, data_dim, glob);
  return array_type_ptr;
}
}  // namespace onnx2air
}  // namespace nn
