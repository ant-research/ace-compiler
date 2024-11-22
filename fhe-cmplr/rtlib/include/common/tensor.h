//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_TENSOR_H
#define RTLIB_COMMON_TENSOR_H

//! @brief tensor.h
//! define data structures and APIs for tensor operation

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief tensor data structure
typedef struct {
  SHAPE  _shape;   //!< Shape of the tensor
  double _vals[];  //!< Elements of the tensor
} TENSOR;

#define TENSOR_N(T) (T)->_shape._n
#define TENSOR_C(T) (T)->_shape._c
#define TENSOR_H(T) (T)->_shape._h
#define TENSOR_W(T) (T)->_shape._w

#define TENSOR_SIZE(T) TENSOR_N(T) * TENSOR_C(T) * TENSOR_H(T) * TENSOR_W(T)

#define TENSOR_ELEM(T, n, c, h, w) \
  T->_vals[w + TENSOR_W(T) * (h + TENSOR_H(T) * (c + TENSOR_C(T) * n))]

#define FOR_ALL_TENSOR_ELEM(T, n, c, h, w)     \
  for (size_t n = 0; n < TENSOR_N(T); n++)     \
    for (size_t c = 0; c < TENSOR_C(T); c++)   \
      for (size_t h = 0; h < TENSOR_H(T); h++) \
        for (size_t w = 0; w < TENSOR_W(T); w++)

//! Get a row of data from input vector at given row_idx
static inline float* Slice(float* vec, size_t row_idx, size_t col) {
  return vec + row_idx * col;
}

//! @brief allocate tensor (NCHW)
TENSOR* Alloc_tensor(size_t n, size_t c, size_t h, size_t w, const double* val);

//! @brief free tensor
void Free_tensor(TENSOR* tensor);

//! @brief check tensor t1 & t2 match
bool Is_tensor_match(TENSOR* t1, TENSOR* t2);

//! @brief add two tensor
TENSOR* Add_tensor(TENSOR* t1, TENSOR* t2);

//! @brief print tensor
void Print_tensor(FILE* fp, TENSOR* tensor);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_TENSOR_H
