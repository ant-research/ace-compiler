//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_FUNC_H
#define AIR_FUNC_H

#include "nn/onnx2air/onnx2air_decl.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {
class AIRGEN;

/**
 * @brief Interface of generating the function for onnx graph.
 *
 */
class AIRFUNCGEN {
public:
  AIRFUNCGEN(AIRGEN* airgen);
  ~AIRFUNCGEN();

  bool    Convert_func(FUNC_SCOPE* func_scope, onnx::GraphProto& onnx_graph);
  AIRGEN* Get_airgen() { return _airgen; }

private:
  AIRFUNCGEN(void);               // REQUIRED UNDEFINED UNWANTED methods
  AIRFUNCGEN(const AIRFUNCGEN&);  // REQUIRED UNDEFINED UNWANTED methods
  AIRFUNCGEN& operator=(
      const AIRFUNCGEN&);  // REQUIRED UNDEFINED UNWANTED methods

  AIRGEN* _airgen;
};
}  // namespace onnx2air
}  // namespace nn

#endif
