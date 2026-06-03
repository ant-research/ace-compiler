//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_BIT_OPERATION_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_BIT_OPERATION_IMPL_H

#include <math.h>

#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Reverses bits of an integer.
//! Reverse bits of the given value with a specified bit width.
//! For example, reversing the value 6 = 0b110 with a width of 5
//! would result in reversing 0b00110, which becomes 0b01100 = 12.
//! @param value Value to be reversed
//! @param width Number of bits to consider in reversal
//! @return the reversed int value of the input
static inline int64_t Reverse_bits(int64_t value, size_t width) {
  int64_t result = (-1 << width) & value;
  for (long i = width - 1; i >= 0; i--) {
    result |= (value & 1) << i;
    value >>= 1;
  }
  return result;
}

//! @brief Reverses list by reversing the bits of the indices.
//! Reverse indices of the given list.
//! For example, reversing the list [0, 1, 2, 3, 4, 5, 6, 7] would become
//! [0, 4, 2, 6, 1, 5, 3, 7], since 1 = 0b001 reversed is 0b100 = 4,
//! 3 = 0b011 reversed is 0b110 = 6.
//! @param result the reversed list based on indices
//! @param values list of values to be reversed
//! @param len length of values, Length of list must be a power of two
static inline void Bit_reverse_vec(int64_t* result, int64_t* values,
                                   size_t len) {
  size_t width = (size_t)log2(len);
  for (size_t i = 0; i < len; i++) {
    result[i] = values[Reverse_bits(i, width)];
  }
}

//! @brief Reverses list by reversing the bits of the indices for complex
//! @param result the reversed list based on indices
//! @param values list of values to be reversed
//! @param len length of values, Length of list must be a power of two
static inline void Bit_reverse_vec_for_complex(DCMPLX* result, DCMPLX* values,
                                               size_t len) {
  size_t width = (size_t)log2(len);
  for (int i = 0; i < len; i++) {
    result[i] = values[Reverse_bits(i, width)];
  }
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_UTIL_INCLUDE_BIT_OPERATION_IMPL_H