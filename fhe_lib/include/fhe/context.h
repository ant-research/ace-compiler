//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief context.h
//! Unified FHE context template for runtime backends.
//!
//! FheContext<Backend> owns and orchestrates shared runtime components
//! (Evaluator, EncMgr, BtsMgr) and a primary FheSession that holds
//! per-user key material (KeyMgr with crypto context, SK/PK).
//!
//! Lifetime is managed by the caller via Create() + MakeCurrent().
//! The HE dispatch layer (rt_phantom.h / rt_ace.h) resolves the
//! context through thread-local Current(), NOT through a singleton.
//!
//! Resource initialization is controlled by two parameters:
//! - KeyMode:     how keys are obtained (ONLINE/PRELOAD/PRELOAD_FULL/ON_DEMAND).
//! - EncPolicy:   when weights are loaded/encoded (LOAD_RAW/LOAD_ENCODED/LOAD_ON_DEMAND).

#ifndef FHERT_CONTEXT_H
#define FHERT_CONTEXT_H

#include "fhe/enc/enc_mgr.h"
#include "fhe/fhe_config.h"
#include "fhe/kms/key_mgr.h"
#include "fhe/session.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Forward declaration for Io_get_input (defined in common/io_api.h)
extern "C" void* Io_get_input(const char* name, size_t idx);

namespace fhert
{

//=============================================================================
// FheContext<Backend>
//=============================================================================

template <typename Backend> class FheContext
{
public:
    // ── Backend value types (signature shortcuts, NOT owned by FheContext) ──
    using Ciphertext = typename Backend::Ciphertext;
    using Plaintext = typename Backend::Plaintext;
    using ScaleType = typename Backend::ScaleType;
    using LevelType = typename Backend::LevelType;

    // ── Lifecycle ──────────────────────────────────────

    /// Create a new FheContext with the given key mode and encoding policy.
    ///
    /// @param key_mode   How keys are obtained:
    ///                   - ONLINE:        generate all keys on GPU
    ///                   - PRELOAD:       load eval keys (RLK+GK) from disk
    ///                   - PRELOAD_FULL:  load all keys (SK+PK+RLK+GK) from disk
    ///                   - ON_DEMAND:       load eval keys on-demand (reserved)
    ///                   - ON_DEMAND_FULL:  load all keys on-demand (reserved)
    /// @param enc_policy When weights are loaded/encoded:
    ///                   - LOAD_RAW:       read file to memory, no encoding
    ///                   - LOAD_ENCODED:   read + encode raw weights at init
    ///                   - LOAD_ON_DEMAND: read per weight at inference (reserved)
    static std::unique_ptr<FheContext, void (*)(FheContext*)> Create(KeyMode key_mode, EncPolicy enc_policy)
    {
        return std::unique_ptr<FheContext, void (*)(FheContext*)>(new FheContext(key_mode, enc_policy),
                                                                  [](FheContext* p) { delete p; });
    }

    /// Destructor — RAII: member destruction order is reverse of declaration.
    /// _evaluator (unique_ptr auto-reset, ACE Evaluator clears eval_ptr/ctx),
    /// _session (KeyMgr auto release_context), then _crypto_ctx is freed.
    ~FheContext() = default;

    // Non-copyable, non-movable
    FheContext(const FheContext&) = delete;
    FheContext& operator=(const FheContext&) = delete;
    FheContext(FheContext&&) = delete;
    FheContext& operator=(FheContext&&) = delete;

    // ── Thread-local context management ────────────────
    // The TLS slot lives in the backend .so (VendorTraits::current_context_ptr/
    // set_current_context_ptr) and is NEVER read inline from a `static inline
    // thread_local` here. Reason: a shared-template-header inline thread_local
    // would be inlined into the compiled FHE kernel .so and, on a non-main
    // worker pthread, each DSO resolves to its own TLS instance (nullptr) ->
    // crash. The function-call indirection confines the TLS access to one DSO
    // (same pattern as ace get/set_device_context).

    /// Set this context as the calling thread's current context.
    void MakeCurrent() { Backend::set_current_context_ptr(this); }

    /// Get the calling thread's current context.
    static FheContext& Current()
    {
        void* p = Backend::current_context_ptr();
        IS_TRUE(p != nullptr, "FheContext::Current() called with no active context — "
                              "call MakeCurrent() first");
        return *static_cast<FheContext*>(p);
    }

    /// Clear the calling thread's current context pointer.
    static void ClearCurrent() { Backend::set_current_context_ptr(nullptr); }

    // ── Component accessors ─────────────────────────────

    /// Primary session — owns KeyMgr (crypto context + keys).
    FheSession<Backend>& session() { return _session; }

    /// Session by index: 0 = primary (_session), i>=1 = the i-th extra session.
    /// Used for per-session key/evaluator routing in multi-session (Mode B-P).
    FheSession<Backend>& session(int idx)
    {
        if (idx == 0) {
            return _session;
        }
        IS_TRUE(idx >= 1 && idx <= static_cast<int>(_extra_sessions.size()), "session idx out of range");
        return *_extra_sessions[idx - 1];
    }

    /// Active session — the session of the calling thread (Mode B-P: per-thread).
    /// Defaults to the primary session. Routed via the thread-local active idx.
    FheSession<Backend>& active_session() { return session(Backend::active_session_idx()); }

    /// Key manager (delegates to the calling thread's active session's KeyMgr).
    KeyMgr<Backend>& key_mgr() { return session(Backend::active_session_idx()).key_mgr(); }

    /// Direct access to the backend crypto context (PhantomContext / HostContext).
    /// Owned by FheContext, not by KeyMgr.
    auto* crypto_context() { return _crypto_ctx.get(); }

    /// Crypto: encrypt/decrypt operations (delegates to the calling thread's
    /// active session).
    typename Backend::Crypto& crypto() { return session(Backend::active_session_idx()).crypto(); }

    EncMgr<Backend>& enc_mgr() { return *_enc_mgr; }

    /// Evaluator for the calling thread's active session (Mode B-P: one
    /// evaluator per session, routed by thread-local active idx).
    typename Backend::Evaluator& evaluator() { return cur_evaluator(); }
    typename Backend::BtsMgr& bootstrap() { return _bootstrap; }

    /// Per-thread active session index (0 = primary). The TLS slot lives in the
    /// backend .so (VendorTraits::active_session_idx/set_active_session_idx)
    /// and is accessed via these — NOT via an inline thread_local here, to keep
    /// the compiled kernel .so from inlining a cross-DSO thread_local read.

    /// The calling thread's active session index.
    int active_session_idx() { return Backend::active_session_idx(); }

    /// The evaluator bound to the calling thread's active session (idx 0 =
    /// primary _evaluator, idx>=1 = _extra_evaluators[idx-1]).
    typename Backend::Evaluator& cur_evaluator()
    {
        const int idx = Backend::active_session_idx();
        IS_TRUE(idx >= 0 && idx <= static_cast<int>(_extra_evaluators.size()),
                "cur_evaluator: active session idx out of range");
        return (idx == 0) ? _evaluator : _extra_evaluators[idx - 1];
    }

    /// The bootstrap manager — shared across all sessions (Plan B).
    /// Bootstrapper's LT coefficients / ModularReducer are model-level (read-only,
    /// user-independent). Bootstrap workspace (babyct/giantct/...) are function-
    /// local (per-call stack) → no cross-thread race. The per-session key routing
    /// is handled by switching each Bootstrapper's `ckks` pointer to the calling
    /// thread's evaluator at FheContext::Bootstrap entry (context.cu).
    typename Backend::BtsMgr& cur_bootstrap() { return _bootstrap; }

    /// Number of sessions with evaluators (1 primary + N extra).
    int num_sessions() const { return 1 + static_cast<int>(_extra_evaluators.size()); }

    /// Set the calling thread's active session index (Mode B-P hot-switch on
    /// a worker thread). No key upload, no GPU work — each evaluator is
    /// already bound to its session's keys at create_session time.
    void set_active_session_idx_for_this_thread(int idx)
    {
        IS_TRUE(idx >= 0 && idx < num_sessions(), "set_active_session_idx_for_this_thread: idx out of range");
        Backend::set_active_session_idx(idx);
    }

    /// Activate session `idx` on the CALLING thread (Mode B-P worker-thread
    /// entry point). Sets: this context as thread-local Current(), the active
    /// session idx (evaluator + key routing), and backend per-thread state
    /// (phantom: tl_current_stream; ace: tl_device_context). Must be called on
    /// each worker thread before `_run_kernel()`. Backend-specialized in .cu.
    void Activate_session_for_this_thread(int idx);

    // ── Session switching ─────────────────────────────────

    /// Switch the active session: update the evaluator to use the new
    /// session's keys (relin_key, galois_key, encryptor, decryptor).
    /// The new session must have been initialized with the same crypto context.
    void switch_session(FheSession<Backend>& session);

    /// Create an additional session bound to this shared context, loading a
    /// full key bundle (SK/PK/RLK/GK) from `key_dir` via the explicit-dir
    /// KeyMgr::Load_keys (NOT the process-global key_path).  The session's
    /// galois elements MUST match the primary's (the shared GaloisTool is not
    /// rebuilt here).  Builds the session's OWN evaluator bound to its keys
    /// (Mode B-P: per-session evaluator, no shared-evaluator mutation).
    /// Returns the session index (>=1) or -1 on failure.
    int create_session(const std::string& key_dir);

    /// Switch by index: 0 = primary (_session), i>=1 = the i-th extra session
    /// created by create_session().  Sets the calling thread's active idx — a
    /// microsecond-scale operation (no key upload, no context rebuild).
    void switch_session_by_idx(int idx);

    /// Release all galois-key slots for session idx (free their HBM). The
    /// session structure (evaluator, GaloisTool, BtsMgr) stays intact; only
    /// the galois key bytes are evicted. A subsequent Ensure_galois_key
    /// re-loads from disk on demand. Called by the service layer when a user
    /// is done (not needed until next request). No-op unless on-demand.
    void release_session(int idx);

    // ── Configuration queries ──────────────────────────

    /// Key mode (delegates to calling thread's active session's KeyMgr).
    KeyMode key_mode() { return session(Backend::active_session_idx()).key_mgr().key_mode(); }
    EncPolicy enc_policy() const { return _enc_policy; }

    /// True when this context can perform end-to-end verification
    /// (encrypt inputs + decrypt outputs).  Requires SK/PK.
    /// Delegates to the calling thread's active session's KeyMgr.
    bool can_verify() { return session(Backend::active_session_idx()).key_mgr().can_verify(); }

    uint64_t scaling_mod_size() const { return _params.scaling_mod_size; }
    uint64_t slot_count() const { return _params.slot_count; }
    uint64_t num_prime_parts() const { return _params.num_prime_parts; }

    /// Access the full FHE parameter struct.
    const FheParams& params() const { return _params; }

    // ── Encode operations ──────────────────────────────

    void Encode_float(Plaintext& pt, const float* input, size_t len, ScaleType scale, LevelType level);
    void Encode_float_cst_lvl(Plaintext& pt, const float* input, size_t len, ScaleType scale, int level);
    void Encode_float_mask(Plaintext& pt, float input, size_t len, ScaleType scale, LevelType level);
    void Encode_float_mask_cst_lvl(Plaintext& pt, float input, size_t len, ScaleType scale, int level);

    // ── HE operations ──────────────────────────────────

    void Add(Ciphertext& res, const Ciphertext& op1, const Ciphertext& op2)
    {
        if (op1.size() == 0) {
            res = op2;
            return;
        }
        if (op2.size() == 0) {
            res = op1;
            return;
        }
        Ciphertext tmp1 = op1, tmp2 = op2;
        Backend::equal_level_ciph(cur_evaluator(), tmp1, tmp2);
        Backend::Add(cur_evaluator(), res, tmp1, tmp2);
    }

    void Add(Ciphertext& res, const Ciphertext& op1, const Plaintext& op2)
    {
        Ciphertext tmp1 = op1;
        Plaintext tmp2 = op2;
        Backend::equal_level_plain(cur_evaluator(), tmp1, tmp2);
        Backend::Add(cur_evaluator(), res, tmp1, tmp2);
    }

    void Add_const(Ciphertext& res, const Ciphertext& op1, double op2)
    {
        Plaintext pt;
        Backend::encode_const(cur_evaluator(), pt, op2, op1);
        Backend::Add(cur_evaluator(), res, op1, pt);
    }

    void Mul(Ciphertext& res, const Ciphertext& op1, const Ciphertext& op2)
    {
        Ciphertext tmp1 = op1, tmp2 = op2;
        Backend::equal_level_ciph(cur_evaluator(), tmp1, tmp2);
        Backend::Mul(cur_evaluator(), res, tmp1, tmp2);
    }

    void Mul(Ciphertext& res, const Ciphertext& op1, const Plaintext& op2)
    {
        Ciphertext tmp1 = op1;
        Plaintext tmp2 = op2;
        Backend::equal_level_plain(cur_evaluator(), tmp1, tmp2);
        Backend::Mul(cur_evaluator(), res, tmp1, tmp2);
    }

    void Mul_const(Ciphertext& res, const Ciphertext& op1, double op2)
    {
        Plaintext pt;
        Backend::encode_const(cur_evaluator(), pt, op2, op1);
        Backend::Mul(cur_evaluator(), res, op1, pt);
    }

    void Rotate(Ciphertext& res, const Ciphertext& op1, int step)
    {
        if (step < 0) {
            step = static_cast<int>(_params.slot_count + step);
        }
        // On-demand galois-key loading (KeyMode::ON_DEMAND): make the relin
        // key for this rotation step resident in HBM before the vendor derefs
        // it. No-op in PRELOAD/ONLINE (Ensure_galois_key returns true early).
        auto& km = session(Backend::active_session_idx()).key_mgr();
        km.Ensure_galois_key(step);
        Backend::Rotate(cur_evaluator(), km.galois_key(), res, op1, step);
    }

    /// Explicit galois-key prefetch point (stage-2): codegen emits Load_galois_key(step)
    /// before a rotate so the load can be hoisted/moved for timing. Idempotent;
    /// no-op unless KeyMode::ON_DEMAND(_FULL). Same path as Rotate's internal call.
    void Ensure_galois_key(int step)
    {
        if (step < 0) {
            step = static_cast<int>(_params.slot_count + step);
        }
        session(Backend::active_session_idx()).key_mgr().Ensure_galois_key(step);
    }

    /// Stage-3 liveness-driven free: codegen emits Free_galois_key(step) at a
    /// rotate's last use (fhe-cmplr MFREE-style key liveness). Evicts the slot's
    /// HBM. No-op unless ON_DEMAND(_FULL). Mirror of Ensure_galois_key.
    void Evict_galois_key(int step)
    {
        if (step < 0) {
            step = static_cast<int>(_params.slot_count + step);
        }
        session(Backend::active_session_idx()).key_mgr().Evict_galois_key(step);
    }

    void Rescale(Ciphertext& res, const Ciphertext& op1) { Backend::Rescale_next(cur_evaluator(), res, op1); }

    void Mod_switch(Ciphertext& res, const Ciphertext& op1) { Backend::Mod_switch_next(cur_evaluator(), res, op1); }

    void Relin(Ciphertext& res, const Ciphertext& op1)
    {
        Backend::Relinearize(cur_evaluator(), session(Backend::active_session_idx()).key_mgr().relin_key(), res, op1);
    }

    void Bootstrap(Ciphertext& res, Ciphertext& op1, int level, int slot);

    // ── Memory / Scale / Level / Debug / I/O ───────────

    void Free_cipher(Ciphertext& ct) { Backend::free_cipher(ct); }
    void Free_plain(Plaintext& pt) { Backend::free_plain(pt); }

    ScaleType Scale(const Ciphertext& op)
    {
        return static_cast<uint64_t>(std::log2(op.scale())) / _params.scaling_mod_size;
    }

    LevelType Level(const Ciphertext& op) { return Backend::ciphertext_level(op); }

    void Decrypt(Ciphertext& ct, std::vector<double>& vec)
    {
        auto host_ct = Backend::to_host_ciphertext(ct, *_crypto_ctx);
        typename Backend::HostPlaintext host_pt;
        session(Backend::active_session_idx()).crypto().decryptor->decrypt(host_ct, host_pt);
        Backend::decode_from_host(cur_evaluator(), host_pt, vec);
    }

    void Decode(Plaintext& pt, std::vector<double>& vec)
    {
        auto host_pt = Backend::to_host_plaintext_for_decode(pt, *_crypto_ctx);
        Backend::decode_from_host(cur_evaluator(), host_pt, vec);
    }
    void Prepare_input(void* tensor, const char* name);

    Ciphertext Get_input_data(const char* name, size_t idx)
    {
        Ciphertext* data = static_cast<Ciphertext*>(Io_get_input(name, idx));
        IS_TRUE(data != nullptr, "not find data");
        Ciphertext ret = std::move(*data);
        delete data;
        return ret;
    }

    double* Handle_output(const char* name, size_t idx);

private:
    explicit FheContext(KeyMode key_mode, EncPolicy enc_policy);

    // ── Thread-local context pointer ────────────────────
    // (Removed: TLS now lives in backend .so via Backend::current_context_ptr/
    //  set_current_context_ptr; see MakeCurrent/Current/ClearCurrent above.)

    // ── Configuration state ────────────────────────────
    EncPolicy _enc_policy = EncPolicy::LOAD_RAW;
    FheParams _params;

    // ── Components (declaration order = initialization/destroy order) ──
    //
    // _crypto_ctx (backend crypto context, e.g. PhantomContext / HostContext)
    //   is created first, then a non-owning pointer is passed to _session's
    //   KeyMgr.  _session's KeyMgr uses the context but does not own it.
    //   _evaluator and _enc_mgr depend on the crypto context being alive.
    //   Destruction is in reverse order: the explicit destructor resets the
    //   evaluator, then _session KeyMgr clears its pointer, then _crypto_ctx
    //   is destroyed by unique_ptr.
    std::unique_ptr<typename Backend::Context> _crypto_ctx;
    FheSession<Backend> _session;
    ///< Extra sessions for multi-user mode B/B-P (index 1..N). Created on demand
    ///< by create_session(); owned by this context, share the crypto context.
    std::vector<std::unique_ptr<FheSession<Backend>>> _extra_sessions;
    ///< Primary session's evaluator (idx 0). Built once at init, bound to primary
    ///< keys. Kept as a single member so the init Stage-2 encoder / Stage-3
    ///< evaluator / Stage-4 galois paths are unchanged.
    typename Backend::Evaluator _evaluator;
    ///< Extra-session evaluators (Mode B-P, index 1..N). Each built at
    ///< create_session and bound to its session's keys — immutable after, so
    ///< concurrent ops never mutate a shared evaluator. cur_evaluator() returns
    ///< _evaluator for idx 0, else _extra_evaluators[idx-1].
    std::vector<typename Backend::Evaluator> _extra_evaluators;
    ///< Per-session CUDA stream owners (phantom: cuda_stream_wrapper; ace: empty
    ///< struct placeholder). Index 0 = primary. Owned by context.
    std::vector<typename Backend::SessionStreamOwner> _session_stream_owners;
    typename Backend::BtsMgr _bootstrap;
    ///< Extra-session bootstrap managers (Mode B-P). One per extra session, each
    ///< built from that session's CKKSEvaluator so a bootstrap's encode/decode
    ///< hits the session's own PhantomCKKSEncoder (independent FFT `in_` scratch),
    ///< and its Bootstrapper/ModularReducer workspace is per-instance. Without
    ///< this, Bootstrap() would route two worker threads through the single
    ///< primary `_bootstrap` (shared Bootstrapper) and race its mutable GPU
    ///< workspace. cur_bootstrap() returns _bootstrap for idx 0, else
    ///< _extra_bootstraps[idx-1].
    std::vector<typename Backend::BtsMgr> _extra_bootstraps;
    ///< Extra-session GaloisTool copies (Mode B-P, phantom). One per extra
    ///< session, each built on the session's CUDA stream (~18 MiB) so runtime
    ///< apply_galois/rotate/conjugate read a stream-ordered tool via the
    ///< thread-local override (phantom set_thread_galois_tool), instead of the
    ///< shared PhantomContext::key_galois_tool_ built on default_stream (whose
    ///< cross-stream reads corrupt the CUDA context under parallel bootstrap).
    ///< Replaces the 22 GiB galois-key regen workaround. Activate_session sets
    ///< the calling thread's tool override; idx 0 uses the shared tool (nullptr).
    std::vector<std::unique_ptr<typename Backend::GaloisTool>> _extra_galois_tools;
    std::shared_ptr<EncMgr<Backend>> _enc_mgr;
};

}  // namespace fhert

#endif  // FHERT_CONTEXT_H
