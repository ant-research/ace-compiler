//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief staged_transport.cpp
//! Host-staged I/O transport implementation with chunked streaming.

#include "fhe/io/staged_transport.h"

#include "fhe/io/cuda_check.h"

#include <cuda_runtime.h>

namespace rtio
{

//=============================================================================
// Constructor
//=============================================================================

StagedTransport::StagedTransport(const std::string& path, const char* mode, size_t chunk_size)
    : FileTransport(path, mode, chunk_size)
{
}

//=============================================================================
// GPU data I/O (chunked streaming)
//=============================================================================

bool StagedTransport::write_gpu(const void* gpu_ptr, size_t offset, size_t size)
{
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

bool StagedTransport::read_gpu(void* gpu_ptr, size_t offset, size_t size)
{
    if (!_fd || size == 0)
        return size == 0;

    uint8_t* gpu_data = static_cast<uint8_t*>(gpu_ptr);
    size_t remaining = size;
    size_t pos = 0;

    while (remaining > 0) {
        size_t this_chunk = (remaining > _chunk_size) ? _chunk_size : remaining;

        // Read chunk from file
        if (std::fseek(_fd, static_cast<long>(offset + pos), SEEK_SET) != 0) {
            return false;
        }
        size_t nread = std::fread(_chunk.data(), 1, this_chunk, _fd);
        if (nread != this_chunk)
            return false;

        // Accumulate CRC if active (data is in host buffer anyway)
        if (_crc_active) {
            update_crc(_chunk.data(), this_chunk);
        }

        // H2D: copy chunk from host buffer to GPU
        FHE_CHECK_CUDA(cudaMemcpy(gpu_data + pos, _chunk.data(), this_chunk, cudaMemcpyHostToDevice));

        pos += this_chunk;
        remaining -= this_chunk;
    }

    return true;
}

}  // namespace rtio
