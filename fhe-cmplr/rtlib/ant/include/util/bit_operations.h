//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_BIT_OPERATIONS_H
#define RTLIB_INCLUDE_BIT_OPERATIONS_H

#include <math.h>
#include <stdlib.h>

#include "util/fhe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reverses bits of an integer.
 * Reverse bits of the given value with a specified bit width.
 * For example, reversing the value 6 = 0b110 with a width of 5
 * would result in reversing 0b00110, which becomes 0b01100 = 12.
 *
 * @param value Value to be reversed
 * @param width Number of bits to consider in reversal
 * @return The reversed int value of the input
 */
int64_t Reverse_bits(int64_t value, size_t width);

/**
 * @brief Reverses list by reversing the bits of the indices.
 * Reverse indices of the given list.
 * For example, reversing the list [0, 1, 2, 3, 4, 5, 6, 7] would become
 * [0, 4, 2, 6, 1, 5, 3, 7], since 1 = 0b001 reversed is 0b100 = 4,
 * 3 = 0b011 reversed is 0b110 = 6.
 *
 * @param result The reversed list based on indices
 * @param values List of values to be reversed
 * @param len Length of values, Length of list must be a power of two
 */
void Bit_reverse_vec(int64_t* result, int64_t* values, size_t len);

/**
 * @brief Reverses list by reversing the bits of the indices for complex
 *
 * @param result The reversed list based on indices
 * @param values List of values to be reversed
 * @param len Length of values, Length of list must be a power of two
 */
void Bit_reverse_vec_for_complex(DCMPLX* result, DCMPLX* values, size_t len);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_BIT_OPERATIONS_H