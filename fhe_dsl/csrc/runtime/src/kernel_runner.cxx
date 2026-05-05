//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdexcept>
#include <iostream>
#include "kernel_runner.h"
#include "rtlib_interface.h"

namespace ace {
namespace runtime {

KERNEL_RUNNER::KERNEL_RUNNER(const std::string& lib_path) : _lib_path(lib_path) {
  _handle = dlopen(lib_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
  if (!_handle) {
      throw std::runtime_error("Failed to load library: " + std::string(dlerror()));
  }
}

KERNEL_RUNNER::~KERNEL_RUNNER() {
  std::cout << "Closing =========== " << _lib_path << std::endl;
  if (_handle) {
      dlclose(_handle);
      _handle = nullptr;
  }
}

bool KERNEL_RUNNER::Load_kernel(const std::string& symbol_name) {
  if (!_handle) return false;

  dlerror(); // Clear any existing error

  _prepare_ctx    = (Prepare_context_func)dlsym(_handle, "Prepare_context");
  _final_ctx      = (Finalize_context_func)dlsym(_handle, "Finalize_context");
  _prepare_input  = (Prepare_input_func)dlsym(_handle, "Prepare_input");
  _handle_output  = (Handle_output_func)dlsym(_handle, "Handle_output");
  _run_kernel     = (Kernel_func)dlsym(_handle, "Main_graph");

  if (!_prepare_ctx || !_run_kernel) {
      dlclose(_handle);
      _handle = nullptr;
      return false;
  }

  return true;
}

double* KERNEL_RUNNER::Run(
  const std::vector<torch::Tensor>& inputs,
  const std::string& output_name
) {
  // Auto-reload if .so was previously closed (e.g., by a prior Run cycle)
  if (!_handle) {
      _handle = dlopen(_lib_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
      if (!_handle) {
          throw std::runtime_error("Failed to reload library: " + _lib_path +
                                   " error: " + std::string(dlerror()));
      }
      if (!Load_kernel()) {
          throw std::runtime_error("Failed to reload kernel from: " + _lib_path);
      }
  }

  // 1. Prepare_context
  _prepare_ctx();

  std::vector<TENSOR*> allocated_tensors;

  // 2-3. Alloc_tensor + Prepare_input (+ optional Print)
  for (size_t i = 0; i < inputs.size(); ++i) {
      auto t = inputs[i].to(torch::kFloat64);
      auto sizes = t.sizes();
      if (sizes.size() != 4) {
          throw std::runtime_error("Input must be 4D [N,C,H,W]");
      }
      size_t n = sizes[0], c = sizes[1], h = sizes[2], w = sizes[3];
      const double* data = static_cast<const double*>(t.data_ptr());

      TENSOR* tensor = Alloc_tensor(n, c, h, w, data);
      allocated_tensors.push_back(tensor);

      Print_tensor(stdout, tensor);
      std::string name = get_encode_scheme_c(i)->_name;
      _prepare_input(tensor, name.c_str());
  }

  // 4. Run
  auto rs = _run_kernel();

  // 6. Handle_output
  double* result = _handle_output(output_name.c_str());

  // 7. Clear
  for (auto tensor : allocated_tensors) {
      Free_tensor(tensor);
  }

  // 8. Finalize context
  _final_ctx();

  // 9. Unload .so to allow other .so files to load cleanly.
  //    Run() will auto-reload on next call via dlopen + Load_kernel.
  dlclose(_handle);
  _handle = nullptr;

  return result;
}

} // namespace runtime
} // namespace ace
