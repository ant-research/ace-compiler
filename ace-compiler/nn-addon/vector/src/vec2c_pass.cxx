//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vec2c_pass.h"

#include "nn/vector/vec2c_driver.h"

using namespace std;

namespace nn {

namespace vector {

VEC2C_PASS::VEC2C_PASS() : _driver(nullptr) {}

R_CODE VEC2C_PASS::Init(air::driver::DRIVER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE VEC2C_PASS::Pre_run() {
  _config.Update_options();
  return R_CODE::NORMAL;
}

R_CODE VEC2C_PASS::Run() {
  std::ofstream of(_driver->Context()->Ofile());
  VEC2C_DRIVER  vec2c(of, _config);
  vec2c.Run(_driver->Glob_scope());
  return R_CODE::NORMAL;
}

void VEC2C_PASS::Post_run() {}

void VEC2C_PASS::Fini() {}

}  // namespace vector

}  // namespace nn
