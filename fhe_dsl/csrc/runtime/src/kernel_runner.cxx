//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdexcept>
#include "ace/runtime/kernel_runner.h"
#include "ace/runtime/rtlib_interface.h"
#include "ace/runtime/fhe_profiling.h"

namespace ace {
namespace runtime {

KERNEL_RUNNER::KERNEL_RUNNER(const std::string& lib_path, bool use_cuda_graph)
    : _lib_path(lib_path), _use_cuda_graph(use_cuda_graph) {
  _handle = dlopen(lib_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
  if (!_handle) {
      throw std::runtime_error("Failed to load library: " + std::string(dlerror()));
  }

  if (!Load_kernel()) {
      dlclose(_handle);
      _handle = nullptr;
      throw std::runtime_error("Failed to load kernel symbols from: " + lib_path);
  }

#ifdef USE_CUDA
  if (_use_cuda_graph) {
      cudaError_t err = cudaStreamCreateWithFlags(&_stream, cudaStreamNonBlocking);
      if (err != cudaSuccess) {
          _use_cuda_graph = false;
      }
  }
#endif
}

KERNEL_RUNNER::~KERNEL_RUNNER() {
  Finalize();
#ifdef USE_CUDA
  if (_graph_exec) {
      cudaGraphExecDestroy(_graph_exec);
      _graph_exec = nullptr;
  }
  if (_graph) {
      cudaGraphDestroy(_graph);
      _graph = nullptr;
  }
  if (_stream) {
      cudaStreamDestroy(_stream);
      _stream = nullptr;
  }
#endif
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
      return false;
  }

  return true;
}

bool KERNEL_RUNNER::Init() {
  if (_initialized.load()) return true;

  FHE_PROFILE_SCOPE_WITH_MEM("fhe::init");
  record_mem_snapshot("init_before");
  _prepare_ctx();
  record_mem_snapshot("init_after");
  _initialized.store(true);
  return true;
}

double* KERNEL_RUNNER::Run(
  const std::vector<torch::Tensor>& inputs,
  const std::string& output_name
) {
  FHE_PROFILE_SCOPE_WITH_MEM("fhe::run");

  // Auto-initialize if not already done (backward compatible)
  if (!_initialized.load()) {
      if (!Init()) {
          throw std::runtime_error("Failed to initialize kernel for: " + _lib_path);
      }
  }

  record_mem_snapshot("run_start");

  std::vector<TENSOR*> allocated_tensors;

  // Per-inference: Alloc_tensor + Prepare_input
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

      std::string name = get_encode_scheme_c(i)->_name;
      _prepare_input(tensor, name.c_str());
  }

  record_mem_snapshot("run_before_execute");

  // Per-inference: Run kernel
  // CUDA Graph is not supported in Run() path because it includes I/O
  // (Prepare_input + Handle_output) which changes per invocation.
  // Use the phase-split API (Prepare_input_only -> Execute -> Get_output)
  // for CUDA Graph support.
  _run_kernel();

  record_mem_snapshot("run_after_execute");

  // Per-inference: Handle_output
  double* result = _handle_output(output_name.c_str());

  // Per-inference: Free tensors
  for (auto tensor : allocated_tensors) {
      Free_tensor(tensor);
  }

  record_mem_snapshot("run_end");

  return result;
}

void KERNEL_RUNNER::Prepare_input_only(const std::vector<torch::Tensor>& inputs) {
  FHE_PROFILE_SCOPE_WITH_MEM("fhe::prepare_input");
  std::lock_guard<std::mutex> lock(_io_mutex);

  Phase expected = Phase::IDLE;
  if (!_phase.compare_exchange_strong(expected, Phase::PREPARED)) {
      throw std::runtime_error(
          "Prepare_input_only: invalid phase transition (expected IDLE, got "
          + std::to_string(static_cast<int>(expected)) + ")");
  }

  if (!_initialized.load()) {
      if (!Init()) {
          _phase.store(Phase::IDLE);
          throw std::runtime_error("Failed to initialize kernel for: " + _lib_path);
      }
  }

  // Free any previously allocated tensors
  for (auto tensor : _allocated_tensors) {
      Free_tensor(tensor);
  }
  _allocated_tensors.clear();

  record_mem_snapshot("prepare_input_before");

  // Alloc_tensor + Prepare_input
  for (size_t i = 0; i < inputs.size(); ++i) {
      auto t = inputs[i].to(torch::kFloat64);
      auto sizes = t.sizes();
      if (sizes.size() != 4) {
          _phase.store(Phase::IDLE);
          throw std::runtime_error("Input must be 4D [N,C,H,W]");
      }
      size_t n = sizes[0], c = sizes[1], h = sizes[2], w = sizes[3];
      const double* data = static_cast<const double*>(t.data_ptr());

      TENSOR* tensor = Alloc_tensor(n, c, h, w, data);
      _allocated_tensors.push_back(tensor);

      std::string name = get_encode_scheme_c(i)->_name;
      _prepare_input(tensor, name.c_str());
  }

  record_mem_snapshot("prepare_input_after");
}

bool KERNEL_RUNNER::Execute() {
  FHE_PROFILE_SCOPE_WITH_MEM("fhe::execute");
  Phase expected = Phase::PREPARED;
  if (!_phase.compare_exchange_strong(expected, Phase::EXECUTED)) {
      throw std::runtime_error(
          "Execute: invalid phase transition (expected PREPARED, got "
          + std::to_string(static_cast<int>(expected)) + ")");
  }

  if (!_run_kernel) {
      throw std::runtime_error("Kernel not loaded for: " + _lib_path);
  }
  record_mem_snapshot("execute_before");

#ifdef USE_CUDA
  if (_use_cuda_graph && _graph_captured && !_graph_capture_failed) {
      // Replay captured graph
      cudaError_t err = cudaGraphLaunch(_graph_exec, _stream);
      if (err != cudaSuccess) {
          _graph_capture_failed = true;
          bool result = _run_kernel();
          record_mem_snapshot("execute_after");
          return result;
      }
      cudaStreamSynchronize(_stream);
      record_mem_snapshot("execute_after");
      return true;
  }
#endif

  bool result = _run_kernel();
  record_mem_snapshot("execute_after");
  return result;
}

bool KERNEL_RUNNER::Capture_graph() {
#ifdef USE_CUDA
  if (!_use_cuda_graph || !_initialized.load() || !_run_kernel) {
      return false;
  }

  // Destroy any previously captured graph
  if (_graph_exec) {
      cudaGraphExecDestroy(_graph_exec);
      _graph_exec = nullptr;
  }
  if (_graph) {
      cudaGraphDestroy(_graph);
      _graph = nullptr;
  }
  _graph_captured = false;
  _graph_capture_failed = false;

  // Run Main_graph() once under stream capture
  cudaError_t err = cudaStreamBeginCapture(_stream, cudaStreamCaptureModeGlobal);
  if (err != cudaSuccess) {
      _graph_capture_failed = true;
      return false;
  }

  _run_kernel();

  err = cudaStreamEndCapture(_stream, &_graph);
  if (err != cudaSuccess || _graph == nullptr) {
      _graph_capture_failed = true;
      return false;
  }

  err = cudaGraphInstantiate(&_graph_exec, _graph, 0);
  if (err != cudaSuccess) {
      _graph_capture_failed = true;
      cudaGraphDestroy(_graph);
      _graph = nullptr;
      return false;
  }

  _graph_captured = true;
  return true;
#else
  return false;
#endif
}

double* KERNEL_RUNNER::Get_output(const std::string& output_name) {
  FHE_PROFILE_SCOPE_WITH_MEM("fhe::get_output");
  std::lock_guard<std::mutex> lock(_io_mutex);

  Phase expected = Phase::EXECUTED;
  if (!_phase.compare_exchange_strong(expected, Phase::IDLE)) {
      throw std::runtime_error(
          "Get_output: invalid phase transition (expected EXECUTED, got "
          + std::to_string(static_cast<int>(expected)) + ")");
  }

  double* result = _handle_output(output_name.c_str());

  record_mem_snapshot("get_output_before_free");

  // Free tensors allocated by Prepare_input_only
  for (auto tensor : _allocated_tensors) {
      Free_tensor(tensor);
  }
  _allocated_tensors.clear();

  record_mem_snapshot("get_output_after_free");

  return result;
}

void KERNEL_RUNNER::Finalize() {
  FHE_PROFILE_SCOPE_WITH_MEM("fhe::finalize");
  if (!_initialized.load() || !_final_ctx) return;
  record_mem_snapshot("finalize_before");
  _final_ctx();
  record_mem_snapshot("finalize_after");
  _initialized.store(false);

#ifdef USE_CUDA
  // Destroy captured graph; it references the old FHE context's memory
  if (_graph_exec) {
      cudaGraphExecDestroy(_graph_exec);
      _graph_exec = nullptr;
  }
  if (_graph) {
      cudaGraphDestroy(_graph);
      _graph = nullptr;
  }
  _graph_captured = false;
  _graph_capture_failed = false;
#endif
}

} // namespace runtime
} // namespace ace
