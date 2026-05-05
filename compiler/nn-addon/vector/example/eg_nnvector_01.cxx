//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <filesystem>

#include "nn/core/opcode.h"
#include "nn/onnx2air/onnx2air_decl.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"

using namespace air::base;
using namespace nn::core;
using namespace nn::vector;

int main(int argc, char* argv[]) {
  AIR_ASSERT_MSG(argc == 2, "must provide onnx file as input");

  if (!std::filesystem::exists(argv[1])) {
    std::cerr << argv[1] << " does not exist" << std::endl;
    exit(1);
  }

  bool ret = air::core::Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  ret = nn::core::Register_nn();
  AIR_ASSERT_MSG(ret, "Register nn domain failed");
  ret = nn::vector::Register_vector_domain();
  AIR_ASSERT_MSG(ret, "Register vector domain failed");

  GLOB_SCOPE*                   tensor_glob = GLOB_SCOPE::Get();
  nn::onnx2air::ONNX2AIR_CONFIG onx_cfg;
  nn::onnx2air::Onnx2air_driver(tensor_glob, nullptr, onx_cfg, argv[1]);

  std::cout << ">> Tensor IR:" << std::endl;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tensor_glob->Begin_func_scope();
       it != tensor_glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    func->Print();
  }

  //   tensor->vector
  std::cout << ">> Vector IR:---------------------------" << std::endl;
  nn::vector::VECTOR_CTX    ctx;
  nn::vector::VECTOR_CONFIG config;
  GLOB_SCOPE*               vector_glob =
      nn::vector::Vector_driver(tensor_glob, ctx, nullptr, config);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = vector_glob->Begin_func_scope();
       it != vector_glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    func->Print();
  }

  return 0;
}
