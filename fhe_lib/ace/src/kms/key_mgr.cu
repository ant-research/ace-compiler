//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief key_mgr.cu
//! KeyMgr<VendorTraits> explicit instantiation.
//!
//! Manages key lifecycle for the ACE backend: HostContext creation, key
//! generation, KMS save/load. Uses VendorTraits for vendor-specific
//! context/key creation and KeySerializer<VendorTraits> for KMS.

#include "ace/he_engine/host/ckks.h"
#include "ace/he_engine/host/keygenerator.h"
#include "common/common.h"
#include "common/error.h"
#include "common/rt_api.h"
#include "fhe/kms/key_mgr.h"
#include "fhe/need_bts.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/ace/ace_config.h"
#include "fhe/vendor/ace/vendor_traits.h"

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
bool KeyMgr<ace::VendorTraits>::Save_keys(const std::string& dir, const std::vector<uint32_t>& galois_elts,
                                          rtio::IoPolicy io_policy) const;

template <> bool KeyMgr<ace::VendorTraits>::Load_keys(const std::string& dir, rtio::IoPolicy io_policy);

template <> void KeyMgr<ace::VendorTraits>::Init_encode_only(ContextT& ctx, bool with_bootstrap);

template <> void KeyMgr<ace::VendorTraits>::generate_dummy_keys_for_evaluator();

//=============================================================================
// KeyMgr<VendorTraits> — Init (mode-aware)
//=============================================================================

template <> void KeyMgr<ace::VendorTraits>::Init(ContextT& ctx, bool with_bootstrap)
{
    using Traits = ace::VendorTraits;

    // Store non-owning pointer to the externally-owned context.
    _ctx = &ctx;

    spdlog::info("[kms] HostContext received (N={})", (unsigned)Traits::poly_degree(*_ctx));

    // Read key mode from C global state
    _key_mode = static_cast<KeyMode>(Get_key_mode());

    switch (_key_mode) {
    case KeyMode::ONLINE: {
        // Default: generate all keys on CPU/host
        spdlog::info("[kms] KeyMode::ONLINE — generating keys (sk, pk, rlk)...");
        Traits::generate_keys(*_ctx, _sk, _pk, _rlk, _rtk);
        spdlog::info("[kms] Keys generated.");
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

        // Load only evaluation keys (RLK + GK) — compute-only, no SK/PK
        spdlog::info("[kms] KeyMode::PRELOAD — loading eval keys (RLK+GK) from {}", key_path);

        _rlk = std::make_unique<RelinKey>();
        _rtk = std::make_unique<GaloisKey>();

        bool loaded = KeySerializerT::Load_eval_keys(std::string(key_path), *_rlk, *_rtk, *_ctx, _loaded_galois_elts,
                                                     effective_io_policy());

        if (loaded) {
            spdlog::info("[kms] Successfully loaded eval keys from {}", key_path);
            _keys_loaded = true;
            // _sk and _pk remain nullptr
        }
        else {
            throw std::runtime_error(std::string("[kms] Failed to load eval keys from ") + key_path +
                                     " in PRELOAD mode");
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

        if (Load_keys(std::string(key_path), effective_io_policy())) {
            spdlog::info("[kms] Successfully loaded keys from {}", key_path);
            _keys_loaded = true;

            if (with_bootstrap) {
                // TODO: sparse wrapping handling
            }
        }
        else {
            throw std::runtime_error(std::string("[kms] Failed to load keys from ") + key_path +
                                     " in PRELOAD_FULL mode");
        }
        break;
    }

    case KeyMode::ON_DEMAND: {
        // On-demand key loading at runtime (reserved for future use)
        throw std::runtime_error("[kms] KeyMode::ON_DEMAND is not yet implemented");
    }
    }
}

//=============================================================================
// KeyMgr<VendorTraits> — generate_dummy_keys_for_evaluator
//=============================================================================

template <> void KeyMgr<ace::VendorTraits>::generate_dummy_keys_for_evaluator()
{
    using Traits = ace::VendorTraits;
    if (_sk && _pk) {
        // Already have keys — nothing to do
        return;
    }
    spdlog::info("[compute-only] Generating dummy SK/PK for evaluator construction");
    auto dummy_rlk = std::make_unique<RelinKey>();
    auto dummy_rtk = std::make_unique<GaloisKey>();
    Traits::generate_keys(*_ctx, _sk, _pk, dummy_rlk, dummy_rtk);
    // dummy_rlk and dummy_rtk go out of scope — we keep _sk and _pk as dummies.
}

//=============================================================================
// KeyMgr<VendorTraits> — set_keys
//=============================================================================

template <>
void KeyMgr<ace::VendorTraits>::set_keys(std::unique_ptr<SecretKey> sk, std::unique_ptr<PublicKey> pk,
                                         std::unique_ptr<RelinKey> rlk, std::unique_ptr<GaloisKey> rtk)
{
    _sk = std::move(sk);
    _pk = std::move(pk);
    _rlk = std::move(rlk);
    _rtk = std::move(rtk);
}

//=============================================================================
// KeyMgr<VendorTraits> — Init_encode_only
//=============================================================================

template <> void KeyMgr<ace::VendorTraits>::Init_encode_only(ContextT& ctx, bool with_bootstrap)
{
    using Traits = ace::VendorTraits;

    // Store non-owning pointer to the externally-owned context.
    _ctx = &ctx;

    spdlog::info("[encode-only] HostContext received (N={}) — no key generation", (unsigned)Traits::poly_degree(*_ctx));

    // _sk, _pk, _rlk, _rtk remain nullptr — no key generation needed for encoding.
    _encode_only = true;
}

//=============================================================================
// KeyMgr<VendorTraits> — Save_keys / Load_keys
//=============================================================================

template <>
bool KeyMgr<ace::VendorTraits>::Save_keys(const std::string& dir, const std::vector<uint32_t>& galois_elts,
                                          rtio::IoPolicy io_policy) const
{

    if (!_sk) {
        throw std::runtime_error("[KMS] Cannot save keys: secret key not generated");
    }

    return KeySerializerT::Save_keys(dir, *_sk, *_pk, *_rlk, *_rtk, *_ctx, galois_elts, io_policy);
}

template <> bool KeyMgr<ace::VendorTraits>::Load_keys(const std::string& dir, rtio::IoPolicy io_policy)
{

    // Create fresh key objects for population (ACE requires HostContext for load)
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
// KeyMgr<ace::VendorTraits> — on-demand stubs
//-----------------------------------------------------------------------------
// Stage-1 on-demand galois-key loading is implemented for the phantom backend.
// ACE backend keeps full preload (ON_DEMAND still throws in Init above). These
// no-op Ensure_* specializations exist so FheContext<ace>::Rotate (inline in
// context.h, which calls key_mgr().Ensure_galois_key()) links. They return
// true unconditionally: in ACE the full key set is always resident, so there
// is nothing to lazily fetch. When ACE on-demand is implemented, replace these
// with real lazy-slot logic mirroring the phantom specializations.
//=============================================================================

template <> bool KeyMgr<ace::VendorTraits>::Ensure_galois_key(int /*step*/)
{
    return true;
}

template <> bool KeyMgr<ace::VendorTraits>::Ensure_bootstrap_keys()
{
    return true;
}

template <> void KeyMgr<ace::VendorTraits>::ensure_one_slot(size_t /*i*/, uint32_t /*elt*/, int /*step*/)
{
    // No-op: ACE keeps full preload; nothing to lazily fetch.
}

template <> bool KeyMgr<ace::VendorTraits>::Prepare_on_demand_keys(const std::string& /*key_dir*/)
{
    // ACE has no on-demand impl yet — return false so create_session falls
    // back to Load_keys (full preload), preserving existing ACE behavior.
    return false;
}

template <> bool KeyMgr<ace::VendorTraits>::Evict_galois_key(int /*step*/)
{
    // No-op: ACE keeps full preload; nothing to evict.
    return true;
}

template <> void KeyMgr<ace::VendorTraits>::evict_one_slot(size_t /*i*/) {}

}  // namespace fhert
