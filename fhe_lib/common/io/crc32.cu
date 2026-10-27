//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief crc32.cu
//! CRC32 checksum utility implementation.

#include "fhe/io/crc32.h"
#include "fhe/io/cuda_check.h"

#include <cuda_runtime.h>
#include <vector>

namespace rtio
{

//=============================================================================
// Static data
//=============================================================================

uint32_t Crc32::table_[256] = {};
bool Crc32::_table_ready = false;

//=============================================================================
// Table initialization
//=============================================================================

void Crc32::table_init()
{
    if (_table_ready)
        return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        table_[i] = crc;
    }
    _table_ready = true;
}

//=============================================================================
// Instance methods
//=============================================================================

void Crc32::reset()
{
    table_init();
    _crc = 0xFFFFFFFF;
}

void Crc32::update(const void* data, size_t len)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < len; i++) {
        _crc = (_crc >> 8) ^ table_[(_crc ^ bytes[i]) & 0xFF];
    }
}

void Crc32::update_gpu(const void* gpu_ptr, size_t len, size_t chunk_size)
{
    if (len == 0)
        return;
    if (chunk_size == 0)
        chunk_size = 4UL * 1024 * 1024;

    std::vector<uint8_t> host_chunk(chunk_size);
    const uint8_t* gpu_data = static_cast<const uint8_t*>(gpu_ptr);

    size_t remaining = len;
    size_t offset = 0;
    while (remaining > 0) {
        size_t this_chunk = (remaining > chunk_size) ? chunk_size : remaining;
        FHE_CHECK_CUDA(cudaMemcpy(host_chunk.data(), gpu_data + offset, this_chunk, cudaMemcpyDeviceToHost));
        for (size_t i = 0; i < this_chunk; i++) {
            _crc = (_crc >> 8) ^ table_[(_crc ^ host_chunk[i]) & 0xFF];
        }
        offset += this_chunk;
        remaining -= this_chunk;
    }
}

uint32_t Crc32::finalize() const
{
    uint32_t result = _crc ^ 0xFFFFFFFF;
    return result;
}

//=============================================================================
// Static convenience methods
//=============================================================================

uint32_t Crc32::compute(const void* data, size_t len)
{
    Crc32 crc;
    crc.update(data, len);
    return crc.finalize();
}

uint32_t Crc32::compute_gpu(const void* gpu_ptr, size_t len, size_t chunk_size)
{
    Crc32 crc;
    crc.update_gpu(gpu_ptr, len, chunk_size);
    return crc.finalize();
}

}  // namespace rtio
