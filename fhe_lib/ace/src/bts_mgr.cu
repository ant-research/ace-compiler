//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/vendor/ace/bts_mgr.h"

#if ACE_LIBRARY_ENABLE_BTS
#include "NVTXWrapper.h"
#endif

#include "common/common.h"
#include "common/error.h"
#include "common/rt_api.h"
#include "fhe/need_bts.h"
#include "fhe/vendor/ace/ace_config.h"

namespace ace
{

//=============================================================================
// Bootstrap enabled path
//=============================================================================

#if ACE_LIBRARY_ENABLE_BTS

void BtsMgr::Make_bts_parms(CKKS_PARAMS* prog_param, acelib::host::HostParameter& parms_out,
                            EvalBootstrap::BootstrapContext& bts_parms_out)
{
    using config::LTHoistingMode;

    long logN = log2(prog_param->_poly_degree);
    long full_logn = logN - 1;
    int hw = ace::Bts_hamming_weight_or_default(prog_param->_hamming_weight);
#ifndef ACE_LIBRARY_32
    int logq = prog_param->_scaling_mod_size;
    int total_level = prog_param->_mul_depth;
    int evalmod_depth = 8;
    int logp = prog_param->_first_mod_size;
    int log_special_prime = prog_param->_first_mod_size;
#endif

    auto config = bootstrap::BootstrapConfig();
    config.crypto_params.logN = logN;
    config.crypto_params.logSlots = full_logn;
    config.crypto_params.hamming_weight = hw;
    if (config.crypto_params.hamming_weight == 192) {
        config.evalmod_params.n_double_angle = 2;
        config.evalmod_params.degree = 59;
    }
    else if (config.crypto_params.hamming_weight == 32) {
        config.evalmod_params.n_double_angle = 3;
        config.evalmod_params.degree = 22;
    }
    else {
        FMT_ASSERT(false, "ACE bootstrap currently supports only hamming weight "
                          "32 or 192");
    }
    config.evalmod_params.inverse_degree = 1;
    config.SetEvalModLogE(10);
    config.SetHoistingMode(LTHoistingMode::DOUBLE_HOIST);

#ifndef ACE_LIBRARY_32
    int stc_depth = 3;
    int cts_depth = 4;
    int remain_level = total_level - (stc_depth + cts_depth + evalmod_depth);
    // Configure the modulus chain with the new SetGeneratedRnsChain API
    // (the legacy config.crypto_params.logQ0/logQi/level/logP/n_special_prime
    // fields were removed in ace-library). Maps the RemovedRnsChainSpec fields
    // 1:1 from the old scalar fields — mirrors rtlib d2d85df4 adaptation.
    bootstrap::GeneratedRnsChainSpec generated_chain;
    generated_chain.base_q_bits = logp;
    generated_chain.normal_modulus_bits = {logq};
    generated_chain.normal_transition_count = remain_level;
    generated_chain.auxiliary_prime_bits = {log_special_prime};
    generated_chain.auxiliary_prime_count = 8;
    config.SetGeneratedRnsChain(generated_chain);

    config.cts_params.depth = cts_depth;
    config.cts_params.logQ = logp;
    config.stc_params.depth = stc_depth;
    config.stc_params.logQ = logp;
    if (config.cts_params.depth == 4) {
        config.cts_params.groups = {4, 4, 4, 3};
        config.stc_params.groups = {5, 5, 5};
    }

    config.evalmod_params.logQ = logp;
    config.evalmod_params.logE = 10;
    config.EnableHoistedRelin();
    config.EnableScaleAlignment();
    config.EnableLazyRescale(true);
    config.EnableEagerConstantAddition(false);
    config.EnableEvalModMergeMDRS();
    config.SetPolyEvalFusionMode(config::EvalPolyFusionMode::ON);
    config.SetPolyEvalArithMode(config::FusedLeafConstArithMode::MONT_LAZY);

    config.SetBabyStepFusion(config::BabyStepFusion::BATCH_BASIC);
    config.SetMacStrategy(config::BSGSMacFusion::FULL_BATCHING);
    config.SetPreGaloisKeys(true);
    config.SetKeyStrategy(config::LTRotKeyStrategy::FULL_KEYS);
    config.SetFusedBabyMac(true);
    config.optimizations.linear_transform.giant_step_prep_mode = config::GiantStepPrepMode::BATCH_MODDOWN_MODUP;
    config.optimizations.linear_transform.giant_step_rotate_fusion = config::GiantStepRotateFusion::BATCH_PQ;
    config.optimizations.linear_transform.giant_step_reduce_fusion = config::GiantStepReduceFusion::ADD_MANY;
    config.EnableLTMergeMDRS();
    config.EnablePeriodicityOpt(true);
    config.ValidatePeriodicityRuntime(false);

    config.subsum_strategy.method = reduce::ReduceMethod::BSGS;
    config.subsum_strategy.rotation_in_pq_domain = true;
    config.subsum_strategy.impl_mode = reduce::stages::ReduceImplMode::DEFERRED;
    config.subsum_strategy.reduce_fusion = reduce::stages::ReduceFusion::BATCH_2D;
    config.subsum_strategy.baby_step_method = reduce::PhaseMethod::SEQUENTIAL_HOIST;
    config.subsum_strategy.giant_step_method = reduce::PhaseMethod::SEQUENTIAL_HOIST;
    config.subsum_strategy.cipher_add_fusion = true;

    config.msg_scale = pow(2.0, logq);
    config.final_scale = pow(2.0, logq);

    config.SetSlimBootstrap(false);
    config.SetRealInput(true);
    config.SetRemoveImaginary(false);
    config.EnableSparseKeyWrapping(false);
    config.EnableSSEForced(false);

    bool enable_score;
    if (config.slim_mode)
        enable_score = false;
    else
        enable_score = false;

    config.EnableSCORE(enable_score);

    auto bts_parms = EvalBootstrap::generate_bts_context(config);

    const int N = 1 << bts_parms.logN();
    acelib::host::HostParameter parms(acelib::host::scheme_type::ckks);
    parms.set_poly_modulus_degree(N);

    parms.set_coeff_modulus(
        acelib::host::CoeffModulus::Create(bts_parms.rns_chain_config_request().coeff_modulus_primes()));
#else
    const auto modulus_blocks = ace::Bts_set_f_modulus_blocks(prog_param->_input_level);

    config.SetSlimBootstrap(true)
        .SetRealInput(true)
        .SetRemoveImaginary(true)
        .EnableSCORE(false)
        .SkipEvalMod(false)
        .SetArcSinPolyDegree(1)
        .SetCosEven(false)
        .SetHoistingMode(config::LTHoistingMode::DOUBLE_HOIST);

    config.optimizations.linear_transform.compute_mode = config::BSGSComputeMode::FULL;
    config.SetMacStrategy(config::BSGSMacFusion::SEQUENTIAL);
    config.SetPreGaloisKeys(true);
    config.SetBabyStepFusion(config::BabyStepFusion::NONE);
    config.optimizations.linear_transform.giant_step_prep_mode = config::GiantStepPrepMode::NONE;
    config.optimizations.linear_transform.giant_step_rotate_fusion = config::GiantStepRotateFusion::NONE;
    config.optimizations.linear_transform.giant_step_reduce_fusion = config::GiantStepReduceFusion::STREAM_ADD;
    config.SetKeyStrategy(config::LTRotKeyStrategy::LESS_GIANT_KEY);
    config.EnableLTMergeMDRS(false);
    config.EnablePeriodicityOpt(true);
    config.ValidatePeriodicityRuntime(true);

    config.subsum_strategy.method = reduce::ReduceMethod::BINARY;
    config.subsum_strategy.rotation_in_pq_domain = true;

    config.EnableEvalModMergeMDRS(true)
        .EnableHoistedRelin(true)
        .EnableScaleAlignment(true)
        .EnableLazyRescale(true)
        .EnableEagerConstantAddition(false)
        .SetPolyEvalFusionMode(config::EvalPolyFusionMode::OFF)
        .SetPolyEvalArithMode(config::FusedLeafConstArithMode::MONT_EAGER)
        .EnableSSEForced(true)
        .EnableSparseKeyWrapping(false);

    ace::Apply_32bit_bts_modulus_config(config, modulus_blocks, prog_param->_scaling_mod_size);
    if (config.cts_params.depth == 4) {
        config.cts_params.groups = {5, 4, 3, 3};
        config.stc_params.groups = {5, 5, 5};
    }

    auto bts_parms = EvalBootstrap::generate_bts_context(config);

    const int N = 1 << bts_parms.logN();
    acelib::host::HostParameter parms(acelib::host::scheme_type::ckks);
    parms.set_poly_modulus_degree(N);

    parms.set_coeff_modulus(acelib::host::CoeffModulus::Create(ace::Flatten_modulus_blocks(modulus_blocks)));
#endif
    parms.set_secret_key_hamming_weight(config.crypto_params.hamming_weight);
    const long sparse_slots = (1 << bts_parms.logSlots());
    if (sparse_slots != N / 2) {
        parms.set_sparse_slots(sparse_slots);
    }
    // Take auxiliary-prime count from the generated RNS-chain request, mirroring
    // rtlib f158254c. config.crypto_params.n_special_prime is gone on the new
    // ace-library; rns_chain_config_request().special_modulus_size() is the
    // authoritative source.
    const auto& chain_request = bts_parms.rns_chain_config_request();
    parms.set_n_special_primes(chain_request.special_modulus_size());
    parms.set_rns_chain_config(chain_request);

    parms_out = parms;
    bts_parms_out = std::move(bts_parms);
}

void BtsMgr::Init_bts_cpu_context(CKKS_PARAMS* prog_param, bool& use_sparse_wrapping,
                                  acelib::host::HostContext** host_context_out)
{
    acelib::host::HostParameter host_bts_parms(acelib::host::scheme_type::ckks);
    Make_bts_parms(prog_param, host_bts_parms, _bts_parms);
    use_sparse_wrapping = _bts_parms.sparse_key_wrapping();

    auto host_ctx = std::make_unique<acelib::host::HostContext>(host_bts_parms, true,
                                                                acelib::host::sec_level_type::none);
    _host_context = host_ctx.get();

    // Transfer ownership to caller (ACE_CONTEXT will store it)
    *host_context_out = host_ctx.release();
}

void BtsMgr::Init_bts_evaluator_components(acelib::host::CKKSEncoder& encoder, const acelib::host::SecretKey& sk,
                                           const acelib::host::PublicKey& pk, acelib::host::RelinKeys& rlk,
                                           acelib::host::GaloisKeys& rtk, acelib::host::KeyGenerator& kgen,
                                           acelib::Evaluator*& eval_out, acelib::ContextPro*& ctx_out,
                                           acelib::MEMORY_LAYOUT data_layout, const std::vector<int>& gal_steps)
{
    _dev_eval_owner = std::make_unique<EvalWrapper>(encoder, acelib::ContextPro::get_device_context(*_host_context),
                                                    true);
    _dev_eval = _dev_eval_owner.get();

    bool use_sparse_wrapping = _bts_parms.sparse_key_wrapping();
    if (use_sparse_wrapping) {
        _dev_eval->set_sparse_key_wrapping_enabled(true);
    }

    // Bind the evaluator-wide EVK mode BEFORE EvalBootstrap construction and
    // requirement collection (mirrors rtlib d2d85df4 ordering). ACE-Library
    // replaces the old EvkRequirementMode arg with an evaluator-bound
    // EvkKeySetMode that must be set before EvalBootstrap is built (its
    // planning reads the bound mode); collect_requirements() is now
    // parameterless.
#ifdef ACE_LIBRARY_32
    _dev_eval->configure_evk_key_set_mode(acelib::EvkKeySetMode::CompactWhereEligible);
#else
    _dev_eval->configure_evk_key_set_mode(acelib::EvkKeySetMode::FullOnly);
#endif

    auto bts_parms_copy = _bts_parms;
    _boot = std::make_unique<EvalBootstrap>(std::move(bts_parms_copy), *_dev_eval);
#ifdef USE_NVTX
    _boot->profile = true;
#endif
    auto evk_requirements = _boot->collect_requirements();

    evk_requirements.galois_keys.require_steps(gal_steps);
    _dev_eval->enable_memory_pool(0.3);
    _dev_eval->get_publickey_from(pk);
    _dev_eval->get_secretkey_from(sk);

    if (evk_requirements.needs_relin_keys()) {
        kgen.create_relin_keys(evk_requirements.relin_keys, rlk);
    }

    kgen.create_galois_keys(evk_requirements.galois_keys, rtk, false);
    _dev_eval->get_evk_from(rtk, acelib::EvkConfigs::galois_key(true, data_layout,
                                                                acelib::EvkTransferMode::FULL_TRANSFER, std::nullopt,
                                                                evk_requirements.galois_keys.pre_galois_keys()));
    _dev_eval->get_evk_from(rlk, acelib::EvkConfigs::relin_key(data_layout));
    _dev_eval->context().prewarm(evk_requirements.runtime_aux);
    _dev_eval->preload_all();

    if (use_sparse_wrapping) {
        auto sparse_wrapping_keys = kgen.create_sparse_wrapping_keys();
        _dev_eval->get_sparse_wrapping_keys_from(sparse_wrapping_keys, data_layout);
    }
    _boot->prepare_encoded_const();
    CUDA_CHECK(cudaDeviceSynchronize());

    // Return the bootstrap device evaluator and context to the caller
    eval_out = _dev_eval;
    ctx_out = const_cast<acelib::ContextPro*>(&_dev_eval->context());
}

void BtsMgr::Bootstrap(acelib::DeviceCipher* op1, acelib::DeviceCipher* res, int level)
{
    acelib::DeviceCipher out;
    acelib::DeviceCipher boot_input = *op1;
    // Level resolution needs the context from the evaluator.
    auto& bts_ctx = _dev_eval->context();
    if (!_bts_parms.evalmod_handler_.parms().skip_) {
        auto entry_parm_id = bts_ctx.get_parms_id_from_level(_bts_parms.boot_entry_level());
        _dev_eval->mod_switch_to_inplace(boot_input, entry_parm_id);
    }
    out = _boot->bootstrap(boot_input);
#ifdef ACE_LIBRARY_32
    *res = out;
#else
    auto parm_id = bts_ctx.get_parms_id_from_level(static_cast<uint64_t>(level - 1));
    _dev_eval->mod_switch_to(out, parm_id, *res);
#endif
    CUDA_CHECK(cudaDeviceSynchronize());
}

#else  // !ACE_LIBRARY_ENABLE_BTS

void BtsMgr::Init_bts_cpu_context(CKKS_PARAMS* prog_param, bool& use_sparse_wrapping,
                                  acelib::host::HostContext** host_context_out)
{
    (void)prog_param;
    (void)use_sparse_wrapping;
    (void)host_context_out;
    FMT_ASSERT(false, "ACE library was built without bootstrap support.");
}

void BtsMgr::Init_bts_evaluator_components(acelib::host::CKKSEncoder& encoder, const acelib::host::SecretKey& sk,
                                           const acelib::host::PublicKey& pk, acelib::host::RelinKeys& rlk,
                                           acelib::host::GaloisKeys& rtk, acelib::host::KeyGenerator& kgen,
                                           acelib::Evaluator*& eval_out, acelib::ContextPro*& ctx_out,
                                           acelib::MEMORY_LAYOUT data_layout, const std::vector<int>& gal_steps)
{
    (void)encoder;
    (void)sk;
    (void)pk;
    (void)rlk;
    (void)rtk;
    (void)kgen;
    (void)eval_out;
    (void)ctx_out;
    (void)data_layout;
    (void)gal_steps;
    FMT_ASSERT(false, "ACE library was built without bootstrap support.");
}

void BtsMgr::Bootstrap(acelib::DeviceCipher* op1, acelib::DeviceCipher* res, int level)
{
    (void)op1;
    (void)res;
    (void)level;
    FMT_ASSERT(false, "AceLibrary_bootstrap called, but ACE was built without "
                      "bootstrap support.");
}

#endif  // ACE_LIBRARY_ENABLE_BTS

}  // namespace ace
