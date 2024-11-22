//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef FHE_POLY_TEST_GEN_EXPECT_DATA_H
#define FHE_POLY_TEST_GEN_EXPECT_DATA_H

#include <iostream>
#include <string>

static inline void Gen_msg_add_ciph(std::ofstream& of) {
  std::string emit_str =
      "\
double *Msg_add(double *input1, double *input2, size_t input_len) {\n\
  double *exp_data = (double *) malloc(sizeof(double) * input_len);\n\
  memset(exp_data, 0, sizeof(double) * input_len);\n\
  for (size_t idx = 0; idx < input_len; idx++) {\n\
    exp_data[idx] = input1[idx] + input2[idx];\n\
  }\n\
  return exp_data;\n\
}";
  of << emit_str << std::endl;
}

static inline void Gen_msg_add_float(std::ofstream& of) {
  std::string emit_str =
      "\
double *Msg_add_float(double *input, size_t input_len, float f) {\n\
  double *exp_data = (double *) malloc(sizeof(double) * input_len);\n\
  for (size_t idx = 0; idx < input_len; idx++) {\n\
    exp_data[idx] = input[idx] + f;\n\
  }\n\
  return exp_data;\n\
}";
  of << emit_str << std::endl;
}

static inline void Gen_msg_mul_float(std::ofstream& of) {
  std::string emit_str =
      "\
double *Msg_mul_float(double *input, size_t input_len, float f) {\n\
  double *exp_data = (double *) malloc(sizeof(double) * input_len);\n\
  for (size_t idx = 0; idx < input_len; idx++) {\n\
    exp_data[idx] = input[idx] * f;\n\
  }\n\
  return exp_data;\n\
}";
  of << emit_str << std::endl;
}

static inline void Gen_msg_mul_ciph(std::ofstream& of) {
  std::string emit_str =
      "\
double *Msg_mul_ciph(double *input1, double *input2, size_t input_len) {\n\
  double *exp_data = (double *) malloc(sizeof(double) * input_len);\n\
  for (size_t idx = 0; idx < input_len; idx++) {\n\
    exp_data[idx] = input1[idx] * input2[idx];\n\
  }\n\
  return exp_data;\n\
}";
  of << emit_str << std::endl;
}

static inline void Gen_msg_rotate(std::ofstream& of) {
  std::string emit_str =
      "\
double *Msg_rotate(double *input, size_t input_len, uint32_t rot_idx) {\n\
  uint32_t  len = Degree() / 2;\n\
  double input_data[len];\n\
  memset(input_data, 0, sizeof(double) * len);\n\
  memcpy(input_data, input, sizeof(double) * input_len);\n\
  double *exp_data = (double *) malloc(sizeof(double) * len);\n\
  memset(exp_data, 0, sizeof(double) * len);\n\
  for (size_t idx = 0; idx < len; idx++) {\n\
    exp_data[idx] = input_data[(idx + rot_idx) % len];\n\
  }\n\
  return exp_data;\n\
}";
  of << emit_str << std::endl;
}

void Gen_expected(std::ofstream& of);

#endif