//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief mapped_transport.cpp
//! Host-mapped I/O transport: mmap + cudaMemcpy for reads,
//! chunked D2H + fwrite for writes.

#include "fhe/io/mapped_transport.h"

#include "fhe/io/cuda_check.h"

#include <cuda_runtime.h>
#include <sys/mman.h>
#include <unistd.h>

namespace rtio
{

//=============================================================================
// Constructor
//=============================================================================

MappedTransport::MappedTransport(const std::string& path, const char* mode, size_t chunk_size)
    : FileTransport(path, mode, chunk_size)
{
}

//=============================================================================
// GPU data I/O
//=============================================================================

bool MappedTransport::write_gpu(const void* gpu_ptr, size_t offset, size_t size)
{
    // Write path: same as StagedTransport (D2H chunk + fwrite)
    // mmap-based writes are problematic (msync, dirty page tracking, COW),
    // and the write path is less performance-critical than reads.
    if (!_fd || size == 0)
        return size == 0;

    const uint8_t* gpu_data = static_cast<const uint8_t*>(gpu_ptr);
    size_t remaining = size;
    size_t pos = 0;

    while (remaining > 0) {
        size_t this_chunk = (remaining > _chunk_size) ? _chunk_size : remaining;

        // D2H: copy chunk from GPU to host buffer
        FHE_CHECK_CUDA(cudaMemcpy(_chunk.data(), gpu_data + pos, this_chunk, cudaMemcpyDeviceToHost));

        // Write chunk to file
        if (std::fseek(_fd, static_cast<long>(offset + pos), SEEK_SET) != 0) {
            return false;
        }
        size_t written = std::fwrite(_chunk.data(), 1, this_chunk, _fd);
        if (written != this_chunk)
            return false;

        pos += this_chunk;
        remaining -= this_chunk;
    }

    std::fflush(_fd);

    // Update file size
    size_t end = offset + size;
    if (end > _file_size)
        _file_size = end;

    return true;
}

bool MappedTransport::read_gpu(void* gpu_ptr, size_t offset, size_t size)
{
    // Read path: mmap the file region → chunked cudaMemcpy H2D → munmap
    // This eliminates the fread() copy from page cache to user buffer that
    // StagedTransport performs. The data goes directly from page cache
    // (mmap'd pages) to GPU via cudaMemcpy.
    if (!_fd || size == 0)
        return size == 0;

    // Flush any buffered writes before mmap (safety: ensure FILE* buffer
    // content is visible on disk so mmap sees consistent data)
    std::fflush(_fd);

    // Get raw file descriptor for mmap
    int raw_fd = ::fileno(_fd);
    if (raw_fd < 0)
        return false;

    // Page-align the mmap offset (POSIX requirement)
    long page_size = ::sysconf(_SC_PAGESIZE);
    size_t page_offset = offset & ~static_cast<size_t>(page_size - 1);  // round down
    size_t page_delta = offset - page_offset;                           // how far into the first page
    size_t map_size = size + page_delta;

    // Bounds check: mapped region must not exceed file size
    if (page_offset + map_size > _file_size)
        return false;

    // mmap the region with MAP_PRIVATE (read-only semantics, COW)
    void* mapped = ::mmap(nullptr, map_size, PROT_READ, MAP_PRIVATE, raw_fd, static_cast<off_t>(page_offset));
    if (mapped == MAP_FAILED)
        return false;

    const uint8_t* src = static_cast<const uint8_t*>(mapped) + page_delta;
    uint8_t* dst = static_cast<uint8_t*>(gpu_ptr);

    // Compute CRC on mmap'd data (data is in host memory via page cache,
    // zero extra cost — no additional copy needed)
    if (_crc_active) {
        update_crc(src, size);
    }

    // Chunked cudaMemcpy H2D from mmap'd pages to GPU
    size_t remaining = size;
    size_t pos = 0;
    while (remaining > 0) {
        size_t this_chunk = (remaining > _chunk_size) ? _chunk_size : remaining;

        FHE_CHECK_CUDA(cudaMemcpy(dst + pos, src + pos, this_chunk, cudaMemcpyHostToDevice));

        pos += this_chunk;
        remaining -= this_chunk;
    }

    // Unmap the region
    ::munmap(mapped, map_size);

    return true;
}

}  // namespace rtio
