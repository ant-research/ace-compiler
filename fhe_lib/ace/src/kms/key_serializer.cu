//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_serializer.cu
//! KeySerializer<VendorTraits> explicit instantiation.
//!
//! Serializes ACE host keys using the common KeyWriter/KeyReader protocol.
//! ACE keys are host-side SEAL objects with built-in save()/load() — we wrap
//! these serialized blobs in the KeyHeader + CRC32 format for GDS support.

#include "common/common.h"
#include "common/error.h"
#include "fhe/kms/key_serializer.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/ace/vendor_traits.h"

#include <chrono>
#include <spdlog/spdlog.h>
#include <sstream>
#include <vector>

namespace fhert
{

using Traits = ace::VendorTraits;

//=============================================================================
// Per-key save implementations
//=============================================================================

template <>
size_t KeySerializer<Traits>::save_secret_key(rtio::IoTransport& io, const SecretKey& sk, const ContextT& ctx)
{

    auto access = Traits::access_secret_key(sk, ctx);

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::SECRET_KEY);
    hdr.poly_modulus_degree = access.poly_modulus_degree;
    hdr.coeff_modulus_size = access.coeff_modulus_size;
    hdr.data_size = access.serialized.size();

    KeyWriter writer(io, hdr);
    writer.write_header();
    writer.write_meta(access.serialized.data(), access.serialized.size());
    return writer.finish();
}

template <>
size_t KeySerializer<Traits>::save_public_key(rtio::IoTransport& io, const PublicKey& pk, const ContextT& ctx)
{

    auto access = Traits::access_public_key(pk);

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::PUBLIC_KEY);
    hdr.poly_modulus_degree = Traits::poly_degree(ctx);
    hdr.coeff_modulus_size = Traits::coeff_mod_size(ctx);
    hdr.data_size = access.serialized.size();

    KeyWriter writer(io, hdr);
    writer.write_header();
    writer.write_meta(access.serialized.data(), access.serialized.size());
    return writer.finish();
}

template <>
size_t KeySerializer<Traits>::save_relin_key(rtio::IoTransport& io, const RelinKey& rlk, const ContextT& ctx)
{

    auto access = Traits::access_relin_key(rlk);

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::RELIN_KEY);
    hdr.poly_modulus_degree = Traits::poly_degree(ctx);
    hdr.coeff_modulus_size = Traits::coeff_mod_size(ctx);
    hdr.data_size = access.serialized.size();

    KeyWriter writer(io, hdr);
    writer.write_header();
    writer.write_meta(access.serialized.data(), access.serialized.size());
    return writer.finish();
}

template <>
size_t KeySerializer<Traits>::save_galois_key(rtio::IoTransport& io, const GaloisKey& rtk, const ContextT& ctx,
                                              const std::vector<uint32_t>& galois_elts)
{

    auto access = Traits::access_galois_key(rtk);

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::GALOIS_KEY);
    hdr.poly_modulus_degree = Traits::poly_degree(ctx);
    hdr.coeff_modulus_size = Traits::coeff_mod_size(ctx);
    hdr.data_size = access.serialized.size();
    hdr.num_galois_elts = static_cast<uint32_t>(galois_elts.size());

    KeyWriter writer(io, hdr);
    writer.write_header();
    // Write galois elements as metadata before the key blob
    writer.write_meta(galois_elts.data(), galois_elts.size() * sizeof(uint32_t));
    writer.write_meta(access.serialized.data(), access.serialized.size());
    return writer.finish();
}

//=============================================================================
// Per-key load implementations
//=============================================================================

template <> size_t KeySerializer<Traits>::load_secret_key(rtio::IoTransport& io, SecretKey& sk, const ContextT& ctx)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();

    std::string blob(hdr.data_size, '\0');
    reader.read_meta(blob.data(), hdr.data_size);
    reader.validate_crc(hdr.crc32);

    Traits::SecretKeyPopulate data;
    data.serialized = std::move(blob);
    Traits::populate_secret_key(sk, data, hdr.poly_modulus_degree, hdr.coeff_modulus_size, const_cast<ContextT&>(ctx));

    return reader.offset();
}

template <> size_t KeySerializer<Traits>::load_public_key(rtio::IoTransport& io, PublicKey& pk, const ContextT& ctx)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();

    std::string blob(hdr.data_size, '\0');
    reader.read_meta(blob.data(), hdr.data_size);
    reader.validate_crc(hdr.crc32);

    Traits::PublicKeyPopulate data;
    data.serialized = std::move(blob);
    Traits::populate_public_key(pk, data, hdr.poly_modulus_degree, hdr.coeff_modulus_size, const_cast<ContextT&>(ctx));

    return reader.offset();
}

template <> size_t KeySerializer<Traits>::load_relin_key(rtio::IoTransport& io, RelinKey& rlk, const ContextT& ctx)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();

    std::string blob(hdr.data_size, '\0');
    reader.read_meta(blob.data(), hdr.data_size);
    reader.validate_crc(hdr.crc32);

    Traits::RelinKeyPopulate data;
    data.serialized = std::move(blob);
    Traits::populate_relin_key(rlk, data, hdr.poly_modulus_degree, hdr.coeff_modulus_size, const_cast<ContextT&>(ctx));

    return reader.offset();
}

template <>
size_t KeySerializer<Traits>::load_galois_key(rtio::IoTransport& io, GaloisKey& rtk, const ContextT& ctx,
                                              std::vector<uint32_t>& galois_elts)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();

    // Read galois elements
    size_t galois_elts_size = hdr.num_galois_elts * sizeof(uint32_t);
    galois_elts.resize(hdr.num_galois_elts);
    reader.read_meta(galois_elts.data(), galois_elts_size);

    // Read key blob
    std::string blob(hdr.data_size, '\0');
    reader.read_meta(blob.data(), hdr.data_size);
    reader.validate_crc(hdr.crc32);

    Traits::GaloisKeyPopulate data;
    data.serialized = std::move(blob);
    data.galois_elts = galois_elts;
    Traits::populate_galois_key(rtk, data, hdr.poly_modulus_degree, hdr.coeff_modulus_size, const_cast<ContextT&>(ctx));

    return reader.offset();
}

//=============================================================================
// Galois-key dispatch — ACE uses single-file format
//=============================================================================

template <>
bool KeySerializer<Traits>::save_galois_keys(const std::string& model_dir, const GaloisKey& rtk, const ContextT& ctx,
                                             const std::vector<uint32_t>& galois_elts, rtio::IoPolicy io_policy)
{

    // ACE uses the legacy single-file format (KeyWriter protocol)
    auto io = rtio::open_writer(model_dir + "/galois_key.bin", Traits::io_location(), io_policy);
    if (!io) {
        spdlog::error("[kms] Failed to open galois_key.bin for writing");
        return false;
    }
    if (save_galois_key(*io, rtk, ctx, galois_elts) == 0) {
        spdlog::error("[kms] Failed to save galois_key.bin");
        return false;
    }
    return true;
}

template <>
bool KeySerializer<Traits>::load_galois_keys(const std::string& /*key_dir*/, const std::string& model_dir,
                                             GaloisKey& rtk, ContextT& ctx, std::vector<uint32_t>& galois_elts,
                                             rtio::IoPolicy io_policy)
{

    // ACE uses the legacy single-file format (KeyReader protocol)
    g_galois_load_time_seconds = -1.0;  // reset before load
    auto t_gk0 = std::chrono::high_resolution_clock::now();

    auto io = rtio::open_reader(model_dir + "/galois_key.bin", Traits::io_location(), io_policy);
    if (!io) {
        spdlog::error("[kms] Failed to open galois_key.bin for reading");
        return false;
    }
    if (load_galois_key(*io, rtk, ctx, galois_elts) == 0) {
        spdlog::error("[kms] Failed to load galois_key.bin");
        return false;
    }

    auto t_gk1 = std::chrono::high_resolution_clock::now();
    double s_galois = std::chrono::duration<double>(t_gk1 - t_gk0).count();
    g_galois_load_time_seconds = s_galois;
    spdlog::info("[kms] GaloisKey load: {:.2f}s ({} elts)", s_galois, galois_elts.size());
    return true;
}

//=============================================================================
// Stub implementations for per-key/merged galois formats (not used by ACE)
//=============================================================================

template <>
bool KeySerializer<Traits>::save_galois_key_per_key(const std::string&, const GaloisKey&, const ContextT&,
                                                    const std::vector<uint32_t>&, const std::vector<std::string>&,
                                                    rtio::IoPolicy)
{
    spdlog::error("[kms] ACE backend does not support per-key galois format");
    return false;
}

template <>
bool KeySerializer<Traits>::load_galois_key_per_key(const std::string&, GaloisKey&, ContextT&, std::vector<uint32_t>&,
                                                    rtio::IoPolicy)
{
    spdlog::error("[kms] ACE backend does not support per-key galois format");
    return false;
}

template <>
bool KeySerializer<Traits>::save_galois_key_merged(const std::string&, const GaloisKey&, const ContextT&,
                                                   const std::vector<uint32_t>&, const std::vector<std::string>&,
                                                   rtio::IoPolicy)
{
    spdlog::error("[kms] ACE backend does not support merged galois format");
    return false;
}

template <>
bool KeySerializer<Traits>::load_galois_key_merged(const std::string&, GaloisKey&, ContextT&, std::vector<uint32_t>&,
                                                   const GaloisKeyIndex&, rtio::IoPolicy)
{
    spdlog::error("[kms] ACE backend does not support merged galois format");
    return false;
}

}  // namespace fhert
