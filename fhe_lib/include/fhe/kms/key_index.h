//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_index.h
//! Galois key index: JSON-based metadata for per-key galois_key files.
//!
//! The galois_key.index.json file describes the layout of per-key galois
//! key files (galois_key.00.bin, galois_key.01.bin, ...) or a merged
//! file (galois_key.merged.bin). It stores polynomial parameters, disk
//! mapping, and optional CRC32 checksums.
//!
//! Format versions:
//!   version 1: per-key files, no offset field (backward compatible)
//!   version 2: merged file, offset field present in each RelinKeyEntry

#ifndef FHERT_KEY_INDEX_H
#define FHERT_KEY_INDEX_H

#include "fhe/kms/key_header.h"  // for key_parms_id_type

#include <cstdint>
#include <string>
#include <vector>

namespace fhert
{

//=============================================================================
// GaloisKeyIndex — per-key file index
//=============================================================================

//! Metadata for the per-key galois key file bundle.
//!
//! Written as galois_key.index.json alongside the individual .bin files.
//! Each .bin file contains raw GPU data (no KeyHeader, no per-file CRC).
struct GaloisKeyIndex
{
    //! Index format version (currently 1).
    uint32_t version = 1;

    //! Polynomial modulus degree N.
    uint64_t poly_modulus_degree = 0;

    //! Coefficient modulus size L.
    uint64_t coeff_modulus_size = 0;

    //! Parameters ID from the SEAL context.
    key_parms_id_type parms_id = {};

    //! Galois elements corresponding to each relin_key.
    std::vector<uint32_t> galois_elts;

    //! Size of one key component: 2 * N * L * sizeof(uint64_t).
    uint64_t key_component_bytes = 0;

    //! Disk paths for multi-disk distribution.
    //! Key i is stored on disk_paths[i % disk_paths.size()].
    //! The index.json is always written to disk_paths[0]/model/.
    std::vector<std::string> disk_paths;

    //! Per-relin-key entry in the index.
    struct RelinKeyEntry
    {
        std::string file;    //!< Filename, e.g. "galois_key.00.bin" or "galois_key.merged.0.0.bin"
        int disk_index = 0;  //!< Index into disk_paths
        int file_slot = 0;   //!< Sub-file slot within disk (0 = default, >0 for v3 multi-file)
        size_t offset = 0;   //!< Byte offset in the file (0 = per-key format, >0 = merged format)
        size_t size = 0;     //!< Data size in bytes
        uint64_t nk = 2;     //!< Number of key components
        uint32_t crc32 = 0;  //!< CRC32 (0 = not computed / GDS path)
    };

    //! One entry per relin_key in the galois key.
    std::vector<RelinKeyEntry> relin_keys;
};

//=============================================================================
// JSON read/write
//=============================================================================

//! Write GaloisKeyIndex to a JSON file.
//! @param index_path  Path to galois_key.index.json
//! @param idx         Index data to write
//! @return true on success
bool write_galois_key_index(const std::string& index_path, const GaloisKeyIndex& idx);

//! Read GaloisKeyIndex from a JSON file.
//! @param index_path  Path to galois_key.index.json
//! @param idx         [out] Index data read from file
//! @return true on success
bool read_galois_key_index(const std::string& index_path, GaloisKeyIndex& idx);

}  // namespace fhert

#endif  // FHERT_KEY_INDEX_H
