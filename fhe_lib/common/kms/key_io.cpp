//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_protocol.cpp
//! KeyWriter / KeyReader implementation — protocol-level key file I/O.

#include "fhe/kms/key_io.h"

#include <cstdio>
#include <cstring>

namespace fhert
{

//=============================================================================
// KeyWriter
//=============================================================================

bool KeyWriter::write_header()
{
    // Write header placeholder at offset 0 (CRC not yet known).
    // Advance _offset past the header so subsequent write_meta /
    // write_gpu_data write to the correct position (after the header).
    _offset = sizeof(KeyHeader);
    _crc.reset();
    _gds_used = false;  // will be set by write_gpu_data if GDS is active
    return _io.write(&_hdr, 0, sizeof(_hdr));
}

bool KeyWriter::write_meta(const void* data, size_t size)
{
    if (!_io.write(data, _offset, size))
        return false;
    _crc.update(data, size);
    _offset += size;
    return true;
}

bool KeyWriter::write_gpu_data(const void* gpu_ptr, size_t size)
{
    if (size == 0)
        return true;
    if (!_io.write_gpu(gpu_ptr, _offset, size))
        return false;

    if (_io.used_gds()) {
        // GDS path: skip CRC computation. GPU→SSD direct transfer is
        // protected by NVMe ECC + PCIe TLP CRC + HBM ECC. Computing
        // CRC would require a redundant D2H copy that defeats the
        // purpose of GDS (zero host memory footprint).
        _gds_used = true;
    }
    else {
        // Staged path: data passes through host memory anyway —
        // CRC is essentially free.
        _crc.update_gpu(gpu_ptr, size);
    }

    _offset += size;
    return true;
}

size_t KeyWriter::finish()
{
    // Compute final CRC and patch the header.
    // For GDS path: CRC was not computed (update_gpu skipped), so
    // finalize() returns a meaningless value. Set crc32=0 in header
    // to signal "no CRC — trust hardware ECC" to the reader.
    uint32_t final_crc = _crc.finalize();
    if (_gds_used) {
        _hdr.crc32 = 0;
    }
    else {
        _hdr.crc32 = final_crc;
    }

    // Rewrite the header at offset 0 with the correct CRC
    if (!_io.write(&_hdr, 0, sizeof(_hdr)))
        return 0;

    return _offset;
}

//=============================================================================
// KeyReader
//=============================================================================

KeyHeader KeyReader::read_header(uint64_t expect_poly_deg, uint64_t expect_coeff_size)
{
    KeyHeader hdr;
    if (!_io.read(&hdr, 0, sizeof(hdr))) {
        return {};  // Return default-constructed (invalid) header
    }

    if (!validate_key_header(hdr))
        return {};

    if (expect_poly_deg != 0 && hdr.poly_modulus_degree != expect_poly_deg) {
        return {};
    }
    if (expect_coeff_size != 0 && hdr.coeff_modulus_size != expect_coeff_size) {
        return {};
    }

    // Start reading data after the header
    _offset = sizeof(KeyHeader);
    _crc.reset();

    return hdr;
}

bool KeyReader::read_meta(void* buf, size_t size)
{
    if (!_io.read(buf, _offset, size))
        return false;
    _crc.update(buf, size);
    _offset += size;
    return true;
}

bool KeyReader::read_gpu_data(void* gpu_ptr, size_t size)
{
    if (!_io.read_gpu(gpu_ptr, _offset, size))
        return false;

    if (_io.used_gds()) {
        // GDS path: skip CRC computation. GPU←SSD direct transfer is
        // protected by the hardware ECC chain. Computing CRC would
        // require a redundant D2H copy that defeats GDS benefits.
        //
        // Note: even if we skip CRC here, the header may contain
        // crc32=0 (written by GDS save path), which is accepted
        // by validate_crc().
        _gds_read = true;
    }
    else {
        // Staged path: data passes through host memory anyway.
        _crc.update_gpu(gpu_ptr, size);
    }

    _offset += size;
    return true;
}

bool KeyReader::validate_crc(uint32_t expected_crc) const
{
    // GDS-saved keys have crc32=0 in the header (no CRC was computed
    // during save — hardware ECC protects the NVMe↔GPU path).
    // Trust the hardware and skip validation.
    if (expected_crc == 0)
        return true;

    // GDS read path: CRC was not computed for GPU data (update_gpu skipped)
    // because computing CRC would require a redundant D2H copy. Trust the
    // hardware ECC chain (NVMe ECC + PCIe TLP CRC + HBM ECC) instead.
    // This covers the mixed-mode case: keys saved via staged (with CRC)
    // but loaded via GDS (CRC computation skipped for GPU data).
    if (_gds_read)
        return true;

    // Staged path: CRC was computed during both save and load, validate now.
    // If computed CRC is 0 (shouldn't happen for real data), also
    // accept — this preserves the old "GdsTransport returns 0" path.
    uint32_t computed = _crc.finalize();
    if (computed == 0)
        return true;

    return computed == expected_crc;
}

}  // namespace fhert
