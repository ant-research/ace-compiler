//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/he_engine/host/ckks.h"
#include "ace/he_engine/host/keygenerator.h"
#include "ace/he_engine/RnsChainConfig.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/ace/ace_config.h"

#if ACE_LIBRARY_ENABLE_BTS
#include "ace/he_engine/host/decryptor.h"
#include "ace/he_engine/host/encryptor.h"
#endif

#include "common/common.h"
#include "common/error.h"
#include "fhe/need_bts.h"

#include <spdlog/spdlog.h>

namespace ace
{

//=============================================================================
// 32-bit modulus configuration (ACE_LIBRARY_32)
//=============================================================================

#ifdef ACE_LIBRARY_32

bool Build_mod_vector(int mul_depth, int n_sp, std::vector<uint64_t>& out, std::string& err)
{
    out.clear();
    err.clear();

    if (kModBase.size() < 2) {
        err = "not enough mod_base (need 2)";
        return false;
    }
    out.push_back(CKKS_32BIT_MODULI::kModBase[0]);
    out.push_back(CKKS_32BIT_MODULI::kModBase[1]);

    size_t idx53 = 0;
    size_t need_norm_q = 2 * static_cast<size_t>(mul_depth);

    if (need_norm_q > 0) {
        size_t taken53 = 0;
        const auto& mod53 = CKKS_32BIT_MODULI::kMod53;
        for (size_t i = 0; i < mod53.size() && taken53 < need_norm_q; ++i) {
            out.push_back(mod53[i]);
            ++taken53;
        }
        need_norm_q -= taken53;
    }

    if (need_norm_q > 0) {
        err = "not enough moduli to satisfy mul_depth";
        return false;
    }

    if (CKKS_32BIT_MODULI::kModNsp.size() < static_cast<size_t>(n_sp * 2)) {
        err = "not enough moduli to satisfy n_sp";
        return false;
    }
    for (int i = 0; i < n_sp * 2; ++i) {
        out.push_back(CKKS_32BIT_MODULI::kModNsp[i]);
    }

    return true;
}

bool Calculate_scales_per_level(uint64_t scaling_mod_size, const std::vector<uint64_t>& modulus, int mul_depth,
                                int n_sp, std::vector<double>& scales_out, std::string& err)
{
    scales_out.clear();
    err.clear();

    size_t special_mod_count = static_cast<size_t>(n_sp * 2);
    size_t total_mod_count = modulus.size();

    if (total_mod_count < special_mod_count) {
        err = "Total modulus count is less than special modulus count.";
        return false;
    }
    size_t normal_mod_count = total_mod_count - special_mod_count;

    if (mul_depth < 0) {
        err = "Multiplication depth cannot be negative.";
        return false;
    }
    if (normal_mod_count <= static_cast<size_t>(2 * mul_depth)) {
        err = "Not enough normal moduli to support the given multiplication depth.";
        return false;
    }

    std::vector<double> temp_scales;

    double current_scale = std::pow(2.0, static_cast<double>(scaling_mod_size));
    temp_scales.push_back(current_scale);
    temp_scales.push_back(current_scale);

    for (int i = 0; i < mul_depth; ++i) {
        size_t mod_index_1 = normal_mod_count - 2 * (i + 1);
        size_t mod_index_2 = mod_index_1 + 1;

        uint64_t mod1 = modulus[mod_index_1];
        uint64_t mod2 = modulus[mod_index_2];

        if (mod1 == 0 || mod2 == 0) {
            err = "Modulus is zero, cannot perform division.";
            return false;
        }

        double mod_product = static_cast<double>(mod1) * static_cast<double>(mod2);
        current_scale = (current_scale * current_scale) / mod_product;
        temp_scales.push_back(current_scale);
        temp_scales.push_back(current_scale);
    }

    for (int i = 0; i < static_cast<int>(normal_mod_count) - mul_depth * 2 - 2; ++i) {
        temp_scales.push_back(current_scale);
    }

    if (temp_scales.size() != normal_mod_count) {
        err = "temp_scales.size() != normal_mod_count.";
        return false;
    }

    std::reverse(temp_scales.begin(), temp_scales.end());
    scales_out = temp_scales;

    return true;
}

bool Validate_32bit_modulus_config(int mul_depth, int n_sp, const std::vector<uint64_t>& modulus,
                                   const std::vector<double>& scales, const std::vector<int>& scale_flags,
                                   std::string& err)
{
    err.clear();
    if (mul_depth < 0) {
        err = "Multiplication depth cannot be negative.";
        return false;
    }
    if (n_sp <= 0) {
        err = "Special-prime part count must be positive.";
        return false;
    }

    const size_t normal_mod_count = static_cast<size_t>(2 + 2 * mul_depth);
    const size_t special_mod_count = static_cast<size_t>(2 * n_sp);
    if (modulus.size() != normal_mod_count + special_mod_count) {
        err = "32-bit modulus count does not match double-rescale layout.";
        return false;
    }
    if (scales.size() != normal_mod_count) {
        err = "32-bit scale count must match normal modulus count.";
        return false;
    }
    if (scale_flags.size() != normal_mod_count) {
        err = "32-bit scale flag count must match normal modulus count.";
        return false;
    }
    for (size_t i = 0; i < scale_flags.size(); i += 2) {
        if (scale_flags[i] != 0 || scale_flags[i + 1] != 2) {
            err = "32-bit scale flags must use [0, 2] double-rescale pairs.";
            return false;
        }
    }

    return true;
}

#endif  // ACE_LIBRARY_32

//=============================================================================
// Bootstrap modulus configuration (ACE_LIBRARY_ENABLE_BTS)
//=============================================================================

#if ACE_LIBRARY_ENABLE_BTS

int Prime_bit_width(uint64_t prime)
{
    int bits = 0;
    while (prime != 0) {
        bits++;
        prime >>= 1;
    }
    return bits;
}

std::vector<int> Paired_modulus_bit_widths(const std::vector<uint64_t>& primes, const char* block_name)
{
    FMT_ASSERT(primes.size() % 2 == 0, "%s must contain 32-bit prime pairs", block_name);

    std::vector<int> bits;
    bits.reserve(primes.size() / 2);
    for (size_t i = 0; i < primes.size(); i += 2) {
        bits.push_back(Prime_bit_width(primes[i]) + Prime_bit_width(primes[i + 1]));
    }
    return bits;
}

std::vector<int> Physical_prime_bit_widths(const std::vector<uint64_t>& primes)
{
    std::vector<int> bits;
    bits.reserve(primes.size());
    for (uint64_t prime : primes) {
        bits.push_back(Prime_bit_width(prime));
    }
    return bits;
}

int Max_bit_width_or(const std::vector<int>& bits, int fallback)
{
    if (bits.empty()) {
        return fallback;
    }

    int max_bits = bits.front();
    for (size_t i = 1; i < bits.size(); i++) {
        max_bits = std::max(max_bits, bits[i]);
    }
    return max_bits;
}

std::vector<uint64_t> Concat_vectors(std::initializer_list<std::vector<uint64_t>> blocks)
{
    size_t total_size = 0;
    for (const auto& block : blocks) {
        total_size += block.size();
    }

    std::vector<uint64_t> result;
    result.reserve(total_size);
    for (const auto& block : blocks) {
        result.insert(result.end(), block.begin(), block.end());
    }
    return result;
}

std::vector<uint64_t> Take_prefix(const std::vector<uint64_t>& values, size_t count)
{
    FMT_ASSERT(count <= values.size(), "requested more primes than the block contains");
    return std::vector<uint64_t>(values.begin(), values.begin() + static_cast<std::ptrdiff_t>(count));
}

std::vector<uint64_t> Take_suffix_pairs(const std::vector<uint64_t>& values, size_t pair_count)
{
    const size_t value_count = pair_count * 2;
    FMT_ASSERT(values.size() % 2 == 0, "source must contain 32-bit prime pairs");
    FMT_ASSERT(value_count <= values.size(), "requested more prime pairs than the block contains");
    return std::vector<uint64_t>(values.end() - static_cast<std::ptrdiff_t>(value_count), values.end());
}

size_t Prime_pair_count(const std::vector<uint64_t>& primes, const char* block_name)
{
    FMT_ASSERT(primes.size() % 2 == 0, "%s must contain 32-bit prime pairs", block_name);
    return primes.size() / 2;
}

size_t Bts_normal_pair_count(size_t input_level, size_t fixed_pair_count, size_t max_normal_pair_count)
{
    FMT_ASSERT(input_level >= fixed_pair_count,
               "32-bit BTS input level %zu is too small: fixed bootstrap "
               "blocks need %zu logical prime pairs",
               input_level, fixed_pair_count);

    const size_t normal_pair_count = input_level - fixed_pair_count;
    FMT_ASSERT(normal_pair_count <= max_normal_pair_count,
               "32-bit BTS input level %zu needs %zu normal prime pairs, "
               "but only %zu are available",
               input_level, normal_pair_count, max_normal_pair_count);
    return normal_pair_count;
}

Bts_modulus_blocks Bts_set_f_modulus_blocks(size_t input_level)
{
    const std::vector<uint64_t> q0_bits55{0x7e00001, 0xffa0001};
    const std::vector<uint64_t> stc_default{0x6520001, 0x6640001, 0x6a00001, 0x6a20001, 0x6ae0001, 0x61e0001};
    const std::vector<uint64_t> cts_default{0x1320001, 0x3ee0001, 0x1fc0001, 0x2680001, 0x1aa0001, 0x3720001};
    const std::vector<uint64_t> cts4 = Concat_vectors({cts_default, {0x17a0001, 0x3120001}});
    const std::vector<uint64_t> normal_scale_stable50{0x1900001, 0x28c0001, 0x15c0001, 0x2ee0001, 0x19c0001, 0x27c0001,
                                                      0xbe0001,  0x5640001, 0x1360001, 0x34e0001, 0xca0001,  0x5140001,
                                                      0x14a0001, 0x31c0001, 0xac0001,  0x5f80001, 0x840001,  0x7cc0001};
    const std::vector<uint64_t> evalmod_scale_optimized{
        0x9400001, 0xd980001, 0x8200001, 0xf8a0001, 0xaf20001, 0xb7a0001, 0x96c0001, 0xd5a0001,
        0x98a0001, 0xd300001, 0xa000001, 0xc940001, 0x9120001, 0xdde0001, 0x9f60001, 0xca00001};
    const std::vector<uint64_t> nsp_bits56{0xfd20001, 0xc060001, 0xfc60001, 0xc300001, 0xfb40001, 0xc360001,
                                           0xfb20001, 0xc420001, 0xf9c0001, 0xc4c0001, 0xf960001, 0xc640001,
                                           0xf4e0001, 0xc760001, 0xf480001, 0xc8a0001};

    const size_t fixed_pair_count = Prime_pair_count(q0_bits55, "Q0") + Prime_pair_count(stc_default, "StC") +
                                    Prime_pair_count(evalmod_scale_optimized, "EvalMod") +
                                    Prime_pair_count(cts4, "CtS");
    const size_t normal_pair_count = Bts_normal_pair_count(input_level, fixed_pair_count,
                                                           Prime_pair_count(normal_scale_stable50, "Normal"));

    return {q0_bits55,
            stc_default,
            Take_suffix_pairs(normal_scale_stable50, normal_pair_count),
            evalmod_scale_optimized,
            cts4,
            Take_prefix(nsp_bits56, 12)};
}

std::vector<uint64_t> Flatten_modulus_blocks(const Bts_modulus_blocks& blocks)
{
    return Concat_vectors({blocks.q0, blocks.stc, blocks.normal, blocks.evalmod, blocks.cts, blocks.special});
}

void Apply_32bit_bts_modulus_config(bootstrap::BootstrapConfig& config, const Bts_modulus_blocks& blocks,
                                    size_t scaling_mod_size)
{
    const auto q0_bits = Paired_modulus_bit_widths(blocks.q0, "Q0");
    FMT_ASSERT(q0_bits.size() == 1, "Q0 must contain one logical modulus");

    const auto stc_bits = Paired_modulus_bit_widths(blocks.stc, "StC");
    const auto normal_bits = Paired_modulus_bit_widths(blocks.normal, "Normal");
    const auto evalmod_bits = Paired_modulus_bit_widths(blocks.evalmod, "EvalMod");
    const auto cts_bits = Paired_modulus_bit_widths(blocks.cts, "CtS");

    config.SetCtSDepth(static_cast<int>(cts_bits.size()));
    config.SetCtSLogQ(Max_bit_width_or(cts_bits, config.cts_params.logQ));
    config.SetStCDepth(static_cast<int>(stc_bits.size()));
    config.SetStCLogQ(Max_bit_width_or(stc_bits, config.stc_params.logQ));

    // Configure the modulus chain with the new SetGeneratedRnsChain API
    // (legacy config.crypto_params.logQ0/logQi/level/logP/n_special_prime were
    // removed in ace-library). Maps the RemovedRnsChainSpec fields from the old
    // scalars — mirrors rtlib d2d85df4 adaptation; fhe_lib/ace's per-block path
    // derives them from Bts_modulus_blocks instead of scalar logp/logq args.
    bootstrap::GeneratedRnsChainSpec generated_chain;
    generated_chain.base_q_bits = q0_bits.front();
    generated_chain.normal_modulus_bits = normal_bits;
    generated_chain.normal_transition_count = static_cast<int>(normal_bits.size());
    generated_chain.auxiliary_prime_bits = Physical_prime_bit_widths(blocks.special);
    generated_chain.auxiliary_prime_count = static_cast<int>(blocks.special.size());
    config.SetGeneratedRnsChain(generated_chain);
    config.SetMsgScale(std::pow(2.0, static_cast<double>(scaling_mod_size)));

    if (!evalmod_bits.empty()) {
        config.evalmod_params.logQ = Max_bit_width_or(evalmod_bits, config.evalmod_params.logQ);
    }

    if (!normal_bits.empty()) {
        config.final_scale = std::pow(2.0, normal_bits.front() - 2);
    }
    else {
        config.final_scale = config.msg_scale;
    }
}

int Bts_hamming_weight_or_default(size_t hamming_weight)
{
    if (hamming_weight == 0) {
        return 32;
    }
    FMT_ASSERT(hamming_weight == 32 || hamming_weight == 192,
               "ACE bootstrap currently supports only hamming weight 32 or 192");
    return static_cast<int>(hamming_weight);
}

#endif  // ACE_LIBRARY_ENABLE_BTS

//=============================================================================
// Common CKKS parameter helpers
//=============================================================================

int N_special_primes(const CKKS_PARAMS* prog_param)
{
    if (prog_param->_num_q_parts) {
        return prog_param->_num_q_parts;
    }
    constexpr int kDefaultDnum = 3;
    return prog_param->_mul_depth / kDefaultDnum + 1;
}

acelib::host::sec_level_type Security_level(const CKKS_PARAMS* prog_param, uint32_t degree)
{
    acelib::host::sec_level_type sec = acelib::host::sec_level_type::tc128;
    switch (prog_param->_sec_level) {
    case 128:
        sec = acelib::host::sec_level_type::tc128;
        break;
    case 192:
        sec = acelib::host::sec_level_type::tc192;
        break;
    case 256:
        sec = acelib::host::sec_level_type::tc256;
        break;
    default:
        sec = acelib::host::sec_level_type::none;
        break;
    }

    if (degree < 4096 && sec != acelib::host::sec_level_type::none) {
        DEV_WARN("WARNING: degree %d too small, reset security level to none\n", degree);
        return acelib::host::sec_level_type::none;
    }
    return sec;
}

void Log_context_params(const CKKS_PARAMS* prog_param)
{
    if (false) {  // kAceLibraryDebug -- controlled by ACE_LIBRARY_DEBUG
        return;
    }

    spdlog::debug("[ace] ckks_param: _provider={}, _poly_degree={}, _sec_level={}, "
                  "mul_depth={}, _first_mod_size={}, _scaling_mod_size={}, "
                  "_num_q_parts={}, _num_rot_idx={}",
                  static_cast<int>(prog_param->_provider), prog_param->_poly_degree, prog_param->_sec_level,
                  prog_param->_mul_depth, prog_param->_first_mod_size, prog_param->_scaling_mod_size,
                  prog_param->_num_q_parts, prog_param->_num_rot_idx);
}

void Configure_plain_parameters(CKKS_PARAMS* prog_param, acelib::host::HostParameter& parms, uint32_t degree,
                                int n_special_primes)
{
    parms.set_poly_modulus_degree(degree);

    if (Need_bts()) {
        return;
    }

#ifdef ACE_LIBRARY_32
    std::vector<uint64_t> modulus;
    std::vector<double> scales;
    std::string mod_err;
    if (!Build_mod_vector(prog_param->_mul_depth, n_special_primes, modulus, mod_err)) {
        FMT_ASSERT(false, "AceLibrary context init failed: %s", mod_err.c_str());
    }
    if (!Calculate_scales_per_level(prog_param->_scaling_mod_size, modulus, prog_param->_mul_depth, n_special_primes,
                                    scales, mod_err)) {
        FMT_ASSERT(false, "AceLibrary context init failed: %s", mod_err.c_str());
    }

    parms.set_scale_per_level(scales);
    parms.set_n_special_primes(n_special_primes * 2);
    std::vector<int> scale_flag_vec;
    for (int i = 0; i < 1 + prog_param->_mul_depth; ++i) {
        scale_flag_vec.push_back(0);
        scale_flag_vec.push_back(2);
    }
    if (!Validate_32bit_modulus_config(prog_param->_mul_depth, n_special_primes, modulus, scales, scale_flag_vec,
                                       mod_err)) {
        FMT_ASSERT(false, "AceLibrary context init failed: %s", mod_err.c_str());
    }
    parms.set_scale_flag(scale_flag_vec);
    parms.set_coeff_modulus(acelib::host::CoeffModulus::Create(modulus));
#else
    std::vector<int> bits;
    bits.push_back(prog_param->_first_mod_size);

    for (uint32_t i = 0; i < prog_param->_mul_depth; ++i) {
        bits.push_back(prog_param->_scaling_mod_size);
    }
    for (int i = 0; i < n_special_primes; ++i) {
        bits.push_back(prog_param->_first_mod_size);
    }
    if (n_special_primes > 1) {
        parms.set_n_special_primes(n_special_primes);
    }
    auto coeffs = acelib::host::CoeffModulus::Create(degree, bits);
    parms.set_coeff_modulus(coeffs);

    // Build the RNS-chain config (64-bit single-rescale path). New acelib
    // derives logical-level / rescale metadata from the chain; without it
    // mul/rescale scale accumulates wrong (gemm output off by ~scale factor).
    // canonical Q = first_mod + mul_depth*scaling; aux (special) = the rest.
    std::vector<uint64_t> q_primes;
    std::vector<uint64_t> aux_primes;
    const size_t q_mod_count = static_cast<size_t>(1 + prog_param->_mul_depth);
    for (size_t i = 0; i < coeffs.size(); ++i) {
        (i < q_mod_count ? q_primes : aux_primes).push_back(coeffs[i].value());
    }
    size_t log_n = 0;
    for (size_t n = degree; n > 1; n >>= 1) {
        log_n++;
    }
    auto chain_config = acelib::RnsChainConfig::PrefixChain(
        "ace_runtime", log_n, std::pow(2.0L, static_cast<long double>(prog_param->_scaling_mod_size)),
        std::move(q_primes), std::move(aux_primes),
        /*min_q_limb_count=*/1);
    parms.set_rns_chain_config(chain_config);
#endif
}

}  // namespace ace
