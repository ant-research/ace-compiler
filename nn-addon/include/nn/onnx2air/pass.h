//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_ONNX2AIR_PASS_H
#define NN_ONNX2AIR_PASS_H

#include "air/driver/driver.h"
#include "air/driver/pass.h"
#include "config.h"

namespace nn {

namespace onnx2air {

class ONNX2AIR_PASS : public air::driver::PASS<ONNX2AIR_CONFIG> {
public:
  ONNX2AIR_PASS();

  R_CODE Init(air::driver::DRIVER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "ONNX2AIR"; }

private:
  air::driver::DRIVER* _driver;
};

}  // namespace onnx2air

}  // namespace nn

#endif  // NN_ONNX2AIR_PASS_H
