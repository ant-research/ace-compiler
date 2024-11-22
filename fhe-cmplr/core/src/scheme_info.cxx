//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/scheme_info.h"

#include <cmath>

#include "air/util/debug.h"

namespace fhe {
namespace core {

#define HAMMING_WEIGHT_THRESHOLD 192
// Mul_depth of CKKS bootstrapping is determined by the impl in rtlib.
// Following mul_depth values must be modified according to the
// parameters setted in:
//   rtlib/ant/include/util/ckks_bootstrap_context.h
//   rtlib/ant/src/ckks/cipher_eval.c:Bootstrap.
// CKKS bootstrapping is composed of 4 steps: Mod_raise, Coeff2slot, Mod_reduce,
// and Slot2coeff. Mod_raise consumes zero mul_depth.
// Bootstrapping mul_depth= (Coeff2slot + Mod_reduce + Slot2coeff).
// In current impl, mul_depth of Coeff2slot and Slot2coeff are both 3.
// Definitional domain of Mod_reduce decreases with hamming weight.
// Therefore, mul_depth of Mod_reduce decreases with hamming weight.
// In case of hamming weight <= 192, Mod_reduce consumes 9 mul_depth,
// and mul_depth of bootstrapping is 15.
// In case of hamming weight > 192, Mod_reduce consumes 13 mul_depth,
// and mul_depth of bootstrapping is 19.
#define BOOTSTRAP_MUL_DEPTH_UNDER_THRESHOLD 15
#define BOOTSTRAP_MUL_DEPTH_ABOVE_THRESHOLD 19

#define HIGH_MUL_LEVEL_THRESHOLD 18

// bit number of P prime of CRT primes
#define BIT_NUM_OF_P_PRIME 60

enum PRIME_INFO_KIND : uint32_t {
  LOW_MUL_LEVEL  = 0,  // mul_level in [1, 18)
  HIGH_MUL_LEVEL = 1,  // mul_level in [18, -)
  LAST           = 2,
};

// prime info for mul_level in [1, 18)
static const CTX_PARAM::PRIME_INFO Low_mul_level_prime_info[] = {
    {33, 30},
};

#define LEAST_POLY_DEG_POW 3
// prime info for mul_level in [18, -)
static const CTX_PARAM::PRIME_INFO High_mul_level_prime_info[] = {
    CTX_PARAM::PRIME_INFO(60, 50),  // poly_deg = 2^3
    CTX_PARAM::PRIME_INFO(60, 51),  // poly_deg = 2^4
    CTX_PARAM::PRIME_INFO(60, 51),  // poly_deg = 2^5
    CTX_PARAM::PRIME_INFO(60, 53),  // poly_deg = 2^6
    CTX_PARAM::PRIME_INFO(60, 54),  // poly_deg = 2^7
    CTX_PARAM::PRIME_INFO(60, 54),  // poly_deg = 2^8
    CTX_PARAM::PRIME_INFO(60, 54),  // poly_deg = 2^9
    CTX_PARAM::PRIME_INFO(60, 56),  // poly_deg = 2^10
    CTX_PARAM::PRIME_INFO(60, 58),  // poly_deg = 2^11
    CTX_PARAM::PRIME_INFO(60, 58),  // poly_deg = 2^12
    CTX_PARAM::PRIME_INFO(60, 59),  // poly_deg = 2^13
    CTX_PARAM::PRIME_INFO(60, 59),  // poly_deg = 2^14
    CTX_PARAM::PRIME_INFO(60, 59),  // poly_deg = 2^15
    CTX_PARAM::PRIME_INFO(60, 59),  // poly_deg = 2^16
};

void CTX_PARAM::Update_prime_info() {
  // 1. update bit number of q0 and sf
  const PRIME_INFO* prime_info = nullptr;
  if (Get_mul_level() >= HIGH_MUL_LEVEL_THRESHOLD) {
    uint32_t poly_deg_pow = round(log2(Get_poly_degree()));
    AIR_ASSERT(poly_deg_pow >= LEAST_POLY_DEG_POW);
    uint32_t prime_info_id = poly_deg_pow - LEAST_POLY_DEG_POW;
    prime_info             = &High_mul_level_prime_info[prime_info_id];
  } else {
    prime_info = &Low_mul_level_prime_info[0];
  }
  AIR_ASSERT(prime_info != nullptr);
  Set_first_prime_bit_num(prime_info->First_prime_bit_num());
  Set_scaling_factor_bit_num(prime_info->Scale_factor_bit_num());

  // 2. update q_part_num
  if (Get_mul_level() > 3)
    Set_q_part_num(3);
  else if (Get_mul_level() == 0)
    Set_q_part_num(1);
  else
    Set_q_part_num(2);
}

void CTX_PARAM::Print(std::ostream& out) {
  std::string indent(4, ' ');
  out << "CTX_PARAM {" << std::endl;
  out << indent << Get_poly_degree() << ", // poly degree" << std::endl;
  out << indent << Get_security_level() << ", // security level" << std::endl;
  out << indent << Get_mul_level() << ", // mul level" << std::endl;
  out << indent << Get_first_prime_bit_num() << ", // first prime bit num"
      << std::endl;
  out << indent << Get_scaling_factor_bit_num() << ", // scaling factor bit num"
      << std::endl;
  out << indent << Get_q_part_num() << ", // q part num" << std::endl;

  out << indent << Get_rotate_index().size() << ", // rotate index num"
      << std::endl;
  out << indent << "{";
  for (int32_t idx : Get_rotate_index()) {
    out << idx << ", ";
  }
  out << "}, // rotate index" << std::endl;

  out << "}" << std::endl;
}

uint32_t CTX_PARAM::Mul_depth_of_bootstrap() {
  uint32_t hw = Get_hamming_weight();
  if (hw > 0 && hw <= HAMMING_WEIGHT_THRESHOLD) {
    return BOOTSTRAP_MUL_DEPTH_UNDER_THRESHOLD;
  } else {
    return BOOTSTRAP_MUL_DEPTH_ABOVE_THRESHOLD;
  }
}

uint32_t CTX_PARAM::Get_p_prime_num() const {
  uint32_t num_per_part = std::ceil(1. * Get_mul_level() / Get_q_part_num());
  uint32_t bit_num      = Get_first_prime_bit_num() +
                     (num_per_part - 1) * Get_scaling_factor_bit_num();
  uint32_t p_prime_num = std::ceil(1. * bit_num / BIT_NUM_OF_P_PRIME);
  return p_prime_num;
}

uint32_t CTX_PARAM::Get_modulus_bit_num() const {
  uint32_t mod_bit_num = Get_first_prime_bit_num();
  if (Get_mul_level() > 1) {
    mod_bit_num += (Get_mul_level() - 1) * Get_scaling_factor_bit_num();
  }
  mod_bit_num += Get_p_prime_num() * BIT_NUM_OF_P_PRIME;

  return mod_bit_num;
}

}  // namespace core
}  // namespace fhe