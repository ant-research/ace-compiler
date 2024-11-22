//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_UNITTEST_HELPER_H
#define RTLIB_UNITTEST_HELPER_H

#include <iostream>

#include "common/trace.h"
#include "gtest/gtest.h"
#include "util/ciphertext.h"
#include "util/fhe_types.h"
#include "util/fhe_utils.h"

#define RTLIB_UT_ENV()                     \
  DECL_ENV(TEST_DEGREE, "TEST_DEGREE", 16) \
  DECL_ENV(NUM_Q, "NUM_Q", 3)              \
  DECL_ENV(NUM_Q_PART, "NUM_Q_PART", 3)    \
  DECL_ENV(Q0_BITS, "Q0_BITS", 60)         \
  DECL_ENV(SF_BITS, "SF_BITS", 51)         \
  DECL_ENV(ITERATION, "ITERATION", 5)      \
  DECL_ENV(SPARSE, "SPARSE", 4)

typedef enum {
#define DECL_ENV(ID, NAME, DEFVAL) ID,
  RTLIB_UT_ENV()
#undef DECL_ENV
  // last id
  ENV_LAST
} RTLIB_UT_ENV_ID;

#define CHECK_INT_COEFFS(vals, degree, expected)  \
  {                                               \
    for (uint32_t idx = 0; idx < degree; idx++) { \
      EXPECT_EQ(vals[idx], expected[idx]);        \
    }                                             \
  }

#define CHECK_BIG_INT_COEFFS(coeffs, degree, expected)     \
  {                                                        \
    for (uint32_t idx = 0; idx < degree; idx++) {          \
      EXPECT_EQ(BI_CMP_SI(coeffs[idx], expected[idx]), 0); \
    }                                                      \
  }

const char Default_err_msg[] = "Error: vectors are not approximately equal.";

/**
 * @brief Checks whether two vectors are approximately equal.
 *
 * @param vec1 Vector 1(complex)
 * @param vec2 Vector 2(complex)
 * @param error How close each entry of the two vectors must be to be
 * approximately equal.
 * @param error_message Message to output if not equal.
 */
static inline void Check_complex_vector_approx_eq(
    VALUE_LIST* vec1, VALUE_LIST* vec2, float error = 0.00001,
    const char* error_message = Default_err_msg) {
  IS_TRUE(LIST_TYPE(vec1) == DCMPLX_TYPE && LIST_TYPE(vec2) == DCMPLX_TYPE,
          "check_complex_vector_approx_eq: vec1 & vec2 should be complex value "
          "list");
  size_t vec1_len = LIST_LEN(vec1);
  IS_TRUE(vec1_len == LIST_LEN(vec2), "length not match");
  bool is_diff = false;
  for (size_t i = 0; i < vec1_len; i++) {
    DCMPLX vec1_ith = DCMPLX_VALUE_AT(vec1, i);
    DCMPLX vec2_ith = DCMPLX_VALUE_AT(vec2, i);
    if ((abs(vec1_ith.real() - vec2_ith.real()) > error) ||
        (abs(vec1_ith.imag() - vec2_ith.imag()) > error)) {
      std::cout << "NOTE: VALUES DO NOT MATCH AT INDEX " << i
                << ", you could get degails from trace file. " << std::endl;

      IS_TRACE("-------- VALUES DO NOT MATCH AT INDEX %ld -------- \n", i);
      IS_TRACE("(%f + %fi) != (%f + %fi)\n", vec1_ith.real(), vec1_ith.imag(),
               vec2_ith.real(), vec2_ith.imag());
      is_diff = true;
      // always report failure
      EXPECT_TRUE(0);
    }
  }
  if (is_diff) {
    IS_TRACE("v1:\n");
    IS_TRACE_CMD(Print_value_list(Get_trace_file(), vec1));
    IS_TRACE("v2:\n");
    IS_TRACE_CMD(Print_value_list(Get_trace_file(), vec2));
  }
}

//! @brief Calculates the approximation max error
static inline void Calculate_approx_max_error(VALUE_LIST* vec1,
                                              VALUE_LIST* vec2) {
  IS_TRUE(LIST_TYPE(vec1) == DCMPLX_TYPE && LIST_TYPE(vec2) == DCMPLX_TYPE,
          "check_complex_vector_approx_eq: vec1 & vec2 should be complex value "
          "list");
  size_t len = LIST_LEN(vec1);
  IS_TRUE(len == LIST_LEN(vec2), "length not match");
  double max_error = 0;
  for (size_t i = 0; i < len; ++i) {
    DCMPLX vec1_ith = DCMPLX_VALUE_AT(vec1, i);
    DCMPLX vec2_ith = DCMPLX_VALUE_AT(vec2, i);
    double error    = abs(vec1_ith.real() - vec2_ith.real());
    if (max_error < error) max_error = error;
  }
  printf("precision: log2: %f, digit after decimal: 10^%f\n",
         fabs(log2(max_error)), log10(max_error));
}

/**
 * @brief Create crt context with given q primes & p primes
 *
 * @param crt return crt context
 * @param ring_degree ring degree
 * @param q_primes value list of q primes
 * @param p_primes value list of p primes
 */
static inline void Create_crt_with_primes(CRT_CONTEXT* crt,
                                          uint32_t     ring_degree,
                                          VALUE_LIST*  q_primes,
                                          VALUE_LIST*  p_primes) {
  Set_primes(Get_q(crt), q_primes, ring_degree);
  Set_primes(Get_p(crt), p_primes, ring_degree);

  Precompute_primes(Get_q(crt), true);

  Precompute_primes(Get_p(crt), true);

  Precompute_new_base(Get_q(crt), Get_p(crt));
  Precompute_new_base(Get_p(crt), Get_q(crt));
}

//! @brief Global envoriment for unit test
class UT_GLOB_ENV : public testing::Environment {
public:
  class ENV_INFO {
  public:
    const char* _env_name;
    size_t      _env_value;
  };

  void SetUp() override {
#define DECL_ENV(ID, NAME, VALUE) \
  Env_info[ID]._env_name  = NAME; \
  Env_info[ID]._env_value = VALUE;
    RTLIB_UT_ENV()
#undef DECL_ENV
  }

  static size_t Get_env(RTLIB_UT_ENV_ID id) {
    char*  env_str   = getenv(Env_info[id]._env_name);
    size_t env_value = Env_info[id]._env_value;
    if (env_str != nullptr && env_str[0] != '\0') {
      env_value = strtoul(env_str, NULL, 10);
    }
    return env_value;
  }

  static ENV_INFO Env_info[ENV_LAST];
};

#endif  // RTLIB_UNITTEST_HELPER_H
