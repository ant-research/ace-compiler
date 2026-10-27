//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_io.h
//! Templated plaintext I/O for FHE runtime backends.
//!
//! EncIo<VendorTraits> provides serialize/deserialize for vendor-
//! specific plaintext types using the common EncHeader format.
//!
//! The implementation is fully generic — vendor-specific behavior is
//! factored into three hooks on the VendorTraits struct:
//!   - chain_index_from_access(const PtAccess&) → uint64_t
//!   - pt_reserved2_from_access(const PtAccess&) → uint32_t
//!   - set_level_info(PtPopulate&, const EncHeader&)
//!
//! No backend-specific .cu file is needed; all methods are inline here.

#ifndef FHERT_ENC_IO_H
#define FHERT_ENC_IO_H

#include "fhe/enc/enc_header.h"
#include "fhe/io/crc32.h"
#include "fhe/io/cuda_check.h"
#include "fhe/io/transport.h"
#include "fhe/util/logging.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cuda_runtime.h>
#include <vector>

namespace fhert
{

//=============================================================================
// EncIo<VendorTraits>
//=============================================================================

template <typename Traits> class EncIo
{
public:
    using Plaintext = typename Traits::Plaintext;
    using ContextT = typename Traits::Context;
    using PtAccess = typename Traits::PtAccess;
    using PtPopulate = typename Traits::PtPopulate;

    /// Compute serialized buffer size for a plaintext
    static size_t buffer_length(const Plaintext& pt)
    {
        auto access = Traits::access_plaintext(pt);
        return sizeof(EncHeader) + access.data_size;
    }

    /// Serialize: GPU → host buffer (with EncHeader + CRC32)
    static bool serialize(const Plaintext& pt, char* buf, size_t buf_size)
    {
        auto access = Traits::access_plaintext(pt);
        size_t required = sizeof(EncHeader) + access.data_size;
        if (buf_size < required) {
            spdlog::error("[enc_io] Buffer too small: need {}, have {}", required, buf_size);
            return false;
        }

        // Build header using vendor hooks
        EncHeader hdr{};
        std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
        hdr.version = ENC_FORMAT_VERSION;
        hdr.reserved = 0;
        hdr.header_size = sizeof(EncHeader);
        hdr.poly_modulus_degree = access.poly_modulus_degree;
        hdr.coeff_modulus_size = access.coeff_modulus_size;
        hdr.chain_index = Traits::chain_index_from_access(access);
        hdr.scale = access.scale;
        hdr.data_size = access.data_size;
        hdr.reserved2 = Traits::pt_reserved2_from_access(access);

        // Copy GPU data to host buffer and compute CRC32
        char* data_ptr = buf + sizeof(EncHeader);
        if (access.data_size > 0 && access.gpu_data != nullptr) {
            FHE_CHECK_CUDA(cudaMemcpy(data_ptr, access.gpu_data, access.data_size, cudaMemcpyDeviceToHost));
            rtio::Crc32 crc;
            crc.update(data_ptr, access.data_size);
            hdr.crc32 = crc.finalize();
        }
        else {
            hdr.crc32 = 0;
            hdr.data_size = 0;
        }

        // Write header into buffer
        std::memcpy(buf, &hdr, sizeof(EncHeader));
        return true;
    }

    /// Deserialize: host buffer → GPU (validates header + CRC32)
    static bool deserialize(const char* buf, size_t buf_size, Plaintext& pt, ContextT* ctx = nullptr,
                            void* stream = nullptr)
    {
        if (buf_size < sizeof(EncHeader)) {
            spdlog::error("[enc_io] Buffer too small for header: {} < {}", buf_size, sizeof(EncHeader));
            return false;
        }

        // Read and validate header
        EncHeader hdr;
        std::memcpy(&hdr, buf, sizeof(EncHeader));
        if (!validate_enc_header(hdr)) {
            spdlog::error("[enc_io] Invalid plaintext header");
            return false;
        }

        if (buf_size < sizeof(EncHeader) + hdr.data_size) {
            spdlog::error("[enc_io] Buffer too small for data: {} < {}", buf_size, sizeof(EncHeader) + hdr.data_size);
            return false;
        }

        // Validate CRC32
        const char* data_ptr = buf + sizeof(EncHeader);
        if (hdr.data_size > 0 && hdr.crc32 != 0) {
            rtio::Crc32 crc;
            crc.update(data_ptr, hdr.data_size);
            uint32_t computed = crc.finalize();
            if (computed != hdr.crc32) {
                spdlog::error("[enc_io] CRC32 mismatch: expected 0x{:08X}, got 0x{:08X}", hdr.crc32, computed);
                return false;
            }
        }

        // Populate the plaintext
        if (hdr.data_size > 0) {
            PtPopulate populate;
            populate.host_data = data_ptr;
            populate.data_size = hdr.data_size;
            populate.poly_modulus_degree = hdr.poly_modulus_degree;
            populate.coeff_modulus_size = hdr.coeff_modulus_size;
            populate.scale = hdr.scale;
            Traits::set_level_info(populate, hdr);

            Traits::populate_plaintext(pt, populate, ctx, stream);
        }

        return true;
    }

    /// Save via IoTransport (header via write, GPU data via write_gpu)
    static size_t save(rtio::IoTransport& io, const Plaintext& pt)
    {
        auto access = Traits::access_plaintext(pt);

        // Build header using vendor hooks
        EncHeader hdr{};
        std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
        hdr.version = ENC_FORMAT_VERSION;
        hdr.reserved = 0;
        hdr.header_size = sizeof(EncHeader);
        hdr.poly_modulus_degree = access.poly_modulus_degree;
        hdr.coeff_modulus_size = access.coeff_modulus_size;
        hdr.chain_index = Traits::chain_index_from_access(access);
        hdr.scale = access.scale;
        hdr.data_size = access.data_size;
        hdr.reserved2 = Traits::pt_reserved2_from_access(access);

        // Compute CRC32 of GPU data
        if (access.data_size > 0 && access.gpu_data != nullptr) {
            hdr.crc32 = rtio::Crc32::compute_gpu(access.gpu_data, access.data_size);
        }
        else {
            hdr.crc32 = 0;
            hdr.data_size = 0;
        }

        // Write header at offset 0
        size_t offset = 0;
        if (!io.write(reinterpret_cast<const char*>(&hdr), offset, sizeof(hdr))) {
            return 0;
        }
        offset += sizeof(hdr);

        // Write GPU data via IoTransport (supports GDS if available)
        if (hdr.data_size > 0 && access.gpu_data != nullptr) {
            if (!io.write_gpu(access.gpu_data, offset, hdr.data_size)) {
                return 0;
            }
            offset += hdr.data_size;
        }

        return offset;
    }

    /// Load via IoTransport (header via read, data via read into host buffer)
    static size_t load(rtio::IoTransport& io, Plaintext& pt, ContextT* ctx = nullptr, void* stream = nullptr)
    {
        // Read header at offset 0
        EncHeader hdr;
        if (!io.read(reinterpret_cast<char*>(&hdr), 0, sizeof(hdr))) {
            return 0;
        }
        if (!validate_enc_header(hdr)) {
            spdlog::error("[enc_io] Invalid plaintext header in file: {}", io.path());
            return 0;
        }

        size_t total_size = sizeof(EncHeader) + hdr.data_size;

        // Read data into host buffer
        if (hdr.data_size > 0) {
            std::vector<char> host_buf(hdr.data_size);
            if (!io.read(host_buf.data(), sizeof(EncHeader), hdr.data_size)) {
                spdlog::error("[enc_io] Failed to read plaintext data from: {}", io.path());
                return 0;
            }

            // Validate CRC (staged transport computes it; GDS transport returns 0)
            uint32_t computed_crc = io.crc32();
            if (computed_crc == 0) {
                // Fallback: compute CRC from host buffer
                rtio::Crc32 crc;
                crc.update(host_buf.data(), hdr.data_size);
                computed_crc = crc.finalize();
            }
            if (computed_crc != 0 && computed_crc != hdr.crc32) {
                spdlog::error("[enc_io] CRC32 mismatch loading plaintext from {}: "
                              "expected 0x{:08X}, got 0x{:08X}",
                              io.path(), hdr.crc32, computed_crc);
                return 0;
            }

            // Populate the plaintext
            PtPopulate populate;
            populate.host_data = host_buf.data();
            populate.data_size = hdr.data_size;
            populate.poly_modulus_degree = hdr.poly_modulus_degree;
            populate.coeff_modulus_size = hdr.coeff_modulus_size;
            populate.scale = hdr.scale;
            Traits::set_level_info(populate, hdr);

            Traits::populate_plaintext(pt, populate, ctx, stream);
        }

        return total_size;
    }
};

}  // namespace fhert

#endif  // FHERT_ENC_IO_H
