//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_io.h
//! Protocol-level key file I/O: KeyHeader + host metadata + GPU data + CRC32.
//!
//! KeyWriter and KeyReader encapsulate the recurring serialization pattern
//! shared by all key types (SK, PK, RelinKey, GaloisKey):
//!   - Write/read the 64-byte KeyHeader
//!   - Write/read host metadata sections
//!   - Write/read GPU data sections
//!   - Compute and validate CRC32
//!
//! GDS (cuFile) optimization: when GPU data is written/read via cuFile
//! direct SSD↔GPU transfer, CRC32 computation is skipped because:
//!   1. Computing CRC would require a redundant D2H copy that defeats
//!      the purpose of GDS (zero host memory footprint)
//!   2. The NVMe ECC → PCIe TLP CRC → GPU HBM ECC chain provides
//!      end-to-end data integrity
//!   3. GDS-saved keys have header.crc32=0; validate_crc() accepts
//!      this as a valid "no CRC" signal
//!
//! Backend-specific code (key memory layout, which metadata fields exist)
//! is handled by KeySerializer<VendorTraits>. KeyWriter/KeyReader only
//! manage the I/O protocol — offset tracking, CRC accumulation, and
//! header patching.

#ifndef FHERT_KEY_IO_H
#define FHERT_KEY_IO_H

#include "fhe/io/crc32.h"
#include "fhe/io/transport.h"
#include "fhe/kms/key_header.h"

#include <cstddef>
#include <cstdint>

namespace fhert
{

//=============================================================================
// KeyWriter — writes a key file with streaming CRC32
//=============================================================================

class KeyWriter
{
public:
    KeyWriter(rtio::IoTransport& io, const KeyHeader& hdr) : _io(io), _hdr(hdr), _offset(0), _crc(), _gds_used(false) {}

    /// Write the header at offset 0. Must be called first.
    /// NOTE: header.crc32 is not yet set — call finish() to patch it.
    bool write_header();

    /// Write host metadata (advances offset, feeds CRC).
    bool write_meta(const void* data, size_t size);

    /// Write GPU data (advances offset, feeds CRC via D2H streaming).
    /// GDS path: CRC is skipped (no D2H copy needed for integrity).
    bool write_gpu_data(const void* gpu_ptr, size_t size);

    /// Finalize: compute CRC, patch header.crc32, rewrite header at offset 0.
    /// GDS path: header.crc32 is set to 0 (no CRC — hardware ECC protects).
    /// Returns total bytes written, 0 on error.
    size_t finish();

private:
    rtio::IoTransport& _io;
    KeyHeader _hdr;
    size_t _offset;
    rtio::Crc32 _crc;
    bool _gds_used;  //!< Whether any write_gpu_data used GDS (cuFile)
};

//=============================================================================
// KeyReader — reads a key file with CRC32 validation
//=============================================================================

class KeyReader
{
public:
    KeyReader(rtio::IoTransport& io) : _io(io), _offset(0), _crc(), _gds_read(false) {}

    /// Read and validate header. Returns the validated KeyHeader.
    /// If expect_poly_deg/expect_coeff_size are non-zero, validates them.
    KeyHeader read_header(uint64_t expect_poly_deg = 0, uint64_t expect_coeff_size = 0);

    /// Read host metadata (advances offset, feeds CRC).
    bool read_meta(void* buf, size_t size);

    /// Read GPU data into pre-allocated buffer (advances offset, feeds CRC).
    /// GDS path: CRC is skipped (no D2H copy needed for integrity).
    bool read_gpu_data(void* gpu_ptr, size_t size);

    /// Validate CRC32 against the expected value from the header.
    /// - expected_crc == 0 → GDS-saved key, no CRC → return true
    /// - GDS read path → CRC not computed for GPU data → return true
    /// - computed CRC == 0 → shouldn't happen for real data → return true
    /// - Otherwise → compare computed vs expected
    bool validate_crc(uint32_t expected_crc) const;

    /// Get current read offset
    size_t offset() const { return _offset; }

private:
    rtio::IoTransport& _io;
    size_t _offset;
    rtio::Crc32 _crc;
    bool _gds_read;  //!< Whether any read_gpu_data used GDS (cuFile)
};

}  // namespace fhert

#endif  // FHERT_KEY_IO_H
