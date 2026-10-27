//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_mgr.cpp
//! KeyMgr<VendorTraits> explicit instantiation.
//!
//! Manages key lifecycle: context creation, key generation, KMS save/load.
//! Uses VendorTraits for vendor-specific context/key creation and
//! KeySerializer<VendorTraits> for KMS operations.

#include "common/common.h"
#include "common/rt_api.h"
#include "fhe/io/cuda_check.h"
#include "fhe/kms/key_mgr.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/phantom/vendor_traits.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vector>

namespace fhert
{

//=============================================================================
// FORWARD DECLARATIONS — must precede first use
//=============================================================================

template <>
bool KeyMgr<phantom::VendorTraits>::Save_keys(const std::string& dir, const std::vector<uint32_t>& galois_elts,
                                              rtio::IoPolicy io_policy) const;

template <> bool KeyMgr<phantom::VendorTraits>::Load_keys(const std::string& dir, rtio::IoPolicy io_policy);

template <> void KeyMgr<phantom::VendorTraits>::Init_encode_only(ContextT& ctx, bool with_bootstrap);

template <> void KeyMgr<phantom::VendorTraits>::generate_dummy_keys_for_evaluator();

// On-demand Ensure_* specializations — forward-declared so Ensure_galois_key
// (which calls Ensure_bootstrap_keys on the full-set-prefetch fallback) can
// reference the specialization before its definition below.
template <> bool KeyMgr<phantom::VendorTraits>::Ensure_galois_key(int step);
template <> bool KeyMgr<phantom::VendorTraits>::Ensure_bootstrap_keys();
template <> void KeyMgr<phantom::VendorTraits>::ensure_one_slot(size_t i, uint32_t elt, int step);
template <> bool KeyMgr<phantom::VendorTraits>::Prepare_on_demand_keys(const std::string& key_dir);
template <> bool KeyMgr<phantom::VendorTraits>::Evict_galois_key(int step);
template <> void KeyMgr<phantom::VendorTraits>::evict_one_slot(size_t i);

//=============================================================================
// KeyMgr<VendorTraits> — Key accessors with validation
//=============================================================================
// KeyMgr<VendorTraits> — Init (mode-aware)
//=============================================================================

template <> void KeyMgr<phantom::VendorTraits>::Init(ContextT& ctx, bool with_bootstrap)
{
    using Traits = phantom::VendorTraits;

    // Store non-owning pointer to the externally-owned context.
    _ctx = &ctx;

    spdlog::info("[kms] PhantomContext received (N={})", (unsigned)Traits::poly_degree(*_ctx));

    // Read key mode from C global state (set by Set_key_mode)
    _key_mode = static_cast<KeyMode>(Get_key_mode());

    switch (_key_mode) {
    case KeyMode::ONLINE: {
        // Default: generate all keys on GPU
        spdlog::info("[kms] KeyMode::ONLINE — generating keys (sk, pk, rlk, rtk)...");
        auto t4 = std::chrono::high_resolution_clock::now();
        Traits::generate_keys(*_ctx, _sk, _pk, _rlk, _rtk);
        auto t5 = std::chrono::high_resolution_clock::now();
        double s_gen_keys = std::chrono::duration<double>(t5 - t4).count();

        spdlog::info("[kms] Keys generated [gen_keys={:.2f}s]", s_gen_keys);
        spdlog::debug("[kms] key handles: sk={} pk={} rlk={} rtk={}", (void*)_sk.get(), (void*)_pk.get(),
                      (void*)_rlk.get(), (void*)_rtk.get());
        break;
    }

    case KeyMode::PRELOAD: {
        // Load only eval keys (RLK + GK) from disk — compute-only, no SK/PK.
        // This is the production default.
        const char* key_path = Get_key_path();
        if (!key_path || key_path[0] == '\0') {
            throw std::runtime_error("[kms] PRELOAD mode requires key_path to be set");
        }

        spdlog::info("[kms] KeyMode::PRELOAD — loading eval keys (RLK+GK) from {}", key_path);

        _rlk = std::make_unique<RelinKey>();
        _rtk = std::make_unique<GaloisKey>();

        {
            auto t2 = std::chrono::high_resolution_clock::now();
            bool loaded = KeySerializerT::Load_eval_keys(std::string(key_path), *_rlk, *_rtk, *_ctx,
                                                         _loaded_galois_elts, effective_io_policy());
            auto t3 = std::chrono::high_resolution_clock::now();
            double s_load_keys = std::chrono::duration<double>(t3 - t2).count();

            if (loaded) {
                spdlog::info("[kms] Successfully loaded eval keys from {} [load_eval_keys={:.2f}s]", key_path,
                             s_load_keys);
                _keys_loaded = true;
                // _sk and _pk remain nullptr
            }
            else {
                throw std::runtime_error(std::string("[kms] Failed to load eval keys from ") + key_path +
                                         " in PRELOAD mode");
            }
        }
        break;
    }

    case KeyMode::PRELOAD_FULL: {
        // Load all keys (SK, PK, RLK, GK) — full pipeline, can encrypt/decrypt.
        // Used for debug/verification (e2e testing).
        const char* key_path = Get_key_path();
        if (!key_path || key_path[0] == '\0') {
            throw std::runtime_error("[kms] PRELOAD_FULL mode requires key_path to be set");
        }

        spdlog::info("[kms] KeyMode::PRELOAD_FULL — loading all keys from {}", key_path);

        {
            auto t2 = std::chrono::high_resolution_clock::now();
            bool loaded = Load_keys(std::string(key_path), effective_io_policy());
            auto t3 = std::chrono::high_resolution_clock::now();
            double s_load_keys = std::chrono::duration<double>(t3 - t2).count();

            if (loaded) {
                spdlog::info("[kms] Successfully loaded keys from {} [load_keys={:.2f}s]", key_path, s_load_keys);
                _keys_loaded = true;
            }
            else {
                throw std::runtime_error(std::string("[kms] Failed to load keys from ") + key_path +
                                         " in PRELOAD_FULL mode");
            }
        }
        break;
    }

    case KeyMode::ON_DEMAND: {
        // On-demand galois-key loading: preload relin (small, used throughout),
        // read ONLY the galois-key index (no key bytes), and set up an empty-
        // shell galois key whose slots are filled lazily by Ensure_galois_key.
        const char* key_path = Get_key_path();
        if (!key_path || key_path[0] == '\0') {
            throw std::runtime_error("[kms] ON_DEMAND mode requires key_path to be set");
        }
        if (!Prepare_on_demand_keys(std::string(key_path))) {
            throw std::runtime_error(std::string("[kms] ON_DEMAND prepare failed from ") + key_path);
        }
        break;
    }

    case KeyMode::ON_DEMAND_FULL: {
        // Full-pipeline on-demand: SK+PK+RLK preloaded (for encrypt/decrypt),
        // galois keys deferred to Ensure_galois_key. Used for end-to-end
        // verification of the on-demand path via FHERuntime.run().
        const char* key_path = Get_key_path();
        if (!key_path || key_path[0] == '\0') {
            throw std::runtime_error("[kms] ON_DEMAND_FULL mode requires key_path to be set");
        }
        if (!Prepare_on_demand_keys(std::string(key_path))) {
            throw std::runtime_error(std::string("[kms] ON_DEMAND_FULL prepare failed from ") + key_path);
        }
        break;
    }
    }
}

//=============================================================================
// KeyMgr<VendorTraits> — Prepare_on_demand_keys
//-----------------------------------------------------------------------------
// Build an on-demand (empty-shell galois) key bundle from key_dir, honoring
// the current key_mode. ON_DEMAND preloads relin only; ON_DEMAND_FULL also
// preloads SK/PK. Both defer galois-key bytes (read index only + pre-size
// empty slots filled lazily by Ensure_galois_key). Returns false if the
// current mode is not ON_DEMAND(_FULL) — caller should fall back to Load_keys.
// Called by Init AND by create_session (the latter previously called
// Load_keys which ignored key_mode, so extra sessions always full-loaded —
// this helper fixes that so multi-user create_session also goes on-demand).
//=============================================================================
template <> bool KeyMgr<phantom::VendorTraits>::Prepare_on_demand_keys(const std::string& key_dir)
{
    using Traits = phantom::VendorTraits;
    const bool full = (_key_mode == KeyMode::ON_DEMAND_FULL);
    if (!full && _key_mode != KeyMode::ON_DEMAND) {
        return false;  // not an on-demand mode; caller should use Load_keys
    }

    spdlog::info("[kms] Prepare_on_demand_keys (mode={}): {} galois deferred, {} preloaded",
                 full ? "ON_DEMAND_FULL" : "ON_DEMAND", full ? "SK/PK/RLK" : "relin");

    if (full) {
        _sk = std::make_unique<SecretKey>();
        _pk = std::make_unique<PublicKey>();
        _rlk = std::make_unique<RelinKey>();
        if (!KeySerializerT::Load_sk_pk_relin_only(key_dir, *_sk, *_pk, *_rlk, *_ctx, effective_io_policy())) {
            throw std::runtime_error("[kms] Prepare_on_demand_keys: failed to load SK/PK/RLK from " + key_dir);
        }
    }
    else {
        _rlk = std::make_unique<RelinKey>();
        if (!KeySerializerT::Load_relin_key_only(key_dir, *_rlk, *_ctx, effective_io_policy())) {
            throw std::runtime_error("[kms] Prepare_on_demand_keys: failed to load relin key from " + key_dir);
        }
    }

    _rtk = std::make_unique<GaloisKey>();
    if (!KeySerializerT::Load_galois_index_only(key_dir, _gk_index, effective_io_policy())) {
        throw std::runtime_error("[kms] Prepare_on_demand_keys: failed to read galois index from " + key_dir);
    }

    _loaded_galois_elts = _gk_index.galois_elts;
    Traits::rebuild_galois_tool(*_ctx, _loaded_galois_elts);

    std::array<uint64_t, 4> parms_id{};
    std::memcpy(parms_id.data(), &_gk_index.parms_id, sizeof(uint64_t) * 4);
    Traits::prealloc_galois_slots(*_rtk, _gk_index.relin_keys.size(), parms_id);
    _slot_present.assign(_gk_index.relin_keys.size(), false);

    _keys_loaded = true;
    _on_demand_ready = true;
    spdlog::info("[kms] ON_DEMAND{} ready: {} galois slots ({} elts) deferred, {} preloaded (from {})",
                 full ? "_FULL" : "", _gk_index.relin_keys.size(), _loaded_galois_elts.size(),
                 full ? "SK/PK/RLK" : "relin", key_dir);
    return true;
}

template <> void KeyMgr<phantom::VendorTraits>::generate_dummy_keys_for_evaluator()
{
    using Traits = phantom::VendorTraits;
    if (_sk && _pk) {
        // Already have keys — nothing to do
        return;
    }
    spdlog::info("[compute-only] Generating dummy SK/PK for CKKSEvaluator construction");
    // generate_keys creates all 4 keys; we only keep SK and PK (as dummies)
    // The RLK and GK generated here are temporary and will be discarded
    // (the real RLK/GK were already loaded from disk).
    auto dummy_rlk = std::make_unique<RelinKey>();
    auto dummy_rtk = std::make_unique<GaloisKey>();
    Traits::generate_keys(*_ctx, _sk, _pk, dummy_rlk, dummy_rtk);
    // dummy_rlk and dummy_rtk go out of scope here — we keep _sk and _pk
    // as dummy keys for CKKSEvaluator. The real RLK/GK are in _rlk and _rtk.
}

//=============================================================================
// KeyMgr<VendorTraits> — Init_encode_only
//=============================================================================

template <> void KeyMgr<phantom::VendorTraits>::Init_encode_only(ContextT& ctx, bool with_bootstrap)
{
    using Traits = phantom::VendorTraits;

    // Store non-owning pointer to the externally-owned context.
    _ctx = &ctx;

    spdlog::info("[encode-only] PhantomContext received (N={}) — no key generation",
                 (unsigned)Traits::poly_degree(*_ctx));

    // _sk, _pk, _rlk, _rtk remain nullptr — no key generation needed for encoding.
    _encode_only = true;
}

//=============================================================================
// KeyMgr<VendorTraits> — Save_keys / Load_keys
//=============================================================================

template <>
bool KeyMgr<phantom::VendorTraits>::Save_keys(const std::string& dir, const std::vector<uint32_t>& galois_elts,
                                              rtio::IoPolicy io_policy) const
{

    if (!_sk || !_sk->is_generated()) {
        throw std::runtime_error("[KMS] Cannot save keys: secret key not generated");
    }

    return KeySerializerT::Save_keys(dir, *_sk, *_pk, *_rlk, *_rtk, *_ctx, galois_elts, io_policy);
}

template <> bool KeyMgr<phantom::VendorTraits>::Load_keys(const std::string& dir, rtio::IoPolicy io_policy)
{
    // Create fresh key objects for population
    _sk = std::make_unique<SecretKey>();
    _pk = std::make_unique<PublicKey>();
    _rlk = std::make_unique<RelinKey>();
    _rtk = std::make_unique<GaloisKey>();

    bool ok = KeySerializerT::Load_keys(dir, *_sk, *_pk, *_rlk, *_rtk, *_ctx, _loaded_galois_elts, io_policy);

    // Fold the galois-key load timing (written by KeySerializer into its static
    // member) into this KeyMgr's instance field, surfaced via the unified
    // Kms_get_galois_load_time_seconds() export. Reset on failure.
    _galois_load_time_seconds = ok ? KeySerializerT::g_galois_load_time_seconds : -1.0;

    if (!ok) {
        _sk.reset();
        _pk.reset();
        _rlk.reset();
        _rtk.reset();
    }
    return ok;
}

//=============================================================================
// KeyMgr<phantom::VendorTraits> — on-demand galois-key lazily-loaded slots
//=============================================================================

template <> bool KeyMgr<phantom::VendorTraits>::Ensure_galois_key(int step)
{
    using Traits = phantom::VendorTraits;
    // Only the ON_DEMAND path uses lazy slots; in any other mode the full key
    // set is already resident and this is a no-op.
    if (!_on_demand_ready || !_rtk) {
        return true;
    }

    // Normalize negative step the same way FheContext::Rotate does. We rely on
    // step being normalized at the Rotate call site already, but be defensive.
    if (step < 0) {
        step = static_cast<int>(Traits::poly_degree(*_ctx) / 2) + step;
    }

    const uint32_t elt = Traits::galois_elt_from_step(*_ctx, step);

    // Stage-2: the requested elt may be a "composite" galois element Phantom
    // never loads directly — large/odd rotation steps are served via NAF
    // decomposition (evaluate.cu ~826): step -> naf(step) = sum ±2^i, each
    // component temp_step = ±2^i rotated recursively and consuming the key for
    // elt = get_elt_from_step(temp_step) (= 5^(±2^i)). So instead of prefetching
    // the full set (the stage-1 fallback), prefetch only the NAF component
    // elts the vendor will actually dereference — typically log(|step|) keys,
    // not all 67. This is the per-key lazy load: working-set residency, not
    // full-set.
    const auto& elts = _loaded_galois_elts;
    auto it = std::find(elts.begin(), elts.end(), elt);
    if (it != elts.end()) {
        // Directly loaded: ensure just this slot.
        size_t i = static_cast<size_t>(std::distance(elts.begin(), it));
        ensure_one_slot(i, elt, step);
        return true;
    }

    // Composite elt — prefetch its NAF component elts (each directly loaded).
    auto comp_elts = Traits::naf_component_elts(*_ctx, step);
    for (uint32_t ce : comp_elts) {
        auto ci = std::find(elts.begin(), elts.end(), ce);
        if (ci != elts.end()) {
            ensure_one_slot(static_cast<size_t>(std::distance(elts.begin(), ci)), ce, step);
        }
        // A component elt also missing from the set would be anomalous (NAF of
        // ±2^i maps to 5^(±2^i), all of which the saved set stores) — skip it
        // and let the vendor throw if it truly needs one we didn't load.
    }
    return true;
}

// Ensure a single galois-key slot is resident: if not yet loaded, read that
// one relin key's bytes from disk and populate it. Idempotent. Shared by the
// direct-elt path above and the NAF-component prefetch.
template <> void KeyMgr<phantom::VendorTraits>::ensure_one_slot(size_t i, uint32_t elt, int step)
{
    using Traits = phantom::VendorTraits;
    if (i >= _slot_present.size() || _slot_present[i]) {
        return;  // already resident (or out of range — let vendor throw)
    }
    const auto& entry = _gk_index.relin_keys[i];
    const uint64_t nk = entry.nk;
    const size_t kcb = static_cast<size_t>(_gk_index.key_component_bytes);

    std::vector<uint64_t*> bufs(nk, nullptr);
    for (uint64_t j = 0; j < nk; j++) {
        FHE_CHECK_CUDA(cudaMalloc(&bufs[j], kcb));
    }
    bool ok = KeySerializerT::Read_one_relin_key(_gk_index, i, bufs, effective_io_policy());
    if (!ok) {
        for (auto* b : bufs) {
            if (b)
                cudaFree(b);
        }
        throw std::runtime_error("[kms] Ensure_galois_key: Read_one_relin_key failed for slot " + std::to_string(i));
    }

    std::array<uint64_t, 4> parms_id{};
    std::memcpy(parms_id.data(), &_gk_index.parms_id, sizeof(uint64_t) * 4);
    const uint64_t N = _gk_index.poly_modulus_degree;
    const uint64_t L = _gk_index.coeff_modulus_size;
    Traits::populate_one_relin_key(*_rtk, i, bufs, parms_id, N, L);
    // bufs ownership transferred into the slot's cuda_auto_ptr; do NOT free.

    _slot_present[i] = true;
    spdlog::debug("[kms] Ensure_galois_key: loaded slot {} (elt={}, step={}): {} of {} resident", i, elt, step,
                  std::count(_slot_present.begin(), _slot_present.end(), true), _slot_present.size());
}

// Evict a single resident slot: clear relin_keys_[i] (cudaFreeAsync the HBM
// via PhantomRelinKey move-assign destructor) and mark absent. Idempotent.
template <> void KeyMgr<phantom::VendorTraits>::evict_one_slot(size_t i)
{
    using Traits = phantom::VendorTraits;
    if (i >= _slot_present.size() || !_slot_present[i] || !_rtk) {
        return;  // already empty / out of range / no key object
    }
    Traits::clear_galois_slot(*_rtk, i);
    _slot_present[i] = false;
    spdlog::debug("[kms] Evict_galois_key: freed slot {} : {} of {} resident", i,
                  std::count(_slot_present.begin(), _slot_present.end(), true), _slot_present.size());
}

// Release ALL resident galois-key slots — free every loaded key's HBM. Called
// by release_session (user done) and as a destructor safety-net. The index +
// empty-shell slots stay, so Ensure re-loads from disk on next use.
template <> void KeyMgr<phantom::VendorTraits>::Release_all_keys()
{
    if (!_on_demand_ready || !_rtk) {
        return;
    }
    size_t resident = std::count(_slot_present.begin(), _slot_present.end(), true);
    if (resident == 0) {
        return;  // nothing to free
    }
    for (size_t i = 0; i < _slot_present.size(); i++) {
        evict_one_slot(i);
    }
    spdlog::info("[kms] Release_all_keys: freed {} of {} galois slots", resident, _slot_present.size());
}

// Mirror of Ensure_galois_key: resolve step -> elt -> NAF components (the
// keys the vendor actually dereferences for this rotate) and evict those slots.
// Composite elts (not directly in the set) never get a resident slot themselves
// — only their NAF component primitives do — so evicting components is correct.
// Direct elts in the set: evict that slot. No-op unless on-demand.
template <> bool KeyMgr<phantom::VendorTraits>::Evict_galois_key(int step)
{
    using Traits = phantom::VendorTraits;
    if (!_on_demand_ready || !_rtk) {
        return true;  // PRELOAD/ONLINE: nothing to evict (full set, never freed)
    }
    if (step < 0) {
        step = static_cast<int>(Traits::poly_degree(*_ctx) / 2) + step;
    }
    const uint32_t elt = Traits::galois_elt_from_step(*_ctx, step);
    const auto& elts = _loaded_galois_elts;

    auto it = std::find(elts.begin(), elts.end(), elt);
    if (it != elts.end()) {
        evict_one_slot(static_cast<size_t>(std::distance(elts.begin(), it)));
        return true;
    }
    // Composite elt: evict its NAF component slots (the primitives the vendor
    // used for this rotate). NOTE: those primitives may be reused by a LATER
    // rotate, so fhe-cmplr's liveness analysis must emit Free only at TRUE
    // last-use, not after every rotate — emitting naively here would free a
    // key the next rotate still needs. This runtime method trusts the caller
    // (codegen) has placed it at the last-use site.
    auto comp_elts = Traits::naf_component_elts(*_ctx, step);
    for (uint32_t ce : comp_elts) {
        auto ci = std::find(elts.begin(), elts.end(), ce);
        if (ci != elts.end()) {
            evict_one_slot(static_cast<size_t>(std::distance(elts.begin(), ci)));
        }
    }
    return true;
}

template <> bool KeyMgr<phantom::VendorTraits>::Ensure_bootstrap_keys()
{
    if (!_on_demand_ready) {
        return true;
    }
    // Stage-1 conservative pre-fetch: bootstrap's internal rotations are opaque
    // at this layer, so ensure every galois element is resident before entering
    // bootstrap. Reuses the per-slot ensure_one_slot so the load path is shared
    // with Ensure_galois_key. (Stage-2 could narrow this once the phantom
    // bootstrap key-requirement set is enumerated — NAF-aware like Rotate.)
    const auto& elts = _loaded_galois_elts;
    for (size_t i = 0; i < _gk_index.relin_keys.size() && i < elts.size(); i++) {
        ensure_one_slot(i, elts[i], /*step*/ 0);
    }
    return true;
}

}  // namespace fhert
