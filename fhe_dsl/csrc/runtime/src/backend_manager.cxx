//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <torch/torch.h>

#include "backend_manager.h"
#include "config_manager.h"
#include <stdexcept>

namespace ace {
namespace runtime {

void BACKEND_MANAGER::Register_backend(const std::string& name, const std::string& lib_path) {
    auto runner = std::make_unique<KERNEL_RUNNER>(lib_path);
    if (!runner->Load_kernel()) {
        throw std::runtime_error("Failed to load kernel from: " + lib_path);
    }
    _runners[name] = std::move(runner);
}

torch::Tensor BACKEND_MANAGER::Run(
  const std::string& backend_name,
  const std::vector<torch::Tensor>& inputs,
  const std::string& output_name
) {
  auto it = _runners.find(backend_name);
  if (it == _runners.end()) {
      throw std::runtime_error("Backend not registered: " + backend_name);
  }

  auto& runner = it->second;
  _result = runner->Run(inputs, output_name);

  // Get output shape from config (decode_schemes)
  // Default to {1,1,1,1} if not configured
  std::vector<long> output_shape_vec = {1, 1, 1, 1};

  const auto& config = CONFIG_MANAGER::Instance().Get_config();
  if (config._decode_sch.size() > 0) {
      const auto& decode_sch = config._decode_sch[0];
      output_shape_vec[0] = static_cast<long>(decode_sch._shape._n);
      output_shape_vec[1] = static_cast<long>(decode_sch._shape._c);
      output_shape_vec[2] = static_cast<long>(decode_sch._shape._h);
      output_shape_vec[3] = static_cast<long>(decode_sch._shape._w);
  }

  auto options = torch::TensorOptions().dtype(torch::kFloat64);
  torch::Tensor output = torch::from_blob(_result, output_shape_vec, options).clone();

  return output;
}

//! @brief Plaintext Validate
bool BACKEND_MANAGER::Validate_result(const torch::Tensor& tensor) {
  double* expect = tensor.data_ptr<double>();
  int sz = tensor.numel();

  LOG_INFO(VALIDATOR::Compose_str("Result: ", _result, sz));
  LOG_INFO(VALIDATOR::Compose_str("Expect: ", expect, sz));

  bool    res_relative    = VALIDATOR::Validate_relative_error(_result, expect, sz);
  bool    res_absolute    = VALIDATOR::Validate_absolute_error(_result, expect, sz);
  free(_result);
  if (res_relative || res_absolute) {
      LOG_INFO("Inference SUCCESS!");
  } else {
      LOG_INFO("Inference FAILED!");
      return false;
  }
  return true;
}

} // namespace runtime
} // namespace ace