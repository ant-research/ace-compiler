//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_header.h
//! Shared key serialization header for FHE runtime backends.
//!
//! KeyHeader defines the binary format for serialized FHE keys (SK, PK,
//! RelinKey, GaloisKey). Both phantom and ace backends read/write this
//! same format. The per-key GPU memory layout is backend-specific and
//! handled by KeySerializer<VendorTraits>.

#ifndef FHERT_KEY_HEADER_H
#define FHERT_KEY_HEADER_H

#include <array>
#include <cstdint>

namespace fhert
{

//=============================================================================
// Constants
//=============================================================================

//! Magic number for key files: "FHEK" ASCII little-endian
constexpr uint32_t KEY_MAGIC = 0x4648454B;

//! Current key format version
constexpr uint16_t KEY_FORMAT_VERSION = 1;

//=============================================================================
// KeyType
//=============================================================================

//! Key type codes — stored in KeyHeader::key_type
enum class KeyType : uint16_t
{
    SECRET_KEY = 1,
    PUBLIC_KEY = 2,
    RELIN_KEY = 3,
    GALOIS_KEY = 4,
};

//=============================================================================
// parms_id_type — same layout as vendor parms_id_type
//=============================================================================

using key_parms_id_type = std::array<uint64_t, 4>;

//=============================================================================
// KeyHeader — Fixed 64-byte header for serialized keys
//=============================================================================

#pragma pack(push, 1)
struct KeyHeader
{
    uint32_t magic = KEY_MAGIC;             //!< [0:4]   "FHEK"
    uint16_t version = KEY_FORMAT_VERSION;  //!< [4:6]   Format version
    uint16_t key_type = 0;                  //!< [6:8]   KeyType enum value
    uint32_t header_size = 64;              //!< [8:12]  Size of this header
    uint64_t poly_modulus_degree = 0;       //!< [12:20] N
    uint64_t coeff_modulus_size = 0;        //!< [20:28] Number of RNS moduli
    uint64_t data_size = 0;                 //!< [28:36] Key data section (bytes)
    uint32_t special_modulus_size = 0;      //!< [36:40] Number of P moduli
    uint64_t key_specific = 0;              //!< [40:48] Key-type specific field
    uint32_t num_galois_elts = 0;           //!< [48:52] GaloisKey only
    uint32_t reserved[2] = {0, 0};          //!< [52:60] Reserved, must be 0
    uint32_t crc32 = 0;                     //!< [60:64] CRC32 of data section
};
#pragma pack(pop)

static_assert(sizeof(KeyHeader) == 64, "KeyHeader must be exactly 64 bytes");

//=============================================================================
// Header validation
//=============================================================================

//! @brief Validate a KeyHeader
//! @return true if header has correct magic, version, header_size, key_type
inline bool validate_key_header(const KeyHeader& hdr)
{
    if (hdr.magic != KEY_MAGIC)
        return false;
    if (hdr.version != KEY_FORMAT_VERSION)
        return false;
    if (hdr.header_size != sizeof(KeyHeader))
        return false;
    if (hdr.key_type < 1 || hdr.key_type > 4)
        return false;
    return true;
}

}  // namespace fhert

#endif  // FHERT_KEY_HEADER_H
