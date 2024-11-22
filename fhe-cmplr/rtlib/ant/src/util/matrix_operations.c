//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/matrix_operations.h"

#include "common/trace.h"
#include "util/fhe_types.h"
#include "util/fhe_utils.h"

#define GEN_ALLOC_MATRIX(NAME, T, VAL_TYPE)                          \
  MATRIX* Alloc_##NAME##_matrix(size_t rows, size_t cols, T* vals) { \
    MATRIX* mat     = (MATRIX*)malloc(sizeof(MATRIX));               \
    mat->_type      = VAL_TYPE;                                      \
    mat->_rows      = rows;                                          \
    mat->_cols      = cols;                                          \
    size_t mem_size = Matrix_mem_size(mat);                          \
    mat->_vals._a   = (ANY_VAL*)malloc(mem_size);                    \
    if (vals == NULL) {                                              \
      memset(mat->_vals._a, 0, mem_size);                            \
    } else {                                                         \
      memcpy(mat->_vals._a, vals, mem_size);                         \
    }                                                                \
    return mat;                                                      \
  }

// generate function body for Alloc_dcmplx_matrix
GEN_ALLOC_MATRIX(dcmplx, DCMPLX, DCMPLX_TYPE)
// generate function body for Alloc_int64_matrix
GEN_ALLOC_MATRIX(int64, int64_t, I64_TYPE)
// generate function body for Alloc_dbl_matrix
GEN_ALLOC_MATRIX(dbl, double, DBL_TYPE)

void Free_matrix(MATRIX* matrix) {
  if (matrix == NULL) return;
  if (matrix->_vals._a) {
    free(matrix->_vals._a);
    matrix->_vals._a = NULL;
  }
  free(matrix);
}

size_t Matrix_mem_size(MATRIX* mat) {
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

bool Same_dim(MATRIX* mat0, MATRIX* mat1) {
  if (!mat0 || !mat1) return FALSE;
  if (mat0->_type == mat1->_type && mat0->_rows == mat1->_rows &&
      mat0->_cols == mat1->_cols) {
    return TRUE;
  }
  return FALSE;
}

void Diagonal_matrix(VALUE_LIST* diag_res, MATRIX* mat, size_t diag_index) {
  size_t rows = MATRIX_ROWS(mat);
  size_t cols = MATRIX_COLS(mat);
  IS_TRUE(diag_res && mat, "null diag result or input matrix");
  IS_TRUE(LIST_LEN(diag_res) == cols, "return list length not equal");
  IS_TRUE(!Is_empty(diag_res), "empty diag result");

  switch (MATRIX_TYPE(mat)) {
    case I64_TYPE:
      for (size_t i = 0; i < cols; i++) {
        I64_VALUE_AT(diag_res, i) =
            INT64_MATRIX_ELEM(mat, i % rows, (diag_index + i) % cols);
      }
      break;
    case DBL_TYPE:
      for (size_t i = 0; i < cols; i++) {
        DBL_VALUE_AT(diag_res, i) =
            DBL_MATRIX_ELEM(mat, i % rows, (diag_index + i) % cols);
      }
      break;
    case DCMPLX_TYPE:
      for (size_t i = 0; i < cols; i++) {
        DCMPLX_VALUE_AT(diag_res, i) =
            DCMPLX_MATRIX_ELEM(mat, i % rows, (diag_index + i) % cols);
      }
      break;
    default: {
      IS_TRUE(FALSE, ("Invalid type"));
    }
  }
}

void Rotate_vector(VALUE_LIST* res, VALUE_LIST* vec, int32_t rotation) {
  IS_TRUE(res && vec && res != vec, "res and input vector should be different");
  IS_TRUE(LIST_LEN(res) == LIST_LEN(vec), "unmatched vector length");
  IS_TRUE(LIST_TYPE(res) == LIST_TYPE(vec), "unmatched vector value type");
  IS_TRUE(!Is_empty(res), "empty result");

  size_t len = LIST_LEN(res);
  switch (LIST_TYPE(res)) {
    case I64_TYPE:
      for (size_t idx = 0; idx < len; idx++) {
        I64_VALUE_AT(res, idx) =
            I64_VALUE_AT(vec, Mod_int64(idx + rotation, len));
      }
      break;
    case DBL_TYPE:
      for (size_t idx = 0; idx < len; idx++) {
        DBL_VALUE_AT(res, idx) =
            DBL_VALUE_AT(vec, Mod_int64(idx + rotation, len));
      }
      break;
    case DCMPLX_TYPE:
      for (size_t idx = 0; idx < len; idx++) {
        DCMPLX_VALUE_AT(res, idx) =
            DCMPLX_VALUE_AT(vec, Mod_int64(idx + rotation, len));
      }
      break;
    default:
      IS_TRUE(FALSE, ("Invalid type"));
  }
}

void Print_matrix(FILE* fp, MATRIX* matrix) {
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
