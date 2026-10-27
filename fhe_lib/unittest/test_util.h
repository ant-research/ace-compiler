//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_util.h
//! Shared test utilities for fhe_lib unittest.
//!
//! Eliminates duplication of make_temp_path(), remove_file(), GPU buffer
//! management, and verification helpers across test files.

#ifndef FHE_TEST_UTIL_H
#define FHE_TEST_UTIL_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

namespace rtio::test
{

//=============================================================================
// Temp file management
//=============================================================================

//! Generate a unique temporary file path.
//! Format: /tmp/fhert_{prefix}_test_{pid}_{counter}_{suffix}
std::string make_temp_path(const char* prefix, const char* suffix);

//! Remove a file (ignores errors if the file does not exist).
void remove_file(const std::string& path);

//! RAII guard that removes a file when destroyed.
class TempFileGuard
{
public:
    explicit TempFileGuard(const std::string& path) : _path(path) {}
    ~TempFileGuard() { std::remove(_path.c_str()); }
    TempFileGuard(const TempFileGuard&) = delete;
    TempFileGuard& operator=(const TempFileGuard&) = delete;

    // Move-only (needed for reassignment in test SetUp)
    TempFileGuard(TempFileGuard&& other) noexcept : _path(std::move(other._path))
    {
        other._path.clear();  // Prevent other from deleting the file
    }
    TempFileGuard& operator=(TempFileGuard&& other) noexcept
    {
        if (this != &other) {
            if (!_path.empty())
                std::remove(_path.c_str());
            _path = std::move(other._path);
            other._path.clear();
        }
        return *this;
    }

    const std::string& path() const { return _path; }

private:
    std::string _path;
};

//! Create a TempFileGuard with a generated temp path.
TempFileGuard make_temp_file(const char* prefix, const char* suffix);

//=============================================================================
// GPU buffer management (only compiled under ACE_PHANTOM_GPU_TESTS)
//=============================================================================

#ifdef ACE_PHANTOM_GPU_TESTS

//! RAII wrapper around cudaMalloc/cudaFree.
//! Implementation in test_util.cpp (which includes cuda_runtime.h
//! outside the rtio::test namespace).
class GpuBuffer
{
public:
    explicit GpuBuffer(size_t size);
    ~GpuBuffer() noexcept;
    GpuBuffer(const GpuBuffer&) = delete;
    GpuBuffer& operator=(const GpuBuffer&) = delete;

    // Move-only
    GpuBuffer(GpuBuffer&& other) noexcept;
    GpuBuffer& operator=(GpuBuffer&& other) noexcept;

    void* ptr() { return ptr_; }
    const void* ptr() const { return ptr_; }
    size_t size() const { return size_; }

private:
    void* ptr_ = nullptr;
    size_t size_ = 0;
};

//! Fill a GPU buffer with a repeated byte pattern (H2D memcpy).
void fill_gpu_pattern(void* gpu_ptr, size_t size, uint8_t pattern_byte);

//! Verify that a GPU buffer's contents match the expected byte pattern.
//! @return true if all bytes match, false otherwise
bool verify_gpu_data(void* gpu_ptr, size_t size, uint8_t expected_byte);

#endif  // ACE_PHANTOM_GPU_TESTS

}  // namespace rtio::test

#endif  // FHE_TEST_UTIL_H
