//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/driver/onnx_fhe_cmplr.h"

#include <iostream>

#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/opcode.h"
#include "fhe/sihe/sihe_opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"

using namespace std;

namespace fhe {

namespace driver {

ONNX_FHE_COMPILER::ONNX_FHE_COMPILER()
    : air::driver::DRIVER(true), _fhe_cmplr(false) {
  bool ret;
  ret = air::core::Register_core();
  AIR_ASSERT(ret);
  ret = nn::core::Register_nn();
  AIR_ASSERT(ret);
  ret = nn::vector::Register_vector_domain();
  AIR_ASSERT(ret);
  ret = fhe::sihe::Register_sihe_domain();
  AIR_ASSERT(ret);
  ret = fhe::ckks::Register_ckks_domain();
  AIR_ASSERT(ret);
  ret = fhe::poly::Register_polynomial();
  AIR_ASSERT(ret);
}

R_CODE ONNX_FHE_COMPILER::Init(int argc, char** argv) {
  R_CODE ret_code = _onnx_pass.Init(this);
  if (ret_code == R_CODE::NORMAL) {
    ret_code = _fhe_cmplr.Init(this);
  } else {
    return ret_code;
  }
  if (ret_code == R_CODE::NORMAL) {
    return air::driver::DRIVER::Init(argc, argv);
  } else {
    return ret_code;
  }
}

R_CODE ONNX_FHE_COMPILER::Pre_run() {
  R_CODE ret_code = _onnx_pass.Pre_run(this);
  if (ret_code == R_CODE::NORMAL) {
    ret_code = _fhe_cmplr.Pre_run();
  }
  return ret_code;
}

R_CODE ONNX_FHE_COMPILER::Run() {
  R_CODE ret_code = _onnx_pass.Run(this);
  if (ret_code == R_CODE::NORMAL) {
    _fhe_cmplr.Run();
  }
  return ret_code;
}

void ONNX_FHE_COMPILER::Post_run() {
  _fhe_cmplr.Post_run();
  _onnx_pass.Post_run(this);
}

void ONNX_FHE_COMPILER::Fini() {
  _fhe_cmplr.Fini();
  _onnx_pass.Fini(this);
}

}  // namespace driver

}  // namespace fhe
