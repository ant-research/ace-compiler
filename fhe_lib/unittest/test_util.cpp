//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_util.cpp
//! Shared test utilities implementation.

// Include CUDA headers BEFORE test_util.h to ensure CUDA symbols
// are in the global namespace, not inside rtio::test.
#ifdef ACE_PHANTOM_GPU_TESTS
#include "fhe/io/cuda_check.h"

#include <cuda_runtime.h>
#endif

#include "test_util.h"

#include <unistd.h>
#include <vector>

namespace rtio::test
{

//=============================================================================
// Temp file management
//=============================================================================

std::string make_temp_path(const char* prefix, const char* suffix)
{
    static int counter = 0;
    char buf[256];
    std::snprintf(buf, sizeof(buf), "/tmp/fhert_%s_test_%d_%d_%s", prefix, static_cast<int>(getpid()), ++counter,
                  suffix);
    return std::string(buf);
}

void remove_file(const std::string& path)
{
    std::remove(path.c_str());
}

TempFileGuard make_temp_file(const char* prefix, const char* suffix)
{
    return TempFileGuard(make_temp_path(prefix, suffix));
}

//=============================================================================
// GPU buffer management (only compiled under ACE_PHANTOM_GPU_TESTS)
//=============================================================================

#ifdef ACE_PHANTOM_GPU_TESTS

GpuBuffer::GpuBuffer(size_t size) : size_(size)
{
    FHE_CHECK_CUDA(cudaMalloc(&ptr_, size));
}

GpuBuffer::~GpuBuffer() noexcept
{
    if (ptr_) {
        cudaFree(ptr_);  // Cannot throw from destructor
    }
}

GpuBuffer::GpuBuffer(GpuBuffer&& other) noexcept : ptr_(other.ptr_), size_(other.size_)
{
    other.ptr_ = nullptr;
    other.size_ = 0;
}

GpuBuffer& GpuBuffer::operator=(GpuBuffer&& other) noexcept
{
    if (this != &other) {
        if (ptr_)
            cudaFree(ptr_);
        ptr_ = other.ptr_;
        size_ = other.size_;
        other.ptr_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

void fill_gpu_pattern(void* gpu_ptr, size_t size, uint8_t pattern_byte)
{
    std::vector<uint8_t> host_buf(size, pattern_byte);
    FHE_CHECK_CUDA(cudaMemcpy(gpu_ptr, host_buf.data(), size, cudaMemcpyHostToDevice));
}

bool verify_gpu_data(void* gpu_ptr, size_t size, uint8_t expected_byte)
{
    std::vector<uint8_t> host_buf(size);
    FHE_CHECK_CUDA(cudaMemcpy(host_buf.data(), gpu_ptr, size, cudaMemcpyDeviceToHost));
    for (size_t i = 0; i < size; i++) {
        if (host_buf[i] != expected_byte) {
            return false;
        }
    }
    return true;
}

#endif  // ACE_PHANTOM_GPU_TESTS

}  // namespace rtio::test
