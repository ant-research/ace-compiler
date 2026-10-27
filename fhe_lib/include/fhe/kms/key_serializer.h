//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_serializer.h
//! Templated key serialization for FHE runtime backends.
//!
//! KeySerializer<VendorTraits> provides the high-level Save_keys/Load_keys
//! API that orchestrates directory structure and per-key serialization.
//! Each backend provides a VendorTraits struct that maps this framework
//! to its vendor key types and memory layouts.
//!
//! IO is configured via IoPolicy (not IoConfig) — business code describes
//! the scenario, the IO layer decides the transport. See io_policy.h.
//!
//! Vendor-specific behavior is factored into hooks on VendorTraits:
//!   - io_location() → IoLoc::Host or IoLoc::Device
//!   - should_save_key(key) → bool (skip ungenerated keys?)
//!   - resolve_galois_elts(ctx, elts) → std::vector<uint32_t>
//!
//! The per-key save/load methods and galois-key format dispatch remain
//! backend-specific (implemented in each backend's key_serializer.cu).

#ifndef FHERT_KEY_SERIALIZER_H
#define FHERT_KEY_SERIALIZER_H

#include "fhe/io/io_policy.h"
#include "fhe/kms/key_header.h"
#include "fhe/kms/key_index.h"
#include "fhe/kms/key_io.h"
#include "fhe/util/logging.h"

#include <cstdint>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace fhert
{

//=============================================================================
// KeySerializer<VendorTraits>
//=============================================================================

template <typename Traits> class KeySerializer
{
public:
    using SecretKey = typename Traits::SecretKey;
    using PublicKey = typename Traits::PublicKey;
    using RelinKey = typename Traits::RelinKey;
    using GaloisKey = typename Traits::GaloisKey;
    using ContextT = typename Traits::Context;

    /// Wall-clock seconds of the most recent galois-key load performed by this
    /// serializer specialization (-1.0 before any load / on failure). Set inside
    /// load_galois_keys; read by KeyMgr::Load_keys into its instance field so
    /// the galois timer is surfaced through FheContext::Current().key_mgr().
    /// Single-context-per-process keeps this safe without atomics.
    /// `inline` so it has a single definition across the backends that include
    /// this header (C++17 inline variable), avoiding ODR/linker duplicates.
    static inline double g_galois_load_time_seconds = -1.0;

    //-------------------------------------------------------------------------
    // High-level orchestration (inline template — common to all backends)
    //-------------------------------------------------------------------------

    /// Save all keys to a directory bundle.
    static bool Save_keys(const std::string& key_dir, const SecretKey& sk, const PublicKey& pk, const RelinKey& rlk,
                          const GaloisKey& rtk, const ContextT& ctx, const std::vector<uint32_t>& galois_elts = {},
                          rtio::IoPolicy io_policy = rtio::IoPolicy::auto_)
    {
        const auto loc = Traits::io_location();
        auto elts = Traits::resolve_galois_elts(ctx, galois_elts);

        // Create directory structure
        std::string global_dir = key_dir + "/global";
        std::string model_dir = key_dir + "/model";
        ensure_dir(key_dir);
        ensure_dir(global_dir);
        ensure_dir(model_dir);

        // Save SecretKey
        {
            auto io = rtio::open_writer(global_dir + "/secret_key.bin", loc, io_policy);
            if (!io || save_secret_key(*io, sk, ctx) == 0) {
                spdlog::error("[kms] Failed to save secret_key.bin");
                return false;
            }
        }

        // Save PublicKey (skip if not generated)
        if (Traits::template should_save_key<PublicKey>(pk)) {
            auto io = rtio::open_writer(global_dir + "/public_key.bin", loc, io_policy);
            if (!io || save_public_key(*io, pk, ctx) == 0) {
                spdlog::error("[kms] Failed to save public_key.bin");
                return false;
            }
        }

        // Save RelinKey (skip if not generated)
        if (Traits::template should_save_key<RelinKey>(rlk)) {
            auto io = rtio::open_writer(model_dir + "/relin_key.bin", loc, io_policy);
            if (!io || save_relin_key(*io, rlk, ctx) == 0) {
                spdlog::error("[kms] Failed to save relin_key.bin");
                return false;
            }
        }

        // Save GaloisKey (skip if not generated)
        if (Traits::template should_save_key<GaloisKey>(rtk)) {
            if (!save_galois_keys(model_dir, rtk, ctx, elts, io_policy)) {
                return false;
            }
        }

        spdlog::info("[kms] Keys saved to {}", key_dir);
        return true;
    }

    /// Load all keys from a directory bundle.
    static bool Load_keys(const std::string& key_dir, SecretKey& sk, PublicKey& pk, RelinKey& rlk, GaloisKey& rtk,
                          ContextT& ctx, std::vector<uint32_t>& loaded_galois_elts,
                          rtio::IoPolicy io_policy = rtio::IoPolicy::auto_)
    {
        const auto loc = Traits::io_location();
        std::string global_dir = key_dir + "/global";
        std::string model_dir = key_dir + "/model";

        // Load SecretKey
        {
            auto io = rtio::open_reader(global_dir + "/secret_key.bin", loc, io_policy);
            if (!io || load_secret_key(*io, sk, ctx) == 0) {
                spdlog::error("[kms] Failed to load secret_key.bin");
                return false;
            }
        }

        // Load PublicKey
        {
            auto io = rtio::open_reader(global_dir + "/public_key.bin", loc, io_policy);
            if (!io || load_public_key(*io, pk, ctx) == 0) {
                spdlog::error("[kms] Failed to load public_key.bin");
                return false;
            }
        }

        // Load RelinKey
        {
            auto io = rtio::open_reader(model_dir + "/relin_key.bin", loc, io_policy);
            if (!io || load_relin_key(*io, rlk, ctx) == 0) {
                spdlog::error("[kms] Failed to load relin_key.bin");
                return false;
            }
        }

        // Load GaloisKey
        {
            if (!load_galois_keys(key_dir, model_dir, rtk, ctx, loaded_galois_elts, io_policy)) {
                spdlog::error("[kms] Failed to load galois keys");
                return false;
            }
        }

        spdlog::info("[kms] Keys loaded from {} (galois_elts={})", key_dir, loaded_galois_elts.size());
        return true;
    }

    /// Load evaluation keys (RLK + GK) only.
    static bool Load_eval_keys(const std::string& key_dir, RelinKey& rlk, GaloisKey& rtk, ContextT& ctx,
                               std::vector<uint32_t>& loaded_galois_elts,
                               rtio::IoPolicy io_policy = rtio::IoPolicy::auto_)
    {
        const auto loc = Traits::io_location();
        std::string model_dir = key_dir + "/model";

        // Load RelinKey
        {
            auto io = rtio::open_reader(model_dir + "/relin_key.bin", loc, io_policy);
            if (!io || load_relin_key(*io, rlk, ctx) == 0) {
                spdlog::error("[kms] Failed to load relin_key.bin");
                return false;
            }
        }

        // Load GaloisKey
        {
            if (!load_galois_keys(key_dir, model_dir, rtk, ctx, loaded_galois_elts, io_policy)) {
                spdlog::error("[kms] Failed to load galois keys");
                return false;
            }
        }

        spdlog::info("[kms] Eval keys (RLK+GK) loaded from {} (galois_elts={})", key_dir, loaded_galois_elts.size());
        return true;
    }

    /// Save evaluation keys (RLK + GK) only.
    static bool Save_eval_keys(const std::string& key_dir, const RelinKey& rlk, const GaloisKey& rtk,
                               const ContextT& ctx, const std::vector<uint32_t>& galois_elts = {},
                               rtio::IoPolicy io_policy = rtio::IoPolicy::auto_)
    {
        const auto loc = Traits::io_location();
        auto elts = Traits::resolve_galois_elts(ctx, galois_elts);

        // Create model/ directory only (no global/ for eval-only)
        std::string model_dir = key_dir + "/model";
        ensure_dir(key_dir);
        ensure_dir(model_dir);

        // Save RelinKey (skip if not generated)
        if (Traits::template should_save_key<RelinKey>(rlk)) {
            auto io = rtio::open_writer(model_dir + "/relin_key.bin", loc, io_policy);
            if (!io || save_relin_key(*io, rlk, ctx) == 0) {
                spdlog::error("[kms] Failed to save relin_key.bin");
                return false;
            }
        }

        // Save GaloisKey (skip if not generated)
        if (Traits::template should_save_key<GaloisKey>(rtk)) {
            if (!save_galois_keys(model_dir, rtk, ctx, elts, io_policy)) {
                return false;
            }
        }

        spdlog::info("[kms] Eval keys (RLK+GK) saved to {}", key_dir);
        return true;
    }

    //-------------------------------------------------------------------------
    // Per-key save/load — backend-specific (explicit specialization in .cu)
    //-------------------------------------------------------------------------

    static size_t save_secret_key(rtio::IoTransport& io, const SecretKey& sk, const ContextT& ctx);
    static size_t save_public_key(rtio::IoTransport& io, const PublicKey& pk, const ContextT& ctx);
    static size_t save_relin_key(rtio::IoTransport& io, const RelinKey& rlk, const ContextT& ctx);

    static size_t load_secret_key(rtio::IoTransport& io, SecretKey& sk, const ContextT& ctx);
    static size_t load_public_key(rtio::IoTransport& io, PublicKey& pk, const ContextT& ctx);
    static size_t load_relin_key(rtio::IoTransport& io, RelinKey& rlk, const ContextT& ctx);

    //-------------------------------------------------------------------------
    // Galois-key save/load — backend-specific dispatch
    //-------------------------------------------------------------------------

    /// Save galois keys to the model directory.
    /// ACE: single-file format via save_galois_key().
    /// Phantom: dispatches to per-key or merged format via IoConfig.
    static bool save_galois_keys(const std::string& model_dir, const GaloisKey& rtk, const ContextT& ctx,
                                 const std::vector<uint32_t>& galois_elts, rtio::IoPolicy io_policy);

    /// Load galois keys from the key directory.
    /// ACE: single-file format via load_galois_key().
    /// Phantom: dispatches to per-key format (with merged fallback).
    static bool load_galois_keys(const std::string& key_dir, const std::string& model_dir, GaloisKey& rtk,
                                 ContextT& ctx, std::vector<uint32_t>& galois_elts, rtio::IoPolicy io_policy);

    //-------------------------------------------------------------------------
    // Per-key galois key save/load (legacy single-file format)
    //-------------------------------------------------------------------------

    static size_t save_galois_key(rtio::IoTransport& io, const GaloisKey& rtk, const ContextT& ctx,
                                  const std::vector<uint32_t>& galois_elts);
    static size_t load_galois_key(rtio::IoTransport& io, GaloisKey& rtk, const ContextT& ctx,
                                  std::vector<uint32_t>& galois_elts);

    //-------------------------------------------------------------------------
    // Per-key and merged galois key formats (Phantom-specific)
    //-------------------------------------------------------------------------

    static bool save_galois_key_per_key(const std::string& model_dir, const GaloisKey& rtk, const ContextT& ctx,
                                        const std::vector<uint32_t>& galois_elts,
                                        const std::vector<std::string>& disk_paths, rtio::IoPolicy io_policy);

    static bool load_galois_key_per_key(const std::string& key_dir, GaloisKey& rtk, ContextT& ctx,
                                        std::vector<uint32_t>& galois_elts, rtio::IoPolicy io_policy);

    static bool save_galois_key_merged(const std::string& model_dir, const GaloisKey& rtk, const ContextT& ctx,
                                       const std::vector<uint32_t>& galois_elts,
                                       const std::vector<std::string>& disk_paths, rtio::IoPolicy io_policy);

    static bool load_galois_key_merged(const std::string& key_dir, GaloisKey& rtk, ContextT& ctx,
                                       std::vector<uint32_t>& galois_elts, const GaloisKeyIndex& idx,
                                       rtio::IoPolicy io_policy);

    //-------------------------------------------------------------------------
    // On-demand galois-key primitives (Phantom-specific)
    //-------------------------------------------------------------------------

    /// Read only the galois-key index (galois_key.index.json) without loading
    /// any key bytes. Used by KeyMode::ON_DEMAND Init to get the element set
    /// + per-entry layout up front, deferring key-byte reads to Read_one_relin_key.
    /// @param key_dir  Directory containing model/galois_key.index.json
    /// @param idx      [out] Parsed index
    /// @return true on success
    static bool Load_galois_index_only(const std::string& key_dir, GaloisKeyIndex& idx, rtio::IoPolicy io_policy);

    /// Load only the relinearization key (model_dir/relin_key.bin) — used by
    /// KeyMode::ON_DEMAND Init, which fully preloads the small relin key but
    /// defers all galois-key bytes to Ensure_galois_key.
    static bool Load_relin_key_only(const std::string& key_dir, RelinKey& rlk, ContextT& ctx, rtio::IoPolicy io_policy);

    /// Load SK + PK + RLK only (no galois-key bytes). Used by
    /// KeyMode::ON_DEMAND_FULL Init for end-to-end verification: it needs
    /// encrypt/decrypt (SK/PK) but defers all galois bytes to Ensure_galois_key.
    static bool Load_sk_pk_relin_only(const std::string& key_dir, SecretKey& sk, PublicKey& pk, RelinKey& rlk,
                                      ContextT& ctx, rtio::IoPolicy io_policy);

    /// Read ONE relin key's bytes (nk components) from disk into pre-allocated
    /// device buffers. Extracted from the load_galois_key_per_key worker so
    /// ON_DEMAND's Ensure_galois_key can fetch a single key on cache miss.
    /// @param idx    GaloisKeyIndex (from Load_galois_index_only)
    /// @param i      Relin-key index (parallel to idx.galois_elts[i])
    /// @param bufs   [out] vector of nk device pointers, each key_component_bytes in size,
    ///               allocated by the caller. Not freed on failure.
    /// @return true on success
    static bool Read_one_relin_key(const GaloisKeyIndex& idx, size_t i,
                                   /*out*/ std::vector<uint64_t*>& bufs, rtio::IoPolicy io_policy);

private:
    /// Ensure directory exists (no error if already exists).
    static void ensure_dir(const std::string& path) { mkdir(path.c_str(), 0755); }
};

}  // namespace fhert

#endif  // FHERT_KEY_SERIALIZER_H
