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
//! Replaces the monolithic ace::ACE_CONTEXT with the common template framework.

#include "ace/he_engine/host/ckks.h"
#include "ace/he_engine/host/decryptor.h"
#include "ace/he_engine/host/encryptor.h"
#include "ace/he_engine/host/keygenerator.h"
#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"
#include "common/tensor.h"
#include "common/trace.h"
#include "fhe/context.h"
#include "fhe/need_bts.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/ace/ace_config.h"
#include "fhe/vendor/ace/bts_mgr.h"
#include "fhe/vendor/ace/vendor_traits.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#ifndef ACE_LIBRARY_DEBUG
#define ACE_LIBRARY_DEBUG 0
#endif

using namespace ace;

// Use Traits type aliases so specializations match the template base signatures.
using Ciphertext = ace::VendorTraits::Ciphertext;
using Plaintext = ace::VendorTraits::Plaintext;
using ScaleType = ace::VendorTraits::ScaleType;
using LevelType = ace::VendorTraits::LevelType;

#ifdef ACE_LIBRARY_32
#define ADJUST_LEVEL_REDUCE_ERROR
#endif

namespace
{

/// Convert logical level (1-based from compiler) to ACE runtime level.
/// ACE_LIBRARY_32 uses double-rescale: 2*level - 1.
/// Standard mode uses: level - 1.
inline uint64_t Runtime_level(int level)
{
    FMT_ASSERT(level > 0, "Runtime_level: logical level must be >= 1, got %d", level);
#ifdef ACE_LIBRARY_32
    return static_cast<uint64_t>(2 * level - 1);
#else
    return static_cast<uint64_t>(level - 1);
#endif
}

constexpr bool kAceLibraryDebug = ACE_LIBRARY_DEBUG != 0;

FILE* Ace_debug_file()
{
    return Is_trace_on() ? Get_trace_file() : stderr;
}

FILE* Ace_dump_file()
{
    return Is_trace_on() ? Get_trace_file() : stdout;
}

}  // namespace

namespace fhert
{

//=============================================================================
// FheContext<VendorTraits> — Constructor & Destructor
//=============================================================================

template <>
FheContext<ace::VendorTraits>::FheContext(KeyMode key_mode, EncPolicy enc_policy) : _enc_policy(enc_policy), _params()
{

    using Traits = ace::VendorTraits;

    bool with_bootstrap = Need_bts();
    CKKS_PARAMS* prog_param = Get_context_params();
    IS_TRUE(static_cast<int>(prog_param->_provider) == LIB_ACE, "provider is not ACE");

    _params.scaling_mod_size = prog_param->_scaling_mod_size;

    if (with_bootstrap) {
        // --- Bootstrap path ---
        // 1. BtsMgr creates the HostContext with bootstrap-specific
        //    coeff_modulus and sec_level=none. This HostContext has correct
        //    parameters that pass parameters_set() validation.
        bool use_sparse_wrapping = false;
        acelib::host::HostContext* bts_host_ctx = nullptr;
        _bootstrap.Init_bts_cpu_context(prog_param, use_sparse_wrapping, &bts_host_ctx);
        IS_TRUE(bts_host_ctx != nullptr, "Bootstrap HostContext creation failed");
        IS_TRUE(bts_host_ctx->parameters_set(), "Bootstrap HostContext parameters not set");

        // 2. FheContext takes ownership of the bootstrap HostContext.
        _crypto_ctx.reset(bts_host_ctx);

        // 3. Bind session to context and key mode (set_context done by constructor).
        _session = FheSession<Traits>(_crypto_ctx.get(), key_mode);

        // 4. Create a KeyGenerator with the correct force_dense_secret flag.
        //    When use_sparse_wrapping is enabled, we need force_dense_secret=true
        //    so that create_sparse_wrapping_keys() works correctly.
        //    We use this single KeyGenerator for ALL key generation to ensure
        //    consistency (SK, PK, RLK, and later galois keys all come from
        //    the same KeyGenerator).
        spdlog::info("[kms] HostContext provided externally (N={})", (unsigned)Traits::poly_degree(*_crypto_ctx));
        spdlog::info("[kms] Generating keys (sk, pk, rlk)...");

        auto kgen = std::make_unique<acelib::host::KeyGenerator>(*_crypto_ctx,
                                                                 use_sparse_wrapping /* force_dense_secret */);

        auto sk = std::make_unique<acelib::host::SecretKey>();
        *sk = kgen->secret_key();
        auto pk = std::make_unique<acelib::host::PublicKey>();
        kgen->create_public_key(*pk);
        auto rlk = std::make_unique<acelib::host::RelinKeys>();
        kgen->create_relin_keys(*rlk);
        auto rtk = std::make_unique<acelib::host::GaloisKeys>();

        _session.key_mgr().set_keys(std::move(sk), std::move(pk), std::move(rlk), std::move(rtk));
        spdlog::info("[kms] Keys generated.");

        // 5. Create encoder from the bootstrap HostContext.
        _evaluator.encoder = Traits::create_encoder(*_crypto_ctx);

        // 6. Create encryptor and decryptor from the bootstrap HostContext.
        //    Encryptor needs both PK and SK for symmetric encryption (encrypt_symmetric).
        _session.crypto().encryptor.reset(new acelib::host::Encryptor(*_crypto_ctx, _session.key_mgr().public_key(),
                                                                      _session.key_mgr().secret_key()));
        _session.crypto().decryptor.reset(new acelib::host::Decryptor(*_crypto_ctx, _session.key_mgr().secret_key()));

        // 7. Build galois steps (deduplicated for bootstrap).
        std::vector<int> gal_steps_vector;
        for (int i = 0; i < prog_param->_num_rot_idx; ++i) {
            int rot = prog_param->_rot_idxs[i];
            if (std::find(gal_steps_vector.begin(), gal_steps_vector.end(), rot) == gal_steps_vector.end()) {
                gal_steps_vector.push_back(rot);
            }
        }

        // 8. Initialize bootstrap evaluator. This creates the GPU evaluator
        //    with bootstrap support (EvalWrapper + EvalBootstrap) and returns
        //    the evaluator and context pointers.
        acelib::Evaluator* bts_eval = nullptr;
        acelib::ContextPro* bts_ctx = nullptr;

        _bootstrap.Init_bts_evaluator_components(*_evaluator.encoder, _session.key_mgr().secret_key(),
                                                 _session.key_mgr().public_key(), _session.key_mgr().relin_key(),
                                                 _session.key_mgr().galois_key(), *kgen, bts_eval, bts_ctx,
                                                 acelib::MEMORY_LAYOUT::ROW_MAJOR, gal_steps_vector);

        IS_TRUE(bts_eval != nullptr, "Bootstrap evaluator creation failed");
        IS_TRUE(bts_ctx != nullptr, "Bootstrap context is null");

        // Set the non-owning evaluator pointer and context.
        // The bootstrap EvalWrapper is owned by BtsMgr; we keep a
        // non-owning pointer via eval_ptr so active_eval() returns it.
        _evaluator.eval_ptr = bts_eval;
        _evaluator.ctx = bts_ctx;

        // Set device context singleton for EncIo
        Traits::set_device_context(_evaluator.ctx);
    }
    else {
        // --- Non-bootstrap path ---
        // 1a. Create HostContext (owned by FheContext).
        _crypto_ctx = Traits::create_context(with_bootstrap);

        // 1b. Bind session to context and key mode (set_context done by constructor).
        _session = FheSession<Traits>(_crypto_ctx.get(), key_mode);

        // 1c. Initialize key manager (loads or generates keys based on mode).
        _session.key_mgr().Init(*_crypto_ctx, with_bootstrap);

        // Key mode is stored in KeyMgr; access via session.key_mode().

        // 2. Create encoder.
        _evaluator.encoder = Traits::create_encoder(*_crypto_ctx);

        // 3. Create GPU evaluator.
        _evaluator.eval = std::make_unique<acelib::Evaluator>(acelib::ContextPro::get_device_context(*_crypto_ctx),
                                                              true);
        _evaluator.eval->enable_memory_pool(0.4);
        _evaluator.eval_ptr = _evaluator.eval.get();  // also set non-owning ptr
        _evaluator.ctx = const_cast<acelib::ContextPro*>(&(_evaluator.eval->context()));

        // Set device context singleton for EncIo
        Traits::set_device_context(_evaluator.ctx);

        // 4. Create encryptor and decryptor.
        _session.crypto().encryptor.reset(new acelib::host::Encryptor(*_crypto_ctx, _session.key_mgr().public_key()));
        _session.crypto().decryptor.reset(new acelib::host::Decryptor(*_crypto_ctx, _session.key_mgr().secret_key()));

        // Mode B-P:ACE has no per-session CUDA stream (host/PSDK); reserve the
        // idx-0 placeholder so _session_stream_owners stays index-aligned with
        // sessions ([0]=primary, [1..N]=extra).
        _session_stream_owners.emplace_back(std::make_unique<typename Traits::SessionStreamStub>());

        // 5. Upload keys to GPU evaluator.
        acelib::MEMORY_LAYOUT data_layout = acelib::MEMORY_LAYOUT::ROW_MAJOR;

        // Set galois steps
        std::vector<int> gal_steps_vector;
        for (int i = 0; i < prog_param->_num_rot_idx; ++i) {
            gal_steps_vector.push_back(prog_param->_rot_idxs[i]);
        }

        // Normal path: upload keys and set up galois keys
        auto kgen = std::make_unique<acelib::host::KeyGenerator>(*_crypto_ctx);
        Traits::create_galois_keys_from_steps(*kgen, _session.key_mgr().galois_key(), gal_steps_vector);
        Traits::upload_keys_to_evaluator(_evaluator, _session.key_mgr().public_key(), _session.key_mgr().secret_key(),
                                         _session.key_mgr().relin_key(), _session.key_mgr().galois_key(), data_layout);

        std::vector<int> bits;
        bits.push_back(prog_param->_first_mod_size);
        for (uint32_t i = 0; i < prog_param->_mul_depth; ++i)
            bits.push_back(prog_param->_scaling_mod_size);
        bits.push_back(prog_param->_first_mod_size);
        _params.num_prime_parts = bits.size();
    }

    _params.slot_count = prog_param->_poly_degree / 2;

    spdlog::info("[ace] ckks_param: _provider={}, _poly_degree={}, _sec_level={}, "
                 "mul_depth={}, _first_mod_size={}, _scaling_mod_size={}, "
                 "_num_q_parts={}, _num_rot_idx={}, _num_prime_parts={}",
                 static_cast<int>(prog_param->_provider), prog_param->_poly_degree, prog_param->_sec_level,
                 prog_param->_mul_depth, prog_param->_first_mod_size, prog_param->_scaling_mod_size,
                 prog_param->_num_q_parts, prog_param->_num_rot_idx, _params.num_prime_parts);

    // --- Initialize enc_mgr ---
    RT_DATA_INFO* data_info = Get_rt_data_info();
    if (data_info != nullptr) {
        _enc_mgr = std::make_shared<EncMgr<ace::VendorTraits>>();
        _enc_mgr->Init(data_info->_file_name, _crypto_ctx.get(), _enc_policy);
    }
}

template <> void FheContext<ace::VendorTraits>::switch_session(FheSession<ace::VendorTraits>& /*session*/)
{
    // Mode B-P: session switching is a per-thread index set (see
    // switch_session_by_idx / set_active_session_idx_for_this_thread). Each
    // session has its OWN evaluator pre-bound to its keys at create_session
    // time, so there is no shared-evaluator key mutation / GPU upload here.
    spdlog::warn("[fhe] switch_session(FheSession&) is a no-op in Mode B-P; use switch_session_by_idx(idx).");
}

template <> int FheContext<ace::VendorTraits>::create_session(const std::string& key_dir)
{
    if (!_crypto_ctx) {
        spdlog::error("[fhe] create_session: no crypto context (call init first)");
        return -1;
    }
    using Traits = ace::VendorTraits;
    const KeyMode km = _session.key_mgr().key_mode();
    auto sess = std::make_unique<FheSession<ace::VendorTraits>>(_crypto_ctx.get(), km);
    if (!sess->key_mgr().Load_keys(key_dir, _session.key_mgr().effective_io_policy())) {
        spdlog::error("[fhe] create_session: Load_keys failed for {}", key_dir);
        return -1;
    }
    IS_TRUE(sess->key_mgr().relin_key_ptr() != nullptr, "create_session: loaded session missing relin keys");
    IS_TRUE(sess->key_mgr().galois_key_ptr() != nullptr, "create_session: loaded session missing galois keys");

    // Mode B-P: build this session's OWN evaluator (sharing the crypto context
    // + NTT/sin-cos tables), and upload THIS session's keys to it once. The ace
    // evaluator's relin/galois are GPU-resident via upload_keys_to_evaluator.
    // (ace's create_evaluator returns a full Evaluator wrapper; the primary
    // path builds eval manually, so we mirror that here for the extra session.)
    typename Traits::Evaluator ev;
    ev.encoder = Traits::create_encoder(*_crypto_ctx);
    ev.eval = std::make_unique<acelib::Evaluator>(acelib::ContextPro::get_device_context(*_crypto_ctx), true);
    ev.eval->enable_memory_pool(0.4);
    ev.eval_ptr = ev.eval.get();
    ev.ctx = const_cast<acelib::ContextPro*>(&(ev.eval->context()));
    if (ev.active_eval()) {
        Traits::upload_keys_to_evaluator(ev, sess->key_mgr().public_key(), sess->key_mgr().secret_key(),
                                         sess->key_mgr().relin_key(), sess->key_mgr().galois_key(),
                                         acelib::MEMORY_LAYOUT::ROW_MAJOR);
    }
    if (sess->key_mgr().can_verify()) {
        sess->crypto().encryptor.reset(
            new acelib::host::Encryptor(*_crypto_ctx, sess->key_mgr().public_key(), sess->key_mgr().secret_key()));
        sess->crypto().decryptor.reset(new acelib::host::Decryptor(*_crypto_ctx, sess->key_mgr().secret_key()));
    }

    // ACE has no per-session CUDA stream (host/PSDK); placeholder owner keeps
    // the multi-session storage uniform across backends.
    _session_stream_owners.emplace_back(std::make_unique<typename Traits::SessionStreamStub>());

    // NOTE: shared GaloisTool is NOT rebuilt — extra sessions must use the SAME
    // galois elements as the primary (true for same-model keys).
    _extra_sessions.push_back(std::move(sess));
    _extra_evaluators.emplace_back(std::move(ev));
    return static_cast<int>(_extra_sessions.size());
}

template <> void FheContext<ace::VendorTraits>::switch_session_by_idx(int idx)
{
    if (idx < 0 || idx >= num_sessions()) {
        spdlog::error("[fhe] switch_session_by_idx: index {} out of range [0, {}]", idx, num_sessions() - 1);
        return;
    }
    set_active_session_idx_for_this_thread(idx);
    Io_bind(idx);  // route Io_set/get_input/output to this session's own per-slot store
}

template <> void FheContext<ace::VendorTraits>::Activate_session_for_this_thread(int idx)
{
    using Traits = ace::VendorTraits;
    IS_TRUE(idx >= 0 && idx < num_sessions(), "Activate_session_for_this_thread: idx out of range");
    set_active_session_idx_for_this_thread(idx);
    Io_bind(idx);  // route Io_set/get_input/output to this session's own per-slot store
    MakeCurrent();
    // ACE per-thread device context is thread_local (defaults nullptr). Bind it
    // to this session's evaluator's ctx so EncIo works on this worker thread.
    auto& ev = (idx == 0) ? _evaluator : _extra_evaluators[idx - 1];
    if (ev.ctx) {
        Traits::set_device_context(ev.ctx);
    }
}

//=============================================================================
// FheContext<VendorTraits> — Encode operations
//=============================================================================

template <>
void FheContext<ace::VendorTraits>::Encode_float(Plaintext& pt, const float* input, size_t len, ScaleType scale,
                                                 LevelType level)
{
    std::vector<double> vec(input, input + len);
    _evaluator.active_eval()->encode(vec, level, std::pow(2.0, _params.scaling_mod_size * scale), pt);
}

template <>
void FheContext<ace::VendorTraits>::Encode_float_cst_lvl(Plaintext& pt, const float* input, size_t len, double scale,
                                                         int level)
{
    std::vector<double> vec(input, input + len);
    auto parm_id = _evaluator.ctx->get_parms_id_from_level(Runtime_level(level));
    _evaluator.active_eval()->encode(vec, parm_id, std::pow(2.0, _params.scaling_mod_size * scale), pt);
}

template <>
void FheContext<ace::VendorTraits>::Encode_float_mask(Plaintext& pt, float input, size_t len, ScaleType scale,
                                                      LevelType level)
{
    std::vector<double> vec(len, input);
    double scale_this_level;
#ifdef ACE_LIBRARY_32
    auto data_context_ptr = _evaluator.ctx->get_context_data(level);
    FMT_ASSERT(data_context_ptr != nullptr, "Encode_float_mask Error: data_context_ptr null!");
    scale_this_level = data_context_ptr->scale_per_level();
#else
    scale_this_level = std::pow(2.0, _params.scaling_mod_size) * scale;
#endif
    _evaluator.active_eval()->encode(vec, level, scale_this_level, pt);
}

template <>
void FheContext<ace::VendorTraits>::Encode_float_mask_cst_lvl(Plaintext& pt, float input, size_t len, double scale,
                                                              int level)
{
    std::vector<double> vec(len, input);
    auto parm_id = _evaluator.ctx->get_parms_id_from_level(Runtime_level(level));
    double scale_this_level;
#ifdef ACE_LIBRARY_32
    auto data_context_ptr = _evaluator.ctx->get_context_data(parm_id);
    FMT_ASSERT(data_context_ptr != nullptr, "Encode_float_mask_cst_lvl Error: data_context_ptr null!");
    scale_this_level = data_context_ptr->scale_per_level();
#else
    scale_this_level = std::pow(2.0, _params.scaling_mod_size) * scale;
#endif
    _evaluator.active_eval()->encode(vec, parm_id, scale_this_level, pt);
}

//=============================================================================
// FheContext<VendorTraits> — HE operations
//=============================================================================

template <> void FheContext<ace::VendorTraits>::Bootstrap(Ciphertext& res, Ciphertext& op1, int level, int slot)
{
    (void)slot;  // reserved for future use
    _bootstrap.Bootstrap(&op1, &res, level);
}

//=============================================================================
// Memory / Scale / Level / Debug / I/O
//=============================================================================

template <> void FheContext<ace::VendorTraits>::Prepare_input(void* input, const char* name)
{
    using Traits = ace::VendorTraits;
    auto* tensor = static_cast<TENSOR*>(input);
    size_t len = TENSOR_SIZE(tensor);
    std::vector<double> vec(tensor->_vals, tensor->_vals + len);

    CKKS_PARAMS* prog_param = Get_context_params();
    uint64_t input_level = prog_param->_input_level;

    // Use ContextPro's parms_id, which is compatible with both the host
    // CKKS encoder/encryptor and the device evaluator (ContextPro shares
    // the same parms_id space as the HostContext it was created from).
    auto parm_id = _evaluator.ctx->get_parms_id_from_level(Runtime_level(input_level));
    acelib::host::Plaintext pt;
    _evaluator.encoder->encode(vec, parm_id, std::pow(2.0, _params.scaling_mod_size), pt);
    acelib::host::Ciphertext ct;
    if (Need_bts()) {
        session(Traits::active_session_idx()).crypto().encryptor->encrypt_symmetric(pt, ct);
    }
    else {
        session(Traits::active_session_idx()).crypto().encryptor->encrypt(pt, ct);
    }
    Ciphertext* gpu_ct =
        new Ciphertext(Ciphertext::from(ct, _evaluator.active_eval()->context(), acelib::MEMORY_LAYOUT::ROW_MAJOR));
    cudaDeviceSynchronize();
    Io_set_input(name, 0, gpu_ct);
}

template <> double* FheContext<ace::VendorTraits>::Handle_output(const char* name, size_t idx)
{
    using Traits = ace::VendorTraits;
    Ciphertext* data = static_cast<Ciphertext*>(Io_get_output(name, idx));
    IS_TRUE(data != nullptr, "not find data");
    acelib::host::Ciphertext host_data;
    acelib::host::Plaintext pt;
    host_data = (*data).to(*_crypto_ctx);
    session(Traits::active_session_idx()).crypto().decryptor->decrypt(host_data, pt);
    std::vector<double> vec;
    _evaluator.encoder->decode(pt, vec);
    double* msg = static_cast<double*>(malloc(sizeof(double) * vec.size()));
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    return msg;
}

}  // namespace fhert
