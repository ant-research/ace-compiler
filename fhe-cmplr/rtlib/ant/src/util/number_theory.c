//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/number_theory.h"

#include "util/bit_operations.h"
#include "util/fhe_std_parms.h"
#include "util/fhe_utils.h"

int64_t Fast_mod_exp(int64_t val, int64_t exp, MODULUS* modulus) {
  // It should use int128_t to ensure val will not exceed half of MAX_INT64,
  // Otherwise, the result will be wrong
  int64_t mod_val = Get_mod_val(modulus);
  int64_t result  = 1;
  int64_t base    = val % mod_val;
  while (exp > 0) {
    if (exp & 1) {
      result = Mul_int64_mod_barret(result, base, modulus);
    }
    base = Mul_int64_mod_barret(base, base, modulus);
    exp >>= 1;
  }
  if (result < 0) {
    result += mod_val;
  }
  return result;
}

int64_t Mod_exp(int64_t val, int64_t exp, int64_t modulus) {
  // It should use int128_t to ensure val will not exceed half of MAX_INT64,
  // Otherwise, the result will be wrong
  int64_t result = 1;
  int64_t base   = val % modulus;
  while (exp > 0) {
    if (exp & 1) {
      result = Mul_int64_with_mod(result, base, modulus);
    }
    base = Mul_int64_with_mod(base, base, modulus);
    exp >>= 1;
  }
  if (result < 0) {
    result += modulus;
  }
  return result;
}

// modulus value must be prime
int64_t Mod_inv_prime(int64_t val, MODULUS* modulus) {
  FMT_ASSERT(Is_prime(Get_mod_val(modulus)), "not a prime");
  return Fast_mod_exp(val, Get_mod_val(modulus) - 2, modulus);
}

int64_t Mod_inv(int64_t val, MODULUS* modulus) {
  int64_t mod_value = Get_mod_val(modulus);
  int64_t a         = val % mod_value;
  FMT_ASSERT(a != 0, "val does not have Mod_inv");
  if (mod_value == 1) {
    return 0;
  }
  int64_t y = 0;
  int64_t x = 1;
  while (a > 1) {
    int64_t t = mod_value;
    int64_t q = a / t;
    mod_value = a % t;
    a         = t;
    t         = y;
    y         = x - q * y;
    x         = t;
  }
  if (x < 0) {
    x += Get_mod_val(modulus);
  }
  return x;
}

void Bi_mod_inv(BIG_INT result, BIG_INT val, int64_t modulus) {
  IS_TRUE(Is_prime(modulus), " Bi_mod_inv: modulus not a prime");
  BIG_INT tmp, bm;
  BI_INITS(tmp, bm);
  BI_ASSIGN_SI(bm, modulus);
  BI_MOD(tmp, val, bm);
  mpz_invert(result, tmp, bm);
  BI_FREES(tmp, bm);
}

int64_t Find_generator(MODULUS* modulus) {
  int64_t mod_val = Get_mod_val(modulus);
  IS_TRUE(Is_prime(mod_val), "modulus not a prime");
  int64_t phi = mod_val - 1;

  // find prime factors of phi
  int64_t       len_max = (int64_t)log2(phi);
  PRIME_FACTOR* s       = (PRIME_FACTOR*)malloc(sizeof(PRIME_FACTOR) +
                                                sizeof(int64_t) * (size_t)len_max);
  s->_len               = 0;
  int64_t number        = phi;
  for (int64_t i = 2; i <= (int64_t)sqrt(number); i++) {
    if (number % i == 0) {
      s->_len += 1;
      s->_factor[s->_len - 1] = i;
      while (number % i == 0) {
        number /= i;
      }
    }
  }
  if (number > 1) {
    s->_len += 1;
    s->_factor[s->_len - 1] = number;
  }
  IS_TRUE((size_t)len_max >= s->_len, ("length of  PRIME_FACTOR exceeded"));

  // get generator of PRIME modulus
  for (int64_t r = 2; r <= phi; r++) {
    bool flag = false;
    for (size_t i = 0; i <= s->_len - 1; i++) {
      if (Fast_mod_exp(r, phi / s->_factor[i], modulus) == 1) {
        flag = true;
        break;
      }
    }
    if (flag == false) {
      free(s);
      return r;
    }
  }
  free(s);
  return -1;
}

int64_t Root_of_unity(int64_t order, MODULUS* modulus) {
  int64_t mod_val = Get_mod_val(modulus);
  IS_TRUE(Is_prime(mod_val), "modulus not a prime");
  // Must have order q | m - 1, where m is the modulus.
  // The values m and q do not satisfy this.
  FMT_ASSERT((mod_val - 1) % order == 0,
             "moudlus -1 should evenly divisible by order");
  int64_t hash_res = Get_rou(order, mod_val);
  if (hash_res) {
    return hash_res;
  }

  int64_t generator = Find_generator(modulus);
  // No primitive root of unity mod m
  IS_TRUE(generator, "null generator");

  int64_t result = Fast_mod_exp(generator, (mod_val - 1) / order, modulus);
  if (result == 1) {
    return Root_of_unity(order, modulus);
  }
  return result;
}

// reserved code: may need to compare performance
bool Is_prime(int64_t number) {
  if (number < 2) return false;
  if (number != 2 && number % 2 == 0) return false;

  // Find largest odd factor of n-1.
  int64_t exp = number - 1;
  while (exp % 2 == 0) {
    exp /= 2;
  }

  time_t t;
  srand((unsigned)time(&t));
  size_t num_trials = 200;
  for (size_t i = 1; i <= num_trials; i++) {
    int rand_val = rand() % number + 1;
    if (rand_val == number) continue;
    int64_t new_exp = exp;
    int64_t power   = Mod_exp(rand_val, new_exp, number);
    while (new_exp != number - 1 && power != 1 && power != number - 1) {
      power = Mul_int64_with_mod(power, power, number);
      new_exp *= 2;
    }
    if (power != number - 1 && new_exp % 2 == 0) return false;
  }
  return true;
}

uint32_t Find_automorphism_index(int32_t rot_idx, MODULUS* modulus) {
  if (rot_idx == 0) {
    return 1;
  } else if (rot_idx == (int32_t)(Get_mod_val(modulus) - 1)) {
    return (uint32_t)rot_idx;
  } else {
    int32_t gen = 5;
    if (rot_idx < 0) {
      gen = (int32_t)Mod_inv(5, modulus);
    }
    return Fast_mod_exp(gen, abs(rot_idx), modulus);
  }
}

void Precompute_automorphism_order(VALUE_LIST* precomp, uint32_t k, uint32_t n,
                                   bool is_ntt) {
  size_t   m            = n << 1;
  size_t   logm         = round(log2(m));
  size_t   logn         = round(log2(n));
  int64_t* precomp_data = Get_i64_values(precomp);
  if (is_ntt) {
    for (size_t j = 0; j < n; j++) {
      size_t j_tmp       = ((j << 1) + 1);
      size_t idx         = ((j_tmp * k) - (((j_tmp * k) >> logm) << logm)) >> 1;
      size_t jrev        = Reverse_bits(j, logn);
      size_t idxrev      = Reverse_bits(idx, logn);
      precomp_data[jrev] = idxrev;
    }
  } else {
    for (size_t j = 0; j < n; j++) {
      size_t npow  = j * k;
      size_t shift = npow % m;
      if (shift < n) {
        precomp_data[shift] = j;
      } else {
        precomp_data[shift - n] = -1 * j;
      }
    }
  }
}