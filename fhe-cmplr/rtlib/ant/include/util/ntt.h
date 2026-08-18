//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_NTTRNS_H
#define RTLIB_INCLUDE_NTTRNS_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util/fhe_types.h"
#include "util/fhe_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An instance of Number/Fermat Theoretic Transform parameters.
 * Here, R is the quotient ring Z_a[x]/f(x), where f(x) = x^d + 1.
 * The NTT_CONTEXT keeps track of the ring degree d, the coefficient
 * modulus a, a root of unity w so that w^2d = 1 (mod a), and
 * precomputations to perform the NTT/FTT and the inverse NTT/FTT.
 *
 */
typedef struct {
  uint32_t    _degree;           // Degree of the polynomial ring.
  size_t      _degree_inv;       // the inverse of degree
  size_t      _degree_inv_prec;  // precompute const of degree inv
  MODULUS*    _coeff_modulus;    // modulus for coefficients of the polynomial
  VALUE_LIST* _rou;              // The ith member of the list is w^i, where w
                                 // is a root of unity
  VALUE_LIST* _rou_inv;          // The ith member of the list is w^(-i),
                                 // where w is a root of unity.
  VALUE_LIST* _rou_prec;         // precomputed const for root_of_unity
  VALUE_LIST* _rou_inv_prec;  // precomputed const for inverse of root_of_unity
  VALUE_LIST* _scaled_rou_inv;  // The ith member of the list is 1/n * w^(-i),
                                // where w is a root of unity.
  VALUE_LIST*
      _reversed_bits;  // The ith member of the list is the bits of i
                       // reversed, used in the iterative implementation of NTT.
} NTT_CONTEXT;

/**
 * @brief malloc ntt context
 *
 * @return NTT_CONTEXT*
 */
NTT_CONTEXT* Alloc_nttcontext();

/**
 * @brief Inits ntt context with a coefficient modulus for the polynomial ring
 * Z[x]/f(x) where f has the given poly_degree.
 *
 * @param ntt ntt context that initializated
 * @param poly_degree Degree of the polynomial ring
 * @param coeff_modulus modulus for coefficients of the polynomial
 */
void Init_nttcontext(NTT_CONTEXT* ntt, uint32_t poly_degree,
                     MODULUS* coeff_modulus);

/**
 * @brief Inits ntt context with given ntt
 *
 * @param res ntt context that initializated
 * @param ntt given ntt context
 */
void Init_nttcontext_from_ntt(NTT_CONTEXT* res, NTT_CONTEXT* ntt);

/**
 * @brief cleanup ntt context memory
 *
 * @param ntt ntt context that will be cleanup
 */
void Free_nttcontext(NTT_CONTEXT* ntt);

/**
 * @brief cleanup memebers of ntt context
 *
 * @param ntt ntt context that will be cleanup
 */
void Free_ntt_members(NTT_CONTEXT* ntt);

/**
 * @brief performs precomputations for the NTT and inverse NTT.
 *
 * @param ntt ntt context that precomputed
 * @param root_of_unity root of unity to perform the NTT with
 */
void Precompute_ntt(NTT_CONTEXT* ntt, int64_t root_of_unity);

/**
 * @brief runs NTT on the given coefficients
 * runs iterated NTT with the given coefficients and roots of unity. See
 * paper for pseudocode.
 *
 * @param res List of transformed coefficients
 * @param ntt NTT_CONTEXT that performed
 * @param coeffs List of coefficients to transform. Must be the
 * length of the polynomial degree
 * @param rou powers of roots of unity to be used for transformation.
 * For inverse NTT, this is the powers of the inverse root of unity
 */
void Run_ntt(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs,
             VALUE_LIST* rou);

/**
 * @brief runs forward FTT on the given coefficients
 * runs forward FTT with the given coefficients and parameters in the context
 *
 * @param res list of transformed coefficients
 * @param ntt NTT_CONTEXT that performed
 * @param coeffs List of coefficients to transform. Must be the
 * length of the polynomial degree
 */
void Ftt_fwd(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs);

/**
 * @brief runs inverse FTT on the given coefficients
 *
 * @param res List of inversely transformed coefficients
 * @param ntt NTT_CONTEXT that performed
 * @param coeffs List of coefficients to transform. Must be the
 * length of the polynomial degree.
 */
void Ftt_inv(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs);

/**
 * @brief The FFT_CONTEXT keeps track of the length of the vector and
 * precomputations to perform FFT.
 *
 */
typedef struct {
  size_t      _fft_length;
  VALUE_LIST* _rou;           /* DCMPLX */
  VALUE_LIST* _rou_inv;       /* DCMPLX */
  VALUE_LIST* _rot_group;     /* int64_t */
  VALUE_LIST* _reversed_bits; /* int64_t */
} FFT_CONTEXT;

/**
 * @brief Inits FFT_CONTEXT with a length for the FFT vector.
 *
 * @param fft_length Length of the FFT vector.
 * @return FFT_CONTEXT* fft that be initialized
 */
FFT_CONTEXT* Alloc_fftcontext(size_t fft_length);

/**
 * @brief cleanup FFT_CONTEXT memory
 *
 * @param fft fft that be cleanuped
 */
void Free_fftcontext(FFT_CONTEXT* fft);

/**
 * @brief Performs precomputations for the FFT.
 * Precomputes all powers of roots of unity for the FFT and powers of inverse
 * roots of unity for the inverse FFT.
 *
 * @param fft fft that be precomputec
 */
void Precompute_fft(FFT_CONTEXT* fft);

/**
 * @brief Runs iterated FFT with the given coefficients and roots of unity. See
 * paper for pseudocode.
 *
 * @param res List of transformed coefficients.
 * @param fft FFT_CONTEXT that performed.
 * @param coneffs List of coefficients to transform. Must be the
 * length of the polynomial degree.
 * @param rou Powers of roots of unity to be used for transformation.
 * For inverse NTT, this is the powers of the inverse root of unity.
 */
void Run_fft(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coneffs,
             VALUE_LIST* rou);

/**
 * @brief Runs forward FFT with the given values and parameters in the context.
 *
 * @param res List of transformed coefficients.
 * @param fft FFT_CONTEXT that performed.
 * @param coeffs List of complex numbers to transform.
 */
void Fft_fwd(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs);

/**
 * @brief Runs inverse FFT with the given values and parameters in the context.
 *
 * @param res List of transformed coefficients.
 * @param fft FFT_CONTEXT that performed.
 * @param coeffs List of complex numbers to transform.
 */
void Fft_inv(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs);

/**
 * @brief Checks that the length of the input vector to embedding is the correct
 * size. Throws an error if the length of the input vector to embedding is not
 * 1/4 the size of the FFT vector.
 *
 * @param fft FFT_CONTEXT that performed.
 * @param values input vector
 */
void Check_embedding_input(FFT_CONTEXT* fft, VALUE_LIST* values);

/**
 * @brief Computes a variant of the canonical embedding on the given
 * coefficients. Computes the canonical embedding which consists of evaluating a
 * given polynomial at roots of unity that are indexed 1 (mod 4), w, w^5, w^9,
 * ... The evaluations are returned in the order: w, w^5, w^(5^2), ...
 *
 * @param res List of transformed coefficients.
 * @param fft FFT_CONTEXT that performed.
 * @param coeffs List of complex numbers to transform.
 */
void Embedding(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs);

/**
 * @brief Computes the inverse variant of the canonical embedding.
 *
 * @param res List of transformed coefficients.
 * @param fft FFT_CONTEXT that performed.
 * @param coeffs List of complex numbers to transform.
 */
void Embedding_inv(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs);

/**
 * @brief print ntt context
 *
 * @param fp
 * @param ntt
 */
void Print_ntt(FILE* fp, NTT_CONTEXT* ntt);

/**
 * @brief print fft context
 *
 * @param fp
 * @param fft
 */
void Print_fft(FILE* fp, FFT_CONTEXT* fft);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_NTTRNS_H
