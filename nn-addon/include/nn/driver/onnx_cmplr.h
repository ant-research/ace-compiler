//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_DRIVER_ONNX_CMPLR_H
#define NN_DRIVER_ONNX_CMPLR_H

#include "air/driver/driver.h"
#include "onnx_pipeline.h"

namespace nn {

namespace driver {

class ONNX_COMPILER : public air::driver::DRIVER {
public:
  ONNX_COMPILER(bool standalone);

  template <typename UP_DRV>
  R_CODE Init(UP_DRV* drv) {
    air::driver::DRIVER::Init(drv);
    return _pass_mgr.Init(this);
  }

  R_CODE Init(int argc, char** argv) {
    _pass_mgr.Init(this);
    return air::driver::DRIVER::Init(argc, argv);
  }

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

private:
  ONNX_PASS_MANAGER _pass_mgr;

  // TODO: add option handler here

};  // ONNX_COMPILER

}  // namespace driver

}  // namespace nn

#endif  // NN_DRIVER_ONNX_CMPLR_H
