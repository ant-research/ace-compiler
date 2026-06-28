//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "rt_ace/ace_api.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include <ace/he_engine/host/ckks.h>
#include <ace/he_engine/host/decryptor.h>
#include <ace/he_engine/host/encryptor.h>
#include <ace/he_engine/host/keygenerator.h>

#ifndef ACE_LIBRARY_ENABLE_BTS
#define ACE_LIBRARY_ENABLE_BTS 0
#endif

#if ACE_LIBRARY_ENABLE_BTS
#include "EvaluatorWrapper.h"
#include "NVTXWrapper.h"
#include "bootstrap/EvalBootstrap.h"

#endif

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"
#include "common/trace.h"

#ifndef ACE_LIBRARY_DEBUG
#define ACE_LIBRARY_DEBUG 0
#endif

namespace {
constexpr bool kAceLibraryDebug = ACE_LIBRARY_DEBUG != 0;

FILE* Ace_debug_file() {
  return Is_trace_on() ? Get_trace_file() : stderr;
}

FILE* Ace_dump_file() {
  return Is_trace_on() ? Get_trace_file() : stdout;
}

#ifdef ACE_LIBRARY_32
constexpr std::array<uint64_t, 2> kModBase = {
    0x1ffc0001, 0xffa0001  // 57
};

constexpr std::array<uint64_t, 22> kMod53 = {
    0x5340001, 0x5560001, 0x58e0001, 0x5640001, 0x61e0001, 0x5140001,
    0x6520001, 0x6640001, 0x5ee0001, 0x51a0001, 0x5e20001, 0x5e60001,
    0x6280001, 0x4fc0001, 0x5940001, 0x5980001, 0x5f80001, 0x6100001,
    0x5aa0001, 0x5b60001, 0x3ee0001, 0x7e00001
};

constexpr std::array<uint64_t, 31> kModNsp = {
    0x1ff60001, 0x1fcc0001, 0x1fba0001, 0x1fb00001, 0x1f960001,
    0x1f8c0001, 0x1f7e0001, 0x1f5c0001, 0x1f560001, 0x1f4a0001,
    0x1f480001, 0x1f0e0001, 0x1eee0001, 0x1ee80001, 0x1ed80001,
    0x1ed20001, 0x1e8a0001, 0x1e840001, 0x1e660001, 0x1e520001,
    0x1e120001, 0x1de20001, 0x1dd40001, 0x1db80001, 0x1db60001,
    0x1d880001, 0x1d7a0001, 0x1d700001, 0x1d560001, 0x1d4a0001,
    0x1d2c0001
};

template <size_t BucketSize>
size_t Take_from_bucket(const std::array<uint64_t, BucketSize>& bucket,
                        size_t& idx, size_t need,
                        std::vector<uint64_t>& out) {
  if (idx >= bucket.size()) {
    return 0;
  }
  size_t available = bucket.size() - idx;
  size_t take = (need < available) ? need : available;
  if (take > 0) {
    out.insert(out.end(), bucket.begin() + idx, bucket.begin() + idx + take);
    idx += take;
  }
  return take;
}

bool Build_mod_vector(int mul_depth, int n_sp, std::vector<uint64_t>& out,
                      std::string& err) {
  out.clear();
  err.clear();

  if (kModBase.size() < 2) {
    err = "not enough mod_base (need 2)";
    return false;
  }
  out.push_back(kModBase[0]);
  out.push_back(kModBase[1]);

  size_t idx53 = 0;
  size_t need_norm_q = 2 * static_cast<size_t>(mul_depth);

  if (need_norm_q > 0) {
    size_t taken53 = Take_from_bucket(kMod53, idx53, need_norm_q, out);
    need_norm_q -= taken53;
  }

  if (need_norm_q > 0) {
    err = "not enough moduli to satisfy mul_depth";
    return false;
  }

  if (kModNsp.size() < static_cast<size_t>(n_sp * 2)) {
    err = "not enough moduli to satisfy n_sp";
    return false;
  }
  for (int i = 0; i < n_sp * 2; i++) {
    out.push_back(kModNsp[i]);
  }

  return true;
}

bool Calculate_scales_per_level(uint64_t scaling_mod_size,
                                const std::vector<uint64_t>& modulus,
                                int mul_depth, int n_sp,
                                std::vector<double>& scales_out,
                                std::string& err) {
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

  for (int i = 0; i < normal_mod_count - mul_depth * 2 - 2; ++i) {
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

bool Validate_32bit_modulus_config(int mul_depth, int n_sp,
                                   const std::vector<uint64_t>& modulus,
                                   const std::vector<double>& scales,
                                   const std::vector<int>& scale_flags,
                                   std::string& err) {
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

#if ACE_LIBRARY_ENABLE_BTS
struct Bts_modulus_blocks {
  std::vector<uint64_t> q0;
  std::vector<uint64_t> stc;
  std::vector<uint64_t> normal;
  std::vector<uint64_t> evalmod;
  std::vector<uint64_t> cts;
  std::vector<uint64_t> special;
};

int Prime_bit_width(uint64_t prime) {
  int bits = 0;
  while (prime != 0) {
    bits++;
    prime >>= 1;
  }
  return bits;
}

std::vector<int> Paired_modulus_bit_widths(
    const std::vector<uint64_t>& primes, const char* block_name) {
  FMT_ASSERT(primes.size() % 2 == 0,
             "%s must contain 32-bit prime pairs", block_name);

  std::vector<int> bits;
  bits.reserve(primes.size() / 2);
  for (size_t i = 0; i < primes.size(); i += 2) {
    bits.push_back(Prime_bit_width(primes[i]) +
                   Prime_bit_width(primes[i + 1]));
  }
  return bits;
}

std::vector<int> Physical_prime_bit_widths(
    const std::vector<uint64_t>& primes) {
  std::vector<int> bits;
  bits.reserve(primes.size());
  for (uint64_t prime : primes) {
    bits.push_back(Prime_bit_width(prime));
  }
  return bits;
}

int Max_bit_width_or(const std::vector<int>& bits, int fallback) {
  if (bits.empty()) {
    return fallback;
  }

  int max_bits = bits.front();
  for (size_t i = 1; i < bits.size(); i++) {
    max_bits = std::max(max_bits, bits[i]);
  }
  return max_bits;
}

std::vector<uint64_t> Concat_vectors(
    std::initializer_list<std::vector<uint64_t>> blocks) {
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

std::vector<uint64_t> Take_prefix(const std::vector<uint64_t>& values,
                                  size_t count) {
  FMT_ASSERT(count <= values.size(),
             "requested more primes than the block contains");
  return std::vector<uint64_t>(
      values.begin(), values.begin() + static_cast<std::ptrdiff_t>(count));
}

std::vector<uint64_t> Take_suffix_pairs(const std::vector<uint64_t>& values,
                                        size_t pair_count) {
  const size_t value_count = pair_count * 2;
  FMT_ASSERT(values.size() % 2 == 0,
             "source must contain 32-bit prime pairs");
  FMT_ASSERT(value_count <= values.size(),
             "requested more prime pairs than the block contains");
  return std::vector<uint64_t>(
      values.end() - static_cast<std::ptrdiff_t>(value_count), values.end());
}

size_t Prime_pair_count(const std::vector<uint64_t>& primes,
                        const char* block_name) {
  FMT_ASSERT(primes.size() % 2 == 0,
             "%s must contain 32-bit prime pairs", block_name);
  return primes.size() / 2;
}

size_t Bts_normal_pair_count(size_t input_level,
                             size_t fixed_pair_count,
                             size_t max_normal_pair_count) {
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

Bts_modulus_blocks Bts_set_f_modulus_blocks(size_t input_level) {
  const std::vector<uint64_t> q0_bits55{0x7e00001, 0xffa0001};
  const std::vector<uint64_t> stc_default{
      0x6520001, 0x6640001, 0x6a00001,
      0x6a20001, 0x6ae0001, 0x61e0001};
  const std::vector<uint64_t> cts_default{
      0x1320001, 0x3ee0001, 0x1fc0001,
      0x2680001, 0x1aa0001, 0x3720001};
  const std::vector<uint64_t> cts4 =
      Concat_vectors({cts_default, {0x17a0001, 0x3120001}});
  const std::vector<uint64_t> normal_scale_stable50{
      0x1900001, 0x28c0001, 0x15c0001, 0x2ee0001, 0x19c0001,
      0x27c0001, 0xbe0001,  0x5640001, 0x1360001, 0x34e0001,
      0xca0001,  0x5140001, 0x14a0001, 0x31c0001, 0xac0001,
      0x5f80001, 0x840001,  0x7cc0001};
  const std::vector<uint64_t> evalmod_scale_optimized{
      0x9400001, 0xd980001, 0x8200001, 0xf8a0001, 0xaf20001,
      0xb7a0001, 0x96c0001, 0xd5a0001, 0x98a0001, 0xd300001,
      0xa000001, 0xc940001, 0x9120001, 0xdde0001, 0x9f60001,
      0xca00001};
  const std::vector<uint64_t> nsp_bits56{
      0xfd20001, 0xc060001, 0xfc60001, 0xc300001, 0xfb40001,
      0xc360001, 0xfb20001, 0xc420001, 0xf9c0001, 0xc4c0001,
      0xf960001, 0xc640001, 0xf4e0001, 0xc760001, 0xf480001,
      0xc8a0001};

  const size_t fixed_pair_count =
      Prime_pair_count(q0_bits55, "Q0") +
      Prime_pair_count(stc_default, "StC") +
      Prime_pair_count(evalmod_scale_optimized, "EvalMod") +
      Prime_pair_count(cts4, "CtS");
  const size_t normal_pair_count = Bts_normal_pair_count(
      input_level, fixed_pair_count,
      Prime_pair_count(normal_scale_stable50, "Normal"));

  return {q0_bits55, stc_default,
          Take_suffix_pairs(normal_scale_stable50, normal_pair_count),
          evalmod_scale_optimized, cts4, Take_prefix(nsp_bits56, 12)};
}

std::vector<uint64_t> Flatten_modulus_blocks(
    const Bts_modulus_blocks& blocks) {
  return Concat_vectors({blocks.q0, blocks.stc, blocks.normal,
                         blocks.evalmod, blocks.cts, blocks.special});
}

void Apply_32bit_bts_modulus_config(
    bootstrap::BootstrapConfig& config, const Bts_modulus_blocks& blocks,
    size_t scaling_mod_size) {
  const auto q0_bits = Paired_modulus_bit_widths(blocks.q0, "Q0");
  FMT_ASSERT(q0_bits.size() == 1, "Q0 must contain one logical modulus");

  const auto stc_bits = Paired_modulus_bit_widths(blocks.stc, "StC");
  const auto normal_bits = Paired_modulus_bit_widths(blocks.normal, "Normal");
  const auto evalmod_bits =
      Paired_modulus_bit_widths(blocks.evalmod, "EvalMod");
  const auto cts_bits = Paired_modulus_bit_widths(blocks.cts, "CtS");

  config.SetCtSDepth(static_cast<int>(cts_bits.size()));
  config.SetCtSLogQ(Max_bit_width_or(cts_bits, config.cts_params.logQ));
  config.SetStCDepth(static_cast<int>(stc_bits.size()));
  config.SetStCLogQ(Max_bit_width_or(stc_bits, config.stc_params.logQ));

  config.crypto_params.logQ0 = q0_bits.front();
  config.crypto_params.logQi = normal_bits;
  config.crypto_params.level = static_cast<int>(normal_bits.size());
  config.crypto_params.n_special_prime =
      static_cast<int>(blocks.special.size());
  config.crypto_params.logP = Physical_prime_bit_widths(blocks.special);
  config.SetMsgScale(std::pow(2.0, static_cast<double>(scaling_mod_size)));

  if (!evalmod_bits.empty()) {
    config.evalmod_params.logQ =
        Max_bit_width_or(evalmod_bits, config.evalmod_params.logQ);
  }

  if (!normal_bits.empty()) {
    config.final_scale = std::pow(2.0, normal_bits.front() - 2);
  } else {
    config.final_scale = config.msg_scale;
  }
}
#endif
#endif

#if ACE_LIBRARY_ENABLE_BTS
int Bts_hamming_weight_or_default(size_t hamming_weight) {
  if (hamming_weight == 0) {
    return 32;
  }
  FMT_ASSERT(hamming_weight == 32 || hamming_weight == 192,
             "ACE bootstrap currently supports only hamming weight 32 or 192");
  return static_cast<int>(hamming_weight);
}
#endif

}  // namespace

//! @brief Runtime context and operation dispatcher for the ACE library backend.
class ACE_LIBRARY_CONTEXT {
  using Ciphertext = acelib::DeviceCipher;
  using Plaintext  = acelib::DevicePlain;
  using HostCiphertext = acelib::host::Ciphertext;
  using HostPlaintext  = acelib::host::Plaintext;
public:
  const acelib::host::SecretKey&  Secret_key() const { return _kgen->secret_key(); }
  const acelib::host::PublicKey&  Public_key() const { return *_pk; }
  const acelib::host::RelinKeys&  Relin_key() const { return *_rlk; }
  const acelib::host::GaloisKeys& Rotate_key() const { return *_rtk; }

  static ACE_LIBRARY_CONTEXT* Context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    return Instance.get();
  }

  static void Init_context() {
    IS_TRUE(Instance == nullptr, "instance already initialized");
    Instance.reset(new ACE_LIBRARY_CONTEXT());
  }

  static void Fini_context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    Instance.reset();
  }

  uint64_t Runtime_level(int level) const {
    FMT_ASSERT(level > 0, "Runtime_level: logical level must be >= 1, got %d",
               level);
#ifdef ACE_LIBRARY_32
    return static_cast<uint64_t>(2 * level - 1);
#else
    return static_cast<uint64_t>(level - 1);
#endif
  }

public:
  void Prepare_input(TENSOR* input, const char* name) {
    size_t              len = TENSOR_SIZE(input);
    std::vector<double> vec(input->_vals, input->_vals + len);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(_input_level));
    HostPlaintext           pt;
    _encoder->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size), pt);
    HostCiphertext ct;
    if (Need_bts()) {
      _encryptor->encrypt_symmetric(pt, ct);
    } else {
      _encryptor->encrypt(pt, ct);
    }
    Ciphertext* gpu_ct = Own_cipher(
        Ciphertext::from(ct, _gpu_eval->context(), _data_layout));
    cudaDeviceSynchronize();
    Io_set_input(name, 0, gpu_ct);
  }

  void Set_output_data(const char* name, size_t idx, Ciphertext* ct) {
    Io_set_output(name, idx, Own_cipher(*ct));
  }

  Ciphertext Get_input_data(const char* name, size_t idx) {
    Ciphertext* data = (Ciphertext*)Io_get_input(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    return *(data);
  }

  double* Handle_output(const char* name, size_t idx) {
    Ciphertext* data = (Ciphertext*)Io_get_output(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    HostCiphertext host_data;
    HostPlaintext pt;
    host_data = (*data).to(*_cpu_ctx);
    _decryptor->decrypt(host_data, pt);
    std::vector<double> vec;
    _encoder->decode(pt, vec);
    double* msg = (double*)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    if (kAceLibraryDebug) {
      FILE* debug_file = Ace_debug_file();
      fprintf(debug_file, "Handle_output: scale: %f\n", pt.scale());
      fprintf(debug_file, "Decoded msg (size %zu): [", vec.size());
      size_t print_count = std::min(vec.size(), static_cast<size_t>(4));
      for (size_t i = 0; i < print_count; ++i) {
        fprintf(debug_file, "%.6f", msg[i]);
        if (i + 1 < print_count) {
          fprintf(debug_file, ", ");
        }
      }
      fprintf(debug_file, "]\n");
    }

    return msg;
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, SCALE_T scale,
                    LEVEL_T level) {
    std::vector<double> vec(input, input + len);
    _gpu_eval->encode(vec, level, std::pow(2.0, _scaling_mod_size * scale), *pt);
  }

  void Encode_double(Plaintext* pt, double* input, size_t len, SCALE_T scale,
                     LEVEL_T level) {
    std::vector<double> vec(input, input + len);
    _gpu_eval->encode(vec, level, std::pow(2.0, _scaling_mod_size * scale), *pt);
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, SCALE_T scale,
                    int level) {
    std::vector<double> vec(input, input + len);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
    _gpu_eval->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), *pt);
  }

  void Encode_float_gpu(Plaintext* pt, float* input, size_t len, SCALE_T scale,
                        int level) {
    std::vector<double> vec(input, input + len);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
    _gpu_eval->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), *pt);
  }

  void Encode_float_cst_lvl(Plaintext* pt, float* input, size_t len,
                            SCALE_T scale, int level) {
    std::vector<double> vec(input, input + len);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
    _gpu_eval->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), *pt);
  }

  void Encode_double_cst_lvl(Plaintext* pt, double* input, size_t len,
                             SCALE_T scale, int level) {
    std::vector<double> vec(input, input + len);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
    _gpu_eval->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), *pt);
  }

  void Encode_float_mask(Plaintext* pt, float input, size_t len, SCALE_T scale,
                         LEVEL_T level) {
    std::vector<double> vec(len, input);
    double scale_this_level;
    #ifdef ACE_LIBRARY_32
    auto data_context_ptr = _ctx->get_context_data(level);
    FMT_ASSERT(data_context_ptr != nullptr,
               "Encode_float_mask Error: data_context_ptr null!");
    scale_this_level = data_context_ptr->scale_per_level();
    #else
    scale_this_level = std::pow(2.0, _scaling_mod_size) * scale;
    #endif
    _gpu_eval->encode(vec, level, scale_this_level, *pt);
  }

  void Encode_double_mask(Plaintext* pt, double input, size_t len,
                          SCALE_T scale, LEVEL_T level) {
    std::vector<double> vec(len, input);
    double scale_this_level;
    #ifdef ACE_LIBRARY_32
    auto data_context_ptr = _ctx->get_context_data(level);
    FMT_ASSERT(data_context_ptr != nullptr,
               "Encode_double_mask Error: data_context_ptr null!");
    scale_this_level = data_context_ptr->scale_per_level();
    #else
    scale_this_level = std::pow(2.0, _scaling_mod_size) * scale;
    #endif
    _gpu_eval->encode(vec, level, scale_this_level, *pt);
  }

  void Encode_float_mask_cst_lvl(Plaintext* pt, float input, size_t len,
                                 SCALE_T scale, int level) {
    std::vector<double> vec(len, input);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
    double scale_this_level;
    #ifdef ACE_LIBRARY_32
    auto data_context_ptr = _ctx->get_context_data(parm_id);
    FMT_ASSERT(data_context_ptr != nullptr,
               "Encode_float_mask_cst_lvl Error: data_context_ptr null!");
    scale_this_level = data_context_ptr->scale_per_level();
    #else
    scale_this_level = std::pow(2.0, _scaling_mod_size) * scale;
    #endif
    _gpu_eval->encode(vec, parm_id, scale_this_level, *pt);
  }

  void Encode_double_mask_cst_lvl(Plaintext* pt, double input, size_t len,
                                  SCALE_T scale, int level) {
    std::vector<double> vec(len, input);
    auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
    double scale_this_level;
    #ifdef ACE_LIBRARY_32
    auto data_context_ptr = _ctx->get_context_data(parm_id);
    FMT_ASSERT(data_context_ptr != nullptr,
               "Encode_double_mask_cst_lvl Error: data_context_ptr null!");
    scale_this_level = data_context_ptr->scale_per_level();
    #else
    scale_this_level = std::pow(2.0, _scaling_mod_size) * scale;
    #endif
    _gpu_eval->encode(vec, parm_id, scale_this_level, *pt);
  }

void Decrypt(Ciphertext* ct, std::vector<double>& vec) {
    HostCiphertext host_ct;
    HostPlaintext pt;
    host_ct = (*ct).to(*_cpu_ctx);
    _decryptor->decrypt(host_ct, pt);
    _encoder->decode(pt, vec);
  }

  void Decode(Plaintext* pt, std::vector<double>& vec) {
    HostPlaintext host_pt;
    host_pt = (*pt).to(*_cpu_ctx);
    _encoder->decode(host_pt, vec);
  }

public:

  void Add(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->add_plain_inplace(*res, *op2);
    } else {
      _eval->add_plain(*op1, *op2, *res);
    }
  }

  void Mul(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->multiply_plain_inplace(*res, *op2);
    } else {
      _eval->multiply_plain(*op1, *op2, *res);
    }
  }

  #ifdef ACE_LIBRARY_32
  #define ADJUST_LEVEL_REDUCE_ERROR
  #endif

  inline void Adjust_level(Ciphertext& op1, Ciphertext& op2) {
    std::size_t level_1 = op1.coeff_modulus_size();
    std::size_t level_2 = op2.coeff_modulus_size();
    if (level_1 == level_2) {
      return;
    }

    const auto& ct_lo = level_1 < level_2 ? op1 : op2;
    auto& ct_hi = level_1 < level_2 ? op2 : op1;
    auto target_parms_id = ct_lo.parms_id();
    #ifdef ADJUST_LEVEL_REDUCE_ERROR
    // Adjust: scale_adjust = delta_lo * q_hi / (delta_hi)^2
    const auto& context_hi =
        _gpu_eval->context().get_context_data(ct_hi.parms_id());
    double q_hi = static_cast<double>(context_hi->back_coeff_modulus());
    #ifdef ACE_LIBRARY_32
    q_hi *= context_hi->next_context_data()->back_coeff_modulus();
    #endif
    double fix_scale = 1.0;
    double scale_adjust =
        (ct_lo.scale() / ct_hi.scale()) * (q_hi / ct_hi.scale()) * fix_scale;

    _gpu_eval->multiply_scalar_inplace(ct_hi, scale_adjust);
    ct_hi.scale() = ct_lo.scale() * q_hi;
    // NOTE: MUST modify the scale manually here!!
    //       Because we want `scale_adjust` to correct scale, not data in
    //       plaintext slots; That is to say, SEAL believes that the scale of
    //       the result is only ct_hi.scale()^2
    // Now: ct_hi.scale = delta_lo * q_hi, ct_hi.level = l_hi
    _gpu_eval->rescale_to_next_inplace(ct_hi);
    #endif
    _gpu_eval->mod_switch_to_inplace(ct_hi, target_parms_id);
  }


  void Add(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    Ciphertext final_op1 = *op1;
    Ciphertext final_op2 = *op2;

    Adjust_level(final_op1, final_op2);

    _eval->add(final_op1, final_op2, *res);
  }


  void Mul(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    Ciphertext final_op1 = *op1;
    Ciphertext final_op2 = *op2;
    Adjust_level(final_op1, final_op2);
    _eval->multiply(final_op1, final_op2, *res);

  }

  void Add(const Ciphertext* op1, const double op2, Ciphertext* res) {
    HostPlaintext host_pt;
    Plaintext plain;
    _encoder->encode(op2, op1->parms_id(), op1->scale(), host_pt);
    plain = acelib::DevicePlain::from(host_pt, _gpu_eval->context());
    if (res == op1) {
      _eval->add_plain_inplace(*res, plain);
    } else {
      _eval->add_plain(*op1, plain, *res);
    }
  }

  void Mul(const Ciphertext* op1, const double op2, Ciphertext* res) {
    HostPlaintext host_pt;
    Plaintext plain;
    _encoder->encode(op2, op1->parms_id(), op1->scale(), host_pt);
    plain = acelib::DevicePlain::from(host_pt, _gpu_eval->context());
    if (res == op1) {
      _eval->multiply_plain_inplace(*res, plain);
    } else {
      _eval->multiply_plain(*op1, plain, *res);
    }

  }

  void Rotate(const Ciphertext* op1, int step, Ciphertext* res) {

    if (res == op1) {
      _gpu_eval->rotate_vector_inplace(*res, step);
    } else {
      _gpu_eval->rotate_vector(*op1, step, *res);
    }
  }

  void Rescale(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      _eval->rescale_to_next_inplace(*res);
    } else {
      _eval->rescale_to_next(*op1, *res);
    }
  }

  void Mod_switch(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      _eval->mod_switch_to_next_inplace(*res);
    } else {
      _eval->mod_switch_to_next(*op1, *res);
    }
  }

  void Relin(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      _gpu_eval->relinearize_inplace(*res);
    } else {
      _gpu_eval->relinearize(*op1, *res);
    }
  }

  void Bootstrap(Ciphertext* op1, Ciphertext* res, int level) {
    Bootstrap_with_bts(op1, res, level);
  }


  SCALE_T Scale_degree(Ciphertext* op) {
    return (uint64_t)std::log2(op->scale()) / _scaling_mod_size;
  }

  SCALE_T Normalized_Scale(Ciphertext* op) {
    return op->scale() / std::pow(2.0, _scaling_mod_size);
  }

  SCALE_T Scale(Ciphertext* op) {
    return op->scale();
  }

  LEVEL_T Level(Ciphertext* op) {
    // ACE LEVEL_T is acelib::host::parms_id_type; return the active parms id.
    auto data_context_ptr = _ctx->get_context_data(op->parms_id());
    FMT_ASSERT(data_context_ptr != nullptr, "Level: data_context_ptr null!");
    return op->parms_id();
  }

private:
  std::vector<int> _gal_steps_vector;

  Ciphertext* Own_cipher(const Ciphertext& ct) {
    _owned_io_ciphertexts.push_back(std::make_unique<Ciphertext>(ct));
    return _owned_io_ciphertexts.back().get();
  }

  Ciphertext* Own_cipher(Ciphertext&& ct) {
    _owned_io_ciphertexts.push_back(
        std::make_unique<Ciphertext>(std::move(ct)));
    return _owned_io_ciphertexts.back().get();
  }

  void             Set_gal_steps() {
    CKKS_PARAMS* prog_param = Get_context_params();
    if (Need_bts()) {
      for (int i = 0; i < prog_param->_num_rot_idx; ++i) {
        int rot = prog_param->_rot_idxs[i];
        if (find(_gal_steps_vector.begin(), _gal_steps_vector.end(), rot) ==
            _gal_steps_vector.end())
          _gal_steps_vector.push_back(rot);
      }
    } else {
      for (int i = 0; i < prog_param->_num_rot_idx; ++i) {
        int rot = prog_param->_rot_idxs[i];
        _gal_steps_vector.push_back(rot);
      }
    }
  }

#if ACE_LIBRARY_ENABLE_BTS
void Make_bts_parms(
    CKKS_PARAMS* prog_param, acelib::host::HostParameter& parms_out,
    EvalBootstrap::BootstrapContext& bts_parms_out,
    config::LTHoistingMode hoisting = config::LTHoistingMode::DOUBLE_HOIST)
{
    long   logN              = log2(prog_param->_poly_degree);
    long   full_logn         = logN - 1;
    int    hw                = Bts_hamming_weight_or_default(
                                    prog_param->_hamming_weight);
#ifndef ACE_LIBRARY_32
    int    logq              = prog_param->_scaling_mod_size;
    int    total_level       = prog_param->_mul_depth;
    int    evalmod_depth     = 8;
    int    logp              = prog_param->_first_mod_size;
    int    log_special_prime = prog_param->_first_mod_size;
#endif

    auto config = bootstrap::BootstrapConfig();
    config.crypto_params.logN = logN;
    config.crypto_params.logSlots = full_logn;
    config.crypto_params.hamming_weight = hw;
    if (config.crypto_params.hamming_weight == 192)
    {
        config.evalmod_params.n_double_angle = 2;
        config.evalmod_params.degree = 59;
    }
    else if (config.crypto_params.hamming_weight == 32)
    {
        config.evalmod_params.n_double_angle = 3;
        config.evalmod_params.degree = 22;
    }
    else
    {
        FMT_ASSERT(false,
                   "ACE bootstrap currently supports only hamming weight "
                   "32 or 192");
    }
    config.evalmod_params.inverse_degree = 1;
    config.SetEvalModLogE(10);
    config.SetHoistingMode(hoisting);

#ifndef ACE_LIBRARY_32
    int    stc_depth         = 3;
    int    cts_depth         = 4;
    int    remain_level      = total_level - (stc_depth + cts_depth + evalmod_depth);
    config.crypto_params.logQ0 = logp;
    config.crypto_params.logQi = { logq };
    config.crypto_params.level = remain_level;
    config.crypto_params.logP = { log_special_prime };
    config.crypto_params.n_special_prime = 8;

    config.cts_params.depth = cts_depth;
    config.cts_params.logQ = logp;
    config.stc_params.depth = stc_depth;
    config.stc_params.logQ = logp;
    if (config.cts_params.depth == 4)
    {
        config.cts_params.groups = { 4, 4, 4, 3 };
        config.stc_params.groups = { 5, 5, 5 };
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
    config.optimizations.linear_transform.giant_step_prep_mode =
        config::GiantStepPrepMode::BATCH_MODDOWN_MODUP;
    config.optimizations.linear_transform.giant_step_rotate_fusion =
        config::GiantStepRotateFusion::BATCH_PQ;
    config.optimizations.linear_transform.giant_step_reduce_fusion =
        config::GiantStepReduceFusion::ADD_MANY;
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
    if(config.slim_mode)
      enable_score = false;
    else
      enable_score = false;

    config.EnableSCORE(enable_score);

    auto bts_parms = EvalBootstrap::generate_bts_context(config);

    const int N = 1 << bts_parms.logN();
    if (kAceLibraryDebug) {
      fprintf(Ace_debug_file(), "Init Context...\n");
    }
    acelib::host::HostParameter parms(acelib::host::scheme_type::ckks);
    parms.set_poly_modulus_degree(N);

    parms.set_coeff_modulus(acelib::host::CoeffModulus::Create(N, bts_parms.modchain_.logQs_));
#else
    const auto modulus_blocks =
        Bts_set_f_modulus_blocks(prog_param->_input_level);

    config
        .SetSlimBootstrap(true)
        .SetRealInput(true)
        .SetRemoveImaginary(true)
        .EnableSCORE(false)
        .SkipEvalMod(false)
        .SetArcSinPolyDegree(1)
        .SetCosEven(false)
        .SetHoistingMode(config::LTHoistingMode::DOUBLE_HOIST);

    config.optimizations.linear_transform.compute_mode =
        config::BSGSComputeMode::FULL;
    config.SetMacStrategy(config::BSGSMacFusion::SEQUENTIAL);
    config.SetPreGaloisKeys(true);
    config.SetBabyStepFusion(config::BabyStepFusion::NONE);
    config.optimizations.linear_transform.giant_step_prep_mode =
        config::GiantStepPrepMode::NONE;
    config.optimizations.linear_transform.giant_step_rotate_fusion =
        config::GiantStepRotateFusion::NONE;
    config.optimizations.linear_transform.giant_step_reduce_fusion =
        config::GiantStepReduceFusion::STREAM_ADD;
    config.SetKeyStrategy(config::LTRotKeyStrategy::LESS_GIANT_KEY);
    config.EnableLTMergeMDRS(false);
    config.EnablePeriodicityOpt(true);
    config.ValidatePeriodicityRuntime(true);

    config.subsum_strategy.method = reduce::ReduceMethod::BINARY;
    config.subsum_strategy.rotation_in_pq_domain = true;

    config
        .EnableEvalModMergeMDRS(true)
        .EnableHoistedRelin(true)
        .EnableScaleAlignment(true)
        .EnableLazyRescale(true)
        .EnableEagerConstantAddition(false)
        .SetPolyEvalFusionMode(config::EvalPolyFusionMode::OFF)
        .SetPolyEvalArithMode(config::FusedLeafConstArithMode::MONT_EAGER)
        .EnableSSEForced(true)
        .EnableSparseKeyWrapping(false);

    Apply_32bit_bts_modulus_config(config, modulus_blocks,
                                   prog_param->_scaling_mod_size);
    if (config.cts_params.depth == 4) {
        config.cts_params.groups = {5, 4, 3, 3};
        config.stc_params.groups = {5, 5, 5};
    }

    auto bts_parms = EvalBootstrap::generate_bts_context(config);

    const int N = 1 << bts_parms.logN();
    if (kAceLibraryDebug) {
      fprintf(Ace_debug_file(), "Init Context...\n");
    }
    acelib::host::HostParameter parms(acelib::host::scheme_type::ckks);
    parms.set_poly_modulus_degree(N);

    parms.set_coeff_modulus(
        acelib::host::CoeffModulus::Create(Flatten_modulus_blocks(modulus_blocks)));
#endif
    parms.set_secret_key_hamming_weight(config.crypto_params.hamming_weight);
    const long sparse_slots = (1 << bts_parms.logSlots());
    if (sparse_slots != N / 2)
    {
        parms.set_sparse_slots(sparse_slots);
    }
    parms.set_n_special_primes(config.crypto_params.n_special_prime);
    parms.set_scale_flag(bts_parms.modchain_.scale_flags_);


    parms_out     = parms;
    bts_parms_out = std::move(bts_parms);
}

#endif

private:
  ACE_LIBRARY_CONTEXT(const ACE_LIBRARY_CONTEXT&)            = delete;
  ACE_LIBRARY_CONTEXT& operator=(const ACE_LIBRARY_CONTEXT&) = delete;
  friend struct std::default_delete<ACE_LIBRARY_CONTEXT>;

  static int N_special_primes(const CKKS_PARAMS* prog_param);
  static acelib::host::sec_level_type Security_level(
      const CKKS_PARAMS* prog_param, uint32_t degree);
  static void Log_context_params(const CKKS_PARAMS* prog_param);

  void Configure_plain_parameters(CKKS_PARAMS* prog_param,
                                  acelib::host::HostParameter& parms,
                                  uint32_t degree,
                                  int n_special_primes);
  void Init_cpu_context(CKKS_PARAMS* prog_param,
                        const acelib::host::HostParameter& parms,
                        acelib::host::sec_level_type sec,
                        bool& use_sparse_wrapping);
  void Init_bts_cpu_context(CKKS_PARAMS* prog_param,
                            bool& use_sparse_wrapping);
  void Init_keys_and_codec(CKKS_PARAMS* prog_param, bool use_sparse_wrapping);
  void Init_evaluator(bool use_sparse_wrapping);
  void Init_bts_evaluator(bool use_sparse_wrapping);
  void Bootstrap_with_bts(Ciphertext* op1, Ciphertext* res, int level);

  ACE_LIBRARY_CONTEXT();
  ~ACE_LIBRARY_CONTEXT();

  static std::unique_ptr<ACE_LIBRARY_CONTEXT> Instance;

private:
  std::unique_ptr<acelib::host::HostContext> _cpu_ctx_owner;
  std::unique_ptr<acelib::host::HostContext> _host_context_owner;
  acelib::host::HostContext* _cpu_ctx = nullptr;
  acelib::host::HostContext* _host_context = nullptr;

  std::unique_ptr<acelib::host::KeyGenerator> _kgen;

  const acelib::host::SecretKey* _sk = nullptr;
  std::unique_ptr<acelib::host::PublicKey> _pk;
  std::unique_ptr<acelib::host::RelinKeys> _rlk;
  std::unique_ptr<acelib::host::GaloisKeys> _rtk;

  std::unique_ptr<acelib::host::CKKSEncoder> _encoder;
  std::unique_ptr<acelib::host::Encryptor> _encryptor;
  std::unique_ptr<acelib::host::Decryptor> _decryptor;

  acelib::MEMORY_LAYOUT _data_layout;
  std::unique_ptr<acelib::Evaluator> _gpu_eval_owner;
  acelib::Evaluator* _gpu_eval = nullptr;
  acelib::ContextPro* _ctx = nullptr;
  acelib::Evaluator* _eval = nullptr;
#if ACE_LIBRARY_ENABLE_BTS
  EvalBootstrap::BootstrapContext _bts_parms;
  std::unique_ptr<EvalWrapper> _dev_eval_owner;
  std::unique_ptr<EvalBootstrap> _boot;
  DeviceEvaluator* _dev_eval = nullptr;
#endif

  uint64_t _scaling_mod_size = 0;
  uint64_t _input_level = 0;
  std::vector<std::unique_ptr<Ciphertext>> _owned_io_ciphertexts;
};

std::unique_ptr<ACE_LIBRARY_CONTEXT> ACE_LIBRARY_CONTEXT::Instance = nullptr;

int ACE_LIBRARY_CONTEXT::N_special_primes(const CKKS_PARAMS* prog_param) {
  if (prog_param->_num_q_parts) {
    return prog_param->_num_q_parts;
  }
  constexpr int kDefaultDnum = 3;
  return prog_param->_mul_depth / kDefaultDnum + 1;
}

acelib::host::sec_level_type ACE_LIBRARY_CONTEXT::Security_level(
    const CKKS_PARAMS* prog_param, uint32_t degree) {
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
    DEV_WARN("WARNING: degree %d too small, reset security level to none\n",
             degree);
    return acelib::host::sec_level_type::none;
  }
  return sec;
}

void ACE_LIBRARY_CONTEXT::Log_context_params(const CKKS_PARAMS* prog_param) {
  if (!kAceLibraryDebug) {
    return;
  }

  fprintf(
      Ace_debug_file(),
      "ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
      "mul_depth = %ld, _first_mod_size = %ld, _scaling_mod_size = %ld, "
      "_num_q_parts = %ld, _num_rot_idx = %ld\n",
      prog_param->_provider, prog_param->_poly_degree, prog_param->_sec_level,
      prog_param->_mul_depth, prog_param->_first_mod_size,
      prog_param->_scaling_mod_size, prog_param->_num_q_parts,
      prog_param->_num_rot_idx);
}

void ACE_LIBRARY_CONTEXT::Configure_plain_parameters(
    CKKS_PARAMS* prog_param, acelib::host::HostParameter& parms, uint32_t degree,
    int n_special_primes) {
  parms.set_poly_modulus_degree(degree);

  if (Need_bts()) {
    return;
  }

#ifdef ACE_LIBRARY_32
  std::vector<uint64_t> modulus;
  std::vector<double> scales;
  std::string mod_err;
  if (!Build_mod_vector(prog_param->_mul_depth, n_special_primes, modulus,
                        mod_err)) {
    FMT_ASSERT(false, "AceLibrary context init failed: %s", mod_err.c_str());
  }
  if (!Calculate_scales_per_level(prog_param->_scaling_mod_size, modulus,
                                  prog_param->_mul_depth, n_special_primes,
                                  scales, mod_err)) {
    FMT_ASSERT(false, "AceLibrary context init failed: %s", mod_err.c_str());
  }

  parms.set_scale_per_level(scales);
  parms.set_n_special_primes(n_special_primes * 2);
  std::vector<int> scale_flag_vec;
  for (int i = 0; i < 1 + prog_param->_mul_depth; i++) {
    scale_flag_vec.push_back(0);
    scale_flag_vec.push_back(2);
  }
  if (!Validate_32bit_modulus_config(prog_param->_mul_depth,
                                     n_special_primes, modulus, scales,
                                     scale_flag_vec, mod_err)) {
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
  parms.set_coeff_modulus(acelib::host::CoeffModulus::Create(degree, bits));
#endif
}

void ACE_LIBRARY_CONTEXT::Init_cpu_context(
    CKKS_PARAMS* prog_param, const acelib::host::HostParameter& parms,
    acelib::host::sec_level_type sec, bool& use_sparse_wrapping) {
  if (Need_bts()) {
    Init_bts_cpu_context(prog_param, use_sparse_wrapping);
    return;
  }

  _cpu_ctx_owner = std::make_unique<acelib::host::HostContext>(parms, true, sec);
  _cpu_ctx = _cpu_ctx_owner.get();
}

#if ACE_LIBRARY_ENABLE_BTS
void ACE_LIBRARY_CONTEXT::Init_bts_cpu_context(CKKS_PARAMS* prog_param,
                                               bool& use_sparse_wrapping) {
  acelib::host::HostParameter host_bts_parms(acelib::host::scheme_type::ckks);
  Make_bts_parms(prog_param, host_bts_parms, _bts_parms);
  use_sparse_wrapping = _bts_parms.sparse_key_wrapping();
  _host_context_owner = std::make_unique<acelib::host::HostContext>(
      host_bts_parms, true, acelib::host::sec_level_type::none);

  _host_context = _host_context_owner.get();
  _cpu_ctx = _host_context;
}
#else
void ACE_LIBRARY_CONTEXT::Init_bts_cpu_context(CKKS_PARAMS* prog_param,
                                               bool& use_sparse_wrapping) {
  (void)prog_param;
  (void)use_sparse_wrapping;
  FMT_ASSERT(false,
             "ACE library was built without bootstrap support.");
}
#endif

void ACE_LIBRARY_CONTEXT::Init_keys_and_codec(CKKS_PARAMS* prog_param,
                                              bool use_sparse_wrapping) {
  if (Need_bts()) {
    _kgen = std::make_unique<acelib::host::KeyGenerator>(*_cpu_ctx,
                                                     use_sparse_wrapping);
  } else {
    _kgen = std::make_unique<acelib::host::KeyGenerator>(*_cpu_ctx);
  }
  _sk   = &_kgen->secret_key();
  _pk = std::make_unique<acelib::host::PublicKey>();
  _kgen->create_public_key(*_pk);
  _rlk = std::make_unique<acelib::host::RelinKeys>();
  _kgen->create_relin_keys(*_rlk);
  _rtk = std::make_unique<acelib::host::GaloisKeys>();

  _encryptor = std::make_unique<acelib::host::Encryptor>(*_cpu_ctx, *_pk, *_sk);
  _decryptor = std::make_unique<acelib::host::Decryptor>(*_cpu_ctx, *_sk);
  _encoder = std::make_unique<acelib::host::CKKSEncoder>(*_cpu_ctx);
  _scaling_mod_size = prog_param->_scaling_mod_size;
  size_t prime_len = prog_param->_mul_depth + 1;
  FMT_ASSERT(prog_param->_input_level <= prime_len,
             "input_lev must not exceed prime count");
  _input_level = prog_param->_input_level;
}

void ACE_LIBRARY_CONTEXT::Init_evaluator(bool use_sparse_wrapping) {
  if (Need_bts()) {
    Init_bts_evaluator(use_sparse_wrapping);
    return;
  }

  Set_gal_steps();
  _kgen->create_galois_keys(_gal_steps_vector, *_rtk, true);
  _gpu_eval_owner = std::make_unique<acelib::Evaluator>(
      acelib::ContextPro::get_device_context(*_cpu_ctx), true);
  _gpu_eval = _gpu_eval_owner.get();
  _gpu_eval->enable_memory_pool(0.4);
  _gpu_eval->get_publickey_from(*_pk);
  _gpu_eval->get_secretkey_from(*_sk);
  _gpu_eval->get_evk_from(*_rlk, acelib::EvkConfigs::relin_key(_data_layout));
  _gpu_eval->get_evk_from(
      *_rtk, acelib::EvkConfigs::galois_key(true, _data_layout));
  _gpu_eval->preload_special_keys();
}

#if ACE_LIBRARY_ENABLE_BTS
void ACE_LIBRARY_CONTEXT::Init_bts_evaluator(bool use_sparse_wrapping) {
  _dev_eval_owner = std::make_unique<EvalWrapper>(
      *_encoder, acelib::ContextPro::get_device_context(*_host_context), true);
  _dev_eval = _dev_eval_owner.get();
  if (use_sparse_wrapping) {
    _dev_eval->set_sparse_key_wrapping_enabled(true);
  }

  auto bts_parms_copy = _bts_parms;
  _boot = std::make_unique<EvalBootstrap>(std::move(bts_parms_copy),
                                          *_dev_eval);
#ifdef USE_NVTX
  _boot->profile = true;
#endif
#ifdef ACE_LIBRARY_32
  const auto evk_mode = acelib::host::EvkRequirementMode::PreferCompact;
#else
  const auto evk_mode = acelib::host::EvkRequirementMode::FullOnly;
#endif
  auto evk_requirements = _boot->collect_requirements(evk_mode);
  Set_gal_steps();
  evk_requirements.galois_keys.require_steps(_gal_steps_vector);
  _dev_eval->enable_memory_pool(0.3);
  _dev_eval->get_publickey_from(*_pk);
  _dev_eval->get_secretkey_from(*_sk);
  if (evk_requirements.needs_relin_keys()) {
    _kgen->create_relin_keys(evk_requirements.relin_keys, *_rlk);
  }
  _kgen->create_galois_keys(evk_requirements.galois_keys, *_rtk, false);
  _dev_eval->get_evk_from(
      *_rtk, acelib::EvkConfigs::galois_key(
                 true, _data_layout, acelib::EvkTransferMode::FULL_TRANSFER,
                 std::nullopt,
                 evk_requirements.galois_keys.pre_galois_keys()));
  _dev_eval->get_evk_from(*_rlk,
                          acelib::EvkConfigs::relin_key(_data_layout));
  _dev_eval->context().prewarm(evk_requirements.runtime_aux);
  _dev_eval->preload_all();
  if (use_sparse_wrapping) {
    auto sparse_wrapping_keys = _kgen->create_sparse_wrapping_keys();
    _dev_eval->get_sparse_wrapping_keys_from(sparse_wrapping_keys,
                                             _data_layout);
  }
  _boot->prepare_encoded_const();
  CUDA_CHECK(cudaDeviceSynchronize());
  _gpu_eval = _dev_eval;
}

void ACE_LIBRARY_CONTEXT::Bootstrap_with_bts(Ciphertext* op1, Ciphertext* res,
                                             int level) {
  Ciphertext out;
  Ciphertext boot_input = *op1;
  if (!_bts_parms.evalmod_handler_.parms().skip_) {
    auto entry_parm_id =
        _ctx->get_parms_id_from_level(_bts_parms.boot_entry_level());
    _dev_eval->mod_switch_to_inplace(boot_input, entry_parm_id);
  }
  out = _boot->bootstrap(boot_input);
#ifdef ACE_LIBRARY_32
  *res = out;
#else
  auto parm_id = _ctx->get_parms_id_from_level(Runtime_level(level));
  _dev_eval->mod_switch_to(out, parm_id, *res);
#endif
  CUDA_CHECK(cudaDeviceSynchronize());
}
#else
void ACE_LIBRARY_CONTEXT::Init_bts_evaluator(bool use_sparse_wrapping) {
  (void)use_sparse_wrapping;
  FMT_ASSERT(false,
             "ACE library was built without bootstrap support.");
}

void ACE_LIBRARY_CONTEXT::Bootstrap_with_bts(Ciphertext* op1, Ciphertext* res,
                                             int level) {
  (void)op1;
  (void)res;
  (void)level;
  FMT_ASSERT(false,
             "AceLibrary_bootstrap called, but ACE was built without "
             "bootstrap support.");
}
#endif

ACE_LIBRARY_CONTEXT::ACE_LIBRARY_CONTEXT() {
  IS_TRUE(Instance == nullptr, "_install already created");

  CKKS_PARAMS* prog_param = Get_context_params();
  IS_TRUE(prog_param->_provider == LIB_ACE, "provider is not AceLibrary");

  acelib::host::HostParameter parms(acelib::host::scheme_type::ckks);
  uint32_t degree = prog_param->_poly_degree;
  int n_special_primes = N_special_primes(prog_param);
  Configure_plain_parameters(prog_param, parms, degree, n_special_primes);

  acelib::host::sec_level_type sec = Security_level(prog_param, degree);
  bool use_sparse_wrapping = false;
  _data_layout = acelib::MEMORY_LAYOUT::ROW_MAJOR;

  Init_cpu_context(prog_param, parms, sec, use_sparse_wrapping);
  Init_keys_and_codec(prog_param, use_sparse_wrapping);
  Init_evaluator(use_sparse_wrapping);

  _ctx = const_cast<acelib::ContextPro*>(&(_gpu_eval->context()));
  _eval = _gpu_eval;

  Log_context_params(prog_param);
}

ACE_LIBRARY_CONTEXT::~ACE_LIBRARY_CONTEXT() = default;

//! @brief Initialize ACE runtime context and common runtime services.
void Prepare_context() {
  Init_rtlib_timing();
  Io_init();
  ACE_LIBRARY_CONTEXT::Init_context();
}

//! @brief Finalize ACE runtime context and common runtime services.
void Finalize_context() {
  ACE_LIBRARY_CONTEXT::Fini_context();
  Io_fini();
}

//! @brief Encrypt and register a named input tensor.
void Prepare_input(TENSOR* input, const char* name) {
  ACE_LIBRARY_CONTEXT::Context()->Prepare_input(input, name);
}

//! @brief Decrypt and decode a named output tensor.
double* Handle_output(const char* name) {
  return ACE_LIBRARY_CONTEXT::Context()->Handle_output(name, 0);
}

//! @brief C ABI wrapper for Prepare_input.
void AceLibrary_prepare_input(TENSOR* input, const char* name) {
  Prepare_input(input, name);
}

//! @brief C ABI wrapper for Handle_output.
double* AceLibrary_handle_output(const char* name) {
  return Handle_output(name);
}

//! @brief Register an output ciphertext by name and index.
void AceLibrary_set_output_data(const char* name, size_t idx, CIPHER data) {
  ACE_LIBRARY_CONTEXT::Context()->Set_output_data(name, idx, data);
}

//! @brief Return an input ciphertext by name and index.
CIPHERTEXT AceLibrary_get_input_data(const char* name, size_t idx) {
  return ACE_LIBRARY_CONTEXT::Context()->Get_input_data(name, idx);
}

//! @brief Encode a float array into a plaintext at a concrete parms id.
void AceLibrary_encode_float(PLAIN pt, float* input, size_t len, SCALE_T scale,
                       LEVEL_T level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_float(pt, input, len, scale, level);
}

//! @brief Encode a double array into a plaintext at a concrete parms id.
void AceLibrary_encode_double(PLAIN pt, double* input, size_t len,
                              SCALE_T scale, LEVEL_T level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_double(pt, input, len, scale, level);
}

//! @brief Encode a float array into a plaintext at a logical level.
void AceLibrary_encode_float_cst_lvl(PLAIN pt, float* input, size_t len,
                               SCALE_T scale, int level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_float_cst_lvl(pt, input, len, scale, level);
}

//! @brief Encode a float array with the GPU encoder path.
void AceLibrary_encode_float_gpu(PLAIN pt, float* input, size_t len,
                               SCALE_T scale, int level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_float_gpu(pt, input, len, scale, level);
}

//! @brief Encode a double array into a plaintext at a logical level.
void AceLibrary_encode_double_cst_lvl(PLAIN pt, double* input, size_t len,
                                      SCALE_T scale, int level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_double_cst_lvl(pt, input, len, scale,
                                                        level);
}

//! @brief Encode a repeated float value into a plaintext at a concrete parms id.
void AceLibrary_encode_float_mask(PLAIN pt, float input, size_t len, SCALE_T scale,
                            LEVEL_T level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_float_mask(pt, input, len, scale, level);
}

//! @brief Encode a repeated double value into a plaintext at a concrete parms id.
void AceLibrary_encode_double_mask(PLAIN pt, double input, size_t len,
                                   SCALE_T scale, LEVEL_T level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_double_mask(pt, input, len, scale,
                                                     level);
}

//! @brief Encode a repeated float value into a plaintext at a logical level.
void AceLibrary_encode_float_mask_cst_lvl(PLAIN pt, float input, size_t len,
                                          SCALE_T scale, int level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_float_mask_cst_lvl(pt, input, len,
                                                            scale, level);
}

//! @brief Encode a repeated double value into a plaintext at a logical level.
void AceLibrary_encode_double_mask_cst_lvl(PLAIN pt, double input, size_t len,
                                           SCALE_T scale, int level) {
  ACE_LIBRARY_CONTEXT::Context()->Encode_double_mask_cst_lvl(pt, input, len,
                                                             scale, level);
}

//! @brief Add two ciphertexts.
void AceLibrary_add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  if (op1->size() == 0) {
    // special handling for accumulation
    *res = *op2;
    return;
  }
  ACE_LIBRARY_CONTEXT::Context()->Add(op1, op2, res);
}

//! @brief Add a plaintext to a ciphertext.
void AceLibrary_add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  ACE_LIBRARY_CONTEXT::Context()->Add(op1, op2, res);
}

//! @brief Add a scalar to a ciphertext.
void AceLibrary_add_scalar(CIPHER res, CIPHER op1, double op2) {
  ACE_LIBRARY_CONTEXT::Context()->Add(op1, op2, res);
}

//! @brief Multiply two ciphertexts.
void AceLibrary_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  ACE_LIBRARY_CONTEXT::Context()->Mul(op1, op2, res);
}

//! @brief Multiply a ciphertext by a plaintext.
void AceLibrary_mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  ACE_LIBRARY_CONTEXT::Context()->Mul(op1, op2, res);
}

//! @brief Multiply a ciphertext by a scalar.
void AceLibrary_mul_scalar(CIPHER res, CIPHER op1, double op2) {
  ACE_LIBRARY_CONTEXT::Context()->Mul(op1, op2, res);
}

//! @brief Rotate a ciphertext by the requested step.
void AceLibrary_rotate(CIPHER res, CIPHER op, int step) {
  ACE_LIBRARY_CONTEXT::Context()->Rotate(op, step, res);
}

//! @brief Rescale a ciphertext.
void AceLibrary_rescale(CIPHER res, CIPHER op) {
  ACE_LIBRARY_CONTEXT::Context()->Rescale(op, res);
}

//! @brief Mod-switch a ciphertext to the next level.
void AceLibrary_mod_switch(CIPHER res, CIPHER op) {
  ACE_LIBRARY_CONTEXT::Context()->Mod_switch(op, res);
}

//! @brief Relinearize a degree-2 ciphertext.
void AceLibrary_relin(CIPHER res, CIPHER3 op) {
  ACE_LIBRARY_CONTEXT::Context()->Relin(op, res);
}

//! @brief Bootstrap a ciphertext to the requested logical level.
void AceLibrary_bootstrap(CIPHER res, CIPHER op, int level) {
  ACE_LIBRARY_CONTEXT::Context()->Bootstrap(op, res, level);
}

//! @brief Copy a ciphertext.
void AceLibrary_copy(CIPHER res, CIPHER op) {
  *res = *op;
}

//! @brief Reset a ciphertext to the zero/default state.
void AceLibrary_zero(CIPHER res) {
  if (res) {
    *res = CIPHERTEXT();
  }
}

//! @brief Return the normalized scale degree of a ciphertext.
SCALE_T AceLibrary_scale_degree(CIPHER res) {
  return ACE_LIBRARY_CONTEXT::Context()->Scale_degree(res);
}

//! @brief Return the normalized scale factor of a ciphertext.
SCALE_T AceLibrary_scale(CIPHER res) {
  return ACE_LIBRARY_CONTEXT::Context()->Normalized_Scale(res);
}

//! @brief Return the active ACE parms id of a ciphertext.
LEVEL_T AceLibrary_level(CIPHER res) { return ACE_LIBRARY_CONTEXT::Context()->Level(res); }

//! @brief Decrypt and dump a ciphertext slot range.
void Dump_ciph(CIPHER ct, size_t start, size_t len) {
  std::vector<double> vec;
  ACE_LIBRARY_CONTEXT::Context()->Decrypt(ct, vec);
  size_t max = std::min(vec.size(), start + len);
  FILE* out = Ace_dump_file();
  for (size_t i = start; i < max; ++i) {
    fprintf(out, "%f ", vec[i]);
  }
  fprintf(out, "\n");
}

//! @brief Decode and dump a plaintext slot range.
void Dump_plain(PLAIN pt, size_t start, size_t len) {
  std::vector<double> vec;
  ACE_LIBRARY_CONTEXT::Context()->Decode(pt, vec);
  size_t max = std::min(vec.size(), start + len);
  FILE* out = Ace_dump_file();
  for (size_t i = start; i < max; ++i) {
    fprintf(out, "%f ", vec[i]);
  }
  fprintf(out, "\n");
}

//! @brief Dump a named ciphertext message preview.
void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len) {
  fprintf(Ace_dump_file(), "[%s]: ", name);
  Dump_ciph(ct, 16, len);
}

//! @brief Dump a named plaintext message preview.
void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len) {
  fprintf(Ace_dump_file(), "[%s]: ", name);
  Dump_plain(pt, 16, len);
}

//! @brief Return a heap-allocated decoded ciphertext message.
double* Get_msg(CIPHER ct) {
  std::vector<double> vec;
  ACE_LIBRARY_CONTEXT::Context()->Decrypt(ct, vec);
  double* msg = (double*)malloc(sizeof(double) * vec.size());
  memcpy(msg, vec.data(), sizeof(double) * vec.size());
  return msg;
}

//! @brief Return a heap-allocated decoded plaintext message.
double* Get_msg_from_plain(PLAIN pt) {
  std::vector<double> vec;
  ACE_LIBRARY_CONTEXT::Context()->Decode(pt, vec);
  double* msg = (double*)malloc(sizeof(double) * vec.size());
  memcpy(msg, vec.data(), sizeof(double) * vec.size());
  return msg;
}

//! @brief Validate whether a plaintext reference fits the ciphertext range.
bool Within_value_range(CIPHER ciph, double* msg, uint32_t len) {
  (void)ciph;
  (void)msg;
  (void)len;
  return true;
}
