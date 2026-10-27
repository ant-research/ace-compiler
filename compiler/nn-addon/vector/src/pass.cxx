//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/pass.h"

#include "nn/driver/onnx_cmplr.h"
#include "nn/vector/vector_gen.h"

using namespace std;

namespace nn {

namespace vector {

VECTOR_PASS::VECTOR_PASS() : _driver(nullptr) {}

R_CODE VECTOR_PASS::Init(air::driver::DRIVER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE VECTOR_PASS::Pre_run() {
  _config.Update_options();
  return R_CODE::NORMAL;
}

R_CODE VECTOR_PASS::Run() {
  air::base::GLOB_SCOPE* glob = nn::vector::Vector_driver(
      _driver->Glob_scope(), _ctx, _driver->Context(), _config);
  _driver->Update_glob_scope(glob);
  return R_CODE::NORMAL;
}

void VECTOR_PASS::Post_run() {}

void VECTOR_PASS::Fini() {}

}  // namespace vector

}  // namespace nn
