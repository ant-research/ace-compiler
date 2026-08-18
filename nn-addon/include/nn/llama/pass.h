//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_LLAMA_PASS_H
#define NN_LLAMA_PASS_H

#include "air/driver/driver.h"
#include "air/driver/pass.h"
#include "config.h"

namespace nn {

namespace llama {

class LLAMA_PASS : public air::driver::PASS<LLAMA_CONFIG> {
public:
  LLAMA_PASS();

  R_CODE Init(air::driver::DRIVER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "LLAMA"; }

private:
  air::driver::DRIVER* _driver;
};

}  // namespace llama

}  // namespace nn

#endif  // NN_LLAMA_PASS_H
