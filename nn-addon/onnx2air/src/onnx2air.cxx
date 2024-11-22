//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <filesystem>
#include <fstream>
#include <iostream>

#include "air/base/container.h"
#include "air/base/st.h"
#include "nn/core/opcode.h"
#include "nn/onnx2air/air_gen.h"
#include "nn/util/copy_prop.h"
#include "onnx.pb.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;

namespace nn {
namespace onnx2air {

namespace fs = std::filesystem;

static std::string Get_directory_path(const std::string& file_name) {
  fs::path absoulte_path  = file_name;
  fs::path directory_path = absoulte_path.parent_path();
  return directory_path.lexically_normal().string();
}

GLOB_SCOPE* Onnx2air_driver(GLOB_SCOPE*                    glob,
                            const air::driver::DRIVER_CTX* driver_ctx,
                            const ONNX2AIR_CONFIG& cfg, const char* ifile) {
  onnx::ModelProto onnx_model;
  std::ifstream    input(ifile);
  onnx_model.ParseFromIstream(&input);
  nn::onnx2air::AIRGEN air_gen(glob);
  air_gen.Directory_path_set(Get_directory_path(std::string(ifile)));
  if (air_gen.Process_graph(onnx_model) == false) return nullptr;
  GLOB_SCOPE* new_glob =
      nn::opt::Opt_perform_copy_propagation(glob, driver_ctx);
  return new_glob;
}

}  // namespace onnx2air
}  // namespace nn
