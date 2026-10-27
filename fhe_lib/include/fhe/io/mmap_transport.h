//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief mmap_transport.h
//! Host-mapped I/O transport: mmap + cudaMemcpy for GPU reads,
//! chunked D2H + fwrite for GPU writes.
//!
//! MmapTransport maps file regions into the process address space via mmap,
//! then uses cudaMemcpy to transfer from the mapped pages to GPU memory.
//! This eliminates the fread() copy that StagedTransport performs, reducing
//! the read data path by one copy:
//!
//!   StagedTransport: SSD → page cache → user buf → GPU  (fread + cudaMemcpy)
//!   MmapTransport: SSD → page cache → GPU             (mmap  + cudaMemcpy)
//!
//! Write path is identical to StagedTransport (D2H chunk + fwrite) because
//! mmap-based writes are problematic (msync, dirty page tracking, COW).
//!
//! Note: Only read_gpu() uses mmap. Host reads still use fread (small data
//! like headers doesn't benefit from mmap overhead).
//!
//! CRC32 is computed on the mmap'd region (data is in host memory via page
//! cache), unlike GdsTransport where data bypasses host memory entirely.

#ifndef FHE_MMAP_TRANSPORT_H
#define FHE_MMAP_TRANSPORT_H

#include "fhe/io/file_transport.h"

namespace rtio
{

//=============================================================================
// MmapTransport
//=============================================================================

//! I/O transport using mmap for GPU reads and POSIX file I/O for writes.
//!
//! Write path (same as StagedTransport, inherited from FileTransport):
//!   write():       POSIX fseek + fwrite for host data
//!   write_gpu():   D2H chunks → fwrite for GPU data
//!
//! Read path:
//!   read():        POSIX fseek + fread for host data (inherited)
//!   read_gpu():    mmap the file region → chunked cudaMemcpy H2D → munmap
//!   CRC32:         accumulated on the mmap'd data before cudaMemcpy
//!
//! Peak host memory: ~4 MB (one chunk buffer for write_gpu), regardless of
//! data size.  read_gpu does not use the chunk buffer — it reads directly
//! from the mmap'd pages.
class MmapTransport : public FileTransport
{
public:
    //! Open a file for mapped I/O.
    //! @param path       File path
    //! @param mode       "wb" (write) or "rb" (read)
    //! @param chunk_size Host buffer size for write_gpu streaming (default 4 MB)
    MmapTransport(const std::string& path, const char* mode, size_t chunk_size = 4UL * 1024 * 1024);

    //-------------------------------------------------------------------------
    // GPU data I/O
    //-------------------------------------------------------------------------

    bool write_gpu(const void* gpu_ptr, size_t offset, size_t size) override;
    bool read_gpu(void* gpu_ptr, size_t offset, size_t size) override;

    bool used_gds() const override { return false; }
    const char* transport_name() const override { return "mmap"; }
};

}  // namespace rtio

#endif  // FHE_MMAP_TRANSPORT_H
