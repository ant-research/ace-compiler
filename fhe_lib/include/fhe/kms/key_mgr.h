//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_mgr.h
//! Templated key manager for FHE runtime backends.
//!
//! KeyMgr<VendorTraits> manages key lifecycle: generation, KMS
//! save/load, encode-only mode. IO is configured via IoPolicy, not
//! IoPolicy — business code describes the scenario, IO layer decides
//! the transport.

#ifndef FHERT_KEY_MGR_H
#define FHERT_KEY_MGR_H

#include "fhe/fhe_config.h"
#include "fhe/io/io_policy.h"
#include "fhe/kms/key_serializer.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace fhert
{

// KeyMode is defined in fhe/fhe_config.h (included above).

//=============================================================================
// KeyMgr<VendorTraits>
//=============================================================================

template <typename Traits> class KeyMgr
{
public:
    using SecretKey = typename Traits::SecretKey;
    using PublicKey = typename Traits::PublicKey;
    using RelinKey = typename Traits::RelinKey;
    using GaloisKey = typename Traits::GaloisKey;
    using ContextT = typename Traits::Context;
    using KeySerializerT = KeySerializer<Traits>;

    explicit KeyMgr() = default;
    ~KeyMgr() { release_context(); }

    // Disable copy and move
    KeyMgr(const KeyMgr&) = delete;
    KeyMgr& operator=(const KeyMgr&) = delete;
    KeyMgr(KeyMgr&&) = delete;
    KeyMgr& operator=(KeyMgr&&) = delete;

    /// Normal init: use externally-owned context, generate/load keys.
    /// The context must outlive this KeyMgr.
    void Init(ContextT& ctx, bool with_bootstrap);

    /// Set keys directly (bootstrap path).
    void set_keys(std::unique_ptr<SecretKey> sk, std::unique_ptr<PublicKey> pk, std::unique_ptr<RelinKey> rlk,
                  std::unique_ptr<GaloisKey> rtk);

    /// Generate dummy SK/PK for compute-only evaluator construction.
    /// In PRELOAD mode (eval-only), RLK and GK are loaded from disk but
    /// SK/PK are needed by CKKSEvaluator. This generates throwaway SK/PK
    /// that should never be used for real encryption/decryption.
    void generate_dummy_keys_for_evaluator();

    /// Set non-owning context pointer (for bootstrap path where BtsMgr
    /// creates the context and FheContext transfers ownership later).
    void set_context(ContextT& ctx) { _ctx = &ctx; }

    /// Encode-only init: use externally-owned context, skip key generation.
    void Init_encode_only(ContextT& ctx, bool with_bootstrap);

    // Key accessors — return nullptr if key is not available (compute-only mode)
    const SecretKey* secret_key_ptr() const { return _sk.get(); }
    const PublicKey* public_key_ptr() const { return _pk.get(); }
    const RelinKey* relin_key_ptr() const { return _rlk.get(); }
    const GaloisKey* galois_key_ptr() const { return _rtk.get(); }

    // Key accessors — throw if key is not available
    const SecretKey& secret_key() const
    {
        if (!_sk) {
            throw std::runtime_error("[kms] Secret key not available (COMPUTE_ONLY or encode-only mode)");
        }
        return *_sk;
    }
    const PublicKey& public_key() const
    {
        if (!_pk) {
            throw std::runtime_error("[kms] Public key not available (COMPUTE_ONLY or encode-only mode)");
        }
        return *_pk;
    }
    const RelinKey& relin_key() const { return *_rlk; }
    const GaloisKey& galois_key() const { return *_rtk; }
    RelinKey& relin_key() { return *_rlk; }
    GaloisKey& galois_key() { return *_rtk; }

    // KMS — IO configured via IoPolicy
    bool Save_keys(const std::string& dir, const std::vector<uint32_t>& galois_elts = {},
                   rtio::IoPolicy io_policy = rtio::IoPolicy::auto_) const;
    bool Load_keys(const std::string& dir, rtio::IoPolicy io_policy = rtio::IoPolicy::auto_);
    bool keys_loaded_from_kms() const { return _keys_loaded; }
    const std::vector<uint32_t>& loaded_galois_elts() const { return _loaded_galois_elts; }

    /// Wall-clock seconds of the most recent galois-key load; -1.0 before any
    /// load or on failure. Set by Load_keys; surfaced through the unified
    /// Kms_get_galois_load_time_seconds() dlsym contract (fhe/kms/kms_runtime_api.h) on every backend.
    double galois_load_time_seconds() const { return _galois_load_time_seconds; }

    // IO policy — replaces old set_io_config(IoConfig)
    void set_io_policy(rtio::IoPolicy policy) { _io_policy = policy; }
    rtio::IoPolicy effective_io_policy() const { return _io_policy; }

    // Encode-only query
    bool is_encode_only() const { return _encode_only; }

    // Key mode
    KeyMode key_mode() const { return _key_mode; }
    void set_key_mode(KeyMode mode) { _key_mode = mode; }

    //-------------------------------------------------------------------------
    // On-demand galois-key loading (KeyMode::ON_DEMAND)
    //
    // The galois key object (_rtk) is an empty shell at Init time: its element
    // set is populated up front, but no key bytes are resident in HBM until an
    // op needs them. Ensure_galois_key(step) lazily fetches the one relin key
    // the vendor will dereference for this rotation step, populating the slot
    // at the same index the vendor's evaluator computes (galois_elts[i] <->
    // relin_keys_[i]). Stage 1 does not evict: once loaded, a key stays
    // resident. Relin key (_rlk) is always fully preloaded (small, used
    // throughout).
    //
    // Backend-specific (phantom/ace specializations live in key_mgr.cu).
    //-------------------------------------------------------------------------
    bool Ensure_galois_key(int step);
    bool Ensure_bootstrap_keys();
    bool on_demand_ready() const { return _on_demand_ready; }

    /// Build an on-demand (empty-shell galois) key bundle from `key_dir`,
    /// honoring the current key_mode: ON_DEMAND preloads relin only,
    /// ON_DEMAND_FULL preloads SK/PK/RLK. Both defer galois-key bytes to
    /// Ensure_galois_key. Used by Init (ONLINE→ON_DEMAND switch not supported;
    /// here for the create_session path) and create_session so extra sessions
    /// ALSO go on-demand (create_session historically called Load_keys, which
    /// ignored key_mode and always full-loaded). No-op effect (returns false)
    /// if key_mode is not ON_DEMAND(_FULL) — caller should use Load_keys then.
    /// Backend-specific (phantom specialization in key_mgr.cu).
    bool Prepare_on_demand_keys(const std::string& key_dir);

    /// Evict (free) the galois-key slot this rotation step dereferences, so its
    /// HBM is reclaimed before the next key is loaded. Pairs with Ensure_galois_key:
    /// codegen emits Load(step) before a rotate (last-use analysis in fhe-cmplr
    /// stage-3) and Free_galois_key(step) after the rotate's last use, so the
    /// resident key-set tracks the program's live set instead of growing to
    /// full-set. No-op unless on-demand; no-op if the slot is already empty /
    /// not present. Step is resolved the same way as Ensure_galois_key (elt +
    /// NAF components), so a composite step's primitive keys are all evicted.
    /// Backend-specific (phantom specialization in key_mgr.cu).
    bool Evict_galois_key(int step);

    /// Release ALL resident galois-key slots (free every loaded key's HBM via
    /// cudaFreeAsync, mark all _slot_present=false). Idempotent — no-op for slots
    /// already empty. Called by release_session (user done, reclaim HBM) and as
    /// a destructor safety-net. The galois-key index and empty-shell slots stay
    /// intact, so a subsequent Ensure_galois_key re-loads from disk on demand.
    /// No-op unless on-demand mode.
    void Release_all_keys();

private:
    // Ensure ONE galois-key slot (by index i) is HBM-resident: read its bytes
    // from disk and populate the lazy slot if missing. Idempotent. Used by
    // Ensure_galois_key (direct-elt path + NAF component prefetch). Backend
    // specializations live in key_mgr.cu.
    void ensure_one_slot(size_t i, uint32_t elt, int step);

    // Evict ONE resident slot: move-assign an empty PhantomRelinKey into
    // _rtk->relin_keys_[i] (the old key's cudaAutoPtr destructs, cudaFreeAsync
    // its HBM) and mark _slot_present[i]=false. Idempotent. Vendor evaluates
    // get_relin_keys(idx) inline each call (no cached raw ptr), so an emptied
    // slot is safe until Ensure re-fills it. Backend-specific.
    void evict_one_slot(size_t i);

public:
    /// Can this context encrypt/decrypt? True when SK/PK are available
    /// (ONLINE, PRELOAD_FULL, ON_DEMAND_FULL). False when only eval keys
    /// are loaded (PRELOAD, ON_DEMAND) — the production compute-only mode.
    bool can_verify() const
    {
        return _key_mode == KeyMode::ONLINE || _key_mode == KeyMode::PRELOAD_FULL ||
               _key_mode == KeyMode::ON_DEMAND_FULL;
    }

    /// Release context GPU resources without destroying the KeyMgr.
    void release_context() { _ctx = nullptr; }

    /// Release encoder — placeholder.
    void reset_encoder()
    { /* no-op for now */
    }

private:
    ContextT* _ctx = nullptr;  ///< Non-owning; owned by FheContext
    std::unique_ptr<SecretKey> _sk;
    std::unique_ptr<PublicKey> _pk;
    std::unique_ptr<RelinKey> _rlk;
    std::unique_ptr<GaloisKey> _rtk;

    bool _encode_only = false;
    bool _keys_loaded = false;
    KeyMode _key_mode = KeyMode::ONLINE;
    std::vector<uint32_t> _loaded_galois_elts;

    rtio::IoPolicy _io_policy = rtio::IoPolicy::auto_;

    // On-demand galois-key state. _gk_index holds the parsed file layout (read
    // at Init, key bytes deferred). _slot_present[i] mirrors whether
    // _rtk->relin_keys_[i] has been populated; both parallel _loaded_galois_elts.
    bool _on_demand_ready = false;
    GaloisKeyIndex _gk_index{};
    std::vector<bool> _slot_present{};

    /// Set by Load_keys to the galois-key load wall-clock time; -1.0 before
    /// any load. See galois_load_time_seconds().
    double _galois_load_time_seconds = -1.0;
};

}  // namespace fhert

#endif  // FHERT_KEY_MGR_H
