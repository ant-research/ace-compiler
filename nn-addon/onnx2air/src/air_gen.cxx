//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/air_gen.h"

#include "nn/onnx2air/air_func.h"

namespace nn {
namespace onnx2air {
AIRGEN::~AIRGEN() {}

bool AIRGEN::Process_graph(onnx::ModelProto& onnx_model) {
  onnx::GraphProto onnx_graph = onnx_model.graph();
  AIRFUNCGEN       func_bldr(this);

  FUNC_SCOPE* func_scope = Sg().Convert_func_sym(onnx_graph);

  if (func_scope == nullptr) return false;

  return func_bldr.Convert_func(func_scope, onnx_graph);
}

STR_PTR
AIRGEN::Enter_string(const char* str) { return _glob->New_str(str); }
}  // namespace onnx2air
}  // namespace nn
