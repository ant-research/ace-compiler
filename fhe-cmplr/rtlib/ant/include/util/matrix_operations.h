//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_UTIL_MATRIX_OPERATIONS_H
#define RTLIB_INCLUDE_UTIL_MATRIX_OPERATIONS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fhe_types.h"
#include "fhe_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief MATRIX can hold either int64_t, double, DCMPLX values
typedef struct {
  VALUE_TYPE _type;
  size_t     _rows;
  size_t     _cols;
  union {
    ANY_VAL* _a;  //!< any values
    int64_t* _i;  //!< int64 values
    double*  _d;  //!< double values
    DCMPLX*  _c;  //!< double complex values
  } _vals;
} MATRIX;

#define MATRIX_ROW_ADDR(A, cols, row)  &(A[(row) * (cols)])
#define MATRIX_IDX(cols, row, col)     ((row) * (cols) + (col))
#define MATRIX_ELEM(A, cols, row, col) A[MATRIX_IDX(cols, row, col)]
#define INT64_MATRIX_ELEM(A, row, col) \
  MATRIX_ELEM(A->_vals._i, A->_cols, row, col)
#define DBL_MATRIX_ELEM(A, row, col) \
  MATRIX_ELEM(A->_vals._d, A->_cols, row, col)
#define DCMPLX_MATRIX_ELEM(A, row, col) \
  MATRIX_ELEM(A->_vals._c, A->_cols, row, col)
#define DCMPLX_MATRIX_ROW(A, row) \
  MATRIX_ROW_ADDR(A->_vals._c, MATRIX_COLS(A), row)
#define MATRIX_ROWS(A) A->_rows
#define MATRIX_COLS(A) A->_cols
#define MATRIX_TYPE(A) A->_type
#define FOR_ALL_MAT_ELEM(A, row, col)               \
  for (size_t row = 0; row < MATRIX_ROWS(A); row++) \
    for (size_t col = 0; col < MATRIX_COLS(A); col++)

//! @brief Alloc a matrix with [rows][cols] dimension
MATRIX* Alloc_dcmplx_matrix(size_t rows, size_t cols, DCMPLX* vals);
MATRIX* Alloc_int64_matrix(size_t rows, size_t cols, int64_t* vals);
MATRIX* Alloc_double_matrix(size_t rows, size_t cols, double* vals);

//! @brief Free matrix memory
void Free_matrix(MATRIX* matrix);

//! @brief Returns matrix data memory size
size_t Matrix_mem_size(MATRIX* mat);

//! @brief Check matrix are with same dimension
bool Same_dim(MATRIX* mat0, MATRIX* mat1);

//! @brief Returns ith diagonal of matrix, where i is the diag_index.
//! Returns the ith diagonal (A_0i, A_1(i+1), ..., A_N(i-1)) of a matrix A,
//! where i is the diag_index.
//! @param diag_index Index of diagonal to return.
void Diagonal_matrix(VALUE_LIST* diag_res, MATRIX* mat, size_t diag_index);

//! @brief Rotates vector to the left by rotation.
//! Returns the rotated vector (v_i, v_(i+1), ..., v_(i-1)) of a vector v, where
//! i is the rotation.
void Rotate_vector(VALUE_LIST* res, VALUE_LIST* vec, int32_t rotation);

//! @brief Print matrix
void Print_matrix(FILE* fp, MATRIX* matrix);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_UTIL_MATRIX_OPERATIONS_H
