//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief io_bulk_reader.h
//! Multi-file parallel read executor for bulk IO operations.
//!
//! IoBulkReader encapsulates the pattern of reading multiple files from
//! multiple NVMe disks in parallel, which is the core pattern for:
//!   - Scenario 1 (startup load): loading all galois key sub-files at once
//!   - Scenario 2 (runtime load): loading a single data file on demand
//!
//! Files are auto-grouped by disk based on path prefix matching against
//! a list of disk_paths. Each disk group is read in parallel by a
//! dedicated thread, with files on the same disk read sequentially.
//!
//! Usage (Scenario 1 — bulk load):
//!   rtio::IoBulkReader reader(rtio::IoPolicy::auto_,
//!                              {"/mnt/nvme0", "/mnt/nvme1"}, 1);
//!   std::vector<rtio::BulkReadFile> files = {
//!       {"/mnt/nvme0/data/part0.bin", 1ULL << 30},
//!       {"/mnt/nvme1/data/part1.bin", 1ULL << 30},
//!   };
//!   void* bufs[2];
//!   cudaMalloc(&bufs[0], 1ULL << 30);
//!   cudaMalloc(&bufs[1], 1ULL << 30);
//!   auto result = reader.read(files, {bufs[0], bufs[1]});
//!   if (result.success) printf("%.2f GiB/s\n", result.gib_per_second());
//!
//! Usage (Scenario 2 — single file):
//!   rtio::IoBulkReader reader(rtio::IoPolicy::auto_);
//!   auto result = reader.read("/mnt/nvme0/data/key.bin", buf, size);

#ifndef FHE_IO_BULK_READER_H
#define FHE_IO_BULK_READER_H

#include "fhe/io/io_policy.h"

#include <cstddef>
#include <string>
#include <vector>

namespace rtio
{

//=============================================================================
// BulkReadFile — description of a file to read
//=============================================================================

//! Description of a single file to read in a bulk operation.
struct BulkReadFile
{
    std::string path;  //!< File path on disk
    size_t size;       //!< Expected file size in bytes (0 = read entire file)
};

//=============================================================================
// BulkReadResult — outcome of a bulk read operation
//=============================================================================

//! Result of a bulk read operation.
struct BulkReadResult
{
    bool success = false;          //!< Whether all reads succeeded
    double elapsed_seconds = 0.0;  //!< Wall-clock time for the read phase
    size_t total_bytes = 0;        //!< Total bytes read across all files
    std::string transport_name;    //!< Transport name of the first reader (e.g., "gds+mt")

    //! Throughput in GiB/s (1 GiB = 1024^3 bytes)
    double gib_per_second() const
    {
        if (elapsed_seconds <= 0.0)
            return 0.0;
        const double gib = 1024.0 * 1024.0 * 1024.0;
        return static_cast<double>(total_bytes) / gib / elapsed_seconds;
    }
};

//=============================================================================
// IoBulkReader — multi-file parallel read executor
//=============================================================================

//! Multi-file parallel read executor.
//!
//! Groups files by disk (based on path prefix matching against disk_paths)
//! and reads each disk group in parallel using a dedicated thread. Files
//! on the same disk are read sequentially with open_reader() →
//! register_buffer → read_gpu → deregister_all lifecycle.
//!
//! Two read modes:
//!   - read(): Each file gets its own GPU buffer (different buffers).
//!     Used for key load where each key component needs its own buffer.
//!   - read_reuse_buffer(): Per-disk GPU buffer reused across files on
//!     the same disk. Used for benchmarking with constrained VRAM.
class IoBulkReader
{
public:
    //! Construct a bulk reader for parallel multi-file reading.
    //! @param policy            IO policy (determines transport selection)
    //! @param disk_paths        Multi-disk root paths for file grouping.
    //!                          Files whose path starts with disk_paths[d]
    //!                          are assigned to disk group d.
    //! @param num_io_threads    Number of IO threads per reader (default 1
    //!                          for v3 multi-file format where each sub-file
    //!                          already provides an independent NVMe queue)
    IoBulkReader(IoPolicy policy = IoPolicy::auto_, const std::vector<std::string>& disk_paths = {},
                 int num_io_threads = 1);

    //! Read multiple files into separate GPU buffers in parallel.
    //! @param files    File list with paths and expected sizes
    //! @param buffers  Pre-allocated GPU buffers (one per file, each cudaMalloc'd)
    //! @return Result with timing, total bytes, and success
    //!
    //! One thread per disk group reads all files on that disk sequentially.
    //! Disk groups run in parallel. The caller must ensure buffers[i] has
    //! at least files[i].size bytes allocated.
    BulkReadResult read(const std::vector<BulkReadFile>& files, const std::vector<void*>& buffers);

    //! Read a single file into a GPU buffer.
    //! @param path    File path
    //! @param buffer  Pre-allocated GPU buffer
    //! @param size    Number of bytes to read
    //! @return Result with timing and success
    BulkReadResult read(const std::string& path, void* buffer, size_t size);

    //! Read multiple files with per-disk buffer reuse (same-buffer mode).
    //! @param files       File list with paths and expected sizes
    //! @param disk_bufs   GPU buffers, one per disk group (indexed by disk group)
    //! @param disk_buf_size Size of each per-disk buffer
    //! @return Result with timing, total bytes, and success
    //!
    //! Files on the same disk are read sequentially into the same buffer.
    //! This is useful for benchmarking where VRAM is constrained and
    //! each file's data is processed before reading the next.
    BulkReadResult read_reuse_buffer(const std::vector<BulkReadFile>& files, const std::vector<void*>& disk_bufs,
                                     size_t disk_buf_size);

private:
    //! Determine which disk group a file belongs to based on path prefix.
    //! Returns the index into _disk_paths, or 0 if no match.
    int disk_group_for_path(const std::string& path) const;

    IoPolicy _policy;
    std::vector<std::string> _disk_paths;
    int _num_io_threads;
};

}  // namespace rtio

#endif  // FHE_IO_BULK_READER_H
