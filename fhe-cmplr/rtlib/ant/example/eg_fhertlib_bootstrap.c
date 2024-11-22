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

double Expected_data[] = {2.0380783, -2.1710918, 0.975174, 1.648381};
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

// for add_const.onnx
int main(int argc, char* argv[]) {
  Prepare_context();

  double  input1[]    = {1.0380784273147583, -3.1710917949676514,
                         -0.024826018139719963, 0.6483810544013977};
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
#include "eg_fhertlib_bootstrap.inc"