//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_POLY_EVAL_H
#define RTLIB_INCLUDE_POLY_EVAL_H

#include "context/ckks_context.h"
#include "lpoly/poly.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief A polynomial in the ring R_a
//! Here, R is the quotient ring Z[x]/f(x), where f(x) = x^d + 1.
//! The polynomial keeps track of the ring degree d, the coefficient
//! modulus a, and the coefficients in an array.
typedef struct {
  uint32_t _ring_degree;     // degree d of polynomial that determines the
                             // quotient ring R
  size_t _num_alloc_primes;  // number of allocated number of primes
                             // including q and p
  size_t   _num_primes;      // number of q primes
  size_t   _num_primes_p;    // number of p primes
  bool     _is_ntt;          // polynomial is ntt or intt
  int64_t* _data;            // array of coefficients in polynomial
} POLYNOMIAL;

typedef POLYNOMIAL* POLY;
typedef POLYNOMIAL* POLY_ARR;

//! @brief Get ring degree from polynomial
static inline uint32_t Get_rdgree(POLY poly) { return poly->_ring_degree; }

//! @brief Get level of poly
static inline size_t Poly_level(POLY poly) { return poly->_num_primes; }

//! @brief Get number of p primes from polynomial
static inline size_t Num_p(POLY poly) { return poly->_num_primes_p; }

//! @brief Get number of p and q primes
static inline size_t Get_num_pq(POLY poly) {
  return poly->_num_primes_p + poly->_num_primes;
}

//! @brief Get number of allocated number of primes, include P & Q
static inline size_t Num_alloc(POLY poly) { return poly->_num_alloc_primes; }

//! @brief Get length of allocated number of primes
static inline size_t Get_poly_alloc_len(POLY poly) {
  return Get_rdgree(poly) * Num_alloc(poly);
}

//! @brief Get memory size of allocated number of primes
static inline size_t Get_poly_mem_size(POLY poly) {
  return Get_poly_alloc_len(poly) * sizeof(int64_t);
}

//! @brief Get starting address of data of polynomial
static inline int64_t* Get_poly_coeffs(POLY poly) { return poly->_data; }

//! @brief Set level of polynomial
static inline void Set_poly_level(POLY res, size_t level) {
  FMT_ASSERT(Num_alloc(res) >= level + Num_p(res),
             "raised level exceed alloc_size");
  res->_num_primes = level;
}

//! @brief Set new level of polynomial, and also saved the original level
static inline size_t Save_poly_level(POLY res, size_t level) {
  size_t old_level = Poly_level(res);
  Set_poly_level(res, level);
  return old_level;
}

//! @brief Set level of polynomial back to original level
static inline void Restore_poly_level(POLY res, size_t level) {
  Set_poly_level(res, level);
}

//! @brief Check if polynomial is NTT or not
static inline bool Is_ntt(POLY poly) { return poly->_is_ntt; }

//! @brief Set polynomial is ntt or not
static inline void Set_is_ntt(POLY poly, bool v) { poly->_is_ntt = v; }

//! @brief Check if the size of two given polynomial match
static inline bool Is_size_match(POLY poly1, POLY poly2) {
  if (poly1 == NULL || poly2 == NULL) {
    return FALSE;
  }
  if (Get_rdgree(poly1) == Get_rdgree(poly2) &&
      Poly_level(poly1) == Poly_level(poly2)) {
    return TRUE;
  }
  return FALSE;
}

//! @brief Alloc polynomial from given parameters
//! @param poly return polynomial
//! @param ring_degree ring degree of polynomial
//! @param num_primes number of q primes
//! @param num_primes_p number of p primes
static inline void Alloc_poly_data(POLY poly, uint32_t ring_degree,
                                   size_t num_primes, size_t num_primes_p) {
  poly->_ring_degree      = ring_degree;
  poly->_num_primes       = num_primes;
  poly->_num_primes_p     = num_primes_p;
  poly->_num_alloc_primes = num_primes + num_primes_p;
  size_t alloc_size = sizeof(int64_t) * poly->_num_alloc_primes * ring_degree;
  poly->_data       = (int64_t*)malloc(alloc_size);
  poly->_is_ntt     = FALSE;
  memset(poly->_data, 0, alloc_size);
}

//! @brief Initialize polynomial with given parameters
//! @param poly polynomial to be initialized
//! @param ring_degree ring degree of polynomial
//! @param num_primes number of q primes
//! @param num_primes_p number of p primes
//! @param data init value
static inline void Init_poly_data(POLYNOMIAL* poly, uint32_t ring_degree,
                                  size_t num_primes, size_t num_primes_p,
                                  int64_t* data) {
  poly->_ring_degree  = ring_degree;
  poly->_num_primes   = num_primes;
  poly->_num_primes_p = num_primes_p;
  poly->_data         = data;
  if (poly->_num_alloc_primes == 0)
    poly->_num_alloc_primes = num_primes + num_primes_p;
  poly->_is_ntt = FALSE;
}

//! @brief Cleanup data of polynomial
void Free_poly_data(POLY poly);

//! @brief Alloc polynomial
//! @param degree poly degree
//! @param num_q number of Q primes
//! @param num_p numbe of P primes
//! @return the pointer to the allocated polynomial memory
static inline POLY Alloc_poly(uint32_t degree, size_t num_q, size_t num_p) {
  FMT_ASSERT(num_q > 0, "Alloc_poly: q primes should not be NULL");
  POLY poly = (POLY)malloc(sizeof(POLYNOMIAL));
  Alloc_poly_data(poly, degree, num_q, num_p);
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(poly, TRUE);
  return poly;
}

//! @brief Initialize poly by given poly size, do not copy contents
//! @param res polynomial to be initialized
//! @param poly given polynomial
void Init_poly(POLY res, POLY poly);

//! @brief Cleanup polynomial
static inline void Free_poly(POLY poly) {
  Free_poly_data(poly);
  free(poly);
  poly = NULL;
}

//! @brief Alloc polynomial array, the polynomial data is only allocate
//! once to reduce the malloc time, the memory should be freed by Free_polys
//! @param cnt array element count
//! @param degree poly degree
//! @param num_q number of Q primes
//! @param num_p numbe of P primes
//! @return the pointer to the allocated polynomial array
POLY_ARR Alloc_polys(uint32_t cnt, uint32_t degree, size_t num_q, size_t num_p);

//! @brief Free polynomial array
//! @param polys polynomial array, polynomial element data is continus
//! pointer array: |polys[0]|polys[1]|....|polys[n]|
//!                    |       |             |
//! data array:    |data[0] |data[1] |... |data[n] | --> alloc once
static inline void Free_polys(POLY_ARR polys) {
  FMT_ASSERT(polys, "null polynomial array");
  POLY     poly_head = &polys[0];
  int64_t* poly_data = Get_poly_coeffs(poly_head);
  if (poly_data) free(poly_data);
  free(poly_head);
}

//! @brief Copy polynomial
void Copy_poly(POLY res, POLY poly);

//! @brief Initialize res polynomial by size
//! @param res result polynomial
//! @param num_q number of q primes
//! @param is_ext specifiy whether result contains P coeffcients
static inline POLY Init_poly_by_size(POLY res, size_t num_q, bool is_ext) {
  FMT_ASSERT(num_q > 0, "Alloc_poly: q primes should not be NULL");
  // free old result data if exists
  if (Num_alloc(res) != 0) {
    Free_poly_data(res);
  }
  Alloc_poly_data(res, Degree(), num_q,
                  is_ext ? Get_crt_num_p(Get_crt_context()) : 0);
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(res, TRUE);

  return res;
}

//! @brief Initialize res polynomial by operand
//! @param res result polynomial
//! @param opnd1 the first operand
//! @param opnd2 the second operand
//! @param is_ext specifiy whether result contains P coeffcients
static inline POLY Init_poly_by_opnd(POLY res, POLY opnd1, POLY opnd2,
                                     bool is_ext) {
  CRT_CONTEXT* crt    = Get_crt_context();
  POLY         picked = NULL;
  if (opnd1 == NULL || Poly_level(opnd1) == 0)
    picked = opnd2;
  else if (opnd2 == NULL || Poly_level(opnd2) == 0)
    picked = opnd1;
  else {
    // pick smaller level poly
    picked = Poly_level(opnd1) <= Poly_level(opnd2) ? opnd1 : opnd2;
  }
  // if opnd is zero ciph, just return
  if (picked == NULL) return res;
  if (Get_poly_coeffs(res) != NULL && Poly_level(res) >= Poly_level(picked)) {
    FMT_ASSERT(Poly_level(res) >= Poly_level(picked) &&
                   Get_rdgree(res) == Get_rdgree(picked),
               "unmatched size");
    Set_poly_level(res, Poly_level(picked));
    if (is_ext) {
      FMT_ASSERT(Num_p(res) == Get_crt_num_p(crt), "invalid p number");
    }
  } else {
    if (Get_poly_coeffs(res) != NULL) Free_poly_data(res);
    uint32_t ring_degree = Get_rdgree(picked);
    size_t   num_q       = Poly_level(picked);
    Alloc_poly_data(res, ring_degree, num_q, is_ext ? Get_crt_num_p(crt) : 0);
  }
  Set_is_ntt(res, true);
  return res;
}

//! @brief Get coefficients from polynomial
//! @param poly polynomial
//! @param level current level
//! @param degree poly degree of polynomial
//! @return int64_t*
static inline int64_t* Coeffs(POLY poly, size_t level, uint32_t degree) {
  assert(level <= Get_num_pq(poly) && "index overflow");
  return Get_poly_coeffs(poly) + level * degree;
}

//! @brief Set coefficients for destination polynomial
//! @param dst destination polynomial
//! @param level current level
//! @param degree poly degree of polynomial
//! @param src input coefficients
static inline void Set_coeffs(POLY dst, uint32_t level, uint32_t degree,
                              int64_t* src) {
  assert(level <= Get_num_pq(dst) && "index overflow");
  int64_t* dst_coeffs = Coeffs(dst, level, degree);
  memcpy(dst_coeffs, src, sizeof(int64_t) * degree);
}

//! @brief Set data for the given idx of polynomial
//! @param val input data
//! @param idx index of given data
static inline void Set_coeff_at(POLY poly, int64_t val, uint32_t idx) {
  FMT_ASSERT(poly, "null poly");
  FMT_ASSERT(idx < Get_poly_alloc_len(poly), "idx outof bound");
  poly->_data[idx] = val;
}

//! @brief Get length of decomposed poly
//! @param poly input poly
//! @return size_t
static inline size_t Num_decomp(POLY poly) {
  return Get_num_decomp(Get_crt_context(), Poly_level(poly));
}

//! @brief Digit decompose of given part
//! @param res result poly
//! @param poly input poly
//! @param q_part_idx index of q part
POLY Decomp(POLY res, POLY poly, uint32_t q_part_idx);

//! @brief Raise poly from part Q base to P*partQ base
//! @param res result poly
//! @param poly input poly
//! @param q_part_idx index of q part
POLY Mod_up(POLY res, POLY poly, uint32_t q_part_idx);

//! @brief Decompose and raise at given part index
//! @param res result poly
//! @param poly input poly
//! @param q_part_idx index of q part
POLY Decomp_modup(POLY res, POLY poly, uint32_t q_part_idx);

//! @brief Reduce poly from P*Q to Q
//! @param res result poly
//! @param poly input poly
POLY Mod_down(POLY res, POLY poly);

//! @brief Rescale poly to res
POLY Rescale(POLY res, POLY poly);

//! @brief Modswitch poly to res
POLY Modswitch(POLY res, POLY poly);

//! @brief Precompute for key switch (Decompose and Modup)
//! @param res decomposed and raised polynomial array
//! @param size the size of polynomial array
//! @param input polynomial
//! @return decomposed and raised polynomial array
POLY_ARR Precomp(POLY_ARR res, size_t size, POLY input);

//! @brief Allocate and precompute for key switch
//! @return new memory is returned, call Free_precomp() to free
VALUE_LIST* Alloc_precomp(POLY input);

//! @brief Cleanup VALUE_LIST for precomputation switch key
void Free_precomp(VALUE_LIST* precompute);

//! @brief Dot product for key switch
//! @param res result polynomial
//! @param p0 polynomial array (precomputed result)
//! @param p1 polynomial array (keys)
//! @param size the size of polynomial array
POLY Dot_prod(POLY res, POLY_ARR p0, POLY_ARR p1, uint32_t size);

//! @brief Extend polynomial poly to its extend format by multiplying Pmodq
//! @param res extended polynomial
//! @param poly input polynomial
POLY Extend(POLY res, POLY poly);

//! @brief Perform Fused operation: Mod_down->Rescale
//! @param res result poly
//! @param poly input extended poly
POLY Mod_down_rescale(POLY res, POLY poly);

//! @brief Convert polynomial from coefficient form to NTT
void Conv_poly2ntt_inplace(POLY poly);
void Conv_poly2ntt(POLY res, POLY poly);

//! @brief Convert polynomial from NTT to coefficient form
void Conv_ntt2poly_inplace(POLY poly);

//! @brief Derive coeffcients from poly with q_cnt & p_cnt
void Derive_poly(POLY res, POLY poly, size_t q_cnt, size_t p_cnt);

//! @brief Add polynomial by a scalar vector.
//! @param scalars scalar list to be added to the current polynomial.
POLY Scalars_integer_add_poly(POLY res, POLY poly, VALUE_LIST* scalars);

//! @brief Multiplies polynomial by a scalar.
//! @param res the product of the polynomial mul the scalar
//! @param poly given polynomial
//! @param scalar Scalar to be multiplied to the current polynomial.
POLY Scalar_integer_multiply_poly(POLY res, POLY poly, int64_t scalar);

//! @brief Multiplies polynomial by a scalar vector.
//! @param scalars scalar list to be multiplied to the current polynomial.
POLY Scalars_integer_multiply_poly(POLY res, POLY poly, VALUE_LIST* scalars);

//! @brief Multiplies polynomial by a scalar vector with qpart
//! @param res the product of the polynomial mul
//! @param poly given polynomial
//! @param scalars scalar list to be multiplied to the current polynomial.
//! @param part_idx index of qPart
POLY Scalars_integer_multiply_poly_qpart(POLY res, POLY poly,
                                         VALUE_LIST* scalars, size_t part_idx);

//! @brief Adds the current polynomial to poly inside the ring R_a.
//! @param sum POLYNOMIAL which is the sum of the two
//! @param poly1 POLYNOMIAL to be added
//! @param poly2 POLYNOMIAL to be added
POLY Add_poly(POLY sum, POLY poly1, POLY poly2);

//! @brief Subtracts second polynomial from first polynomial in the ring.
//! @param poly_diff poly which is the difference between the two polys
//! @param poly1 POLYNOMIAL to be subtracted
//! @param poly2 POLYNOMIAL to be subtracted
POLY Sub_poly(POLY poly_diff, POLY poly1, POLY poly2);

//! @brief Multiplies two polynomials in the ring using NTT.
//! Multiplies the current polynomial to poly inside the ring R_a
//! using the Number Theoretic Transform (NTT) in O(nlogn).
//! @param res a POLYNOMIAL which is the product of the two polynomials
//! @param poly1 input poly to be multiply
//! @param poly2 input poly to be multiply
POLY Mul_poly(POLY res, POLY poly1, POLY poly2);

//! @brief Perform polynomial multiply with precomputation
//! @param res result poly
//! @param poly1 multipler polynomial
//! @param poly2  multipler polynomial
//! @param poly2_prec poly2 precomputed polynomial
POLY Mul_poly_fast(POLY res, POLY poly1, POLY poly2, POLY poly2_prec);

//! @brief Transformation for automorphism
//! @param res the product of the automorphism
//! @param poly given polynomial
//! @param precomp precomputed from Precompute_automorphism_order
POLY Automorphism_transform(POLY res, POLY poly, VALUE_LIST* precomp);

//! @brief Transform values(level 0 of poly) to rns polynomial
void Transform_values_from_level0(POLY res, POLY poly);

//! @brief Transform regular value to rns polynomial
//! @param poly rns polynomial
//! @param value input original value
//! @param without_mod with modulus or not
void Transform_value_to_rns_poly(POLY poly, VALUE_LIST* value,
                                 bool without_mod);

//! @brief Reconstructs from rns polynomial to the regular value
//! @param res a value list whose values are reconstructed.
//! @param poly rns polynomial
void Reconstruct_rns_poly_to_value(VALUE_LIST* res, POLY poly);

//! @brief Sample for polynomial coeffcient with uniform distribution
//! @param poly sampled polynomial
void Sample_uniform_poly(POLY poly);

//! @brief Sample for polynomial coeffcient with ternary distribution
//! @param poly sampled polynomial
//! @param hamming_weight Hamming weight for sparse ternary distribution,
//! for uniform distribution the value is zero
void Sample_ternary_poly(POLY poly, size_t hamming_weight);

//! @brief Lightweight printing for polynomial with limited coefficents and
//! levels
//! @param fp file pointer
//! @param input polynomial to be printed
void Print_poly_lite(FILE* fp, POLY input);

//! @brief Print raw data of polynomial
//! @param fp output
//! @param poly given polynomial
void Print_poly_rawdata(FILE* fp, POLY poly);

//! @brief Print polynomial
//! @param fp output
//! @param poly given polynomial
void Print_poly(FILE* fp, POLY poly);

// APIs for lpoly2c
//! @brief Get lpoly from rns_poly at given idx
//! need free data by Free_lpoly_data().
L_POLY Lpoly_from_poly(POLY poly, size_t idx);

//! @brief Get lpoly from array of rns_poly at given idx
//! need free data by Free_lpoly_data().
L_POLY Lpoly_from_poly_arr(size_t size, POLY_ARR poly_arr, size_t idx);

//! @brief Set poly data with given lpoly & idx
void Set_poly_data(POLY poly, L_POLY lpoly, size_t idx);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_POLY_EVAL_H
