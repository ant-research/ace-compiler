//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <chrono>

#include "common/rt_config.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "util/ciphertext.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_encoder.h"
#include "util/ckks_encryptor.h"
#include "util/ckks_evaluator.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"
#include "util/plaintext.h"
#include "util/random_sample.h"

using namespace std;
using namespace chrono;
using namespace testing;

// Need to run test with a command line argument with for the polynomial degree.
//   TEST_DEGREE=16 ./c_fhe_all_tests --gtest_filter= TEST_POLY_PERF*
class TEST_POLY_PERF : public ::testing::Test {
protected:
  void SetUp() override {
    _degree         = UT_GLOB_ENV::Get_env(TEST_DEGREE);
    size_t parts    = UT_GLOB_ENV::Get_env(NUM_Q_PART);
    size_t num_q    = UT_GLOB_ENV::Get_env(NUM_Q);
    _cur_q          = UT_GLOB_ENV::Get_env(CUR_Q);
    _num_iterations = UT_GLOB_ENV::Get_env(ITERATION);
    _param          = Alloc_ckks_parameter();
    Set_num_q_parts(_param, parts);
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, num_q,
                                         UT_GLOB_ENV::Get_env(Q0_BITS),
                                         UT_GLOB_ENV::Get_env(SF_BITS), 0);
    cout << "Degree = " << _degree << " log(q0) = " << _param->_first_mod_size
         << " log(scaling_factor) = " << log2(Get_param_sc(_param))
         << " num_q = " << _param->_num_primes << " cur_q =" << _cur_q
         << " num_p = " << _param->_num_p_primes
         << " q_part = " << _param->_num_q_parts << endl;
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _evaluator = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
    _relin_key = _keygen->_relin_key;
  }

  void TearDown() override {
    // RTLIB_TM_REPORT();
    Free_ckks_evaluator(_evaluator);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_encoder(_encoder);
    Free_ckks_key_generator(_keygen);
    Free_ckks_parameters(_param);
  }

  size_t Get_degree() { return _degree; }
  size_t Get_num_iter() { return _num_iterations; }

  POLYNOMIAL* Raise_rns_base(POLYNOMIAL* res, POLYNOMIAL* poly) {
    CRT_CONTEXT* crt = Get_param_crt(_param);
    if (Is_ntt(poly)) {
      Conv_ntt2poly_inplace(poly, crt);
    }
    CRT_PRIMES* q_primes = Get_q(crt);
    CRT_PRIMES* p_primes = Get_p(crt);
    size_t      p_len    = Get_primes_cnt(p_primes);
    memcpy(Get_poly_coeffs(res), Get_poly_coeffs(poly),
           sizeof(int64_t) * Get_num_q(poly) * _degree);

    POLYNOMIAL new_base_poly;
    Extract_poly(&new_base_poly, res, Get_num_q(poly), p_len);

    Fast_base_conv(&new_base_poly, poly, p_primes, q_primes);

    Conv_poly2ntt_inplace(res, crt);
    return res;
  }

  microseconds Run_test_add(VALUE_LIST* msg1, VALUE_LIST* msg2, bool is_ext) {
    size_t       msg1_len = LIST_LEN(msg1);
    PLAINTEXT*   plain1   = Alloc_plaintext();
    PLAINTEXT*   plain2   = Alloc_plaintext();
    CIPHERTEXT*  ciph1    = Alloc_ciphertext();
    CIPHERTEXT*  ciph2    = Alloc_ciphertext();
    CRT_CONTEXT* crt      = Get_param_crt(_param);
    ENCODE_AT_LEVEL(plain1, _encoder, msg1, _cur_q, _degree / 2);
    ENCODE_AT_LEVEL(plain2, _encoder, msg2, _cur_q, _degree / 2);
    Encrypt_msg(ciph1, _encryptor, plain1);
    Encrypt_msg(ciph2, _encryptor, plain2);

    // extend to QP modulus
    POLYNOMIAL poly1_qp, poly2_qp;
    Alloc_poly_data(&poly1_qp, _degree, Get_ciph_prime_cnt(ciph1),
                    Get_primes_cnt(Get_p(crt)));
    Alloc_poly_data(&poly2_qp, _degree, Get_ciph_prime_cnt(ciph2),
                    Get_primes_cnt(Get_p(crt)));
    Raise_rns_base(&poly1_qp, Get_c0(ciph1));
    Raise_rns_base(&poly2_qp, Get_c0(ciph2));

    POLYNOMIAL res = {._data = NULL};
    Init_poly(&res, &poly1_qp);

    microseconds ret(0);
    if (is_ext) {
      auto start = chrono::system_clock::now();
      Add_poly(&res, &poly1_qp, &poly2_qp, crt, Get_p_primes(crt));
      auto end = chrono::system_clock::now();
      ret      = duration_cast<microseconds>(end - start);
    } else {
      auto start = chrono::system_clock::now();
      Add_poly(&res, &poly1_qp, &poly2_qp, crt, NULL);
      auto end = chrono::system_clock::now();
      ret      = duration_cast<microseconds>(end - start);
    }

    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph2);
    Free_poly_data(&poly1_qp);
    Free_poly_data(&poly2_qp);
    Free_poly_data(&res);
    return ret;
  }

  microseconds Run_test_sub(VALUE_LIST* msg1, VALUE_LIST* msg2, bool is_ext) {
    CRT_CONTEXT* crt      = Get_param_crt(_param);
    size_t       msg1_len = LIST_LEN(msg1);
    PLAINTEXT*   plain1   = Alloc_plaintext();
    PLAINTEXT*   plain2   = Alloc_plaintext();
    CIPHERTEXT*  ciph1    = Alloc_ciphertext();
    CIPHERTEXT*  ciph2    = Alloc_ciphertext();
    ENCODE_AT_LEVEL(plain1, _encoder, msg1, _cur_q, _degree / 2);
    ENCODE_AT_LEVEL(plain2, _encoder, msg2, _cur_q, _degree / 2);
    Encrypt_msg(ciph1, _encryptor, plain1);
    Encrypt_msg(ciph2, _encryptor, plain2);

    // extend to QP modulus
    POLYNOMIAL poly1_qp, poly2_qp;
    Alloc_poly_data(&poly1_qp, _degree, Get_ciph_prime_cnt(ciph1),
                    Get_primes_cnt(Get_p(crt)));
    Alloc_poly_data(&poly2_qp, _degree, Get_ciph_prime_cnt(ciph2),
                    Get_primes_cnt(Get_p(crt)));
    Raise_rns_base(&poly1_qp, Get_c0(ciph1));
    Raise_rns_base(&poly2_qp, Get_c0(ciph2));

    POLYNOMIAL res = {._data = NULL};
    Init_poly(&res, &poly1_qp);

    microseconds ret(0);
    if (is_ext) {
      auto start = chrono::system_clock::now();
      Sub_poly(&res, &poly1_qp, &poly2_qp, crt, Get_p_primes(crt));
      auto end = chrono::system_clock::now();
      ret      = duration_cast<microseconds>(end - start);
    } else {
      auto start = chrono::system_clock::now();
      Sub_poly(&res, &poly1_qp, &poly2_qp, crt, NULL);
      auto end = chrono::system_clock::now();
      ret      = duration_cast<microseconds>(end - start);
    }

    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph2);
    Free_poly_data(&poly1_qp);
    Free_poly_data(&poly2_qp);
    Free_poly_data(&res);
    return ret;
  }

  microseconds Run_test_mul(VALUE_LIST* msg1, VALUE_LIST* msg2, bool is_ext) {
    CRT_CONTEXT* crt      = Get_param_crt(_param);
    size_t       msg1_len = LIST_LEN(msg1);
    PLAINTEXT*   plain1   = Alloc_plaintext();
    PLAINTEXT*   plain2   = Alloc_plaintext();
    CIPHERTEXT*  ciph1    = Alloc_ciphertext();
    CIPHERTEXT*  ciph2    = Alloc_ciphertext();
    ENCODE_AT_LEVEL(plain1, _encoder, msg1, _cur_q, _degree / 2);
    ENCODE_AT_LEVEL(plain2, _encoder, msg2, _cur_q, _degree / 2);
    Encrypt_msg(ciph1, _encryptor, plain1);
    Encrypt_msg(ciph2, _encryptor, plain2);

    // extend to QP modulus
    POLYNOMIAL poly1_qp, poly2_qp;
    Alloc_poly_data(&poly1_qp, _degree, Get_ciph_prime_cnt(ciph1),
                    Get_primes_cnt(Get_p(crt)));
    Alloc_poly_data(&poly2_qp, _degree, Get_ciph_prime_cnt(ciph2),
                    Get_primes_cnt(Get_p(crt)));
    Raise_rns_base(&poly1_qp, Get_c0(ciph1));
    Raise_rns_base(&poly2_qp, Get_c0(ciph2));

    POLYNOMIAL res = {._data = NULL};
    Init_poly(&res, &poly1_qp);

    microseconds ret(0);
    if (is_ext) {
      auto start = chrono::system_clock::now();
      Multiply_poly_fast(&res, &poly1_qp, &poly2_qp, crt, Get_p_primes(crt));
      auto end = chrono::system_clock::now();
      ret      = duration_cast<microseconds>(end - start);
    } else {
      auto start = chrono::system_clock::now();
      Multiply_poly_fast(&res, &poly1_qp, &poly2_qp, crt, NULL);
      auto end = chrono::system_clock::now();
      ret      = duration_cast<microseconds>(end - start);
    }
    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph2);
    Free_poly_data(&poly1_qp);
    Free_poly_data(&poly2_qp);
    Free_poly_data(&res);
    return ret;
  }

  microseconds Run_test_rotate(VALUE_LIST* vec, int32_t rot, bool is_ext) {
    CRT_CONTEXT* crt        = Get_param_crt(_param);
    size_t       length     = _degree / 2;
    size_t       vec_length = LIST_LEN(vec);

    PLAINTEXT*  plain   = Alloc_plaintext();
    CIPHERTEXT* ciph    = Alloc_ciphertext();
    SWITCH_KEY* rot_key = Alloc_switch_key();
    ENCODE_AT_LEVEL(plain, _encoder, vec, _cur_q, _degree / 2);
    Encrypt_msg(ciph, _encryptor, plain);

    // generate precompute automorphism index
    Insert_rot_map(_keygen, rot);
    MODULUS two_n_modulus;
    Init_modulus(&two_n_modulus, 2 * _degree);
    uint32_t    k       = Find_automorphism_index(rot, &two_n_modulus);
    VALUE_LIST* precomp = Get_precomp_auto_order(_keygen, k);
    IS_TRUE(precomp, "cannot find precomputed automorphism order");

    // extend to QP modulus
    POLYNOMIAL poly1_qp;
    Alloc_poly_data(&poly1_qp, _degree, Get_ciph_prime_cnt(ciph),
                    Get_primes_cnt(Get_p(crt)));
    Raise_rns_base(&poly1_qp, Get_c0(ciph));
    POLYNOMIAL res = {._data = NULL};
    Init_poly(&res, &poly1_qp);

    microseconds ret(0);
    size_t       num_p = Get_num_p(&res);
    if (!is_ext) {
      Set_num_p(&res, 0);
    }
    auto start = chrono::system_clock::now();
    Rotate_poly(&res, &poly1_qp, k, precomp, _param->_crt_context);
    auto end = chrono::system_clock::now();

    if (!is_ext) {
      Set_num_p(&res, num_p);
    }
    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_poly_data(&poly1_qp);
    Free_poly_data(&res);
    return duration_cast<microseconds>(end - start);
  }

  microseconds Run_test_rescale(VALUE_LIST* msg1, VALUE_LIST* msg2) {
    size_t      msg1_len  = LIST_LEN(msg1);
    PLAINTEXT*  plain1    = Alloc_plaintext();
    PLAINTEXT*  plain2    = Alloc_plaintext();
    CIPHERTEXT* ciph1     = Alloc_ciphertext();
    CIPHERTEXT* ciph2     = Alloc_ciphertext();
    CIPHERTEXT* ciph_prod = Alloc_ciphertext();
    ENCODE_AT_LEVEL(plain1, _encoder, msg1, _cur_q, _degree / 2);
    ENCODE_AT_LEVEL(plain2, _encoder, msg2, _cur_q, _degree / 2);
    Encrypt_msg(ciph1, _encryptor, plain1);
    Encrypt_msg(ciph2, _encryptor, plain2);

    Mul_ciphertext(ciph_prod, ciph1, ciph2, _relin_key, _evaluator);

    POLYNOMIAL res = {._data = NULL};
    Init_poly(&res, Get_c0(ciph_prod));

    auto start = chrono::system_clock::now();
    Rescale_poly(&res, Get_c0(ciph_prod), _param->_crt_context);
    auto         end = chrono::system_clock::now();
    microseconds ret(0);
    ret = duration_cast<microseconds>(end - start);

    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph2);
    Free_ciphertext(ciph_prod);
    Free_poly_data(&res);
    return ret;
  }

  microseconds Run_test_precomp(VALUE_LIST* vec) {
    CRT_CONTEXT* crt        = Get_param_crt(_param);
    size_t       length     = _degree / 2;
    size_t       vec_length = LIST_LEN(vec);

    PLAINTEXT*  plain   = Alloc_plaintext();
    CIPHERTEXT* ciph    = Alloc_ciphertext();
    SWITCH_KEY* rot_key = Alloc_switch_key();
    ENCODE_AT_LEVEL(plain, _encoder, vec, _cur_q, _degree / 2);
    Encrypt_msg(ciph, _encryptor, plain);

    microseconds ret(0);
    auto         start = chrono::system_clock::now();
    VALUE_LIST*  precomputed =
        Switch_key_precompute(Get_c1(ciph), _param->_crt_context);
    auto end = chrono::system_clock::now();

    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_switch_key_precomputed(precomputed);
    return duration_cast<microseconds>(end - start);
  }

  microseconds Run_test_dotprod(VALUE_LIST* vec, int32_t rot, bool is_fast) {
    CRT_CONTEXT* crt        = Get_param_crt(_param);
    size_t       length     = _degree / 2;
    size_t       vec_length = LIST_LEN(vec);

    PLAINTEXT*  plain   = Alloc_plaintext();
    CIPHERTEXT* ciph    = Alloc_ciphertext();
    SWITCH_KEY* rot_key = Alloc_switch_key();
    ENCODE_AT_LEVEL(plain, _encoder, vec, _cur_q, _degree / 2);
    Encrypt_msg(ciph, _encryptor, plain);

    // generate precompute automorphism index
    Insert_rot_map(_keygen, rot);
    uint32_t auto_idx = Get_precomp_auto_idx(_keygen, rot);
    IS_TRUE(auto_idx, "cannot get precompute automorphism index");
    rot_key = Get_auto_key(_keygen, auto_idx);

    VALUE_LIST* precomputed =
        Switch_key_precompute(Get_c1(ciph), _param->_crt_context);
    size_t part_size = LIST_LEN(precomputed);
    IS_TRUE(part_size <= Get_swk_size(key), "unmatched size");

    POLYNOMIAL** p0  = (POLYNOMIAL**)Get_ptr_values(precomputed);
    POLYNOMIAL** p1  = (POLYNOMIAL**)malloc((sizeof(POLYNOMIAL*)) * part_size);
    POLYNOMIAL   res = {._data = NULL};
    Init_poly(&res, p0[0]);

    POLYNOMIAL poly[part_size];
    for (uint32_t idx = 0; idx < part_size; idx++) {
      p1[idx] = &poly[idx];
      Derive_poly(&poly[idx], Get_pk0(Get_swk_at(rot_key, idx)),
                  Get_poly_level(&res), Get_num_p(&res));
    }
    microseconds ret(0);
    auto         start = chrono::system_clock::now();
    if (is_fast) {
      Fast_dotprod_poly(&res, p0, p1, Get_q_primes(crt), Get_p_primes(crt),
                        part_size);
    } else {
      Dotprod_poly(&res, p0, p1, Get_q_primes(crt), Get_p_primes(crt),
                   part_size);
    }
    auto end = chrono::system_clock::now();

    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_switch_key_precomputed(precomputed);
    free(p1);
    Free_poly_data(&res);
    return duration_cast<microseconds>(end - start);
  }

  microseconds Run_test_moddown(VALUE_LIST* vec, int32_t rot) {
    CRT_CONTEXT* crt        = Get_param_crt(_param);
    size_t       length     = _degree / 2;
    size_t       vec_length = LIST_LEN(vec);

    PLAINTEXT*  plain    = Alloc_plaintext();
    CIPHERTEXT* ciph     = Alloc_ciphertext();
    CIPHERTEXT* rot_ciph = Alloc_ciphertext();
    SWITCH_KEY* rot_key  = Alloc_switch_key();
    ENCODE_AT_LEVEL(plain, _encoder, vec, _cur_q, _degree / 2);
    Encrypt_msg(ciph, _encryptor, plain);

    // generate precompute automorphism index
    Insert_rot_map(_keygen, rot);
    uint32_t auto_idx = Get_precomp_auto_idx(_keygen, rot);
    IS_TRUE(auto_idx, "cannot get precompute automorphism index");
    rot_key = Get_auto_key(_keygen, auto_idx);

    VALUE_LIST* precomputed =
        Switch_key_precompute(Get_c1(ciph), _param->_crt_context);
    Fast_rotate_ext(rot_ciph, ciph, rot, rot_key, _evaluator, precomputed,
                    false);
    POLYNOMIAL res = {._data = NULL};
    Init_poly(&res, Get_c0(ciph));

    auto start = chrono::system_clock::now();
    Reduce_rns_base(&res, Get_c0(rot_ciph), crt);
    auto end = chrono::system_clock::now();

    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_ciphertext(rot_ciph);
    Free_poly_data(&res);
    return duration_cast<microseconds>(end - start);
  }

  microseconds Run_test_extend(VALUE_LIST* vec) {
    CRT_CONTEXT* crt        = Get_param_crt(_param);
    size_t       length     = _degree / 2;
    size_t       vec_length = LIST_LEN(vec);

    PLAINTEXT*  plain = Alloc_plaintext();
    CIPHERTEXT* ciph  = Alloc_ciphertext();
    ENCODE_AT_LEVEL(plain, _encoder, vec, _cur_q, _degree / 2);
    Encrypt_msg(ciph, _encryptor, plain);

    POLYNOMIAL res = {._data = NULL};
    Alloc_poly_data(&res, _degree, Get_num_q(Get_c0(ciph)), Get_crt_num_p(crt));

    auto start = chrono::system_clock::now();
    Scalars_integer_multiply_poly(&res, Get_c0(ciph), Get_pmodq(Get_p(crt)),
                                  Get_q(crt), NULL);
    auto end = chrono::system_clock::now();

    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_poly_data(&res);
    return duration_cast<microseconds>(end - start);
  }

private:
  size_t              _degree;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_EVALUATOR*     _evaluator;
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
  SWITCH_KEY*         _relin_key;
  size_t              _num_iterations;
  uint32_t            _cur_q;
};

TEST_F(TEST_POLY_PERF, add) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_add(vec1, vec2, false);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Add:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, add_ext) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_add(vec1, vec2, true);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Add_ext:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, sub) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_sub(vec1, vec2, false);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Sub:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, sub_ext) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_sub(vec1, vec2, true);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Sub_ext:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, mul) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_mul(vec1, vec2, false);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Mul:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, mul_ext) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_mul(vec1, vec2, true);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Mul_ext:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, rotate) {
  IS_TRACE_CMD(Print_param(Get_trace_file(), _param));
  size_t       num_iterations = Get_num_iter();
  microseconds total(0);

  size_t      length = Get_degree() / 2;
  VALUE_LIST* dc_vec = Alloc_value_list(DCMPLX_TYPE, length);
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds gen_rot_key_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), length);
    total += Run_test_rotate(dc_vec, i * 20, false);
  }

  Free_value_list(dc_vec);
  cout << string(80, '-') << endl
       << string(80, '-') << endl
       << left << setw(24) << "Rotate:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, rotate_ext) {
  IS_TRACE_CMD(Print_param(Get_trace_file(), _param));
  size_t       num_iterations = Get_num_iter();
  microseconds total(0);

  size_t      length = Get_degree() / 2;
  VALUE_LIST* dc_vec = Alloc_value_list(DCMPLX_TYPE, length);
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds gen_rot_key_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), length);
    total += Run_test_rotate(dc_vec, i * 20, true);
  }

  Free_value_list(dc_vec);
  cout << string(80, '-') << endl
       << string(80, '-') << endl
       << left << setw(24) << "Rotate_ext:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, rescale) {
  microseconds total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST*  vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_rescale(vec1, vec2);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);

  cout << string(80, '-') << endl
       << left << setw(24) << "Rescale:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, precomp) {
  microseconds total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    total += Run_test_precomp(vec1);
  }
  Free_value_list(vec1);

  cout << string(80, '-') << endl
       << left << setw(24) << "Precomp:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, dotprod) {
  microseconds total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    total += Run_test_dotprod(vec1, i * 20, false);
  }
  Free_value_list(vec1);

  cout << string(80, '-') << endl
       << left << setw(24) << "Dotprod:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, fast_dotprod) {
  microseconds total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    total += Run_test_dotprod(vec1, i * 20, true);
  }
  Free_value_list(vec1);

  cout << string(80, '-') << endl
       << left << setw(24) << "Fast_dotprod:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, moddown) {
  microseconds total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    total += Run_test_moddown(vec1, i * 20);
  }
  Free_value_list(vec1);

  cout << string(80, '-') << endl
       << left << setw(24) << "Moddown:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_POLY_PERF, extend) {
  microseconds total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    total += Run_test_extend(vec1);
  }
  Free_value_list(vec1);

  cout << string(80, '-') << endl
       << left << setw(24) << "Extend:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}
