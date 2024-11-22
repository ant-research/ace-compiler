//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CONST_H
#define AIR_CONST_H

#include "nn/onnx2air/onnx2air_decl.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {

class AIRGEN;
class AIRCONSTGEN {
public:
  AIRCONSTGEN(AIRGEN* airgen) : _airgen(airgen) {}

  ~AIRCONSTGEN() {}

  CONSTANT_PTR Convert_const(const onnx::TensorProto& tensor, TYPE_PTR ty_ptr);
  int          Data_elem_size(int32_t data_type);

  // Converting and Saving an ONNX Model to External Data
  // Example:
  //
  // import onnx
  // onnx_model = onnx.load("conv2d.onnx")
  // onnx.save_model(onnx_model, "/tmp/model.onnx", save_as_external_data=True,
  // all_tensors_to_one_file=True, location="filename", size_threshold=0,
  // convert_attribute=False)
  //
  void   Read_external_data(const onnx::TensorProto& tensor);
  size_t Parse_offset_or_length(const std::string& value);

private:
  AIRCONSTGEN(void);                // REQUIRED UNDEFINED UNWANTED methods
  AIRCONSTGEN(const AIRCONSTGEN&);  // REQUIRED UNDEFINED UNWANTED methods
  AIRCONSTGEN& operator=(
      const AIRCONSTGEN&);  // REQUIRED UNDEFINED UNWANTED methods

  AIRGEN* _airgen;
};

}  // namespace onnx2air
}  // namespace nn

#endif
