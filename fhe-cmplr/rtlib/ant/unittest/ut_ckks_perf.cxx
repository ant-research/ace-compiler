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

// Need to run test with a command line argument with for the polynomial degree.
// For multiplication with polynomial degree 16, run
//   TEST_DEGREE=16 ./c_fhe_all_tests --gtest_filter= TEST_EVALUATOR_PERF*
// For bootstrapping with polynomial degree 32, run
//   TEST_DEGREE=32 ./c_fhe_all_tests --gtest_filter= TEST_BOOTSTRAP_PERF*
class TEST_EVALUATOR_PERF : public ::testing::Test {
protected:
  void SetUp() override {
    _degree         = UT_GLOB_ENV::Get_env(TEST_DEGREE);
    size_t parts    = UT_GLOB_ENV::Get_env(NUM_Q_PART);
    size_t num_q    = UT_GLOB_ENV::Get_env(NUM_Q);
    _num_iterations = UT_GLOB_ENV::Get_env(ITERATION);
    _param          = Alloc_ckks_parameter();
    Set_num_q_parts(_param, parts);
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, num_q,
                                         UT_GLOB_ENV::Get_env(Q0_BITS),
                                         UT_GLOB_ENV::Get_env(SF_BITS), 0);
    cout << "Degree = " << _degree << " log(q0) = " << _param->_first_mod_size
         << " log(scaling_factor) = " << log2(Get_param_sc(_param))
         << " num_q = " << _param->_num_primes
         << " num_p = " << _param->_num_p_primes
         << " q_part = " << _param->_num_q_parts << endl;
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _relin_key = _keygen->_relin_key;
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _evaluator = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
  }

  void TearDown() override {
    RTLIB_TM_REPORT();
    Free_ckks_evaluator(_evaluator);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_encoder(_encoder);
    Free_ckks_key_generator(_keygen);
    Free_ckks_parameters(_param);
  }

  size_t Get_degree() { return _degree; }
  size_t Get_num_iter() { return _num_iterations; }

  microseconds Run_test_add(VALUE_LIST* msg1, VALUE_LIST* msg2,
                            microseconds& encode_time,
                            microseconds& decode_time,
                            microseconds& encrypt_time,
                            microseconds& decrypt_time) {
    size_t      msg1_len      = LIST_LEN(msg1);
    PLAINTEXT*  plain1        = Alloc_plaintext();
    PLAINTEXT*  plain2        = Alloc_plaintext();
    CIPHERTEXT* ciph1         = Alloc_ciphertext();
    CIPHERTEXT* ciph2         = Alloc_ciphertext();
    CIPHERTEXT* ciph_sum      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_sum = Alloc_plaintext();
    VALUE_LIST* decoded_sum   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
    auto        start         = chrono::system_clock::now();
    ENCODE(plain1, _encoder, msg1);
    auto end    = chrono::system_clock::now();
    encode_time = duration_cast<microseconds>(end - start);
    ENCODE(plain2, _encoder, msg2);

    start = chrono::system_clock::now();
    Encrypt_msg(ciph1, _encryptor, plain1);
    end          = chrono::system_clock::now();
    encrypt_time = duration_cast<microseconds>(end - start);
    Encrypt_msg(ciph2, _encryptor, plain2);

    start = chrono::system_clock::now();
    Add_ciphertext(ciph_sum, ciph1, ciph2, _evaluator);
    end = chrono::system_clock::now();
    microseconds ret(0);
    ret = duration_cast<microseconds>(end - start);

    start = chrono::system_clock::now();
    Decrypt(decrypted_sum, _decryptor, ciph_sum, NULL);
    end          = chrono::system_clock::now();
    decrypt_time = duration_cast<microseconds>(end - start);

    start = chrono::system_clock::now();
    Decode(decoded_sum, _encoder, decrypted_sum);
    end         = chrono::system_clock::now();
    decode_time = duration_cast<microseconds>(end - start);

    VALUE_LIST* sum = Alloc_value_list(DCMPLX_TYPE, msg1_len);
    for (size_t idx = 0; idx < msg1_len; idx++) {
      DCMPLX_VALUE_AT(sum, idx) =
          DCMPLX_VALUE_AT(msg1, idx) + DCMPLX_VALUE_AT(msg2, idx);
    }
    Check_complex_vector_approx_eq(sum, decoded_sum, 0.005);
    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph2);
    Free_ciphertext(ciph_sum);
    Free_plaintext(decrypted_sum);
    Free_value_list(decoded_sum);
    Free_value_list(sum);
    return ret;
  }

  microseconds Run_test_add_plain(VALUE_LIST* msg1, VALUE_LIST* msg2) {
    size_t      msg1_len      = LIST_LEN(msg1);
    PLAINTEXT*  plain1        = Alloc_plaintext();
    PLAINTEXT*  plain2        = Alloc_plaintext();
    CIPHERTEXT* ciph1         = Alloc_ciphertext();
    CIPHERTEXT* ciph_sum      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_sum = Alloc_plaintext();
    VALUE_LIST* decoded_sum   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
    VALUE_LIST* plain_sum     = Alloc_value_list(DCMPLX_TYPE, _degree / 2);
    for (size_t i = 0; i < _degree / 2; i++) {
      DCMPLX_VALUE_AT(plain_sum, i) =
          DCMPLX_VALUE_AT(msg1, i) + DCMPLX_VALUE_AT(msg2, i);
    }
    ENCODE(plain1, _encoder, msg1);
    ENCODE(plain2, _encoder, msg2);
    Encrypt_msg(ciph1, _encryptor, plain1);

    auto start = chrono::system_clock::now();
    Add_plaintext(ciph_sum, ciph1, plain2, _evaluator);
    auto         end = chrono::system_clock::now();
    microseconds ret(0);
    ret = duration_cast<microseconds>(end - start);

    Decrypt(decrypted_sum, _decryptor, ciph_sum, NULL);
    Decode(decoded_sum, _encoder, decrypted_sum);

    Check_complex_vector_approx_eq(plain_sum, decoded_sum, 0.001);
    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph_sum);
    Free_plaintext(decrypted_sum);
    Free_value_list(decoded_sum);
    Free_value_list(plain_sum);
    return duration_cast<microseconds>(end - start);
  }

  microseconds Run_test_multiply_plain(VALUE_LIST* msg1, VALUE_LIST* msg2,
                                       microseconds& rescale_time) {
    size_t      num_slots      = LIST_LEN(msg1);
    PLAINTEXT*  plain1         = Alloc_plaintext();
    PLAINTEXT*  plain2         = Alloc_plaintext();
    CIPHERTEXT* ciph1          = Alloc_ciphertext();
    CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_prod = Alloc_plaintext();
    VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, num_slots);
    VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, num_slots);

    for (size_t i = 0; i < num_slots; i++) {
      DCMPLX_VALUE_AT(plain_prod, i) =
          DCMPLX_VALUE_AT(msg1, i) * DCMPLX_VALUE_AT(msg2, i);
    }
    ENCODE(plain1, _encoder, msg1);
    ENCODE(plain2, _encoder, msg2);
    Encrypt_msg(ciph1, _encryptor, plain1);

    auto start = chrono::system_clock::now();
    Mul_plaintext(ciph_prod, ciph1, plain2, _evaluator);
    auto         end = chrono::system_clock::now();
    microseconds ret(0);
    ret = duration_cast<microseconds>(end - start);

    start = chrono::system_clock::now();
    Rescale_ciphertext(ciph_prod, ciph_prod, _evaluator);
    end          = chrono::system_clock::now();
    rescale_time = duration_cast<microseconds>(end - start);

    Decrypt(decrypted_prod, _decryptor, ciph_prod, NULL);
    Decode(decoded_prod, _encoder, decrypted_prod);
    Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.01);
    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph_prod);
    Free_plaintext(decrypted_prod);
    Free_value_list(decoded_prod);
    Free_value_list(plain_prod);
    return ret;
  }

  microseconds Run_test_multiply(VALUE_LIST* msg1, VALUE_LIST* msg2,
                                 microseconds& nontt_rescale_time) {
    size_t      num_slots      = LIST_LEN(msg1);
    PLAINTEXT*  plain1         = Alloc_plaintext();
    PLAINTEXT*  plain2         = Alloc_plaintext();
    CIPHERTEXT* ciph1          = Alloc_ciphertext();
    CIPHERTEXT* ciph2          = Alloc_ciphertext();
    CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_prod = Alloc_plaintext();
    VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, num_slots);
    VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, num_slots);
    for (size_t i = 0; i < num_slots; i++) {
      DCMPLX_VALUE_AT(plain_prod, i) =
          DCMPLX_VALUE_AT(msg1, i) * DCMPLX_VALUE_AT(msg2, i);
    }

    ENCODE(plain1, _encoder, msg1);
    ENCODE(plain2, _encoder, msg2);
    Encrypt_msg(ciph1, _encryptor, plain1);
    Encrypt_msg(ciph2, _encryptor, plain2);
    auto start = chrono::system_clock::now();
    Mul_ciphertext(ciph_prod, ciph1, ciph2, _relin_key, _evaluator);
    auto         end = chrono::system_clock::now();
    microseconds ret(0);
    ret = duration_cast<microseconds>(end - start);

    // only for performance test: ignore conv time from ntt to intt
    if (Is_ntt(Get_c0(ciph_prod))) {
      Conv_ntt2poly_inplace(Get_c0(ciph_prod), _param->_crt_context);
    }
    if (Is_ntt(Get_c1(ciph_prod))) {
      Conv_ntt2poly_inplace(Get_c1(ciph_prod), _param->_crt_context);
    }
    start = chrono::system_clock::now();
    Rescale_ciphertext(ciph_prod, ciph_prod, _evaluator);
    end                = chrono::system_clock::now();
    nontt_rescale_time = duration_cast<microseconds>(end - start);

    Decrypt(decrypted_prod, _decryptor, ciph_prod, NULL);
    Decode(decoded_prod, _encoder, decrypted_prod);
    Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.01);
    Free_plaintext(decrypted_prod);
    Free_ciphertext(ciph_prod);
    Free_ciphertext(ciph2);
    Free_ciphertext(ciph1);
    Free_plaintext(plain2);
    Free_plaintext(plain1);
    Free_value_list(decoded_prod);
    Free_value_list(plain_prod);
    return ret;
  }

  microseconds Run_test_rotate(VALUE_LIST* vec, int32_t rot,
                               microseconds& gen_rot_key_time) {
    size_t length     = _degree / 2;
    size_t vec_length = LIST_LEN(vec);

    PLAINTEXT*  plain         = Alloc_plaintext();
    CIPHERTEXT* ciph          = Alloc_ciphertext();
    SWITCH_KEY* rot_key       = Alloc_switch_key();
    CIPHERTEXT* ciph_rot      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_rot = Alloc_plaintext();
    VALUE_LIST* decoded_rot   = Alloc_value_list(DCMPLX_TYPE, vec_length);

    VALUE_LIST* rot_msg = Alloc_value_list(DCMPLX_TYPE, vec_length);
    for (size_t idx = 0; idx < vec_length; idx++) {
      DCMPLX_VALUE_AT(rot_msg, idx) =
          Get_dcmplx_value_at(vec, (idx + rot) % length);
    }

    ENCODE(plain, _encoder, vec);
    Encrypt_msg(ciph, _encryptor, plain);

    auto start = chrono::system_clock::now();
    // generate precompute automorphism index
    Insert_rot_map(_keygen, rot);
    auto end         = chrono::system_clock::now();
    gen_rot_key_time = duration_cast<microseconds>(end - start);

    start             = chrono::system_clock::now();
    uint32_t auto_idx = Get_precomp_auto_idx(_keygen, rot);
    IS_TRUE(auto_idx, "cannot get precompute automorphism index");
    rot_key = Get_auto_key(_keygen, auto_idx);
    IS_TRUE(rot_key, "cannot find auto key");
    Eval_fast_rotate(ciph_rot, ciph, rot, rot_key, _evaluator);
    end = chrono::system_clock::now();

    Decrypt(decrypted_rot, _decryptor, ciph_rot, NULL);
    Decode(decoded_rot, _encoder, decrypted_rot);

    Check_complex_vector_approx_eq(rot_msg, decoded_rot, 0.005);
    Free_value_list(decoded_rot);
    Free_plaintext(decrypted_rot);
    Free_ciphertext(ciph_rot);
    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_value_list(rot_msg);
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
};

TEST_F(TEST_EVALUATOR_PERF, Run_test_add) {
  microseconds total(0);
  microseconds encode_total(0);
  microseconds decode_total(0);
  microseconds encrypt_total(0);
  microseconds decrypt_total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds encode_time(0);
    microseconds decode_time(0);
    microseconds encrypt_time(0);
    microseconds decrypt_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_add(vec1, vec2, encode_time, decode_time, encrypt_time,
                          decrypt_time);
    encode_total += encode_time;
    decode_total += decode_time;
    encrypt_total += encrypt_time;
    decrypt_total += decrypt_time;
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Encode:" << right << setw(10)
       << (double)encode_total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "Decode:" << right << setw(10)
       << (double)decode_total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "Encrypt:" << right << setw(10)
       << (double)encrypt_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl
       << left << setw(24) << "Decrypt:" << right << setw(10)
       << (double)decrypt_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl
       << left << setw(24) << "Add:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_EVALUATOR_PERF, Run_test_add_plain) {
  microseconds total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* vec2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
    total += Run_test_add_plain(vec1, vec2);
  }
  Free_value_list(vec1);
  Free_value_list(vec2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Add plain:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_EVALUATOR_PERF, Run_test_multiply_plain) {
  microseconds total(0);
  microseconds rescale_total(0);

  size_t      len            = Get_degree() / 2;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* msg1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* msg2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds rescale_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(msg1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(msg2), len);
    total += Run_test_multiply_plain(msg1, msg2, rescale_time);
    rescale_total += rescale_time;
  }
  Free_value_list(msg1);
  Free_value_list(msg2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Multiply plain:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "Rescale(ntt-input):" << right << setw(10)
       << (double)rescale_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_EVALUATOR_PERF, Run_test_multiply) {
  microseconds total_time(0);
  microseconds nontt_rescale_total(0);
  size_t       len            = Get_degree() / 2;
  size_t       num_iterations = Get_num_iter();
  VALUE_LIST*  msg1           = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST*  msg2           = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds nontt_rescale_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(msg1), len);
    Sample_random_complex_vector(DCMPLX_VALUES(msg2), len);
    total_time += Run_test_multiply(msg1, msg2, nontt_rescale_time);
    nontt_rescale_total += nontt_rescale_time;
  }
  Free_value_list(msg1);
  Free_value_list(msg2);
  cout << string(80, '-') << endl
       << left << setw(24) << "Multiply:" << right << setw(10)
       << (double)total_time.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "Rescale(nontt-input):" << right << setw(10)
       << (double)nontt_rescale_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_EVALUATOR_PERF, Run_test_rotate) {
  IS_TRACE_CMD(Print_param(Get_trace_file(), _param));
  size_t num_iterations = Get_num_iter();
  IS_TRACE("\nNumber of multiplications: %ld\n", num_iterations);
  microseconds total(0);
  microseconds gen_rot_key_total(0);

  size_t      length = Get_degree() / 2;
  VALUE_LIST* dc_vec = Alloc_value_list(DCMPLX_TYPE, length);
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds gen_rot_key_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), length);
    total += Run_test_rotate(dc_vec, i * 20, gen_rot_key_time);
    gen_rot_key_total += gen_rot_key_time;
  }

  Free_value_list(dc_vec);
  cout << string(80, '-') << endl
       << left << setw(24) << "Generate rotation key:" << right << setw(10)
       << (double)gen_rot_key_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl
       << left << setw(24) << "Rotate:" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_EVALUATOR_PERF, Run_fwd_ntt) {
  size_t       num_iterations = 5000;
  microseconds total(0);
  size_t       degree = Get_degree();
  VALUE_LIST*  input  = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST*  output = Alloc_value_list(I64_TYPE, degree);

  Sample_uniform(input, 2004342542654);
  Init_i64_value_list(output, degree, Get_i64_values(input));
  NTT_CONTEXT* ntt     = Alloc_nttcontext();
  int64_t      mod_val = 1125899904679937;
  MODULUS      modulus;
  Init_modulus(&modulus, mod_val);
  Init_nttcontext(ntt, degree, &modulus);

  for (size_t i = 0; i < num_iterations; i++) {
    auto start = chrono::system_clock::now();
    Ftt_fwd(output, ntt, ntt->_rou);
    auto end = chrono::system_clock::now();
    total += duration_cast<microseconds>(end - start);
  }

  cout << string(80, '-') << endl
       << left << setw(24) << "fwd ntt:" << right << setw(10)
       << (double)total.count() / num_iterations << " us" << right << setw(24)
       << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
  Free_value_list(input);
  Free_value_list(output);
  Free_nttcontext(ntt);
}

TEST_F(TEST_EVALUATOR_PERF, Run_inv_ntt) {
  size_t       num_iterations = 5000;
  microseconds total(0);
  size_t       degree = Get_degree();
  VALUE_LIST*  input  = Alloc_value_list(I64_TYPE, degree);
  VALUE_LIST*  output = Alloc_value_list(I64_TYPE, degree);
  Sample_uniform(input, 2004342542654);
  NTT_CONTEXT* ntt     = Alloc_nttcontext();
  int64_t      mod_val = 1125899904679937;
  MODULUS      modulus;
  Init_modulus(&modulus, mod_val);
  Init_nttcontext(ntt, degree, &modulus);

  for (size_t i = 0; i < num_iterations; i++) {
    auto start = chrono::system_clock::now();
    Ftt_inv(output, ntt, input);
    auto end = chrono::system_clock::now();
    total += duration_cast<microseconds>(end - start);
  }

  cout << string(80, '-') << endl
       << left << setw(24) << "inv ntt:" << right << setw(10)
       << (double)total.count() / num_iterations << " us" << right << setw(24)
       << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl;
  Free_value_list(input);
  Free_value_list(output);
  Free_nttcontext(ntt);
}

class TEST_BOOTSTRAP_PERF : public ::testing::Test {
protected:
  void SetUp() override {
    _degree                 = UT_GLOB_ENV::Get_env(TEST_DEGREE);
    size_t parts            = UT_GLOB_ENV::Get_env(NUM_Q_PART);
    size_t num_q            = UT_GLOB_ENV::Get_env(NUM_Q);
    size_t scaling_mod_size = UT_GLOB_ENV::Get_env(SF_BITS);
    size_t q0_bits          = UT_GLOB_ENV::Get_env(Q0_BITS);
    _num_iterations         = UT_GLOB_ENV::Get_env(ITERATION);
    // bootstrap need at least 19 levels
    num_q  = num_q < 20 ? 20 : num_q;
    _param = Alloc_ckks_parameter();
    Set_num_q_parts(_param, parts);
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, num_q,
                                         q0_bits, scaling_mod_size, 0);
    cout << "Degree = " << _degree << " log(q0) = " << _param->_first_mod_size
         << " log(scaling_factor) = " << log2(Get_param_sc(_param))
         << " num_q = " << _param->_num_primes
         << " num_p = " << _param->_num_p_primes
         << " q_part = " << _param->_num_q_parts << endl;
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _relin_key = _keygen->_relin_key;
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _evaluator = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
    _bts_ctx   = Get_bts_ctx(_evaluator);
    // do not clear imag part
    Set_rtlib_config(CONF_BTS_CLEAR_IMAG, 0);
  }

  void TearDown() override {
    RTLIB_TM_REPORT();
    Free_ckks_evaluator(_evaluator);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_encoder(_encoder);
    Free_ckks_key_generator(_keygen);
    Free_ckks_parameters(_param);
  }

  size_t Get_degree() { return _degree; }
  size_t Get_num_iter() { return _num_iterations; }

  microseconds Run_test_bootstrap(VALUE_LIST* vec, VL_UI32* level_budget,
                                  VL_UI32* dim1, size_t init_level,
                                  uint32_t      num_slots,
                                  microseconds& bts_setup_time,
                                  microseconds& bts_keygen_time) {
    size_t len = LIST_LEN(vec);

    PLAINTEXT*  plain           = Alloc_plaintext();
    CIPHERTEXT* ciph            = Alloc_ciphertext();
    CIPHERTEXT* res_ciph        = Alloc_ciphertext();
    PLAINTEXT*  decrypted_plain = Alloc_plaintext();
    VALUE_LIST* decoded_val     = Alloc_value_list(DCMPLX_TYPE, len);

    Encode_at_level_internal(plain, _encoder, vec, init_level, num_slots);

    Encrypt_msg(ciph, _encryptor, plain);

    // step 1: set parmeters
    uint32_t num_iteration = 1;
    uint32_t precision     = 0;

    auto start = chrono::system_clock::now();
    // step 2: bootstrap setup
    Bootstrap_setup(_bts_ctx, level_budget, dim1, num_slots);
    auto end       = chrono::system_clock::now();
    bts_setup_time = duration_cast<microseconds>(end - start);

    // step 2: bootstrap keygen
    start = chrono::system_clock::now();
    Bootstrap_keygen(_bts_ctx, num_slots);
    end             = chrono::system_clock::now();
    bts_keygen_time = duration_cast<microseconds>(end - start);

    // step 3: call bootstrap driver
    start = chrono::system_clock::now();
    Eval_bootstrap(res_ciph, ciph, num_iteration, precision, 0, _bts_ctx);
    end = chrono::system_clock::now();

    std::cout << "number of levels remaining after bootstrapping: "
              << Get_poly_level(Get_c0(res_ciph)) << std::endl;

    Decrypt(decrypted_plain, _decryptor, res_ciph, NULL);
    Decode(decoded_val, _encoder, decrypted_plain);

    Check_complex_vector_approx_eq(vec, decoded_val, 0.001);
    Calculate_approx_max_error(vec, decoded_val);

    Free_plaintext(plain);
    Free_ciphertext(ciph);
    Free_ciphertext(res_ciph);
    Free_plaintext(decrypted_plain);
    Free_value_list(decoded_val);
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
  CKKS_BTS_CTX*       _bts_ctx;
};

TEST_F(TEST_BOOTSTRAP_PERF, Run_test_bootstrap_full) {
  microseconds total(0);
  microseconds bts_setup_total(0);
  microseconds bts_keygen_total(0);

  size_t num_slots = Get_degree() / 2;
  // NOTE: current bootstrap precision is not big enough for larger input length
  size_t len = num_slots > 16 ? 16 : num_slots;
  std::cout << "vec_len = " << len << ", slots = " << num_slots << std::endl;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec            = Alloc_value_list(DCMPLX_TYPE, len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  // encode plain at level = 1
  size_t init_level = 1;
  std::cout << "number of levels before bootstrapping: " << init_level
            << std::endl;
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds bts_setup_time(0);
    microseconds bts_keygen_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
    total += Run_test_bootstrap(vec, level_budget, dim1, init_level, num_slots,
                                bts_setup_time, bts_keygen_time);
    bts_setup_total += bts_setup_time;
    bts_keygen_total += bts_keygen_time;
  }
  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
  cout << string(80, '-') << endl
       << left << setw(24) << "Eval_bootstrap(full):" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "BTS_SETUP(full):" << right << setw(10)
       << (double)bts_setup_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl
       << left << setw(24) << "BTS_KEYGEN(full):" << right << setw(10)
       << (double)bts_keygen_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl;
}

TEST_F(TEST_BOOTSTRAP_PERF, Run_test_bootstrap_sparse) {
  microseconds total(0);
  microseconds bts_setup_total(0);
  microseconds bts_keygen_total(0);

  size_t num_slots = Get_degree() / UT_GLOB_ENV::Get_env(SPARSE);
  // NOTE: current bootstrap precision is not big enough for larger input length
  size_t      len            = num_slots > 16 ? 16 : num_slots;
  size_t      num_iterations = Get_num_iter();
  VALUE_LIST* vec            = Alloc_value_list(DCMPLX_TYPE, len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  // encode plain at level = 1
  size_t init_level = 1;
  std::cout << "number of levels before bootstrapping: " << init_level
            << std::endl;
  for (size_t i = 0; i < num_iterations; i++) {
    microseconds bts_setup_time(0);
    microseconds bts_keygen_time(0);
    Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
    total += Run_test_bootstrap(vec, level_budget, dim1, init_level, num_slots,
                                bts_setup_time, bts_keygen_time);
    bts_setup_total += bts_setup_time;
    bts_keygen_total += bts_keygen_time;
  }
  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
  cout << string(80, '-') << endl
       << left << setw(24) << "Eval_bootstrap(slots = " << num_slots
       << "):" << right << setw(10)
       << (double)total.count() / num_iterations / 1000 << " ms" << right
       << setw(24) << "avarage of " << num_iterations << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "BTS_SETUP(slots = " << num_slots << "):" << right
       << setw(10) << (double)bts_setup_total.count() / num_iterations / 1000
       << " ms" << right << setw(24) << "avarage of " << num_iterations
       << " runs" << endl
       << string(80, '-') << endl
       << left << setw(24) << "BTS_KEYGEN(slots = " << num_slots
       << "):" << right << setw(10)
       << (double)bts_keygen_total.count() / num_iterations / 1000 << " ms"
       << right << setw(24) << "avarage of " << num_iterations << " runs"
       << endl
       << string(80, '-') << endl;
}
