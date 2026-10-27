//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_NTT_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_NTT_IMPL_H

#include <stdint.h>

#include "util/ntt.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief An instance of Number/Fermat Theoretic Transform parameters.
//! Here, R is the quotient ring Z_a[x]/f(x), where f(x) = x^d + 1.
//! The NTT_CONTEXT keeps track of the ring degree d, the coefficient
//! modulus a, a root of unity w so that w^2d = 1 (mod a), and
//! precomputations to perform the NTT/FTT and the inverse NTT/FTT.
struct NTT_CONTEXT {
  uint32_t _degree;           //!< Degree of the polynomial ring.
  size_t   _degree_inv;       //!< the inverse of degree
  size_t   _degree_inv_prec;  //!< precompute const of degree inv
  MODULUS* _coeff_modulus;    //!< modulus for coefficients of the polynomial
  VALUE_LIST*
      _rou;  //!< The ith member of the list is w^i, where w is a root of unity
  VALUE_LIST* _rou_inv;  //!< The ith member of the list is w^(-i), where w is a
                         //!< root of unity.
  VALUE_LIST* _rou_prec;  //!< precomputed const for root_of_unity
  VALUE_LIST*
      _rou_inv_prec;  //!< precomputed const for inverse of root_of_unity
  VALUE_LIST* _scaled_rou_inv;  //!< The ith member of the list is 1/n * w^(-i),
                                //!< where w is a root of unity.
  VALUE_LIST* _reversed_bits;   //!< The ith member of the list is the bits of i
                                //!< reversed, used in the iterative
                                //!< implementation of NTT.
};

//! @brief Malloc ntt context
NTT_CONTEXT* Alloc_nttcontext();

//! @brief Inits ntt context with a coefficient modulus for the polynomial ring
//! Z[x]/f(x) where f has the given poly_degree.
//! @param ntt ntt context that initializated
//! @param poly_degree degree of the polynomial ring
//! @param coeff_modulus modulus for coefficients of the polynomial
void Init_nttcontext(NTT_CONTEXT* ntt, uint32_t poly_degree,
                     MODULUS* coeff_modulus);

//! @brief Cleanup memebers of ntt context
void Free_ntt_members(NTT_CONTEXT* ntt);

//! @brief Cleanup ntt context memory
void Free_nttcontext(NTT_CONTEXT* ntt);

//! @brief Performs precomputations for the NTT and inverse NTT.
//! @param root_of_unity root of unity to perform the NTT with
void Precompute_ntt(NTT_CONTEXT* ntt, int64_t root_of_unity);

//! @brief Print ntt context
void Print_ntt(FILE* fp, NTT_CONTEXT* ntt);

//! @brief The FFT_CONTEXT keeps track of the length of the vector and
//! precomputations to perform FFT.
struct FFT_CONTEXT {
  size_t      _fft_length;
  VALUE_LIST* _rou;           /* DCMPLX */
  VALUE_LIST* _rou_inv;       /* DCMPLX */
  VALUE_LIST* _rot_group;     /* int64_t */
  VALUE_LIST* _reversed_bits; /* int64_t */
};

//! @brief Performs precomputations for the FFT.
//! Precomputes all powers of roots of unity for the FFT and powers of inverse
//! roots of unity for the inverse FFT.
void Precompute_fft(FFT_CONTEXT* fft);

//! @brief Checks that the length of the input vector to embedding is the
//! correct size. Throws an error if the length of the input vector to embedding
//! is not 1/4 the size of the FFT vector.
//! @param values input vector
void Check_embedding_input(FFT_CONTEXT* fft, VALUE_LIST* values);

//! @brief Print fft context
void Print_fft(FILE* fp, FFT_CONTEXT* fft);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_UTIL_INCLUDE_NTT_IMPL_H
