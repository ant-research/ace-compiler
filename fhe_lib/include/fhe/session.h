//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief session.h
//! Per-user FHE session — owns key material and per-user cryptographic state.
//!
//! FheContext owns shared compute resources (Evaluator, EncMgr, BtsMgr,
//! crypto context).  FheSession owns the per-user cryptographic state
//! (KeyMgr with SK/PK/RLK/GK, Crypto with Encryptor/Decryptor).
//!
//! In the current single-user architecture, FheContext creates one
//! FheSession internally.  For multi-user inference, additional sessions
//! can be created and bound to the same shared FheContext via
//! switch_session().
//!
//! KeyMgr is held via unique_ptr to break the header dependency chain:
//! session.h only forward-declares KeyMgr, so including session.h does
//! not transitively include key_mgr.h -> key_serializer.h -> spdlog.

#ifndef FHERT_SESSION_H
#define FHERT_SESSION_H

#include "fhe/fhe_config.h"

#include <memory>

namespace fhert
{

// Forward declaration — breaks the include chain:
//   session.h -/-> key_mgr.h -> key_serializer.h -> spdlog
// The full definition is available when key_mgr.h is included separately
// (as it is in context.cu and other implementation files).
template <typename Traits> class KeyMgr;

//=============================================================================
// FheSession<Traits>
//=============================================================================

template <typename Traits> class FheSession
{
public:
    using Crypto = typename Traits::Crypto;
    using KeyMgrType = KeyMgr<Traits>;
    using ContextT = typename Traits::Context;

    /// Construct a session bound to an externally-owned crypto context.
    /// The context must outlive this session.
    /// This is the preferred constructor — the session is immediately usable
    /// after KeyMgr::Init() is called (typically by FheContext).
    explicit FheSession(ContextT* ctx, KeyMode key_mode) : _key_mgr(std::make_unique<KeyMgrType>())
    {
        if (ctx) {
            _key_mgr->set_context(*ctx);
        }
        _key_mgr->set_key_mode(key_mode);
    }

    /// Default-construct an empty session.
    /// Provided for two-phase initialization where the crypto context is
    /// not yet available at construction time (e.g., FheContext bootstrap
    /// path).  Prefer the explicit constructor when the context is known.
    FheSession() : _key_mgr(std::make_unique<KeyMgrType>()) {}

    ~FheSession() = default;

    FheSession(const FheSession&) = delete;
    FheSession& operator=(const FheSession&) = delete;
    FheSession(FheSession&&) = default;
    FheSession& operator=(FheSession&&) = default;

    // --- Key manager (owned by this session) ---
    KeyMgrType& key_mgr() { return *_key_mgr; }
    const KeyMgrType& key_mgr() const { return *_key_mgr; }

    // --- Crypto: encrypt/decrypt (owned by this session) ---
    Crypto& crypto() { return _crypto; }
    const Crypto& crypto() const { return _crypto; }

private:
    std::unique_ptr<KeyMgrType> _key_mgr;  ///< Per-session key material (indirect for header isolation)
    Crypto _crypto;                        ///< Per-session encrypt/decrypt (keyed ops)
};

}  // namespace fhert

#endif  // FHERT_SESSION_H
