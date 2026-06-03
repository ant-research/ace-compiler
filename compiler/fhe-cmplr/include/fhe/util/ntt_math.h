//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_UTIL_NTT_MATH_H
#define FHE_UTIL_NTT_MATH_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

namespace fhe {
namespace util {

typedef __uint128_t U128;

//! @brief compute (a+b)%mod.
static inline uint64_t Modadd(uint64_t a, uint64_t b, uint64_t mod) {
  assert((a < mod && b < mod) && "input should be less than mod!!!");
  uint64_t sum = a + b;
  if ((sum < a) || (sum >= mod)) {
    sum -= mod;
  }
  return sum;
}

//! @brief compute (a-b)%mod.
static inline uint64_t Modsub(uint64_t a, uint64_t b, uint64_t mod) {
  assert((a < mod && b < mod) && "input should be less than mod!!!");
  uint64_t diff = a - b;
  if (diff > a) {
    diff += mod;
  }
  return diff;
}

//! @brief compute (a*b)%mod.
static inline uint64_t Modmul(const uint64_t a, const uint64_t b,
                              const uint64_t mod) {
  if (a == 0 || b == 0) return 0;
  return ((U128)a * (U128)b) % mod;
}

//! @brief compute (basenum^order)%mod.
static inline uint64_t Modpow(uint64_t basenum, uint64_t order, uint64_t mod) {
  assert(basenum < mod && "input should be less than mod!!!");
  uint64_t basepow = basenum;
  uint64_t ret     = 1;
  while (order != 0) {
    if (order & 1) {
      ret = Modmul(ret, basepow, mod);
    }
    order >>= 1;
    basepow = Modmul(basepow, basepow, mod);
  }
  return ret;
}

//! @brief get the inverse of a in modulus mod.
static inline uint64_t Modinv(uint64_t a, uint64_t mod) {
  assert(a < mod && "input should be less than mod!!!");
  assert(a != 0 && "0 can't inv!!!");
  return Modpow(a, mod - 2, mod);
}

//! @brief get 1+Log2_comp(a>>1), used for computing the number of lines of NTT
//! twiddle factors.
static inline uint64_t Log2_comp(uint64_t a) {
  if (a == 1) {
    return 0;
  } else {
    return 1 + Log2_comp(a >> 1);
  }
}

//! @brief compute the Bit_reverse of a number.  eg. Bit_rev(10,4)=5, but
//! Bit_rev(10,3) is not allowed.
//! @param index operand num.
//! @param blen bitlen.(Assuming that index<2^blen.)
//! @return Bit_reverse of index.
static inline uint64_t Bit_rev(uint64_t index, uint64_t blen) {
  uint64_t tmp1 =
      ((index & 0xaaaaaaaaaaaaaaaa) >> 1) | ((index & 0x5555555555555555) << 1);
  uint64_t tmp2 =
      ((tmp1 & 0xcccccccccccccccc) >> 2) | ((tmp1 & 0x3333333333333333) << 2);
  uint64_t tmp3 =
      ((tmp2 & 0xf0f0f0f0f0f0f0f0) >> 4) | ((tmp2 & 0x0f0f0f0f0f0f0f0f) << 4);
  uint64_t tmp4 =
      ((tmp3 & 0xff00ff00ff00ff00) >> 8) | ((tmp3 & 0x00ff00ff00ff00ff) << 8);
  uint64_t tmp5 =
      ((tmp4 & 0xffff0000ffff0000) >> 16) | ((tmp4 & 0x0000ffff0000ffff) << 16);
  uint64_t tmp6 = (tmp5 >> 32) | (tmp5 << 32);
  return tmp6 >> (64 - blen);
}

//! @brief find the 2n-th root of p.
//! @param n expected N.
//! @param p the prime.
//! @return the special root.(which is the first root.)
static inline uint64_t Find_2nth_root(uint64_t n, uint64_t p) {
  uint64_t dn = n << 1;
  assert(p % dn == 1);
  uint64_t h = (p - 1) / dn;
  uint64_t g = 2;
  uint64_t r;
  while (1) {
    r             = Modpow(g, h, p);
    uint64_t flag = Modpow(r, n, p);
    if (flag == p - 1) {
      return r;
    }
    g++;
  }
}

//! @brief function for getting index of twiddle factor in NTT.
//! @param logbase the current ntt stage.
//! @param index the index of data.
//! @param log_n the total num of ntt stages.
static inline uint64_t Get_w_ind(uint64_t logbase, uint64_t index,
                                 uint64_t log_n) {
  if (log_n < logbase) {
    return (uint64_t)(-1);
  }
  uint64_t ret = index << (log_n - logbase);
  return ret;
}

}  // namespace util
}  // namespace fhe
#endif  // FHE_UTIL_NTT_MATH_H