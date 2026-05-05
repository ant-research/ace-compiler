//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/pass.h"

#include "nn/driver/onnx_cmplr.h"
#include "nn/onnx2air/onnx2air_decl.h"

using namespace std;
using namespace air::base;
using namespace air::core;
using namespace air::util;

using namespace nn::onnx2air;

namespace nn {

namespace onnx2air {

ONNX2AIR_PASS::ONNX2AIR_PASS() : _driver(nullptr) {}

R_CODE ONNX2AIR_PASS::Init(air::driver::DRIVER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE ONNX2AIR_PASS::Pre_run() {
  _config.Update_options();
  return R_CODE::NORMAL;
}

R_CODE ONNX2AIR_PASS::Run() {
  if (Onnx2air_driver(_driver->Context()->Glob_scope(), _driver->Context(),
                      _config, _driver->Context()->Ifile()) == nullptr) {
    // TODO: the reason for failure may be because of input file or internel
    // implement, need to classify.
    return R_CODE::USER;
  }
  return R_CODE::NORMAL;
}

void ONNX2AIR_PASS::Post_run() {}

void ONNX2AIR_PASS::Fini() {}

}  // namespace onnx2air

}  // namespace nn
