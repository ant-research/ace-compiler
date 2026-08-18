//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/tensor.h"

#include <stdlib.h>
#include <string.h>

#include "common/error.h"

static TENSOR* Alloc_tensor_buffer(size_t n, size_t c, size_t h, size_t w,
                                   size_t tsize) {
  IS_TRUE(n * c * h * w * sizeof(double) == tsize, "buffer size mismatch");
  TENSOR* tensor    = (TENSOR*)malloc(sizeof(TENSOR) + tsize);
  tensor->_shape._n = n;
  tensor->_shape._c = c;
  tensor->_shape._h = h;
  tensor->_shape._w = w;
  return tensor;
}

TENSOR* Alloc_tensor(size_t n, size_t c, size_t h, size_t w,
                     const double* vals) {
  size_t  tsize  = n * c * h * w * sizeof(double);
  TENSOR* tensor = Alloc_tensor_buffer(n, c, h, w, tsize);
  if (vals == NULL) {
    memset(tensor->_vals, 0, tsize);
  } else {
    memcpy(tensor->_vals, vals, tsize);
  }
  return tensor;
}

void Free_tensor(TENSOR* tensor) { free(tensor); }

bool Is_tensor_match(TENSOR* t1, TENSOR* t2) {
  if (TENSOR_N(t1) == TENSOR_N(t2) && TENSOR_C(t1) == TENSOR_C(t2) &&
      TENSOR_H(t1) == TENSOR_H(t2) && TENSOR_W(t1) == TENSOR_W(t2)) {
    return true;
  }
  return false;
}

TENSOR* Add_tensor(TENSOR* t1, TENSOR* t2) {
  IS_TRUE(Is_tensor_match(t1, t2), "input tensor not match");
  size_t  tn    = TENSOR_N(t1);
  size_t  tc    = TENSOR_C(t1);
  size_t  th    = TENSOR_H(t1);
  size_t  tw    = TENSOR_W(t1);
  size_t  tsize = tn * tc * th * tw * sizeof(double);
  TENSOR* res   = Alloc_tensor_buffer(tn, tc, th, tw, tsize);
  FOR_ALL_TENSOR_ELEM(res, n, c, h, w) {
    TENSOR_ELEM(res, n, c, h, w) =
        TENSOR_ELEM(t1, n, c, h, w) + TENSOR_ELEM(t2, n, c, h, w);
  }
  return res;
}

void Print_tensor(FILE* fp, TENSOR* tensor) {
  fprintf(fp, "(tensor): [\n");
  for (size_t n = 0; n < TENSOR_N(tensor); n++) {
    if (n) fprintf(fp, "\n");
    for (size_t c = 0; c < TENSOR_C(tensor); c++) {
      if (c) fprintf(fp, "\n");
      for (size_t h = 0; h < TENSOR_H(tensor); h++) {
        if (h) fprintf(fp, "\n");
        for (size_t w = 0; w < TENSOR_W(tensor); w++) {
          fprintf(fp, " %f", TENSOR_ELEM(tensor, n, c, h, w));
        }
      }
    }
  }
  fprintf(fp, "\n]\n");
}
