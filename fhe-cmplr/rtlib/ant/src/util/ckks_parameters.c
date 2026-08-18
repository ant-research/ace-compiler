//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/ckks_parameters.h"

#include "common/rt_config.h"
#include "common/trace.h"

void Init_ckks_parameters_with_multiply_depth(CKKS_PARAMETER* param,
                                              uint32_t        poly_degree,
                                              SECURITY_LEVEL  level,
                                              size_t          multiply_depth,
                                              size_t          hamming_weight) {
  param->_poly_degree    = poly_degree;
  param->_sec_level      = level ? level : HE_STD_128_CLASSIC;
  param->_hamming_weight = hamming_weight;

  param->_multiply_depth = multiply_depth;
  param->_num_primes     = multiply_depth + 1;
  FMT_ASSERT(param->_num_primes > 0,
             "length of q primes should be greater than 0");

  if (param->_num_q_parts == 0) {
    param->_num_q_parts = Get_default_num_q_parts(param->_multiply_depth);
  }

  if (level != HE_STD_NOT_SET) {
    // validate parameters
    size_t q_bound = Get_qbound(param->_sec_level, param->_poly_degree);
    // Estimate ciphertext modulus Q bound for HYBRID P*Q
    q_bound +=
        ceil(ceil((double)(q_bound) / param->_num_q_parts) / AUXBITS) * AUXBITS;
    // validate q bound & N
    FMT_ASSERT(
        q_bound <= Get_max_bit_count(param->_sec_level, param->_poly_degree),
        "invalid param set for security level");
  }

  // In most cases, the scale is chosen half as large as the second last prime
  // (or the last if there is only one). This avoids "scale out of bound" error
  // in ciphertext/plaintext multiplications. see safe_scale()
  param->_scaling_factor =
      1UL << Get_default_sf_bits(level, poly_degree, param->_num_primes);
  Init_crtcontext(param->_crt_context, param->_sec_level, poly_degree,
                  param->_multiply_depth, param->_num_q_parts);
  // The number of special modulus (usually L + 1)
  param->_num_p_primes = Get_primes_cnt(Get_p(param->_crt_context));
  param->_first_mod_size =
      (uint32_t)log2(Get_mod_val(Get_q_modulus_head(param->_crt_context)));

  Init_rtlib_config();
  IS_TRACE_CMD(Print_param(T_FILE, param));
}

void Init_ckks_parameters_with_prime_size(
    CKKS_PARAMETER* param, uint32_t poly_degree, SECURITY_LEVEL level,
    size_t prime_len, size_t first_mod_size, size_t scaling_mod_size,
    size_t hamming_weight) {
  param->_poly_degree    = poly_degree;
  param->_sec_level      = level;
  param->_hamming_weight = hamming_weight;
  param->_scaling_factor = 1UL << scaling_mod_size;
  param->_first_mod_size = first_mod_size;

  FMT_ASSERT(prime_len > 0, "length of q primes should be greater than 0");
  param->_num_primes     = prime_len;
  param->_multiply_depth = prime_len - 1;
  if (param->_num_q_parts == 0) {
    param->_num_q_parts = Get_default_num_q_parts(param->_multiply_depth);
  }

  Init_crtcontext_with_prime_size(
      param->_crt_context, param->_sec_level, poly_degree, param->_num_primes,
      first_mod_size, scaling_mod_size, param->_num_q_parts);
  // The number of special modulus (usually L + 1)
  param->_num_p_primes = Get_primes_cnt(Get_p(param->_crt_context));

  if (level != HE_STD_NOT_SET) {
    // validate parameters
    size_t q_bound = Get_coeff_bit_count(prime_len, param);
    // Estimate ciphertext modulus Q bound for HYBRID P*Q
    q_bound +=
        ceil(ceil((double)(q_bound) / param->_num_q_parts) / AUXBITS) * AUXBITS;
    // validate q bound & N
    FMT_ASSERT(
        q_bound <= Get_max_bit_count(param->_sec_level, param->_poly_degree),
        "invalid param set for security level, you could increase poly_degree "
        "or num_q_parts, or choose a smaller first_mod_size or "
        "sclaing_mod_size ");
  }

  Init_rtlib_config();
  IS_TRACE_CMD(Print_param(T_FILE, param));
}

CKKS_PARAMETER* Alloc_ckks_parameter() {
  CKKS_PARAMETER* param = (CKKS_PARAMETER*)malloc(sizeof(CKKS_PARAMETER));
  memset(param, 0, sizeof(CKKS_PARAMETER));
  param->_crt_context = Alloc_crtcontext();
  return param;
}

void Free_ckks_parameters(CKKS_PARAMETER* param) {
  if (param == NULL) return;
  if (param->_crt_context) {
    Free_crtcontext(param->_crt_context);
    param->_crt_context = NULL;
  }
  free(param);
}

void Print_param(FILE* fp, CKKS_PARAMETER* param) {
  fprintf(fp, "CKKS Parameters\n");
  fprintf(fp, "\tdegree: %d\n", param->_poly_degree);
  fprintf(fp, "\tscaling_factor = 1 << %lf\n", log2(param->_scaling_factor));
  fprintf(fp, "\tsec_level= %d\n", param->_sec_level);
  fprintf(fp, "\tmultiply_depth= %ld\n", param->_multiply_depth);
  fprintf(fp, "\tnum_primes= %ld\n", param->_num_primes);
  VALUE_LIST* q_prime = (Get_q(param->_crt_context))->_primes;
  FOR_ALL_ELEM(q_prime, idx) {
    fprintf(
        fp, "\tq%ld = %ld\n", idx,
        Get_mod_val(Get_modulus((CRT_PRIME*)Get_ptr_value_at(q_prime, idx))));
  }
  fprintf(fp, "\tnum_p_primes= %ld\n", param->_num_p_primes);
  VALUE_LIST* p_prime = (Get_p(param->_crt_context))->_primes;
  FOR_ALL_ELEM(p_prime, idx) {
    fprintf(
        fp, "\tp%ld = %ld\n", idx,
        Get_mod_val(Get_modulus((CRT_PRIME*)Get_ptr_value_at(p_prime, idx))));
  }
  fprintf(fp, "\tnum_q_parts= %ld\n", param->_num_q_parts);
  char* rns;
  if (param->_crt_context) {
    rns = "Yes";
  } else {
    rns = "No";
  }
  fprintf(fp, "\tRNS: %s\n", rns);
}
