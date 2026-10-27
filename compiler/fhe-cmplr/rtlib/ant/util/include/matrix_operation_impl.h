//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_MATRIX_OPERATION_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_MATRIX_OPERATION_IMPL_H

#include <stdio.h>

#include "util/matrix_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Returns matrix data memory size
static inline size_t Matrix_mem_size(MATRIX* mat) {
  IS_TRUE(mat, ("null mat"));
  switch (mat->_type) {
    case I64_TYPE:
      return mat->_rows * mat->_cols * sizeof(int64_t);
      break;
    case DBL_TYPE:
      return mat->_rows * mat->_cols * sizeof(double);
      break;
    case DCMPLX_TYPE:
      return mat->_rows * mat->_cols * sizeof(DCMPLX);
      break;
    default:
      IS_TRUE(FALSE, "Invalid matrix type");
  }
  return 0;
}

//! @brief Print matrix
static inline void Print_matrix(FILE* fp, MATRIX* matrix) {
  fprintf(fp, "[%ld][%ld]\n", matrix->_rows, matrix->_cols);
  for (size_t i = 0; i < MATRIX_ROWS(matrix); i++) {
    for (size_t j = 0; j < MATRIX_COLS(matrix); j++) {
      switch (matrix->_type) {
        case I64_TYPE: {
          fprintf(fp, "%ld ", INT64_MATRIX_ELEM(matrix, i, j));
        } break;
        case DBL_TYPE: {
          fprintf(fp, "%f ", DBL_MATRIX_ELEM(matrix, i, j));
        } break;
        case DCMPLX_TYPE: {
          DCMPLX elem = DCMPLX_MATRIX_ELEM(matrix, i, j);
          fprintf(fp, "%f+%fi ", creal(elem), cimag(elem));
        } break;
        default: {
          IS_TRUE(FALSE, "Invalid type");
        }
      }
    }
  }
  fprintf(fp, "\n");
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_UTIL_INCLUDE_MATRIX_OPERATION_IMPL_H
