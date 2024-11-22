//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rt_config.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "util/crt.h"
#include "util/polynomial.h"

const size_t  Coeff_len  = 4;
const int64_t Coeffs1[4] = {0, 1, 4, 5};
const int64_t Coeffs2[4] = {1, 2, 4, 3};

class TEST_POLYNOMIAL : public ::testing::Test {
protected:
  void SetUp() override {
    _degree     = 4;
    _num_primes = 4;
    _crt        = Alloc_crtcontext();
    Init_crtcontext(_crt, HE_STD_NOT_SET, _degree, _num_primes - 1, 1);
  }

  size_t       Get_prime() { return _num_primes; }
  size_t       Get_degree() { return _degree; }
  CRT_CONTEXT* Get_crt() { return _crt; }

  void TearDown() override { Free_crtcontext(_crt); }

private:
  size_t       _degree;
  size_t       _num_primes;
  CRT_CONTEXT* _crt;
};

TEST_F(TEST_POLYNOMIAL, sub_poly) {
  CRT_CONTEXT* crt        = Get_crt();
  size_t       degree     = Get_degree();
  size_t       num_primes = Get_prime();
  VALUE_LIST*  res_list   = Alloc_value_list(BIGINT_TYPE, degree);
  POLYNOMIAL   poly_diff, int_poly1, int_poly2;
  Alloc_poly_data(&poly_diff, degree, num_primes, 0);
  Alloc_poly_data(&int_poly1, degree, num_primes, 0);
  Alloc_poly_data(&int_poly2, degree, num_primes, 0);
  VALUE_LIST* coeff1_list = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST* coeff2_list = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(coeff1_list, Coeff_len, (int64_t*)Coeffs1);
  Init_i64_value_list(coeff2_list, Coeff_len, (int64_t*)Coeffs2);

  Transform_values_to_qbase(&int_poly1, crt, coeff1_list, TRUE);
  Transform_values_to_qbase(&int_poly2, crt, coeff2_list, TRUE);
  Free_value_list(coeff1_list);
  Free_value_list(coeff2_list);

  Sub_poly(&poly_diff, &int_poly1, &int_poly2, crt, NULL);
  Reconstruct_qbase_to_values(res_list, &poly_diff, crt);

  int64_t expect_diff[] = {-1, -1, 0, 2};
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), Coeff_len, expect_diff);

  Free_value_list(res_list);
  Free_poly_data(&poly_diff);
}

TEST_F(TEST_POLYNOMIAL, scalar_integer_multiply) {
  CRT_CONTEXT* crt          = Get_crt();
  size_t       degree       = Get_degree();
  size_t       num_primes   = Get_prime();
  size_t       num_p_primes = Get_crt_num_p(crt);
  int64_t      coeffs[4]    = {1, 2, 0, 2};
  POLYNOMIAL   poly, res;
  Alloc_poly_data(&poly, degree, num_primes, num_p_primes);
  Alloc_poly_data(&res, degree, num_primes, num_p_primes);
  VALUE_LIST* coeffs_list = Alloc_value_list(I64_TYPE, degree);
  Init_i64_value_list(coeffs_list, Coeff_len, coeffs);
  Transform_values_to_qpbase(&poly, crt, coeffs_list, TRUE);

  BIG_INT scalar_bi;
  BI_INIT_ASSIGN_SI(scalar_bi, 1);
  BI_LSHIFT(scalar_bi, 90);
  VALUE_LIST* rns_scalars =
      Alloc_value_list(I64_TYPE, num_primes + num_p_primes);
  Transform_to_qpcrt(rns_scalars, crt, scalar_bi);
  Scalars_integer_multiply_poly(&res, &poly, rns_scalars, Get_q(crt),
                                Get_p(crt));
  VALUE_LIST* res_list = Alloc_value_list(BIGINT_TYPE, degree);
  Reconstruct_qpbase_to_values(res_list, &res, crt);
  BIG_INT expected[Coeff_len];
  BI_INITS(expected[0], expected[1], expected[2], expected[3]);
  BI_ASSIGN_STR(expected[0],
                "-4234122996226045669535073379756974364316141568040530388511752"
                "88235321080999229169794868007406250890815428620930309280",
                10);
  BI_ASSIGN_STR(expected[1],
                "41403981040940048341563847090540789359302648074027463648667295"
                "9290799317085996429200924615953862068765383785129182913",
                10);
  BI_ASSIGN_STR(expected[2], "0", 10);
  BI_ASSIGN_STR(expected[3],
                "41403981040940048341563847090540789359302648074027463648667295"
                "9290799317085996429200924615953862068765383785129182913",
                10);
  BI_FREES(expected[0], expected[1], expected[2], expected[3]);
  Free_poly_data(&poly);
  Free_poly_data(&res);
  Free_value_list(coeffs_list);
  Free_value_list(res_list);
  Free_value_list(rns_scalars);
  BI_FREES(scalar_bi);
}

TEST_F(TEST_POLYNOMIAL, rotate_poly) {
  int64_t      coeffs[Coeff_len] = {0, 1, 4, 59};
  int32_t      rotation_size     = 3;
  size_t       num_primes        = Get_prime();
  size_t       degree            = 4;
  CRT_CONTEXT* crt               = Alloc_crtcontext();
  Init_crtcontext(crt, HE_STD_NOT_SET, degree, num_primes - 1, 1);
  POLYNOMIAL poly, res;
  Alloc_poly_data(&poly, degree, num_primes, 0);
  Alloc_poly_data(&res, degree, num_primes, 0);
  VALUE_LIST* coeffs_list = Alloc_value_list(I64_TYPE, Coeff_len);
  Init_i64_value_list(coeffs_list, Coeff_len, coeffs);
  Transform_values_to_qbase(&poly, crt, coeffs_list, TRUE);

  Rotate_poly_with_rotation_idx(&res, &poly, rotation_size, crt);

  VALUE_LIST* res_list = Alloc_value_list(BIGINT_TYPE, degree);
  Reconstruct_qbase_to_values(res_list, &res, crt);
  int64_t expected1[] = {0, -1, 4, -59};
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), Coeff_len, expected1);

  rotation_size = 6;
  Rotate_poly_with_rotation_idx(&res, &poly, rotation_size, crt);
  Reconstruct_qbase_to_values(res_list, &res, crt);
  CHECK_BIG_INT_COEFFS(Get_bint_values(res_list), Coeff_len, coeffs);

  Free_poly_data(&poly);
  Free_poly_data(&res);
  Free_value_list(coeffs_list);
  Free_value_list(res_list);
  Free_crtcontext(crt);
}

TEST_F(TEST_POLYNOMIAL, add_poly_int) {
  size_t       num_primes = Get_prime();
  CRT_CONTEXT* crt        = Alloc_crtcontext();
  Init_crtcontext(crt, HE_STD_NOT_SET, 4, num_primes - 1, 1);
  VALUE_LIST* val1 = Alloc_value_list(I64_TYPE, 4);
  VALUE_LIST* val2 = Alloc_value_list(I64_TYPE, 4);
  VALUE_LIST* val3 = Alloc_value_list(BIGINT_TYPE, 4);
  Init_i64_value_list(val1, 4, (int64_t*)Coeffs1);
  Init_i64_value_list(val2, 4, (int64_t*)Coeffs2);

  POLYNOMIAL poly_sum, int_poly1, int_poly2;
  Alloc_poly_data(&poly_sum, 4, 4, 0);
  Alloc_poly_data(&int_poly1, 4, 4, 0);
  Alloc_poly_data(&int_poly2, 4, 4, 0);

  Transform_values_to_qbase(&int_poly1, crt, val1, TRUE);
  Transform_values_to_qbase(&int_poly2, crt, val2, TRUE);

  Add_poly(&poly_sum, &int_poly1, &int_poly2, crt, NULL);
  Reconstruct_qbase_to_values(val3, &poly_sum, crt);
  int64_t expect_sum[] = {1, 3, 8, 8};
  CHECK_BIG_INT_COEFFS(Get_bint_values(val3), 4, expect_sum);
  Free_poly_data(&poly_sum);
  Free_poly_data(&int_poly1);
  Free_poly_data(&int_poly2);
  Free_value_list(val1);
  Free_value_list(val2);
  Free_value_list(val3);
  Free_crtcontext(crt);
}

TEST_F(TEST_POLYNOMIAL, Multiply_poly_fast) {
  CRT_CONTEXT* crt        = Alloc_crtcontext();
  size_t       num_primes = Get_prime();

  Init_crtcontext(crt, HE_STD_NOT_SET, 4, num_primes - 1, 1);
  VALUE_LIST* val1 = Alloc_value_list(I64_TYPE, 4);
  VALUE_LIST* val2 = Alloc_value_list(I64_TYPE, 4);
  VALUE_LIST* val3 = Alloc_value_list(BIGINT_TYPE, 4);
  VALUE_LIST* val4 = Alloc_value_list(BIGINT_TYPE, 4);
  Init_i64_value_list(val1, 4, (int64_t*)Coeffs1);
  Init_i64_value_list(val2, 4, (int64_t*)Coeffs2);

  POLYNOMIAL poly_prod1, poly_prod2, int_poly1, int_poly2;
  Alloc_poly_data(&poly_prod1, 4, 4, 0);
  Alloc_poly_data(&poly_prod2, 4, 4, 0);
  Alloc_poly_data(&int_poly1, 4, 4, 0);
  Alloc_poly_data(&int_poly2, 4, 4, 0);

  Transform_values_to_qbase(&int_poly1, crt, val1, TRUE);
  Transform_values_to_qbase(&int_poly2, crt, val2, TRUE);

  Multiply_poly_fast(&poly_prod1, &int_poly1, &int_poly2, crt, NULL);
  Multiply_poly_fast(&poly_prod2, &int_poly2, &int_poly1, crt, NULL);
  Conv_ntt2poly_inplace(&poly_prod1, crt);
  Conv_ntt2poly_inplace(&poly_prod2, crt);
  Reconstruct_qbase_to_values(val3, &poly_prod1, crt);
  Reconstruct_qbase_to_values(val4, &poly_prod2, crt);
  int64_t expected[16] = {-29, -31, -9, 17};
  CHECK_BIG_INT_COEFFS(Get_bint_values(val3), 4, expected);
  CHECK_BIG_INT_COEFFS(Get_bint_values(val4), 4, expected);

  Free_poly_data(&poly_prod1);
  Free_poly_data(&poly_prod2);
  Free_poly_data(&int_poly1);
  Free_poly_data(&int_poly2);
  Free_value_list(val1);
  Free_value_list(val2);
  Free_value_list(val3);
  Free_value_list(val4);
  Free_crtcontext(crt);
}

TEST_F(TEST_POLYNOMIAL, mod_small_poly) {
  CRT_CONTEXT* crt        = Alloc_crtcontext();
  size_t       num_primes = Get_prime();
  uint64_t     mod        = 60;
  BIG_INT      half_mod;
  BI_INIT_ASSIGN_SI(half_mod, mod / 2);

  Init_crtcontext(crt, HE_STD_NOT_SET, 4, num_primes - 1, 1);

  int64_t    coeffs[4] = {-1, 3, 78, 10999};
  POLYNOMIAL poly, res;
  Alloc_poly_data(&poly, 4, 4, 0);
  Alloc_poly_data(&res, 4, 4, 0);
  VALUE_LIST* vals = Alloc_value_list(I64_TYPE, 4);
  Init_i64_value_list(vals, 4, coeffs);
  Transform_values_to_qbase(&poly, crt, vals, TRUE);

  VALUE_LIST* combined = Alloc_value_list(BIGINT_TYPE, 4);
  Reconstruct_qbase_to_values(combined, &poly, crt);
  FOR_ALL_ELEM(combined, idx) {
    BIG_INT* bi = Get_bint_value_at(combined, idx);
    BI_MOD_UI(*bi, *bi, mod);
    if (BI_CMP(*bi, half_mod) > 0) {
      BI_SUB_UI(*bi, *bi, mod);
    }
  }
  int64_t expected[] = {-1, 3, 18, 19};
  CHECK_BIG_INT_COEFFS(Get_bint_values(combined), 4, expected);
  Free_crtcontext(crt);
  Free_poly_data(&poly);
  Free_poly_data(&res);
  Free_value_list(combined);
  Free_value_list(vals);

  BI_FREES(half_mod);
}

TEST_F(TEST_POLYNOMIAL, crt_convert) {
  CRT_CONTEXT* crt       = Get_crt();
  size_t       degree    = Get_degree();
  VALUE_LIST*  val       = Alloc_value_list(I64_TYPE, 4);
  VALUE_LIST*  res       = Alloc_value_list(BIGINT_TYPE, 4);
  int64_t      values[4] = {3, -4, 5, 8};
  POLYNOMIAL   poly;
  Init_i64_value_list(val, 4, values);
  Alloc_poly_data(&poly, degree, Get_primes_cnt(Get_q(crt)),
                  Get_primes_cnt(Get_p(crt)));
  Transform_values_to_qpbase(&poly, crt, val, TRUE);
  Reconstruct_qpbase_to_values(res, &poly, crt);
  CHECK_BIG_INT_COEFFS(Get_bint_values(res), 4, values);
  Free_value_list(val);
  Free_value_list(res);
  Free_poly_data(&poly);
}

TEST_F(TEST_POLYNOMIAL, conv_poly_and_ntt) {
  VALUE_LIST q_primes;
  int64_t    q_data[1] = {0xffffffffffc0001ULL};
  Init_i64_value_list_no_copy(&q_primes, 1, q_data);
  CRT_CONTEXT* test_crt = Alloc_crtcontext();
  Create_crt_with_primes(test_crt, 4, &q_primes, &q_primes);
  int64_t    data[4]     = {2, 4, 9, 15};
  int64_t    expected[4] = {2, 4, 9, 15};
  POLYNOMIAL poly;
  Init_poly_data(&poly, 4, 1, 0, data);
  Conv_poly2ntt_inplace(&poly, test_crt);
  Conv_ntt2poly_inplace(&poly, test_crt);
  CHECK_INT_COEFFS(Get_poly_coeffs(&poly), 4, expected);
}

TEST_F(TEST_POLYNOMIAL, sample_uniform_poly) {
  POLYNOMIAL   poly;
  VL_CRTPRIME* q_primes = Get_q_primes(Get_crt());
  VL_CRTPRIME* p_primes = Get_p_primes(Get_crt());
  Alloc_poly_data(&poly, Get_degree(), LIST_LEN(q_primes), LIST_LEN(p_primes));
  Sample_uniform_poly(&poly, q_primes, p_primes);
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
}

TEST_F(TEST_POLYNOMIAL, Sample_ternary_poly) {
  POLYNOMIAL   poly;
  VL_CRTPRIME* q_primes = Get_q_primes(Get_crt());
  VL_CRTPRIME* p_primes = Get_p_primes(Get_crt());
  Alloc_poly_data(&poly, Get_degree(), LIST_LEN(q_primes), LIST_LEN(p_primes));
  Sample_ternary_poly(&poly, q_primes, p_primes, 0);
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
}

TEST_F(TEST_POLYNOMIAL, precompute) {
  size_t       num_primes = 10;
  CRT_CONTEXT* crt        = Alloc_crtcontext();
  Init_crtcontext_with_prime_size(crt, HE_STD_NOT_SET, Get_degree(), num_primes,
                                  60, 59, 3);
  VALUE_LIST* val1 = Alloc_value_list(I64_TYPE, Get_degree());
  Init_i64_value_list(val1, 4, (int64_t*)Coeffs1);

  POLYNOMIAL poly;
  Alloc_poly_data(&poly, Get_degree(), num_primes, 0);
  Transform_values_to_qbase(&poly, crt, val1, TRUE);
  Conv_poly2ntt_inplace(&poly, crt);

  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 1);
  VALUE_LIST* precomputed1 = Switch_key_precompute(&poly, crt);
  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 0);
  VALUE_LIST* precomputed2 = Switch_key_precompute(&poly, crt);
  EXPECT_EQ(LIST_LEN(precomputed1), LIST_LEN(precomputed2));
  FOR_ALL_ELEM(precomputed1, idx) {
    POLYNOMIAL* poly1 = (POLYNOMIAL*)Get_ptr_value_at(precomputed1, idx);
    POLYNOMIAL* poly2 = (POLYNOMIAL*)Get_ptr_value_at(precomputed2, idx);
    EXPECT_EQ(Get_poly_alloc_len(poly1), Get_poly_alloc_len(poly2));
    FOR_ALL_COEFF(poly1, poly_idx) {
      EXPECT_EQ(Get_coeff_at(poly1, poly_idx), Get_coeff_at(poly2, poly_idx));
    }
  }
  Free_poly_list(precomputed1);
  Free_poly_list(precomputed2);
  Free_crtcontext(crt);
}

TEST_F(TEST_POLYNOMIAL, precompute_intt) {
  size_t       num_primes = 10;
  CRT_CONTEXT* crt        = Alloc_crtcontext();
  Init_crtcontext_with_prime_size(crt, HE_STD_NOT_SET, Get_degree(), num_primes,
                                  60, 59, 3);
  VALUE_LIST* val1 = Alloc_value_list(I64_TYPE, Get_degree());
  Init_i64_value_list(val1, 4, (int64_t*)Coeffs1);

  POLYNOMIAL poly;
  Alloc_poly_data(&poly, Get_degree(), num_primes, 0);
  Transform_values_to_qbase(&poly, crt, val1, TRUE);

  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 1);
  VALUE_LIST* precomputed1 = Switch_key_precompute(&poly, crt);
  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 0);
  VALUE_LIST* precomputed2 = Switch_key_precompute(&poly, crt);

  EXPECT_EQ(LIST_LEN(precomputed1), LIST_LEN(precomputed2));
  FOR_ALL_ELEM(precomputed1, idx) {
    POLYNOMIAL* poly1 = (POLYNOMIAL*)Get_ptr_value_at(precomputed1, idx);
    POLYNOMIAL* poly2 = (POLYNOMIAL*)Get_ptr_value_at(precomputed2, idx);
    EXPECT_EQ(Get_poly_alloc_len(poly1), Get_poly_alloc_len(poly2));
    FOR_ALL_COEFF(poly1, poly_idx) {
      EXPECT_EQ(Get_coeff_at(poly1, poly_idx), Get_coeff_at(poly2, poly_idx));
    }
  }
  Free_poly_list(precomputed1);
  Free_poly_list(precomputed2);
  Free_crtcontext(crt);
}