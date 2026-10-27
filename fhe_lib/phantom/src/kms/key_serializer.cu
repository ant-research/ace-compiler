//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_serializer.cpp
//! KeySerializer<VendorTraits> explicit instantiation for VendorTraits.
//!
//! The template is declared in common/key_serializer.h. This file provides
//! the implementation of the per-key save/load methods that use KeyWriter/
//! KeyReader (common I/O protocol) + VendorTraits accessors (vendor-
//! specific raw pointer extraction and key object population).
//!
//! Galois keys use per-key format: one raw .bin file per relin_key + a
//! galois_key.index.json. Loading supports parallel I/O via num_io_threads.

#include "fhe/io/cuda_check.h"
#include "fhe/io/io_bulk_reader.h"
#include "fhe/io/io_policy.h"
#include "fhe/io/transport.h"
#include "fhe/kms/key_serializer.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/phantom/vendor_traits.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cuda_runtime.h>
#include <map>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <thread>
#include <vector>
// Note: io_policy.h is the ONLY IO header needed by business code.
// open_reader() returns an IoTransport; the concrete implementation
// (Staged/Mapped/Gds) is selected by IoPolicy internally.

//=============================================================================
// Galois key load time (seconds) is an inline static member of
// KeySerializer<Traits> (defined in key_serializer.h); set inside
// load_galois_keys() below, read by KeyMgr::Load_keys into the KeyMgr instance
// field surfaced via Kms_get_galois_load_time_seconds().
//=============================================================================

namespace fhert
{

//=============================================================================
// Helper: ensure directory exists
//=============================================================================

static void ensure_dir(const std::string& path)
{
    mkdir(path.c_str(), 0755);
}

//=============================================================================
// Helper: format per-key filename
//=============================================================================

static std::string make_per_key_filename(uint64_t index)
{
    std::string num = std::to_string(index);
    if (num.size() < 2)
        num = "0" + num;  // zero-pad to 2 digits: 00, 01, ...
    return "galois_key." + num + ".bin";
}

//=============================================================================
// KeySerializer<VendorTraits> — Single-file key save implementations
// (SecretKey, PublicKey, RelinKey remain in KeyHeader/KeyWriter format)
//=============================================================================

template <>
size_t KeySerializer<phantom::VendorTraits>::save_secret_key(rtio::IoTransport& io, const SecretKey& sk,
                                                             const ContextT& ctx)
{

    auto access = phantom::VendorTraits::access_secret_key(sk, ctx);
    const uint64_t N = sk.poly_modulus_degree();
    const uint64_t L = sk.coeff_modulus_size();
    const size_t data_rns_bytes = N * L * sizeof(uint64_t);
    const size_t sk_array_bytes = access.sk_max_power * N * L * sizeof(uint64_t);
    const size_t data_size = sizeof(uint64_t) + data_rns_bytes + sk_array_bytes;

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::SECRET_KEY);
    hdr.poly_modulus_degree = N;
    hdr.coeff_modulus_size = L;
    hdr.data_size = data_size;
    hdr.key_specific = access.sk_max_power;

    KeyWriter writer(io, hdr);
    writer.write_header();
    writer.write_meta(&access.sk_max_power, sizeof(uint64_t));
    writer.write_gpu_data(access.data_rns_ptr, data_rns_bytes);
    writer.write_gpu_data(access.sk_array_ptr, sk_array_bytes);
    size_t total = writer.finish();

    if (total > 0) {
        spdlog::info("[kms] Saved secret_key.bin via {} ({} bytes, N={}, L={}, max_pow={})", io.transport_name(), total,
                     N, L, access.sk_max_power);
    }
    return total;
}

template <>
size_t KeySerializer<phantom::VendorTraits>::save_public_key(rtio::IoTransport& io, const PublicKey& pk,
                                                             const ContextT& ctx)
{

    auto access = phantom::VendorTraits::access_public_key(pk);
    const uint64_t N = access.poly_modulus_degree;
    const uint64_t L = access.coeff_modulus_size;
    const size_t pk_data_bytes = access.pk_size * N * L * sizeof(uint64_t);
    const size_t data_size = sizeof(uint64_t) + sizeof(double) + pk_data_bytes;

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::PUBLIC_KEY);
    hdr.poly_modulus_degree = N;
    hdr.coeff_modulus_size = L;
    hdr.data_size = data_size;
    hdr.key_specific = access.pk_size;

    KeyWriter writer(io, hdr);
    writer.write_header();

    // Write chain_index + scale as metadata
    uint8_t meta_buf[sizeof(uint64_t) + sizeof(double)];
    std::memcpy(meta_buf, &access.chain_index, sizeof(uint64_t));
    std::memcpy(meta_buf + sizeof(uint64_t), &access.scale, sizeof(double));
    writer.write_meta(meta_buf, sizeof(meta_buf));

    writer.write_gpu_data(access.pk_data_ptr, pk_data_bytes);
    size_t total = writer.finish();

    if (total > 0) {
        spdlog::info("[kms] Saved public_key.bin via {} ({} bytes, pk_size={})", io.transport_name(), total,
                     access.pk_size);
    }
    return total;
}

template <>
size_t KeySerializer<phantom::VendorTraits>::save_relin_key(rtio::IoTransport& io, const RelinKey& rlk,
                                                            const ContextT& ctx)
{

    auto access = phantom::VendorTraits::access_relin_key(rlk);
    const uint64_t N = phantom::VendorTraits::poly_degree(ctx);
    const uint64_t L = phantom::VendorTraits::coeff_mod_size(ctx);
    const uint64_t num_keys = access.key_data_ptrs.size();
    const size_t key_component_bytes = 2 * N * L * sizeof(uint64_t);
    const size_t data_size = sizeof(uint64_t) + sizeof(key_parms_id_type) + num_keys * key_component_bytes;

    KeyHeader hdr;
    hdr.key_type = static_cast<uint16_t>(KeyType::RELIN_KEY);
    hdr.poly_modulus_degree = N;
    hdr.coeff_modulus_size = L;
    hdr.data_size = data_size;
    hdr.key_specific = num_keys;

    KeyWriter writer(io, hdr);
    writer.write_header();
    writer.write_meta(&num_keys, sizeof(uint64_t));
    writer.write_meta(access.parms_id.data(), sizeof(key_parms_id_type));

    for (uint64_t i = 0; i < num_keys; i++) {
        writer.write_gpu_data(access.key_data_ptrs[i], key_component_bytes);
    }
    size_t total = writer.finish();

    if (total > 0) {
        spdlog::info("[kms] Saved relin_key.bin via {} ({} bytes, {} keys)", io.transport_name(), total, num_keys);
    }
    return total;
}

//=============================================================================
// KeySerializer<VendorTraits> — Per-key galois key save
//=============================================================================

template <>
bool KeySerializer<phantom::VendorTraits>::save_galois_key_per_key(const std::string& model_dir, const GaloisKey& rtk,
                                                                   const ContextT& ctx,
                                                                   const std::vector<uint32_t>& galois_elts,
                                                                   const std::vector<std::string>& disk_paths,
                                                                   rtio::IoPolicy io_policy)
{

    auto access = phantom::VendorTraits::access_galois_key(rtk);
    const uint64_t N = phantom::VendorTraits::poly_degree(ctx);
    const uint64_t L = phantom::VendorTraits::coeff_mod_size(ctx);
    const uint64_t num_relin_keys = access.relin_key_ptrs.size();
    const size_t key_component_bytes = 2 * N * L * sizeof(uint64_t);

    // Resolve effective disk paths
    std::vector<std::string> effective_disks;
    if (!disk_paths.empty()) {
        effective_disks = disk_paths;
    }
    else {
        effective_disks.push_back(model_dir);
    }
    const size_t num_disks = effective_disks.size();

    // Ensure subdirectory on each disk
    for (const auto& dp : effective_disks) {
        ensure_dir(dp);
    }

    GaloisKeyIndex idx;
    idx.poly_modulus_degree = N;
    idx.coeff_modulus_size = L;
    idx.parms_id = access.parms_id;
    idx.galois_elts = galois_elts;
    idx.key_component_bytes = key_component_bytes;
    idx.disk_paths = effective_disks;

    // Phantom backend always uses per-key (non-merged) format
    idx.version = 1;

    for (uint64_t i = 0; i < num_relin_keys; i++) {
        const auto& rk = access.relin_key_ptrs[i];
        int disk_idx = static_cast<int>(i % num_disks);
        std::string filename = make_per_key_filename(i);
        std::string filepath = effective_disks[disk_idx] + "/" + filename;

        auto io = rtio::open_writer(filepath, rtio::IoLoc::Device, io_policy);

        // Write each key component as raw GPU data
        size_t written = 0;
        for (const auto& key_ptr : rk) {
            if (!io->write_gpu(key_ptr, written, key_component_bytes)) {
                spdlog::error("[kms] Failed to write {}", filepath);
                return false;
            }
            written += key_component_bytes;
        }

        GaloisKeyIndex::RelinKeyEntry entry;
        entry.file = filename;
        entry.disk_index = disk_idx;
        entry.size = written;
        entry.nk = rk.size();
        entry.crc32 = 0;
        idx.relin_keys.push_back(entry);

        spdlog::info("[kms] Saved {} via {} ({} bytes, nk={}, disk={})", filepath, io->transport_name(), written,
                     rk.size(), disk_idx);
    }

    // Write index.json next to the key files
    std::string index_path = effective_disks[0] + "/galois_key.index.json";
    if (!write_galois_key_index(index_path, idx)) {
        return false;
    }

    spdlog::info("[kms] Saved galois_key per-key: {} files, {} disks, {} elts", num_relin_keys, num_disks,
                 galois_elts.size());
    return true;
}

//=============================================================================
// KeySerializer<VendorTraits> — save_galois_key_merged
//=============================================================================

template <>
bool KeySerializer<phantom::VendorTraits>::save_galois_key_merged(const std::string& model_dir, const GaloisKey& rtk,
                                                                  const ContextT& ctx,
                                                                  const std::vector<uint32_t>& galois_elts,
                                                                  const std::vector<std::string>& disk_paths,
                                                                  rtio::IoPolicy io_policy)
{

    auto access = phantom::VendorTraits::access_galois_key(rtk);
    const uint64_t N = phantom::VendorTraits::poly_degree(ctx);
    const uint64_t L = phantom::VendorTraits::coeff_mod_size(ctx);
    const uint64_t num_relin_keys = access.relin_key_ptrs.size();
    const size_t key_component_bytes = 2 * N * L * sizeof(uint64_t);

    // Resolve effective disk paths
    std::vector<std::string> effective_disks;
    if (!disk_paths.empty()) {
        effective_disks = disk_paths;
    }
    else {
        effective_disks.push_back(model_dir);
    }
    const size_t num_disks = effective_disks.size();

    // Ensure subdirectory on each disk
    for (const auto& dp : effective_disks) {
        ensure_dir(dp);
    }

    GaloisKeyIndex idx;
    idx.poly_modulus_degree = N;
    idx.coeff_modulus_size = L;
    idx.parms_id = access.parms_id;
    idx.galois_elts = galois_elts;
    idx.key_component_bytes = key_component_bytes;
    idx.disk_paths = effective_disks;
    idx.version = 3;  // merged multi-file format

    // Number of sub-files per disk.  Multiple files per disk give N
    // independent NVMe submission queues for GDS reads, which is critical
    // for throughput (4 files/disk = 4× queue depth ≈ 4× bandwidth).
    const int FILES_PER_DISK = 4;

    // Open FILES_PER_DISK writers per disk.  Keys are assigned round-robin
    // across the (num_disks × FILES_PER_DISK) writers so that each sub-file
    // gets a contiguous batch of keys.
    // Writer index = (disk * FILES_PER_DISK) + file_slot
    const int total_writers = static_cast<int>(num_disks) * FILES_PER_DISK;
    std::vector<std::unique_ptr<rtio::IoTransport>> writers(total_writers);
    std::vector<size_t> writer_offsets(total_writers, 0);
    std::vector<std::string> writer_filenames(total_writers);

    for (size_t d = 0; d < num_disks; d++) {
        for (int s = 0; s < FILES_PER_DISK; s++) {
            int w = static_cast<int>(d) * FILES_PER_DISK + s;
            writer_filenames[w] = "galois_key.merged." + std::to_string(d) + "." + std::to_string(s) + ".bin";
            std::string filepath = effective_disks[d] + "/" + writer_filenames[w];
            writers[w] = rtio::open_writer(filepath, rtio::IoLoc::Device, io_policy);
            if (!writers[w]) {
                spdlog::error("[kms] Cannot open merged file {} for writing", filepath);
                return false;
            }
        }
    }

    for (uint64_t i = 0; i < num_relin_keys; i++) {
        const auto& rk = access.relin_key_ptrs[i];
        int disk_idx = static_cast<int>(i % num_disks);
        // Distribute keys for this disk across FILES_PER_DISK sub-files
        int file_slot = static_cast<int>((i / num_disks) % FILES_PER_DISK);
        int w = disk_idx * FILES_PER_DISK + file_slot;
        size_t offset = writer_offsets[w];

        for (const auto& key_ptr : rk) {
            if (!writers[w]->write_gpu(key_ptr, offset, key_component_bytes)) {
                spdlog::error("[kms] Failed to write key {} at offset {} on disk {} slot {}", i, offset, disk_idx,
                              file_slot);
                return false;
            }
            offset += key_component_bytes;
        }

        GaloisKeyIndex::RelinKeyEntry entry;
        entry.file = writer_filenames[w];
        entry.disk_index = disk_idx;
        entry.file_slot = file_slot;
        entry.offset = writer_offsets[w];
        entry.size = offset - writer_offsets[w];
        entry.nk = rk.size();
        entry.crc32 = 0;
        idx.relin_keys.push_back(entry);

        writer_offsets[w] = offset;
    }

    // Writers are closed by unique_ptr destructor

    // Write index.json next to the key files (on disk 0)
    std::string index_path = effective_disks[0] + "/galois_key.index.json";
    if (!write_galois_key_index(index_path, idx)) {
        return false;
    }

    // Summary
    size_t total_bytes = 0;
    for (int w = 0; w < total_writers; w++) {
        total_bytes += writer_offsets[w];
        if (writer_offsets[w] > 0) {
            spdlog::debug("[kms] Merged file {}: {} bytes", writer_filenames[w], writer_offsets[w]);
        }
    }
    spdlog::info("[kms] Saved galois_key merged: {} keys, {} disks, {} files/disk, {} elts, {:.2f} GiB", num_relin_keys,
                 num_disks, FILES_PER_DISK, galois_elts.size(), total_bytes / (1024.0 * 1024 * 1024));
    return true;
}

//=============================================================================
// KeySerializer<VendorTraits> — Single-file key load implementations
// (SecretKey, PublicKey, RelinKey remain in KeyHeader/KeyReader format)
//=============================================================================

template <>
size_t KeySerializer<phantom::VendorTraits>::load_secret_key(rtio::IoTransport& io, SecretKey& sk, const ContextT& ctx)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();
    if (!validate_key_header(hdr))
        return 0;
    if (hdr.key_type != static_cast<uint16_t>(KeyType::SECRET_KEY))
        return 0;

    const uint64_t N = hdr.poly_modulus_degree;
    const uint64_t L = hdr.coeff_modulus_size;

    // Read metadata
    uint64_t sk_max_power;
    if (!reader.read_meta(&sk_max_power, sizeof(uint64_t)))
        return 0;

    // Allocate GPU memory and read
    const size_t data_rns_bytes = N * L * sizeof(uint64_t);
    const size_t sk_array_bytes = sk_max_power * N * L * sizeof(uint64_t);

    uint64_t* data_rns = nullptr;
    uint64_t* sk_array = nullptr;
    FHE_CHECK_CUDA(cudaMalloc(&data_rns, data_rns_bytes));
    if (!reader.read_gpu_data(data_rns, data_rns_bytes)) {
        cudaFree(data_rns);
        return 0;
    }

    FHE_CHECK_CUDA(cudaMalloc(&sk_array, sk_array_bytes));
    if (!reader.read_gpu_data(sk_array, sk_array_bytes)) {
        cudaFree(data_rns);
        cudaFree(sk_array);
        return 0;
    }

    // Validate CRC
    if (!reader.validate_crc(hdr.crc32)) {
        spdlog::warn("[kms] CRC mismatch in secret_key.bin");
        cudaFree(data_rns);
        cudaFree(sk_array);
        return 0;
    }

    // Populate key object
    typename phantom::VendorTraits::SecretKeyPopulate populate;
    populate.data_rns = data_rns;
    populate.sk_array = sk_array;
    populate.sk_max_pow = sk_max_power;
    phantom::VendorTraits::populate_secret_key(sk, populate, N, L, const_cast<ContextT&>(ctx));

    spdlog::info("[kms] Loaded secret_key.bin via {} (N={}, L={}, max_pow={})", io.transport_name(), N, L,
                 sk_max_power);
    return reader.offset();
}

template <>
size_t KeySerializer<phantom::VendorTraits>::load_public_key(rtio::IoTransport& io, PublicKey& pk, const ContextT& ctx)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();
    if (!validate_key_header(hdr))
        return 0;
    if (hdr.key_type != static_cast<uint16_t>(KeyType::PUBLIC_KEY))
        return 0;

    const uint64_t N = hdr.poly_modulus_degree;
    const uint64_t L = hdr.coeff_modulus_size;
    const uint64_t pk_size = hdr.key_specific;

    // Read metadata (chain_index + scale)
    uint8_t meta_buf[sizeof(uint64_t) + sizeof(double)];
    if (!reader.read_meta(meta_buf, sizeof(meta_buf)))
        return 0;
    uint64_t chain_index;
    double scale;
    std::memcpy(&chain_index, meta_buf, sizeof(uint64_t));
    std::memcpy(&scale, meta_buf + sizeof(uint64_t), sizeof(double));

    // Allocate and read GPU data
    const size_t pk_data_bytes = pk_size * N * L * sizeof(uint64_t);
    uint64_t* pk_data = nullptr;
    FHE_CHECK_CUDA(cudaMalloc(&pk_data, pk_data_bytes));
    if (!reader.read_gpu_data(pk_data, pk_data_bytes)) {
        cudaFree(pk_data);
        return 0;
    }

    // Validate CRC
    if (!reader.validate_crc(hdr.crc32)) {
        spdlog::warn("[kms] CRC mismatch in public_key.bin");
        cudaFree(pk_data);
        return 0;
    }

    // Populate key object
    typename phantom::VendorTraits::PublicKeyPopulate populate;
    populate.pk_data = pk_data;
    populate.pk_size = pk_size;
    populate.chain_index = chain_index;
    populate.scale = scale;
    phantom::VendorTraits::populate_public_key(pk, populate, N, L, const_cast<ContextT&>(ctx));

    spdlog::info("[kms] Loaded public_key.bin via {} (N={}, L={}, pk_size={})", io.transport_name(), N, L, pk_size);
    return reader.offset();
}

template <>
size_t KeySerializer<phantom::VendorTraits>::load_relin_key(rtio::IoTransport& io, RelinKey& rlk, const ContextT& ctx)
{

    KeyReader reader(io);
    KeyHeader hdr = reader.read_header();
    if (!validate_key_header(hdr))
        return 0;
    if (hdr.key_type != static_cast<uint16_t>(KeyType::RELIN_KEY))
        return 0;

    const uint64_t N = hdr.poly_modulus_degree;
    const uint64_t L = hdr.coeff_modulus_size;

    // Read metadata
    uint64_t num_keys;
    if (!reader.read_meta(&num_keys, sizeof(uint64_t)))
        return 0;

    key_parms_id_type parms_id;
    if (!reader.read_meta(parms_id.data(), sizeof(key_parms_id_type)))
        return 0;

    // Allocate and read GPU data
    const size_t key_component_bytes = 2 * N * L * sizeof(uint64_t);
    std::vector<uint64_t*> key_data(num_keys);
    for (uint64_t i = 0; i < num_keys; i++) {
        FHE_CHECK_CUDA(cudaMalloc(&key_data[i], key_component_bytes));
        if (!reader.read_gpu_data(key_data[i], key_component_bytes)) {
            for (uint64_t j = 0; j <= i; j++) {
                if (key_data[j])
                    cudaFree(key_data[j]);
            }
            return 0;
        }
    }

    // Validate CRC
    if (!reader.validate_crc(hdr.crc32)) {
        spdlog::warn("[kms] CRC mismatch in relin_key.bin");
        for (auto& ptr : key_data) {
            if (ptr)
                cudaFree(ptr);
        }
        return 0;
    }

    // Populate key object
    typename phantom::VendorTraits::RelinKeyPopulate populate;
    populate.key_data = key_data;
    populate.num_keys = num_keys;
    populate.parms_id = parms_id;
    phantom::VendorTraits::populate_relin_key(rlk, populate, N, L, const_cast<ContextT&>(ctx));

    spdlog::info("[kms] Loaded relin_key.bin via {} (N={}, L={}, {} keys)", io.transport_name(), N, L, num_keys);
    return reader.offset();
}

//=============================================================================
// KeySerializer<VendorTraits> — Merged galois key load (GDS-optimized)
//=============================================================================

template <>
bool KeySerializer<phantom::VendorTraits>::load_galois_key_merged(const std::string& key_dir, GaloisKey& rtk,
                                                                  ContextT& ctx, std::vector<uint32_t>& galois_elts,
                                                                  const GaloisKeyIndex& idx, rtio::IoPolicy io_policy)
{

    spdlog::debug("[key-load] load_galois_key_merged ENTER: key_dir={}, version={}, num_relin_keys={}, disk_paths={}",
                  key_dir, idx.version, idx.relin_keys.size(), idx.disk_paths.size());

    using Clock = std::chrono::high_resolution_clock;
    galois_elts = idx.galois_elts;

    const uint64_t N = idx.poly_modulus_degree;
    const uint64_t L = idx.coeff_modulus_size;
    const size_t key_component_bytes = idx.key_component_bytes;
    const size_t num_relin_keys = idx.relin_keys.size();
    const auto& disk_paths = idx.disk_paths;

    auto t_step0 = Clock::now();

    // ── Step 1: Group keys by (disk, file_slot) ──
    // Version 3 (multi-file per disk): each disk has N sub-files with
    // independent NVMe submission queues → N× queue depth for GDS reads.
    // This matches the bench_io_read multi-file pattern.
    struct FileReader
    {
        int disk_index;
        int file_slot;
        std::string filepath;
        std::vector<size_t> key_indices;
        size_t total_bytes = 0;
        // No buf, no reader — allocated/destroyed per-file to match
        // bench_io_read's open→register→read→deregister lifecycle.
    };

    // Use pair<disk, slot> as key for grouping
    std::map<std::pair<int, int>, FileReader> file_readers;

    for (size_t i = 0; i < num_relin_keys; i++) {
        const auto& entry = idx.relin_keys[i];
        int d = entry.disk_index;
        int slot = idx.version >= 3 ? entry.file_slot : 0;
        auto key = std::make_pair(d, slot);
        if (file_readers.find(key) == file_readers.end()) {
            FileReader fr;
            fr.disk_index = d;
            fr.file_slot = slot;
            if (static_cast<size_t>(d) < disk_paths.size()) {
                fr.filepath = disk_paths[d] + "/" + entry.file;
            }
            else {
                fr.filepath = key_dir + "/model/" + entry.file;
            }
            file_readers[key] = std::move(fr);
        }
        file_readers[key].key_indices.push_back(i);
        file_readers[key].total_bytes += entry.size;
    }

    // Find max sub-file size per disk (for staging buffer allocation)
    std::map<int, size_t> max_file_bytes_per_disk;
    for (auto& [key, fr] : file_readers) {
        auto& mx = max_file_bytes_per_disk[fr.disk_index];
        mx = std::max(mx, fr.total_bytes);
    }

    // Group by disk
    std::map<int, std::vector<FileReader*>> per_disk_readers;
    for (auto& [key, fr] : file_readers) {
        per_disk_readers[fr.disk_index].push_back(&fr);
    }

    int num_files = static_cast<int>(file_readers.size());

    // ── Step 1b: Allocate per-key GPU buffers + per-disk reusable staging buffer ──
    // Staging buffer pattern matches bench_io_read: 1 buffer per disk, reused
    // across files.  Per-key buffers allocated upfront for D2D scatter target.
    std::vector<std::vector<uint64_t*>> relin_key_data(num_relin_keys);
    for (size_t i = 0; i < num_relin_keys; i++) {
        uint64_t nk = idx.relin_keys[i].nk;
        relin_key_data[i].resize(nk, nullptr);
        for (uint64_t j = 0; j < nk; j++) {
            FHE_CHECK_CUDA(cudaMalloc(&relin_key_data[i][j], key_component_bytes));
        }
    }

    // Per-disk reusable staging buffer (sized to largest sub-file on that disk)
    std::map<int, void*> disk_bufs;
    for (auto& [d, max_bytes] : max_file_bytes_per_disk) {
        FHE_CHECK_CUDA(cudaMalloc(&disk_bufs[d], max_bytes));
    }

    auto t_step1 = Clock::now();
    spdlog::info("[key-load] Step1 alloc per-key + staging: {:.3f}s ({} sub-files)",
                 std::chrono::duration<double>(t_step1 - t_step0).count(), num_files);

    // ── Step 2: Pure IO read — using IoBulkReader for parallel multi-file reads ──
    // Allocate one GPU buffer per sub-file (data must survive until D2D scatter).
    std::map<std::pair<int, int>, void*> file_bufs;
    for (auto& [key, fr] : file_readers) {
        FHE_CHECK_CUDA(cudaMalloc(&file_bufs[key], fr.total_bytes));
    }

    // Build file list and buffer list for IoBulkReader
    std::vector<rtio::BulkReadFile> bulk_files;
    std::vector<void*> bulk_buffers;
    bulk_files.reserve(file_readers.size());
    bulk_buffers.reserve(file_readers.size());
    for (auto& [key, fr] : file_readers) {
        bulk_files.push_back({fr.filepath, fr.total_bytes});
        bulk_buffers.push_back(file_bufs[key]);
    }

    // Execute bulk read (one thread per disk, files on same disk read sequentially)
    rtio::IoBulkReader bulk_reader(io_policy, disk_paths, /*num_io_threads=*/1);
    auto t_io_start = Clock::now();
    auto io_result = bulk_reader.read(bulk_files, bulk_buffers);
    auto t_io_end = Clock::now();
    double s_io = std::chrono::duration<double>(t_io_end - t_io_start).count();
    size_t total_galois_bytes = 0;
    for (auto& [key, fr] : file_readers)
        total_galois_bytes += fr.total_bytes;
    spdlog::info("[key-load] Step2 pure-IO read: {:.3f}s ({:.2f} GiB/s, {:.2f} GiB total, {} files, transport={})",
                 s_io, total_galois_bytes / (1024.0 * 1024 * 1024) / (s_io > 0 ? s_io : 1),
                 total_galois_bytes / (1024.0 * 1024 * 1024), num_files, io_result.transport_name);

    // Free per-disk staging buffers (no longer needed)
    for (auto& [d, buf] : disk_bufs) {
        if (buf)
            cudaFree(buf);
    }

    if (!io_result.success) {
        for (auto& [k, buf] : file_bufs) {
            if (buf)
                cudaFree(buf);
        }
        for (auto& rk : relin_key_data) {
            for (auto ptr : rk) {
                if (ptr)
                    cudaFree(ptr);
            }
        }
        return false;
    }

    // ── Step 3: D2D scatter from file buffers into per-key GPU buffers ──
    auto t_step3 = Clock::now();

    for (auto& [key, fr] : file_readers) {
        char* base = static_cast<char*>(file_bufs[key]);
        for (size_t ki : fr.key_indices) {
            const auto& entry = idx.relin_keys[ki];
            size_t src_offset = entry.offset;
            uint64_t nk = entry.nk;
            for (uint64_t j = 0; j < nk; j++) {
                FHE_CHECK_CUDA(cudaMemcpy(relin_key_data[ki][j], base + src_offset + j * key_component_bytes,
                                          key_component_bytes, cudaMemcpyDeviceToDevice));
            }
        }
    }

    // Free file staging buffers
    for (auto& [k, buf] : file_bufs) {
        if (buf)
            cudaFree(buf);
    }

    auto t_step3_end = Clock::now();
    spdlog::info("[key-load] Step3 D2D scatter: {:.3f}s", std::chrono::duration<double>(t_step3_end - t_step3).count());

    if (!io_result.success) {
        for (auto& rk : relin_key_data) {
            for (auto ptr : rk) {
                if (ptr)
                    cudaFree(ptr);
            }
        }
        return false;
    }

    // ── Step 4: Assemble GaloisKey ──
    typename phantom::VendorTraits::GaloisKeyPopulate populate;
    populate.relin_key_data = relin_key_data;
    populate.galois_elts = galois_elts;
    populate.parms_id = idx.parms_id;
    phantom::VendorTraits::populate_galois_key(rtk, populate, N, L, ctx);

    spdlog::info("[kms] Loaded galois_key merged: {} relin keys, {} elts, {} sub-files", num_relin_keys,
                 galois_elts.size(), num_files);
    return true;
}

//=============================================================================
// KeySerializer<VendorTraits> — Per-key galois key load (parallel)
//=============================================================================

template <>
bool KeySerializer<phantom::VendorTraits>::load_galois_key_per_key(const std::string& key_dir, GaloisKey& rtk,
                                                                   ContextT& ctx, std::vector<uint32_t>& galois_elts,
                                                                   rtio::IoPolicy io_policy)
{

    galois_elts.clear();

    // Step 1: Read index.json
    std::string index_path = key_dir + "/model/galois_key.index.json";

    GaloisKeyIndex idx;
    if (!read_galois_key_index(index_path, idx)) {
        spdlog::error("[kms] Cannot read galois_key.index.json from {}", index_path);
        return false;
    }

    const uint64_t N = idx.poly_modulus_degree;
    const uint64_t L = idx.coeff_modulus_size;
    const size_t key_component_bytes = idx.key_component_bytes;
    const size_t num_relin_keys = idx.relin_keys.size();

    // Dispatch: merged format (version >= 2 or offset > 0) vs per-key format
    bool is_merged = (idx.version >= 2);
    if (!is_merged) {
        for (const auto& e : idx.relin_keys) {
            if (e.offset > 0) {
                is_merged = true;
                break;
            }
        }
    }
    spdlog::debug("[key-load] load_galois_key_per_key: version={}, is_merged={}, num_relin_keys={}", idx.version,
                  is_merged, num_relin_keys);
    if (is_merged) {
        return load_galois_key_merged(key_dir, rtk, ctx, galois_elts, idx, io_policy);
    }

    // --- Per-key format (legacy, version 1) ---

    // disk_paths from index.json record where files were actually saved
    const auto& disk_paths = idx.disk_paths;

    // Step 2: Pre-allocate all GPU buffers (sequential but fast — no I/O)
    std::vector<std::vector<uint64_t*>> relin_key_data(num_relin_keys);
    for (size_t i = 0; i < num_relin_keys; i++) {
        uint64_t nk = idx.relin_keys[i].nk;
        relin_key_data[i].resize(nk, nullptr);
        for (uint64_t j = 0; j < nk; j++) {
            FHE_CHECK_CUDA(cudaMalloc(&relin_key_data[i][j], key_component_bytes));
        }
    }

    // Step 3: Parallel load
    // FHERT_GALOIS_LOAD_THREADS=<n> overrides thread count (default 4). Set to 1
    // to run single-threaded — a diagnostic for hang reports on hosts where the
    // multi-threaded cudaMemcpy/fopen path dead-waits.
    int req_threads = std::min(static_cast<int>(num_relin_keys), 4);
    if (const char* env = std::getenv("FHERT_GALOIS_LOAD_THREADS")) {
        int n = std::atoi(env);
        if (n > 0)
            req_threads = std::min(n, static_cast<int>(num_relin_keys));
    }
    const int num_threads = req_threads;
    // FHERT_GALOIS_LOAD_DBG=1 enables the [gload-dbg] per-key trace to stderr.
    // Default off to keep the load path quiet.
    const bool dbg_on = [] {
        const char* e = std::getenv("FHERT_GALOIS_LOAD_DBG");
        return e != nullptr && std::atoi(e) != 0;
    }();
    std::atomic<size_t> next_key{0};
    std::atomic<bool> failed{false};

    auto worker = [&](int thread_id) {
        while (!failed.load()) {
            size_t i = next_key.fetch_add(1);
            if (i >= num_relin_keys)
                break;

            const auto& entry = idx.relin_keys[i];
            uint64_t nk = entry.nk;

            // Build file path
            std::string filepath;
            if (entry.disk_index >= 0 && static_cast<size_t>(entry.disk_index) < disk_paths.size()) {
                filepath = disk_paths[entry.disk_index] + "/" + entry.file;
            }
            else {
                // Fallback: try model_dir directly
                filepath = key_dir + "/model/" + entry.file;
            }

            // Open reader for this key file
            if (dbg_on) {
                std::fprintf(stderr, "[gload-dbg] T%d: key %zu fopen-start %s\n", thread_id, i, filepath.c_str());
                std::fflush(stderr);
            }
            auto io = rtio::open_reader(filepath, rtio::IoLoc::Device, io_policy);
            if (dbg_on) {
                std::fprintf(stderr, "[gload-dbg] T%d: key %zu fopen-done read-start nk=%lu\n", thread_id, i,
                             static_cast<unsigned long>(nk));
                std::fflush(stderr);
            }

            // Read each key component directly into pre-allocated GPU buffer
            size_t offset = 0;
            bool ok = true;
            for (uint64_t j = 0; j < nk; j++) {
                if (!io->read_gpu(relin_key_data[i][j], offset, key_component_bytes)) {
                    spdlog::error("[kms] Thread {} failed to read {} component {}", thread_id, filepath, j);
                    ok = false;
                    break;
                }
                offset += key_component_bytes;
            }
            if (dbg_on) {
                std::fprintf(stderr, "[gload-dbg] T%d: key %zu read-done ok=%d\n", thread_id, i, ok);
                std::fflush(stderr);
            }

            if (ok) {
                spdlog::debug("[kms] Thread {} loaded {} ({} bytes, via {})", thread_id, filepath, offset,
                              io->transport_name());
            }
            else {
                failed.store(true);
            }
        }
    };

    spdlog::info("[kms] Starting parallel galois key load: {} keys, {} threads", num_relin_keys, num_threads);

    if (num_threads <= 1) {
        // Single-threaded path (no thread overhead)
        worker(0);
    }
    else {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        for (int t = 0; t < num_threads; t++) {
            threads.emplace_back(worker, t);
        }
        for (auto& th : threads) {
            th.join();
        }
    }

    if (failed.load()) {
        spdlog::error("[kms] Parallel galois key load failed");
        // Clean up allocated GPU memory
        for (auto& rk : relin_key_data) {
            for (auto ptr : rk) {
                if (ptr)
                    cudaFree(ptr);
            }
        }
        return false;
    }

    // Step 4: CRC validation (optional — skip for GDS-saved keys where crc32=0)
    // For staged-saved keys, we could reload and check, but that defeats the
    // purpose. CRC info in index.json is for integrity verification at
    // save time or offline auditing. GDS path is protected by hardware ECC.
    // If needed, a dedicated verify step can D2H + CRC all keys.

    // Step 5: Assemble GaloisKeyPopulate and populate
    galois_elts = idx.galois_elts;

    typename phantom::VendorTraits::GaloisKeyPopulate populate;
    populate.relin_key_data = relin_key_data;
    populate.galois_elts = galois_elts;
    populate.parms_id = idx.parms_id;
    phantom::VendorTraits::populate_galois_key(rtk, populate, N, L, ctx);

    spdlog::info("[kms] Loaded galois_key per-key: {} relin keys, {} elts", num_relin_keys, galois_elts.size());
    return true;
}

//=============================================================================
// KeySerializer<phantom::VendorTraits> — on-demand primitives
//=============================================================================

template <>
bool KeySerializer<phantom::VendorTraits>::Load_sk_pk_relin_only(const std::string& key_dir, SecretKey& sk,
                                                                 PublicKey& pk, RelinKey& rlk, ContextT& ctx,
                                                                 rtio::IoPolicy io_policy)
{
    // SK + PK + RLK (no galois bytes) for ON_DEMAND_FULL: needs encrypt/decrypt
    // but defers galois key bytes to Ensure_galois_key. Mirrors Load_keys' SK/PK/RLK
    // segments but skips the galois load.
    const auto loc = phantom::VendorTraits::io_location();
    std::string global_dir = key_dir + "/global";
    std::string model_dir = key_dir + "/model";

    {
        auto io = rtio::open_reader(global_dir + "/secret_key.bin", loc, io_policy);
        if (!io || load_secret_key(*io, sk, ctx) == 0) {
            spdlog::error("[kms] Failed to load secret_key.bin (ON_DEMAND_FULL)");
            return false;
        }
    }
    {
        auto io = rtio::open_reader(global_dir + "/public_key.bin", loc, io_policy);
        if (!io || load_public_key(*io, pk, ctx) == 0) {
            spdlog::error("[kms] Failed to load public_key.bin (ON_DEMAND_FULL)");
            return false;
        }
    }
    {
        auto io = rtio::open_reader(model_dir + "/relin_key.bin", loc, io_policy);
        if (!io || load_relin_key(*io, rlk, ctx) == 0) {
            spdlog::error("[kms] Failed to load relin_key.bin (ON_DEMAND_FULL)");
            return false;
        }
    }
    spdlog::info("[kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from {}", key_dir);
    return true;
}

template <>
bool KeySerializer<phantom::VendorTraits>::Load_relin_key_only(const std::string& key_dir, RelinKey& rlk, ContextT& ctx,
                                                               rtio::IoPolicy io_policy)
{
    // Relin key is small and used throughout the program — fully preload it
    // even in ON_DEMAND mode. Only galois keys are deferred.
    const auto loc = phantom::VendorTraits::io_location();
    std::string model_dir = key_dir + "/model";
    auto io = rtio::open_reader(model_dir + "/relin_key.bin", loc, io_policy);
    if (!io || load_relin_key(*io, rlk, ctx) == 0) {
        spdlog::error("[kms] Failed to load relin_key.bin (ON_DEMAND)");
        return false;
    }
    spdlog::info("[kms] Relin key preloaded (ON_DEMAND) from {}", model_dir);
    return true;
}

template <>
bool KeySerializer<phantom::VendorTraits>::Load_galois_index_only(const std::string& key_dir, GaloisKeyIndex& idx,
                                                                  rtio::IoPolicy /*io_policy*/)
{
    // Only the index JSON is read here; key bytes are deferred to
    // Read_one_relin_key by the ON_DEMAND path. io_policy is unused for the
    // (small, host-side) JSON read but kept in the signature for symmetry.
    std::string index_path = key_dir + "/model/galois_key.index.json";
    if (!read_galois_key_index(index_path, idx)) {
        spdlog::error("[kms] Cannot read galois_key.index.json from {}", index_path);
        return false;
    }
    spdlog::info("[kms] Loaded galois-key index only: {} relin keys, {} elts (ON_DEMAND, no key bytes read)",
                 idx.relin_keys.size(), idx.galois_elts.size());
    return true;
}

template <>
bool KeySerializer<phantom::VendorTraits>::Read_one_relin_key(const GaloisKeyIndex& idx, size_t i,
                                                              /*out*/ std::vector<uint64_t*>& bufs,
                                                              rtio::IoPolicy io_policy)
{
    if (i >= idx.relin_keys.size()) {
        spdlog::error("[kms] Read_one_relin_key: index {} out of range ({})", i, idx.relin_keys.size());
        return false;
    }
    const auto& entry = idx.relin_keys[i];
    const uint64_t nk = entry.nk;
    const size_t kcb = static_cast<size_t>(idx.key_component_bytes);

    if (bufs.size() < nk) {
        spdlog::error("[kms] Read_one_relin_key: bufs has {} entries, need {} (nk)", bufs.size(), nk);
        return false;
    }

    // Build file path (same logic as the load_galois_key_per_key worker).
    std::string filepath;
    const auto& disk_paths = idx.disk_paths;
    if (entry.disk_index >= 0 && static_cast<size_t>(entry.disk_index) < disk_paths.size()) {
        filepath = disk_paths[entry.disk_index] + "/" + entry.file;
    }
    else {
        // Fall back to model/ — caller knows the key_dir; reconstruct from a
        // symlink-free convention by attempting the index's disk hint first.
        spdlog::warn("[kms] Read_one_relin_key: bad disk_index {} for key {}", entry.disk_index, i);
        return false;
    }

    auto io = rtio::open_reader(filepath, rtio::IoLoc::Device, io_policy);
    if (!io) {
        spdlog::error("[kms] Read_one_relin_key: open_reader failed for {}", filepath);
        return false;
    }

    // entry.offset is this relin key's first component's byte offset within the
    // (possibly merged) sub-file; component j sits at entry.offset + j*kcb.
    // For v1 per-key format entry.offset == 0, so this reduces to the worker path.
    bool ok = true;
    for (uint64_t j = 0; j < nk; j++) {
        size_t comp_off = entry.offset + static_cast<size_t>(j) * kcb;
        if (!io->read_gpu(bufs[j], comp_off, kcb)) {
            spdlog::error("[kms] Read_one_relin_key: failed to read {} component {} at offset {}", filepath, j,
                          comp_off);
            ok = false;
            break;
        }
    }

    if (ok) {
        spdlog::debug("[kms] Read_one_relin_key: loaded key {} ({} comps x {} bytes, via {})", i, nk, kcb,
                      io->transport_name());
    }
    return ok;
}

//=============================================================================
// KeySerializer<VendorTraits> — Galois-key save/load dispatch
//=============================================================================

template <>
bool KeySerializer<phantom::VendorTraits>::save_galois_keys(const std::string& model_dir, const GaloisKey& rtk,
                                                            const ContextT& ctx,
                                                            const std::vector<uint32_t>& galois_elts,
                                                            rtio::IoPolicy io_policy)
{

    // Dispatch based on galois_merged_format setting.
    auto io_cfg = rtio::get_global_io_config();
    if (io_cfg.galois_merged_format) {
        return save_galois_key_merged(model_dir, rtk, ctx, galois_elts, io_cfg.disk_paths, io_policy);
    }
    else {
        return save_galois_key_per_key(model_dir, rtk, ctx, galois_elts, io_cfg.disk_paths, io_policy);
    }
}

template <>
bool KeySerializer<phantom::VendorTraits>::load_galois_keys(const std::string& key_dir,
                                                            const std::string& /*model_dir*/, GaloisKey& rtk,
                                                            ContextT& ctx, std::vector<uint32_t>& galois_elts,
                                                            rtio::IoPolicy io_policy)
{

    // Phantom always loads via per-key format (which dispatches to merged
    // internally if the index version ≥ 2).
    g_galois_load_time_seconds = -1.0;  // reset before load
    auto t_gk0 = std::chrono::high_resolution_clock::now();
    if (!load_galois_key_per_key(key_dir, rtk, ctx, galois_elts, io_policy)) {
        spdlog::error("[kms] Failed to load galois_key (per-key format)");
        return false;
    }
    auto t_gk1 = std::chrono::high_resolution_clock::now();
    double s_galois = std::chrono::duration<double>(t_gk1 - t_gk0).count();
    g_galois_load_time_seconds = s_galois;
    spdlog::info("[kms] GaloisKey load: {:.2f}s ({} elts)", s_galois, galois_elts.size());
    return true;
}

}  // namespace fhert
