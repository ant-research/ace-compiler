//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/air_func.h"

#include "nn/onnx2air/air_gen.h"
#include "nn/onnx2air/air_stmt.h"
#include "nn/onnx2air/air_utils.h"

namespace nn {
namespace onnx2air {

AIRFUNCGEN::AIRFUNCGEN(AIRGEN* airgen) : _airgen(airgen) {}

AIRFUNCGEN::~AIRFUNCGEN() {}

bool AIRFUNCGEN::Convert_func(FUNC_SCOPE*       func_scope,
                              onnx::GraphProto& onnx_graph) {
  AIRSTMTGEN stmt_gen(Get_airgen());
  return stmt_gen.Convert_stmts(func_scope, onnx_graph);
}

}  // namespace onnx2air
}  // namespace nn
