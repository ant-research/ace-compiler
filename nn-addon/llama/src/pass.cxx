//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/llama/pass.h"

#include "nn/llama/llama.h"

namespace nn {

namespace llama {

LLAMA_PASS::LLAMA_PASS() : _driver(nullptr) {}

R_CODE LLAMA_PASS::Init(air::driver::DRIVER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE LLAMA_PASS::Pre_run() {
  _config.Update_options();
  return R_CODE::NORMAL;
}

R_CODE LLAMA_PASS::Run() {
  if (llama::Llama_driver(_driver->Context()->Glob_scope(), _driver->Context(),
                          _config) == nullptr) {
    return R_CODE::USER;
  }
  return R_CODE::NORMAL;
}

void LLAMA_PASS::Post_run() {}

void LLAMA_PASS::Fini() {}

}  // namespace llama

}  // namespace nn
