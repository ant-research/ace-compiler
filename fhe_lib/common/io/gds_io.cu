//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief gds_io.cu
//! GPU Direct Storage (GDS / cuFile) I/O implementation.
//!
//! This file provides:
//!   - gds_env_enabled(): legacy environment variable check
//!   - gds_parallel_read(): internal parallel cuFile read engine used by
//!     GdsTransport for multi-threaded reads
//!
//! All cuFile symbols are resolved at runtime via dlopen/dlsym (see
//! cufile_api.h). There are no compile-time #ifdef branches — a single
//! binary works with or without cuFile installed.
//!
//! The GdsTransport class (gds_transport.cpp) uses gds_parallel_read()
//! for GPU reads and implements write directly with the cuFile API.

#include "fhe/io/gds_io.h"

#include <atomic>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace rtio
{

//=============================================================================
// Environment variable control (legacy)
//=============================================================================

int gds_env_enabled()
{
    const char* env = std::getenv("ACE_GDS_ENABLED");
    if (env == nullptr) {
        return -1;  // not set → auto-detect
    }
    int val = std::atoi(env);
    return val ? 1 : 0;  // "1" → force-enable, "0" → force-disable
}

bool gds_driver_available()
{
    return GdsDriver::instance().available();
}

//=============================================================================
// Internal: parallel cuFile read with multiple handles
//=============================================================================

//! Perform a parallel chunked cuFile read using pre-registered handles.
//!
//! Each handle (and its underlying fd) submits I/O to the NVMe independently.
//! An atomic chunk dispenser distributes work across handles, so N handles
//! approximate N× the submission queue depth — matching the gdsio benchmark
//! pattern for maximum throughput.
//!
//! @param api          cuFile API function pointers
//! @param handles      Array of registered cuFile handles
//! @param num_handles  Number of handles in the array
//! @param gpu_ptr      Destination GPU buffer (must be cudaMalloc'd + BufRegistered)
//! @param offset       Byte offset in the file to start reading
//! @param size         Number of bytes to read
//! @param chunk_size   Chunk size for each cuFileRead request (default 8 MiB)
//! @return true if read succeeded, false on error
bool gds_parallel_read(const CuFileApi& api, void** handles, int num_handles, void* gpu_ptr, size_t offset, size_t size,
                       size_t chunk_size)
{
    if (size == 0)
        return true;
    if (num_handles <= 0)
        return false;

    const size_t cs = (chunk_size > 0) ? chunk_size : 8UL * 1024 * 1024;
    const size_t num_chunks = (size + cs - 1) / cs;

    // Single-handle path: no thread overhead
    if (num_handles == 1) {
        CUfileHandle_t fh = static_cast<CUfileHandle_t>(handles[0]);
        char* ptr = static_cast<char*>(gpu_ptr);
        size_t remaining = size;
        size_t current_offset = offset;
        while (remaining > 0) {
            size_t this_chunk = std::min(cs, remaining);
            ssize_t bytes_read = api.Read(fh, ptr, this_chunk, static_cast<off_t>(current_offset), 0);
            if (bytes_read < 0)
                return false;
            ssize_t done = bytes_read;
            while (static_cast<size_t>(done) < this_chunk) {
                ssize_t more = api.Read(fh, ptr + done, this_chunk - static_cast<size_t>(done),
                                        static_cast<off_t>(current_offset + done), 0);
                if (more <= 0)
                    return false;
                done += more;
            }
            ptr += this_chunk;
            current_offset += this_chunk;
            remaining -= this_chunk;
        }
        return true;
    }

    // Multi-handle parallel chunked read
    std::atomic<bool> read_error{false};
    std::atomic<size_t> next_chunk{0};

    auto worker = [&](int tid) {
        CUfileHandle_t fh = static_cast<CUfileHandle_t>(handles[tid]);
        size_t ci;
        while ((ci = next_chunk.fetch_add(1)) < num_chunks) {
            if (read_error.load(std::memory_order_relaxed))
                break;

            size_t chunk_offset = offset + ci * cs;
            size_t chunk_bytes = std::min(cs, size - ci * cs);
            char* chunk_ptr = static_cast<char*>(gpu_ptr) + ci * cs;

            ssize_t bytes_read = api.Read(fh, chunk_ptr, chunk_bytes, static_cast<off_t>(chunk_offset), 0);
            if (bytes_read < 0) {
                read_error.store(true, std::memory_order_relaxed);
                break;
            }
            ssize_t remaining_chunk = static_cast<ssize_t>(chunk_bytes) - bytes_read;
            while (remaining_chunk > 0 && !read_error.load(std::memory_order_relaxed)) {
                ssize_t more = api.Read(fh, chunk_ptr + (chunk_bytes - remaining_chunk),
                                        static_cast<size_t>(remaining_chunk),
                                        static_cast<off_t>(chunk_offset + (chunk_bytes - remaining_chunk)), 0);
                if (more <= 0) {
                    read_error.store(true, std::memory_order_relaxed);
                    break;
                }
                remaining_chunk -= more;
            }
        }
    };

    // Launch workers: one per cuFileHandle, each on its own thread
    std::vector<std::thread> w_threads;
    w_threads.reserve(num_handles - 1);
    for (int t = 1; t < num_handles; t++) {
        w_threads.emplace_back(worker, t);
    }
    worker(0);  // Main thread runs worker 0
    for (auto& th : w_threads)
        th.join();

    return !read_error.load();
}

}  // namespace rtio
