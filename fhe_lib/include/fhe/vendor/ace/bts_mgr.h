//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_ACE_BTS_MGR_H
#define RTLIB_RT_ACE_BTS_MGR_H

//! @file bts_mgr.h
//! @brief ACE backend `BtsMgr` — owns bootstrap host context + device
//!        evaluator wrapper, performs `Bootstrap()`.
//!
//! Framework placement note: backend IMPLEMENTATION detail, NOT the
//! vendor-injection point. Lives under fhe/vendor/ace/ because
//! `vendor_traits.h` references `ace::BtsMgr` as a trait type alias
//! (`using BtsMgr = ace::BtsMgr;`). Implementation body is in
//! fhe_lib/ace/src/bts_mgr.cu; only vendor_traits.h and context.cu
//! consume this header.

#include "ace/he_engine/Context.h"
#include "ace/he_engine/Evaluator.h"
#include "ace/he_engine/host/ckks.h"
#include "ace/he_engine/host/keygenerator.h"
#include "common/common.h"
#include "fhe/vendor/ace/ace_config.h"

#include <cstdint>
#include <vector>

#if ACE_LIBRARY_ENABLE_BTS && defined(ACE_BTS_IMPL)
#include "bootstrap/EvalBootstrap.h"
#include "EvaluatorWrapper.h"
#endif

namespace ace
{

#if ACE_LIBRARY_ENABLE_BTS && !defined(ACE_BTS_IMPL)
// When bootstrap is enabled but private headers are not available
// (e.g., generated kernels), forward-declare the private types.
class EvalWrapper;
namespace bootstrap
{
class EvalBootstrap;
}
#endif

/// @brief Manages bootstrap configuration and execution.
///
/// Owns the bootstrap context parameters, device evaluator wrapper, and
/// bootstrap evaluator. Provides Init methods to set up CPU and GPU contexts
/// for bootstrap, and a Bootstrap method to refresh ciphertext noise.
///
/// When ACE_LIBRARY_ENABLE_BTS is 0, all methods become stubs that assert
/// on invocation (bootstrap is not supported).
class BtsMgr
{
public:
    BtsMgr() = default;
    ~BtsMgr() = default;

    // Disable copy and move
    BtsMgr(const BtsMgr&) = delete;
    BtsMgr& operator=(const BtsMgr&) = delete;
    BtsMgr(BtsMgr&&) = delete;
    BtsMgr& operator=(BtsMgr&&) = delete;

    /// @brief Initialize the bootstrap CPU context (host parameters + bootstrap
    ///        parameters). Sets use_sparse_wrapping output.
    /// @param prog_param           Compiler-provided CKKS parameters
    /// @param use_sparse_wrapping  [out] Whether sparse key wrapping is needed
    /// @param host_context_out     [out] The created host context for bootstrap
    void Init_bts_cpu_context(CKKS_PARAMS* prog_param, bool& use_sparse_wrapping,
                              acelib::host::HostContext** host_context_out);

    /// @brief Initialize the bootstrap device evaluator using individual components.
    /// This overload works with the new FheContext<VendorTraits> path which
    /// doesn't use ace::KEY_MANAGER / ace::CODEC.
    /// @param encoder            CKKS encoder
    /// @param sk                 Secret key
    /// @param pk                 Public key
    /// @param rlk                Relinearization key (mutable: may be regenerated)
    /// @param rtk                Galois key (mutable: will be regenerated with bts steps)
    /// @param kgen               Key generator for creating relin/galois keys
    /// @param eval_out           [out] Device evaluator pointer to set
    /// @param ctx_out            [out] ContextPro pointer to set
    /// @param data_layout        Memory layout for GPU data
    /// @param gal_steps          Galois rotation steps needed
    void Init_bts_evaluator_components(acelib::host::CKKSEncoder& encoder, const acelib::host::SecretKey& sk,
                                       const acelib::host::PublicKey& pk, acelib::host::RelinKeys& rlk,
                                       acelib::host::GaloisKeys& rtk, acelib::host::KeyGenerator& kgen,
                                       acelib::Evaluator*& eval_out, acelib::ContextPro*& ctx_out,
                                       acelib::MEMORY_LAYOUT data_layout, const std::vector<int>& gal_steps);

    /// @brief Perform bootstrap on a ciphertext.
    /// @param op1    Input ciphertext
    /// @param res    Output ciphertext (refreshed)
    /// @param level  Target logical level after bootstrap
    void Bootstrap(acelib::DeviceCipher* op1, acelib::DeviceCipher* res, int level);

    /// @brief Return the device evaluator (may be null if no bootstrap).
    /// Only available when ACE_BTS_IMPL is defined (library build, not generated kernels).
#if ACE_LIBRARY_ENABLE_BTS && defined(ACE_BTS_IMPL)
    EvalWrapper* dev_eval() { return _dev_eval; }
    const EvalWrapper* dev_eval() const { return _dev_eval; }
#else
    void* dev_eval() { return nullptr; }
    const void* dev_eval() const { return nullptr; }
#endif

    /// @brief Return the host context created for bootstrap (may be null).
    acelib::host::HostContext* host_context()
    {
#if ACE_LIBRARY_ENABLE_BTS && defined(ACE_BTS_IMPL)
        return _host_context;
#else
        return nullptr;
#endif
    }
    const acelib::host::HostContext* host_context() const
    {
#if ACE_LIBRARY_ENABLE_BTS && defined(ACE_BTS_IMPL)
        return _host_context;
#else
        return nullptr;
#endif
    }

#if ACE_LIBRARY_ENABLE_BTS && defined(ACE_BTS_IMPL)
    /// @brief Access the bootstrap parameters.
    EvalBootstrap::BootstrapContext& bts_parms() { return _bts_parms; }
#endif

private:
#if ACE_LIBRARY_ENABLE_BTS && defined(ACE_BTS_IMPL)
    /// @brief Build bootstrap parameters from compiler parameters.
    void Make_bts_parms(CKKS_PARAMS* prog_param, acelib::host::HostParameter& parms_out,
                        EvalBootstrap::BootstrapContext& bts_parms_out);

    EvalBootstrap::BootstrapContext _bts_parms;
    std::unique_ptr<EvalWrapper> _dev_eval_owner;
    std::unique_ptr<EvalBootstrap> _boot;
    EvalWrapper* _dev_eval = nullptr;
    acelib::host::HostContext* _host_context = nullptr;
#endif
};

}  // namespace ace

#endif  // RTLIB_RT_ACE_BTS_MGR_H
