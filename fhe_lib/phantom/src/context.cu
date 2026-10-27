//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief context.cu
//! FheContext<VendorTraits> explicit instantiation + HE operations.
//!
//! PHANTOM_CONTEXT is now a backward-compat alias for FheContext<PhantomTraits>.

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"
#include "common/tensor.h"
#include "fhe/context.h"
#include "fhe/need_bts.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/phantom/bts_mgr.h"
#include "fhe/vendor/phantom/vendor_traits.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

using namespace phantom;
using namespace phantom::arith;
using namespace phantom::util;

// Use Traits type aliases so specializations match the template base signatures.
using Ciphertext = phantom::VendorTraits::Ciphertext;
using Plaintext = phantom::VendorTraits::Plaintext;
using ScaleType = phantom::VendorTraits::ScaleType;
using LevelType = phantom::VendorTraits::LevelType;

namespace fhert
{

//=============================================================================
// FheContext<VendorTraits> — Constructor & Destructor
//=============================================================================

template <>
FheContext<phantom::VendorTraits>::FheContext(KeyMode key_mode, EncPolicy enc_policy)
    : _enc_policy(enc_policy), _params()
{

    using Traits = phantom::VendorTraits;

    auto t_total_0 = std::chrono::high_resolution_clock::now();

    bool with_bootstrap = Need_bts();
    CKKS_PARAMS* prog_param = Get_context_params();
    IS_TRUE(prog_param->_provider == LIB_PHANTOM, "provider is not PHANTOM");

    _params.scaling_mod_size = prog_param->_scaling_mod_size;
    if (with_bootstrap) {
        _params.slot_count = prog_param->_poly_degree / 2;
    }

    // --- Stage 1a: Create PhantomContext (owned by FheContext) ---
    auto t0 = std::chrono::high_resolution_clock::now();
    _crypto_ctx = Traits::create_context(with_bootstrap);
    auto t1 = std::chrono::high_resolution_clock::now();
    double s_create_ctx = std::chrono::duration<double>(t1 - t0).count();
    spdlog::info("[phantom] PhantomContext created (N={}) [{:.2f}s]", (unsigned)Traits::poly_degree(*_crypto_ctx),
                 s_create_ctx);

    // --- Stage 1b: Initialize key manager (uses externally-owned context) ---
    // Bind the session to the crypto context and set the key mode.
    // KeyMgr::Init() then loads or generates keys based on the mode.
    //   PRELOAD loads eval-only (RLK+GK), PRELOAD_FULL loads all (SK+PK+RLK+GK).
    _session = FheSession<Traits>(_crypto_ctx.get(), key_mode);
    auto t1_0 = std::chrono::high_resolution_clock::now();
    _session.key_mgr().Init(*_crypto_ctx, with_bootstrap);
    // Synchronize key mode from KeyMgr (it reads the global state).
    auto t1_1 = std::chrono::high_resolution_clock::now();
    double s_key_mgr = std::chrono::duration<double>(t1_1 - t1_0).count();

    // --- Stage 2: Create encoder ---
    auto t2_0 = std::chrono::high_resolution_clock::now();
    _evaluator.encoder = Traits::create_encoder(*_crypto_ctx);
    auto t2_1 = std::chrono::high_resolution_clock::now();
    double s_encoder = std::chrono::duration<double>(t2_1 - t2_0).count();

    // --- Stage 3: Create evaluator (allocates GPU memory, uploads keys) ---
    auto t3_0 = std::chrono::high_resolution_clock::now();

    if (!can_verify()) {
        // Compute-only mode: SK and PK are not loaded from disk.
        // Generate minimal dummy SK/PK solely for CKKSEvaluator construction.
        // The dummy keys are stored in _key_mgr so they outlive the evaluator.
        // Prepare_input() and Handle_output() are guarded to prevent real use.
        spdlog::info("[compute-only] Generating dummy SK/PK for evaluator construction...");
        _session.key_mgr().generate_dummy_keys_for_evaluator();
        _evaluator.eval = Traits::create_evaluator(*_crypto_ctx, _session.key_mgr().secret_key(),
                                                   _session.key_mgr().public_key(), _session.key_mgr().relin_key(),
                                                   _session.key_mgr().galois_key(), _evaluator.encoder.get(),
                                                   _params.scaling_mod_size);
    }
    else {
        _evaluator.eval = Traits::create_evaluator(*_crypto_ctx, _session.key_mgr().secret_key(),
                                                   _session.key_mgr().public_key(), _session.key_mgr().relin_key(),
                                                   _session.key_mgr().galois_key(), _evaluator.encoder.get(),
                                                   _params.scaling_mod_size);
    }
    auto t3_1 = std::chrono::high_resolution_clock::now();
    double s_evaluator = std::chrono::duration<double>(t3_1 - t3_0).count();

    // Initialize the primary session's Crypto from the evaluator.
    // The CKKSEvaluator stores encryptor/decryptor internally; we mirror
    // them into the session's Crypto for unified access across backends.
    _session.crypto().encryptor = std::make_unique<Encryptor>(_evaluator.eval->encryptor);
    _session.crypto().decryptor = std::make_unique<Decryptor>(_evaluator.eval->decryptor);

    // Per-session CUDA stream for the primary session (idx 0, Mode B-P). The
    // stream owner vector is index-aligned with sessions: [0]=primary, [1..N]
    // = extra sessions (added by create_session). Worker threads set phantom's
    // thread_local tl_current_stream to their session's wrapper.
    _session_stream_owners.emplace_back(std::make_unique<phantom::util::cuda_stream_wrapper>());

    // --- KMS: rebuild GaloisTool from loaded galois elements ---
    if (_session.key_mgr().keys_loaded_from_kms()) {
        const auto& galois_elts = _session.key_mgr().loaded_galois_elts();
        if (!galois_elts.empty()) {
            auto* ctx = _crypto_ctx.get();
            Traits::rebuild_galois_tool(*ctx, galois_elts);
            spdlog::info("[kms] Rebuilt GaloisTool with {} loaded galois elements", galois_elts.size());
        }
    }

    // --- Stage 4: Bootstrap / galois key initialization ---
    auto t4_0 = std::chrono::high_resolution_clock::now();
    if (with_bootstrap) {
        _params.num_prime_parts = Traits::init_bootstrap(
            _evaluator.eval.get(), _bootstrap, _session.key_mgr().keys_loaded_from_kms(), _params.scaling_mod_size,
            prog_param->_mul_depth, prog_param->_first_mod_size, prog_param->_scaling_mod_size);
    }
    else {
        std::vector<int> bits;
        bits.push_back(prog_param->_first_mod_size);
        for (uint32_t i = 0; i < prog_param->_mul_depth; ++i)
            bits.push_back(prog_param->_scaling_mod_size);
        bits.push_back(prog_param->_first_mod_size);
        _params.num_prime_parts = bits.size();

        // When keys were loaded from KMS, galois keys are already in the
        // evaluator (populated by create_evaluator from loaded GPU data).
        // Skip key regeneration — otherwise we discard the loaded keys and
        // spend ~17s regenerating them from the secret key on GPU.
        if (!_session.key_mgr().keys_loaded_from_kms()) {
            std::vector<int> rotation_keys(prog_param->_rot_idxs, prog_param->_rot_idxs + prog_param->_num_rot_idx);
            std::vector<int> gal_steps_vector;
            for (auto rot : rotation_keys) {
                if (std::find(gal_steps_vector.begin(), gal_steps_vector.end(), rot) == gal_steps_vector.end())
                    gal_steps_vector.push_back(rot);
            }

            Traits::create_galois_keys_from_steps(*_evaluator.eval, gal_steps_vector);
        }
        else {
            spdlog::info("[kms] Skipping galois key generation — using keys loaded from disk");
        }
    }
    auto t4_1 = std::chrono::high_resolution_clock::now();
    double s_galois_init = std::chrono::duration<double>(t4_1 - t4_0).count();

    // Bootstrap key sync prefetch (hoist): in ON_DEMAND(_FULL), prime all 67
    // galois slots now from the index just read in KeyMgr::Init — so the first
    // FheContext::Bootstrap's Ensure_bootstrap_keys (context.cu Bootstrap entry)
    // hits early-return (ensure_one_slot idempotent, key_mgr.cu:377) instead of
    // blocking the critical path on ~1-2s of NVMe read. No-op for PRELOAD/ONLINE
    // (_on_demand_ready == false → Ensure_bootstrap_keys returns immediately).
    // Doesn't change HBM peak (21.4G/user unchanged), only hoists the timing.
    //
    // Known coupling limits of this SYNC hoist (by design, accepted):
    //  (1) bound to create_session/Init wall time — the 21.4G read blocks THIS
    //      call synchronously (~5s measured), so N create_session calls are
    //      serial in the setup phase. Not optimized for setup speed; setup is
    //      not the control target — per-user inference (load+compute overlap)
    //      is. Lifting the read off the setup path requires a background
    //      prefetch thread (decouple prefetch from create_session), not async IO.
    //  (2) bound to a concrete user — bootstrap galois keys are per-session and
    //      non-shareable (each user's keys differ), so prefetch can only fetch
    //      THIS user's 67 slots, keyed by key_dir. Cannot pre-fetch an unknown
    //      future user. This is a hard FHE constraint, not introduced by hoist.
    // Next-step load+compute overlap for inference galois keys (constant/
    // variable) is a separate workstream; see
    // .claude/plans/multi-user/key-load-insertion-point-analysis.md.
    // Bootstrap key sync prefetch (hoist): in ON_DEMAND(_FULL), prime all 67
    // galois slots now from the index just read in KeyMgr::Init — so the first
    // FheContext::Bootstrap's Ensure_bootstrap_keys (context.cu Bootstrap entry)
    // hits early-return (ensure_one_slot idempotent, key_mgr.cu:377) instead of
    // blocking the critical path on ~1-2s of NVMe read. No-op for PRELOAD/ONLINE
    // (_on_demand_ready == false → Ensure_bootstrap_keys returns immediately).
    // Doesn't change HBM peak (21.4G/user unchanged), only hoists the timing.
    //
    // FHERT_SKIP_BOOTSTRAP_HOIST=1 skips this prefetch. Intended for a client
    // that only encrypts/decrypts (never bootstraps/evals) and so never needs
    // the 21 GiB galois set resident — it avoids a wasteful load at startup.
    // Server/eval callers must NOT set it (first bootstrap would pay the cost
    // on the critical path, or the slots stay cold mid-eval). Default off.
    if (with_bootstrap && !std::getenv("FHERT_SKIP_BOOTSTRAP_HOIST")) {
        auto t_pf0 = std::chrono::high_resolution_clock::now();
        _session.key_mgr().Ensure_bootstrap_keys();
        auto t_pf1 = std::chrono::high_resolution_clock::now();
        spdlog::info("[kms] bootstrap key prefetch (hoist) {:.3f}s",
                     std::chrono::duration<double>(t_pf1 - t_pf0).count());
    }
    else if (with_bootstrap && std::getenv("FHERT_SKIP_BOOTSTRAP_HOIST")) {
        spdlog::info(
            "[kms] bootstrap key prefetch (hoist) skipped (FHERT_SKIP_BOOTSTRAP_HOIST set; client encrypt-only path)");
    }

    spdlog::debug("[phantom] ckks_param: _provider={}, _poly_degree={}, _sec_level={}, "
                  "mul_depth={}, _first_mod_size={}, _scaling_mod_size={}, "
                  "_num_q_parts={}, _num_rot_idx={}, _num_prime_parts={}",
                  static_cast<int>(prog_param->_provider), prog_param->_poly_degree, prog_param->_sec_level,
                  prog_param->_mul_depth, prog_param->_first_mod_size, prog_param->_scaling_mod_size,
                  prog_param->_num_q_parts, prog_param->_num_rot_idx, _params.num_prime_parts);

    // --- Stage 5: Weight file init ---
    auto t5_0 = std::chrono::high_resolution_clock::now();
    RT_DATA_INFO* data_info = Get_rt_data_info();
    if (data_info != nullptr) {
        _enc_mgr = std::make_shared<EncMgr<phantom::VendorTraits>>();
        _enc_mgr->Init(data_info->_file_name, _crypto_ctx.get(), _enc_policy);
    }
    auto t5_1 = std::chrono::high_resolution_clock::now();
    double s_enc_mgr = std::chrono::duration<double>(t5_1 - t5_0).count();

    auto t_total_1 = std::chrono::high_resolution_clock::now();
    double s_total = std::chrono::duration<double>(t_total_1 - t_total_0).count();

    spdlog::debug("[timing] FheContext init: total={:.2f}s | key_mgr={:.2f}s encoder={:.2f}s "
                  "evaluator={:.2f}s galois_init={:.2f}s enc_mgr={:.2f}s",
                  s_total, s_key_mgr, s_encoder, s_evaluator, s_galois_init, s_enc_mgr);
}

template <> void FheContext<phantom::VendorTraits>::switch_session(FheSession<phantom::VendorTraits>& /*session*/)
{
    // Mode B-P: session switching is a per-thread index set (see
    // switch_session_by_idx / set_active_session_idx_for_this_thread). Each
    // session has its OWN evaluator pre-bound to its keys at create_session
    // time, so there is no shared-evaluator key mutation and no GPU work here.
    // This overload is retained for API compatibility (called by
    // switch_session_by_idx's idx==0 arm) and is a no-op on the primary thread.
    spdlog::warn("[fhe] switch_session(FheSession&) is a no-op in Mode B-P; use switch_session_by_idx(idx) to set the "
                 "calling thread's active session.");
}

template <> int FheContext<phantom::VendorTraits>::create_session(const std::string& key_dir)
{
    if (!_crypto_ctx) {
        spdlog::error("[fhe] create_session: no crypto context (call init first)");
        return -1;
    }
    using Traits = phantom::VendorTraits;
    // Inherit the primary session's key mode and IO policy so the extra session
    // loads the same key set (e.g. PRELOAD_FULL = SK/PK/RLK/GK).
    const KeyMode km = _session.key_mgr().key_mode();
    // Build a fresh session sharing this context, then load a full key bundle
    // from the explicit dir (NOT the process-global key_path).
    auto sess = std::make_unique<FheSession<phantom::VendorTraits>>(_crypto_ctx.get(), km);
    // Honor key_mode: on-demand modes build an empty-shell galois (deferred to
    // Ensure_galois_key); otherwise full-load via Load_keys. Previously this
    // always called Load_keys (which ignores key_mode), so extra sessions in
    // ON_DEMAND(_FULL) still full-loaded galois ~22 GiB — defeating on-demand
    // for multi-user. Prepare_on_demand_keys returns false for non-on-demand
    // modes, falling back to Load_keys (PRELOAD_FULL etc. unchanged).
    if (!sess->key_mgr().Prepare_on_demand_keys(key_dir)) {
        if (!sess->key_mgr().Load_keys(key_dir, _session.key_mgr().effective_io_policy())) {
            spdlog::error("[fhe] create_session: Load_keys failed for {}", key_dir);
            return -1;
        }
    }
    IS_TRUE(sess->key_mgr().relin_key_ptr() != nullptr, "create_session: loaded session missing relin keys");
    IS_TRUE(sess->key_mgr().galois_key_ptr() != nullptr, "create_session: loaded session missing galois keys");

    // Mode B-P: build this session's OWN evaluator (sharing the crypto context
    // + NTT/sin-cos tables), bound to THIS session's keys. create_evaluator sets
    // the CKKSEvaluator's relin/galois/encrypt/decrypt pointers from the loaded
    // keys — no shared-evaluator mutation, keys already resident from Load_keys.
    typename Traits::Evaluator ev;
    ev.encoder = Traits::create_encoder(*_crypto_ctx);
    ev.eval = Traits::create_evaluator(*_crypto_ctx, sess->key_mgr().secret_key(), sess->key_mgr().public_key(),
                                       sess->key_mgr().relin_key(), sess->key_mgr().galois_key(), ev.encoder.get(),
                                       _params.scaling_mod_size);
    sess->crypto().encryptor = std::make_unique<Encryptor>(ev.eval->encryptor);
    sess->crypto().decryptor = std::make_unique<Decryptor>(ev.eval->decryptor);

    // Per-session CUDA stream (Mode B-P): worker threads running this session
    // set phantom's thread_local tl_current_stream to this wrapper.
    _session_stream_owners.emplace_back(std::make_unique<phantom::util::cuda_stream_wrapper>());

    // Bootstrap key sync prefetch (hoist): prime this session's 67 galois slots
    // now (Prepare_on_demand_keys already read the index + set _on_demand_ready)
    // so the first Bootstrap hits early-return instead of blocking on NVMe.
    // No-op outside on-demand modes. Must run before push_back moves sess.
    // Same known sync-hoist coupling limits as the primary-Init prefetch above
    // (blocks create_session wall time; bound to this concrete user).
    sess->key_mgr().Ensure_bootstrap_keys();

    _extra_sessions.push_back(std::move(sess));
    _extra_evaluators.emplace_back(std::move(ev));

    // Per-session GaloisTool copy (Mode B-P): build this session's OWN
    // PhantomGaloisTool on THIS session's CUDA stream (~18 MiB), using the
    // same galois elements as the primary (loaded from key_dir). Runtime
    // apply_galois/rotate/conjugate then read it via the thread-local override
    // (set in Activate_session_for_this_thread) instead of the shared
    // PhantomContext::key_galois_tool_ (built on default_stream during primary
    // init — whose cross-stream reads corrupt the CUDA context under parallel
    // bootstrap). This REPLACES the regen workaround: previously
    // create_galois_keys_from_steps rebuilt the shared tool on current_stream
    // but ALSO regenerated a 22 GiB galois-key set (~17s + ~22G per session).
    {
        const auto& gelts = _extra_sessions.back()->key_mgr().loaded_galois_elts();
        if (!gelts.empty()) {
            int log_n = phantom::arith::get_power_of_two(_crypto_ctx->poly_degree_);
            // Build on the session's stream: set tl_current_stream first so the
            // PhantomGaloisTool ctor's cudaMemcpyAsync of permutation_tables_
            // lands on the session stream (stream-ordered w.r.t. later reads).
            auto* sav_stream = &phantom::util::global_variables::get_thread_stream();
            phantom::util::global_variables::set_thread_stream(_session_stream_owners.back().get());
            _extra_galois_tools.emplace_back(std::make_unique<::PhantomGaloisTool>(
                gelts, log_n, _session_stream_owners.back()->get_stream(), /*is_bfv=*/false));
            phantom::util::global_variables::set_thread_stream(sav_stream);
        }
    }

    // Plan B: share the primary _bootstrap across all sessions (no per-session
    // BtsMgr). Bootstrapper's LT coefficients / ModularReducer are model-level
    // read-only; bootstrap workspace is function-local (per-call); per-session
    // key routing is done by switching Bootstrapper.ckks to cur_evaluator at
    // FheContext::Bootstrap entry. Saves ~16 GiB per extra session (previously
    // each extra session built 4 Bootstrappers via init_bootstrap).

    return static_cast<int>(_extra_sessions.size());  // 1-based index
}

template <> void FheContext<phantom::VendorTraits>::switch_session_by_idx(int idx)
{
    if (idx < 0 || idx >= num_sessions()) {
        spdlog::error("[fhe] switch_session_by_idx: index {} out of range [0, {}]", idx, num_sessions() - 1);
        return;
    }
    // Mode B-P: set the CALLING thread's active session index. No key upload,
    // no GPU work — each session's evaluator was bound to its keys at
    // create_session/init time and is immutable. Different worker threads can
    // hold different active indices concurrently.
    set_active_session_idx_for_this_thread(idx);
    Io_bind(idx);  // route Io_set/get_input/output to this session's own per-slot store
}

template <> void FheContext<phantom::VendorTraits>::release_session(int idx)
{
    if (idx < 0 || idx >= num_sessions()) {
        spdlog::error("[fhe] release_session: index {} out of range [0, {}]", idx, num_sessions() - 1);
        return;
    }
    // Free all galois-key slots for this session (free HBM). The session
    // structure (evaluator, GaloisTool, BtsMgr) stays; only galois key bytes
    // are evicted. Ensure_galois_key re-loads from disk on next use.
    session(idx).key_mgr().Release_all_keys();
}

template <> void FheContext<phantom::VendorTraits>::Activate_session_for_this_thread(int idx)
{
    IS_TRUE(idx >= 0 && idx < num_sessions(), "Activate_session_for_this_thread: idx out of range");
    // Worker pthreads run the compiled FHE kernel; ensure THIS thread has a
    // current CUDA context before any kernel launch/stream use. The CUDA runtime
    // lazily binds the primary context per-thread, but explicit cudaSetDevice
    // forces it so; without it a worker's kernel launch can SEGV.
    cudaSetDevice(0);
    // Route this thread's ops to session `idx`'s evaluator + keys.
    set_active_session_idx_for_this_thread(idx);
    // Route Io_set/get_input/output (rtlib common io_lib) to this session's
    // OWN per-session store. Without this, Io used a process-global singleton
    // (#pragma omp threadprivate, OpenMP-only) → K>=2 concurrent workers
    // shared one Input_data → Get_input_data's std::move+delete raced →
    // double-free / tcache abort (B-P K=3 race root).
    Io_bind(idx);
    // Make this FheContext the thread-local current context (compiled kernel
    // ops resolve the context via FheContext::Current()).
    MakeCurrent();
    // Bind phantom's per-thread CUDA stream via the phantom .so setter — setting
    // the thread_local must stay confined to phantom .so's TLS (a cross-DSO
    // thread_local write from fhe_lib would touch a different TLS instance).
    //
    // NOTE: idx==0 (primary) deliberately does NOT rebind the thread stream.
    // The primary runs on phantom's default stream (legacy single-session
    // behavior — the pre-B-P Run() path never called set_thread_stream). Its
    // _session_stream_owners[0] would be destroyed at Finalize_context, leaving
    // the thread-local stream dangling → a second Prepare_context's key-gen
    // cudaMallocAsync(stream) hits "invalid resource handle". Only extra
    // sessions (idx>=1) need a per-session stream binding.
    if (idx >= 1 && !_session_stream_owners.empty() && idx < static_cast<int>(_session_stream_owners.size())) {
        phantom::util::global_variables::set_thread_stream(_session_stream_owners[idx].get());
    }
    else if (idx >= 1) {
        phantom::util::global_variables::set_thread_stream(nullptr);
    }
    // idx==0: leave the thread stream untouched (default stream).
    // Bind phantom's per-thread GaloisTool override (Mode B-P). idx 0 uses the
    // shared PhantomContext::key_galois_tool_ (nullptr); idx>=1 uses this
    // session's own tool copy (built on the session stream at create_session).
    // Same cross-DSO TLS discipline as the stream above.
    if (idx >= 1 && (idx - 1) < static_cast<int>(_extra_galois_tools.size())) {
        phantom::util::global_variables::set_thread_galois_tool(_extra_galois_tools[idx - 1].get());
    }
    else {
        phantom::util::global_variables::set_thread_galois_tool(nullptr);
    }
}

//=============================================================================
// FheContext<VendorTraits> — Encode operations
//=============================================================================

template <>
void FheContext<phantom::VendorTraits>::Encode_float(Plaintext& pt, const float* input, size_t len, double scale,
                                                     size_t level)
{
    std::vector<double> vec(input, input + len);
    // Route to the per-session encoder (cur_evaluator): each session owns its own
    // PhantomCKKSEncoder with its own mutable FFT scratch (gpu_ckks_msg_vec_->in_).
    // Using the primary _evaluator here would race two worker threads on one encoder.
    cur_evaluator().eval->encoder.encode(vec, _params.num_prime_parts - level,
                                         std::pow(2.0, _params.scaling_mod_size * scale), pt);
}

template <>
void FheContext<phantom::VendorTraits>::Encode_float_cst_lvl(Plaintext& pt, const float* input, size_t len,
                                                             double scale, int level)
{
    std::vector<double> vec(input, input + len);
    auto& context_data = _crypto_ctx->get_context_data(level);
    cur_evaluator().encoder->encode(*_crypto_ctx, vec, std::pow(2.0, _params.scaling_mod_size * scale), pt,
                                    context_data.chain_index());
}

template <>
void FheContext<phantom::VendorTraits>::Encode_float_mask(Plaintext& pt, float input, size_t len, double scale,
                                                          size_t level)
{
    std::vector<double> vec(len, input);
    cur_evaluator().eval->encoder.encode(vec, _params.num_prime_parts - level,
                                         std::pow(2.0, _params.scaling_mod_size * scale), pt);
}

template <>
void FheContext<phantom::VendorTraits>::Encode_float_mask_cst_lvl(Plaintext& pt, float input, size_t len, double scale,
                                                                  int level)
{
    std::vector<double> vec(len, input);
    auto& context_data = _crypto_ctx->get_context_data(level);
    cur_evaluator().encoder->encode(*_crypto_ctx, vec, std::pow(2.0, _params.scaling_mod_size * scale), pt,
                                    context_data.chain_index());
}

//=============================================================================
// FheContext<VendorTraits> — HE operations
//=============================================================================

template <> void FheContext<phantom::VendorTraits>::Bootstrap(Ciphertext& res, Ciphertext& op1, int level, int slot)
{
    // Route to the calling thread's session: bootstrap's internal encode/decode
    // + Bootstrapper/ModularReducer workspace are per-instance (each session has
    // its own BtsMgr built from its own CKKSEvaluator / PhantomCKKSEncoder). The
    // primary _evaluator/_bootstrap would be shared across worker threads.
    auto& ev = cur_evaluator();
    auto& bts = cur_bootstrap();
    // Plan B: BtsMgr is shared across sessions. Switch each Bootstrapper's
    // `ckks` pointer to the calling thread's evaluator so bootstrap's
    // rotate/mul/encode route to THIS session's keys. Bootstrapper uses ckks
    // only via real-time method calls (no cached key state), and its workspace
    // buffers are function-local → concurrent sessions can share safely.
    for (auto s : {BTS_SLOTS::POW_15, BTS_SLOTS::POW_14, BTS_SLOTS::POW_13, BTS_SLOTS::POW_12}) {
        auto* b = bts.Get(s);
        if (b != nullptr) {
            b->ckks = ev.eval.get();
        }
    }
    session(phantom::VendorTraits::active_session_idx()).key_mgr().Ensure_bootstrap_keys();
    ev.eval->evaluator.mod_switch_to_inplace(op1, _params.num_prime_parts - 1);

    int effective_slot = slot;
    if (effective_slot == 0) {
        effective_slot = static_cast<uint32_t>(BTS_SLOTS::POW_15);
    }

    Ciphertext* bootstrap_input = &op1;

    Ciphertext cyclic_input;
    bool cyclic_input_used = false;
    if (effective_slot > 0 && static_cast<uint64_t>(effective_slot) < _params.slot_count) {
        cyclic_input = op1;
        Ciphertext rotated_input = op1;
        for (uint64_t offset = effective_slot; offset < _params.slot_count; offset += effective_slot) {
            Rotate(rotated_input, rotated_input, effective_slot);
            Add(cyclic_input, rotated_input, cyclic_input);
        }
        rotated_input.release();
        bootstrap_input = &cyclic_input;
        cyclic_input_used = true;
    }

    Ciphertext in_place_input;
    bool in_place_bootstrap = &res == &op1;
    if (in_place_bootstrap) {
        in_place_input = *bootstrap_input;
        bootstrap_input = &in_place_input;
    }

    res.release();
    BTS_SLOTS slot_enum;
    switch (effective_slot) {
    case 32768:
        slot_enum = BTS_SLOTS::POW_15;
        break;
    case 16384:
        slot_enum = BTS_SLOTS::POW_14;
        break;
    case 8192:
        slot_enum = BTS_SLOTS::POW_13;
        break;
    case 4096:
        slot_enum = BTS_SLOTS::POW_12;
        break;
    default:
        slot_enum = BTS_SLOTS::POW_15;
        break;
    }
    auto bs = bts.Get(slot_enum);
    IS_TRUE(bs != nullptr, "Unsupported slot size for bootstrap");
    bs->bootstrap_real_3(res, *bootstrap_input);

    if (in_place_bootstrap) {
        in_place_input.release();
    }
    if (cyclic_input_used) {
        cyclic_input.release();
    }

    int target_level = _params.num_prime_parts - level;
    if (level != 0 && target_level > res.chain_index()) {
        ev.eval->evaluator.mod_switch_to_inplace(res, target_level);
    }
}

//=============================================================================
// Memory / Scale / Level / Debug / I/O
//=============================================================================

template <> void FheContext<phantom::VendorTraits>::Prepare_input(void* input, const char* name)
{
    using Traits = phantom::VendorTraits;
    if (!can_verify()) {
        throw std::runtime_error("[compute-only] Prepare_input() is not available without SK/PK. "
                                 "Inputs must be pre-encrypted and set via Io_set_input() externally.");
    }
    auto* tensor = static_cast<TENSOR*>(input);
    size_t len = TENSOR_SIZE(tensor);
    std::vector<double> vec(tensor->_vals, tensor->_vals + len);
    CKKS_PARAMS* prog_param = Get_context_params();
    Plaintext pt;
    int chain_index = _params.num_prime_parts - prog_param->_input_level;
    double encode_scale = std::pow(2.0, _params.scaling_mod_size);
    cur_evaluator().eval->encoder.encode(vec, chain_index, encode_scale, pt);
    Ciphertext* ct = new Ciphertext;
    session(Traits::active_session_idx()).crypto().encryptor->encrypt(pt, *ct);
    Io_set_input(name, 0, ct);
}

template <> double* FheContext<phantom::VendorTraits>::Handle_output(const char* name, size_t idx)
{
    using Traits = phantom::VendorTraits;
    if (!can_verify()) {
        throw std::runtime_error("[compute-only] Handle_output() is not available without SK/PK. "
                                 "Output ciphertexts must be retrieved and decrypted externally.");
    }
    Ciphertext* data = (Ciphertext*)Io_get_output(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    Plaintext pt;
    session(Traits::active_session_idx()).crypto().decryptor->decrypt(*data, pt);
    std::vector<double> vec;
    cur_evaluator().eval->encoder.decode(pt, vec);
    double* msg = (double*)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    delete data;
    return msg;
}

}  // namespace fhert

namespace phantom
{
using PHANTOM_CONTEXT = fhert::FheContext<VendorTraits>;
}  // namespace phantom
