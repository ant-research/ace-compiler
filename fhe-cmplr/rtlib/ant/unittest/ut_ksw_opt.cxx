//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <chrono>

#include "ckks/bootstrap.h"
#include "ckks/ciphertext.h"
#include "ckks/decryptor.h"
#include "ckks/encoder.h"
#include "ckks/encryptor.h"
#include "ckks/evaluator.h"
#include "ckks/key_gen.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "common/rt_api.h"
#include "common/rt_config.h"
#include "context/ckks_context.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "poly/rns_poly.h"
#include "util/matrix_operation.h"
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
    Set_context_params(_degree, num_q, q0_bits, sf_bits, parts);
    Prepare_context();

    // TODO: should be removed after refactor CKKS APIs
    _param     = (CKKS_PARAMETER*)Param();
    _keygen    = (CKKS_KEY_GENERATOR*)Keygen();
    _evaluator = (CKKS_EVALUATOR*)Eval();
    _encoder   = (CKKS_ENCODER*)Encoder();
    _decryptor = (CKKS_DECRYPTOR*)Decryptor();
    _encryptor = (CKKS_ENCRYPTOR*)Encryptor();
  }

  void TearDown() override { Finalize_context(); }

  void Report_time(string title, microseconds time, size_t num_iteration);

  size_t              Get_degree() { return _degree; }
  size_t              Get_num_iter() { return _num_iterations; }
  size_t              Get_depth() { return _param->_multiply_depth; }
  CKKS_ENCODER*       Get_encoder() { return _encoder; }
  CKKS_ENCRYPTOR*     Get_encryptor() { return _encryptor; }
  CKKS_KEY_GENERATOR* Get_keygen() { return _keygen; }
  CKKS_EVALUATOR*     Get_eval() { return _evaluator; }
  CKKS_DECRYPTOR*     Get_decryptor() { return _decryptor; }

  microseconds Run_ksw_modup_hoist_base(VALUE_LIST* vec1, int32_t num_rots);
  microseconds Run_ksw_modup_hoist_opt(VALUE_LIST* vec1, int32_t num_rots);

  microseconds Run_ksw_moddown_hoist_base(int32_t hoist_cnt);
  microseconds Run_ksw_moddown_hoist_opt(int32_t hoist_cnt);

  microseconds Run_ksw_moddown_rescale_base(VALUE_LIST* vec1, VALUE_LIST* vec2);
  microseconds Run_ksw_moddown_rescale_opt(VALUE_LIST* vec1, VALUE_LIST* vec2);

  microseconds Run_ksw_moddown_rescale_modup_base(VALUE_LIST* vec1,
                                                  VALUE_LIST* vec2,
                                                  int32_t rot1, int32_t rot2);
  microseconds Run_ksw_moddown_rescale_modup_opt(VALUE_LIST* vec1,
                                                 VALUE_LIST* vec2, int32_t rot1,
                                                 int32_t rot2);

private:
  size_t              _degree;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_EVALUATOR*     _evaluator;
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
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
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Eval_fast_rotate(rot_ciph, ciph, rot_idx, Get_eval());
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
  VALUE_LIST* precomputed = Alloc_precomp(Get_c1(ciph));
  FOR_ALL_ELEM(rot_lists, idx) {
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Fast_rotate(rot_ciph, ciph, rot_idx, Get_eval(), precomputed);
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

  Free_precomp(precomputed);
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
    int32_t rot_idx = (int32_t)Get_i64_value_at(rot_lists, idx);
    Eval_fast_rotate(rot_ciph, (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx),
                     rot_idx, Get_eval());
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
    int32_t     rot_idx     = (int32_t)Get_i64_value_at(rot_lists, idx);
    CIPHERTEXT* ciph_i      = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, idx);
    VALUE_LIST* precomputed = Alloc_precomp(Get_c1(ciph_i));
    Fast_rotate_ext(rot_ciph, ciph_i, rot_idx, _evaluator, precomputed, true);
    if (idx == 0) {
      Init_ciphertext_from_ciph(sum_ciph, rot_ciph, rot_ciph->_scaling_factor,
                                rot_ciph->_sf_degree);
    }
    // add at QP modulus
    Add_ciphertext(sum_ciph, sum_ciph, rot_ciph, Get_eval());
    Free_precomp(precomputed);
  }

  // moddown hoisted
  CIPHERTEXT* ciph = (CIPHERTEXT*)Get_ptr_value_at(cipher_lists, 0);
  Init_ciphertext_from_ciph(out_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  Mod_down(Get_c0(out_ciph), Get_c0(sum_ciph));
  Mod_down(Get_c1(out_ciph), Get_c1(sum_ciph));

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
  Mul_ciphertext(mul_ciph, ciph1, ciph2, Get_eval());
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

  ENCODE_AT_LEVEL(plain1, Get_encoder(), vec1, Get_depth());
  ENCODE_AT_LEVEL(plain2, Get_encoder(), vec2, Get_depth());
  Encrypt_msg(ciph1, Get_encryptor(), plain1);
  Encrypt_msg(ciph2, Get_encryptor(), plain2);

  auto start = chrono::system_clock::now();
  Mul_ciphertext3(mul_ciph3, ciph1, ciph2, Get_eval());
  Relinearize_ciph3_ext(mul_ciph, mul_ciph3, Get_eval());

  // Mod_down
  Conv_ntt2poly_inplace(Get_c0(mul_ciph));
  Conv_ntt2poly_inplace(Get_c1(mul_ciph));
  Init_ciphertext_from_ciph(mul_ciph_reduce, ciph1,
                            Get_ciph3_sfactor(mul_ciph3),
                            Get_ciph3_sf_degree(mul_ciph3));
  Mod_down(Get_c0(mul_ciph_reduce), Get_c0(mul_ciph));
  Mod_down(Get_c1(mul_ciph_reduce), Get_c1(mul_ciph));

  // Rescale
  Rescale_ciphertext(out_ciph, mul_ciph_reduce, Get_eval());
  Conv_poly2ntt_inplace(Get_c0(out_ciph));
  Conv_poly2ntt_inplace(Get_c1(out_ciph));
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

  // encode & encrypt msg
  ENCODE(plain1, Get_encoder(), vec1);
  ENCODE(plain2, Get_encoder(), vec2);
  Encrypt_msg(ciph, Get_encryptor(), plain1);

  // Rotate(Rescale(Rotate(vec1, rot1) * plain2), rot2)
  auto start = chrono::system_clock::now();
  Eval_fast_rotate(rot_ciph, ciph, rot1, Get_eval());
  Mul_plaintext(mul_ciph, rot_ciph, plain2, Get_eval());
  Rescale_ciphertext(rescale_ciph, mul_ciph, Get_eval());
  Eval_fast_rotate(out_ciph, rescale_ciph, rot2, Get_eval());
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

  // encode & encrypt msg
  ENCODE(plain1, Get_encoder(), vec1);
  Encrypt_msg(ciph, Get_encryptor(), plain1);
  Encode_ext_at_level(plain2, Get_encoder(), vec2, Get_ciph_level(ciph),
                      Get_ciph_slots(ciph), Get_p_cnt());

  // Rotate(Rescale(Rotate(vec1, rot1) * plain2), rot2)
  auto        start       = chrono::system_clock::now();
  VALUE_LIST* precomputed = Alloc_precomp(Get_c1(ciph));
  Fast_rotate_ext(rot_ciph, ciph, rot1, Get_eval(), precomputed, true);
  Mul_plaintext(mul_ciph, rot_ciph, plain2, Get_eval());

  // Mod_down
  // Conv_ntt2poly_inplace(Get_c0(mul_ciph));
  Conv_ntt2poly_inplace(Get_c1(mul_ciph));
  Init_ciphertext_from_ciph(mul_ciph_reduce, ciph, Get_ciph_sfactor(mul_ciph),
                            Get_ciph_sf_degree(mul_ciph));
  Mod_down(Get_c0(mul_ciph_reduce), Get_c0(mul_ciph));
  Mod_down(Get_c1(mul_ciph_reduce), Get_c1(mul_ciph));
  Rescale_ciphertext(rescale_ciph, mul_ciph_reduce, Get_eval());
  // conv rescal_ciph back to ntt form
  Conv_poly2ntt_inplace(Get_c1(rescale_ciph));

  VALUE_LIST* precomputed2 = Alloc_precomp(Get_c1(rescale_ciph));
  Fast_rotate(out_ciph, rescale_ciph, rot2, Get_eval(), precomputed2);
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

  Free_precomp(precomputed);
  Free_precomp(precomputed2);
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
  size_t num_rots = 10;

  string prefix = "Run_ksw_modup_hoist_base with ";
  for (size_t j = 2; j < num_rots; j++) {
    string title(prefix);
    title += std::to_string(j);
    title += " rotates ";
    microseconds rot_time = Run_ksw_modup_hoist_base(msg, j);

    Report_time(title, rot_time, Get_num_iter());
  }
  Free_value_list(msg);
}

TEST_F(TEST_KSW_OPT, Run_ksw_modup_hoist_opt) {
  size_t      len      = Get_degree() / 2;
  VALUE_LIST* msg      = Alloc_value_list(DCMPLX_TYPE, len);
  size_t      num_rots = 10;
  Sample_random_complex_vector(DCMPLX_VALUES(msg), len);

  string prefix = "Run_ksw_modup_hoist_opt with ";
  for (size_t j = 2; j < num_rots; j++) {
    string title(prefix);
    title += std::to_string(j);
    title += " rotates ";
    microseconds rot_time = Run_ksw_modup_hoist_opt(msg, j);

    Report_time(title, rot_time, Get_num_iter());
  }
  Free_value_list(msg);
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_hoist_base) {
  size_t num_rots = 10;
  string prefix   = "Run_ksw_moddown_hoist_base with ";
  for (size_t j = 2; j < num_rots; j++) {
    string title(prefix);
    title += std::to_string(j);
    title += " rotates ";
    microseconds rot_time = Run_ksw_moddown_hoist_base(j);
    Report_time(title, rot_time, Get_num_iter());
  }
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_hoist_opt) {
  size_t num_rots = 10;
  string prefix   = "Run_ksw_moddown_hoist_opt with ";
  for (size_t j = 2; j < num_rots; j++) {
    string title(prefix);
    title += std::to_string(j);
    title += " rotates ";
    microseconds rot_time = Run_ksw_moddown_hoist_opt(j);
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

  microseconds tot_time = Run_ksw_moddown_rescale_base(vec1, vec2);
  Report_time(prefix, tot_time, Get_num_iter());
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_opt) {
  string      prefix = "Run_ksw_moddown_rescale_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time = Run_ksw_moddown_rescale_opt(vec1, vec2);
  Report_time(prefix, tot_time, Get_num_iter());
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_modup_base) {
  string      prefix = "Run_ksw_moddown_rescale_modup_base ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time = Run_ksw_moddown_rescale_modup_base(vec1, vec2, 2, 3);
  Report_time(prefix, tot_time, Get_num_iter());
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_KSW_OPT, Run_ksw_moddown_rescale_modup_opt) {
  string      prefix = "Run_ksw_moddown_rescale_modup_opt ";
  uint32_t    len    = Get_degree() / 2;
  VALUE_LIST* vec1   = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2   = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);

  microseconds tot_time = Run_ksw_moddown_rescale_modup_opt(vec1, vec2, 2, 3);
  Report_time(prefix, tot_time, Get_num_iter());
  Free_value_list(vec1);
  Free_value_list(vec2);
}