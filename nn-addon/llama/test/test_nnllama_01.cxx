//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/core/opcode.h"
#include "nn/core/opcode.h"
#include "nn/llama/llama.h"

using namespace air::base;

int main() {
  bool ret = air::core::Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  ret = nn::core::Register_nn();
  AIR_ASSERT_MSG(ret, "Register nn domain failed");

  air::driver::DRIVER_CTX ctx;
  nn::llama::LLAMA_CONFIG config;
  GLOB_SCOPE*             glob = air::base::GLOB_SCOPE::Get();
  GLOB_SCOPE* new_glob         = nn::llama::Llama_driver(glob, &ctx, config);
  new_glob->Print();

  return 0;
}
