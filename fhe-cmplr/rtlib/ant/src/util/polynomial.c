//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/polynomial.h"

#include "common/rt_config.h"
#include "util/fhe_bignumber.h"
#include "util/random_sample.h"
#include "util/secret_key.h"

POLYNOMIAL* Add_poly(POLYNOMIAL* sum, POLYNOMIAL* poly1, POLYNOMIAL* poly2,
                     CRT_CONTEXT* crt, VL_CRTPRIME* p_modulus) {
  if (!Is_ntt_match(poly1, poly2)) {
    if (!Is_ntt(poly1)) {
      Conv_poly2ntt_inplace(poly1, crt);
    } else if (!Is_ntt(poly2)) {
      Conv_poly2ntt_inplace(poly2, crt);
    }
  }
  IS_TRUE(Is_size_match(poly1, poly2) && Is_size_match(sum, poly1),
          "size not match");
  VL_CRTPRIME* q_modulus = Get_q_primes(crt);
  IS_TRUE(Get_poly_level(sum) <= LIST_LEN(q_modulus), "primes not match");
  int64_t* sum_coeffs   = Get_poly_coeffs(sum);
  int64_t* poly1_coeffs = Get_poly_coeffs(poly1);
  int64_t* poly2_coeffs = Get_poly_coeffs(poly2);
  MODULUS* modulus      = Get_modulus_head(q_modulus);
  for (size_t module_idx = 0; module_idx < Get_poly_level(sum); module_idx++) {
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(sum); poly_idx++) {
      *sum_coeffs = Add_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                       Get_mod_val(modulus));
      sum_coeffs++;
      poly1_coeffs++;
      poly2_coeffs++;
    }
    modulus = Get_next_modulus(modulus);
  }
  if (p_modulus) {
    sum_coeffs   = Get_p_coeffs(sum);
    poly1_coeffs = Get_p_coeffs(poly1);
    poly2_coeffs = Get_p_coeffs(poly2);
    modulus      = Get_modulus_head(p_modulus);
    for (size_t module_idx = 0; module_idx < Get_num_p(sum); module_idx++) {
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(sum); poly_idx++) {
        *sum_coeffs = Add_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                         Get_mod_val(modulus));
        sum_coeffs++;
        poly1_coeffs++;
        poly2_coeffs++;
      }
      modulus = Get_next_modulus(modulus);
    }
  }
  Set_is_ntt(sum, Is_ntt(poly1));
  return sum;
}

POLYNOMIAL* Sub_poly(POLYNOMIAL* poly_diff, POLYNOMIAL* poly1,
                     POLYNOMIAL* poly2, CRT_CONTEXT* crt,
                     VALUE_LIST* p_modulus) {
  if (!Is_ntt_match(poly1, poly2)) {
    if (!Is_ntt(poly1)) {
      Conv_poly2ntt_inplace(poly1, crt);
    } else if (!Is_ntt(poly2)) {
      Conv_poly2ntt_inplace(poly2, crt);
    }
  }
  IS_TRUE(Is_size_match(poly1, poly2) && Is_size_match(poly_diff, poly1),
          "size not match");
  VALUE_LIST* q_modulus = Get_q_primes(crt);
  IS_TRUE(Get_poly_level(poly_diff) <= LIST_LEN(q_modulus), "primes not match");
  int64_t* poly_diff_coeffs = Get_poly_coeffs(poly_diff);
  int64_t* poly1_coeffs     = Get_poly_coeffs(poly1);
  int64_t* poly2_coeffs     = Get_poly_coeffs(poly2);
  MODULUS* modulus          = Get_modulus_head(q_modulus);
  for (size_t mod_idx = 0; mod_idx < Get_poly_level(poly_diff); mod_idx++) {
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(poly_diff); poly_idx++) {
      *poly_diff_coeffs = Sub_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                             Get_mod_val(modulus));
      poly_diff_coeffs++;
      poly1_coeffs++;
      poly2_coeffs++;
    }
    modulus = Get_next_modulus(modulus);
  }
  if (p_modulus) {
    poly_diff_coeffs = Get_p_coeffs(poly_diff);
    poly1_coeffs     = Get_p_coeffs(poly1);
    poly2_coeffs     = Get_p_coeffs(poly2);
    modulus          = Get_modulus_head(p_modulus);
    for (size_t mod_idx = 0; mod_idx < Get_num_p(poly_diff); mod_idx++) {
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(poly_diff);
           poly_idx++) {
        *poly_diff_coeffs = Sub_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                               Get_mod_val(modulus));
        poly_diff_coeffs++;
        poly1_coeffs++;
        poly2_coeffs++;
      }
      modulus = Get_next_modulus(modulus);
    }
  }
  Set_is_ntt(poly_diff, Is_ntt(poly1));
  return poly_diff;
}

void Multiply_ntt(POLYNOMIAL* res, POLYNOMIAL* poly1, POLYNOMIAL* poly2,
                  VALUE_LIST* q_primes, VALUE_LIST* p_primes) {
  FMT_ASSERT(Is_ntt(poly1) && Is_ntt(poly2), "operand is not ntt form");
  FMT_ASSERT(Is_size_match(res, poly1) && Is_size_match(res, poly2),
             "size not match");
  int64_t* res_data   = Get_poly_coeffs(res);
  int64_t* poly1_data = Get_poly_coeffs(poly1);
  int64_t* poly2_data = Get_poly_coeffs(poly2);
  MODULUS* q_modulus  = Get_modulus_head(q_primes);
  for (size_t module_idx = 0; module_idx < Get_poly_level(res); module_idx++) {
    for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
      *res_data = Mul_int64_mod_barret(*poly1_data, *poly2_data, q_modulus);
      poly1_data++;
      poly2_data++;
      res_data++;
    }
    q_modulus = Get_next_modulus(q_modulus);
  }
  if (p_primes) {
    MODULUS* p_modulus = Get_modulus_head(p_primes);
    poly1_data         = Get_p_coeffs(poly1);
    poly2_data         = Get_p_coeffs(poly2);
    res_data           = Get_p_coeffs(res);
    for (size_t module_idx = 0; module_idx < Get_num_p(res); module_idx++) {
      for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
        *res_data = Mul_int64_mod_barret(*poly1_data, *poly2_data, p_modulus);
        poly1_data++;
        poly2_data++;
        res_data++;
      }
      p_modulus = Get_next_modulus(p_modulus);
    }
  }
  Set_is_ntt(res, TRUE);
}

void Multiply_add(POLYNOMIAL* res, POLYNOMIAL* poly1, POLYNOMIAL* poly2,
                  VALUE_LIST* q_primes, VALUE_LIST* p_primes) {
  IS_TRUE(Is_ntt(res) && Is_ntt(poly1) && Is_ntt(poly2), "opnd/res is not ntt");
  IS_TRUE(Is_size_match(res, poly1) && Is_size_match(res, poly2),
          "level not matched");
  int64_t* res_data   = Get_poly_coeffs(res);
  int64_t* poly1_data = Get_poly_coeffs(poly1);
  int64_t* poly2_data = Get_poly_coeffs(poly2);
  MODULUS* q_modulus  = Get_modulus_head(q_primes);
  for (size_t module_idx = 0; module_idx < Get_poly_level(res); module_idx++) {
    for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
      int64_t tmp = Mul_int64_mod_barret(*poly1_data, *poly2_data, q_modulus);
      *res_data   = Add_int64_with_mod(*res_data, tmp, Get_mod_val(q_modulus));
      poly1_data++;
      poly2_data++;
      res_data++;
    }
    q_modulus = Get_next_modulus(q_modulus);
  }
  if (p_primes) {
    MODULUS* p_modulus = Get_modulus_head(p_primes);
    res_data           = Get_p_coeffs(res);
    poly1_data         = Get_p_coeffs(poly1);
    poly2_data         = Get_p_coeffs(poly2);
    for (size_t module_idx = 0; module_idx < Get_num_p(res); module_idx++) {
      for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
        int64_t tmp = Mul_int64_mod_barret(*poly1_data, *poly2_data, p_modulus);
        *res_data = Add_int64_with_mod(*res_data, tmp, Get_mod_val(p_modulus));
        poly1_data++;
        poly2_data++;
        res_data++;
      }
      p_modulus = Get_next_modulus(p_modulus);
    }
  }
}

POLYNOMIAL* Multiply_poly_fast(POLYNOMIAL* res, POLYNOMIAL* poly1,
                               POLYNOMIAL* poly2, CRT_CONTEXT* crt,
                               VALUE_LIST* p_primes) {
  if (!Is_ntt(poly1)) {
    Conv_poly2ntt_inplace(poly1, crt);
  }
  if (!Is_ntt(poly2)) {
    Conv_poly2ntt_inplace(poly2, crt);
  }
  Multiply_ntt(res, poly1, poly2, Get_q_primes(crt), p_primes);
  return res;
}

POLYNOMIAL* Scalar_integer_multiply_poly(POLYNOMIAL* res, POLYNOMIAL* poly,
                                         int64_t scalar, VALUE_LIST* q_modulus,
                                         VALUE_LIST* p_modulus) {
  IS_TRUE(Is_size_match(res, poly), "size not match");
  IS_TRUE(Get_poly_level(res) <= LIST_LEN(q_modulus), "primes not match");

  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Get_modulus_head(q_modulus);
  for (size_t module_idx = 0; module_idx < Get_poly_level(res); module_idx++) {
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
      *res_coeffs =
          Mul_int64_with_mod(*poly_coeffs, scalar, Get_mod_val(modulus));
      res_coeffs++;
      poly_coeffs++;
    }
    modulus = Get_next_modulus(modulus);
  }
  if (p_modulus) {
    modulus     = Get_modulus_head(p_modulus);
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    for (size_t module_idx = 0; module_idx < Get_num_p(res); module_idx++) {
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *res_coeffs =
            Mul_int64_with_mod(*poly_coeffs, scalar, Get_mod_val(modulus));
        res_coeffs++;
        poly_coeffs++;
      }
      modulus = Get_next_modulus(modulus);
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLYNOMIAL* Scalars_integer_multiply_poly(POLYNOMIAL* res, POLYNOMIAL* poly,
                                          VALUE_LIST* scalars, CRT_PRIMES* q,
                                          CRT_PRIMES* p) {
  IS_TRUE(Is_size_match(res, poly), "size not match");
  IS_TRUE(Get_poly_level(res) <= Get_primes_cnt(q), "primes not match");

  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Get_modulus_head(Get_primes(q));
  for (size_t module_idx = 0; module_idx < Get_poly_level(res); module_idx++) {
    int64_t scalar = Get_i64_value_at(scalars, module_idx);
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
      *res_coeffs = Mul_int64_mod_barret(*poly_coeffs, scalar, modulus);
      res_coeffs++;
      poly_coeffs++;
    }
    modulus = Get_next_modulus(modulus);
  }
  if (p) {
    modulus     = Get_modulus_head(Get_primes(p));
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    for (size_t module_idx = 0; module_idx < Get_num_p(res); module_idx++) {
      int64_t scalar = Get_i64_value_at(scalars, module_idx);
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *res_coeffs = Mul_int64_mod_barret(*poly_coeffs, scalar, modulus);
        res_coeffs++;
        poly_coeffs++;
      }
      modulus = Get_next_modulus(modulus);
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLYNOMIAL* Scalars_integer_multiply_poly_qpart(
    POLYNOMIAL* res, POLYNOMIAL* poly, VALUE_LIST* scalars, CRT_PRIMES* q,
    CRT_PRIMES* qpart, size_t part_idx) {
  IS_TRUE(Is_size_match(res, poly), "level not match");
  IS_TRUE(Get_poly_level(res) == Get_primes_cnt(q), "primes not match");
  size_t num_per_part = Get_per_part_size(qpart);

  size_t start_part_idx = num_per_part * part_idx;
  size_t end_part_idx  = (Get_poly_level(res) > (start_part_idx + num_per_part))
                             ? (start_part_idx + num_per_part)
                             : Get_poly_level(res);
  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Get_modulus_head(Get_primes(q));
  for (size_t i = 0; i < Get_poly_level(res); i++) {
    if (i >= start_part_idx && i < end_part_idx) {
      int64_t scalar = Get_i64_value_at(scalars, i);
      for (uint32_t j = 0; j < Get_rdgree(res); j++) {
        size_t idx = i * Get_rdgree(res) + j;
        res_coeffs[idx] =
            Mul_int64_mod_barret(poly_coeffs[idx], scalar, modulus);
      }
    }
    modulus = Get_next_modulus(modulus);
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLYNOMIAL* Automorphism_transform(POLYNOMIAL* res, POLYNOMIAL* poly,
                                   VALUE_LIST* precomp, CRT_CONTEXT* crt) {
  uint32_t degree = Get_rdgree(poly);
  IS_TRUE(Is_size_match(res, poly), "op not matched");
  IS_TRUE(LIST_LEN(precomp) == degree, "invalid index table length");

  int64_t* poly_coeffs  = Get_poly_coeffs(poly);
  int64_t* res_coeffs   = Get_poly_coeffs(res);
  int64_t* precomp_data = Get_i64_values(precomp);
  MODULUS* modulus      = Get_modulus_head(Get_q_primes(crt));
  for (size_t module_idx = 0; module_idx < Get_poly_level(res); module_idx++) {
    int64_t qi = Get_mod_val(modulus);
    for (uint32_t poly_idx = 0; poly_idx < degree; poly_idx++) {
      int64_t map_idx = precomp_data[poly_idx];
      if (map_idx >= 0) {
        *res_coeffs = poly_coeffs[module_idx * degree + map_idx];
      } else {
        *res_coeffs = qi - poly_coeffs[module_idx * degree - map_idx];
      }
      res_coeffs++;
    }
    modulus = Get_next_modulus(modulus);
  }
  modulus     = Get_modulus_head(Get_p_primes(crt));
  res_coeffs  = Get_p_coeffs(res);
  poly_coeffs = Get_p_coeffs(poly);
  for (size_t module_idx = 0; module_idx < Get_num_p(poly); module_idx++) {
    int64_t pi = Get_mod_val(modulus);
    for (uint32_t poly_idx = 0; poly_idx < degree; poly_idx++) {
      int64_t map_idx = precomp_data[poly_idx];
      if (map_idx >= 0) {
        *res_coeffs = poly_coeffs[module_idx * degree + map_idx];
      } else {
        *res_coeffs = pi - poly_coeffs[module_idx * degree - map_idx];
      }
      res_coeffs++;
    }
    modulus = Get_next_modulus(modulus);
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLYNOMIAL* Rotate_poly(POLYNOMIAL* res, POLYNOMIAL* poly, uint32_t k,
                        VALUE_LIST* precomp, CRT_CONTEXT* crt) {
  IS_TRUE(Is_size_match(res, poly), "size not matched");
  IS_TRUE(k % 2 != 0, "idx should be odd");
  Automorphism_transform(res, poly, precomp, crt);
  return res;
}

void Rotate_poly_with_rotation_idx(POLYNOMIAL* res, POLYNOMIAL* poly,
                                   int32_t rotation, CRT_CONTEXT* crt) {
  size_t  degree = Get_rdgree(poly);
  MODULUS two_n_modulus;
  Init_modulus(&two_n_modulus, 2 * degree);
  uint32_t    k       = Find_automorphism_index(rotation, &two_n_modulus);
  VALUE_LIST* precomp = Alloc_value_list(I64_TYPE, degree);
  Precompute_automorphism_order(precomp, k, degree, Is_ntt(poly));
  Rotate_poly(res, poly, k, precomp, crt);
  Free_value_list(precomp);
}

void Transform_values_to_rns(int64_t* res, size_t res_len,
                             CRT_PRIMES* crt_primes, VALUE_LIST* vals,
                             bool without_mod) {
  CRT_PRIME* prime    = Get_prime_head(crt_primes);
  size_t     len      = Get_primes_cnt(crt_primes);
  size_t     vals_len = LIST_LEN(vals);
  IS_TRUE(res_len >= len * vals_len, "transform_to_crt: length not match");
  if (vals->_type == I64_TYPE) {
    int64_t big_bound = Max_64bit_value();
    int64_t big_half  = big_bound >> 1;
    for (size_t i = 0; i < len; i++) {
      int64_t  mod_val = Get_modulus_val(prime);
      int64_t  diff    = big_bound - mod_val;
      int64_t* val     = Get_i64_values(vals);
      for (size_t val_idx = 0; val_idx < vals_len; val_idx++) {
        if (without_mod) {
          // value range of *val is within mod_val of primes,
          // so it's no need to use the modulus(%) operator.
          *res = *val < 0 ? (*val + mod_val) : *val;
        } else {
          if (*val > big_half) {
            *res = Mod_int64(*val - diff, mod_val);
          } else {
            *res = Mod_int64(*val, mod_val);
          }
        }
        res++;
        val++;
      }
      prime = Get_next_prime(prime);
    }
  } else if (vals->_type == I128_TYPE) {
    INT128_T big_bound = Max_128bit_value();
    INT128_T big_half  = ((UINT128_T)big_bound) >> 1;
    for (size_t i = 0; i < len; i++) {
      int64_t   mod_val = Get_modulus_val(prime);
      INT128_T  diff    = big_bound - mod_val;
      INT128_T* val     = Get_i128_values(vals);
      for (size_t val_idx = 0; val_idx < vals_len; val_idx++) {
        if (*val > big_half) {
          *res = Mod_int128(*val - diff, mod_val);
        } else {
          *res = Mod_int128(*val, mod_val);
        }
        res++;
        val++;
      }
      prime = Get_next_prime(prime);
    }
  } else if (vals->_type == BIGINT_TYPE) {
    for (size_t i = 0; i < len; i++) {
      for (size_t val_idx = 0; val_idx < vals_len; val_idx++) {
        int64_t mod_val = Mod_bigint(*(Get_bint_value_at(vals, val_idx)),
                                     Get_modulus_val(Get_nth_prime(prime, i)));
        res[i * vals_len + val_idx] = mod_val;
      }
    }
  } else {
    IS_TRUE(FALSE, "invalid type");
  }
}

void Transform_values_to_qbase(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                               VALUE_LIST* value, bool without_mod) {
  CRT_PRIMES* q_primes    = Get_q(crt);
  size_t      q_prime_cnt = Get_primes_cnt(q_primes);
  Transform_values_to_rns(Get_poly_coeffs(poly), Get_rdgree(poly) * q_prime_cnt,
                          q_primes, value, without_mod);
}

void Transform_values_at_level(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                               VALUE_LIST* value, bool without_mod,
                               size_t q_level, size_t p_cnt) {
  CRT_PRIMES* q_primes = Get_q(crt);

  VL_CRTPRIME* primes = Alloc_value_list(PTR_TYPE, q_level);
  Copy_vl_crtprime(primes, Get_primes(q_primes), q_level);
  CRT_PRIMES primes_at_level = {._type = Q_TYPE, ._primes = primes};

  Transform_values_to_rns(Get_poly_coeffs(poly), Get_rdgree(poly) * q_level,
                          &primes_at_level, value, without_mod);
  if (p_cnt) {
    Transform_values_to_rns(Get_poly_coeffs(poly) + Get_rdgree(poly) * q_level,
                            Get_rdgree(poly) * p_cnt, Get_p(crt), value,
                            without_mod);
  }

  Free_value_list(primes);
}

void Transform_values_to_qpbase(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                                VALUE_LIST* value, bool without_mod) {
  CRT_PRIMES* q_primes  = Get_q(crt);
  CRT_PRIMES* p_primes  = Get_p(crt);
  size_t      part1_len = Get_primes_cnt(q_primes) * Get_rdgree(poly);
  size_t      part2_len = Get_primes_cnt(p_primes) * Get_rdgree(poly);
  Transform_values_to_rns(Get_poly_coeffs(poly), part1_len, q_primes, value,
                          without_mod);
  Transform_values_to_rns(Get_poly_coeffs(poly) + part1_len, part2_len,
                          p_primes, value, without_mod);
}

// Reconstructs original polynomial from vals from the CRT representation to the
// regular representation. Returns:
//     A value list whose values are reconstructed.
void Reconstruct_rns_poly_to_values(VALUE_LIST* res, POLYNOMIAL* rns_poly,
                                    CRT_PRIMES* crt_primes) {
  IS_TRUE(Get_poly_level(rns_poly) == Get_primes_cnt(crt_primes) ||
              Get_num_pq(rns_poly) == Get_primes_cnt(crt_primes),
          "invalid length");
  IS_TRUE(!Is_ntt(rns_poly), "rns_poly is ntt-form");
  BIG_INT intermed_val, tmp_val, half_bigm;
  BI_INITS(intermed_val, half_bigm);
  BI_INIT_ASSIGN_SI(tmp_val, 0);
  BI_DIV_UI(half_bigm, GET_BIG_M(crt_primes), 2);

  VL_BIGINT* hat_inv = Get_hat_inv(crt_primes);
  VL_BIGINT* hat     = Get_hat(crt_primes);
  for (uint32_t i = 0; i < Get_rdgree(rns_poly); i++) {
    BI_ASSIGN_SI(tmp_val, 0);
    for (size_t prime_idx = 0; prime_idx < Get_primes_cnt(crt_primes);
         prime_idx++) {
      // TODO: The hat * hat_inv can be precomputed out of the inner loop
      BI_MUL(intermed_val, BIGINT_VALUE_AT(hat_inv, prime_idx),
             BIGINT_VALUE_AT(hat, prime_idx));
      BI_MUL_UI(intermed_val, intermed_val,
                Get_coeff_at(rns_poly, prime_idx * Get_rdgree(rns_poly) + i));
      BI_ADD(tmp_val, tmp_val, intermed_val);
    }
    BI_MOD(tmp_val, tmp_val, GET_BIG_M(crt_primes));
    if (BI_CMP(tmp_val, half_bigm) > 0) {
      BI_SUB(tmp_val, tmp_val, GET_BIG_M(crt_primes));
    }

    Set_bint_value(res, i, tmp_val);
  }
  BI_FREES(intermed_val, tmp_val, half_bigm);
}

void Reconstruct_qpbase_to_values(VALUE_LIST* res, POLYNOMIAL* poly,
                                  CRT_CONTEXT* crt) {
  CRT_PRIMES combined_primes;
  memset(&combined_primes, 0, sizeof(combined_primes));
  Set_prime_type(&combined_primes, Q_TYPE);
  CRT_PRIMES* q           = Get_q(crt);
  CRT_PRIMES* p           = Get_p(crt);
  size_t      q_len       = Get_primes_cnt(q);
  size_t      p_len       = Get_primes_cnt(p);
  VALUE_LIST* comb_primes = Alloc_value_list(PTR_TYPE, q_len + p_len);
  CRT_PRIME*  primes      = Alloc_crt_primes(q_len + p_len);
  memcpy(primes, Get_prime_head(q), sizeof(CRT_PRIME) * q_len);
  memcpy(Get_nth_prime(primes, q_len), Get_prime_head(p),
         sizeof(CRT_PRIME) * p_len);
  for (size_t idx = 0; idx < q_len + p_len; idx++) {
    PTR_VALUE_AT(comb_primes, idx) = (PTR)Get_nth_prime(primes, idx);
  }
  combined_primes._primes = comb_primes;
  Precompute_primes(&combined_primes, true);

  Reconstruct_rns_poly_to_values(res, poly, &combined_primes);
  free(primes);
  Free_value_list(comb_primes);
  Free_precompute(Get_precomp(&combined_primes));
}

void Reconstruct_qbase_to_values(VALUE_LIST* res, POLYNOMIAL* poly,
                                 CRT_CONTEXT* crt) {
  Reconstruct_rns_poly_to_values(res, poly, Get_q(crt));
}

void Conv_poly2ntt_inplace(POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  CRT_PRIME* q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME* p_prime     = Get_prime_head(Get_p(crt));
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_fwd(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    poly_data += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }
  if (Get_num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    for (size_t pi = 0; pi < Get_num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_fwd(&tmp_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      poly_data += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(poly, TRUE);
}

void Conv_poly2ntt_inplace_with_primes(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                                       VL_CRTPRIME* primes) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  IS_TRUE(LIST_LEN(primes) == Get_poly_level(poly), "primes size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);

  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_fwd(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
  }
  Set_is_ntt(poly, TRUE);
}

void Conv_poly2ntt_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               CRT_CONTEXT* crt, VL_CRTPRIME* primes) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  IS_TRUE(LIST_LEN(primes) == Get_poly_level(poly), "primes size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  int64_t*   res_data    = Get_poly_coeffs(res);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt, res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);

  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
  }
  Set_is_ntt(res, TRUE);
}

void Conv_poly2ntt(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  FMT_ASSERT(Is_size_match(res, poly), "size not match");
  CRT_PRIME* q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME* p_prime     = Get_prime_head(Get_p(crt));
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  int64_t*   res_data    = Get_poly_coeffs(res);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  VALUE_LIST res_ntt;
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);
  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }

  if (Get_num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    res_ntt._vals._i = Get_p_coeffs(res);
    for (size_t pi = 0; pi < Get_num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      res_ntt._vals._i += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(res, TRUE);
}

void Conv_ntt2poly(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  FMT_ASSERT(Is_size_match(res, poly), "size not match");

  CRT_PRIME* q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME* p_prime     = Get_prime_head(Get_p(crt));
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  int64_t*   res_data    = Get_poly_coeffs(res);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt, res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);
  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_inv(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }
  if (Get_num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    res_ntt._vals._i = Get_p_coeffs(res);
    for (size_t pi = 0; pi < Get_num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_inv(&res_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      res_ntt._vals._i += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(res, FALSE);
}

void Conv_ntt2poly_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               CRT_CONTEXT* crt, VL_CRTPRIME* primes) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  FMT_ASSERT(Is_size_match(res, poly), "size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  int64_t*   res_data    = Get_poly_coeffs(res);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt, res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);

  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_inv(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
  }
  Set_is_ntt(res, FALSE);
}

void Conv_ntt2poly_inplace_with_primes(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                                       VL_CRTPRIME* primes) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  FMT_ASSERT(Get_poly_level(poly) == LIST_LEN(primes), "primes size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_inv(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    poly_data += ring_degree;
  }
  Set_is_ntt(poly, FALSE);
}

void Conv_ntt2poly_inplace(POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  CRT_PRIME* q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME* p_prime     = Get_prime_head(Get_p(crt));
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  for (size_t qi = 0; qi < Get_poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_inv(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    poly_data += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }
  if (Get_num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    for (size_t pi = 0; pi < Get_num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_inv(&tmp_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      poly_data += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(poly, FALSE);
}

void Extract_poly(POLYNOMIAL* res, POLYNOMIAL* poly, size_t start_prime_ofst,
                  size_t prime_cnt) {
  FMT_ASSERT(poly && start_prime_ofst <= Get_num_alloc_primes(poly) &&
                 start_prime_ofst + prime_cnt <= Get_num_alloc_primes(poly),
             "invalid range");
  int64_t* start_data =
      Get_poly_coeffs(poly) + start_prime_ofst * Get_rdgree(poly);
  res->_num_alloc_primes = prime_cnt;
  Init_poly_data(res, Get_rdgree(poly), prime_cnt, 0, start_data);
  Set_is_ntt(res, Is_ntt(poly));
}

void Derive_poly(POLYNOMIAL* res, POLYNOMIAL* poly, size_t q_cnt,
                 size_t p_cnt) {
  FMT_ASSERT(poly && res && q_cnt <= Get_poly_level(poly) && p_cnt <= Get_num_p(poly),
             "invalid range");
  res->_num_alloc_primes = Get_num_alloc_primes(poly);
  Init_poly_data(res, Get_rdgree(poly), q_cnt, p_cnt, Get_poly_coeffs(poly));
  Set_is_ntt(res, Is_ntt(poly));
}

// fast convert polynomial RNS bases from old_primes to new_primes
void Fast_base_conv(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                    CRT_PRIMES* new_primes, CRT_PRIMES* old_primes) {
  uint32_t ring_degree = Get_rdgree(old_poly);
  // NOTE: after rescale num_primes of poly may not equal to num_primes of crt
  IS_TRUE(Get_poly_level(new_poly) <= Get_primes_cnt(new_primes) &&
              Get_poly_level(old_poly) <= Get_primes_cnt(old_primes),
          "unmatched size");
  IS_TRUE(Get_rdgree(new_poly) == Get_rdgree(old_poly),
          "unmatched ring_degree");

  size_t      old_level    = Get_poly_level(old_poly);
  size_t      new_level    = Get_poly_level(new_poly);
  CRT_PRIME*  old_prime    = Get_prime_head(old_primes);
  CRT_PRIME*  new_prime    = Get_prime_head(new_primes);
  VALUE_LIST* inv_mod_self = Is_q(old_primes)
                                 ? Get_qhatinvmodq_at(old_primes, old_level - 1)
                                 : Get_phatinvmodp(old_primes);
  VALUE_LIST* inv_mod_self_prec =
      Is_q(old_primes) ? Get_qhatinvmodq_prec_at(old_primes, old_level - 1)
                       : Get_phatinvmodp_prec(old_primes);

  // element-wise old_poly * inv_mod_self
  int64_t* arr_mul_inv_modself =
      malloc(sizeof(int64_t) * ring_degree * old_level);
  int64_t* p_mul_inv_modself = arr_mul_inv_modself;
  int64_t* old_coeffs        = Get_poly_coeffs(old_poly);
  for (size_t o_idx = 0; o_idx < old_level; o_idx++) {
    int64_t mod              = Get_modulus_val(Get_nth_prime(old_prime, o_idx));
    int64_t inv_mod_self_val = Get_i64_value_at(inv_mod_self, o_idx);
    int64_t inv_mod_self_val_prec = Get_i64_value_at(inv_mod_self_prec, o_idx);
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      *p_mul_inv_modself = Fast_mul_const_with_mod(
          *old_coeffs, inv_mod_self_val, inv_mod_self_val_prec, mod);
      p_mul_inv_modself++;
      old_coeffs++;
    }
  }
  for (size_t n_idx = 0; n_idx < new_level; n_idx++) {
    MODULUS*    new_modulus = Get_modulus(Get_nth_prime(new_prime, n_idx));
    VALUE_LIST* mod_nb      = Get_phatmodq_at(old_primes, n_idx);
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      INT128_T sum = 0;
      for (size_t o_idx = 0; o_idx < old_level; o_idx++) {
        sum += (INT128_T)(arr_mul_inv_modself[o_idx * ring_degree + d_idx]) *
               Get_i64_value_at(mod_nb, o_idx);
      }
      int64_t new_value = Mod_barrett_128(sum, new_modulus);
      Set_coeff_at(new_poly, new_value, n_idx * ring_degree + d_idx);
    }
  }
  Set_is_ntt(new_poly, Is_ntt(old_poly));
  free(arr_mul_inv_modself);
}

void Fast_base_conv_with_parts(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                               CRT_PRIMES* qpart, VL_CRTPRIME* new_primes,
                               size_t part_idx, size_t level) {
  uint32_t     ring_degree = Get_rdgree(old_poly);
  VL_CRTPRIME* qpart_prime = Get_qpart_prime_at_l1(qpart, part_idx);
  size_t       partq_level = Get_poly_level(old_poly) - 1;
  IS_TRUE(Get_rdgree(new_poly) == Get_rdgree(old_poly),
          "unmatched ring_degree");
  VALUE_LIST* ql_hatinvmodq =
      VL_L2_VALUE_AT(Get_qlhatinvmodq(qpart), part_idx, partq_level);
  VL_VL_I64* ql_hatmodp = VL_L2_VALUE_AT(Get_qlhatmodp(qpart), level, part_idx);
  size_t     size_q     = Get_poly_level(old_poly) > LIST_LEN(qpart_prime)
                              ? LIST_LEN(qpart_prime)
                              : Get_poly_level(old_poly);
  size_t     size_p     = Get_poly_level(new_poly);

  INT128_T sum[size_p];
  memset(sum, 0, size_p * sizeof(INT128_T));

  for (size_t n = 0; n < ring_degree; n++) {
    for (size_t i = 0; i < size_q; i++) {
      int64_t  val = Get_coeff_at(old_poly, i * ring_degree + n);
      MODULUS* qi  = (MODULUS*)Get_modulus(Get_vlprime_at(qpart_prime, i));
      int64_t  mul_inv_modq =
          Mul_int64_mod_barret(val, I64_VALUE_AT(ql_hatinvmodq, i), qi);
      VALUE_LIST* qhat_modp = VL_VALUE_AT(ql_hatmodp, i);
      for (size_t j = 0; j < size_p; j++) {
        sum[j] += (INT128_T)mul_inv_modq * Get_i64_value_at(qhat_modp, j);
      }
    }
    for (size_t j = 0; j < size_p; j++) {
      int64_t new_value =
          Mod_barrett_128(sum[j], Get_modulus(Get_vlprime_at(new_primes, j)));
      Set_coeff_at(new_poly, new_value, j * ring_degree + n);
      sum[j] = 0;
    }
  }
}

void Decompose_poly(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt,
                    size_t num_qpartl, uint32_t q_part_idx) {
  size_t       ql              = Get_num_q(poly);
  uint32_t     degree          = Get_rdgree(poly);
  CRT_PRIMES*  q_parts         = Get_qpart(crt);
  size_t       part_size       = Get_per_part_size(q_parts);
  VL_CRTPRIME* q_parts_primes  = VL_VALUE_AT(Get_primes(q_parts), q_part_idx);
  size_t       decomp_poly_len = LIST_LEN(q_parts_primes);
  if (q_part_idx == num_qpartl - 1) {
    decomp_poly_len = ql - part_size * q_part_idx;
  }
  if (res->_num_alloc_primes < decomp_poly_len) {
    Free_poly_data(res);
    Alloc_poly_data(res, degree, decomp_poly_len, 0);
  } else {
    res->_num_primes   = decomp_poly_len;
    res->_num_primes_p = 0;
  }
  size_t   start     = part_size * q_part_idx;
  int64_t* coeffs    = Get_poly_coeffs(res);
  int64_t* c1_coeffs = Get_poly_coeffs(poly);
  for (size_t i = 0, idx = start; i < decomp_poly_len; i++, idx++) {
    for (uint32_t d = 0; d < degree; d++) {
      coeffs[i * degree + d] = c1_coeffs[idx * degree + d];
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
}

void Raise_rns_base_with_parts(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                               CRT_CONTEXT* crt, size_t q_cnt,
                               size_t q_part_idx) {
  size_t       level = q_cnt - 1;
  VL_CRTPRIME* new_primes =
      Get_qpart_compl_at(Get_qpart_compl(crt), level, q_part_idx);
  VL_CRTPRIME* old_primes = VL_VALUE_AT(Get_primes(Get_qpart(crt)), q_part_idx);
  CRT_PRIMES*  q_parts    = Get_qpart(crt);
  uint32_t     ring_degree = Get_rdgree(new_poly);
  size_t       ext_len     = LIST_LEN(new_primes);
  size_t       old_q_cnt   = Get_poly_level(old_poly);
  size_t       old_p_cnt   = Get_num_p(old_poly);
  IS_TRUE(new_poly && ring_degree == Get_rdgree(old_poly) &&
              Get_num_p(new_poly) == Get_primes_cnt(Get_p(crt)),
          "raise_rns_base: result size not match");

  POLYNOMIAL qparts_ext2p;
  if (Is_ntt(old_poly)) {
    POLYNOMIAL cloned_old_poly;
    Alloc_poly_data(&cloned_old_poly, ring_degree, old_q_cnt, old_p_cnt);
    Conv_ntt2poly_with_primes(&cloned_old_poly, old_poly, crt, old_primes);
    Alloc_poly_data(&qparts_ext2p, ring_degree, ext_len, 0);

    Fast_base_conv_with_parts(&qparts_ext2p, &cloned_old_poly, q_parts,
                              new_primes, q_part_idx, level);
    Conv_poly2ntt_inplace_with_primes(&qparts_ext2p, crt, new_primes);
    Free_poly_data(&cloned_old_poly);
  } else {
    Alloc_poly_data(&qparts_ext2p, ring_degree, ext_len, 0);
    Fast_base_conv_with_parts(&qparts_ext2p, old_poly, q_parts, new_primes,
                              q_part_idx, level);
  }
  int64_t* new_coeffs = Get_poly_coeffs(new_poly);
  int64_t* ext_coeffs = Get_poly_coeffs(&qparts_ext2p);
  int64_t* old_coeffs = Get_poly_coeffs(old_poly);
  size_t   start_idx  = Get_per_part_size(q_parts) * q_part_idx;
  size_t   end_idx    = start_idx + old_q_cnt;
  size_t   copy_size  = sizeof(int64_t) * ring_degree;

  memcpy(new_coeffs, ext_coeffs, copy_size * start_idx);
  memcpy(new_coeffs + start_idx * ring_degree, old_coeffs,
         copy_size * (end_idx - start_idx));
  memcpy(new_coeffs + end_idx * ring_degree,
         ext_coeffs + (end_idx - old_q_cnt) * ring_degree,
         (Get_num_pq(new_poly) - end_idx) * copy_size);
  Free_poly_data(&qparts_ext2p);

  Set_is_ntt(new_poly, Is_ntt(old_poly));
}

// reduce polynomial RNS from P*Q to Q
void Reduce_rns_base(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                     CRT_CONTEXT* crt) {
  CRT_PRIMES* q_primes    = Get_q(crt);
  CRT_PRIMES* p_primes    = Get_p(crt);
  size_t      p_prime_len = Get_primes_cnt(p_primes);
  uint32_t    ring_degree = Get_rdgree(new_poly);
  IS_TRUE(old_poly && Get_rdgree(old_poly) == Get_rdgree(new_poly) &&
              Get_poly_level(old_poly) == Get_poly_level(new_poly) &&
              Get_num_p(old_poly) == p_prime_len,
          "reduce_rns_base: result size not match");
  // NOTE: after rescale num_primes of poly may not equal to num_primes of crt
  IS_TRUE(Get_poly_level(old_poly) <= Get_primes_cnt(q_primes),
          "reduce_rns_base: unmatched num primes");

  POLYNOMIAL old_poly_p_part;
  Extract_poly(&old_poly_p_part, old_poly, Get_num_q(new_poly), p_prime_len);

  if (Is_ntt(old_poly)) {
    Conv_ntt2poly_inplace_with_primes(&old_poly_p_part, crt, Get_p_primes(crt));
  }
  Fast_base_conv(new_poly, &old_poly_p_part, q_primes, p_primes);
  if (Is_ntt(old_poly)) {
    Conv_poly2ntt_inplace(new_poly, crt);
  }
  CRT_PRIME* q_prime      = Get_prime_at(q_primes, 0);
  int64_t*   p_m_inv_modq = Get_i64_values(Get_pinvmodq(p_primes));
  int64_t*   new_data     = Get_poly_coeffs(new_poly);
  int64_t*   old_q_data   = Get_poly_coeffs(old_poly);
  for (size_t q_idx = 0; q_idx < Get_poly_level(old_poly); q_idx++) {
    MODULUS* q_modulus = Get_modulus(q_prime);
    int64_t  qi        = Get_mod_val(q_modulus);
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      int64_t new_value = Sub_int64_with_mod(*old_q_data++, *new_data, qi);
      *new_data++ = Mul_int64_mod_barret(new_value, *p_m_inv_modq, q_modulus);
    }
    p_m_inv_modq++;
    q_prime++;
  }
  Set_is_ntt(new_poly, Is_ntt(old_poly));
}

CRT_CONTEXT* Debug_crt = NULL;

void Print_poly_rawdata(FILE* fp, POLYNOMIAL* poly) {
  IS_TRUE(poly, "null polynomial");
  if (Get_poly_len(poly) == 0) {
    fprintf(fp, "{}\n");
    return;
  }
  fprintf(fp, "is_ntt: %s, raw data: \n", poly->_is_ntt ? "Y" : "N");
  fprintf(fp, "\ndata: [");
  for (size_t i = 0; i < poly->_num_primes * poly->_ring_degree; i++) {
    fprintf(fp, " %ld", poly->_data[i]);
  }
  fprintf(fp, "]\n");
  for (size_t i = 0; i < poly->_num_primes; i++) {
    fprintf(fp, "\nQ%ld: [", i);
    int64_t* coeffs = poly->_data + i * poly->_ring_degree;
    for (int64_t j = 0; j < poly->_ring_degree; j++) {
      fprintf(fp, "%ld ", coeffs[j]);
    }
    fprintf(fp, " ]");
  }
  for (size_t i = 0; i < poly->_num_primes_p; i++) {
    fprintf(fp, "\nP%ld: [", i);
    int64_t* coeffs =
        poly->_data + (poly->_num_primes + i) * poly->_ring_degree;
    for (int64_t j = 0; j < poly->_ring_degree; j++) {
      fprintf(fp, "%ld ", coeffs[j]);
    }
    fprintf(fp, " ]");
  }
  fprintf(fp, "\n");
}

void Print_poly_detail(FILE* fp, POLYNOMIAL* poly, VL_CRTPRIME* primes,
                       bool detail) {
  IS_TRUE(poly, "null polynomial");
  if (Get_poly_len(poly) == 0) {
    fprintf(fp, "{}\n");
    return;
  }
  fprintf(fp, "level = %ld; ", Get_poly_level(poly));
  fprintf(fp, "%d * %ld", poly->_ring_degree, poly->_num_primes);
  if (poly->_num_primes_p) {
    fprintf(fp, "* %ld", poly->_num_primes_p);
  }
  fprintf(fp, " {");
  for (size_t i = 0; i < poly->_num_primes; i++) {
    fprintf(fp, " {");
    int64_t* coeffs = poly->_data + i * poly->_ring_degree;
    for (int64_t j = poly->_ring_degree - 1; j >= 0; j--) {
      if (coeffs[j]) {
        fprintf(fp, "%ld", coeffs[j]);
        if (j != 0) {
          fprintf(fp, "x^%ld + ", j);
        }
      }
    }
    fprintf(fp, " }");
  }
  for (size_t i = 0; i < poly->_num_primes_p; i++) {
    fprintf(fp, " {");
    int64_t* coeffs = poly->_data + poly->_num_primes * poly->_ring_degree +
                      i * poly->_ring_degree;
    for (int64_t j = poly->_ring_degree - 1; j >= 0; j--) {
      if (coeffs[j]) {
        fprintf(fp, "%ld", coeffs[j]);
        if (j != 0) {
          fprintf(fp, "x^%ld + ", j);
        }
      }
    }
    fprintf(fp, " }");
  }
  fprintf(fp, "}\n");
  Print_poly_rawdata(fp, poly);

  if (detail && primes) {
    if (poly->_is_ntt) {
      POLYNOMIAL non_ntt;
      Alloc_poly_data(&non_ntt, poly->_ring_degree, poly->_num_primes,
                      poly->_num_primes_p);
      Conv_ntt2poly_with_primes(&non_ntt, poly, Debug_crt, primes);
      Print_poly_rawdata(fp, &non_ntt);
      Free_poly_data(&non_ntt);
    } else {
      POLYNOMIAL ntt;
      Alloc_poly_data(&ntt, poly->_ring_degree, poly->_num_primes,
                      poly->_num_primes_p);
      Conv_poly2ntt_with_primes(&ntt, poly, Debug_crt, primes);
      Print_poly_rawdata(fp, &ntt);
      Free_poly_data(&ntt);
    }
  }
  fprintf(fp, "\n\n");
}

void Print_poly(FILE* fp, POLYNOMIAL* poly) {
  // Print_poly_detail(fp, poly, NULL, FALSE);
  Print_poly_rawdata(fp, poly);
}

void Print_rns_poly(FILE* fp, POLYNOMIAL* poly) {
  VALUE_LIST* res = Alloc_value_list(BIGINT_TYPE, poly->_ring_degree);
  if (poly->_num_primes_p) {
    Reconstruct_qpbase_to_values(res, poly, Debug_crt);
  } else {
    Reconstruct_qbase_to_values(res, poly, Debug_crt);
  }
  for (int64_t idx = poly->_ring_degree - 1; idx >= 0; idx--) {
    if (BI_CMP_SI(BIGINT_VALUE_AT(res, idx), 0) == 0) continue;
    BI_FPRINTF(fp, BI_FORMAT, BIGINT_VALUE_AT(res, idx));
    if (idx != 0) {
      BI_FPRINTF(fp, "x^%ld + ", idx);
    }
  }
  fprintf(fp, "\n");
  Free_value_list(res);
}

void Print_poly_list(FILE* fp, VALUE_LIST* poly_list) {
  FOR_ALL_ELEM(poly_list, idx) {
    POLYNOMIAL* poly = (POLYNOMIAL*)Get_ptr_value_at(poly_list, idx);
    fprintf(fp, "POLY[%ld]\n", idx);
    Print_poly(fp, poly);
  }
}

POLYNOMIAL* Rescale_poly(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  VALUE_LIST* coeff_modulus = Get_q_primes(crt);
  size_t      level         = Get_poly_level(poly);
  size_t      res_level     = Get_poly_level(res);
  IS_TRUE(res_level == level && level <= LIST_LEN(coeff_modulus),
          "Rescale_poly: primes not match");
  IS_TRUE(Get_rdgree(res) == Get_rdgree(poly),
          "Rescale_poly: degree not match");
  FMT_ASSERT(level > 1, "Rescale_poly: level not enough after rescale");

  size_t       degree      = Get_rdgree(poly);
  int64_t*     result      = Get_poly_coeffs(res);
  int64_t*     poly_coeffs = Get_poly_coeffs(poly);
  int64_t*     last_coeffs = poly_coeffs + (level - 1) * degree;
  CRT_PRIMES*  primes      = Get_q(crt);
  VL_CRTPRIME* q_primes    = Get_primes(primes);
  CRT_PRIME*   last_prime  = Get_vlprime_at(q_primes, level - 1);
  int64_t      last_mod    = Get_modulus_val(last_prime);
  // get precomputed value
  VL_I64* ql_inv_mod_qi      = Get_ql_inv_mod_qi_at(primes, level - 2);
  VL_I64* ql_inv_mod_qi_prec = Get_ql_inv_mod_qi_prec_at(primes, level - 2);
  VL_I64* ql_ql_inv_mod_ql_div_ql_mod_qi =
      Get_ql_ql_inv_mod_ql_div_ql_mod_qi_at(primes, level - 2);
  VL_I64* ql_ql_inv_mod_ql_div_ql_mod_qi_prec =
      Get_ql_ql_inv_mod_ql_div_ql_mod_qi_prec_at(primes, level - 2);
  if (Is_ntt(poly)) {
    // Convert last input to non-NTT form
    VALUE_LIST* last_input_ntt_form = Alloc_value_list(I64_TYPE, degree);
    Init_i64_value_list(last_input_ntt_form, Get_rdgree(poly), last_coeffs);
    NTT_CONTEXT* last_ntt_table = Get_ntt(last_prime);
    Ftt_inv(last_input_ntt_form, last_ntt_table, last_input_ntt_form);
    last_coeffs = Get_i64_values(last_input_ntt_form);

    VALUE_LIST* last_input = Alloc_value_list(I64_TYPE, degree);
    for (size_t module_idx = 0; module_idx < Get_poly_level(poly) - 1;
         module_idx++) {
      CRT_PRIME*   prime   = Get_vlprime_at(q_primes, module_idx);
      NTT_CONTEXT* ntt     = Get_ntt(prime);
      MODULUS*     modulus = Get_modulus(prime);
      int64_t      mod     = Get_mod_val(modulus);
      int64_t      qlql_mod_inv =
          Get_i64_value_at(ql_ql_inv_mod_ql_div_ql_mod_qi, module_idx);
      int64_t qlql_mod_inv_prec =
          Get_i64_value_at(ql_ql_inv_mod_ql_div_ql_mod_qi_prec, module_idx);
      int64_t mod_inverse = Get_i64_value_at(ql_inv_mod_qi, module_idx);
      int64_t mod_inverse_prec =
          Get_i64_value_at(ql_inv_mod_qi_prec, module_idx);
      // Switch mod for last input from ql to qi
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        I64_VALUE_AT(last_input, poly_idx) = Fast_mul_const_with_mod(
            Switch_modulus(last_coeffs[poly_idx], last_mod, mod), qlql_mod_inv,
            qlql_mod_inv_prec, mod);
      }
      // Transform last input back to ntt
      Ftt_fwd(last_input, ntt, last_input);
      for (size_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *result = Fast_mul_const_with_mod(*poly_coeffs, mod_inverse,
                                          mod_inverse_prec, mod);
        *result = Add_int64_with_mod(
            *result, Get_i64_value_at(last_input, poly_idx), mod);
        poly_coeffs++;
        result++;
      }
    }
    Free_value_list(last_input_ntt_form);
    Free_value_list(last_input);
    Set_is_ntt(res, TRUE);
  } else {
    CRT_PRIME* prime_head = Get_vlprime_at(coeff_modulus, 0);
    CRT_PRIME* prime      = prime_head;
    for (size_t module_idx = 0; module_idx < Get_poly_level(poly) - 1;
         module_idx++) {
      MODULUS* modulus     = Get_modulus(prime);
      int64_t  mod         = Get_mod_val(modulus);
      int64_t  mod_inverse = Get_i64_value_at(ql_inv_mod_qi, module_idx);
      int64_t  mod_inverse_prec =
          Get_i64_value_at(ql_inv_mod_qi_prec, module_idx);
      for (size_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        int64_t last_coeff = Mod_barrett_64(last_coeffs[poly_idx], modulus);
        int64_t new_val    = Sub_int64_with_mod(*poly_coeffs, last_coeff, mod);
        *result            = Fast_mul_const_with_mod(new_val, mod_inverse,
                                                     mod_inverse_prec, mod);
        poly_coeffs++;
        result++;
      }
      prime = Get_next_prime(prime);
    }
    Set_is_ntt(res, FALSE);
  }
  return res;
}

VALUE_LIST* Switch_key_precompute_no_fusion(POLYNOMIAL*  poly,
                                            CRT_CONTEXT* crt) {
  CRT_PRIMES* q_parts     = Get_qpart(crt);
  CRT_PRIMES* p           = Get_p(crt);
  size_t      ql          = poly->_num_primes;
  size_t      num_p       = Get_primes_cnt(p);
  size_t      part_size   = Get_per_part_size(q_parts);
  size_t      num_qpart   = Get_num_parts(q_parts);
  size_t      num_qpartl  = ceil((double)ql / part_size);
  size_t      ring_degree = poly->_ring_degree;
  if (num_qpartl > num_qpart) {
    num_qpartl = num_qpart;
  }

  POLYNOMIAL decomp_poly;
  Alloc_poly_data(&decomp_poly, ring_degree, ql + num_p, 0);
  VALUE_LIST* parts_ct_ext = Alloc_value_list(PTR_TYPE, num_qpartl);
  Alloc_poly_list(parts_ct_ext, ring_degree, ql, num_p);
  for (size_t part = 0; part < num_qpartl; part++) {
    VL_CRTPRIME* q_parts_primes = VL_VALUE_AT(Get_primes(q_parts), part);
    size_t decomp_poly_len      = part == num_qpartl - 1 ? ql - part_size * part
                                                         : LIST_LEN(q_parts_primes);
    Set_poly_level(&decomp_poly, decomp_poly_len);
    Decompose_poly(&decomp_poly, poly, crt, num_qpartl, part);

    POLYNOMIAL* ext = (POLYNOMIAL*)Get_ptr_value_at(parts_ct_ext, part);
    Raise_rns_base_with_parts(ext, &decomp_poly, crt, ql, part);
  }
  Free_poly_data(&decomp_poly);
  return parts_ct_ext;
}

//! @brief Fusion version of precompute for switch key
//! combined decompose and raise, faster than Switch_key_precompute_no_fusion
//! about 10%, note that new memory is returned
VALUE_LIST* Switch_key_precompute_fusion(POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  CRT_PRIMES* q_parts    = Get_qpart(crt);
  size_t      num_q      = Get_num_q(poly);
  size_t      num_p      = Get_crt_num_p(crt);
  size_t      num_decomp = Get_num_decomp_poly(poly, crt);
  uint32_t    degree     = Get_rdgree(poly);

  VALUE_LIST* poly_raised_list = Alloc_value_list(PTR_TYPE, num_decomp);
  Alloc_poly_list(poly_raised_list, degree, num_q, num_p);
  FOR_ALL_ELEM(poly_raised_list, decomp_idx) {
    POLYNOMIAL* raised =
        (POLYNOMIAL*)Get_ptr_value_at(poly_raised_list, decomp_idx);
    Decompose_modup(raised, poly, crt, num_decomp, decomp_idx);
  }
  return poly_raised_list;
}

void Decompose_modup(POLYNOMIAL* raised, POLYNOMIAL* poly, CRT_CONTEXT* crt,
                     size_t num_decomp, size_t decomp_idx) {
  uint32_t     degree    = Get_rdgree(poly);
  CRT_PRIMES*  q_parts   = Get_qpart(crt);
  size_t       part_size = Get_per_part_size(q_parts);
  size_t       num_q     = Get_num_q(poly);
  VL_CRTPRIME* compl_primes =
      Get_qpart_compl_at(Get_qpart_compl(crt), num_q - 1, decomp_idx);
  VL_CRTPRIME* part2_primes = Get_vl_value_at(Get_primes(q_parts), decomp_idx);

  // number of primes of part1,2,3
  size_t num_part1       = Get_per_part_size(q_parts) * decomp_idx;
  size_t num_part2       = (decomp_idx == num_decomp - 1)
                               ? num_q - part_size * decomp_idx
                               : LIST_LEN(part2_primes);
  size_t num_part3       = Get_num_pq(raised) - num_part1 - num_part2;
  size_t num_compl       = LIST_LEN(compl_primes);
  size_t part2_start_idx = num_part1;
  size_t part2_end_idx   = part2_start_idx + num_part2;
  size_t part3_start_idx = num_part1 + num_part2;

  // decompose poly at decomp_idx to part2
  POLYNOMIAL part2_poly;
  Extract_poly(&part2_poly, raised, part2_start_idx, num_part2);
  int64_t* part2_coeffs = Get_poly_coeffs(raised);
  int64_t* poly_coeffs  = Get_poly_coeffs(poly);
  for (size_t i = part2_start_idx; i < part2_end_idx; i++) {
    for (size_t d = 0, idx = i * degree; d < degree; d++, idx++) {
      part2_coeffs[idx] = poly_coeffs[idx];
    }
  }
  Set_is_ntt(&part2_poly, Is_ntt(poly));

  // raise part2 to part1 and part3
  POLYNOMIAL  part2_poly_intt;
  POLYNOMIAL* part2_poly_intt_ptr;
  if (Is_ntt(poly)) {
    Alloc_poly_data(&part2_poly_intt, degree, num_part2, 0);
    Conv_ntt2poly_with_primes(&part2_poly_intt, &part2_poly, crt, part2_primes);
    part2_poly_intt_ptr = &part2_poly_intt;
  } else {
    part2_poly_intt_ptr = &part2_poly;
  }

  VALUE_LIST* ql_hatinvmodq =
      VL_L2_VALUE_AT(Get_qlhatinvmodq(q_parts), decomp_idx, num_part2 - 1);
  VL_VL_I64* ql_hatmodp =
      VL_L2_VALUE_AT(Get_qlhatmodp(q_parts), num_q - 1, decomp_idx);

  VL_CRTPRIME part1_primes, part3_primes;
  Extract_value_list(&part1_primes, compl_primes, 0, num_part1);
  Extract_value_list(&part3_primes, compl_primes, num_part1, num_part3);

  INT128_T sum[num_compl];
  memset(sum, 0, num_compl * sizeof(INT128_T));

  for (size_t n = 0; n < degree; n++) {
    for (size_t i = 0; i < num_part2; i++) {
      int64_t  val = Get_coeff_at(part2_poly_intt_ptr, i * degree + n);
      MODULUS* qi  = (MODULUS*)Get_modulus(Get_vlprime_at(part2_primes, i));
      int64_t  mul_inv_modq =
          Mul_int64_mod_barret(val, Get_i64_value_at(ql_hatinvmodq, i), qi);
      VALUE_LIST* qhat_modp = VL_VALUE_AT(ql_hatmodp, i);
      for (size_t j = 0; j < num_compl; j++) {
        sum[j] += (INT128_T)mul_inv_modq * Get_i64_value_at(qhat_modp, j);
      }
    }
    for (size_t j = 0; j < num_part1; j++) {
      int64_t new_value = Mod_barrett_128(
          sum[j], Get_modulus(Get_vlprime_at(&part1_primes, j)));
      Set_coeff_at(raised, new_value, j * degree + n);
      sum[j] = 0;
    }

    for (size_t j1 = 0, j2 = num_part1; j1 < num_part3; j1++, j2++) {
      int64_t new_value = Mod_barrett_128(
          sum[j2], Get_modulus(Get_vlprime_at(&part3_primes, j1)));
      Set_coeff_at(raised, new_value, (j1 + part3_start_idx) * degree + n);
      sum[j2] = 0;
    }
  }
  if (Is_ntt(poly)) {
    POLYNOMIAL part1_poly, part3_poly;
    Extract_poly(&part1_poly, raised, 0, num_part1);
    Extract_poly(&part3_poly, raised, part3_start_idx, num_part3);
    Set_is_ntt(&part1_poly, false);
    Set_is_ntt(&part3_poly, false);
    Conv_poly2ntt_inplace_with_primes(&part1_poly, crt, &part1_primes);
    Conv_poly2ntt_inplace_with_primes(&part3_poly, crt, &part3_primes);
    Set_is_ntt(raised, true);
    Free_poly_data(&part2_poly_intt);
  } else {
    Set_is_ntt(raised, false);
  }
}

VALUE_LIST* Switch_key_precompute(POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  if (Get_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP)) {
    return Switch_key_precompute_fusion(poly, crt);
  } else {
    return Switch_key_precompute_no_fusion(poly, crt);
  }
}

void Free_switch_key_precomputed(VALUE_LIST* precomputed) {
  Free_poly_list(precomputed);
}

void Sample_uniform_poly(POLYNOMIAL* poly, VL_CRTPRIME* q_primes,
                         VL_CRTPRIME* p_primes) {
  MODULUS*   q_modulus = Get_modulus_head(q_primes);
  VALUE_LIST samples;
  int64_t*   coeffs   = Get_poly_coeffs(poly);
  uint32_t   r_degree = Get_rdgree(poly);
  for (size_t module_idx = 0; module_idx < Get_poly_level(poly); module_idx++) {
    Init_i64_value_list_no_copy(&samples, r_degree,
                                coeffs + (module_idx * r_degree));
    Sample_uniform(&samples, Get_mod_val(q_modulus));
    q_modulus = Get_next_modulus(q_modulus);
  }
  if (p_primes) {
    coeffs             = Get_p_coeffs(poly);
    MODULUS* p_modulus = Get_modulus_head(p_primes);
    for (size_t module_idx = 0; module_idx < Get_num_p(poly); module_idx++) {
      Init_i64_value_list_no_copy(&samples, r_degree,
                                  coeffs + (module_idx * r_degree));
      Sample_uniform(&samples, Get_mod_val(p_modulus));
      p_modulus = Get_next_modulus(p_modulus);
    }
  }
}

void Sample_ternary_poly(POLYNOMIAL* poly, VL_CRTPRIME* q_primes,
                         VL_CRTPRIME* p_primes, size_t hamming_weight) {
  MODULUS* q_modulus = Get_modulus_head(q_primes);
  uint32_t r_degree  = Get_rdgree(poly);

  // ternary uniform for secret key only need to sample one group of r_degree
  // data, then the sampled value transformed to different q base
  VALUE_LIST* samples = Alloc_value_list(I64_TYPE, r_degree);
  Sample_ternary(samples, hamming_weight);

  int64_t* coeffs      = Get_poly_coeffs(poly);
  int64_t* sample_data = Get_i64_values(samples);
  for (size_t module_idx = 0; module_idx < Get_poly_level(poly); module_idx++) {
    for (size_t idx = 0; idx < r_degree; idx++) {
      coeffs[idx] = sample_data[idx];
      if (coeffs[idx] < 0) {
        coeffs[idx] += Get_mod_val(q_modulus);
      }
    }
    coeffs += r_degree;
    q_modulus = Get_next_modulus(q_modulus);
  }

  // for p part, switch to p modulus based on q0
  if (p_primes) {
    coeffs              = Get_p_coeffs(poly);
    int64_t* q0         = Get_poly_coeffs(poly);
    int64_t  q0_mod_val = Get_mod_val(Get_modulus_head(q_primes));
    MODULUS* p_modulus  = Get_modulus_head(p_primes);
    for (size_t module_idx = 0; module_idx < Get_num_p(poly); module_idx++) {
      for (size_t idx = 0; idx < r_degree; idx++) {
        coeffs[idx] =
            Switch_modulus(q0[idx], q0_mod_val, Get_mod_val(p_modulus));
      }
      coeffs += r_degree;
      p_modulus = Get_next_modulus(p_modulus);
    }
  }
  Free_value_list(samples);
}
