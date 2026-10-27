//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief io_bulk_reader.cpp
//! Multi-file parallel read executor implementation.

#include "fhe/io/io_bulk_reader.h"

#include "fhe/io/transport.h"
#include "fhe/util/logging.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <cuda_runtime.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

namespace rtio
{

//=============================================================================
// Constructor
//=============================================================================

IoBulkReader::IoBulkReader(IoPolicy policy, const std::vector<std::string>& disk_paths, int num_io_threads)
    : _policy(policy), _disk_paths(disk_paths), _num_io_threads(num_io_threads > 0 ? num_io_threads : 1)
{
}

//=============================================================================
// Disk group assignment
//=============================================================================

int IoBulkReader::disk_group_for_path(const std::string& path) const
{
    // Match the longest disk_path prefix
    int best_group = 0;
    size_t best_len = 0;
    for (int d = 0; d < static_cast<int>(_disk_paths.size()); d++) {
        const auto& dp = _disk_paths[d];
        if (dp.size() > best_len && path.size() >= dp.size() && path.compare(0, dp.size(), dp) == 0) {
            best_len = dp.size();
            best_group = d;
        }
    }
    return best_group;
}

//=============================================================================
// read() — parallel multi-file with per-file buffers
//=============================================================================

BulkReadResult IoBulkReader::read(const std::vector<BulkReadFile>& files, const std::vector<void*>& buffers)
{
    BulkReadResult result;
    result.success = false;
    result.total_bytes = 0;

    if (files.empty()) {
        result.success = true;
        return result;
    }

    if (files.size() != buffers.size()) {
        spdlog::error("[io] read(): files.size()={} != buffers.size()={}", files.size(), buffers.size());
        return result;
    }

    // Resolve IoConfig for the policy
    IoConfig cfg;  // Use defaults from open_reader's resolve
    // We call open_reader which does resolve_io_config internally

    // Group files by disk
    int num_disks = std::max(1, static_cast<int>(_disk_paths.size()));
    std::vector<std::vector<size_t>> groups(num_disks);
    for (size_t i = 0; i < files.size(); i++) {
        int g = disk_group_for_path(files[i].path);
        groups[g].push_back(i);
    }

    // Count active groups (those with files)
    int active_groups = 0;
    for (auto& g : groups) {
        if (!g.empty())
            active_groups++;
    }

    // Single-file shortcut
    if (files.size() == 1) {
        auto t0 = std::chrono::high_resolution_clock::now();
        auto reader = open_reader(files[0].path, IoLoc::Device, _policy);
        if (!reader) {
            spdlog::error("[io] Failed to open reader for {}", files[0].path);
            return result;
        }
        reader->register_buffer(buffers[0], files[0].size);
        bool ok = reader->read_gpu(buffers[0], 0, files[0].size);
        // Get transport name AFTER read — _used_gds is set during read_gpu
        result.transport_name = reader->transport_name();
        reader->deregister_all();
        auto t1 = std::chrono::high_resolution_clock::now();
        result.elapsed_seconds = std::chrono::duration<double>(t1 - t0).count();
        result.total_bytes = files[0].size;
        result.success = ok;
        return result;
    }

    // Multi-file parallel read: one thread per disk group
    std::atomic<bool> failed{false};
    std::string first_transport;

    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    threads.reserve(active_groups);

    for (int g = 0; g < num_disks; g++) {
        if (groups[g].empty())
            continue;

        threads.emplace_back([this, &groups, &files, &buffers, g, &failed, &first_transport]() {
            cudaSetDevice(0);  // Ensure CUDA context in this thread

            for (size_t idx : groups[g]) {
                if (failed.load(std::memory_order_relaxed))
                    return;

                auto reader = open_reader(files[idx].path, IoLoc::Device, _policy);
                if (!reader) {
                    spdlog::error("[io] Failed to open reader for {}", files[idx].path);
                    failed.store(true, std::memory_order_relaxed);
                    return;
                }

                reader->register_buffer(buffers[idx], files[idx].size);
                if (!reader->read_gpu(buffers[idx], 0, files[idx].size)) {
                    spdlog::error("[io] read_gpu failed for {}", files[idx].path);
                    reader->deregister_all();
                    failed.store(true, std::memory_order_relaxed);
                    return;
                }
                // Get transport name AFTER read — _used_gds is set during read_gpu
                if (g == 0 && idx == groups[0][0]) {
                    first_transport = reader->transport_name();
                }
                reader->deregister_all();
            }
        });
    }

    for (auto& t : threads)
        t.join();

    auto t1 = std::chrono::high_resolution_clock::now();
    result.elapsed_seconds = std::chrono::duration<double>(t1 - t0).count();
    result.total_bytes = 0;
    for (const auto& f : files)
        result.total_bytes += f.size;
    result.transport_name = first_transport;
    result.success = !failed.load();

    return result;
}

//=============================================================================
// read() — single file convenience
//=============================================================================

BulkReadResult IoBulkReader::read(const std::string& path, void* buffer, size_t size)
{
    BulkReadFile file;
    file.path = path;
    file.size = size;
    return read({file}, {buffer});
}

//=============================================================================
// read_reuse_buffer() — per-disk buffer reuse
//=============================================================================

BulkReadResult IoBulkReader::read_reuse_buffer(const std::vector<BulkReadFile>& files,
                                               const std::vector<void*>& disk_bufs, size_t disk_buf_size)
{
    BulkReadResult result;
    result.success = false;
    result.total_bytes = 0;

    if (files.empty()) {
        result.success = true;
        return result;
    }

    // Group files by disk
    int num_disks = std::max(1, static_cast<int>(_disk_paths.size()));
    std::vector<std::vector<size_t>> groups(num_disks);
    for (size_t i = 0; i < files.size(); i++) {
        int g = disk_group_for_path(files[i].path);
        groups[g].push_back(i);
    }

    if (static_cast<int>(disk_bufs.size()) < num_disks) {
        spdlog::error("[io] read_reuse_buffer(): disk_bufs.size()={} < num_disks={}", disk_bufs.size(), num_disks);
        return result;
    }

    // Multi-disk parallel read: one thread per disk, reusing same buffer
    std::atomic<bool> failed{false};
    std::string first_transport;
    size_t total_bytes = 0;

    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int g = 0; g < num_disks; g++) {
        if (groups[g].empty())
            continue;

        threads.emplace_back(
            [this, &groups, &files, &disk_bufs, disk_buf_size, g, &failed, &first_transport, &total_bytes]() {
                cudaSetDevice(0);  // Ensure CUDA context in this thread

                void* buf = disk_bufs[g];

                for (size_t idx : groups[g]) {
                    if (failed.load(std::memory_order_relaxed))
                        return;

                    size_t file_size = files[idx].size;
                    if (file_size > disk_buf_size) {
                        spdlog::error("[io] File {} ({} bytes) exceeds disk buffer size ({})", files[idx].path,
                                      file_size, disk_buf_size);
                        failed.store(true, std::memory_order_relaxed);
                        return;
                    }

                    auto reader = open_reader(files[idx].path, IoLoc::Device, _policy);
                    if (!reader) {
                        spdlog::error("[io] Failed to open reader for {}", files[idx].path);
                        failed.store(true, std::memory_order_relaxed);
                        return;
                    }

                    reader->register_buffer(buf, disk_buf_size);
                    if (!reader->read_gpu(buf, 0, file_size)) {
                        spdlog::error("[io] read_gpu failed for {}", files[idx].path);
                        reader->deregister_all();
                        failed.store(true, std::memory_order_relaxed);
                        return;
                    }
                    // Get transport name AFTER read — _used_gds is set during read_gpu
                    if (first_transport.empty()) {
                        first_transport = reader->transport_name();
                    }
                    reader->deregister_all();
                }
            });
    }

    for (auto& t : threads)
        t.join();

    auto t1 = std::chrono::high_resolution_clock::now();
    result.elapsed_seconds = std::chrono::duration<double>(t1 - t0).count();
    for (const auto& f : files)
        result.total_bytes += f.size;
    result.transport_name = first_transport;
    result.success = !failed.load();

    return result;
}

}  // namespace rtio
