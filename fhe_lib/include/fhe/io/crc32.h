//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief crc32.h
//! CRC32 checksum utility for FHE key/data serialization.
//!
//! Provides a stateful, composable CRC32 class that supports incremental
//! updates from both host and GPU data. Lives in rtio:: alongside IoTransport
//! — it's a generic integrity utility with no FHE semantics.

#ifndef RTIO_CRC32_H
#define RTIO_CRC32_H

#include <cstddef>
#include <cstdint>

namespace rtio
{

//=============================================================================
// Crc32 — Stateful CRC32 checksum
//=============================================================================

class Crc32
{
public:
    Crc32() { reset(); }

    /// Reset to initial state
    void reset();

    /// Feed host data into the CRC accumulator
    void update(const void* data, size_t len);

    /// Feed GPU data (streams D2H in chunks, feeds CRC)
    void update_gpu(const void* gpu_ptr, size_t len, size_t chunk_size = 4UL * 1024 * 1024);

    /// Finalize and return CRC value
    uint32_t finalize() const;

    /// One-shot: compute CRC of host data
    static uint32_t compute(const void* data, size_t len);

    /// One-shot: compute CRC of GPU data
    static uint32_t compute_gpu(const void* gpu_ptr, size_t len, size_t chunk_size = 4UL * 1024 * 1024);

private:
    uint32_t _crc;
    static uint32_t table_[256];
    static bool _table_ready;
    static void table_init();
};

}  // namespace rtio

#endif  // RTIO_CRC32_H
