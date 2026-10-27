//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/driver/onnx_cmplr.h"

#include <iostream>

#include "air/core/opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"

using namespace std;

namespace nn {

namespace driver {

ONNX_COMPILER::ONNX_COMPILER(bool standalone)
    : air::driver::DRIVER(standalone) {
  if (standalone) {
    // for standalone driver, register all needed opcodes
    bool ret;
    ret = air::core::Register_core();
    AIR_ASSERT(ret);
    ret = nn::core::Register_nn();
    AIR_ASSERT(ret);
    ret = nn::vector::Register_vector_domain();
    AIR_ASSERT(ret);
  }
}

R_CODE ONNX_COMPILER::Pre_run() { return _pass_mgr.Pre_run(this); }

R_CODE ONNX_COMPILER::Run() { return _pass_mgr.Run(this); }

void ONNX_COMPILER::Post_run() { _pass_mgr.Post_run(this); }

void ONNX_COMPILER::Fini() { _pass_mgr.Fini(this); }

}  // namespace driver

}  // namespace nn
