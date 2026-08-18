//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/driver/onnx_cmplr.h"

int main(int argc, char* argv[]) {
  nn::driver::ONNX_COMPILER cmplr(true);
  R_CODE                    ret_code = cmplr.Init(argc, argv);
  if (ret_code == R_CODE::NORMAL) {
    ret_code = cmplr.Pre_run();
  } else {
    return int(ret_code);
  }
  if (ret_code == R_CODE::NORMAL) {
    ret_code = cmplr.Run();
  }
  cmplr.Post_run();
  cmplr.Fini();

  return int(ret_code);
}
