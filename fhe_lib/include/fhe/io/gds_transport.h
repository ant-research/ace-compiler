//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief gds_transport.h
//! GDS (cuFile) I/O transport for direct SSD ↔ GPU HBM transfers.
//!
//! GdsTransport uses NVIDIA cuFile for GPU data I/O and POSIX for small
//! host data (headers, metadata). If cuFile read/write fails and
//! fallback_on_error is true, falls back to POSIX + cudaMemcpy.
//! Multi-threaded reads use gds_parallel_read() internally.

#ifndef FHE_GDS_TRANSPORT_H
#define FHE_GDS_TRANSPORT_H

#include "fhe/io/transport.h"

#include <cstdio>
#include <string>

namespace rtio
{

//=============================================================================
// GdsTransport
//=============================================================================

//! I/O transport using GPUDirect Storage (cuFile) for GPU data.
//!
//! Write path:
//!   write():       POSIX fwrite for host data
//!   write_gpu():   cuFileWrite for GPU data (single-threaded, with POSIX fallback)
//!
//! Read path:
//!   read():        POSIX fread for host data
//!   read_gpu():    cuFileRead via gds_parallel_read() for GPU data (multi-threaded,
//!                  with POSIX+cudaMemcpy fallback)
//!   CRC32:         NOT computed (returns 0) — hardware ECC protects path
//!
//! Peak host memory: ~0 for GPU data. Small buffers for header I/O only.
//!
//! IMPORTANT: Do not construct directly — use create_io_transport() which
//! checks cuFile availability and falls back to StagedTransport if needed.
class GdsTransport : public IoTransport
{
public:
    //! Open a file for GDS I/O.
    //! @param path  File path
    //! @param mode  "wb" (write) or "rb" (read)
    //! @param fallback_on_error  If true, GDS I/O errors trigger POSIX fallback
    //! @param chunk_size  I/O request size for cuFile (default 8 MiB)
    //! @param num_io_threads  Number of parallel I/O threads (default 4)
    //! @throws std::runtime_error if the file cannot be opened
    explicit GdsTransport(const std::string& path, const char* mode, bool fallback_on_error = false,
                          size_t chunk_size = 8UL * 1024 * 1024, int num_io_threads = 4);

    //! Close the file and release resources.
    ~GdsTransport() override;

    // Non-copyable, non-movable
    GdsTransport(const GdsTransport&) = delete;
    GdsTransport& operator=(const GdsTransport&) = delete;

    //-------------------------------------------------------------------------
    // IoTransport interface
    //-------------------------------------------------------------------------

    bool write(const void* data, size_t offset, size_t size) override;
    bool read(void* buf, size_t offset, size_t size) override;
    bool write_gpu(const void* gpu_ptr, size_t offset, size_t size) override;
    bool read_gpu(void* gpu_ptr, size_t offset, size_t size) override;

    void reset_crc() override;
    uint32_t crc32() const override;

    const std::string& path() const override { return _path; }
    size_t file_size() const override { return _file_size; }
    bool used_gds() const override { return _used_gds; }
    const char* transport_name() const override;

    //! Register a GPU buffer for cuFile I/O.
    //! Calls cuFileBufRegister internally. Must be called after cudaMalloc
    //! and before read_gpu/write_gpu.
    bool register_buffer(void* gpu_ptr, size_t size) override;

    //! Deregister all previously registered GPU buffers.
    void deregister_all() override;

private:
    std::string _path;
    FILE* _fd = nullptr;
    size_t _file_size = 0;
    bool _used_gds = false;                  //!< Whether cuFile was actually used
    bool _fallback_on_error = false;         //!< Whether to fall back to POSIX on GDS error
    bool _fallback_logged = false;           //!< Whether GDS→POSIX fallback was already logged for this file
    size_t _chunk_size = 8UL * 1024 * 1024;  //!< cuFile I/O request size (8 MiB default)
    int _num_io_threads = 4;                 //!< Number of parallel I/O threads

    //! Registered GPU buffer tracking for deregistration
    struct RegisteredBuf
    {
        void* ptr;
        size_t size;
    };
    std::vector<RegisteredBuf> _registered_bufs;
};

}  // namespace rtio

#endif  // FHE_GDS_TRANSPORT_H
