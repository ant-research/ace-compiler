//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_FHE_PROFILING_H
#define ACE_RUNTIME_FHE_PROFILING_H

#include <ATen/record_function.h>
#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>

#ifdef USE_CUDA
#include <cuda_runtime_api.h>
#include <nvToolsExt.h>
#endif

namespace ace {
namespace runtime {

// ---------------------------------------------------------------------------
// FHE_PROFILE_SCOPE: profiling annotation visible in both torch.profiler and
// nsys.
//
// Always uses RECORD_USER_SCOPE (torch.profiler visible).
// On GPU builds, additionally pushes an NVTX range (nsys visible).
// The two appear in different trace categories and do not duplicate.
//
// Usage: FHE_PROFILE_SCOPE("fhe::execute");
// ---------------------------------------------------------------------------

#ifdef USE_CUDA

class NvtxRangeGuard {
public:
  explicit NvtxRangeGuard(const std::string& name) : name_(name) {
    nvtxRangePushA(name_.c_str());
  }
  ~NvtxRangeGuard() {
    nvtxRangePop();
  }
  NvtxRangeGuard(const NvtxRangeGuard&) = delete;
  NvtxRangeGuard& operator=(const NvtxRangeGuard&) = delete;

private:
  std::string name_;
};

// RECORD_USER_SCOPE for torch.profiler + NVTX range for nsys
#define FHE_PROFILE_SCOPE(name)                                                  \
  RECORD_USER_SCOPE(name);                                                       \
  ::ace::runtime::NvtxRangeGuard _nvtx_guard_##__LINE__(name)

#else

#define FHE_PROFILE_SCOPE(name) RECORD_USER_SCOPE(name)

#endif  // USE_CUDA

// ---------------------------------------------------------------------------
// Memory sampling
// ---------------------------------------------------------------------------

struct MemInfo {
  size_t gpu_used_mb;
  size_t gpu_free_mb;
  size_t gpu_total_mb;
  bool gpu_available;
};

inline MemInfo sample_memory() {
  MemInfo info = {0, 0, 0, false};
#ifdef USE_CUDA
  size_t free_mem = 0, total_mem = 0;
  if (cudaMemGetInfo(&free_mem, &total_mem) == cudaSuccess) {
    info.gpu_free_mb = free_mem / (1024 * 1024);
    info.gpu_total_mb = total_mem / (1024 * 1024);
    info.gpu_used_mb = info.gpu_total_mb - info.gpu_free_mb;
    info.gpu_available = true;
  }
#endif
  return info;
}

// Record a memory snapshot as an instant marker in the profiler trace.
// Uses nvtxMarkA (instant event) on GPU, RECORD_USER_SCOPE on CPU.
//
// Example marker name:
//   fhe::mem::init_after::23307MB_gpu_used_74049MB_gpu_free
inline void record_mem_snapshot(const char* phase) {
  auto mem = sample_memory();
  if (!mem.gpu_available) {
    return;
  }
  char name[256];
  snprintf(name, sizeof(name),
           "fhe::mem::%s::%zuMB_gpu_used_%zuMB_gpu_free",
           phase, mem.gpu_used_mb, mem.gpu_free_mb);
#ifdef USE_CUDA
  nvtxMarkA(name);
#endif
  RECORD_USER_SCOPE(name);
}

// Build a profile scope name with memory info appended.
// Returns a std::string so the caller owns the data.
//
// Example output: "fhe::execute [23307MB gpu]"
inline std::string fhe_profile_name_with_mem(const char* name) {
  auto mem = sample_memory();
  if (!mem.gpu_available) {
    return std::string(name);
  }
  char buf[256];
  snprintf(buf, sizeof(buf), "%s [%zuMB gpu]", name, mem.gpu_used_mb);
  return std::string(buf);
}

// FHE_PROFILE_SCOPE_WITH_MEM: like FHE_PROFILE_SCOPE but appends current
// GPU memory usage to the event name, so Perfetto/nsys shows memory
// directly on the FHE phase slice.
//
// Example in Perfetto: "fhe::execute [23307MB gpu]"
#ifdef USE_CUDA

#define FHE_PROFILE_SCOPE_WITH_MEM(name)                                         \
  const std::string _fhe_mem_name_##__LINE__(                                    \
      ::ace::runtime::fhe_profile_name_with_mem(name));                          \
  RECORD_USER_SCOPE(_fhe_mem_name_##__LINE__.c_str());                           \
  ::ace::runtime::NvtxRangeGuard _nvtx_guard_##__LINE__(                         \
      _fhe_mem_name_##__LINE__)

#else

#define FHE_PROFILE_SCOPE_WITH_MEM(name)                                         \
  const std::string _fhe_mem_name_##__LINE__(                                    \
      ::ace::runtime::fhe_profile_name_with_mem(name));                          \
  RECORD_USER_SCOPE(_fhe_mem_name_##__LINE__.c_str())

#endif  // USE_CUDA

}  // namespace runtime
}  // namespace ace

#endif  // ACE_RUNTIME_FHE_PROFILING_H