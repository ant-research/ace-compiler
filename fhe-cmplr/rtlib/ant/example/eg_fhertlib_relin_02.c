//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// This file should be auto generated by onnx2c.py,
// it's used as driver for testing ONNX.

#include <math.h>

#include "common/rtlib.h"

double Expected_data[] = {0.17336736, 0.00977532, 2.3861214, 0.40457968};
int    Expected_len    = 4;

/**
 * @brief generate input data for testing ONNX
 *
 *
 * @param n
 * @param c
 * @param h
 * @param w
 * @param data, data pointer
 * @return TENSOR input data
 */
TENSOR* Generate_input_data(size_t n, size_t c, size_t h, size_t w,
                            double* data) {
  return Alloc_tensor(n, c, h, w, data);
}

/**
 * @brief validate output vector with expect vector
 *
 *
 * @param result double *
 * @param expect double *
 * @param len int
 * @return return true if value match
 */
bool Validate_output_data(double* result, double* expect, int len) {
  double error = 1e-3;
  for (int i = 0; i < len; i++) {
    if (fabs(result[i] - expect[i]) > error) {
      printf("index: %d, value: %f != %f\n", i, result[i], expect[i]);
      return false;
    }
  }
  return true;
}

// output = input1 * input1 + input1 * input1
int main(int argc, char* argv[]) {
  Prepare_context();

  double  input1[]    = {0.29442092776298523, -0.06991183012723923,
                         -1.092273235321045, -0.4497664272785187};
  TENSOR* input_data1 = Generate_input_data(1, 1, 2, 2, input1);
  printf("input");
  Print_tensor(stdout, input_data1);
  Prepare_input(input_data1, "input");
  Free_tensor(input_data1);

  Run_main_graph();

  double* result = Handle_output("output");

  Finalize_context();

  bool res = Validate_output_data(result, Expected_data, Expected_len);
  free(result);
  if (res) {
    printf("SUCESS!\n");
  } else {
    printf("FAILED!\n");
    return 1;
  };
  return 0;
}

// NOTE: This inc file here should be replaced by c file, which is auto
// generated by irb2c from FHE compiler
#include "eg_fhertlib_relin_02.inc"