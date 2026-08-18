//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ONNX2AIR_DECL_H
#define ONNX2AIR_DECL_H

#include <vector>

#include "air/base/container.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "nn/onnx2air/config.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;
using namespace air::driver;

namespace nn {
namespace onnx2air {

#define FUNC_NAME "Main_graph"

extern GLOB_SCOPE* Onnx2air_driver(GLOB_SCOPE*            scope,
                                   const DRIVER_CTX*      driver_ctx,
                                   const ONNX2AIR_CONFIG& cfg,
                                   const char*            ifile);

}  // namespace onnx2air
}  // namespace nn

#endif
