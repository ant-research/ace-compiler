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

#include "util/modular.h"
#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NTT_CONTEXT NTT_CONTEXT;

//! @brief Runs forward FTT on the given coefficients
//! runs forward FTT with the given coefficients and parameters in the context
//! @param res list of transformed coefficients
//! @param ntt NTT_CONTEXT that performed
//! @param coeffs List of coefficients to transform. Must be the
//! length of the polynomial degree
void Ftt_fwd(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs);

//! @brief Runs inverse FTT on the given coefficients
//! @param res List of inversely transformed coefficients
//! @param ntt NTT_CONTEXT that performed
//! @param coeffs List of coefficients to transform. Must be the
//! length of the polynomial degree.
void Ftt_inv(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs);

typedef struct FFT_CONTEXT FFT_CONTEXT;

//! @brief Inits FFT_CONTEXT with a length for the FFT vector.
//! @param fft_length Length of the FFT vector.
//! @return FFT_CONTEXT* fft that be initialized
FFT_CONTEXT* Alloc_fftcontext(size_t fft_length);

//! @brief Cleanup FFT_CONTEXT memory
//! @param fft fft that be cleanuped
void Free_fftcontext(FFT_CONTEXT* fft);

//! @brief Checks that the length of the input vector to embedding is the
//! correct size. Throws an error if the length of the input vector to embedding
//! is not 1/4 the size of the FFT vector.
//! @param fft FFT_CONTEXT that performed.
//! @param values input vector
void Check_embedding_input(FFT_CONTEXT* fft, VALUE_LIST* values);

//! @brief Computes a variant of the canonical embedding on the given
//! coefficients. Computes the canonical embedding which consists of evaluating
//! a given polynomial at roots of unity that are indexed 1 (mod 4), w, w^5,
//! w^9,
//! ... The evaluations are returned in the order: w, w^5, w^(5^2), ...
//! @param res List of transformed coefficients.
//! @param fft FFT_CONTEXT that performed.
//! @param coeffs List of complex numbers to transform.
void Embedding(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs);

//! @brief Computes the inverse variant of the canonical embedding.
//! @param res List of transformed coefficients.
//! @param fft FFT_CONTEXT that performed.
//! @param coeffs List of complex numbers to transform.
void Embedding_inv(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_NTTRNS_H
