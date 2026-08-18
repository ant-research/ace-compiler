//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_PASS_H
#define NN_VECTOR_PASS_H

#include "air/driver/driver.h"
#include "air/driver/pass.h"
#include "config.h"
#include "nn/vector/vector_ctx.h"

namespace nn {

namespace vector {

class VECTOR_PASS : public air::driver::PASS<VECTOR_CONFIG> {
public:
  VECTOR_PASS();

  R_CODE Init(air::driver::DRIVER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "VECTOR"; }

private:
  air::driver::DRIVER* _driver;
  vector::VECTOR_CTX   _ctx;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_PASS_H
