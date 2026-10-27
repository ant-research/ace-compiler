//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief vendor_traits.cu
//! VendorTraits implementation — maps the common FHE framework to the
//! ACE GPU backend's vendor types and memory layouts.

#include "ace/he_engine/host/ckks.h"
#include "ace/he_engine/host/decryptor.h"
#include "ace/he_engine/host/encryptor.h"
#include "ace/he_engine/host/keygenerator.h"
#include "common/common.h"
#include "common/error.h"
#include "common/rt_api.h"
#include "fhe/vendor/ace/ace_config.h"
#include "fhe/vendor/ace/vendor_traits.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>

#if ACE_LIBRARY_ENABLE_BTS
#include "bootstrap/EvalBootstrap.h"
#include "EvaluatorWrapper.h"
#endif

namespace ace
{

//=============================================================================
// Thread-local device context singleton
//=============================================================================

static thread_local acelib::ContextPro* tl_device_context = nullptr;

// Thread-local FHE-context state (Mode B-P), mirroring tl_device_context's
// pattern: TLS in a single TU, accessed only via exported functions so the
// compiled kernel .so never inlines a cross-DSO thread_local read.
static thread_local void* tl_current_context = nullptr;
static thread_local int tl_active_session_idx = 0;

acelib::ContextPro* VendorTraits::get_device_context()
{
    return tl_device_context;
}

void VendorTraits::set_device_context(acelib::ContextPro* ctx)
{
    tl_device_context = ctx;
}

void* VendorTraits::current_context_ptr()
{
    return tl_current_context;
}
void VendorTraits::set_current_context_ptr(void* p)
{
    tl_current_context = p;
}
void* VendorTraits::acquire_current_context_ptr()
{
    void* p = tl_current_context;
    tl_current_context = nullptr;
    return p;
}
int VendorTraits::active_session_idx()
{
    return tl_active_session_idx;
}
void VendorTraits::set_active_session_idx(int idx)
{
    tl_active_session_idx = idx;
}

//=============================================================================
// Key accessors — serialize host keys via save(), extract metadata
//=============================================================================

VendorTraits::SecretKeyAccess VendorTraits::access_secret_key(const SecretKey& sk, const Context& ctx)
{
    SecretKeyAccess acc;
    // Serialize host key to binary blob via ACE's built-in save()
    std::ostringstream oss;
    sk.save(oss);
    acc.serialized = oss.str();

    auto parms = ctx.first_context_data()->parms();
    acc.poly_modulus_degree = parms.poly_modulus_degree();
    acc.coeff_modulus_size = parms.coeff_modulus().size();
    return acc;
}

VendorTraits::PublicKeyAccess VendorTraits::access_public_key(const PublicKey& pk)
{
    PublicKeyAccess acc;
    std::ostringstream oss;
    pk.save(oss);
    acc.serialized = oss.str();
    // N and L are not available from PublicKey alone; they will be
    // stored in the KeyHeader by the KeySerializer from the Context.
    acc.poly_modulus_degree = 0;
    acc.coeff_modulus_size = 0;
    return acc;
}

VendorTraits::RelinKeyAccess VendorTraits::access_relin_key(const RelinKey& rlk)
{
    RelinKeyAccess acc;
    std::ostringstream oss;
    rlk.save(oss);
    acc.serialized = oss.str();
    acc.poly_modulus_degree = 0;
    acc.coeff_modulus_size = 0;
    return acc;
}

VendorTraits::GaloisKeyAccess VendorTraits::access_galois_key(const GaloisKey& rtk)
{
    GaloisKeyAccess acc;
    std::ostringstream oss;
    rtk.save(oss);
    acc.serialized = oss.str();
    acc.poly_modulus_degree = 0;
    acc.coeff_modulus_size = 0;
    // Extract galois elements from the key
    // ACE GaloisKeys store galois elements internally
    // TODO: Extract galois_elts from the key when API is available
    return acc;
}

//=============================================================================
// Key populate — reconstruct host keys from serialized data
//=============================================================================

void VendorTraits::populate_secret_key(SecretKey& sk, const SecretKeyPopulate& data, uint64_t N, uint64_t L,
                                       Context& ctx)
{
    (void)N;
    (void)L;
    std::istringstream iss(data.serialized);
    sk.unsafe_load(ctx, iss);
}

void VendorTraits::populate_public_key(PublicKey& pk, const PublicKeyPopulate& data, uint64_t N, uint64_t L,
                                       Context& ctx)
{
    (void)N;
    (void)L;
    std::istringstream iss(data.serialized);
    pk.load(ctx, iss);
}

void VendorTraits::populate_relin_key(RelinKey& rlk, const RelinKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx)
{
    (void)N;
    (void)L;
    std::istringstream iss(data.serialized);
    rlk.unsafe_load(ctx, iss);
}

void VendorTraits::populate_galois_key(GaloisKey& rtk, const GaloisKeyPopulate& data, uint64_t N, uint64_t L,
                                       Context& ctx)
{
    (void)N;
    (void)L;
    std::istringstream iss(data.serialized);
    rtk.unsafe_load(ctx, iss);
    // galois_elts are stored in the serialized blob; no separate population needed.
}

//=============================================================================
// Context accessors
//=============================================================================

uint64_t VendorTraits::poly_degree(const Context& ctx)
{
    return ctx.first_context_data()->parms().poly_modulus_degree();
}

uint64_t VendorTraits::coeff_mod_size(const Context& ctx)
{
    return ctx.first_context_data()->parms().coeff_modulus().size();
}

std::vector<uint32_t> VendorTraits::get_galois_elts(const Context& ctx)
{
    (void)ctx;
    // ACE does not store galois elements in the context; they are managed
    // by the KeyGenerator/Evaluator. Return empty for now.
    // TODO: Extract from galois key if needed.
    return {};
}

bool VendorTraits::is_key_generated(const SecretKey& sk)
{
    // ACE SecretKey wraps a Plaintext; check if it has coefficient data
    return sk.data().coeff_count() > 0;
}

bool VendorTraits::is_key_generated(const PublicKey& pk)
{
    // ACE PublicKey (SEAL object) is valid once constructed
    (void)pk;
    return true;
}

bool VendorTraits::is_key_generated(const RelinKey& rlk)
{
    // ACE RelinKeys (SEAL object) is valid once constructed
    (void)rlk;
    return true;
}

bool VendorTraits::is_key_generated(const GaloisKey& rtk)
{
    // ACE GaloisKeys (SEAL object) is valid once constructed
    (void)rtk;
    return true;
}

//=============================================================================
// Plaintext accessor / populate
//=============================================================================

VendorTraits::PtAccess VendorTraits::access_plaintext(const Plaintext& pt)
{
    PtAccess acc;
    acc.gpu_data = pt.data();
    acc.poly_modulus_degree = pt.poly_modulus_degree();
    acc.coeff_modulus_size = pt.coeff_modulus_size();
    acc.parms_id = pt.parms_id();
    acc.scale = pt.scale();
    acc.is_ntt_form = pt.is_ntt_form();
    // Data size in bytes = poly_modulus_degree * coeff_modulus_size * sizeof(Dtype)
    // Dtype is uint64_t (64-bit mode) or uint32_t (32-bit mode, ACE_LIBRARY_32)
    size_t elem_count = pt.poly_modulus_degree() * pt.coeff_modulus_size();
#ifdef __DATA_WORD_SIZE_64__
    acc.data_size = elem_count * sizeof(uint64_t);
#else
    acc.data_size = elem_count * sizeof(uint32_t);
#endif
    return acc;
}

void VendorTraits::populate_plaintext(Plaintext& pt, const PtPopulate& data, Context* ctx, void* stream)
{
    (void)ctx;
    // Resize the DevicePlain with the target parms_id, then copy data
    acelib::ContextPro* dev_ctx = get_device_context();
    IS_TRUE(dev_ctx != nullptr, "VendorTraits::populate_plaintext: device context not set");

    // Convert chain_index (stored in EncHeader) back to parms_id
    // The populate caller should set data.parms_id correctly.
    pt.resize(*dev_ctx, data.parms_id);

    // Copy host data to device
    cudaStream_t cu_stream = (stream != nullptr) ? static_cast<cudaStream_t>(stream) : cudaStreamDefault;
    cudaMemcpyAsync(pt.data(), data.host_data, data.data_size, cudaMemcpyHostToDevice, cu_stream);

    // Set metadata
    pt.scale() = data.scale;
    pt.is_ntt_form() = data.is_ntt_form;
}

//=============================================================================
// Context creation
//=============================================================================

std::unique_ptr<VendorTraits::Context> VendorTraits::create_context(bool with_bootstrap)
{
    CKKS_PARAMS* prog_param = Get_context_params();

    acelib::host::HostParameter parms(acelib::host::scheme_type::ckks);
    uint32_t degree = prog_param->_poly_degree;
    int n_special_primes = N_special_primes(prog_param);
    Configure_plain_parameters(prog_param, parms, degree, n_special_primes);

    acelib::host::sec_level_type sec = Security_level(prog_param, degree);

    if (with_bootstrap) {
        // Bootstrap: BtsMgr creates the HostContext with bootstrap params
        // The caller (KeyMgr::Init) must call BtsMgr::Init_bts_cpu_context
        // first, then pass the resulting HostContext here.
        // For now, create a basic context; bootstrap path is handled separately.
        // TODO: Refine bootstrap context creation flow.
        bool use_sparse_wrapping = false;
        acelib::host::HostContext* host_ctx = nullptr;
        // BtsMgr will be initialized by FheContext constructor
        return std::make_unique<Context>(parms, true, sec);
    }

    return std::make_unique<Context>(parms, true, sec);
}

std::unique_ptr<VendorTraits::Context> VendorTraits::create_context_encode_only(bool with_bootstrap)
{
    // Encode-only: create a minimal context without key generation
    return create_context(with_bootstrap);
}

//=============================================================================
// Key generation
//=============================================================================

void VendorTraits::generate_keys(Context& ctx, std::unique_ptr<SecretKey>& sk, std::unique_ptr<PublicKey>& pk,
                                 std::unique_ptr<RelinKey>& rlk, std::unique_ptr<GaloisKey>& rtk,
                                 bool use_sparse_wrapping)
{
    auto kgen = std::make_unique<acelib::host::KeyGenerator>(ctx, use_sparse_wrapping);
    sk = std::make_unique<SecretKey>();
    *sk = kgen->secret_key();

    pk = std::make_unique<PublicKey>();
    kgen->create_public_key(*pk);

    rlk = std::make_unique<RelinKey>();
    kgen->create_relin_keys(*rlk);

    rtk = std::make_unique<GaloisKey>();
    // Galois keys are created later via create_galois_keys_from_steps
    // or by BtsMgr::Init_bts_evaluator_components.
    // Keep the KeyGenerator alive for bootstrap use.
    (void)kgen;  // kgen will be destroyed after keys are generated
}

//=============================================================================
// Encoder / Evaluator creation
//=============================================================================

std::unique_ptr<acelib::host::CKKSEncoder> VendorTraits::create_encoder(Context& ctx)
{
    return std::make_unique<acelib::host::CKKSEncoder>(ctx);
}

std::unique_ptr<VendorTraits::Evaluator>
VendorTraits::create_evaluator(Context& ctx, const SecretKey& sk, const PublicKey& pk, const RelinKey& rlk,
                               const GaloisKey& rtk, acelib::host::CKKSEncoder* encoder, double scaling_mod_size)
{
    (void)sk;
    (void)encoder;
    (void)scaling_mod_size;
    auto eval = std::make_unique<VendorTraits::Evaluator>();
    eval->eval = std::make_unique<acelib::Evaluator>(acelib::ContextPro::get_device_context(ctx), true);
    eval->eval->enable_memory_pool(0.4);
    eval->ctx = const_cast<acelib::ContextPro*>(&eval->eval->context());
    return eval;
}

//=============================================================================
// Upload keys to GPU evaluator
//=============================================================================

void VendorTraits::upload_keys_to_evaluator(Evaluator& eval, const PublicKey& pk, const SecretKey& sk,
                                            const RelinKey& rlk, const GaloisKey& rtk,
                                            acelib::MEMORY_LAYOUT data_layout)
{
    eval.eval->get_publickey_from(pk, data_layout);
    eval.eval->get_secretkey_from(sk, data_layout);
    eval.eval->get_evk_from(rlk, acelib::EvkConfigs::relin_key(data_layout));
    eval.eval->get_evk_from(rtk, acelib::EvkConfigs::galois_key(true, data_layout));
    // preload_all (not preload_special_keys): the galois compact-EVK variants
    // for rotation steps must be materialized to device, which preload_all does
    // (preload_special_keys only covers RelinKeys/SparseWrappingKeys). Without
    // it, resolve_galois_variant_source throws "compact EVK variant unavailable"
    // at run time. Matches acelib's official rotation example.
    eval.eval->preload_all();
}

//=============================================================================
// Bootstrap initialization
//=============================================================================

uint64_t VendorTraits::init_bootstrap(acelib::Evaluator* evaluator, BtsMgr& bootstrap, bool keys_loaded_from_kms,
                                      uint64_t scaling_mod_size, int32_t mul_depth, int32_t first_mod_size,
                                      int32_t scaling_mod_bits)
{
    (void)evaluator;
    (void)keys_loaded_from_kms;
    (void)scaling_mod_size;
    (void)mul_depth;
    (void)first_mod_size;
    (void)scaling_mod_bits;
    // Bootstrap initialization for ACE is handled by BtsMgr
    // directly in the FheContext constructor, as it has complex interactions
    // with the CODEC and key manager. This method returns num_prime_parts
    // which is computed from the bootstrap parameters.
    // TODO: Extract num_prime_parts from bootstrap context.
    CKKS_PARAMS* prog_param = Get_context_params();
    std::vector<int> bits;
    bits.push_back(prog_param->_first_mod_size);
    for (uint32_t i = 0; i < prog_param->_mul_depth; ++i)
        bits.push_back(prog_param->_scaling_mod_size);
    bits.push_back(prog_param->_first_mod_size);
    return bits.size();
}

//=============================================================================
// Galois key creation
//=============================================================================

void VendorTraits::create_galois_keys_from_steps(acelib::host::KeyGenerator& kgen, GaloisKey& rtk,
                                                 const std::vector<int>& steps)
{
    kgen.create_galois_keys(steps, rtk, true);
}

//=============================================================================
// Rebuild galois tool (no-op for ACE)
//=============================================================================

void VendorTraits::rebuild_galois_tool(Context& ctx, const std::vector<uint32_t>& elts)
{
    (void)ctx;
    (void)elts;
    // ACE does not have a separate galois tool to rebuild.
    // Galois keys are managed by the Evaluator's evk_cache.
}

//=============================================================================
// HE operation dispatch hooks — equal_level / encode_const
//=============================================================================

void VendorTraits::equal_level_ciph(Evaluator& eval, Ciphertext& a, Ciphertext& b)
{
    std::size_t level_a = a.coeff_modulus_size();
    std::size_t level_b = b.coeff_modulus_size();
    if (level_a == level_b) {
        return;
    }

    const auto& ct_lo = level_a < level_b ? a : b;
    auto& ct_hi = level_a < level_b ? b : a;
    auto target_parms_id = ct_lo.parms_id();

#ifdef ADJUST_LEVEL_REDUCE_ERROR
    const auto& context_hi = eval.active_eval()->context().get_context_data(ct_hi.parms_id());
    double q_hi = static_cast<double>(context_hi->back_coeff_modulus());
#ifdef ACE_LIBRARY_32
    q_hi *= context_hi->next_context_data()->back_coeff_modulus();
#endif
    double fix_scale = 1.0;
    double scale_adjust = (ct_lo.scale() / ct_hi.scale()) * (q_hi / ct_hi.scale()) * fix_scale;

    eval.active_eval()->multiply_scalar_inplace(ct_hi, scale_adjust);
    ct_hi.scale() = ct_lo.scale() * q_hi;
    eval.active_eval()->rescale_to_next_inplace(ct_hi);
#endif

    eval.active_eval()->mod_switch_to_inplace(ct_hi, target_parms_id);
}

void VendorTraits::equal_level_plain(Evaluator& /*eval*/, Ciphertext& /*ct*/, Plaintext& /*pt*/)
{
    // ACE evaluator handles level matching internally for add_plain/multiply_plain.
    // No explicit level adjustment is needed.
}

void VendorTraits::encode_const(Evaluator& eval, Plaintext& pt, double value, const Ciphertext& ref)
{
    acelib::host::Plaintext host_pt;
    eval.encoder->encode(value, ref.parms_id(), ref.scale(), host_pt);
    pt = acelib::DevicePlain::from(host_pt, eval.active_eval()->context());
}

}  // namespace ace
