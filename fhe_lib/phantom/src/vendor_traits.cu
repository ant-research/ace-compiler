//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief vendor_traits.cu
//! VendorTraits implementation — bridges common framework to Phantom.
//!
//! Key accessor/populate methods extract raw GPU pointers from Phantom key
//! objects for serialization, and reconstruct key objects from deserialized
//! data. Context creation delegates to the compiler's CKKS_PARAMS.
//!
//! NOTE: Phantom key classes (PhantomSecretKey, PhantomPublicKey, etc.)
//! declare VendorTraits as a friend via a local patch, giving
//! access to their private members for populate operations.

#include "common/common.h"
#include "common/rt_api.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/phantom/vendor_traits.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

namespace phantom
{

//=============================================================================
// Key accessors — use Phantom's PUBLIC API
//=============================================================================

VendorTraits::SecretKeyAccess VendorTraits::access_secret_key(const SecretKey& sk, const Context& ctx)
{
    SecretKeyAccess access;
    access.data_rns_ptr = sk.data_rns().get();
    access.sk_array_ptr = sk.secret_key_array_data().get();
    access.sk_max_power = sk.sk_max_power();
    return access;
}

VendorTraits::PublicKeyAccess VendorTraits::access_public_key(const PublicKey& pk)
{
    PublicKeyAccess access;
    const PhantomCiphertext& pk_ct = pk.pk();
    access.pk_data_ptr = pk_ct.data();
    access.pk_size = pk_ct.size();
    access.chain_index = pk_ct.chain_index();
    access.scale = pk_ct.scale();
    access.poly_modulus_degree = pk_ct.poly_modulus_degree();
    access.coeff_modulus_size = pk_ct.coeff_modulus_size();
    return access;
}

VendorTraits::RelinKeyAccess VendorTraits::access_relin_key(const RelinKey& rlk)
{
    RelinKeyAccess access;
    access.key_data_ptrs.clear();
    for (const auto& k : rlk.public_keys()) {
        access.key_data_ptrs.push_back(k.get());
    }
    const auto& pid = rlk.parms_id();
    std::memcpy(access.parms_id.data(), &pid, sizeof(uint64_t) * 4);
    return access;
}

VendorTraits::GaloisKeyAccess VendorTraits::access_galois_key(const GaloisKey& rtk)
{
    GaloisKeyAccess access;
    access.relin_key_ptrs.clear();
    access.galois_elts.clear();

    const auto& rkeys = rtk.relin_keys();
    for (size_t i = 0; i < rkeys.size(); i++) {
        const auto& rk = rkeys[i];
        std::vector<const uint64_t*> comp_ptrs;
        for (const auto& k : rk.public_keys()) {
            comp_ptrs.push_back(k.get());
        }
        access.relin_key_ptrs.push_back(std::move(comp_ptrs));
    }

    const auto& pid = rtk.parms_id();
    std::memcpy(access.parms_id.data(), &pid, sizeof(uint64_t) * 4);
    return access;
}

//=============================================================================
// Key populate — uses friend access to private members
//=============================================================================

void VendorTraits::populate_secret_key(SecretKey& sk, const SecretKeyPopulate& data, uint64_t N, uint64_t L,
                                       Context& ctx)
{
    // Wrap raw GPU pointers into cuda_auto_ptr (same as old KEY_MANAGER::Load_keys)
    auto stream = phantom::util::global_variables::default_stream->get_stream();
    sk.data_rns_ = phantom::util::cuda_auto_ptr<uint64_t>(data.data_rns, N * L, stream);
    sk.secret_key_array_ = phantom::util::cuda_auto_ptr<uint64_t>(data.sk_array, data.sk_max_pow * N * L, stream);
    sk.sk_max_power_ = data.sk_max_pow;
    sk.gen_flag_ = true;
    sk.poly_modulus_degree_ = N;
    sk.coeff_modulus_size_ = L;
    sk.chain_index_ = 0;
}

void VendorTraits::populate_public_key(PublicKey& pk, const PublicKeyPopulate& data, uint64_t N, uint64_t L,
                                       Context& ctx)
{
    // PhantomPublicKey private member pk_ (PhantomCiphertext)
    pk.gen_flag_ = true;
    pk.pk_.resize(data.pk_size, L, N, phantom::util::global_variables::default_stream->get_stream());
    pk.pk_.set_chain_index(data.chain_index);
    pk.pk_.set_scale(data.scale);
    pk.pk_.set_ntt_form(true);

    size_t pk_data_bytes = data.pk_size * N * L * sizeof(uint64_t);
    cudaMemcpy(pk.pk_.data(), data.pk_data, pk_data_bytes, cudaMemcpyDeviceToDevice);
    cudaFree(data.pk_data);
    // parms_id on the PK ciphertext stays at default (parms_id_zero),
    // same as the old KEY_MANAGER::Load_keys behavior.
}

void VendorTraits::populate_relin_key(RelinKey& rlk, const RelinKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx)
{
    rlk.gen_flag_ = true;
    rlk.parms_id_ = data.parms_id;
    size_t key_comp_size = 2 * N * L;
    for (uint64_t i = 0; i < data.num_keys; i++) {
        rlk.public_keys_.emplace_back(phantom::util::cuda_auto_ptr<uint64_t>(
            data.key_data[i], key_comp_size, phantom::util::global_variables::default_stream->get_stream()));
    }
    if (!data.key_data.empty()) {
        auto stream = phantom::util::global_variables::default_stream->get_stream();
        std::vector<uint64_t*> h_ptrs(data.num_keys);
        for (uint64_t i = 0; i < data.num_keys; i++) {
            h_ptrs[i] = rlk.public_keys_[i].get();
        }
        uint64_t** dev_ptrs = nullptr;
        cudaMalloc(&dev_ptrs, data.num_keys * sizeof(uint64_t*));
        cudaMemcpy(dev_ptrs, h_ptrs.data(), data.num_keys * sizeof(uint64_t*), cudaMemcpyHostToDevice);
        rlk.public_keys_ptr_ = phantom::util::cuda_auto_ptr<uint64_t*>(dev_ptrs, data.num_keys, stream);
    }
}

void VendorTraits::populate_galois_key(GaloisKey& rtk, const GaloisKeyPopulate& data, uint64_t N, uint64_t L,
                                       Context& ctx)
{
    rtk.gen_flag_ = true;
    rtk.parms_id_ = data.parms_id;
    size_t key_comp_size = 2 * N * L;
    for (auto& rk_ptrs : data.relin_key_data) {
        PhantomRelinKey rk;
        rk.gen_flag_ = true;
        rk.parms_id_ = data.parms_id;
        for (auto* ptr : rk_ptrs) {
            rk.public_keys_.emplace_back(phantom::util::cuda_auto_ptr<uint64_t>(
                ptr, key_comp_size, phantom::util::global_variables::default_stream->get_stream()));
        }
        if (!rk.public_keys_.empty()) {
            std::vector<uint64_t*> h_ptrs(rk.public_keys_.size());
            for (size_t j = 0; j < rk.public_keys_.size(); j++) {
                h_ptrs[j] = rk.public_keys_[j].get();
            }
            auto stream = phantom::util::global_variables::default_stream->get_stream();
            uint64_t** dev_ptrs = nullptr;
            cudaMalloc(&dev_ptrs, rk.public_keys_.size() * sizeof(uint64_t*));
            cudaMemcpy(dev_ptrs, h_ptrs.data(), rk.public_keys_.size() * sizeof(uint64_t*), cudaMemcpyHostToDevice);
            rk.public_keys_ptr_ = phantom::util::cuda_auto_ptr<uint64_t*>(dev_ptrs, rk.public_keys_.size(), stream);
        }
        rtk.relin_keys_.push_back(std::move(rk));
    }
}

void VendorTraits::populate_one_relin_key(GaloisKey& rtk, size_t i, const std::vector<uint64_t*>& components,
                                          const std::array<uint64_t, 4>& parms_id, uint64_t N, uint64_t L)
{
    // Same per-slot construction as populate_galois_key's loop body, but
    // assign into a specific slot index rather than append — so Ensure_galois_key
    // can load keys out of rotation order. The slot must pre-exist (sized at
    // ON_DEMAND Init) and be empty; we overwrite it with a fresh PhantomRelinKey.
    if (i >= rtk.relin_keys_.size()) {
        throw std::out_of_range("populate_one_relin_key: slot index out of range");
    }

    size_t key_comp_size = 2 * N * L;
    auto stream = phantom::util::global_variables::default_stream->get_stream();

    PhantomRelinKey rk;
    rk.gen_flag_ = true;
    rk.parms_id_ = parms_id;
    for (auto* ptr : components) {
        rk.public_keys_.emplace_back(phantom::util::cuda_auto_ptr<uint64_t>(ptr, key_comp_size, stream));
    }
    if (!rk.public_keys_.empty()) {
        std::vector<uint64_t*> h_ptrs(rk.public_keys_.size());
        for (size_t j = 0; j < rk.public_keys_.size(); j++) {
            h_ptrs[j] = rk.public_keys_[j].get();
        }
        uint64_t** dev_ptrs = nullptr;
        cudaMalloc(&dev_ptrs, rk.public_keys_.size() * sizeof(uint64_t*));
        cudaMemcpy(dev_ptrs, h_ptrs.data(), rk.public_keys_.size() * sizeof(uint64_t*), cudaMemcpyHostToDevice);
        rk.public_keys_ptr_ = phantom::util::cuda_auto_ptr<uint64_t*>(dev_ptrs, rk.public_keys_.size(), stream);
    }
    rtk.relin_keys_[i] = std::move(rk);
}

void VendorTraits::prealloc_galois_slots(GaloisKey& rtk, size_t n, const std::array<uint64_t, 4>& parms_id)
{
    rtk.gen_flag_ = true;
    rtk.parms_id_ = parms_id;
    // resize (not assign) — PhantomRelinKey is move-only (owns cuda_auto_ptr),
    // so default-construct n slots; the existing empty vector resizes cleanly.
    // populate_one_relin_key later move-assigns into a specific slot.
    if (rtk.relin_keys_.size() < n) {
        rtk.relin_keys_.resize(n);
    }
}

void VendorTraits::clear_galois_slot(GaloisKey& rtk, size_t i)
{
    // Move-assign an empty PhantomRelinKey into slot i: the old key's
    // cuda_auto_ptr members drop out of scope and cudaFreeAsync their HBM.
    if (i >= rtk.relin_keys_.size()) {
        return;
    }
    rtk.relin_keys_[i] = PhantomRelinKey{};
}

uint32_t VendorTraits::galois_elt_from_step(const Context& ctx, int step)
{
    if (!ctx.key_galois_tool_) {
        throw std::runtime_error(
            "galois_elt_from_step: context galois tool not built (call rebuild_galois_tool first)");
    }
    return ctx.key_galois_tool_->get_elt_from_step(step);
}

namespace
{
// Reproduce phantom::arith::naf (util/numth.h) — non-adjacent form of an
// integer: the step as a sum of signed powers of two with no two adjacent
// non-zero digits, minimizing Hamming weight. Each component is ±2^i.
std::vector<int> naf(int value)
{
    std::vector<int> res;
    bool sign = value < 0;
    value = std::abs(value);
    for (int i = 0; value; i++) {
        int zi = (value & 0x1) ? 2 - (value & 0x3) : 0;
        value = (value - zi) >> 1;
        if (zi) {
            res.push_back((sign ? -zi : zi) * (1 << i));
        }
    }
    return res;
}
}  // namespace

std::vector<uint32_t> VendorTraits::naf_component_elts(const Context& ctx, int step)
{
    if (!ctx.key_galois_tool_) {
        throw std::runtime_error("naf_component_elts: context galois tool not built");
    }
    auto* tool = ctx.key_galois_tool_.get();
    // Phantom rotate_internal applies naf(step); each component temp_step is
    // rotated recursively. Only temp_step values whose elt is directly loaded
    // consume a key; very large |temp_step| (== N/2) are skipped (conjugation).
    // Collect distinct elts of the ±2^i decomposition so the caller can ensure
    // only those slots.
    std::vector<uint32_t> elts;
    auto add = [&](int s) {
        uint32_t e = tool->get_elt_from_step(s);
        if (std::find(elts.begin(), elts.end(), e) == elts.end()) {
            elts.push_back(e);
        }
    };
    // Phantom's poly degree N comes from the context (the galois tool keeps it
    // private). NAF component temp_step == ±N/2 is the conjugation step, which
    // rotate_internal skips (evaluate.cu:837). get_elt_from_step(±N/2) throws
    // "step count too large", so skip it here too. Its elt (m-1, e.g. 131071) is
    // the conjugate key, present in the loaded set but reached only via
    // conjugate_inplace, not via this rotate's recursion.
    const int64_t half_n = static_cast<int64_t>(ctx.poly_degree_) / 2;  // N/2
    for (int temp_step : naf(step)) {
        if (std::llabs(static_cast<long long>(temp_step)) == half_n) {
            continue;  // conjugation component — phantom skips, so do we
        }
        add(temp_step);
    }
    return elts;
}

//=============================================================================
// Plaintext accessor / populate
//=============================================================================

VendorTraits::PtAccess VendorTraits::access_plaintext(const Plaintext& pt)
{
    PtAccess access;
    access.gpu_data = pt.data();
    access.poly_modulus_degree = pt.poly_modulus_degree();
    access.coeff_modulus_size = pt.coeff_modulus_size();
    access.chain_index = pt.chain_index();
    access.scale = pt.scale();
    access.data_size = pt.coeff_count() * sizeof(uint64_t);
    return access;
}

void VendorTraits::populate_plaintext(Plaintext& pt, const PtPopulate& data, Context* ctx, void* stream)
{
    cudaStream_t s = static_cast<cudaStream_t>(stream);
    pt.resize(static_cast<size_t>(data.coeff_modulus_size), static_cast<size_t>(data.poly_modulus_degree), s);
    if (data.data_size > 0 && data.host_data != nullptr) {
        cudaMemcpy(pt.data(), data.host_data, data.data_size, cudaMemcpyHostToDevice);
    }
    pt.set_chain_index(static_cast<size_t>(data.chain_index));
    pt.set_scale(data.scale);
}

//=============================================================================
// Ciphertext accessor / populate (CiphertextSerializer hooks)
//=============================================================================

VendorTraits::CtAccess VendorTraits::access_ciphertext(const Ciphertext& ct)
{
    CtAccess access;
    access.gpu_data = ct.data();
    access.poly_modulus_degree = ct.poly_modulus_degree();
    access.coeff_modulus_size = ct.coeff_modulus_size();
    access.size = ct.size();
    access.chain_index = ct.chain_index();
    access.scale = ct.scale();
    access.is_ntt_form = ct.is_ntt_form();
    access.data_size = access.size * access.poly_modulus_degree * access.coeff_modulus_size * sizeof(uint64_t);
    // noise_scale_deg is read by phantom keyswitch (eval_key_switch.cu:111
    // depth = encrypted.GetNoiseScaleDeg()) and is_asymmetric by decrypt —
    // both MUST round-trip or eval/decrypt diverge.
    access.noise_scale_deg = static_cast<uint32_t>(ct.GetNoiseScaleDeg());
    access.is_asymmetric = ct.is_asymmetric();
    return access;
}

void VendorTraits::populate_ciphertext(Ciphertext& ct, const CtPopulate& data, Context* ctx, void* stream)
{
    cudaStream_t s = static_cast<cudaStream_t>(stream);
    ct.resize(static_cast<size_t>(data.size), static_cast<size_t>(data.coeff_modulus_size),
              static_cast<size_t>(data.poly_modulus_degree), s);
    if (data.data_size > 0 && data.host_data != nullptr) {
        cudaMemcpy(ct.data(), data.host_data, data.data_size, cudaMemcpyHostToDevice);
    }
    ct.set_chain_index(static_cast<size_t>(data.chain_index));
    ct.set_scale(data.scale);
    ct.set_poly_modulus_degree(static_cast<size_t>(data.poly_modulus_degree));
    ct.set_coeff_modulus_size(static_cast<size_t>(data.coeff_modulus_size));
    ct.set_ntt_form(data.is_ntt_form);
    ct.SetNoiseScaleDeg(static_cast<size_t>(data.noise_scale_deg));  // public setter (ciphertext.h:177)
    ct.SetAsymmetric(data.is_asymmetric);  // public setter (added to phantom-src ciphertext.h)
    // is_asymmetric is read by phantom decrypt to pick the symmetric/asymmetric
    // branch — must round-trip or decrypt diverges (1e272 garbage observed).
}

//=============================================================================
// Context creation
//=============================================================================

std::unique_ptr<VendorTraits::Context> VendorTraits::create_context(bool with_bootstrap)
{
    CKKS_PARAMS* prog_param = Get_context_params();
    EncryptionParameters parms(scheme_type::ckks);
    uint32_t degree = prog_param->_poly_degree;
    parms.set_poly_modulus_degree(degree);

    std::vector<int> bits;
    bits.push_back(prog_param->_first_mod_size);

    if (with_bootstrap) {
        uint32_t bts_required_level = 14;
        uint32_t bts_remaining_level = prog_param->_mul_depth - 14;
        for (uint32_t i = 0; i < bts_remaining_level; ++i)
            bits.push_back(prog_param->_scaling_mod_size);
        for (uint32_t i = 0; i < bts_required_level; ++i)
            bits.push_back(prog_param->_first_mod_size);
        constexpr size_t special_modulus_size = 4;
        for (size_t i = 0; i < special_modulus_size; i++)
            bits.push_back(prog_param->_first_mod_size);
        parms.set_secret_key_hamming_weight(192);
        parms.set_special_modulus_size(special_modulus_size);
    }
    else {
        for (uint32_t i = 0; i < prog_param->_mul_depth; ++i)
            bits.push_back(prog_param->_scaling_mod_size);
        bits.push_back(prog_param->_first_mod_size);
        parms.set_secret_key_hamming_weight(prog_param->_hamming_weight);
    }

    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(degree, bits));
    return std::make_unique<PhantomContext>(parms);
}

std::unique_ptr<VendorTraits::Context> VendorTraits::create_context_encode_only(bool with_bootstrap)
{
    return create_context(with_bootstrap);
}

void VendorTraits::generate_keys(Context& ctx, std::unique_ptr<SecretKey>& sk, std::unique_ptr<PublicKey>& pk,
                                 std::unique_ptr<RelinKey>& rlk, std::unique_ptr<GaloisKey>& rtk)
{
    sk = std::make_unique<PhantomSecretKey>(ctx);
    pk = std::make_unique<PhantomPublicKey>(sk->gen_publickey(ctx));
    rlk = std::make_unique<PhantomRelinKey>(sk->gen_relinkey(ctx));
    rtk = std::make_unique<PhantomGaloisKey>();
}

std::unique_ptr<PhantomCKKSEncoder> VendorTraits::create_encoder(Context& ctx)
{
    return std::make_unique<PhantomCKKSEncoder>(ctx);
}

//=============================================================================
// Context accessors
//=============================================================================

uint64_t VendorTraits::poly_degree(const Context& ctx)
{
    return ctx.poly_degree_;
}

uint64_t VendorTraits::coeff_mod_size(const Context& ctx)
{
    return ctx.coeff_mod_size_;
}

std::vector<uint32_t> VendorTraits::get_galois_elts(const Context& ctx)
{
    if (ctx.key_galois_tool_) {
        return ctx.key_galois_tool_->galois_elts();
    }
    return {};
}

void VendorTraits::rebuild_galois_tool(Context& ctx, const std::vector<uint32_t>& elts)
{
    int log_n = phantom::arith::get_power_of_two(ctx.poly_degree_);
    const auto& s = phantom::util::global_variables::default_stream->get_stream();
    ctx.key_galois_tool_.reset();
    ctx.key_galois_tool_ = std::make_unique<PhantomGaloisTool>(elts, log_n, s, false);
}

bool VendorTraits::is_key_generated(const SecretKey& sk)
{
    return sk.is_generated();
}

bool VendorTraits::is_key_generated(const PublicKey& pk)
{
    return pk.is_generated();
}

bool VendorTraits::is_key_generated(const RelinKey& rlk)
{
    return rlk.is_generated();
}

bool VendorTraits::is_key_generated(const GaloisKey& rtk)
{
    return rtk.is_generated();
}

//=============================================================================
// Evaluator creation
//=============================================================================

std::unique_ptr<CKKSEvaluator> VendorTraits::create_evaluator(Context& ctx, const SecretKey& sk, const PublicKey& pk,
                                                              const RelinKey& rlk, const GaloisKey& rtk,
                                                              PhantomCKKSEncoder* encoder, double scaling_mod_size)
{
    // CKKSEvaluator takes raw non-const pointers
    return std::make_unique<CKKSEvaluator>(&ctx, const_cast<PhantomPublicKey*>(&pk), const_cast<PhantomSecretKey*>(&sk),
                                           encoder, const_cast<PhantomRelinKey*>(&rlk),
                                           const_cast<PhantomGaloisKey*>(&rtk), std::pow(2.0, scaling_mod_size));
}

//=============================================================================
// Bootstrap initialization
//=============================================================================

uint64_t VendorTraits::init_bootstrap(CKKSEvaluator* evaluator, BtsMgr& bootstrap, bool keys_loaded_from_kms,
                                      uint64_t scaling_mod_size, int32_t mul_depth, int32_t first_mod_size,
                                      int32_t scaling_mod_bits)
{

    auto t_total_0 = std::chrono::high_resolution_clock::now();

    uint32_t bts_required_level = 14;
    uint32_t bts_remaining_level = mul_depth - 14;

    // Compute num_prime_parts
    std::vector<int> bits;
    bits.push_back(first_mod_size);
    for (uint32_t i = 0; i < bts_remaining_level; ++i)
        bits.push_back(scaling_mod_bits);
    for (uint32_t i = 0; i < bts_required_level; ++i)
        bits.push_back(first_mod_size);
    constexpr size_t special_modulus_size = 4;
    for (size_t i = 0; i < special_modulus_size; i++)
        bits.push_back(first_mod_size);
    uint64_t num_prime_parts = bits.size() - special_modulus_size + 1;

    long boundary_K = 25;
    long deg = 59;
    long scale_factor = 2;
    long inverse_deg = 1;
    long loge = 10;
    int log_slot_count = 15;

    auto t_bts_0 = std::chrono::high_resolution_clock::now();
    bootstrap.Set(BTS_SLOTS::POW_15,
                  std::make_unique<Bootstrapper>(loge, 15, log_slot_count, mul_depth, std::pow(2.0, scaling_mod_size),
                                                 boundary_K, deg, scale_factor, inverse_deg, evaluator));
    bootstrap.Set(BTS_SLOTS::POW_14,
                  std::make_unique<Bootstrapper>(loge, 14, log_slot_count, mul_depth, std::pow(2.0, scaling_mod_size),
                                                 boundary_K, deg, scale_factor, inverse_deg, evaluator));
    bootstrap.Set(BTS_SLOTS::POW_13,
                  std::make_unique<Bootstrapper>(loge, 13, log_slot_count, mul_depth, std::pow(2.0, scaling_mod_size),
                                                 boundary_K, deg, scale_factor, inverse_deg, evaluator));
    bootstrap.Set(BTS_SLOTS::POW_12,
                  std::make_unique<Bootstrapper>(loge, 12, log_slot_count, mul_depth, std::pow(2.0, scaling_mod_size),
                                                 boundary_K, deg, scale_factor, inverse_deg, evaluator));
    auto t_bts_1 = std::chrono::high_resolution_clock::now();
    double s_bts_ctor = std::chrono::duration<double>(t_bts_1 - t_bts_0).count();

    auto t_prep_0 = std::chrono::high_resolution_clock::now();
    // All 4 Bootstrappers share the same (boundary_K, deg, scale_factor, inverse_deg, loge)
    // so prepare_mod_polynomial produces identical results — compute once, share the rest.
    bootstrap.Get(BTS_SLOTS::POW_15)->prepare_mod_polynomial();
    auto* src_mod = bootstrap.Get(BTS_SLOTS::POW_15)->mod_reducer;
    for (auto slot : {BTS_SLOTS::POW_14, BTS_SLOTS::POW_13, BTS_SLOTS::POW_12}) {
        auto* dst_mod = bootstrap.Get(slot)->mod_reducer;
        dst_mod->sin_cos_polynomial.copy(src_mod->sin_cos_polynomial);
        dst_mod->sin_cos_polynomial.generate_poly_heap();  // copy() skips poly_heap
        dst_mod->inverse_sin_polynomial.copy(src_mod->inverse_sin_polynomial);
        dst_mod->scale_inverse_coeff = src_mod->scale_inverse_coeff;
    }
    auto t_prep_1 = std::chrono::high_resolution_clock::now();
    double s_prep_mod = std::chrono::duration<double>(t_prep_1 - t_prep_0).count();

    std::vector<int> gal_steps_vector;
    gal_steps_vector.push_back(0);
    for (int i = 0; i < log_slot_count; i++) {
        gal_steps_vector.push_back((1 << i));
    }

    auto t_rotkey_0 = std::chrono::high_resolution_clock::now();
    bootstrap.Get(BTS_SLOTS::POW_15)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    bootstrap.Get(BTS_SLOTS::POW_14)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    bootstrap.Get(BTS_SLOTS::POW_13)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    bootstrap.Get(BTS_SLOTS::POW_12)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    auto t_rotkey_1 = std::chrono::high_resolution_clock::now();
    double s_rotkey = std::chrono::duration<double>(t_rotkey_1 - t_rotkey_0).count();

    auto t_galgen_0 = std::chrono::high_resolution_clock::now();
    if (!keys_loaded_from_kms) {
        create_galois_keys_from_steps(*evaluator, gal_steps_vector);
    }
    else {
        spdlog::info("[kms] Skipping galois key generation in bootstrap — using keys loaded from disk");
    }
    auto t_galgen_1 = std::chrono::high_resolution_clock::now();
    double s_galgen = std::chrono::duration<double>(t_galgen_1 - t_galgen_0).count();

    bootstrap.Get(BTS_SLOTS::POW_15)->slot_vec.push_back(15);
    bootstrap.Get(BTS_SLOTS::POW_14)->slot_vec.push_back(14);
    bootstrap.Get(BTS_SLOTS::POW_13)->slot_vec.push_back(13);
    bootstrap.Get(BTS_SLOTS::POW_12)->slot_vec.push_back(12);

    auto t_lt_0 = std::chrono::high_resolution_clock::now();
    bootstrap.Get(BTS_SLOTS::POW_15)->generate_LT_coefficient_3();
    bootstrap.Get(BTS_SLOTS::POW_14)->generate_LT_coefficient_3();
    bootstrap.Get(BTS_SLOTS::POW_13)->generate_LT_coefficient_3();
    bootstrap.Get(BTS_SLOTS::POW_12)->generate_LT_coefficient_3();
    auto t_lt_1 = std::chrono::high_resolution_clock::now();
    double s_lt = std::chrono::duration<double>(t_lt_1 - t_lt_0).count();

    auto t_total_1 = std::chrono::high_resolution_clock::now();
    double s_total = std::chrono::duration<double>(t_total_1 - t_total_0).count();

    spdlog::debug("[timing] init_bootstrap: total={:.2f}s | bts_ctor={:.2f}s prep_mod={:.2f}s rotkey={:.2f}s "
                  "galgen={:.2f}s LT={:.2f}s",
                  s_total, s_bts_ctor, s_prep_mod, s_rotkey, s_galgen, s_lt);

    return num_prime_parts;
}

//=============================================================================
// Galois key generation
//=============================================================================

void VendorTraits::create_galois_keys_from_steps(CKKSEvaluator& evaluator, std::vector<int>& steps)
{
    evaluator.decryptor.create_galois_keys_from_steps(steps, *(const_cast<PhantomGaloisKey*>(evaluator.galois_keys)));
}

//=============================================================================
// Key upload for session switching
//=============================================================================

// Thread-local FHE-context state (Mode B-P). Lives in this single TU so the
// compiled kernel .so never inlines a cross-DSO thread_local read. The void*
// is a FheContext<phantom::VendorTraits>* (cast by context.h).
namespace
{
thread_local void* tl_current_context = nullptr;
thread_local int tl_active_session_idx = 0;
}  // namespace

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

cudaStream_t VendorTraits::current_stream_value()
{
    // The thread-local current_stream wrapper is set by Activate_session to this
    // session's cuda_stream_wrapper. Returns its raw cudaStream_t.
    return phantom::util::global_variables::current_stream().get_stream();
}

void VendorTraits::upload_keys_to_evaluator(CKKSEvaluator& evaluator, Crypto& crypto, Context& ctx, const SecretKey& sk,
                                            const PublicKey& pk, const RelinKey& rlk, const GaloisKey& rtk)
{
    // CKKSEvaluator stores encryptor/decryptor/relin_keys/galois_keys as
    // public members. Reassign them to point to the new session's keys.
    // The const_cast pattern is consistent with create_evaluator().
    evaluator.encryptor = Encryptor(&ctx, const_cast<PhantomPublicKey*>(&pk));
    evaluator.decryptor = Decryptor(&ctx, const_cast<PhantomSecretKey*>(&sk));
    evaluator.relin_keys = const_cast<PhantomRelinKey*>(&rlk);
    evaluator.galois_keys = const_cast<PhantomGaloisKey*>(&rtk);

    // Populate the per-session Crypto struct with the same encryptor/decryptor.
    // Phantom Encryptor/Decryptor are value types holding pointers — make
    // new copies that point to the same keys.
    crypto.encryptor = std::make_unique<Encryptor>(evaluator.encryptor);
    crypto.decryptor = std::make_unique<Decryptor>(evaluator.decryptor);
}

//=============================================================================
// HE operation dispatch hooks — equal_level / encode_const
//=============================================================================

void VendorTraits::equal_level_ciph(Evaluator& eval, Ciphertext& a, Ciphertext& b)
{
    auto level_a = a.chain_index();
    auto level_b = b.chain_index();
    if (level_a != level_b) {
        if (level_a < level_b) {
            eval.eval->evaluator.mod_switch_to_inplace(a, level_b);
        }
        else {
            eval.eval->evaluator.mod_switch_to_inplace(b, level_a);
        }
    }
}

void VendorTraits::equal_level_plain(Evaluator& eval, Ciphertext& ct, Plaintext& pt)
{
    auto level_ct = ct.chain_index();
    auto level_pt = pt.chain_index();
    if (level_ct != level_pt) {
        if (level_ct < level_pt) {
            eval.eval->evaluator.mod_switch_to_inplace(ct, level_pt);
        }
        else {
            eval.eval->evaluator.mod_switch_to_inplace(pt, level_ct);
        }
    }
}

void VendorTraits::encode_const(Evaluator& eval, Plaintext& pt, double value, const Ciphertext& ref)
{
    eval.eval->encoder.encode(value, ref.chain_index(), ref.scale(), pt);
}

}  // namespace phantom
