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
#include "util/ckks_bootstrap_context.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_encoder.h"
#include "util/ckks_encryptor.h"
#include "util/ckks_evaluator.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"
#include "util/matrix_operations.h"
#include "util/plaintext.h"
#include "util/random_sample.h"

using namespace std;
using namespace chrono;
using namespace testing;

class TEST_KSW_OPT : public ::testing::Test {
protected:
  void SetUp() override {
    _degree         = UT_GLOB_ENV::Get_env(TEST_DEGREE);
    size_t parts    = UT_GLOB_ENV::Get_env(NUM_Q_PART);
    size_t num_q    = UT_GLOB_ENV::Get_env(NUM_Q);
    size_t q0_bits  = UT_GLOB_ENV::Get_env(Q0_BITS);
    size_t sf_bits  = UT_GLOB_ENV::Get_env(SF_BITS);
    _num_iterations = UT_GLOB_ENV::Get_env(ITERATION);
    _param          = Alloc_ckks_parameter();
    Set_num_q_parts(_param, parts);
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, num_q,
                                         q0_bits, sf_bits, 0);
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _evaluator = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
    _relin_key = _keygen->_relin_key;

    cout << "Degree = " << _degree << " log(q0) = " << _param->_first_mod_size
         << " log(scaling_factor) = " << log2(Get_param_sc(_param))
         << " num_q = " << _param->_num_primes
         << " num_p = " << _param->_num_p_primes
         << " q_part = " << _param->_num_q_parts << endl;
  }

  void TearDown() override {
    Free_ckks_evaluator(_evaluator);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_encoder(_encoder);
    Free_ckks_key_generator(_keygen);
    Free_ckks_parameters(_param);
  }

  void Report_time(string title, microseconds time, size_t num_iteration);

  size_t              Get_degree() { return _degree; }
  size_t              Get_num_iter() { return _num_iterations; }
  CRT_CONTEXT*        Get_crt() { return _param->_crt_context; }
  CKKS_ENCODER*       Get_encoder() { return _encoder; }
  CKKS_ENCRYPTOR*     Get_encryptor() { return _encryptor; }
  CKKS_KEY_GENERATOR* Get_keygen() { return _keygen; }
  CKKS_EVALUATOR*     Get_eval() { return _evaluator; }
  CKKS_DECRYPTOR*     Get_decryptor() { return _decryptor; }
  SWITCH_KEY*         Get_relin_key() { return _keygen->_relin_key; }
  double              Get_scaling_factor() { return Get_param_sc(_param); }

  microseconds Run_ksw_modup_hoist_base(VALUE_LIST* vec1, int32_t num_rots);
  microseconds Run_ksw_modup_hoist_opt(VALUE_LIST* vec1, int32_t num_rots);

  microseconds Run_ksw_moddown_hoist_base(int32_t hoist_cnt);
  microseconds Run_ksw_moddown_hoist_opt(int32_t hoist_cnt);

  microseconds Run_ksw_moddown_hoist_base2(int32_t hoist_cnt);
  microseconds Run_ksw_moddown_hoist_opt2(int32_t hoist_cnt);

  microseconds Run_ksw_moddown_rescale_base(VALUE_LIST* vec1, VALUE_LIST* vec2);
  microseconds Run_ksw_moddown_rescale_opt(VALUE_LIST* vec1, VALUE_LIST* vec2);
  microseconds Run_ksw_moddown_rescale_opt2(VALUE_LIST* vec1, VALUE_LIST* vec2);

  microseconds Run_ksw_moddown_rotate_rescale_base(VALUE_LIST* vec1,
                                                   VALUE_LIST* vec2,
                                                   int32_t     rot_idx);
  microseconds Run_ksw_moddown_rotate_rescale_opt(VALUE_LIST* vec1,
                                                  VALUE_LIST* vec2,
                                                  int32_t     rot_idx);

  microseconds Run_ksw_moddown_rescale_modup_base(VALUE_LIST* vec1,
                                                  VALUE_LIST* vec2,
                                                  int32_t rot1, int32_t rot2);
  microseconds Run_ksw_moddown_rescale_modup_opt(VALUE_LIST* vec1,
                                                 VALUE_LIST* vec2, int32_t rot1,
                                                 int32_t rot2);
  microseconds Run_ksw_moddown_rescale_modup_opt2(VALUE_LIST* vec1,
                                                  VALUE_LIST* vec2,
                                                  int32_t rot1, int32_t rot2);
  microseconds Run_ksw_moddown_rescale_modup_opt3(VALUE_LIST* vec1,
                                                  VALUE_LIST* vec2,
                                                  int32_t rot1, int32_t rot2);

  void Print_ciph(CIPHERTEXT* ciph, uint32_t slots);

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
};

void TEST_KSW_OPT::Report_time(string title, microseconds time,
                               size_t iteration) {
  cout << string(80, '-') << endl
       << left << setw(24) << title << right << setw(10)
       << (double)time.count() / iteration / 1000 << " ms" << right << setw(24)
       << "avarage of " << iteration << " runs" << endl
       << string(80, '-') << endl;
}

microseconds TEST_KSW_OPT::Run_ksw_modup_hoist_base(VALUE_LIST* vec,
                                                    int32_t     num_rots) {
  size_t      len       = LIST_LEN(vec);
  size_t      slots     = Get_degree() / 2;
  VALUE_LIST* rot_lists = Alloc_value_list(I64_TYPE, num_rots);
  PLAINTEXT*  plain     = Alloc_plaintext();
  PLAINTEXT*  out_plain = Alloc_plaintext();
  CIPHERTEXT* ciph      = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph  = Alloc_ciphertext();
  CIPHERTEXT* out_ciph  = Alloc_ciphertext();

  VALUE_LIST* decoded_vec = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec  = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain, Get_encoder(), vec);
  Encrypt_msg(ciph, Get_encryptor(), plain);
  Init_ciphertext_from_ciph(out_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);

  Sample_uniform(rot_lists, len);
  // generate rotation keys
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Insert_rot_map(Get_keygen(), rot_idx);
  }

  // out_ciph += \sigma rotate(ciph, idx)
  auto start = chrono::system_clock::now();
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx  = (int32_t)Get_i64_value_at(rot_lists, idx);
    uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
    SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);
    Eval_fast_rotate(rot_ciph, ciph, rot_idx, rot_key, Get_eval());
    Add_ciphertext(out_ciph, rot_ciph, out_ciph, Get_eval());
  }
  auto end = chrono::system_clock::now();

  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    for (size_t idx2 = 0; idx2 < len; idx2++) {
      DCMPLX_VALUE_AT(expect_vec, idx2) +=
          Get_dcmplx_value_at(vec, (idx2 + rot_idx) % slots);
    }
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(rot_lists);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(out_ciph);

  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_modup_hoist_opt(VALUE_LIST* vec,
                                                   int32_t     num_rots) {
  size_t      len       = LIST_LEN(vec);
  size_t      slots     = Get_degree() / 2;
  VALUE_LIST* rot_lists = Alloc_value_list(I64_TYPE, num_rots);
  PLAINTEXT*  plain     = Alloc_plaintext();
  PLAINTEXT*  out_plain = Alloc_plaintext();
  CIPHERTEXT* ciph      = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph  = Alloc_ciphertext();
  CIPHERTEXT* out_ciph  = Alloc_ciphertext();

  VALUE_LIST* decoded_vec = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec  = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain, Get_encoder(), vec);
  Encrypt_msg(ciph, Get_encryptor(), plain);
  Init_ciphertext_from_ciph(out_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);

  Sample_uniform(rot_lists, len);
  // generate rotation keys
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Insert_rot_map(Get_keygen(), rot_idx);
  }

  // out_ciph += \sigma rotate(ciph, idx)
  auto        start       = chrono::system_clock::now();
  VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph), Get_crt());
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx  = (int32_t)Get_i64_value_at(rot_lists, idx);
    uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
    SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);
    Fast_rotate(rot_ciph, ciph, rot_idx, rot_key, Get_eval(), precomputed);
    Add_ciphertext(out_ciph, rot_ciph, out_ciph, Get_eval());
  }
  auto end = chrono::system_clock::now();

  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    for (size_t idx2 = 0; idx2 < len; idx2++) {
      DCMPLX_VALUE_AT(expect_vec, idx2) +=
          Get_dcmplx_value_at(vec, (idx2 + rot_idx) % slots);
    }
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_switch_key_precomputed(precomputed);
  Free_value_list(rot_lists);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_hoist_base(int32_t hoist_cnt) {
  size_t      slots        = Get_degree() / 2;
  PLAINTEXT*  plain        = Alloc_plaintext();
  PLAINTEXT*  out_plain    = Alloc_plaintext();
  CIPHERTEXT* rot_ciph     = Alloc_ciphertext();
  CIPHERTEXT* out_ciph     = Alloc_ciphertext();
  VALUE_LIST* cipher_lists = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* rot_lists    = Alloc_value_list(I64_TYPE, hoist_cnt);
  VALUE_LIST* msg_lists    = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* decoded_vec  = Alloc_value_list(DCMPLX_TYPE, slots);
  VALUE_LIST* expect_vec   = Alloc_value_list(DCMPLX_TYPE, slots);

  Sample_uniform(rot_lists, hoist_cnt);

  for (size_t idx = 0; idx < hoist_cnt; idx++) {
    CIPHERTEXT* ciph_i = Alloc_ciphertext();
    VALUE_LIST* msg    = Alloc_value_list(DCMPLX_TYPE, slots);
    Sample_random_complex_vector(DCMPLX_VALUES(msg), slots);

    ENCODE(plain, Get_encoder(), msg);
    Encrypt_msg(ciph_i, Get_encryptor(), plain);
    Set_ptr_value(cipher_lists, idx, (PTR)ciph_i);
    Set_ptr_value(msg_lists, idx, (PTR)msg);
  }

  // generate rotation keys
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Insert_rot_map(Get_keygen(), rot_idx);
  }

  CIPHERTEXT* ciph = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, 0);
  Init_ciphertext_from_ciph(out_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  // out_ciph += \sigma rotate(ciph_i, idx)
  auto start = chrono::system_clock::now();
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx  = (int32_t)Get_i64_value_at(rot_lists, idx);
    uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
    SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);
    Eval_fast_rotate(rot_ciph, (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx),
                     rot_idx, rot_key, Get_eval());
    Add_ciphertext(out_ciph, rot_ciph, out_ciph, Get_eval());
  }

  auto end = chrono::system_clock::now();

  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    VALUE_LIST* vec     = (VALUE_LIST*)Get_ptr_value_at(msg_lists, idx);
    for (size_t idx2 = 0; idx2 < slots; idx2++) {
      DCMPLX_VALUE_AT(expect_vec, idx2) +=
          Get_dcmplx_value_at(vec, (idx2 + rot_idx) % slots);
    }
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  FOR_ALL_ELEM(cipher_lists, idx) {
    Free_ciphertext((CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx));
  }
  FOR_ALL_ELEM(msg_lists, idx) {
    Free_value_list((VALUE_LIST*)Get_ptr_value_at(msg_lists, idx));
  }
  Free_value_list(cipher_lists);
  Free_value_list(msg_lists);
  Free_value_list(rot_lists);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_hoist_opt(int32_t hoist_cnt) {
  size_t      slots        = Get_degree() / 2;
  PLAINTEXT*  plain        = Alloc_plaintext();
  PLAINTEXT*  out_plain    = Alloc_plaintext();
  CIPHERTEXT* rot_ciph     = Alloc_ciphertext();
  CIPHERTEXT* sum_ciph     = Alloc_ciphertext();
  CIPHERTEXT* out_ciph     = Alloc_ciphertext();
  VALUE_LIST* cipher_lists = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* rot_lists    = Alloc_value_list(I64_TYPE, hoist_cnt);
  VALUE_LIST* msg_lists    = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* decoded_vec  = Alloc_value_list(DCMPLX_TYPE, slots);
  VALUE_LIST* expect_vec   = Alloc_value_list(DCMPLX_TYPE, slots);

  Sample_uniform(rot_lists, hoist_cnt);

  for (size_t idx = 0; idx < hoist_cnt; idx++) {
    CIPHERTEXT* ciph_i = Alloc_ciphertext();
    VALUE_LIST* msg    = Alloc_value_list(DCMPLX_TYPE, slots);
    Sample_random_complex_vector(DCMPLX_VALUES(msg), slots);

    ENCODE(plain, Get_encoder(), msg);
    Encrypt_msg(ciph_i, Get_encryptor(), plain);
    Set_ptr_value(cipher_lists, idx, (PTR)ciph_i);
    Set_ptr_value(msg_lists, idx, (PTR)msg);
  }

  // generate rotation keys
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Insert_rot_map(Get_keygen(), rot_idx);
  }

  // out_ciph += \sigma rotate(ciph_i, idx)
  auto start = chrono::system_clock::now();
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx  = (int32_t)Get_i64_value_at(rot_lists, idx);
    uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
    SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);
    CIPHERTEXT* ciph_i   = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx);

    VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph_i), Get_crt());
    Fast_rotate_ext(rot_ciph, ciph_i, rot_idx, rot_key, _evaluator, precomputed,
                    true);
    if (idx == 0) {
      Init_ciphertext_from_ciph(sum_ciph, rot_ciph, rot_ciph->_scaling_factor,
                                rot_ciph->_sf_degree);
    }
    // add at QP modulus
    Add_ciphertext(sum_ciph, sum_ciph, rot_ciph, Get_eval());
    Free_switch_key_precomputed(precomputed);
  }

  // moddown hoisted
  CIPHERTEXT* ciph = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, 0);
  Init_ciphertext_from_ciph(out_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  Reduce_rns_base(Get_c0(out_ciph), Get_c0(sum_ciph), Get_crt());
  Reduce_rns_base(Get_c1(out_ciph), Get_c1(sum_ciph), Get_crt());

  auto end = chrono::system_clock::now();

  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    VALUE_LIST* vec     = (VALUE_LIST*)Get_ptr_value_at(msg_lists, idx);
    for (size_t idx2 = 0; idx2 < slots; idx2++) {
      DCMPLX_VALUE_AT(expect_vec, idx2) +=
          Get_dcmplx_value_at(vec, (idx2 + rot_idx) % slots);
    }
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  FOR_ALL_ELEM(cipher_lists, idx) {
    Free_ciphertext((CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx));
  }
  FOR_ALL_ELEM(msg_lists, idx) {
    Free_value_list((VALUE_LIST*)Get_ptr_value_at(msg_lists, idx));
  }
  Free_value_list(cipher_lists);
  Free_value_list(msg_lists);
  Free_value_list(rot_lists);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(sum_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

void TEST_KSW_OPT::Print_ciph(CIPHERTEXT* ciph, uint32_t slots) {
  VALUE_LIST* decoded_vec = Alloc_value_list(DCMPLX_TYPE, slots);
  PLAINTEXT*  plain       = Alloc_plaintext();
  Decrypt(plain, Get_decryptor(), ciph, NULL);
  Decode(decoded_vec, Get_encoder(), plain);
  Print_value_list(stdout, decoded_vec);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_hoist_base2(int32_t hoist_cnt) {
  size_t      slots           = Get_degree() / 2;
  PLAINTEXT*  plain           = Alloc_plaintext();
  PLAINTEXT*  out_plain       = Alloc_plaintext();
  CIPHERTEXT* rot_ciph        = Alloc_ciphertext();
  CIPHERTEXT* sum_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph        = Alloc_ciphertext();
  CIPHERTEXT* out_ciph        = Alloc_ciphertext();
  VALUE_LIST* cipher_lists    = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* plain_lists     = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* rot_lists       = Alloc_value_list(I64_TYPE, hoist_cnt);
  VALUE_LIST* msg_lists       = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* msg_plain_lists = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* decoded_vec     = Alloc_value_list(DCMPLX_TYPE, slots);
  VALUE_LIST* expect_vec      = Alloc_value_list(DCMPLX_TYPE, slots);

  Sample_uniform(rot_lists, slots);

  for (size_t idx = 0; idx < hoist_cnt; idx++) {
    PLAINTEXT*  plain2 = Alloc_plaintext();
    CIPHERTEXT* ciph_i = Alloc_ciphertext();
    VALUE_LIST* msg    = Alloc_value_list(DCMPLX_TYPE, slots);
    Sample_random_complex_vector(DCMPLX_VALUES(msg), slots);

    ENCODE(plain, Get_encoder(), msg);
    Encrypt_msg(ciph_i, Get_encryptor(), plain);
    Set_ptr_value(cipher_lists, idx, (PTR)ciph_i);
    Set_ptr_value(msg_lists, idx, (PTR)msg);

    VALUE_LIST* msg_plain = Alloc_value_list(DCMPLX_TYPE, slots);
    Sample_random_complex_vector(DCMPLX_VALUES(msg_plain), slots);
    ENCODE(plain2, Get_encoder(), msg_plain);
    Set_ptr_value(plain_lists, idx, (PTR)plain2);
    Set_ptr_value(msg_plain_lists, idx, (PTR)msg_plain);
  }

  // generate rotation keys
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Insert_rot_map(Get_keygen(), rot_idx);
  }

  // out_ciph += \sigma rotate(ciph_i, idx) * plain_i
  auto start = chrono::system_clock::now();
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx  = (int32_t)Get_i64_value_at(rot_lists, idx);
    uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
    SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);
    CIPHERTEXT* ciph_i   = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx);
    PLAINTEXT*  plain_i  = (PLAINTEXT*)Get_ptr_value_at(plain_lists, idx);

    Eval_fast_rotate(rot_ciph, ciph_i, rot_idx, rot_key, Get_eval());
    Mul_plaintext(mul_ciph, rot_ciph, plain_i, Get_eval());

    if (idx == 0) {
      Init_ciphertext_from_ciph(sum_ciph, mul_ciph, mul_ciph->_scaling_factor,
                                mul_ciph->_sf_degree);
    }
    Add_ciphertext(sum_ciph, sum_ciph, mul_ciph, Get_eval());
  }

  auto end = chrono::system_clock::now();

  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx   = (int32_t)Get_i64_value_at(rot_lists, idx);
    VALUE_LIST* vec       = (VALUE_LIST*)Get_ptr_value_at(msg_lists, idx);
    VALUE_LIST* vec_plain = (VALUE_LIST*)Get_ptr_value_at(msg_plain_lists, idx);
    for (size_t idx2 = 0; idx2 < slots; idx2++) {
      DCMPLX_VALUE_AT(expect_vec, idx2) +=
          Get_dcmplx_value_at(vec, (idx2 + rot_idx) % slots) *
          Get_dcmplx_value_at(vec_plain, idx2);
    }
  }

  Decrypt(out_plain, Get_decryptor(), sum_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  FOR_ALL_ELEM(cipher_lists, idx) {
    Free_ciphertext((CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx));
  }
  FOR_ALL_ELEM(msg_lists, idx) {
    Free_value_list((VALUE_LIST*)Get_ptr_value_at(msg_lists, idx));
  }
  Free_value_list(cipher_lists);
  Free_value_list(msg_lists);
  Free_value_list(rot_lists);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(sum_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_hoist_opt2(int32_t hoist_cnt) {
  size_t      slots           = Get_degree() / 2;
  PLAINTEXT*  plain           = Alloc_plaintext();
  PLAINTEXT*  plain2          = Alloc_plaintext();
  PLAINTEXT*  out_plain       = Alloc_plaintext();
  CIPHERTEXT* rot_ciph        = Alloc_ciphertext();
  CIPHERTEXT* sum_ciph        = Alloc_ciphertext();
  CIPHERTEXT* out_ciph        = Alloc_ciphertext();
  VALUE_LIST* cipher_lists    = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* plain_lists     = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* rot_lists       = Alloc_value_list(I64_TYPE, hoist_cnt);
  VALUE_LIST* msg_lists       = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* msg_plain_lists = Alloc_value_list(PTR_TYPE, hoist_cnt);
  VALUE_LIST* decoded_vec     = Alloc_value_list(DCMPLX_TYPE, slots);
  VALUE_LIST* expect_vec      = Alloc_value_list(DCMPLX_TYPE, slots);

  Sample_uniform(rot_lists, hoist_cnt);
  Set_i64_value(rot_lists, 0, 0);
  Set_i64_value(rot_lists, 1, 1);

  for (size_t idx = 0; idx < hoist_cnt; idx++) {
    PLAINTEXT*  plain2 = Alloc_plaintext();
    CIPHERTEXT* ciph_i = Alloc_ciphertext();
    VALUE_LIST* msg    = Alloc_value_list(DCMPLX_TYPE, slots);
    // Sample_random_complex_vector(DCMPLX_VALUES(msg), slots);
    DCMPLX arr1[8] = {0.04086506228656631, -0.01893245471385017,
                      0.04334326361183686, 0.00000000148919092,
                      0.04086502925580827, -0.01893243967448501,
                      0.04334325481663374, 0.00000000110371033};
    DCMPLX arr2[8] = {0.00000000231962457, 4.00000000123034738,
                      1.99999999865953804, 0.00000000172445987,
                      0.00000000035609375, 0.00000000057512084,
                      0.00000000106505860, 0.00000000152033767};
    Init_dcmplx_value_list(msg, 8, arr1);

    ENCODE(plain, Get_encoder(), msg);
    Encrypt_msg(ciph_i, Get_encryptor(), plain);
    Set_ptr_value(cipher_lists, idx, (PTR)ciph_i);
    Set_ptr_value(msg_lists, idx, (PTR)msg);

    VALUE_LIST* msg_plain = Alloc_value_list(DCMPLX_TYPE, slots);
    // Sample_random_complex_vector(DCMPLX_VALUES(msg_plain), slots);

    Init_dcmplx_value_list(msg_plain, 8, arr2);
    Encode_ext_at_level(plain2, Get_encoder(), msg_plain,
                        Get_ciph_level(ciph_i), Get_ciph_slots(ciph_i),
                        Get_crt_num_p(Get_crt()));
    Set_ptr_value(plain_lists, idx, (PTR)plain2);
    Set_ptr_value(msg_plain_lists, idx, (PTR)msg_plain);
  }

  // generate rotation keys
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Insert_rot_map(Get_keygen(), rot_idx);
  }

  // out_ciph += \sigma rotate(ciph_i, idx) * plain_i
  auto start = chrono::system_clock::now();
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx  = (int32_t)Get_i64_value_at(rot_lists, idx);
    uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
    SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);
    CIPHERTEXT* ciph_i   = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx);
    PLAINTEXT*  plain_i  = (PLAINTEXT*)Get_ptr_value_at(plain_lists, idx);

    VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph_i), Get_crt());
    Print_ciph(ciph_i, 8);
    Fast_rotate_ext(rot_ciph, ciph_i, rot_idx, rot_key, _evaluator, precomputed,
                    true);
    Print_ciph(rot_ciph, 8);
    // add at QP modulus
    Mul_plaintext(rot_ciph, rot_ciph, plain_i, Get_eval());
    Print_ciph(rot_ciph, 8);

    if (idx == 0) {
      Init_ciphertext_from_ciph(sum_ciph, rot_ciph, rot_ciph->_scaling_factor,
                                rot_ciph->_sf_degree);
    }
    Add_ciphertext(sum_ciph, sum_ciph, rot_ciph, Get_eval());
    Free_switch_key_precomputed(precomputed);
  }

  // moddown hoisted
  CIPHERTEXT* ciph = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, 0);
  Init_ciphertext_from_ciph(out_ciph, ciph, sum_ciph->_scaling_factor,
                            sum_ciph->_sf_degree);
  Reduce_rns_base(Get_c0(out_ciph), Get_c0(sum_ciph), Get_crt());
  Reduce_rns_base(Get_c1(out_ciph), Get_c1(sum_ciph), Get_crt());

  auto end = chrono::system_clock::now();

  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t     rot_idx   = (int32_t)Get_i64_value_at(rot_lists, idx);
    VALUE_LIST* vec       = (VALUE_LIST*)Get_ptr_value_at(msg_lists, idx);
    VALUE_LIST* vec_plain = (VALUE_LIST*)Get_ptr_value_at(msg_plain_lists, idx);
    for (size_t idx2 = 0; idx2 < slots; idx2++) {
      DCMPLX_VALUE_AT(expect_vec, idx2) +=
          Get_dcmplx_value_at(vec, (idx2 + rot_idx) % slots) *
          Get_dcmplx_value_at(vec_plain, idx2);
    }
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  FOR_ALL_ELEM(cipher_lists, idx) {
    Free_ciphertext((CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx));
  }
  FOR_ALL_ELEM(msg_lists, idx) {
    Free_value_list((VALUE_LIST*)Get_ptr_value_at(msg_lists, idx));
  }
  Free_value_list(cipher_lists);
  Free_value_list(msg_lists);
  Free_value_list(rot_lists);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(sum_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_base(VALUE_LIST* vec1,
                                                        VALUE_LIST* vec2) {
  size_t      len         = LIST_LEN(vec1);
  PLAINTEXT*  plain1      = Alloc_plaintext();
  PLAINTEXT*  plain2      = Alloc_plaintext();
  PLAINTEXT*  out_plain   = Alloc_plaintext();
  CIPHERTEXT* ciph1       = Alloc_ciphertext();
  CIPHERTEXT* ciph2       = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph    = Alloc_ciphertext();
  CIPHERTEXT* out_ciph    = Alloc_ciphertext();
  VALUE_LIST* decoded_vec = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec  = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph1, Get_encryptor(), plain1);
  Encrypt_msg(ciph2, Get_encryptor(), plain2);

  auto start = chrono::system_clock::now();
  Mul_ciphertext(mul_ciph, ciph1, ciph2, Get_relin_key(), Get_eval());
  Rescale_ciphertext(out_ciph, mul_ciph, Get_eval());
  auto end = chrono::system_clock::now();

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        DCMPLX_VALUE_AT(vec1, i) * DCMPLX_VALUE_AT(vec2, i);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph2);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_opt(VALUE_LIST* vec1,
                                                       VALUE_LIST* vec2) {
  size_t       len             = LIST_LEN(vec1);
  PLAINTEXT*   plain1          = Alloc_plaintext();
  PLAINTEXT*   plain2          = Alloc_plaintext();
  PLAINTEXT*   out_plain       = Alloc_plaintext();
  CIPHERTEXT*  ciph1           = Alloc_ciphertext();
  CIPHERTEXT*  ciph2           = Alloc_ciphertext();
  CIPHERTEXT*  mul_ciph        = Alloc_ciphertext();
  CIPHERTEXT3* mul_ciph3       = Alloc_ciphertext3();
  CIPHERTEXT*  mul_ciph_reduce = Alloc_ciphertext();
  CIPHERTEXT*  out_ciph        = Alloc_ciphertext();
  VALUE_LIST*  decoded_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST*  expect_vec      = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph1, Get_encryptor(), plain1);
  Encrypt_msg(ciph2, Get_encryptor(), plain2);

  auto start = chrono::system_clock::now();
  Mul_ciphertext3(mul_ciph3, ciph1, ciph2, Get_eval());
  Relinearize_ciph3_ext(mul_ciph, mul_ciph3, Get_relin_key(), Get_eval());

  // Mod_down
  Conv_ntt2poly_inplace(Get_c0(mul_ciph), Get_crt());
  Conv_ntt2poly_inplace(Get_c1(mul_ciph), Get_crt());
  Init_ciphertext_from_ciph(mul_ciph_reduce, ciph1,
                            Get_ciph3_sfactor(mul_ciph3),
                            Get_ciph3_sf_degree(mul_ciph3));
  Reduce_rns_base(Get_c0(mul_ciph_reduce), Get_c0(mul_ciph), Get_crt());
  Reduce_rns_base(Get_c1(mul_ciph_reduce), Get_c1(mul_ciph), Get_crt());

  // Rescale
  Rescale_ciphertext(out_ciph, mul_ciph_reduce, Get_eval());
  Conv_poly2ntt_inplace(Get_c0(out_ciph), Get_crt());
  Conv_poly2ntt_inplace(Get_c1(out_ciph), Get_crt());
  auto end = chrono::system_clock::now();

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        DCMPLX_VALUE_AT(vec1, i) * DCMPLX_VALUE_AT(vec2, i);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph2);
  Free_ciphertext(mul_ciph);
  Free_ciphertext3(mul_ciph3);
  Free_ciphertext(mul_ciph_reduce);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_opt2(VALUE_LIST* vec1,
                                                        VALUE_LIST* vec2) {
  size_t       len             = LIST_LEN(vec1);
  PLAINTEXT*   plain1          = Alloc_plaintext();
  PLAINTEXT*   plain2          = Alloc_plaintext();
  PLAINTEXT*   out_plain       = Alloc_plaintext();
  CIPHERTEXT*  ciph1           = Alloc_ciphertext();
  CIPHERTEXT*  ciph2           = Alloc_ciphertext();
  CIPHERTEXT*  mul_ciph        = Alloc_ciphertext();
  CIPHERTEXT3* mul_ciph3       = Alloc_ciphertext3();
  CIPHERTEXT*  mul_ciph_reduce = Alloc_ciphertext();
  CIPHERTEXT*  out_ciph        = Alloc_ciphertext();
  VALUE_LIST*  decoded_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST*  expect_vec      = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph1, Get_encryptor(), plain1);
  Encrypt_msg(ciph2, Get_encryptor(), plain2);

  auto start = chrono::system_clock::now();
  Mul_ciphertext3(mul_ciph3, ciph1, ciph2, Get_eval());
  Relinearize_ciph3_ext(mul_ciph, mul_ciph3, Get_relin_key(), Get_eval());

  // Mod_down & rescale
  Init_ciphertext_from_ciph(out_ciph, ciph1,
                            Get_ciph3_sfactor(mul_ciph3) / Get_scaling_factor(),
                            Get_ciph3_sf_degree(mul_ciph3) - 1);

  Reduce_and_rescale(Get_c0(out_ciph), Get_c0(mul_ciph), Get_crt(), true);
  Reduce_and_rescale(Get_c1(out_ciph), Get_c1(mul_ciph), Get_crt(), true);
  auto end = chrono::system_clock::now();

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        DCMPLX_VALUE_AT(vec1, i) * DCMPLX_VALUE_AT(vec2, i);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph2);
  Free_ciphertext(mul_ciph);
  Free_ciphertext3(mul_ciph3);
  Free_ciphertext(mul_ciph_reduce);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rotate_rescale_base(
    VALUE_LIST* vec1, VALUE_LIST* vec2, int32_t rot_idx) {
  size_t      len         = LIST_LEN(vec1);
  size_t      slots       = Get_degree() / 2;
  PLAINTEXT*  plain1      = Alloc_plaintext();
  PLAINTEXT*  plain2      = Alloc_plaintext();
  PLAINTEXT*  out_plain   = Alloc_plaintext();
  CIPHERTEXT* ciph1       = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph    = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph    = Alloc_ciphertext();
  CIPHERTEXT* out_ciph    = Alloc_ciphertext();
  VALUE_LIST* decoded_vec = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* mul_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec  = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph1, Get_encryptor(), plain1);

  Mul_plaintext(mul_ciph, ciph1, plain2, Get_eval());

  // generate rot key
  Insert_rot_map(Get_keygen(), rot_idx);
  uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
  SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);

  auto start = chrono::system_clock::now();
  Eval_fast_rotate(rot_ciph, mul_ciph, rot_idx, rot_key, Get_eval());
  Rescale_ciphertext(out_ciph, rot_ciph, Get_eval());
  auto end = chrono::system_clock::now();

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(mul_vec, i) =
        DCMPLX_VALUE_AT(vec1, i) * DCMPLX_VALUE_AT(vec2, i);
  }
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        DCMPLX_VALUE_AT(mul_vec, (i + rot_idx) % slots);
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(decoded_vec);
  Free_value_list(mul_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph1);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rotate_rescale_opt(VALUE_LIST* vec1,
                                                              VALUE_LIST* vec2,
                                                              int32_t rot_idx) {
  size_t       len         = LIST_LEN(vec1);
  size_t       slots       = Get_degree() / 2;
  PLAINTEXT*   plain1      = Alloc_plaintext();
  PLAINTEXT*   plain2      = Alloc_plaintext();
  PLAINTEXT*   out_plain   = Alloc_plaintext();
  CIPHERTEXT*  ciph1       = Alloc_ciphertext();
  CIPHERTEXT*  rot_ciph    = Alloc_ciphertext();
  CIPHERTEXT*  raised_ciph = Alloc_ciphertext();
  CIPHERTEXT*  mul_ciph    = Alloc_ciphertext();
  CIPHERTEXT*  reduce_ciph = Alloc_ciphertext();
  CIPHERTEXT*  out_ciph    = Alloc_ciphertext();
  VALUE_LIST*  decoded_vec = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST*  mul_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST*  expect_vec  = Alloc_value_list(DCMPLX_TYPE, len);
  CRT_CONTEXT* crt         = Get_crt();

  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph1, Get_encryptor(), plain1);

  Mul_plaintext(mul_ciph, ciph1, plain2, Get_eval());

  // generate rot key
  Insert_rot_map(Get_keygen(), rot_idx);
  uint32_t    auto_idx = Get_precomp_auto_idx(Get_keygen(), rot_idx);
  SWITCH_KEY* rot_key  = Get_auto_key(Get_keygen(), auto_idx);

  auto     start = chrono::system_clock::now();
  uint32_t k     = Get_precomp_auto_idx(Get_eval()->_keygen, rot_idx);
  FMT_ASSERT(k, "cannot get precompute automorphism index");
  VALUE_LIST* precomp = Get_precomp_auto_order(Get_eval()->_keygen, k);
  IS_TRUE(precomp, "cannot find precomputed automorphism order");

  VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(mul_ciph), crt);

  Init_ciphertext(raised_ciph, Get_rdgree(Get_c0(mul_ciph)),
                  Get_num_q(Get_c0(mul_ciph)), Get_primes_cnt(Get_p(crt)),
                  mul_ciph->_scaling_factor, mul_ciph->_sf_degree,
                  mul_ciph->_slots);
  Set_is_ntt(Get_c0(raised_ciph), TRUE);
  Set_is_ntt(Get_c1(raised_ciph), TRUE);
  Init_ciphertext_from_ciph(reduce_ciph, mul_ciph, Get_ciph_sfactor(mul_ciph),
                            Get_ciph_sf_degree(mul_ciph));
  Init_ciphertext_from_ciph(rot_ciph, reduce_ciph, raised_ciph->_scaling_factor,
                            reduce_ciph->_sf_degree);
  double new_factor =
      Get_ciph_sfactor(rot_ciph) / Get_eval()->_params->_scaling_factor;
  Init_ciphertext_from_ciph(out_ciph, rot_ciph, new_factor,
                            rot_ciph->_sf_degree - 1);

  Fast_switch_key_ext(raised_ciph, rot_key, Get_eval(), precomputed,
                      Is_ntt(Get_c0(mul_ciph)));

  // c0
  Reduce_rns_base(Get_c0(reduce_ciph), Get_c0(raised_ciph), crt);
  Add_poly(Get_c0(reduce_ciph), Get_c0(reduce_ciph), Get_c0(mul_ciph), crt,
           NULL);
  Automorphism_transform(Get_c0(rot_ciph), Get_c0(reduce_ciph), precomp, crt);
  Rescale_poly(Get_c0(out_ciph), Get_c0(rot_ciph), crt);

  // c1
  // Reduce_rns_base(Get_c1(reduce_ciph), Get_c1(raised_ciph), crt);
  // Automorphism_transform(Get_c1(rot_ciph), Get_c1(reduce_ciph), precomp,
  // crt); Rescale_poly(Get_c1(out_ciph), Get_c1(rot_ciph), crt);
  Reduce_rotate_and_rescale(Get_c1(out_ciph), rot_idx, Get_c1(raised_ciph), crt,
                            true);

  Mod_down_q_primes(Get_c0(out_ciph));
  // Mod_down_q_primes(Get_c1(out_ciph));

  auto end = chrono::system_clock::now();

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(mul_vec, i) =
        DCMPLX_VALUE_AT(vec1, i) * DCMPLX_VALUE_AT(vec2, i);
  }
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        DCMPLX_VALUE_AT(mul_vec, (i + rot_idx) % slots);
  }

  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(decoded_vec);
  Free_value_list(mul_vec);
  Free_value_list(expect_vec);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph1);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(reduce_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_modup_base(VALUE_LIST* vec1,
                                                              VALUE_LIST* vec2,
                                                              int32_t     rot1,
                                                              int32_t rot2) {
  size_t      len          = LIST_LEN(vec1);
  size_t      slots        = Get_degree() / 2;
  PLAINTEXT*  plain1       = Alloc_plaintext();
  PLAINTEXT*  plain2       = Alloc_plaintext();
  PLAINTEXT*  out_plain    = Alloc_plaintext();
  CIPHERTEXT* ciph         = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph     = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph     = Alloc_ciphertext();
  CIPHERTEXT* rescale_ciph = Alloc_ciphertext();
  CIPHERTEXT* out_ciph     = Alloc_ciphertext();
  VALUE_LIST* decoded_vec  = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec   = Alloc_value_list(DCMPLX_TYPE, len);

  // create rotation keys
  Insert_rot_map(Get_keygen(), rot1);
  Insert_rot_map(Get_keygen(), rot2);
  uint32_t    auto_idx1 = Get_precomp_auto_idx(Get_keygen(), rot1);
  uint32_t    auto_idx2 = Get_precomp_auto_idx(Get_keygen(), rot2);
  SWITCH_KEY* rot_key1  = Get_auto_key(Get_keygen(), auto_idx1);
  SWITCH_KEY* rot_key2  = Get_auto_key(Get_keygen(), auto_idx2);

  // encode & encrypt msg
  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph, Get_encryptor(), plain1);

  // Rotate(Rescale(Rotate(vec1, rot1) * plain2), rot2)
  auto start = chrono::system_clock::now();
  Eval_fast_rotate(rot_ciph, ciph, rot1, rot_key1, Get_eval());
  Mul_plaintext(mul_ciph, rot_ciph, plain2, Get_eval());
  Rescale_ciphertext(rescale_ciph, mul_ciph, Get_eval());
  Eval_fast_rotate(out_ciph, rescale_ciph, rot2, rot_key2, Get_eval());
  auto end = chrono::system_clock::now();

  // check results
  VALUE_LIST* vec3 = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(vec3, i) = Get_dcmplx_value_at(vec1, (i + rot1) % slots) *
                               DCMPLX_VALUE_AT(vec2, i);
  }
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        Get_dcmplx_value_at(vec3, (i + rot2) % slots);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_value_list(vec3);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(rescale_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_modup_opt(VALUE_LIST* vec1,
                                                             VALUE_LIST* vec2,
                                                             int32_t     rot1,
                                                             int32_t     rot2) {
  size_t      len             = LIST_LEN(vec1);
  size_t      slots           = Get_degree() / 2;
  PLAINTEXT*  plain1          = Alloc_plaintext();
  PLAINTEXT*  plain2          = Alloc_plaintext();
  PLAINTEXT*  out_plain       = Alloc_plaintext();
  CIPHERTEXT* ciph            = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph_reduce = Alloc_ciphertext();
  CIPHERTEXT* rescale_ciph    = Alloc_ciphertext();
  CIPHERTEXT* out_ciph        = Alloc_ciphertext();
  VALUE_LIST* decoded_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec      = Alloc_value_list(DCMPLX_TYPE, len);

  // create rotation keys
  Insert_rot_map(Get_keygen(), rot1);
  Insert_rot_map(Get_keygen(), rot2);
  uint32_t    auto_idx1 = Get_precomp_auto_idx(Get_keygen(), rot1);
  uint32_t    auto_idx2 = Get_precomp_auto_idx(Get_keygen(), rot2);
  SWITCH_KEY* rot_key1  = Get_auto_key(Get_keygen(), auto_idx1);
  SWITCH_KEY* rot_key2  = Get_auto_key(Get_keygen(), auto_idx2);

  // encode & encrypt msg
  ENCODE(plain1, Get_encoder(), vec1);
  Encrypt_msg(ciph, Get_encryptor(), plain1);
  Encode_ext_at_level(plain2, Get_encoder(), vec2, Get_ciph_level(ciph),
                      Get_ciph_slots(ciph), Get_crt_num_p(Get_crt()));

  // Rotate(Rescale(Rotate(vec1, rot1) * plain2), rot2)
  auto        start       = chrono::system_clock::now();
  VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph), Get_crt());
  Fast_rotate_ext(rot_ciph, ciph, rot1, rot_key1, Get_eval(), precomputed,
                  true);
  Mul_plaintext(mul_ciph, rot_ciph, plain2, Get_eval());

  // Mod_down
  // Conv_ntt2poly_inplace(Get_c0(mul_ciph), Get_crt());
  Conv_ntt2poly_inplace(Get_c1(mul_ciph), Get_crt());
  Init_ciphertext_from_ciph(mul_ciph_reduce, ciph, Get_ciph_sfactor(mul_ciph),
                            Get_ciph_sf_degree(mul_ciph));
  Reduce_rns_base(Get_c0(mul_ciph_reduce), Get_c0(mul_ciph), Get_crt());
  Reduce_rns_base(Get_c1(mul_ciph_reduce), Get_c1(mul_ciph), Get_crt());
  Rescale_ciphertext(rescale_ciph, mul_ciph_reduce, Get_eval());

  VALUE_LIST* precomputed2 =
      Switch_key_precompute(Get_c1(rescale_ciph), Get_crt());
  Fast_rotate(out_ciph, rescale_ciph, rot2, rot_key2, Get_eval(), precomputed2);
  auto end = chrono::system_clock::now();

  // check results
  VALUE_LIST* vec3 = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(vec3, i) = Get_dcmplx_value_at(vec1, (i + rot1) % slots) *
                               DCMPLX_VALUE_AT(vec2, i);
  }
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        Get_dcmplx_value_at(vec3, (i + rot2) % slots);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_switch_key_precomputed(precomputed);
  Free_switch_key_precomputed(precomputed2);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_value_list(vec3);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(mul_ciph_reduce);
  Free_ciphertext(rescale_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_modup_opt2(VALUE_LIST* vec1,
                                                              VALUE_LIST* vec2,
                                                              int32_t     rot1,
                                                              int32_t rot2) {
  size_t      len             = LIST_LEN(vec1);
  size_t      slots           = Get_degree() / 2;
  PLAINTEXT*  plain1          = Alloc_plaintext();
  PLAINTEXT*  plain2          = Alloc_plaintext();
  PLAINTEXT*  out_plain       = Alloc_plaintext();
  CIPHERTEXT* ciph            = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph_reduce = Alloc_ciphertext();
  CIPHERTEXT* rescale_ciph    = Alloc_ciphertext();
  CIPHERTEXT* out_ciph        = Alloc_ciphertext();
  VALUE_LIST* decoded_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec      = Alloc_value_list(DCMPLX_TYPE, len);

  // create rotation keys
  Insert_rot_map(Get_keygen(), rot1);
  Insert_rot_map(Get_keygen(), rot2);
  uint32_t    auto_idx1 = Get_precomp_auto_idx(Get_keygen(), rot1);
  uint32_t    auto_idx2 = Get_precomp_auto_idx(Get_keygen(), rot2);
  SWITCH_KEY* rot_key1  = Get_auto_key(Get_keygen(), auto_idx1);
  SWITCH_KEY* rot_key2  = Get_auto_key(Get_keygen(), auto_idx2);

  // encode & encrypt msg
  ENCODE(plain1, Get_encoder(), vec1);
  Encrypt_msg(ciph, Get_encryptor(), plain1);
  Encode_ext_at_level(plain2, Get_encoder(), vec2, Get_ciph_level(ciph),
                      Get_ciph_slots(ciph), Get_crt_num_p(Get_crt()));

  // Rotate(Rescale(Rotate(vec1, rot1) * plain2), rot2)
  auto        start       = chrono::system_clock::now();
  VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph), Get_crt());
  Fast_rotate_ext(rot_ciph, ciph, rot1, rot_key1, Get_eval(), precomputed,
                  true);
  Mul_plaintext(mul_ciph, rot_ciph, plain2, Get_eval());

// Mod_down
// Conv_ntt2poly_inplace(Get_c0(mul_ciph), Get_crt());
#if 0
  Conv_ntt2poly_inplace(Get_c1(mul_ciph), Get_crt());
  Init_ciphertext_from_ciph(mul_ciph_reduce, ciph, Get_ciph_sfactor(mul_ciph),
                            Get_ciph_sf_degree(mul_ciph));
  Reduce_rns_base(Get_c0(mul_ciph_reduce), Get_c0(mul_ciph), Get_crt());
  Reduce_rns_base(Get_c1(mul_ciph_reduce), Get_c1(mul_ciph), Get_crt());
  Rescale_ciphertext(rescale_ciph, mul_ciph_reduce, Get_eval());
#endif

  Init_ciphertext_from_ciph(rescale_ciph, ciph,
                            Get_ciph_sfactor(mul_ciph) / Get_scaling_factor(),
                            Get_ciph_sf_degree(mul_ciph) - 1);
  Reduce_and_rescale(Get_c0(rescale_ciph), Get_c0(mul_ciph), Get_crt(), true);
  Reduce_and_rescale(Get_c1(rescale_ciph), Get_c1(mul_ciph), Get_crt(), false);

  VALUE_LIST* precomputed2 =
      Switch_key_precompute(Get_c1(rescale_ciph), Get_crt());
  Fast_rotate(out_ciph, rescale_ciph, rot2, rot_key2, Get_eval(), precomputed2);
  auto end = chrono::system_clock::now();

  // check results
  VALUE_LIST* vec3 = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(vec3, i) = Get_dcmplx_value_at(vec1, (i + rot1) % slots) *
                               DCMPLX_VALUE_AT(vec2, i);
  }
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        Get_dcmplx_value_at(vec3, (i + rot2) % slots);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_switch_key_precomputed(precomputed);
  Free_switch_key_precomputed(precomputed2);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_value_list(vec3);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(mul_ciph_reduce);
  Free_ciphertext(rescale_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

microseconds TEST_KSW_OPT::Run_ksw_moddown_rescale_modup_opt3(VALUE_LIST* vec1,
                                                              VALUE_LIST* vec2,
                                                              int32_t     rot1,
                                                              int32_t rot2) {
  size_t      len             = LIST_LEN(vec1);
  size_t      slots           = Get_degree() / 2;
  PLAINTEXT*  plain1          = Alloc_plaintext();
  PLAINTEXT*  plain2          = Alloc_plaintext();
  PLAINTEXT*  out_plain       = Alloc_plaintext();
  CIPHERTEXT* ciph            = Alloc_ciphertext();
  CIPHERTEXT* rot_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph        = Alloc_ciphertext();
  CIPHERTEXT* mul_ciph_reduce = Alloc_ciphertext();
  CIPHERTEXT* rescale_ciph    = Alloc_ciphertext();
  CIPHERTEXT* out_ciph        = Alloc_ciphertext();
  VALUE_LIST* decoded_vec     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expect_vec      = Alloc_value_list(DCMPLX_TYPE, len);

  // create rotation keys
  Insert_rot_map(Get_keygen(), rot1);
  Insert_rot_map(Get_keygen(), rot2);
  uint32_t    auto_idx1 = Get_precomp_auto_idx(Get_keygen(), rot1);
  uint32_t    auto_idx2 = Get_precomp_auto_idx(Get_keygen(), rot2);
  SWITCH_KEY* rot_key1  = Get_auto_key(Get_keygen(), auto_idx1);
  SWITCH_KEY* rot_key2  = Get_auto_key(Get_keygen(), auto_idx2);

  // encode & encrypt msg
  ENCODE(plain1, Get_encoder(), vec1);
  Encrypt_msg(ciph, Get_encryptor(), plain1);
  Encode_ext_at_level(plain2, Get_encoder(), vec2, Get_ciph_level(ciph),
                      Get_ciph_slots(ciph), Get_crt_num_p(Get_crt()));

  // Rotate(Rescale(Rotate(vec1, rot1) * plain2), rot2)
  auto        start       = chrono::system_clock::now();
  VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph), Get_crt());
  Fast_rotate_ext(rot_ciph, ciph, rot1, rot_key1, Get_eval(), precomputed,
                  true);
  Mul_plaintext(mul_ciph, rot_ciph, plain2, Get_eval());

// Mod_down
// Conv_ntt2poly_inplace(Get_c0(mul_ciph), Get_crt());
#if 0
  Conv_ntt2poly_inplace(Get_c1(mul_ciph), Get_crt());
  Init_ciphertext_from_ciph(mul_ciph_reduce, ciph, Get_ciph_sfactor(mul_ciph),
                            Get_ciph_sf_degree(mul_ciph));
  Reduce_rns_base(Get_c0(mul_ciph_reduce), Get_c0(mul_ciph), Get_crt());
  Reduce_rns_base(Get_c1(mul_ciph_reduce), Get_c1(mul_ciph), Get_crt());
  Rescale_ciphertext(rescale_ciph, mul_ciph_reduce, Get_eval());
#endif

  Init_ciphertext_from_ciph(rescale_ciph, ciph,
                            Get_ciph_sfactor(mul_ciph) / Get_scaling_factor(),
                            Get_ciph_sf_degree(mul_ciph) - 1);
  Reduce_and_rescale(Get_c0(rescale_ciph), Get_c0(mul_ciph), Get_crt(), true);

  // Reduce_and_rescale(Get_c1(rescale_ciph), Get_c1(mul_ciph), Get_crt(),
  // false);

  // VALUE_LIST* precomputed3 =
  //     Switch_key_precompute(Get_c1(rescale_ciph), Get_crt());

  VALUE_LIST* precomputed2 = Reduce_and_rescale_modup(
      Get_c1(rescale_ciph), Get_c1(mul_ciph), Get_crt());

  // Print_poly_list(stdout, precomputed2);
  // Print_poly_list(stdout, precomputed3);

  Fast_rotate(out_ciph, rescale_ciph, rot2, rot_key2, Get_eval(), precomputed2);
  auto end = chrono::system_clock::now();

  // check results
  VALUE_LIST* vec3 = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(vec3, i) = Get_dcmplx_value_at(vec1, (i + rot1) % slots) *
                               DCMPLX_VALUE_AT(vec2, i);
  }
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(expect_vec, i) =
        Get_dcmplx_value_at(vec3, (i + rot2) % slots);
  }
  Decrypt(out_plain, Get_decryptor(), out_ciph, NULL);
  Decode(decoded_vec, Get_encoder(), out_plain);
  Check_complex_vector_approx_eq(expect_vec, decoded_vec, 0.005);

  Free_switch_key_precomputed(precomputed);
  Free_switch_key_precomputed(precomputed2);
  Free_value_list(decoded_vec);
  Free_value_list(expect_vec);
  Free_value_list(vec3);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(mul_ciph);
  Free_ciphertext(mul_ciph_reduce);
  Free_ciphertext(rescale_ciph);
  Free_ciphertext(out_ciph);
  return duration_cast<microseconds>(end - start);
}

TEST_F(TEST_KSW_OPT, Run_ksw_modup_hoist_base) {
  microseconds total_time(0);
  microseconds nontt_rescale_total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  msg            = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(msg), len);
  size_t num_rots = 15;

  string prefix = "Run_ksw_modup_hoist_base with ";
  for (size_t j = 2; j < num_rots; j++) {
    microseconds rot_time(0);
    string       title(prefix);
    title += std::to_string(j);
    title += " rotates num_q = ";
    title += std::to_string(UT_GLOB_ENV::Get_env(NUM_Q));
    for (size_t i = 0; i < num_iterations; i++) {
      rot_time += Run_ksw_modup_hoist_base(msg, j);
    }
    Report_time(title, rot_time, num_iterations);
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_modup_hoist_opt) {
  size_t      len      = Get_degree() / 2;
  VALUE_LIST* msg      = Alloc_value_list(DCMPLX_TYPE, len);
  size_t      num_rots = 15;
  Sample_random_complex_vector(DCMPLX_VALUES(msg), len);

  string prefix = "Run_ksw_modup_hoist_opt with ";
  for (size_t j = 2; j < num_rots; j++) {
    microseconds rot_time(0);
    string       title(prefix);
    title += std::to_string(j);
    title += " rotates num_q = ";
    title += std::to_string(UT_GLOB_ENV::Get_env(NUM_Q));
    for (size_t i = 0; i < Get_num_iter(); i++) {
      rot_time += Run_ksw_modup_hoist_opt(msg, j);
    }
    Report_time(title, rot_time, Get_num_iter());
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_hoist_base) {
  size_t num_rots = 15;
  string prefix   = "Run_ksw_moddown_hoist_base with ";
  for (size_t j = 2; j < num_rots; j++) {
    microseconds rot_time(0);
    string       title(prefix);
    title += std::to_string(j);
    title += " rotates num_q = ";
    title += std::to_string(UT_GLOB_ENV::Get_env(NUM_Q));
    for (size_t i = 0; i < Get_num_iter(); i++) {
      rot_time += Run_ksw_moddown_hoist_base(j);
    }
    Report_time(title, rot_time, Get_num_iter());
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_hoist_opt) {
  size_t num_rots = 15;
  string prefix   = "Run_ksw_moddown_hoist_opt with ";
  for (size_t j = 2; j < num_rots; j++) {
    microseconds rot_time(0);
    string       title(prefix);
    title += std::to_string(j);
    title += " rotates num_q = ";
    title += std::to_string(UT_GLOB_ENV::Get_env(NUM_Q));
    for (size_t i = 0; i < Get_num_iter(); i++) {
      rot_time += Run_ksw_moddown_hoist_opt(j);
    }
    Report_time(title, rot_time, Get_num_iter());
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_hoist_base2) {
  size_t num_rots = 15;
  string prefix   = "Run_ksw_moddown_hoist_base2 with ";
  for (size_t j = 1; j < num_rots; j++) {
    microseconds rot_time(0);
    string       title(prefix);
    title += std::to_string(j);
    title += " rotates num_q = ";
    title += std::to_string(UT_GLOB_ENV::Get_env(NUM_Q));
    for (size_t i = 0; i < Get_num_iter(); i++) {
      rot_time += Run_ksw_moddown_hoist_base2(j);
    }
    Report_time(title, rot_time, Get_num_iter());
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_hoist_opt2) {
  size_t num_rots = 3;
  string prefix   = "Run_ksw_moddown_hoist_opt2 with ";
  for (size_t j = 2; j < num_rots; j++) {
    microseconds rot_time(0);
    string       title(prefix);
    title += std::to_string(j);
    title += " rotates num_q = ";
    title += std::to_string(UT_GLOB_ENV::Get_env(NUM_Q));
    for (size_t i = 0; i < Get_num_iter(); i++) {
      rot_time += Run_ksw_moddown_hoist_opt2(j);
    }
    Report_time(title, rot_time, Get_num_iter());
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_base) {
  string      prefix = "Run_ksw_moddown_rescale_base ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_base(vec1, vec2);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_opt) {
  string      prefix = "Run_ksw_moddown_rescale_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_opt(vec1, vec2);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_opt2) {
  string      prefix = "Run_ksw_moddown_rescale_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_opt2(vec1, vec2);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rotate_rescale_base) {
  string      prefix = "Run_ksw_moddown_rotate_rescale_base ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  int32_t      rot_idx = rand() % len;
  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rotate_rescale_base(vec1, vec2, rot_idx);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rotate_rescale_opt) {
  string      prefix = "Run_ksw_moddown_rescale_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  int32_t      rot_idx = rand() % len;
  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rotate_rescale_opt(vec1, vec2, rot_idx);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_modup_base) {
  string      prefix = "Run_ksw_moddown_rescale_modup_base ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_modup_base(vec1, vec2, 2, 3);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_modup_opt) {
  string      prefix = "Run_ksw_moddown_rescale_modup_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_modup_opt(vec1, vec2, 2, 3);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_modup_opt2) {
  string      prefix = "Run_ksw_moddown_rescale_modup_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_modup_opt2(vec1, vec2, 2, 3);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_modup_opt3) {
  string      prefix = "Run_ksw_moddown_rescale_modup_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time(0);
  for (size_t i = 0; i < Get_num_iter(); i++) {
    tot_time += Run_ksw_moddown_rescale_modup_opt3(vec1, vec2, 2, 3);
  }
  Report_time(prefix, tot_time, Get_num_iter());
}