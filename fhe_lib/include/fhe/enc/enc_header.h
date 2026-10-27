//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_header.h
//! Shared plaintext serialization header for FHE runtime backends.
//!
//! EncHeader defines the binary format for serialized CKKS plaintexts.
//! Both phantom and ace backends read/write this same format.

#ifndef FHERT_ENC_HEADER_H
#define FHERT_ENC_HEADER_H

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace fhert
{

//=============================================================================
// Constants
//=============================================================================

//! Magic string for plaintext entries: "FHEPT\0\0\0"
constexpr char ENC_MAGIC_STR[8] = {'F', 'H', 'E', 'P', 'T', '\0', '\0', '\0'};

//! Current plaintext format version
constexpr uint16_t ENC_FORMAT_VERSION = 1;

//=============================================================================
// EncHeader — Fixed 64-byte header for serialized plaintexts
//=============================================================================

#pragma pack(push, 1)
struct EncHeader
{
    char magic[8];                 //!< [0:8]   "FHEPT\0\0\0"
    uint16_t version;              //!< [8:10]  Format version (= 1)
    uint16_t reserved;             //!< [10:12] Reserved, must be 0
    uint32_t header_size;          //!< [12:16] Size of this header (= 64)
    uint64_t poly_modulus_degree;  //!< [16:24] N
    uint64_t coeff_modulus_size;   //!< [24:32] Number of RNS moduli L
    uint64_t chain_index;          //!< [32:40] Encoding level
    double scale;                  //!< [40:48] Encoding scale
    uint64_t data_size;            //!< [48:56] N * L * sizeof(uint64_t)
    uint32_t crc32;                //!< [56:60] CRC32 of polynomial data
    uint32_t reserved2;            //!< [60:64] Backend-specific flags (ACE: is_ntt_form)
};
#pragma pack(pop)

static_assert(sizeof(EncHeader) == 64, "EncHeader must be exactly 64 bytes");

//=============================================================================
// Header validation (inline — no CUDA dependency)
//=============================================================================

//! @brief Validate a EncHeader
//! @return true if header has correct magic, version, and header_size
inline bool validate_enc_header(const EncHeader& hdr)
{
    if (std::memcmp(hdr.magic, ENC_MAGIC_STR, 8) != 0)
        return false;
    if (hdr.version != ENC_FORMAT_VERSION)
        return false;
    if (hdr.header_size != sizeof(EncHeader))
        return false;
    if (hdr.reserved != 0)
        return false;
    // reserved2 is backend-specific (e.g., ACE stores is_ntt_form), not validated
    if (hdr.poly_modulus_degree == 0)
        return false;
    if (hdr.coeff_modulus_size == 0)
        return false;
    if (hdr.data_size == 0)
        return false;
    return true;
}

}  // namespace fhert

#endif  // FHERT_ENC_HEADER_H
