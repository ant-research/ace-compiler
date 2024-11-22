//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/bit_operations.h"

#include <math.h>

int64_t Reverse_bits(int64_t value, size_t width) {
  int64_t result = (-1 << width) & value;
  for (long i = width - 1; i >= 0; i--) {
    result |= (value & 1) << i;
    value >>= 1;
  }
  return result;
}

void Bit_reverse_vec(int64_t* result, int64_t* values, size_t len) {
  size_t width = (size_t)log2(len);
  for (size_t i = 0; i < len; i++) {
    result[i] = values[Reverse_bits(i, width)];
  }
}

void Bit_reverse_vec_for_complex(DCMPLX* result, DCMPLX* values, size_t len) {
  size_t width = (size_t)log2(len);
  for (int i = 0; i < len; i++) {
    result[i] = values[Reverse_bits(i, width)];
  }
}