//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_DRIVER_ONNX_FHE_CMPLR_H
#define FHE_DRIVER_ONNX_FHE_CMPLR_H

#include "fhe/core/scheme_info_pass.h"
#include "fhe_cmplr.h"
#include "nn/driver/onnx_cmplr.h"

namespace fhe {

namespace driver {

typedef air::driver::PASS_MANAGER<nn::onnx2air::ONNX2AIR_PASS,
                                  fhe::core::SCHEME_INFO_PASS,
                                  nn::vector::VECTOR_PASS>
    ONNX_PASS_MANAGER;

class ONNX_FHE_COMPILER : public air::driver::DRIVER {
public:
  ONNX_FHE_COMPILER();

  R_CODE Init(int argc, char** argv);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

private:
  ONNX_PASS_MANAGER _onnx_pass;
  FHE_COMPILER      _fhe_cmplr;
};  // ONNX_FHE_COMPILER

}  // namespace driver

}  // namespace fhe

#endif  // FHE_DRIVER_ONNX_FHE_CMPLR_H
