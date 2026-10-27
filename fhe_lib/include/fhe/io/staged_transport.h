//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief staged_transport.h
//! Host-staged I/O transport with chunked streaming.
//!
//! StagedTransport uses fixed-size host buffers (~4 MB) to stream data
//! between host memory and files, and between GPU memory and files via
//! cudaMemcpy. This avoids the OOM risk of allocating a full-file host
//! buffer (galois_key.bin can be 19 GB).
//!
//! CRC32 is automatically accumulated during read operations at zero extra
//! cost — data passes through the host chunk buffer anyway.

#ifndef FHE_STAGED_TRANSPORT_H
#define FHE_STAGED_TRANSPORT_H

#include "fhe/io/file_transport.h"

namespace rtio
{

//=============================================================================
// StagedTransport
//=============================================================================

//! I/O transport using POSIX file I/O with fixed-size host chunk buffers.
//!
//! Write path:
//!   write():       POSIX fseek + fwrite for host data (headers, metadata)
//!   write_gpu():   4 MB D2H chunks → fwrite for GPU data
//!
//! Read path:
//!   read():        POSIX fseek + fread for host data
//!   read_gpu():    fread chunk → H2D cudaMemcpy for GPU data
//!   CRC32:         accumulated automatically during read/read_gpu
//!
//! Peak host memory: ~4 MB (one chunk buffer), regardless of data size.
class StagedTransport : public FileTransport
{
public:
    //! Open a file for staged I/O.
    //! @param path       File path
    //! @param mode       "wb" (write) or "rb" (read)
    //! @param chunk_size Host buffer size for streaming (default 4 MB)
    StagedTransport(const std::string& path, const char* mode, size_t chunk_size = 4UL * 1024 * 1024);

    //-------------------------------------------------------------------------
    // GPU data I/O (staged: fread + cudaMemcpy)
    //-------------------------------------------------------------------------

    bool write_gpu(const void* gpu_ptr, size_t offset, size_t size) override;
    bool read_gpu(void* gpu_ptr, size_t offset, size_t size) override;

    bool used_gds() const override { return false; }
    const char* transport_name() const override { return "staged"; }
};

}  // namespace rtio

#endif  // FHE_STAGED_TRANSPORT_H
