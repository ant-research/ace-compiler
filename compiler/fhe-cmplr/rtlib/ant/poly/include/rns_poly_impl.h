//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_POLY_INCLUDE_RNS_POLY_IMPL_H
#define RTLIB_ANT_POLY_INCLUDE_RNS_POLY_IMPL_H

#include "poly/rns_poly.h"
#include "util/crt.h"
#include "util/modular.h"
#include "util/ntt.h"
#include "util/number_theory.h"
#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

// A module to handle polynomial arithmetic in the quotient ring Z_a[x]/f(x).

#define FOR_ALL_COEFF(poly, idx) \
  for (size_t idx = 0; idx < Get_poly_alloc_len(poly); idx++)

//! @brief Get length of polynomial data
static inline size_t Get_poly_len(POLYNOMIAL* poly) {
  return Get_rdgree(poly) * Get_num_pq(poly);
}

//! @brief Get the starting address of coefficients at given level index(idx)
//! @param idx the level index
//! @return coefficients address
static inline int64_t* Get_poly_coeffs_at(POLYNOMIAL* poly, uint32_t idx) {
  IS_TRUE(poly, "null poly");
  IS_TRUE(idx < Num_alloc(poly), "idx outof bound");
  return poly->_data + idx * Get_rdgree(poly);
}

//! @brief Get starting address of p data of polynomial
static inline int64_t* Get_p_coeffs(POLYNOMIAL* poly) {
  return poly->_data + (Num_alloc(poly) - Num_p(poly)) * Get_rdgree(poly);
}

//! @brief Get data at given idx of polynomial
//! @param idx given index
static inline int64_t Get_coeff_at(POLYNOMIAL* poly, uint32_t idx) {
  FMT_ASSERT(poly, "null poly");
  FMT_ASSERT(idx < Get_poly_alloc_len(poly), "idx outof bound");
  return poly->_data[idx];
}

//! @brief Set number of p of polynomial
static inline void Set_num_p(POLYNOMIAL* res, size_t num_p) {
  FMT_ASSERT(Num_alloc(res) >= num_p + Poly_level(res),
             "raised level exceed alloc_size");
  res->_num_primes_p = num_p;
}

//! @brief Print poly list
void Print_poly_list(FILE* fp, VALUE_LIST* poly_list);

//! @brief Check if two given polynomial ntt format match
static inline bool Is_ntt_match(POLYNOMIAL* poly1, POLYNOMIAL* poly2) {
  return (Is_ntt(poly1) == Is_ntt(poly2));
}

//! @brief Check if p primes cnt of two given polynomial match
static inline bool Is_p_cnt_match(POLYNOMIAL* poly1, POLYNOMIAL* poly2) {
  return (Num_p(poly1) == Num_p(poly2));
}

//! @brief Copy low level polynomials with level within [0, dest.level]
//! from src to dest
static inline void Copy_low_level_polynomial(POLYNOMIAL* dest,
                                             POLYNOMIAL* src) {
  FMT_ASSERT(Get_rdgree(dest) == Get_rdgree(src), "unmatched poly degree");
  FMT_ASSERT(Poly_level(src) >= Poly_level(dest), "level out of range");
  FMT_ASSERT(Num_p(dest) >= Num_p(src), "unmatched p primes cnt");

  // copy q coeffs
  memcpy(Get_poly_coeffs(dest), Get_poly_coeffs(src),
         sizeof(int64_t) * Poly_level(dest) * Get_rdgree(dest));
  // copy p coeffs only if src has p coeffs
  if (Num_p(src)) {
    memcpy(Get_p_coeffs(dest), Get_p_coeffs(src),
           sizeof(int64_t) * Num_p(dest) * Get_rdgree(dest));
  }
  Set_is_ntt(dest, Is_ntt(src));
}

//! @brief Extract coeffcients from poly between level range
//! [start_ofst, start_ofst + prime_cnt)
void Extract_poly(POLYNOMIAL* res, POLYNOMIAL* poly, size_t start_ofst,
                  size_t prime_cnt);

// Print polynomial

//! @brief Print polynomial with detail
//! @param fp output
//! @param poly given polynomial
//! @param primes convert ntt to poly with given prime
//! @param detail print detail or not
void Print_poly_detail(FILE* fp, POLYNOMIAL* poly, VL_CRTPRIME* primes,
                       bool detail);

//! @brief Print polynomial after reconstruct to bigint value
//! @param poly given polynomial
void Print_rns_poly(FILE* fp, POLYNOMIAL* poly);

//! @brief Convert polynomial from coefficient form to NTT
void Conv_poly2ntt_inplace_with_primes(POLYNOMIAL* poly, VL_CRTPRIME* primes);
void Conv_poly2ntt_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               VL_CRTPRIME* primes);

//! @brief Convert polynomial from NTT to coefficient form
void Conv_ntt2poly(POLYNOMIAL* res, POLYNOMIAL* poly);
void Conv_ntt2poly_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               VL_CRTPRIME* primes);
void Conv_ntt2poly_inplace_with_primes(POLYNOMIAL* poly, VL_CRTPRIME* primes);

//! @brief Adds two polynomial
//! res = res + (poly1 * poly2)
//! @param res polynomial which is the sum of the two
//! @param poly1 input polynomial
//! @param poly2 input polynomial
void Multiply_add(POLYNOMIAL* res, POLYNOMIAL* poly1, POLYNOMIAL* poly2);

//! @brief Perform dot product of two polynomial arrays
//! @param res result polynomial
//! @param p0 input polynomial array
//! @param p1 input polynomial array
//! @param num the size of polynomial array
//! @return dot prodcut result
POLYNOMIAL* Dotprod_poly(POLYNOMIAL* res, POLY_ARR p0, POLY_ARR p1,
                         uint32_t num);

//! @brief Perform a fast varient of dot product of two polynomial arrays by
//! lazy modulus
//! @param res result polynomial
//! @param p0 input polynomial array
//! @param p1 input polynomial array
//! @param num the size of polynomial array
//! @return dot prodcut result
POLYNOMIAL* Fast_dotprod_poly(POLYNOMIAL* res, POLY_ARR p0, POLY_ARR p1,
                              uint32_t size);

//! @brief Rotate poly with rotation index
void Rotate_poly_with_rotation_idx(POLYNOMIAL* res, POLYNOMIAL* poly,
                                   int32_t rotation);

//! @brief Calculate the Polynomial for faster variant of Barrett Reduction
//! @param res barrett polynomial
//! @param poly input polynomial to be precomputed
//! @return barrett polynomial
POLYNOMIAL* Barrett_poly(POLYNOMIAL* res, POLYNOMIAL* poly);

//! @brief Fast base conversion, convert old_poly with RNS-basis old_primes
//  to new_poly with RNS-basis new_primes
//! @param new_poly polynomial after base conversion
//! @param old_poly input polynomial to be converted
//! @param new_primes new RNS basis
//! @param old_primes old RNS basis
void Fast_base_conv(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                    CRT_PRIMES* new_primes, CRT_PRIMES* old_primes);

//! @brief Fast base conversion, convert old_poly with qPart
//  to new_poly with RNS-basis new_primes
void Fast_base_conv_with_parts(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                               CRT_PRIMES* qpart, VL_CRTPRIME* new_primes,
                               size_t part_idx, size_t level);

//! @brief Fused operation: run Modulus Reduction(P*Q->Q), Rescale and Modup
//! operation
//! @param res result polynomial
//! @param poly input polynomial
//! @return polynomial list after fused operation
VALUE_LIST* Reduce_and_rescale_modup(POLYNOMIAL* res, POLYNOMIAL* poly);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_POLY_INCLUDE_RNS_POLY_IMPL_H