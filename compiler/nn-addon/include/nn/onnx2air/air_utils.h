//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTILS_H
#define AIR_UTILS_H

#include "nn/onnx2air/air_gen.h"

namespace nn {
namespace onnx2air {

TYPE_PTR Get_tensor_ty(ADDR_DATUM_PTR ADDR_DATUM_PTR);
TYPE_PTR Create_tensor_type(TYPE_PTR base_type_idx, std::vector<int>& data_dim,
                            GLOB_SCOPE* glob);
TYPE_PTR Create_array_type(TYPE_PTR elem_ty, std::vector<int>& data_dim,
                           GLOB_SCOPE* glob);
TYPE_PTR Create_struct_type(TYPE_PTR fld_ty);

}  // namespace onnx2air
}  // namespace nn

#endif /* AIR_UTILS_H */
