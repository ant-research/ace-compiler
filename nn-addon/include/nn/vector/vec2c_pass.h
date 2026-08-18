//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_VEC2C_PASS_H
#define NN_VECTOR_VEC2C_PASS_H

#include "air/driver/driver.h"
#include "air/driver/pass.h"
#include "nn/vector/vec2c_config.h"

namespace nn {

namespace vector {

class VEC2C_PASS : public air::driver::PASS<VEC2C_CONFIG> {
public:
  VEC2C_PASS();

  R_CODE Init(air::driver::DRIVER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "VEC2C"; }

private:
  air::driver::DRIVER* _driver;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_VEC2C_PASS_H
