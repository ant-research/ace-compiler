//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file kms_api.h
//! @brief Backend-parameterized C-style KMS key management entry points.
//!
//! `FheContext<VendorTraits>` and `KeySerializer<VendorTraits>` are templates
//! instantiated per backend; their `Current()` accessor and `key_mgr()`/crypto
//! context accessors are identically-shaped regardless of `VendorTraits`. This
//! header lifts the KMS save/load/query logic — previously duplicated verbatim
//! (modulo a typedef) in each backend's `api.cu` — into a single template that
//! each backend instantiates for its own `VendorTraits`.
//!
//! Architectural placement: this header lives in the `fhe/kms/` layer, the
//! SAME layer as `context.h` and `key_mgr.h`, because its only dependency is
//! the template `FheContext<Backend>`. It deliberately does NOT live under
//! `fhe_lib/common/`, which is a strictly lower layer (IO + key serialization
//! protocol) and must not depend on `FheContext`. See fhe_lib/CLAUDE notes.
//!
//! The backend `extern "C"` export symbols (`Kms_save_keys`, etc.) CANNOT be templates
//! (C linkage forbids it), so each backend's `api.cu` still provides the
//! thin `extern "C"` glue that instantiates these templates for its
//! `VendorTraits`. That glue is the only remaining per-backend code; all
//! KMS logic lives here, once.
//!
//! The galois load timer is folded into `KeyMgr<Backend>` (instance field,
//! set by Load_keys) and surfaced here via GetGaloisLoadTimeSeconds<Backend>.

#ifndef FHE_KMS_KMS_API_H
#define FHE_KMS_KMS_API_H

#include "common/io_api.h"  // Io_set_input / Io_get_output (C ABI)
#include "fhe/context.h"
#include "fhe/enc/ciphertext_io.h"  // CiphertextSerializer<Backend>
#include "fhe/kms/key_mgr.h"
#include "fhe/kms/key_serializer.h"
#include "fhe/kms/kms_runtime_api.h"  // Kms_* extern "C" prototypes (incl. Kms_make_current_on_this_thread)

#include <chrono>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace fhert
{
//! Per-call RunCipher stage timings (seconds), captured for the last RunCipher
//! call on THIS thread. Read via Kms_last_runcipher_timing() so the server can
//! ship them back to the client in an HTTP header (server & client are in
//! separate environments — the client can't see server-side logs).
struct RunCipherTiming
{
    double deser_sync = 0;    // deserialize input ct + device sync
    double launder_sync = 0;  // session-stream re-alloc + D2D + sync (cross-stream fix)
    double kernel = 0;        // Main_graph eval (pure GPU inference)
    double serialize = 0;     // output ct readback (D2H) + serialize
};
inline thread_local RunCipherTiming g_last_runcipher_timing;
}  // namespace fhert

namespace fhert
{

//=============================================================================
// KMS key management — template entry points (one implementation, all backends)
//=============================================================================

/// Save all keys (SK + PK + RLK + GK) to `key_dir`.
///
/// @return 0 on success, -1 on failure or thrown exception.
template <typename Backend> int SaveKeys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts)
{
    try {
        auto& ctx = FheContext<Backend>::Current();
        std::vector<uint32_t> elts;
        if (galois_elts && num_elts > 0) {
            elts.assign(galois_elts, galois_elts + num_elts);
        }
        bool ok = ctx.key_mgr().Save_keys(std::string(key_dir), elts, ctx.key_mgr().effective_io_policy());
        return ok ? 0 : -1;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] SaveKeys error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] SaveKeys error: unknown exception");
        return -1;
    }
}

/// Load all keys from `key_dir`.
///
/// @return 0 on success, -1 on failure or thrown exception.
/// (The historical ACE/phantom bodies disagreed on the success return value —
/// 1 vs 0; this canonicalizes on 0 for success to match SaveKeys.)
template <typename Backend> int LoadKeys(const char* key_dir)
{
    try {
        auto& km = FheContext<Backend>::Current().key_mgr();
        bool ok = km.Load_keys(std::string(key_dir), km.effective_io_policy());
        return ok ? 0 : -1;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] LoadKeys error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] LoadKeys error: unknown exception");
        return -1;
    }
}

/// Create an additional session sharing the current context, loading a full
/// key bundle from `key_dir` (multi-user Mode B).
/// @return session index (>=1) on success, -1 on failure.
template <typename Backend> int CreateSession(const char* key_dir)
{
    try {
        return FheContext<Backend>::Current().create_session(std::string(key_dir));
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] CreateSession error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] CreateSession error: unknown exception");
        return -1;
    }
}

/// Switch the active session by index (Mode B): 0 = primary, >=1 = extra.
/// @return 0 on success, -1 on failure.
template <typename Backend> int SwitchSession(int idx)
{
    try {
        FheContext<Backend>::Current().switch_session_by_idx(idx);
        return 0;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] SwitchSession error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] SwitchSession error: unknown exception");
        return -1;
    }
}

/// Mode B-P: activate session `idx` on the CALLING worker thread (sets the
/// thread-local active-session index + evaluator/key routing, makes this
/// context current on this thread, binds backend per-thread state — phantom:
/// per-session CUDA stream; ace: device context). Called by the kernel runner
/// before Main_graph() on each worker thread.
/// @return 0 on success, -1 on failure.
template <typename Backend> int ActivateSession(int idx)
{
    try {
        // Worker pthreads start with a null thread-local current context.
        // Bind the single globally-owned context to THIS thread first, so the
        // Activate path (which internally calls FheContext::Current()) resolves
        // to it rather than dereferencing a null TLS pointer.
        Kms_make_current_on_this_thread();
        FheContext<Backend>::Current().Activate_session_for_this_thread(idx);
        return 0;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] ActivateSession error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] ActivateSession error: unknown exception");
        return -1;
    }
}

/// Release all galois-key slots for session idx (free their HBM). The session
/// structure stays intact; only galois key bytes are evicted. Ensure re-loads
/// from disk on next use. Called by the service layer when a user is done.
/// @return 0 on success, -1 on failure.
template <typename Backend> int ReleaseSession(int idx)
{
    try {
        FheContext<Backend>::Current().release_session(idx);
        return 0;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] ReleaseSession error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] ReleaseSession error: unknown exception");
        return -1;
    }
}

/// Query how many sessions are currently resident (primary + extra created
/// via create_session).
/// @return Number of sessions (>=1 for the primary), or -1 on error.
template <typename Backend> int NumSessions()
{
    try {
        return static_cast<int>(FheContext<Backend>::Current().num_sessions());
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] NumSessions error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] NumSessions error: unknown exception");
        return -1;
    }
}

/// Ciphertext-in / ciphertext-out inference (service "step 2"). Accepts
/// already-encrypted input ciphertext bytes, evals with the active session's
/// keys (no server-side encrypt/decrypt), and serializes the output ciphertext
/// into `out_buf`.
///
/// `kernel_fn` = runtime-dlsym'd Main_graph, passed explicitly (avoids relying
/// on weak-symbol interposition). `input_names`/`input_bufs`/`input_sizes`
/// are parallel arrays (one per input). Caller owns `out_buf` (sized `out_cap`).
///
/// Returns the needed output byte count on success; if it exceeds `out_cap`,
/// the buffer is NOT written and the needed size is still returned (caller
/// retries with a bigger cap). Returns <0 on error.
///
/// Ownership: input ciphertexts are `new`-allocated; their pointers transfer
/// to the Io store (Io_set_input), like Prepare_input — RunCipher does NOT
/// free them. The output ciphertext is read via Io_get_output (which does not
/// delete), serialized, then `delete`d here (same discipline as
/// phantom_lib.cu Handle_output:100, avoiding a leak when the next
/// Io_set_output overwrites the slot).
template <typename Backend>
long RunCipher(int session_idx, bool (*kernel_fn)(), const char* const* input_names, const char* const* input_bufs,
               const size_t* input_sizes, size_t n_inputs, char* out_buf, size_t out_cap, const char* output_name)
{
    using Ciphertext = typename Backend::Ciphertext;
    using SerT = fhert::CiphertextSerializer<Backend>;

    try {
        // Activate the requested session on this thread (idempotent). Sets the
        // thread-local active ctx + per-thread stream + Io_bind,
        // so Io_set_input lands in the right per-session store. An out-of-range
        // idx throws here (caught below).
        Kms_make_current_on_this_thread();
        FheContext<Backend>::Current().Activate_session_for_this_thread(session_idx);

        // Per-stage timing: ALWAYS captured (so Kms_last_runcipher_timing() can
        // return the split to the client via HTTP); the verbose log line is only
        // printed under ACE_RUNCIPHER_TIMING=1. chrono only — no behavior change.
        const bool _rc_timing = (std::getenv("ACE_RUNCIPHER_TIMING") != nullptr);
        auto _rc_t0 = std::chrono::high_resolution_clock::now();
        double _rc_deser = 0, _rc_launder = 0, _rc_kernel = 0, _rc_serial = 0;

        // Single-pass optimization: the caller (KERNEL_RUNNER::Run_cipher) is
        // two-pass — pass1 out_buf=null to query size, pass2 out_buf!=null to
        // serialize. The kernel eval runs in pass1; pass2 must NOT re-run it.
        // pass1 caches its output ct in _cached (function-local static thread_local
        // — one per Backend instantiation); pass2 reuses it (serialize only). If a
        // caller passes out_buf!=null with no cached pass1 (single-pass caller),
        // the cache is empty and we run the full eval below (backward compatible).
        static thread_local Ciphertext* _cached = nullptr;
        if (out_buf != nullptr && _cached != nullptr) {
            // pass2 fast path: reuse pass1's eval output, serialize only.
            auto* out_ct = _cached;
            _cached = nullptr;  // consume — owned by us now
            size_t need = SerT::buffer_length(*out_ct);
            long ret = static_cast<long>(need);
            if (need <= out_cap) {
                if (!SerT::serialize(*out_ct, out_buf, need)) {
                    delete out_ct;
                    throw std::runtime_error("RunCipher: serialize output failed (pass2 cached)");
                }
            }
            // else: caller's buffer too small (shouldn't happen — they queried in
            // pass1); still return need, drop the cached ct (can't keep it).
            delete out_ct;
            _rc_serial = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - _rc_t0).count();
            // thread_local already holds pass1's deser/launder/kernel — fold in pass2's serialize.
            g_last_runcipher_timing.serialize = _rc_serial;
            if (_rc_timing) {
                spdlog::warn("[runcipher] pass2 (cached) serialize={:.3f}s (kernel not re-run)", _rc_serial);
            }
            return ret;
        }

        // Deserialize each input ciphertext and feed it into the Io input store.
        if (n_inputs == 0)
            throw std::runtime_error("RunCipher: no inputs");
        for (size_t i = 0; i < n_inputs; ++i) {
            if (input_names == nullptr || input_names[i] == nullptr)
                throw std::runtime_error("RunCipher: input name " + std::to_string(i) + " null");
            if (input_bufs == nullptr || input_sizes == nullptr || input_sizes[i] == 0)
                throw std::runtime_error("RunCipher: input buf " + std::to_string(i) + " empty");
            auto* ct = new Ciphertext();  // ownership -> Io store
            // Deserialize onto the GPU. populate_ciphertext does H2D on the
            // passed stream (default when null). The kernel later reads this
            // buffer on the active session's stream; without a sync the two
            // streams are unordered and the kernel could read stale memory.
            // Synchronize after each H2D so the buffer is fully resident and
            // visible before the kernel runs. (A cleaner fix routes H2D on the
            // session stream via a Backend trait; this global sync is the
            // minimal correct stopgap.)
            if (!SerT::deserialize(input_bufs[i], input_sizes[i], *ct, nullptr, nullptr)) {
                delete ct;
                throw std::runtime_error("RunCipher: deserialize failed for input " + std::to_string(i));
            }
            // deserialize's H2D ran on the default stream; the kernel (below)
            // reads this buffer on the session stream. Full device sync so the
            // H2D write is complete and visible to the session stream's read.
            cudaDeviceSynchronize();
            {
                auto _t = std::chrono::high_resolution_clock::now();
                _rc_deser = std::chrono::duration<double>(_t - _rc_t0).count();
                _rc_t0 = _t;
            }
            // Re-allocate the input ct's data on the ACTIVE SESSION STREAM and
            // D2D-copy the deserialized coefficients into it. populate_ciphertext
            // allocates the buffer on the default stream (cuda_auto_ptr's
            // cudaStream_ = 0); the kernel's eval runs on the session stream and
            // reads ct.data() stream-ordered against that stream's allocations.
            // A default-stream-allocated buffer made eval diverge from the
            // run_session baseline (Prepare_input encrypts on the session
            // stream) by ~0.3 on GEMM / ~8 on resnet20, even though decrypt of
            // the same ct was bit-exact. Rebuilding the buffer on the session
            // stream closes the cross-stream allocation gap; eval then matches.
            auto in_acc = Backend::access_ciphertext(*ct);
            cudaStream_t sess = Backend::current_stream_value();
            Ciphertext* laundered = new Ciphertext();
            laundered->resize(static_cast<size_t>(in_acc.size), static_cast<size_t>(in_acc.coeff_modulus_size),
                              static_cast<size_t>(in_acc.poly_modulus_degree), sess);
            cudaMemcpyAsync(laundered->data(), in_acc.gpu_data, in_acc.data_size, cudaMemcpyDeviceToDevice, sess);
            laundered->set_chain_index(static_cast<size_t>(in_acc.chain_index));
            laundered->set_scale(in_acc.scale);
            laundered->set_poly_modulus_degree(static_cast<size_t>(in_acc.poly_modulus_degree));
            laundered->set_coeff_modulus_size(static_cast<size_t>(in_acc.coeff_modulus_size));
            laundered->set_ntt_form(in_acc.is_ntt_form);
            laundered->SetNoiseScaleDeg(static_cast<size_t>(in_acc.noise_scale_deg));
            laundered->SetAsymmetric(in_acc.is_asymmetric);
            cudaStreamSynchronize(sess);
            delete ct;
            ct = laundered;
            Io_set_input(input_names[i], 0, ct);
            {
                auto _t = std::chrono::high_resolution_clock::now();
                _rc_launder = std::chrono::duration<double>(_t - _rc_t0).count();
                _rc_t0 = _t;
            }
        }

        // Run the kernel (caller-supplied Main_graph — strong def from the
        // dlopened kernel .so, no weak-interposition assumption).
        if (kernel_fn == nullptr)
            throw std::runtime_error("RunCipher: kernel_fn null");
        if (!kernel_fn())
            throw std::runtime_error("RunCipher: Main_graph returned false");

        // The kernel submitted its work on the active session's CUDA stream.
        // We read back the output ciphertext from the GPU (D2H) on the default
        // stream below, so force a full device sync first to ensure the
        // kernel's writes are complete and visible.
        cudaDeviceSynchronize();
        {
            auto _t = std::chrono::high_resolution_clock::now();
            _rc_kernel = std::chrono::duration<double>(_t - _rc_t0).count();
            _rc_t0 = _t;
        }

        // Read the output ciphertext and serialize it.
        if (output_name == nullptr)
            throw std::runtime_error("RunCipher: output_name null");
        void* out_void = Io_get_output(output_name, 0);
        if (out_void == nullptr)
            throw std::runtime_error("RunCipher: Io_get_output returned null for " + std::string(output_name));
        auto* out_ct = static_cast<Ciphertext*>(out_void);
#ifdef ACE_CIPHER_TRACE
        {  // Diagnostic: dump the eval output ct's fields. Compare against a
            // run_session baseline run (which logs the same output fields). If
            // the output fields already differ here, the eval forked inside
            // kernel_fn(); if they match but serialized-decrypt diverges, the
            // fork is in output serialize/decrypt.
            auto a = Backend::access_ciphertext(*out_ct);
            spdlog::warn("[trace] RunCipher EVAL output name='{}' size={} N={} L={} chain={} "
                         "scale={} ntt={} nsd={} asym={} gpuptr={}",
                         output_name, a.size, a.poly_modulus_degree, a.coeff_modulus_size, a.chain_index, a.scale,
                         a.is_ntt_form, a.noise_scale_deg, a.is_asymmetric, static_cast<const void*>(a.gpu_data));
        }
#endif
        size_t need = SerT::buffer_length(*out_ct);
        long ret = static_cast<long>(need);
        if (need <= out_cap && out_buf != nullptr) {
            // Single-pass caller (out_buf given, no cached pass1): serialize now.
            if (!SerT::serialize(*out_ct, out_buf, need)) {
                delete out_ct;
                throw std::runtime_error("RunCipher: serialize output failed");
            }
            delete out_ct;  // Io_get_output does not delete; we own it here.
        }
        else if (out_buf == nullptr) {
            // pass1 (size query): cache the eval output for pass2 to reuse, so
            // pass2 does NOT re-run the kernel. Don't delete — pass2 owns+deletes.
            _cached = out_ct;
        }
        else {
            // out_buf != nullptr but cap < need (caller under-sized despite pass1):
            // can't write; drop the ct (can't keep for pass2 — pass1 already cached
            // when out_buf==null; this branch only triggers on a misuse). Return need.
            delete out_ct;
        }
        _rc_serial = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - _rc_t0).count();
        if (_rc_timing) {
            spdlog::warn("[runcipher] run_cipher split (session={}): deser+sync={:.3f}s  "
                         "launder+sync={:.3f}s  KERNEL(eval)={:.3f}s  serialize={:.3f}s  total≈{:.3f}s",
                         session_idx, _rc_deser, _rc_launder, _rc_kernel, _rc_serial,
                         _rc_deser + _rc_launder + _rc_kernel + _rc_serial);
        }
        // Always capture so the server can return per-stage timings to the client
        // via Kms_last_runcipher_timing() (server & client are separate envs —
        // the client can't see server logs).
        g_last_runcipher_timing = {_rc_deser, _rc_launder, _rc_kernel, _rc_serial};
        return ret;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] RunCipher error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] RunCipher error: unknown exception");
        return -1;
    }
}

/// Same-origin helper (verification use): encrypt `n_elems` plaintext doubles
/// into a serialized ciphertext at the model's input level/scale, using the
/// active session's encoder + encryptor (requires SK/PK → PRELOAD_FULL /
/// ON_DEMAND_FULL). Mirrors FheContext::Prepare_input but returns bytes.
/// Two-pass out-buf contract like RunCipher.
template <typename Backend>
long EncryptInput(int session_idx, const char* input_name, const double* plain, size_t n_elems, char* out_buf,
                  size_t out_cap)
{
    using Ciphertext = typename Backend::Ciphertext;
    using Plaintext = typename Backend::Plaintext;
    using SerT = fhert::CiphertextSerializer<Backend>;

    try {
        Kms_make_current_on_this_thread();
        auto& ctx = FheContext<Backend>::Current();
        ctx.Activate_session_for_this_thread(session_idx);
        if (!ctx.can_verify())
            throw std::runtime_error("EncryptInput: session has no SK/PK (use PRELOAD_FULL/ON_DEMAND_FULL)");

        CKKS_PARAMS* prog = Get_context_params();
        if (prog == nullptr)
            throw std::runtime_error("EncryptInput: null context params");
        int chain_index = static_cast<int>(ctx.num_prime_parts()) - prog->_input_level;
        double encode_scale = std::pow(2.0, static_cast<double>(ctx.scaling_mod_size()));

        std::vector<double> vec(plain, plain + n_elems);
        Plaintext pt;
        ctx.cur_evaluator().eval->encoder.encode(vec, chain_index, encode_scale, pt);
        Ciphertext ct;
        ctx.session(Backend::active_session_idx()).crypto().encryptor->encrypt(pt, ct);

        // encrypt ran on the session stream; serialize's D2H (in CiphertextSerializer)
        // runs on the default stream. Sync the device so encrypt's GPU writes
        // are complete and visible before D2H reads them.
        cudaDeviceSynchronize();

        size_t need = SerT::buffer_length(ct);
        long ret = static_cast<long>(need);
        if (need <= out_cap && out_buf != nullptr) {
            if (!SerT::serialize(ct, out_buf, need))
                throw std::runtime_error("EncryptInput: serialize failed");
        }
        return ret;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] EncryptInput error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] EncryptInput error: unknown exception");
        return -1;
    }
}

/// Diagnostic: encrypt plain then immediately decrypt (NO serialization between).
/// Isolates helper-internal encrypt/decrypt fidelity from the serialize layer.
template <typename Backend>
long EncryptDecryptPlain(int session_idx, const double* plain, size_t n_elems, double* out_doubles,
                         size_t out_cap_doubles)
{
    using Ciphertext = typename Backend::Ciphertext;
    using Plaintext = typename Backend::Plaintext;

    try {
        Kms_make_current_on_this_thread();
        auto& ctx = FheContext<Backend>::Current();
        ctx.Activate_session_for_this_thread(session_idx);
        if (!ctx.can_verify())
            throw std::runtime_error("EncryptDecryptPlain: no SK/PK");

        CKKS_PARAMS* prog = Get_context_params();
        if (prog == nullptr)
            throw std::runtime_error("EncryptDecryptPlain: null params");
        int chain_index = static_cast<int>(ctx.num_prime_parts()) - prog->_input_level;
        double encode_scale = std::pow(2.0, static_cast<double>(ctx.scaling_mod_size()));

        std::vector<double> vec(plain, plain + n_elems);
        Plaintext pt;
        ctx.cur_evaluator().eval->encoder.encode(vec, chain_index, encode_scale, pt);
        Ciphertext ct;
        ctx.session(Backend::active_session_idx()).crypto().encryptor->encrypt(pt, ct);

        Plaintext pt2;
        ctx.session(Backend::active_session_idx()).crypto().decryptor->decrypt(ct, pt2);
        std::vector<double> out_vec;
        ctx.cur_evaluator().eval->encoder.decode(pt2, out_vec);

        long ret = static_cast<long>(out_vec.size());
        if (out_doubles != nullptr && static_cast<size_t>(ret) <= out_cap_doubles) {
            std::copy(out_vec.begin(), out_vec.end(), out_doubles);
        }
        return ret;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] EncryptDecryptPlain error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] EncryptDecryptPlain error: unknown exception");
        return -1;
    }
}

/// Diagnostic: encrypt → access fields → serialize → deserialize → access
/// fields again. Logs both sets so we can see which field the serialize layer
/// fails to round-trip. Returns 8 (number of field slots) on success.
template <typename Backend>
long SerializeFieldsDiag(int session_idx, const double* plain, size_t n_elems, double* out_fields, size_t out_cap)
{
    using Ciphertext = typename Backend::Ciphertext;
    using Plaintext = typename Backend::Plaintext;
    using SerT = fhert::CiphertextSerializer<Backend>;

    try {
        Kms_make_current_on_this_thread();
        auto& ctx = FheContext<Backend>::Current();
        ctx.Activate_session_for_this_thread(session_idx);
        if (!ctx.can_verify())
            throw std::runtime_error("SerializeFieldsDiag: no SK/PK");

        CKKS_PARAMS* prog = Get_context_params();
        int chain_index = static_cast<int>(ctx.num_prime_parts()) - prog->_input_level;
        double encode_scale = std::pow(2.0, static_cast<double>(ctx.scaling_mod_size()));
        std::vector<double> vec(plain, plain + n_elems);
        Plaintext pt;
        ctx.cur_evaluator().eval->encoder.encode(vec, chain_index, encode_scale, pt);
        Ciphertext ct;
        ctx.session(Backend::active_session_idx()).crypto().encryptor->encrypt(pt, ct);

        auto a0 = Backend::access_ciphertext(ct);
        spdlog::warn("[diag] ORIG  size={} N={} L={} chain={} scale={} ntt={} nsd={} asym={}", a0.size,
                     a0.poly_modulus_degree, a0.coeff_modulus_size, a0.chain_index, a0.scale, a0.is_ntt_form,
                     a0.noise_scale_deg, a0.is_asymmetric);

        // The encrypt ran on the session stream; serialize's D2H runs on the
        // default stream. Sync the device so the encrypt's GPU writes are
        // complete before D2H reads them (default-stream read of session-stream
        // writes is otherwise unordered).
        cudaDeviceSynchronize();

        size_t need = SerT::buffer_length(ct);
        std::vector<char> buf(need, '\0');
        if (!SerT::serialize(ct, buf.data(), need))
            throw std::runtime_error("diag: serialize failed");
        cudaStreamSynchronize(0);

        Ciphertext ct2;
        if (!SerT::deserialize(buf.data(), need, ct2, nullptr, nullptr))
            throw std::runtime_error("diag: deserialize failed");
        cudaStreamSynchronize(0);

        auto a1 = Backend::access_ciphertext(ct2);
        spdlog::warn("[diag] RNDTR size={} N={} L={} chain={} scale={} ntt={} nsd={} asym={}", a1.size,
                     a1.poly_modulus_degree, a1.coeff_modulus_size, a1.chain_index, a1.scale, a1.is_ntt_form,
                     a1.noise_scale_deg, a1.is_asymmetric);

        if (out_fields != nullptr && out_cap >= 8) {
            // packed roundtrip field values for the caller: same 8 fields, post-deserialize.
            out_fields[0] = (double)a1.size;
            out_fields[1] = (double)a1.poly_modulus_degree;
            out_fields[2] = (double)a1.coeff_modulus_size;
            out_fields[3] = (double)a1.chain_index;
            out_fields[4] = (double)a1.scale;
            out_fields[5] = (double)a1.is_ntt_form;
            out_fields[6] = (double)a1.noise_scale_deg;
            out_fields[7] = (double)a1.is_asymmetric;
        }
        return 8;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] SerializeFieldsDiag error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] SerializeFieldsDiag error: unknown exception");
        return -1;
    }
}

/// Diagnostic: serialize the ciphertext currently in the Io input slot `name`
/// (populated by a prior Prepare_input / run_session). Used to capture the
/// rtlib-encrypted input and feed it to run_cipher, isolating encrypt-path
/// divergence from eval-path divergence.
template <typename Backend> long SerializeIoInput(int session_idx, const char* name, char* out_buf, size_t out_cap)
{
    using Ciphertext = typename Backend::Ciphertext;
    using SerT = fhert::CiphertextSerializer<Backend>;

    try {
        Kms_make_current_on_this_thread();
        auto& ctx = FheContext<Backend>::Current();
        ctx.Activate_session_for_this_thread(session_idx);

        void* p = Io_get_input(name, 0);
        if (p == nullptr)
            throw std::runtime_error(std::string("SerializeIoInput: Io_get_input null for ") + name);
        auto* ct = static_cast<Ciphertext*>(p);
        // The Io input was written by rtlib Prepare_input on the session stream;
        // serialize's D2H runs on the default stream — sync first.
        cudaDeviceSynchronize();
        size_t need = SerT::buffer_length(*ct);
        long ret = static_cast<long>(need);
        if (need <= out_cap && out_buf != nullptr) {
            if (!SerT::serialize(*ct, out_buf, need))
                throw std::runtime_error("SerializeIoInput: serialize failed");
        }
        return ret;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] SerializeIoInput error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] SerializeIoInput error: unknown exception");
        return -1;
    }
}

/// Same-origin helper (verification use): deserialize an output ciphertext and
/// decrypt+decode into a host double buffer. Mirrors Handle_output. Returns the
/// number of doubles written (or needed, if > out_cap_doubles); <0 on error.
template <typename Backend>
long DecryptOutput(int session_idx, const char* output_name, const char* in_buf, size_t in_size, double* out_doubles,
                   size_t out_cap_doubles)
{
    using Ciphertext = typename Backend::Ciphertext;
    using Plaintext = typename Backend::Plaintext;
    using SerT = fhert::CiphertextSerializer<Backend>;

    try {
        Kms_make_current_on_this_thread();
        auto& ctx = FheContext<Backend>::Current();
        ctx.Activate_session_for_this_thread(session_idx);
        if (!ctx.can_verify())
            throw std::runtime_error("DecryptOutput: session has no SK/PK");

        Ciphertext ct;
        if (!SerT::deserialize(in_buf, in_size, ct, nullptr, nullptr))
            throw std::runtime_error("DecryptOutput: deserialize failed");

        Plaintext pt;
        ctx.session(Backend::active_session_idx()).crypto().decryptor->decrypt(ct, pt);
        std::vector<double> vec;
        ctx.cur_evaluator().eval->encoder.decode(pt, vec);

        long ret = static_cast<long>(vec.size());
        if (out_doubles != nullptr && static_cast<size_t>(ret) <= out_cap_doubles) {
            std::copy(vec.begin(), vec.end(), out_doubles);
        }
        return ret;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] DecryptOutput error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] DecryptOutput error: unknown exception");
        return -1;
    }
}

/// Save evaluation keys (RLK + GK) only — no SK/PK.
///
/// @return 0 on success, -1 on failure or thrown exception.
template <typename Backend> int SaveEvalKeys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts)
{
    try {
        auto& ctx = FheContext<Backend>::Current();
        std::vector<uint32_t> elts;
        if (galois_elts && num_elts > 0) {
            elts.assign(galois_elts, galois_elts + num_elts);
        }
        bool ok = KeySerializer<Backend>::Save_eval_keys(std::string(key_dir), ctx.key_mgr().relin_key(),
                                                         ctx.key_mgr().galois_key(), *ctx.crypto_context(), elts,
                                                         ctx.key_mgr().effective_io_policy());
        return ok ? 0 : -1;
    }
    catch (const std::exception& e) {
        spdlog::error("[kms] SaveEvalKeys error: {}", e.what());
        return -1;
    }
    catch (...) {
        spdlog::error("[kms] SaveEvalKeys error: unknown exception");
        return -1;
    }
}

/// Load evaluation keys (RLK + GK) only — prefer KeyMode::PRELOAD via
/// Set_key_mode() for the normal flow; this C entry exposes the advanced path.
///
/// @return 0 on success, -1 on failure or thrown exception. The historical
/// phantom body was a placeholder that always returned 0; this canonicalizes
/// the signature but logs that the PRELOAD path is preferred.
template <typename Backend> int LoadEvalKeys(const char* key_dir)
{
    try {
        (void)key_dir;
        spdlog::warn("[kms] LoadEvalKeys: prefer KeyMode::PRELOAD via "
                     "Set_key_mode() for normal eval-only key loading.");
        return 0;
    }
    catch (...) {
        return -1;
    }
}

/// Query whether keys were loaded from KMS vs generated at runtime.
///
/// @return 1 if loaded from KMS, 0 otherwise (never throws — the read is
/// trivial and not wrapped so a missing context surfaces as the assert in
/// Current() rather than a swallowed -1).
template <typename Backend> int KeysLoadedFromKms()
{
    return FheContext<Backend>::Current().key_mgr().keys_loaded_from_kms() ? 1 : 0;
}

/// Query the wall-clock seconds of the most recent galois-key load.
///
/// @return seconds (>= 0) on a successful load; -1.0 before any load or on
/// load failure. Reads the KeyMgr instance field set by Load_keys.
template <typename Backend> double GetGaloisLoadTimeSeconds()
{
    return FheContext<Backend>::Current().key_mgr().galois_load_time_seconds();
}

}  // namespace fhert

#endif  // FHE_KMS_KMS_API_H