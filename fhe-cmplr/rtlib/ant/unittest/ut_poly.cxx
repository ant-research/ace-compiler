//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rt_api.h"
#include "common/rt_config.h"
#include "context/ckks_context.h"
#include "crt_impl.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "poly/rns_poly.h"
#include "rns_poly_impl.h"
#include "util/crt.h"
#include "util/prng.h"

const size_t  Coeff_len  = 4;
const int64_t Coeffs1[4] = {0, 1, 4, 5};
const int64_t Coeffs2[4] = {1, 2, 4, 3};

class TEST_POLYNOMIAL : public ::testing::Test {
protected:
  void SetUp() override {
    _degree     = 4;
    _num_primes = 10;
    _crt        = Alloc_crtcontext();
    Init_crtcontext_with_prime_size(_crt, HE_STD_NOT_SET, _degree, _num_primes,
                                    60, 59, 3);
    // Set crtcontext to global CKKS_CONTEXT
    Set_global_crt_context((PTR_TY)_crt);
  }

  size_t       Get_prime() { return _num_primes; }
  size_t       Get_degree() { return _degree; }
  CRT_CONTEXT* Get_crt() { return _crt; }
  void         Multiply_poly(bool mul_fast);

  void TearDown() override {
    Free_prng();
    Finalize_context();
  }

private:
  size_t       _degree;
  size_t       _num_primes;
  CRT_CONTEXT* _crt;
};

void TEST_POLYNOMIAL::Multiply_poly(bool mul_fast) {
  size_t degree     = Get_degree();
  size_t num_primes = Get_prime();
  FMT_ASSERT(Coeff_len == degree, "degree not match in Mul_poly");
  VALUE_LIST* val1 = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* val2 = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* val3 = Alloc_value_list(BIGINT_TYPE, degree);
  VALUE_LIST* val4 = Alloc_value_list(BIGINT_TYPE, degree);
  Init_i64_value_list(val1, degree, (int64_t*)Coeffs1);
  Init_i64_value_list(val2, degree, (int64_t*)Coeffs2);

  POLYNOMIAL poly_prod1, poly_prod2, int_poly1, int_poly2;
  Alloc_poly_data(&poly_prod1, degree, num_primes, 0);
  Alloc_poly_data(&poly_prod2, degree, num_primes, 0);
  Alloc_poly_data(&int_poly1, degree, num_primes, 0);
  Alloc_poly_data(&int_poly2, degree, num_primes, 0);

  Transform_value_to_rns_poly(&int_poly1, val1, true);
  Transform_value_to_rns_poly(&int_poly2, val2, true);

  if (mul_fast) {
    Conv_poly2ntt_inplace(&int_poly1);
    Conv_poly2ntt_inplace(&int_poly2);

    // init precompute of int_poly1(ntt) & int_poly2(ntt)
    POLYNOMIAL int_poly1_prec;
    POLYNOMIAL int_poly2_prec;
    Alloc_poly_data(&int_poly1_prec, degree, num_primes, 0);
    Alloc_poly_data(&int_poly2_prec, degree, num_primes, 0);
    Barrett_poly(&int_poly1_prec, &int_poly1);
    Barrett_poly(&int_poly2_prec, &int_poly2);

    Mul_poly_fast(&poly_prod1, &int_poly1, &int_poly2, &int_poly2_prec);
    Mul_poly_fast(&poly_prod2, &int_poly2, &int_poly1, &int_poly1_prec);

    Free_poly_data(&int_poly1_prec);
    Free_poly_data(&int_poly2_prec);
  } else {
    Mul_poly(&poly_prod1, &int_poly1, &int_poly2);
    Mul_poly(&poly_prod2, &int_poly2, &int_poly1);
  }
  Conv_ntt2poly_inplace(&poly_prod1);
  Conv_ntt2poly_inplace(&poly_prod2);
  Reconstruct_rns_poly_to_value(val3, &poly_prod1);
  Reconstruct_rns_poly_to_value(val4, &poly_prod2);
  int64_t expected[16] = {-29, -31, -9, 17};
  CHECK_BIG_INT_COEFFS(Get_bint_values(val3), degree, expected);
  CHECK_BIG_INT_COEFFS(Get_bint_values(val4), degree, expected);

  Free_poly_data(&poly_prod1);
  Free_poly_data(&poly_prod2);
  Free_poly_data(&int_poly1);
  Free_poly_data(&int_poly2);
  Free_value_list(val1);
  Free_value_list(val2);
  Free_value_list(val3);
  Free_value_list(val4);
}

TEST_F(TEST_POLYNOMIAL, sub_poly) {
  size_t      degree     = Get_degree();
  size_t      num_primes = Get_prime();
  VALUE_LIST* res_list   = Alloc_value_list(BIGINT_TYPE, degree);
  POLYNOMIAL  poly_diff, int_poly1, int_poly2;
  Alloc_poly_data(&poly_diff, degree, num_primes, 0);
  Alloc_poly_data(&int_poly1, degree, num_primes, 0);
  Alloc_poly_data(&int_poly2, degree, num_primes, 0);
  VALUE_LIST* coeff1_list = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* coeff2_list = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(coeff1_list, Coeff_len, (int64_t*)Coeffs1);
  Init_i64_value_list(coeff2_list, Coeff_len, (int64_t*)Coeffs2);

  Transform_value_to_rns_poly(&int_poly1, coeff1_list, true);
  Transform_value_to_rns_poly(&int_poly2, coeff2_list, true);
  Free_value_list(coeff1_list);
  Free_value_list(coeff2_list);

  Sub_poly(&poly_diff, &int_poly1, &int_poly2);
  Reconstruct_rns_poly_to_value(res_list, &poly_diff);

  int64_t expect_diff[] = {-1, -1, 0, 2};
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), Coeff_len, expect_diff);

  Free_value_list(res_list);
  Free_poly_data(&poly_diff);
  Free_poly_data(&int_poly1);
  Free_poly_data(&int_poly2);
}

TEST_F(TEST_POLYNOMIAL, scalar_integer_multiply) {
  CRT_CONTEXT* crt               = Get_crt();
  size_t       degree            = Get_degree();
  size_t       num_primes        = Get_prime();
  size_t       num_p_primes      = Get_crt_num_p(crt);
  int64_t      coeffs[Coeff_len] = {1, 2, 0, 2};
  FMT_ASSERT(degree == Coeff_len, "degree not match in rotate_poly");
  POLYNOMIAL poly, res;
  Alloc_poly_data(&poly, degree, num_primes, num_p_primes);
  Alloc_poly_data(&res, degree, num_primes, num_p_primes);
  VALUE_LIST* coeffs_list = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(coeffs_list, Coeff_len, coeffs);
  Transform_value_to_rns_poly(&poly, coeffs_list, true);

  BIG_INT scalar_bi;
  BI_INIT_ASSIGN_SI(scalar_bi, 1);
  BI_LSHIFT(scalar_bi, 90);

  // res[Coeff_len] = coeffs[Coeff_len] * scalar_bi
  VALUE_LIST* expected = Alloc_value_list(BIGINT_TYPE, degree);
  FOR_ALL_ELEM(expected, idx) {
    BIG_INT* bi = Get_bint_value_at(expected, idx);
    BI_MUL_SI(*bi, scalar_bi, coeffs[idx]);
  }

  VALUE_LIST* rns_scalars =
      Alloc_value_list(I64_TYPE, num_primes + num_p_primes);
  Transform_to_qpcrt(rns_scalars, crt, scalar_bi);
  Scalars_integer_multiply_poly(&res, &poly, rns_scalars);

  VALUE_LIST* res_list = Alloc_value_list(BIGINT_TYPE, degree);
  Reconstruct_rns_poly_to_value(res_list, &res);

  CHECK_BIG_COEFFS(Get_bint_values(res_list), Coeff_len,
                   Get_bint_values(expected));
  Free_poly_data(&poly);
  Free_poly_data(&res);
  Free_value_list(expected);
  Free_value_list(coeffs_list);
  Free_value_list(res_list);
  Free_value_list(rns_scalars);
  BI_FREES(scalar_bi);
}

TEST_F(TEST_POLYNOMIAL, rotate_poly) {
  int64_t coeffs[Coeff_len] = {0, 1, 4, 59};
  int32_t rotation_size     = 3;
  size_t  num_primes        = Get_prime();
  size_t  degree            = Get_degree();
  FMT_ASSERT(degree == Coeff_len, "degree not match in rotate_poly");
  POLYNOMIAL poly, res;
  Alloc_poly_data(&poly, degree, num_primes, 0);
  Alloc_poly_data(&res, degree, num_primes, 0);
  VALUE_LIST* coeffs_list = Alloc_value_list(I64_TYPE, Coeff_len);
  Init_i64_value_list(coeffs_list, Coeff_len, coeffs);
  Transform_value_to_rns_poly(&poly, coeffs_list, true);

  Rotate_poly_with_rotation_idx(&res, &poly, rotation_size);

  VALUE_LIST* res_list = Alloc_value_list(BIGINT_TYPE, degree);
  Reconstruct_rns_poly_to_value(res_list, &res);
  int64_t expected1[] = {0, -1, 4, -59};
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), Coeff_len, expected1);

  rotation_size = 6;
  Rotate_poly_with_rotation_idx(&res, &poly, rotation_size);
  Reconstruct_rns_poly_to_value(res_list, &res);
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), Coeff_len, coeffs);

  Free_poly_data(&poly);
  Free_poly_data(&res);
  Free_value_list(coeffs_list);
  Free_value_list(res_list);
}

TEST_F(TEST_POLYNOMIAL, add_poly_int) {
  size_t num_primes = Get_prime();
  size_t degree     = Get_degree();
  FMT_ASSERT(Coeff_len == degree, "degree not match in add_poly_int");
  VALUE_LIST* val1 = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* val2 = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* val3 = Alloc_value_list(BIGINT_TYPE, degree);
  Init_i64_value_list(val1, degree, (int64_t*)Coeffs1);
  Init_i64_value_list(val2, degree, (int64_t*)Coeffs2);

  POLYNOMIAL poly_sum, int_poly1, int_poly2;
  Alloc_poly_data(&poly_sum, degree, num_primes, 0);
  Alloc_poly_data(&int_poly1, degree, num_primes, 0);
  Alloc_poly_data(&int_poly2, degree, num_primes, 0);

  Transform_value_to_rns_poly(&int_poly1, val1, true);
  Transform_value_to_rns_poly(&int_poly2, val2, true);

  Add_poly(&poly_sum, &int_poly1, &int_poly2);
  Reconstruct_rns_poly_to_value(val3, &poly_sum);
  int64_t expect_sum[] = {1, 3, 8, 8};
  CHECK_BIG_INT_COEFFS(Get_bint_values(val3), degree, expect_sum);
  Free_poly_data(&poly_sum);
  Free_poly_data(&int_poly1);
  Free_poly_data(&int_poly2);
  Free_value_list(val1);
  Free_value_list(val2);
  Free_value_list(val3);
}

TEST_F(TEST_POLYNOMIAL, Mul_poly) { Multiply_poly(false); }

TEST_F(TEST_POLYNOMIAL, Mul_poly_fast) { Multiply_poly(true); }

TEST_F(TEST_POLYNOMIAL, mod_small_poly) {
  size_t num_primes = Get_prime();
  size_t degree     = Get_degree();
  FMT_ASSERT(degree == Coeff_len, "degree not match in mod_small_poly");

  uint64_t mod = 60;
  BIG_INT  half_mod;
  BI_INIT_ASSIGN_SI(half_mod, mod / 2);

  int64_t    coeffs[Coeff_len] = {-1, 3, 78, 10999};
  POLYNOMIAL poly, res;
  Alloc_poly_data(&poly, degree, num_primes, 0);
  Alloc_poly_data(&res, degree, num_primes, 0);
  VALUE_LIST* vals = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(vals, degree, coeffs);
  Transform_value_to_rns_poly(&poly, vals, true);

  VALUE_LIST* combined = Alloc_value_list(BIGINT_TYPE, degree);
  Reconstruct_rns_poly_to_value(combined, &poly);
  FOR_ALL_ELEM(combined, idx) {
    BIG_INT* bi = Get_bint_value_at(combined, idx);
    BI_MOD_UI(*bi, *bi, mod);
    if (BI_CMP(*bi, half_mod) > 0) {
      BI_SUB_UI(*bi, *bi, mod);
    }
  }
  int64_t expected[] = {-1, 3, 18, 19};
  CHECK_BIG_INT_COEFFS(Get_bint_values(combined), degree, expected);
  Free_poly_data(&poly);
  Free_poly_data(&res);
  Free_value_list(combined);
  Free_value_list(vals);

  BI_FREES(half_mod);
}

TEST_F(TEST_POLYNOMIAL, crt_convert) {
  CRT_CONTEXT* crt    = Get_crt();
  size_t       degree = Get_degree();
  FMT_ASSERT(degree == Coeff_len, "degree not match in crt_convert");
  VALUE_LIST* val               = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* res               = Alloc_value_list(BIGINT_TYPE, degree);
  int64_t     values[Coeff_len] = {3, -4, 5, 8};
  POLYNOMIAL  poly;
  Init_i64_value_list(val, degree, values);
  Alloc_poly_data(&poly, degree, Get_primes_cnt(Get_q(crt)),
                  Get_primes_cnt(Get_p(crt)));
  Transform_value_to_rns_poly(&poly, val, true);
  Reconstruct_rns_poly_to_value(res, &poly);
  CHECK_BIG_INT_COEFFS(Get_bint_values(res), degree, values);
  Free_value_list(val);
  Free_value_list(res);
  Free_poly_data(&poly);
}

TEST_F(TEST_POLYNOMIAL, conv_poly_and_ntt) {
  size_t degree = Get_degree();
  FMT_ASSERT(degree == Coeff_len,
             "degree not match for unittest: conv_poly_and_ntt");
  int64_t    data[Coeff_len]     = {2, 4, 9, 15};
  int64_t    expected[Coeff_len] = {2, 4, 9, 15};
  POLYNOMIAL poly;
  Init_poly_data(&poly, degree, 1, 0, data);
  Conv_poly2ntt_inplace(&poly);
  Conv_ntt2poly_inplace(&poly);
  CHECK_INT_COEFFS(Get_poly_coeffs(&poly), degree, expected);
}

TEST_F(TEST_POLYNOMIAL, sample_uniform_poly) {
  POLYNOMIAL   poly;
  VL_CRTPRIME* q_primes = Get_q_primes(Get_crt());
  VL_CRTPRIME* p_primes = Get_p_primes(Get_crt());
  Alloc_poly_data(&poly, Get_degree(), LIST_LEN(q_primes), LIST_LEN(p_primes));
  Sample_uniform_poly(&poly);
  int64_t* coeffs    = Get_poly_coeffs(&poly);
  MODULUS* q_modulus = Get_modulus_head(q_primes);
  for (size_t i = 0; i < LIST_LEN(q_primes); i++) {
    for (size_t j = 0; j < Get_degree(); j++) {
      EXPECT_LT(*coeffs, Get_mod_val(q_modulus));
      EXPECT_GE(*coeffs, 0);
      coeffs++;
    }
    q_modulus = Get_next_modulus(q_modulus);
  }

  MODULUS* p_modulus = Get_modulus_head(p_primes);
  int64_t* p_coeffs  = Get_p_coeffs(&poly);
  for (size_t i = 0; i < LIST_LEN(p_primes); i++) {
    for (size_t j = 0; j < Get_degree(); j++) {
      EXPECT_LT(*p_coeffs, Get_mod_val(p_modulus));
      EXPECT_GE(*p_coeffs, 0);
      p_coeffs++;
    }
    p_modulus = Get_next_modulus(p_modulus);
  }
  Free_poly_data(&poly);
}

TEST_F(TEST_POLYNOMIAL, Sample_ternary_poly) {
  POLYNOMIAL   poly;
  VL_CRTPRIME* q_primes = Get_q_primes(Get_crt());
  VL_CRTPRIME* p_primes = Get_p_primes(Get_crt());
  Alloc_poly_data(&poly, Get_degree(), LIST_LEN(q_primes), LIST_LEN(p_primes));
  Sample_ternary_poly(&poly, 0);
  int64_t* coeffs    = Get_poly_coeffs(&poly);
  MODULUS* q_modulus = Get_modulus_head(q_primes);
  for (size_t i = 0; i < LIST_LEN(q_primes); i++) {
    for (size_t j = 0; j < Get_degree(); j++) {
      int64_t val = *coeffs;
      EXPECT_LT(val, Get_mod_val(q_modulus));
      EXPECT_GE(val, 0);
      EXPECT_TRUE(val == 0 || val == 1 || val == Get_mod_val(q_modulus) - 1);
      coeffs++;
    }
    q_modulus = Get_next_modulus(q_modulus);
  }

  MODULUS* p_modulus = Get_modulus_head(p_primes);
  int64_t* p_coeffs  = Get_p_coeffs(&poly);
  for (size_t i = 0; i < LIST_LEN(p_primes); i++) {
    for (size_t j = 0; j < Get_degree(); j++) {
      int64_t val = *p_coeffs;
      EXPECT_LT(val, Get_mod_val(p_modulus));
      EXPECT_GE(val, 0);
      EXPECT_TRUE(val == 0 || val == 1 || val == Get_mod_val(p_modulus) - 1);
      p_coeffs++;
    }
    p_modulus = Get_next_modulus(p_modulus);
  }
  Free_poly_data(&poly);
}

TEST_F(TEST_POLYNOMIAL, precompute) {
  size_t degree = Get_degree();
  FMT_ASSERT(degree == Coeff_len, "degree not match for unittest: precompute");
  size_t      num_primes = Get_prime();
  VALUE_LIST* val1       = Alloc_value_list(I64_TYPE, Get_degree());
  Init_i64_value_list(val1, degree, (int64_t*)Coeffs1);

  POLYNOMIAL poly;
  Alloc_poly_data(&poly, Get_degree(), num_primes, 0);
  Transform_value_to_rns_poly(&poly, val1, true);
  Conv_poly2ntt_inplace(&poly);

  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 1);
  VALUE_LIST* precomputed1 = Alloc_precomp(&poly);
  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 0);
  VALUE_LIST* precomputed2 = Alloc_precomp(&poly);
  EXPECT_EQ(LIST_LEN(precomputed1), LIST_LEN(precomputed2));
  FOR_ALL_ELEM(precomputed1, idx) {
    POLYNOMIAL* poly1 = (POLYNOMIAL*)Get_ptr_value_at(precomputed1, idx);
    POLYNOMIAL* poly2 = (POLYNOMIAL*)Get_ptr_value_at(precomputed2, idx);
    EXPECT_EQ(Get_poly_alloc_len(poly1), Get_poly_alloc_len(poly2));
    FOR_ALL_COEFF(poly1, poly_idx) {
      EXPECT_EQ(Get_coeff_at(poly1, poly_idx), Get_coeff_at(poly2, poly_idx));
    }
  }
  Free_precomp(precomputed1);
  Free_precomp(precomputed2);
  Free_poly_data(&poly);
  Free_value_list(val1);
}

TEST_F(TEST_POLYNOMIAL, precompute_intt) {
  size_t degree = Get_degree();
  FMT_ASSERT(degree == Coeff_len, "degree not match for unittest: precompute");
  size_t      num_primes = Get_prime();
  VALUE_LIST* val1       = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(val1, degree, (int64_t*)Coeffs1);

  POLYNOMIAL poly;
  Alloc_poly_data(&poly, degree, num_primes, 0);
  Transform_value_to_rns_poly(&poly, val1, true);

  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 1);
  VALUE_LIST* precomputed1 = Alloc_precomp(&poly);
  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 0);
  VALUE_LIST* precomputed2 = Alloc_precomp(&poly);

  EXPECT_EQ(LIST_LEN(precomputed1), LIST_LEN(precomputed2));
  FOR_ALL_ELEM(precomputed1, idx) {
    POLYNOMIAL* poly1 = (POLYNOMIAL*)Get_ptr_value_at(precomputed1, idx);
    POLYNOMIAL* poly2 = (POLYNOMIAL*)Get_ptr_value_at(precomputed2, idx);
    EXPECT_EQ(Get_poly_alloc_len(poly1), Get_poly_alloc_len(poly2));
    FOR_ALL_COEFF(poly1, poly_idx) {
      EXPECT_EQ(Get_coeff_at(poly1, poly_idx), Get_coeff_at(poly2, poly_idx));
    }
  }
  Free_precomp(precomputed1);
  Free_precomp(precomputed2);
  Free_poly_data(&poly);
  Free_value_list(val1);
}

TEST_F(TEST_POLYNOMIAL, extend_mod_down) {
  size_t degree = Get_degree();
  FMT_ASSERT(degree == Coeff_len, "degree not match for unittest: precompute");
  size_t q_cnt = Get_q_cnt();
  size_t p_cnt = Get_p_cnt();

  // 1. transform value to rns_poly(without p)
  VALUE_LIST* val = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(val, degree, (int64_t*)Coeffs1);
  POLYNOMIAL poly;
  Alloc_poly_data(&poly, degree, q_cnt, 0);
  Transform_value_to_rns_poly(&poly, val, true);

  // 2. extend poly(Q) to poly(Q*P)
  POLYNOMIAL extend_poly;
  Alloc_poly_data(&extend_poly, degree, q_cnt, p_cnt);
  Extend(&extend_poly, &poly);

  // 3. mod down poly(Q*P) to poly(Q)
  POLYNOMIAL res_poly;
  Alloc_poly_data(&res_poly, degree, q_cnt, 0);
  Mod_down(&res_poly, &extend_poly);

  // 4. reconstruct poly to value(without p)
  VALUE_LIST* res_list = Alloc_value_list(BIGINT_TYPE, degree);
  Reconstruct_rns_poly_to_value(res_list, &res_poly);

  // 5. compare with Coeffs1
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), degree, Coeffs1);

  // 6. memory free
  Free_value_list(res_list);
  Free_poly_data(&res_poly);
  Free_poly_data(&extend_poly);
  Free_poly_data(&poly);
  Free_value_list(val);
}
