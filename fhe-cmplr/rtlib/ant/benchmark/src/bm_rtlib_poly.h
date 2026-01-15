//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_BENCHMARK_SRC_BM_RTLIB_POLY_H
#define RTLIB_ANT_BENCHMARK_SRC_BM_RTLIB_POLY_H

#include <random>

#include "common/rt_api.h"
#include "helper.h"
#include "poly/rns_poly.h"
#include "rns_poly_impl.h"

//! @brief Alloc crtcontext with parameters from BM_GLOB_ENV
static CRT_CONTEXT* Alloc_crt() {
  size_t degree    = BM_GLOB_ENV::Get_env(TEST_DEGREE);
  size_t num_q     = BM_GLOB_ENV::Get_env(NUM_Q);
  size_t num_parts = BM_GLOB_ENV::Get_env(NUM_Q_PART);
  size_t q0_bits   = BM_GLOB_ENV::Get_env(Q0_BITS);
  size_t sf_bits   = BM_GLOB_ENV::Get_env(SF_BITS);
  if (num_parts == 0) {
    num_parts = Get_default_num_q_parts(num_q - 1);
  }
  CRT_CONTEXT* crt = Alloc_crtcontext();
  Init_crtcontext_with_prime_size(crt, HE_STD_NOT_SET, degree, num_q, q0_bits,
                                  sf_bits, num_parts);
  Set_global_crt_context((PTR_TY)crt);
  return crt;
}

//! @brief Free crtcontext from global ckks context
static void Free_crt() { Finalize_context(); }

//! @brief Allocate a zero poly with input parameters
static POLYNOMIAL* Alloc_zero_poly(size_t degree, size_t num_q, size_t num_p) {
  POLYNOMIAL* poly = (POLYNOMIAL*)malloc(sizeof(POLYNOMIAL));
  Alloc_poly_data(poly, degree, num_q, num_p);
  return poly;
}

//! @brief Get a random array<int64_t> with input len
static int64_t* Random_array(size_t len) {
  int64_t*        array = new int64_t[len];
  std::mt19937_64 rng(static_cast<unsigned int>(std::random_device{}()));
  std::uniform_int_distribution<int64_t> dist(0, 1000000);
  for (size_t i = 0; i < len; ++i) {
    array[i] = dist(rng);
  }
  return array;
}

//! @brief Allocate random rns poly with input parameters
static POLYNOMIAL* Alloc_rnd_poly(size_t degree, size_t num_q,
                                  bool extend_p = false, bool is_ntt = true) {
  // step1: allocate a zero poly
  POLYNOMIAL* poly = Alloc_zero_poly(degree, num_q, extend_p ? Get_p_cnt() : 0);
  if (is_ntt) {
    Set_is_ntt(poly, TRUE);
  }

  // step2: get random VALUE_LIST<int64_t>
  int64_t*    coeffs = Random_array(degree);
  VALUE_LIST* val    = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(val, degree, coeffs);
  delete[] coeffs;

  // step3: transform rns poly
  Transform_value_to_rns_poly(poly, val, true);

  // step4: cleanup
  Free_value_list(val);
  return poly;
}

//! @brief Get length of decomp
static size_t Decomp_len(size_t q_part_idx) {
  CRT_CONTEXT* crt = Get_crt_context();
  IS_TRUE(q_part_idx < Get_num_decomp(crt, num_q),
          "q_part_idx should not greater than num of decomp");
  return LIST_LEN(Get_vl_value_at(Get_primes(Get_qpart(crt)), q_part_idx));
}

#endif  // RTLIB_ANT_BENCHMARK_SRC_BM_RTLIB_POLY_H