//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file ciphertext_io.h
//! @brief Templated ciphertext serialization for FHE runtime backends.
//!
//! CiphertextSerializer<VendorTraits> provides serialize/deserialize for
//! vendor-specific ciphertext types using the common CtHeader format. This is
//! the payload for FHE serving "ciphertext in / ciphertext out" (Step 2 of the
//! service roadmap): a client encrypts → serialize → network → server
//! deserialize → Io_set_input → eval → Io_get_output → serialize → network →
//! client deserialize → decrypt. The server holds no SK (eval-only).
//!
//! Mirrors EncIo<VendorTraits> (enc_io.h), which covers plaintexts. Vendor
//! behavior is factored into hooks on VendorTraits:
//!   - access_ciphertext(const Ciphertext&) → CtAccess
//!   - populate_ciphertext(Ciphertext&, const CtPopulate&, Context*, stream)
//!   - ct_reserved2_from_access(const CtAccess&) → uint32_t  (stores is_ntt_form)
//!   - set_level_info(CtPopulate&, const CtHeader&)
//!
//! P1 note: noise_scale_deg / is_asymmetric / parms_id have no public setters
//! on PhantomCiphertext, so populate_ciphertext leaves them at default. CKKS
//! eval does not read these, so roundtrip correctness holds. Full fidelity
//! (friend access) is deferred.

#ifndef FHERT_CIPHERTEXT_IO_H
#define FHERT_CIPHERTEXT_IO_H

#include "fhe/enc/enc_header.h"  // ENC_FORMAT_VERSION, validate pattern
#include "fhe/io/crc32.h"
#include "fhe/io/cuda_check.h"
#include "fhe/util/logging.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cuda_runtime.h>

namespace fhert
{

//=============================================================================
// Constants
//=============================================================================

//! Magic string for ciphertext entries: "FHECT\0\0\0"
constexpr char CT_MAGIC_STR[8] = {'F', 'H', 'E', 'C', 'T', '\0', '\0', '\0'};

//! Current ciphertext format version
constexpr uint16_t CT_FORMAT_VERSION = 1;

//=============================================================================
// CtHeader — fixed-size header for serialized ciphertexts
//=============================================================================
// Layout extends EncHeader with ciphertext-specific fields: size (number of
// polys, usually 2), is_ntt_form, noise_scale_deg, is_asymmetric. parms_id is
// NOT carried (Phantom eval routes by chain_index; both ends share the same
// context/.conf so chain_index↔level maps 1:1). Kept packed for a stable
// cross-process binary format.

#pragma pack(push, 1)
struct CtHeader
{
    char magic[8];                 //!< "FHECT\0\0\0"
    uint16_t version;              //!< CT_FORMAT_VERSION
    uint16_t reserved;             //!< 0
    uint32_t header_size;          //!< sizeof(CtHeader)
    uint64_t poly_modulus_degree;  //!< N
    uint64_t coeff_modulus_size;   //!< L
    uint64_t size;                 //!< number of polys (usually 2)
    uint64_t chain_index;          //!< level
    double scale;                  //!< ciphertext scale
    uint64_t data_size;            //!< size * N * L * sizeof(uint64_t)
    uint32_t crc32;                //!< CRC32 of polynomial data
    uint16_t is_ntt_form;          //!< 0/1
    uint16_t is_asymmetric;        //!< 0/1 (best-effort; may be 0 if unset)
    uint32_t noise_scale_deg;      //!< best-effort; may be 0 if unset
    uint32_t reserved2;            //!< 0
};
#pragma pack(pop)

static_assert(sizeof(CtHeader) == 80, "CtHeader must be exactly 80 bytes");

//! @brief Validate a CtHeader (inline, no CUDA dependency)
inline bool validate_ct_header(const CtHeader& hdr)
{
    if (std::memcmp(hdr.magic, CT_MAGIC_STR, 8) != 0)
        return false;
    if (hdr.version != CT_FORMAT_VERSION)
        return false;
    if (hdr.header_size != sizeof(CtHeader))
        return false;
    if (hdr.reserved != 0)
        return false;
    if (hdr.poly_modulus_degree == 0)
        return false;
    if (hdr.coeff_modulus_size == 0)
        return false;
    if (hdr.size == 0)
        return false;
    if (hdr.data_size == 0)
        return false;
    return true;
}

//=============================================================================
// CiphertextSerializer<VendorTraits>
//=============================================================================

template <typename Traits> class CiphertextSerializer
{
public:
    using Ciphertext = typename Traits::Ciphertext;
    using ContextT = typename Traits::Context;
    using CtAccess = typename Traits::CtAccess;
    using CtPopulate = typename Traits::CtPopulate;

    /// Serialized buffer size for a ciphertext.
    static size_t buffer_length(const Ciphertext& ct)
    {
        auto access = Traits::access_ciphertext(ct);
        return sizeof(CtHeader) + access.data_size;
    }

    /// Serialize: GPU → host buffer (CtHeader + CRC32).
    static bool serialize(const Ciphertext& ct, char* buf, size_t buf_size)
    {
        auto access = Traits::access_ciphertext(ct);
        size_t required = sizeof(CtHeader) + access.data_size;
        if (buf_size < required) {
            spdlog::error("[ct_io] Buffer too small: need {}, have {}", required, buf_size);
            return false;
        }

        CtHeader hdr{};
        std::memcpy(hdr.magic, CT_MAGIC_STR, 8);
        hdr.version = CT_FORMAT_VERSION;
        hdr.reserved = 0;
        hdr.header_size = sizeof(CtHeader);
        hdr.poly_modulus_degree = access.poly_modulus_degree;
        hdr.coeff_modulus_size = access.coeff_modulus_size;
        hdr.size = access.size;
        hdr.chain_index = Traits::chain_index_from_ct_access(access);
        hdr.scale = access.scale;
        hdr.data_size = access.data_size;
        hdr.is_ntt_form = access.is_ntt_form ? 1 : 0;
        hdr.is_asymmetric = access.is_asymmetric ? 1 : 0;
        hdr.noise_scale_deg = access.noise_scale_deg;
        hdr.reserved2 = 0;

        char* data_ptr = buf + sizeof(CtHeader);
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

        std::memcpy(buf, &hdr, sizeof(CtHeader));
        return true;
    }

    /// Deserialize: host buffer → GPU (validates header + CRC32).
    static bool deserialize(const char* buf, size_t buf_size, Ciphertext& ct, ContextT* ctx = nullptr,
                            void* stream = nullptr)
    {
        if (buf_size < sizeof(CtHeader)) {
            spdlog::error("[ct_io] Buffer too small for header: {} < {}", buf_size, sizeof(CtHeader));
            return false;
        }
        CtHeader hdr;
        std::memcpy(&hdr, buf, sizeof(CtHeader));
        if (!validate_ct_header(hdr)) {
            spdlog::error("[ct_io] Invalid ciphertext header");
            return false;
        }
        if (buf_size < sizeof(CtHeader) + hdr.data_size) {
            spdlog::error("[ct_io] Buffer too small for data: {} < {}", buf_size, sizeof(CtHeader) + hdr.data_size);
            return false;
        }

        const char* data_ptr = buf + sizeof(CtHeader);
        if (hdr.data_size > 0 && hdr.crc32 != 0) {
            rtio::Crc32 crc;
            crc.update(data_ptr, hdr.data_size);
            uint32_t computed = crc.finalize();
            if (computed != hdr.crc32) {
                spdlog::error("[ct_io] CRC32 mismatch: expected 0x{:08X}, got 0x{:08X}", hdr.crc32, computed);
                return false;
            }
        }

        if (hdr.data_size > 0) {
            CtPopulate populate;
            populate.host_data = data_ptr;
            populate.data_size = hdr.data_size;
            populate.poly_modulus_degree = hdr.poly_modulus_degree;
            populate.coeff_modulus_size = hdr.coeff_modulus_size;
            populate.size = hdr.size;
            populate.scale = hdr.scale;
            populate.is_ntt_form = hdr.is_ntt_form != 0;
            populate.is_asymmetric = hdr.is_asymmetric != 0;
            populate.noise_scale_deg = hdr.noise_scale_deg;
            Traits::set_ct_level_info(populate, hdr);

            Traits::populate_ciphertext(ct, populate, ctx, stream);
        }
        return true;
    }
};

}  // namespace fhert

#endif  // FHERT_CIPHERTEXT_IO_H