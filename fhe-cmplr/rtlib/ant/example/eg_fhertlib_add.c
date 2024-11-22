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

/**
 * @brief generate input data for testing ONNX
 *
 *
 * @return TENSOR input data
 */
TENSOR* Generate_input_data() {
  // return torch.randn(shape);
  // hack input tensor
  double data[8] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};
  return Alloc_tensor(1, 1, 2, 4, data);
}

/**
 * @brief validate output vector with expect vector
 *
 *
 * @param result double *
 * @param expect double *
 * @return return true if value match
 */
bool Validate_output_data(double* result, double* expect, size_t len) {
  for (size_t i = 0; i < len; i++) {
    double d1 = result[i];
    double d2 = expect[i];
    if (fabs(d1 - d2) > 1e-3) {
      printf("-- VALUES DO NOT MATCH AT INDEX %ld --\n", i);
      printf("%f != %f\n", d1, d2);
      return false;
    }
  }
  return true;
}

/**
 * @brief entry function
 * for add.onnx
 *
 * @return return value
 */
int main() {
  TENSOR* input_data1 = Generate_input_data();
  TENSOR* input_data2 = Generate_input_data();
  printf("input");
  Print_tensor(stdout, input_data1);
  printf("onnx::Add_1");
  Print_tensor(stdout, input_data2);

  Prepare_context();

  Prepare_input(input_data1, "input");
  Prepare_input(input_data2, "onnx::Add_1");
  Free_tensor(input_data1);
  Free_tensor(input_data2);

  Run_main_graph();

  double* result = Handle_output("output");

  Finalize_context();

  size_t len      = 8;
  double expect[] = {0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6};
  bool   res      = Validate_output_data(result, expect, len);
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
#include "eg_fhertlib_add.inc"