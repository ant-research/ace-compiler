//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "util/fhe_utils.h"
#include "util/prng.h"
#include "util/random_sample.h"

//! @brief Calculate the mean of an array values
double Mean(int32_t arr[], int32_t size) {
  double sum  = 0.0;
  double mean = 0.0;

  // Compute the mean
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  mean = sum / size;
  return mean;
}

//! @brief Calculate the std deviation of an array values
double Std_deviation(int32_t arr[], int size) {
  double mean     = Mean(arr, size);
  double variance = 0.0;
  // Compute the variance
  for (size_t i = 0; i < size; i++) {
    variance += pow(fabs(arr[i] - mean), 2);
  }
  return sqrt(variance / size);
}

void Test_uniform_prng(int32_t* arr, int size, int32_t min, int32_t max) {
  BLAKE2_PRNG* prng = Get_prng();
  for (size_t idx = 0; idx < size; idx++) {
    arr[idx] = Uniform_int_prng(prng, min, max);
    EXPECT_GE(arr[idx], min);
    EXPECT_LE(arr[idx], max);
  }

  // the distance of mean and std deviation is hard to check for
  // large range, need to perfrom critical value or p-value approach
  // to hypothesis testing
  double exp_mean      = (double)(max - min) / 2 + min;
  double exp_deviation = sqrt(pow((max - min), 2) / 12);
  EXPECT_NEAR(Mean(arr, size), exp_mean, 1);
  EXPECT_NEAR(Std_deviation(arr, size), exp_deviation, 1);
}

TEST(prng, test_unifrom_int) {
  size_t  size = 10000;
  int32_t arr[size];
  Test_uniform_prng(arr, size, 0, 100);
  Test_uniform_prng(arr, size, -1, 1);
}

TEST(sample, test_tenary) {
  VALUE_LIST* value = Alloc_value_list(I64_TYPE, 1024);
  // test hamming weight 0
  Sample_ternary(value, 0);
  FOR_ALL_ELEM(value, idx) {
    int64_t val = Get_i64_value_at(value, idx);
    EXPECT_TRUE(val == 0 || val == 1 || val == -1);
  }

  // test hamming weight 192
  uint32_t hamming_weight = 192;
  Sample_ternary(value, hamming_weight);
  uint32_t one_cnt     = 0;
  uint32_t neg_one_cnt = 0;
  FOR_ALL_ELEM(value, idx) {
    int64_t val = Get_i64_value_at(value, idx);
    EXPECT_TRUE(val == 0 || val == 1 || val == -1);
    if (val == 1) one_cnt++;
    if (val == -1) neg_one_cnt++;
  }
  EXPECT_EQ(one_cnt + neg_one_cnt, hamming_weight);
  EXPECT_LE(abs((int32_t)((int32_t)one_cnt - neg_one_cnt)), 2);

  Free_value_list(value);
}
