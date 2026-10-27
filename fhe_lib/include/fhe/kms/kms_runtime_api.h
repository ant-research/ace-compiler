//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file kms_runtime_api.h
//! @brief KMS dlsym contract resolved by `KERNEL_RUNNER` from the loaded
//!        backend `.so`.
//!
//! These six C-linkage symbols are the KMS analogue of the lifecycle dlsym
//! contract (`Prepare_context`, `Finalize_context`, `Prepare_input`,
//! `Handle_output`, `Main_graph` in `rt_api.h`). Each backend `.so` exports
//! them (implemented in its `api.cu` as a thin instantiation of the
//! `fhert::` templates in `fhe/kms/kms_api.h`); `KERNEL_RUNNER` resolves them
//! via `dlsym` on the same `_handle` it already uses for the lifecycle
//! symbols, and exposes them to Python through the `_C` / `FHERuntime` pybind
//! surface.
//!
//! Names are prefixed `Kms_` (not bare `Save_keys`/`Load_keys`) because the
//! dispatch headers `rt_<backend>.h` already define inline C++ `Save_keys` /
//! `Load_keys` wrappers for generated-kernel codegen; the bare names would
//! collide. `Kms_*` is unambiguous and marks these as the KernelRunner
//! runtime contract.
//!
//! Architectural rule: Python never reaches these via `ctypes.CDLL` /
//! `FHert_*` / `Phantom_*` aliases — those parallel export channels were
//! retired. The active backend is determined by which kernel `.so`
//! `KERNEL_RUNNER` dlopened, not by a symbol-name prefix.
//!
//! This header lives in `fhe_lib/` (NOT the legacy `rtlib/`), so the legacy
//! rtlib baseline is untouched and there is no new rtlib contract entry.
//!
//! @return convention: the `int` functions return `0` on success and `-1` on
//! failure; `Kms_keys_loaded_from_kms` returns `1`/`0`;
//! `Kms_get_galois_load_time_seconds` returns seconds (-1.0 before any load).

#ifndef FHE_KMS_KMS_RUNTIME_API_H
#define FHE_KMS_KMS_RUNTIME_API_H

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    //! Save all keys (SK + PK + RLK + GK) to `key_dir`.
    int Kms_save_keys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts);

    //! Load all keys from `key_dir`.
    int Kms_load_keys(const char* key_dir);

    //! Save evaluation keys (RLK + GK) only — no SK/PK.
    int Kms_save_eval_keys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts);

    //! Load evaluation keys (RLK + GK) only.
    int Kms_load_eval_keys(const char* key_dir);

    //! Return 1 if keys were loaded from KMS, 0 if generated at runtime.
    int Kms_keys_loaded_from_kms();

    //! Wall-clock seconds of the most recent galois-key load (-1.0 if none / failed).
    double Kms_get_galois_load_time_seconds();

    //! Create an additional session sharing the current context, loading a full
    //! key bundle from `key_dir` (multi-user Mode B). Returns the session index
    //! (>=1) on success, -1 on failure.
    int Kms_create_session(const char* key_dir);

    //! Switch the active session by index: 0 = primary, >=1 = extra session.
    //! Returns 0 on success, -1 on out-of-range/failure.
    int Kms_switch_session(int idx);

    //! Mode B-P: activate session `idx` on the CALLING worker thread — sets the
    //! thread-local active session index (evaluator + key routing), makes this
    //! context the thread-local current context, and binds backend per-thread
    //! state (phantom: per-session CUDA stream; ace: device context). Must be
    //! called on each worker thread before Main_graph(). Returns 0/-1.
    int Kms_activate_session(int idx);

    //! Release all galois-key slots for session idx (free their HBM). The
    //! session structure (evaluator/GaloisTool/BtsMgr) stays intact; only galois
    //! key bytes are evicted. Ensure_galois_key re-loads from disk on next use.
    //! Called by the service layer when a user is done. Returns 0/-1.
    int Kms_release_session(int idx);

    //! Query how many sessions are currently resident (primary + extra).
    //! @return number of sessions (>=1), or -1 on error.
    int Kms_num_sessions();

    //! Mode B-P: bind the single globally-owned FheContext to the CALLING
    //! thread as its current context. Worker pthreads start with a null
    //! thread-local current ctx; call this before Kms_activate_session /
    //! any op. Returns 0/-1.
    int Kms_make_current_on_this_thread();

    //! Ciphertext-in / ciphertext-out inference (service "step 2"). Accepts
    //! already-encrypted input ciphertext bytes (client-encrypted), evals with
    //! the active session's keys (no server-side encrypt/decrypt), and returns
    //! the serialized output-ciphertext bytes.
    //!
    //! `kernel_fn` is the runtime-dlsym'd `Main_graph` (passed explicitly to
    //! avoid relying on weak-symbol interposition from a load-order assumption).
    //! `input_names` = Io keys (runtime derives from get_encode_scheme_c(i)
    //! ->_name); backend uses them as `Io_set_input(name, 0, ct)` keys.
    //! `out_buf`/`out_cap` = caller-allocated output buffer. Returns the needed
    //! output byte count on success (>0); if it exceeds `out_cap` the buffer is
    //! NOT written and the needed size is still returned (caller retried with a
    //! bigger cap). Returns <0 on error.
    long Kms_run_cipher(int session_idx, bool (*kernel_fn)(), const char* const* input_names,
                        const char* const* input_bufs, const size_t* input_sizes, size_t n_inputs, char* out_buf,
                        size_t out_cap, const char* output_name);

    //! Same-origin helper (verification use): encrypt a plaintext vector into a
    //! serialized ciphertext at the model's input level/scale, using the active
    //! session's encoder+encryptor (requires SK/PK — i.e. PRELOAD_FULL /
    //! ON_DEMAND_FULL session). Returns the needed byte count (same two-pass
    //! contract as Kms_run_cipher); <0 on error. Mirrors FheContext::Prepare_input.
    long Kms_encrypt_input(int session_idx, const char* input_name, const double* plain, size_t n_elems, char* out_buf,
                           size_t out_cap);

    //! Same-origin helper (verification use): deserialize an output ciphertext
    //! and decrypt+decode into a host double buffer. Returns the number of
    //! doubles written (<= out_cap_doubles); if it exceeds out_cap_doubles the
    //! needed count is still returned; <0 on error. Mirrors Handle_output.
    long Kms_decrypt_output(int session_idx, const char* output_name, const char* in_buf, size_t in_size,
                            double* out_doubles, size_t out_cap_doubles);

    //! Diagnostic: encrypt plain → decrypt (no serialization in between). Used
    //! to isolate helper-internal encrypt/decrypt fidelity from the serialize
    //! layer. Returns the number of doubles written (or needed); <0 on error.
    long Kms_encrypt_decrypt_plain(int session_idx, const double* plain, size_t n_elems, double* out_doubles,
                                   size_t out_cap_doubles);

    //! Diagnostic: encrypt → serialize → deserialize, then return the field
    //! values of the deserialized ciphertext as packed doubles so the caller
    //! can compare against the encrypt-time values. out holds (in order):
    //! after_field[0..7] = size, N, L, chain_index, scale, is_ntt_form,
    //!                     noise_scale_deg, is_asymmetric
    //! Returns 8 on success, <0 on error. (scale is cast to double.)
    long Kms_serialize_fields_diag(int session_idx, const double* plain, size_t n_elems, double* out_fields,
                                   size_t out_cap);

    //! Diagnostic: serialize the ciphertext currently stored in the Io input
    //! slot `name` (populated by a prior Prepare_input / run_session). Lets a
    //! caller capture the rtlib-encrypted input and feed it to run_cipher, to
    //! isolate whether run_cipher's eval divergence comes from the encrypt path
    //! (Kms_encrypt_input vs rtlib Prepare_input) vs the eval path. Two-pass
    //! out-buf contract; <0 on error.
    long Kms_serialize_io_input(int session_idx, const char* name, char* out_buf, size_t out_cap);

    //! Fill out[4] with the last RunCipher call's per-stage seconds on THIS
    //! thread: out[0]=deser+sync, out[1]=launder+sync, out[2]=kernel(eval),
    //! out[3]=serialize. Returns 0 on success, <0 on error. Lets the server
    //! echo the per-stage split to the client (server/client are separate
    //! environments — the client can't read server logs).
    int Kms_last_runcipher_timing(double* out);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // FHE_KMS_KMS_RUNTIME_API_H