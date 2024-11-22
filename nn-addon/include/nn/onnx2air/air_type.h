//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_TYPE_H
#define AIR_TYPE_H

#include "nn/onnx2air/onnx2air_decl.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {

class AIRGEN;
/**
 * @brief Interface of generating the air type for tensor type.
 *
 */
class AIRTYGEN {
public:
  AIRTYGEN(AIRGEN* airgen) : _airgen(airgen) {}
  ~AIRTYGEN() {}

  TYPE_PTR Convert_builtin_type(int32_t data_type);
  TYPE_PTR Create_array_type(TYPE_PTR elem_ty, std::vector<int>& data_dim);
  TYPE_PTR Create_struct_type(TYPE_PTR fld_ty);
  TYPE_PTR Convert_tensor_type(const onnx::TensorProto& tensor);
  TYPE_PTR Convert_io_tensor_type(onnx::ValueInfoProto& vi);
  int      Get_data_num_elements(onnx::TensorProto& tensor);
  AIRGEN*  Get_airgen() { return _airgen; }

private:
  AIRTYGEN(void);                        // REQUIRED UNDEFINED UNWANTED methods
  AIRTYGEN(const AIRTYGEN&);             // REQUIRED UNDEFINED UNWANTED methods
  AIRTYGEN& operator=(const AIRTYGEN&);  // REQUIRED UNDEFINED UNWANTED methods

  AIRGEN* _airgen;
};

}  // namespace onnx2air
}  // namespace nn

#endif /* AIR_TYPE_H */
