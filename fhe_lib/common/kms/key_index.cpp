//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_index.cpp
//! JSON read/write for GaloisKeyIndex.
//!
//! Uses nlohmann/json (header-only) for JSON serialization.
//!
//! Format versions:
//!   version 1: per-key files, no "offset" field in relin_keys entries
//!   version 2: merged file, "offset" field present in relin_keys entries

#include "fhe/kms/key_index.h"

#include "fhe/util/logging.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace fhert
{

using json = nlohmann::json;

//=============================================================================
// JSON schema helpers
//=============================================================================

//! Parse a CRC32 hex string like "0xABCD1234" to uint32_t.
//! Returns 0 if the string is empty or "0x0".
static uint32_t parse_crc32_hex(const std::string& s)
{
    if (s.empty())
        return 0;
    return static_cast<uint32_t>(std::stoul(s, nullptr, 16));
}

//! Format a CRC32 value as hex string "0xABCD1234".
static std::string format_crc32_hex(uint32_t crc)
{
    if (crc == 0)
        return "0x0";
    char buf[16];
    std::snprintf(buf, sizeof(buf), "0x%08X", crc);
    return buf;
}

//=============================================================================
// write_galois_key_index
//=============================================================================

bool write_galois_key_index(const std::string& index_path, const GaloisKeyIndex& idx)
{
    try {
        json j;
        j["version"] = idx.version;
        j["poly_modulus_degree"] = idx.poly_modulus_degree;
        j["coeff_modulus_size"] = idx.coeff_modulus_size;

        // parms_id: uint64_t array
        j["parms_id"] = json::array();
        for (size_t i = 0; i < idx.parms_id.size(); i++) {
            j["parms_id"].push_back(idx.parms_id[i]);
        }

        // galois_elts
        j["galois_elts"] = idx.galois_elts;

        j["key_component_bytes"] = idx.key_component_bytes;

        // disk_paths
        j["disk_paths"] = idx.disk_paths;

        // relin_keys
        j["relin_keys"] = json::array();
        for (const auto& entry : idx.relin_keys) {
            json ej;
            ej["file"] = entry.file;
            ej["disk"] = entry.disk_index;
            if (entry.file_slot > 0) {
                ej["slot"] = entry.file_slot;
            }
            ej["size"] = entry.size;
            ej["nk"] = entry.nk;
            ej["crc32"] = format_crc32_hex(entry.crc32);
            if (entry.offset > 0) {
                ej["offset"] = entry.offset;
            }
            j["relin_keys"].push_back(ej);
        }

        // Write to file (pretty-printed for readability)
        std::ofstream ofs(index_path);
        if (!ofs.is_open()) {
            spdlog::error("[kms] Cannot open index.json for writing: {}", index_path);
            return false;
        }
        ofs << j.dump(2) << std::endl;
        ofs.close();

        spdlog::info("[kms] Wrote {} ({} relin keys, {} disks)", index_path, idx.relin_keys.size(),
                     idx.disk_paths.size());
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] Failed to write index.json: {}", e.what());
        return false;
    }
}

//=============================================================================
// read_galois_key_index
//=============================================================================

bool read_galois_key_index(const std::string& index_path, GaloisKeyIndex& idx)
{
    try {
        std::ifstream ifs(index_path);
        if (!ifs.is_open()) {
            spdlog::error("[kms] Cannot open index.json: {}", index_path);
            return false;
        }

        json j;
        ifs >> j;
        ifs.close();

        idx.version = j.value("version", 1u);
        idx.poly_modulus_degree = j.value("poly_modulus_degree", 0ull);
        idx.coeff_modulus_size = j.value("coeff_modulus_size", 0ull);

        // parms_id
        if (j.contains("parms_id") && j["parms_id"].is_array()) {
            for (size_t i = 0; i < std::min(j["parms_id"].size(), idx.parms_id.size()); i++) {
                idx.parms_id[i] = j["parms_id"][i].get<uint64_t>();
            }
        }

        // galois_elts
        idx.galois_elts = j.value("galois_elts", std::vector<uint32_t>{});

        idx.key_component_bytes = j.value("key_component_bytes", 0ull);

        // disk_paths
        idx.disk_paths = j.value("disk_paths", std::vector<std::string>{});

        // relin_keys
        idx.relin_keys.clear();
        if (j.contains("relin_keys") && j["relin_keys"].is_array()) {
            for (const auto& ej : j["relin_keys"]) {
                GaloisKeyIndex::RelinKeyEntry entry;
                entry.file = ej.value("file", "");
                entry.disk_index = ej.value("disk", 0);
                entry.file_slot = ej.value("slot", 0);
                entry.offset = ej.value("offset", 0ull);
                entry.size = ej.value("size", 0ull);
                entry.nk = ej.value("nk", 2ull);

                // CRC32: parse hex string or accept integer
                if (ej.contains("crc32")) {
                    if (ej["crc32"].is_string()) {
                        entry.crc32 = parse_crc32_hex(ej["crc32"].get<std::string>());
                    }
                    else {
                        entry.crc32 = ej.value("crc32", 0u);
                    }
                }

                idx.relin_keys.push_back(entry);
            }
        }

        // Validate
        if (idx.relin_keys.empty()) {
            spdlog::error("[kms] index.json has no relin_keys entries");
            return false;
        }
        if (idx.poly_modulus_degree == 0 || idx.coeff_modulus_size == 0) {
            spdlog::error("[kms] index.json missing poly_modulus_degree or coeff_modulus_size");
            return false;
        }

        spdlog::info("[kms] Read {} (version={}, N={}, L={}, {} relin keys, {} disks)", index_path, idx.version,
                     idx.poly_modulus_degree, idx.coeff_modulus_size, idx.relin_keys.size(), idx.disk_paths.size());
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] Failed to read index.json: {}", e.what());
        return false;
    }
}

}  // namespace fhert
