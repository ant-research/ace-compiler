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

#include "ace/runtime/provider_manager.h"
#include "ace/runtime/config_manager.h"
#include <stdexcept>

namespace ace {
namespace runtime {

void PROVIDER_MANAGER::Register_provider(const std::string& name, const std::string& lib_path,
                                          bool use_cuda_graph) {
    auto runner = std::make_shared<KERNEL_RUNNER>(lib_path, use_cuda_graph);
    if (!runner->Load_kernel()) {
        throw std::runtime_error("Failed to load kernel from: " + lib_path);
    }
    std::unique_lock<std::shared_mutex> lock(_runners_mutex);
    _runners[name] = std::move(runner);
}

void PROVIDER_MANAGER::Init(const std::string& provider_name) {
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    it->second->Init();
}

std::shared_ptr<KERNEL_RUNNER> PROVIDER_MANAGER::Get_runner(const std::string& provider_name) const {
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    return it->second;  // Returns a copy of the shared_ptr, keeping the runner alive
}

torch::Tensor PROVIDER_MANAGER::Run(
  const std::string& provider_name,
  const std::vector<torch::Tensor>& inputs,
  const std::string& output_name
) {
  auto runner = Get_runner(provider_name);

  double* raw_result = runner->Run(inputs, output_name);

  // Get output shape from config (decode_schemes)
  std::vector<long> output_shape_vec = {1, 1, 1, 1};
  auto config = CONFIG_MANAGER::Instance().Get_config();
  if (config._decode_sch.size() > 0) {
      const auto& decode_sch = config._decode_sch[0];
      output_shape_vec[0] = static_cast<long>(decode_sch._shape._n);
      output_shape_vec[1] = static_cast<long>(decode_sch._shape._c);
      output_shape_vec[2] = static_cast<long>(decode_sch._shape._h);
      output_shape_vec[3] = static_cast<long>(decode_sch._shape._w);
  }

  auto options = torch::TensorOptions().dtype(torch::kFloat64);
  torch::Tensor output = torch::from_blob(raw_result, output_shape_vec, options).clone();

  // Free the raw result buffer — data has been cloned into the tensor
  free(raw_result);

  return output;
}

BATCH_RESULT PROVIDER_MANAGER::Run_batch(
    const std::string& provider_name,
    const std::vector<std::vector<torch::Tensor>>& batch_inputs,
    const std::string& output_name,
    bool verbose) {

    auto runner = Get_runner(provider_name);
    BATCH_RUNNER batch_runner(runner);

    return batch_runner.Run_sequential_batch(batch_inputs, output_name, verbose);
}

BATCH_RESULT PROVIDER_MANAGER::Run_batch_parallel(
    const std::string& provider_name,
    const std::vector<std::vector<torch::Tensor>>& batch_inputs,
    const std::string& output_name,
    int num_threads,
    bool verbose) {

    auto runner = Get_runner(provider_name);
    BATCH_RUNNER batch_runner(runner);

    return batch_runner.Run_parallel_batch(batch_inputs, output_name, num_threads, verbose);
}

void PROVIDER_MANAGER::Finalize(const std::string& provider_name) {
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    it->second->Finalize();
}

bool PROVIDER_MANAGER::Capture_graph(const std::string& provider_name) {
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    return it->second->Capture_graph();
}

} // namespace runtime
} // namespace ace