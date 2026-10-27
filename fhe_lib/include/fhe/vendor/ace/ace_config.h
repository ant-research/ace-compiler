//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_ACE_ACE_CONFIG_H
#define RTLIB_RT_ACE_ACE_CONFIG_H

//! @file ace_config.h
//! @brief ACE backend CKKS parameter / modulus-table helpers (32-bit moduli,
//!        bootstrap modulus blocks, N_special_primes / Security_level /
//!        Configure_plain_parameters ...).
//!
//! Framework placement note: this is a backend IMPLEMENTATION detail (modulus
//! arithmetic + CKKS param derivation), NOT the vendor-injection point into
//! the framework. Unlike bts_mgr.h, vendor_traits.h does NOT include it — it
//! is used only by ACE backend src (.cu) files plus ace/bts_mgr.h. It would
//! belong more naturally under fhe_lib/ace/src/ (internal impl header); it
//! stays here for now as a deferred move — moving it requires touching 7
//! include sites and the bts_mgr.h reverse include.

#include "ace/he_engine/host/ckks.h"
#include "common/common.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#ifndef ACE_LIBRARY_ENABLE_BTS
#define ACE_LIBRARY_ENABLE_BTS 0
#endif

#if ACE_LIBRARY_ENABLE_BTS
// These headers are only available when building the FHErt_ace library itself
// (from the ACE SDK's he-modules/src/ directory). Generated kernels and
// external consumers do not have access to them, so guard with a build flag.
#ifdef ACE_BTS_IMPL
#include "bootstrap/EvalBootstrap.h"
#include "EvaluatorWrapper.h"
#include "NVTXWrapper.h"
#endif
#endif

namespace ace
{

//=============================================================================
// 32-bit modulus constants and configuration (ACE_LIBRARY_32)
//=============================================================================

#ifdef ACE_LIBRARY_32

struct CKKS_32BIT_MODULI
{
    static constexpr std::array<uint64_t, 2> kModBase = {
        0x1ffc0001, 0xffa0001  // 57
    };

    static constexpr std::array<uint64_t, 22> kMod53 = {
        0x5340001, 0x5560001, 0x58e0001, 0x5640001, 0x61e0001, 0x5140001, 0x6520001, 0x6640001,
        0x5ee0001, 0x51a0001, 0x5e20001, 0x5e60001, 0x6280001, 0x4fc0001, 0x5940001, 0x5980001,
        0x5f80001, 0x6100001, 0x5aa0001, 0x5b60001, 0x3ee0001, 0x7e00001};

    static constexpr std::array<uint64_t, 31> kModNsp = {
        0x1ff60001, 0x1fcc0001, 0x1fba0001, 0x1fb00001, 0x1f960001, 0x1f8c0001, 0x1f7e0001, 0x1f5c0001,
        0x1f560001, 0x1f4a0001, 0x1f480001, 0x1f0e0001, 0x1eee0001, 0x1ee80001, 0x1ed80001, 0x1ed20001,
        0x1e8a0001, 0x1e840001, 0x1e660001, 0x1e520001, 0x1e120001, 0x1de20001, 0x1dd40001, 0x1db80001,
        0x1db60001, 0x1d880001, 0x1d7a0001, 0x1d700001, 0x1d560001, 0x1d4a0001, 0x1d2c0001};
};

bool Build_mod_vector(int mul_depth, int n_sp, std::vector<uint64_t>& out, std::string& err);

bool Calculate_scales_per_level(uint64_t scaling_mod_size, const std::vector<uint64_t>& modulus, int mul_depth,
                                int n_sp, std::vector<double>& scales_out, std::string& err);

bool Validate_32bit_modulus_config(int mul_depth, int n_sp, const std::vector<uint64_t>& modulus,
                                   const std::vector<double>& scales, const std::vector<int>& scale_flags,
                                   std::string& err);

#endif  // ACE_LIBRARY_32

//=============================================================================
// Bootstrap modulus configuration (ACE_LIBRARY_ENABLE_BTS)
//=============================================================================

#if ACE_LIBRARY_ENABLE_BTS

struct Bts_modulus_blocks
{
    std::vector<uint64_t> q0;
    std::vector<uint64_t> stc;
    std::vector<uint64_t> normal;
    std::vector<uint64_t> evalmod;
    std::vector<uint64_t> cts;
    std::vector<uint64_t> special;
};

int Prime_bit_width(uint64_t prime);

std::vector<int> Paired_modulus_bit_widths(const std::vector<uint64_t>& primes, const char* block_name);

std::vector<int> Physical_prime_bit_widths(const std::vector<uint64_t>& primes);

int Max_bit_width_or(const std::vector<int>& bits, int fallback);

std::vector<uint64_t> Concat_vectors(std::initializer_list<std::vector<uint64_t>> blocks);

std::vector<uint64_t> Take_prefix(const std::vector<uint64_t>& values, size_t count);

std::vector<uint64_t> Take_suffix_pairs(const std::vector<uint64_t>& values, size_t pair_count);

size_t Prime_pair_count(const std::vector<uint64_t>& primes, const char* block_name);

size_t Bts_normal_pair_count(size_t input_level, size_t fixed_pair_count, size_t max_normal_pair_count);

Bts_modulus_blocks Bts_set_f_modulus_blocks(size_t input_level);

std::vector<uint64_t> Flatten_modulus_blocks(const Bts_modulus_blocks& blocks);

#if defined(ACE_BTS_IMPL)
// Apply_32bit_bts_modulus_config requires bootstrap::BootstrapConfig from
// the private ACE SDK headers (he-modules/src/). Only available during
// library build, not in generated kernels.
void Apply_32bit_bts_modulus_config(bootstrap::BootstrapConfig& config, const Bts_modulus_blocks& blocks,
                                    size_t scaling_mod_size);
#endif

int Bts_hamming_weight_or_default(size_t hamming_weight);

#endif  // ACE_LIBRARY_ENABLE_BTS

//=============================================================================
// Common CKKS parameter helpers
//=============================================================================

int N_special_primes(const CKKS_PARAMS* prog_param);

acelib::host::sec_level_type Security_level(const CKKS_PARAMS* prog_param, uint32_t degree);

void Log_context_params(const CKKS_PARAMS* prog_param);

void Configure_plain_parameters(CKKS_PARAMS* prog_param, acelib::host::HostParameter& parms, uint32_t degree,
                                int n_special_primes);

}  // namespace ace

#endif  // RTLIB_RT_ACE_ACE_CONFIG_H
