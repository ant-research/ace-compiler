//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_FHE_UTILS_H
#define RTLIB_INCLUDE_FHE_UTILS_H

#include <math.h>

#include "util/fhe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration
static inline UINT128_T Precompute_const_128(uint64_t mod);

/**
 * @brief modulus include precomputed barret reduce k & m
 *
 */
typedef struct {
  int64_t   _val;
  int64_t   _br_k;     // precomputed barret reduce k, m/2^k ~= 1/val
  int64_t   _br_m;     // precomputed barret reduce m, m/2^k ~= 1/val
  UINT128_T _prec128;  // precomputed barret for  1 << 128 / val
} MODULUS;

/**
 * @brief get normal value of MODULUS
 *
 * @param m input MUDULUS*
 * @return int64_t
 */
static inline int64_t Get_mod_val(MODULUS* m) { return m->_val; }

/**
 * @brief get barret reduce k from MODULUS
 *
 * @param m input MODULUS *
 * @return int64_t
 */
static inline int64_t Get_br_k(MODULUS* m) { return m->_br_k; }

/**
 * @brief get barret reduce m from MODULUS
 *
 * @param m input MODULUS *
 * @return int64_t
 */
static inline int64_t Get_br_m(MODULUS* m) { return m->_br_m; }

static inline UINT128_T Get_prec128(MODULUS* m) { return m->_prec128; }

/**
 * @brief initialize MODULUS
 *
 * @param m
 * @param val
 */
static inline void Init_modulus(MODULUS* m, int64_t val) {
  m->_val     = val;
  m->_br_k    = 2 * ((int64_t)log2(val) + 1);
  m->_br_m    = (int64_t)(((INT128_T)(1) << m->_br_k) / val);
  m->_prec128 = Precompute_const_128(val);
}

/**
 * @brief alloc MODULUS for given size
 *
 * @param n
 * @return MODULUS*
 */
static inline MODULUS* Alloc_modulus(size_t n) {
  return (MODULUS*)malloc(sizeof(MODULUS) * n);
}

/**
 * @brief print modulus
 *
 * @param fp input file
 * @param m input modulus
 */
static inline void Print_modulus(FILE* fp, MODULUS* m) {
  if (m == NULL) return;
  fprintf(fp, "{val = %ld, br_k = %ld, br_m = %ld} ", Get_mod_val(m),
          Get_br_k(m), Get_br_m(m));
}

/**
 * @brief get modulus value of input modulo (int64_t)
 * C %(remainder) operator is not the same as mathematical modulo(python %)
 * operation Mathematical: modulus(val, modulo) = val - (modulo *
 * Floor(val/modulo))
 * the sign of the result is the same as the divisor (modulo).
 * C: val % modulo = val - modulo * (val/modulo)
 *   the sign of the result is the same as the sign of the dividend(val)
 * Ex: -4 % 73
 *     Mathematical result = 69
 *     C result = -4
 *
 * @param val
 * @param modulo
 * @return int64_t
 */
static inline int64_t Mod_int64(int64_t val, int64_t modulo) {
  IS_TRUE(modulo >= 0, "invalid modulo");
  int64_t ret = val % modulo;
  if (ret < 0) {
    ret += modulo;
  }
  return ret;
}

/**
 * @brief get modulus value of input modulo (int128)
 *
 * @param val
 * @param modulo
 * @return INT128_T
 */
static inline INT128_T Mod_int128(INT128_T val, int64_t modulo) {
  IS_TRUE(modulo != 0, "zero modulo");
  if (modulo < 0) {
    return -Mod_int128(-val, -modulo);
  }
  INT128_T ret = val % modulo;
  if (ret < 0) {
    ret += modulo;
  }
  return ret;
}

/**
 * @brief get modulus value of input modulo (BIG_INT)
 *
 * @param val
 * @param modulo
 * @return int64_t
 */
static inline int64_t Mod_bigint(BIG_INT val, int64_t modulo) {
  IS_TRUE(modulo > 0, "negative or zero modulo");
  BIG_INT mod_res;
  BI_INIT(mod_res);
  BI_MOD_UI(mod_res, val, (uint64_t)modulo);
  IS_TRUE(Is_bigint_in_64bit(mod_res), "mod res not 64 bit");
  int64_t ret = BI_GET_SI(mod_res);
  BI_FREES(mod_res);
  return ret;
}

/**
 * @brief input val is within 64 bits or not
 *
 * @param val input val
 * @return true within 64 bits
 * @return false not within 64 bits
 */
static inline bool Is_128_in_64bit(INT128_T val) {
  return (val < MAX_INT64) && (val > MIN_INT64);
}

/**
 * @brief modular multiplication for int64
 *
 * @param val1 input val1
 * @param val2 input val2
 * @param modulus modulus of modular multiplication
 * @return int64_t
 */
static inline int64_t Mul_int64_with_mod(int64_t val1, int64_t val2,
                                         int64_t modulus) {
  INT128_T res = (INT128_T)val1 * (INT128_T)val2;
  res          = Mod_int128(res, modulus);
  IS_TRUE(Is_128_in_64bit(res), "mul_int64_with_mod: res overflow");
  return (int64_t)res;
}

/**
 * @brief modular addition for int64
 *
 * @param val1 value to be computed
 * @param val2 value to be computed
 * @param modulus modulus of modular addition
 * @return int64_t
 */
static inline int64_t Add_int64_with_mod(int64_t val1, int64_t val2,
                                         int64_t modulus) {
  IS_TRUE(val1 + val2 < (modulus << 1), "add operand outof range");
  val1 += val2;
  return val1 >= modulus ? val1 - modulus : val1;
}

/**
 * @brief modular subtraction for int64
 *
 * @param val1 value to be computed
 * @param val2 value to be computed
 * @param modulus modulus of modular subtraction
 * @return int64_t
 */
static inline int64_t Sub_int64_with_mod(int64_t val1, int64_t val2,
                                         int64_t modulus) {
  IS_TRUE(val1 <= modulus && val2 <= modulus, "sub operands outof range");
  int64_t res = val1 - val2;
  if (res > val1 || res < 0) {
    res = res + modulus;
  }
  return res;
}

//! Barrett Modular Reduction (require:  0 <= val < mod_val^2)
static inline int64_t Mod_barrett_64(int64_t val, MODULUS* modulus) {
  int64_t mod_val = Get_mod_val(modulus);
  IS_TRUE(val < (INT128_T)mod_val * mod_val, "val not supported for barret");
  int64_t   m = Get_br_m(modulus);
  int64_t   k = Get_br_k(modulus);
  int64_t   res;
  UINT128_T tmp = (UINT128_T)val * m;
  tmp >>= k;
  tmp *= mod_val;
  tmp = val - tmp;
  IS_TRUE(Is_128_in_64bit(tmp), "mod_barret overflow");
  res = (int64_t)tmp;
  if (res >= mod_val) {
    return res - mod_val;
  } else {
    return res;
  }
}

//! @brief Barrett modulus reduction for 128bit
//! @param val value to be mod
//! @param modulus modulus for Barrett Modular Reduction
//! @return int64_t
static inline int64_t Mod_barrett_128(UINT128_T val, MODULUS* mod) {
  UINT128_T mu      = Get_prec128(mod);
  int64_t   mod_val = Get_mod_val(mod);
  uint64_t  val_l64 = (uint64_t)val;
  uint64_t  val_h64 = val >> 64;
  uint64_t  mu_l64  = (uint64_t)(mu);
  uint64_t  mu_h64  = mu >> 64;

  // mul left parts, discard lower 64 bits
  uint64_t left_h64 = (uint64_t)(((UINT128_T)val_l64 * mu_l64) >> 64);
  // mul middle first
  UINT128_T mid     = (UINT128_T)val_l64 * mu_h64;
  uint64_t  mid_l64 = (uint64_t)mid;
  uint64_t  mid_h64 = mid >> 64;

  // accumulate and check carray
  uint64_t tmp_1 = mid_l64 + left_h64;
  uint64_t carry = tmp_1 < left_h64;
  // accumulate
  uint64_t tmp_2 = mid_h64 + carry;

  // mul middle second
  mid     = (UINT128_T)val_h64 * mu_l64;
  mid_l64 = (uint64_t)mid;
  mid_h64 = mid >> 64;

  // check carry, is addition overflowed
  carry = (mid_l64 + tmp_1) < tmp_1;
  // accumulate
  left_h64 = mid_h64 + carry;

  // now we have the lower word of (a * mu)/2^128, no need for higher word
  tmp_1 = val_h64 * mu_h64 + tmp_2 + left_h64;

  // subtract lower words only, higher words should be the same
  uint64_t res = val_l64 - tmp_1 * mod_val;

  while (res >= mod_val) res -= mod_val;
  return res;
}

/**
 * @brief modular multiplication with precomputed barret reduce
 *
 * @param val1 value to be computed
 * @param val2 value to be computed
 * @param modulus modulus of modular multiplication
 * @return int64_t
 */
static inline int64_t Mul_int64_mod_barret(int64_t val1, int64_t val2,
                                           MODULUS* modulus) {
  // Tricky notes: remove exp will got worse perf results
  int64_t   exp = Mul_int64_with_mod(val1, val2, Get_mod_val(modulus));
  UINT128_T res = (UINT128_T)val1 * (UINT128_T)val2;
  int64_t   r   = Mod_barrett_128(res, modulus);
  if (exp != r) {
    IS_TRUE(FALSE, "wrong results");
  }
  return r;
}

/**
 * @brief
 *
 * @param val1
 * @param val2
 * @param val2_inv
 * @param mod
 * @return int64_t
 */
static inline int64_t Fast_mul_const_with_mod(uint64_t val1, uint64_t val2,
                                              uint64_t val2_inv, uint64_t mod) {
  uint64_t q      = (uint64_t)(((UINT128_T)val1 * val2_inv) >> 64);
  uint64_t yprime = (val1 * val2) - q * mod;
  uint64_t res    = yprime >= mod ? yprime - mod : yprime;
  IS_TRUE(res < mod, "invalid reduction");
  return (int64_t)res;
}

/**
 * @brief check if the value is a power of two
 *
 * @param value value to be checked
 * @return true power of two
 * @return false not power of two
 */
static inline bool Is_power_of_two(int64_t value) {
  if (value == 0 || (value & (value - 1)) != 0) {
    return FALSE;
  }
  return TRUE;
}
static inline int64_t Max_64bit_value() {
  return (int64_t)(((uint64_t)(1) << 63) - ((uint64_t)(1) << 9)) - 1;
}
//! @brief Max 128 bit value used in 128 bit encoder
static inline INT128_T Max_128bit_value() {
  // 2^127-2^73-1 - max value that could be rounded to INT128_T
  return ((UINT128_T)1 << 127) - ((UINT128_T)1 << 73) - (UINT128_T)1;
}

//! @brief Check if a double exceed Max_128bit_value()
static inline bool Is_128bit_overflow(double d) {
  const double epsilon = 0.000001;
  return epsilon < (fabs(d) - Max_128bit_value());
}

//! @brief Switches the integer corresponding to the new modulus
static inline int64_t Switch_modulus(int64_t val, int64_t old_mod,
                                     int64_t new_mod) {
  // Algorithm:
  // delta = abs(old_mod - new_mod ):
  //  Case 1: old_mod < new_mod
  //    if val > old_mod / 2
  //      val' = i + delta
  //  Case 2: old_mod > new_mod
  //    i > old_mod / 2  val' = i - delta
  int64_t res      = val;
  int64_t half_mod = old_mod >> 1;
  if (new_mod > old_mod) {
    if (res > half_mod) {
      res += (new_mod - old_mod);
    }
  } else {
    int64_t diff = new_mod - (old_mod % new_mod);

    if (res > half_mod) {
      res += diff;
    }
    if (res >= new_mod) {
      res = res % new_mod;
    }
  }
  return res;
}

//! @brief Precomputation for a multiplicand.
static inline uint64_t Precompute_const(uint64_t val, uint64_t mod) {
  UINT128_T precom = ((UINT128_T)val << 64) / mod;
  uint64_t  ret    = (uint64_t)precom;
  return ret;
}

//! @brief Precompute {(1<<128) / mod} for Mod_barrett_128
static inline UINT128_T Precompute_const_128(uint64_t mod) {
  BIG_INT bi_mod, two_power128, two_power64, div, low64, hi64;
  BI_INITS(low64, hi64, div);
  BI_INIT_ASSIGN_SI(two_power128, 1);
  BI_INIT_ASSIGN_SI(two_power64, 1);
  BI_INIT_ASSIGN_UI(bi_mod, mod);
  BI_LSHIFT(two_power128, 128);
  BI_LSHIFT(two_power64, 64);
  BI_DIV(div, two_power128, bi_mod);
  UINT128_T ret;
  BI_MOD(low64, div, two_power64);
  BI_RSHIFT(hi64, div, 64);
  ret = BI_GET_UI(low64);
  ret += (UINT128_T)BI_GET_UI(hi64) << 64;
  BI_FREES(bi_mod, two_power128, two_power64, div, low64, hi64);
  return ret;
}

#ifdef __cplusplus
}
#endif
#endif  // RTLIB_INCLUDE_FHE_UTILS_H