//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_HAL_INCLUDE_NTT_MATH_H
#define RTLIB_ANT_HAL_INCLUDE_NTT_MATH_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t     U8;
typedef uint16_t    U16;
typedef uint32_t    U32;
typedef uint64_t    U64;
typedef __uint128_t U128;

typedef U64* V64;
typedef U64* VV64;

//! @brief allocate a contiguous 2d array
static inline VV64 Alloc_poly_2d(U64 row, U64 columns) {
  VV64 poly_2d = (VV64)malloc(row * columns * sizeof(U64));
  assert(poly_2d != NULL);
  return poly_2d;
}

//! @brief free 2d array
static inline void Free_poly_2d(VV64 poly_2d) { free(poly_2d); }

//! @brief access element with row and column indices
static inline U64 Element(VV64 poly_2d, U64 row, U64 column,
                          U64 column_length) {
  return poly_2d[row * column_length + column];
}

//! @brief set element at row & column
static inline void Set_element(VV64 poly_2d, U64 row, U64 column,
                               U64 column_length, U64 value) {
  poly_2d[row * column_length + column] = value;
}

//! @brief copy poly 2d array from src to dst
static inline void Copy_poly_2d(U64* dst, U64* src, U64 row, U64 column) {
  for (U64 i = 0; i < row; i++) {
    for (U64 j = 0; j < column; j++) {
      U64 value = Element(src, i, j, column);
      Set_element(dst, i, j, column, value);
    }
  }
}

//! @brief print poly 2d array
static inline void Print_poly_2d(VV64 poly_2d, U64 row, U64 column,
                                 const char* name) {
  printf("%s[%lu]: ", name, row);
  for (U64 i = 0; i < row; i++) {
    printf("%lu: [ ", column);
    for (U64 j = 0; j < column; j++) {
      printf("%lu ", Element(poly_2d, i, j, column));
    }
    printf("] ");
  }
  printf("\n");
}

//! @brief print poly 2d array
static inline void Print_poly_1d(V64 poly_1d, U64 len, const char* name) {
  printf("%s[%lu]: ", name, len);
  for (U64 i = 0; i < len; i++) {
    printf("%lu ", poly_1d[i]);
  }
  printf("\n");
}

//! @brief compute (a+b)%mod.
static inline U64 Modadd(U64 a, U64 b, U64 mod) {
  assert((a < mod && b < mod) && "input should be less than mod!!!");
  U64 sum = a + b;
  if ((sum < a) || (sum >= mod)) {
    sum -= mod;
  }
  return sum;
}

//! @brief compute (a-b)%mod.
static inline U64 Modsub(U64 a, U64 b, U64 mod) {
  assert((a < mod && b < mod) && "input should be less than mod!!!");
  U64 diff = a - b;
  if (diff > a) {
    diff += mod;
  }
  return diff;
}

//! @brief compute (a*b)%mod.
static inline U64 Modmul(const U64 a, const U64 b, const U64 mod) {
  if (a == 0 || b == 0) return 0;
  return ((U128)a * (U128)b) % mod;
}

//! @brief compute (basenum^order)%mod.
static inline U64 Modpow(U64 basenum, U64 order, U64 mod) {
  assert(basenum < mod && "input should be less than mod!!!");
  U64 basepow = basenum;
  U64 ret     = 1;
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
static inline U64 Modinv(U64 a, U64 mod) {
  assert(a < mod && "input should be less than mod!!!");
  assert(a != 0 && "0 can't inv!!!");
  return Modpow(a, mod - 2, mod);
}

//! @brief get 1+Log2_comp(a>>1), used for computing the number of lines of NTT
//! twiddle factors.
static inline U64 Log2_comp(U64 a) {
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
static inline U64 Bit_rev(U64 index, U64 blen) {
  U64 tmp1 =
      ((index & 0xaaaaaaaaaaaaaaaa) >> 1) | ((index & 0x5555555555555555) << 1);
  U64 tmp2 =
      ((tmp1 & 0xcccccccccccccccc) >> 2) | ((tmp1 & 0x3333333333333333) << 2);
  U64 tmp3 =
      ((tmp2 & 0xf0f0f0f0f0f0f0f0) >> 4) | ((tmp2 & 0x0f0f0f0f0f0f0f0f) << 4);
  U64 tmp4 =
      ((tmp3 & 0xff00ff00ff00ff00) >> 8) | ((tmp3 & 0x00ff00ff00ff00ff) << 8);
  U64 tmp5 =
      ((tmp4 & 0xffff0000ffff0000) >> 16) | ((tmp4 & 0x0000ffff0000ffff) << 16);
  U64 tmp6 = (tmp5 >> 32) | (tmp5 << 32);
  return tmp6 >> (64 - blen);
}

//! @brief polys re-order (Bit_rev).
static inline void Poly_bit_rev(V64 des, V64 src, U64 blen) {
  U64 len = 1 << blen;
  V64 tmp = (V64)malloc(sizeof(U64) * len);
  for (size_t i = 0; i < len; i++) {
    tmp[i] = src[Bit_rev(i, blen)];
  }
  for (size_t i = 0; i < len; i++) {
    des[i] = tmp[i];
  }
  free(tmp);
}

//! @brief find the 2n-th root of p.
//! @param n expected N.
//! @param p the prime.
//! @return the special root.(which is the first root.)
static inline U64 Find_2nth_root(U64 n, U64 p) {
  U64 dn = n << 1;
  assert(p % dn == 1);
  U64 h = (p - 1) / dn;
  U64 g = 2;
  U64 r;
  while (1) {
    r        = Modpow(g, h, p);
    U64 flag = Modpow(r, n, p);
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
static inline U64 Get_w_ind(U64 logbase, U64 index, U64 log_n) {
  if (log_n < logbase) {
    return (U64)(-1);
  }
  U64 ret = index << (log_n - logbase);
  return ret;
}

#ifdef __cplusplus
}
#endif

#endif