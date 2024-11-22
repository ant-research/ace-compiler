//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "helper.h"
#include "util/ciphertext.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_encoder.h"
#include "util/ckks_encryptor.h"
#include "util/ckks_evaluator.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"
#include "util/fhe_types.h"
#include "util/plaintext.h"
#include "util/public_key.h"
#include "util/random_sample.h"
#include "util/secret_key.h"

class TEST_EVALUATOR : public ::testing::Test {
protected:
  void SetUp() override {
    _degree = 16;
    _param  = Alloc_ckks_parameter();
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, 8, 33,
                                         30, 0);
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _relin_key = Get_relin_key(_keygen);
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _evaluator = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
  }

  void TearDown() override {
    Free_ckks_parameters(_param);
    Free_ckks_key_generator(_keygen);
    Free_ckks_encoder(_encoder);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_evaluator(_evaluator);
  }

  size_t Get_degree() { return _degree; }
  size_t Get_depth() { return Get_mult_depth(_param); }
  void   Run_test_add(VALUE_LIST* msg1, VALUE_LIST* msg2, bool with_secrey_key);
  void   Run_test_add_plain(VALUE_LIST* msg1, VALUE_LIST* msg2);
  void   Run_test_sub_ciph(VALUE_LIST* msg1, VALUE_LIST* msg2);
  void   Run_test_mul_ciphertext(VALUE_LIST* msg1, VALUE_LIST* msg2,
                                 bool with_secrey_key);
  void   Run_test_multiply_plain(VALUE_LIST* msg1, VALUE_LIST* msg2);
  void   Run_test_rescale(VALUE_LIST* msg);
  void   Run_test_downscale(VALUE_LIST* vec1, VALUE_LIST* vec2);
  void   Run_test_mul_const(VALUE_LIST* msg1, double val);
  void   Run_test_add_const(VALUE_LIST* msg1, double val, size_t level);

private:
  size_t              _degree;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_EVALUATOR*     _evaluator;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
  SWITCH_KEY*         _relin_key;
};

void TEST_EVALUATOR::Run_test_add(VALUE_LIST* msg1, VALUE_LIST* msg2,
                                  bool with_secrey_key) {
  size_t msg1_len = LIST_LEN(msg1);
  size_t msg2_len = LIST_LEN(msg2);
  EXPECT_EQ(msg1_len, msg2_len);

  PLAINTEXT*  plain1        = Alloc_plaintext();
  PLAINTEXT*  plain2        = Alloc_plaintext();
  CIPHERTEXT* ciph1         = Alloc_ciphertext();
  CIPHERTEXT* ciph2         = Alloc_ciphertext();
  CIPHERTEXT* ciph_sum      = Alloc_ciphertext();
  PLAINTEXT*  decrypted_sum = Alloc_plaintext();
  VALUE_LIST* decoded_sum   = Alloc_value_list(DCMPLX_TYPE, msg1_len);

  ENCODE(plain1, _encoder, msg1);
  ENCODE(plain2, _encoder, msg2);
  Encrypt_msg(ciph1, _encryptor, plain1);
  Encrypt_msg(ciph2, _encryptor, plain2);
  Add_ciphertext(ciph_sum, ciph1, ciph2, _evaluator);
  Decrypt(decrypted_sum, _decryptor, ciph_sum, NULL);
  Decode(decoded_sum, _encoder, decrypted_sum);

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
}

void TEST_EVALUATOR::Run_test_add_plain(VALUE_LIST* msg1, VALUE_LIST* msg2) {
  size_t msg1_len = LIST_LEN(msg1);

  PLAINTEXT*  plain1        = Alloc_plaintext();
  PLAINTEXT*  plain2        = Alloc_plaintext();
  CIPHERTEXT* ciph1         = Alloc_ciphertext();
  CIPHERTEXT* ciph_sum      = Alloc_ciphertext();
  PLAINTEXT*  decrypted_sum = Alloc_plaintext();
  VALUE_LIST* decoded_sum   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
  VALUE_LIST* plain_sum     = Alloc_value_list(DCMPLX_TYPE, _degree / 2);

  ENCODE(plain1, _encoder, msg1);
  ENCODE(plain2, _encoder, msg2);

  for (size_t i = 0; i < _degree / 2; i++) {
    DCMPLX_VALUE_AT(plain_sum, i) =
        DCMPLX_VALUE_AT(msg1, i) + DCMPLX_VALUE_AT(msg2, i);
  }
  Encrypt_msg(ciph1, _encryptor, plain1);
  Add_plaintext(ciph_sum, ciph1, plain2, _evaluator);
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
}

void TEST_EVALUATOR::Run_test_mul_ciphertext(VALUE_LIST* msg1, VALUE_LIST* msg2,
                                             bool with_secret_key) {
  size_t msg1_len  = LIST_LEN(msg1);
  size_t num_slots = msg1_len;

  PLAINTEXT*  plain1         = Alloc_plaintext();
  PLAINTEXT*  plain2         = Alloc_plaintext();
  CIPHERTEXT* ciph1          = Alloc_ciphertext();
  CIPHERTEXT* ciph2          = Alloc_ciphertext();
  CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
  PLAINTEXT*  decrypted_prod = Alloc_plaintext();
  VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
  VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, num_slots);

  ENCODE(plain1, _encoder, msg1);
  ENCODE(plain2, _encoder, msg2);
  for (size_t i = 0; i < num_slots; i++) {
    DCMPLX_VALUE_AT(plain_prod, i) =
        DCMPLX_VALUE_AT(msg1, i) * DCMPLX_VALUE_AT(msg2, i);
  }
  Encrypt_msg(ciph1, _encryptor, plain1);
  Encrypt_msg(ciph2, _encryptor, plain2);
  Mul_ciphertext(ciph_prod, ciph1, ciph2, _relin_key, _evaluator);
  CIPHERTEXT* rescale_prod = Alloc_ciphertext();
  Rescale_ciphertext(rescale_prod, ciph_prod, _evaluator);

  Decrypt(decrypted_prod, _decryptor, rescale_prod, NULL);
  Decode(decoded_prod, _encoder, decrypted_prod);
  Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.01);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph2);
  Free_ciphertext(ciph_prod);
  Free_ciphertext(rescale_prod);
  Free_plaintext(decrypted_prod);
  Free_value_list(decoded_prod);
  Free_value_list(plain_prod);
}

void TEST_EVALUATOR::Run_test_multiply_plain(VALUE_LIST* msg1,
                                             VALUE_LIST* msg2) {
  size_t msg1_len = LIST_LEN(msg1);
  size_t len      = _degree / 2;

  PLAINTEXT*  plain1         = Alloc_plaintext();
  PLAINTEXT*  plain2         = Alloc_plaintext();
  CIPHERTEXT* ciph1          = Alloc_ciphertext();
  CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
  PLAINTEXT*  decrypted_prod = Alloc_plaintext();
  VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
  VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain1, _encoder, msg1);
  ENCODE(plain2, _encoder, msg2);

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(plain_prod, i) =
        DCMPLX_VALUE_AT(msg1, i) * DCMPLX_VALUE_AT(msg2, i);
  }
  Encrypt_msg(ciph1, _encryptor, plain1);
  Mul_plaintext(ciph_prod, ciph1, plain2, _evaluator);

  CIPHERTEXT* rescale_prod = Alloc_ciphertext();
  Rescale_ciphertext(rescale_prod, ciph_prod, _evaluator);

  Decrypt(decrypted_prod, _decryptor, rescale_prod, NULL);
  Decode(decoded_prod, _encoder, decrypted_prod);
  Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.001);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph_prod);
  Free_ciphertext(rescale_prod);
  Free_plaintext(decrypted_prod);
  Free_value_list(decoded_prod);
  Free_value_list(plain_prod);
}

void TEST_EVALUATOR::Run_test_mul_const(VALUE_LIST* msg1, double val) {
  size_t msg1_len = LIST_LEN(msg1);
  size_t len      = msg1_len;

  PLAINTEXT*  plain1         = Alloc_plaintext();
  PLAINTEXT*  plain2         = Alloc_plaintext();
  CIPHERTEXT* ciph1          = Alloc_ciphertext();
  CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
  PLAINTEXT*  decrypted_prod = Alloc_plaintext();
  VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
  VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, len);

  ENCODE(plain1, _encoder, msg1);

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(plain_prod, i) =
        DCMPLX_VALUE_AT(msg1, i) * DCMPLX_VALUE_AT(msg1, i) * val;
  }
  Encrypt_msg(ciph1, _encryptor, plain1);
  Mul_ciphertext(ciph1, ciph1, ciph1, _relin_key, _evaluator);

  Mul_const(ciph_prod, ciph1, val, _evaluator);

  while (Get_ciph_sf_degree(ciph_prod) > 1) {
    Rescale_ciphertext(ciph_prod, ciph_prod, _evaluator);
  }

  Decrypt(decrypted_prod, _decryptor, ciph_prod, NULL);
  Decode(decoded_prod, _encoder, decrypted_prod);

  Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.001);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph_prod);
  Free_plaintext(decrypted_prod);
  Free_value_list(decoded_prod);
  Free_value_list(plain_prod);
}

void TEST_EVALUATOR::Run_test_add_const(VALUE_LIST* msg1, double val,
                                        size_t level) {
  size_t msg1_len = LIST_LEN(msg1);
  size_t len      = msg1_len;

  PLAINTEXT*  plain1         = Alloc_plaintext();
  PLAINTEXT*  plain2         = Alloc_plaintext();
  CIPHERTEXT* ciph1          = Alloc_ciphertext();
  CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
  PLAINTEXT*  decrypted_prod = Alloc_plaintext();
  VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
  VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, len);

  Encode_at_level_internal(plain1, _encoder, msg1, level, 0);

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(plain_prod, i) =
        DCMPLX_VALUE_AT(msg1, i) * DCMPLX_VALUE_AT(msg1, i) + val;
  }
  Encrypt_msg(ciph1, _encryptor, plain1);

  Mul_ciphertext(ciph1, ciph1, ciph1, _relin_key, _evaluator);
  Add_const(ciph_prod, ciph1, val, _evaluator);

  Decrypt(decrypted_prod, _decryptor, ciph_prod, NULL);
  Decode(decoded_prod, _encoder, decrypted_prod);
  Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.001);
  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_ciphertext(ciph1);
  Free_ciphertext(ciph_prod);
  Free_plaintext(decrypted_prod);
  Free_value_list(decoded_prod);
  Free_value_list(plain_prod);
}

// PI * x^3 + 0.4 x + 1
void TEST_EVALUATOR::Run_test_rescale(VALUE_LIST* msg) {
  size_t msg_len = LIST_LEN(msg);
  size_t len     = _degree / 2;

  PLAINTEXT*  plain          = Alloc_plaintext();
  PLAINTEXT*  pi_plain       = Alloc_plaintext();
  PLAINTEXT*  const1         = Alloc_plaintext();
  PLAINTEXT*  const2         = Alloc_plaintext();
  CIPHERTEXT* ciph           = Alloc_ciphertext();
  CIPHERTEXT* x_ciph         = Alloc_ciphertext();
  CIPHERTEXT* x2_ciph        = Alloc_ciphertext();
  CIPHERTEXT* pi_x_ciph      = Alloc_ciphertext();
  CIPHERTEXT* rescale_prod   = Alloc_ciphertext();
  PLAINTEXT*  decrypted_prod = Alloc_plaintext();
  VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, msg_len);
  VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* pi_value       = Alloc_value_list(DCMPLX_TYPE, msg_len);
  VALUE_LIST* const1_value   = Alloc_value_list(DCMPLX_TYPE, msg_len);
  VALUE_LIST* const2_value   = Alloc_value_list(DCMPLX_TYPE, msg_len);

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(pi_value, i)     = M_PI;
    DCMPLX_VALUE_AT(const1_value, i) = 0.4;
    DCMPLX_VALUE_AT(const2_value, i) = 1.0;
    DCMPLX_VALUE_AT(plain_prod, i) =
        ((M_PI * pow(DCMPLX_VALUE_AT(msg, i), 2) + 0.4) *
         DCMPLX_VALUE_AT(msg, i)) +
        1.0;
  }

  ENCODE(plain, _encoder, msg);
  Encrypt_msg(ciph, _encryptor, plain);
  // x^2
  // level(ciph) = 4
  Mul_ciphertext(x2_ciph, ciph, ciph, _relin_key, _evaluator);
  Rescale_ciphertext(x2_ciph, x2_ciph, _evaluator);
  // PI * x
  ENCODE(pi_plain, _encoder, pi_value);
  Mul_plaintext(pi_x_ciph, ciph, pi_plain, _evaluator);
  Rescale_ciphertext(pi_x_ciph, pi_x_ciph, _evaluator);
  // PI * x^3 = PI * x * x^2
  // x2_ciph(level) = 3, pi_x_ciph(level) = 3
  Mul_ciphertext(rescale_prod, x2_ciph, pi_x_ciph, _relin_key, _evaluator);
  Rescale_ciphertext(rescale_prod, rescale_prod, _evaluator);
  // 0.4 * x
  ENCODE(const1, _encoder, const1_value);
  Mul_plaintext(x_ciph, ciph, const1, _evaluator);
  Rescale_ciphertext(x_ciph, x_ciph, _evaluator);
  // PI * x^3 + 0.4x
  // const1(level) = 4; rescale_prod(level) = 2
  Add_ciphertext(rescale_prod, rescale_prod, x_ciph, _evaluator);
  // PI*x^3 + 0.4x + 1
  ENCODE(const2, _encoder, const2_value);
  // consts(level) = 4; rescale_prod(level) = 1
  Save_poly_level(Get_plain_poly(const2), Get_poly_level(Get_c0(rescale_prod)));
  Add_plaintext(rescale_prod, rescale_prod, const2, _evaluator);

  Decrypt(decrypted_prod, _decryptor, rescale_prod, NULL);
  Decode(decoded_prod, _encoder, decrypted_prod);
  Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.001);

  Free_plaintext(plain);
  Free_plaintext(pi_plain);
  Free_plaintext(const1);
  Free_plaintext(const2);
  Free_plaintext(decrypted_prod);
  Free_ciphertext(ciph);
  Free_ciphertext(x_ciph);
  Free_ciphertext(x2_ciph);
  Free_ciphertext(pi_x_ciph);
  Free_ciphertext(rescale_prod);
  Free_value_list(decoded_prod);
  Free_value_list(plain_prod);
  Free_value_list(pi_value);
  Free_value_list(const1_value);
  Free_value_list(const2_value);
}

// Test Downscale_ciphertext() for (x^2 + y^2)^3
void TEST_EVALUATOR::Run_test_downscale(VALUE_LIST* vec1, VALUE_LIST* vec2) {
  size_t vec_len = LIST_LEN(vec1);
  IS_TRUE(vec_len == LIST_LEN(vec2), "dismatch input vector");
  size_t len = _degree / 2;

  PLAINTEXT*  plain1         = Alloc_plaintext();
  PLAINTEXT*  plain2         = Alloc_plaintext();
  CIPHERTEXT* x              = Alloc_ciphertext();
  CIPHERTEXT* y              = Alloc_ciphertext();
  CIPHERTEXT* x2             = Alloc_ciphertext();
  CIPHERTEXT* y2             = Alloc_ciphertext();
  CIPHERTEXT* z              = Alloc_ciphertext();
  CIPHERTEXT* z2             = Alloc_ciphertext();
  CIPHERTEXT* z3             = Alloc_ciphertext();
  PLAINTEXT*  decrypted_prod = Alloc_plaintext();
  VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, vec_len);
  VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, len);

  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(plain_prod, i) = pow(
        pow(DCMPLX_VALUE_AT(vec1, i), 2) + pow(DCMPLX_VALUE_AT(vec2, i), 2), 3);
  }

  double   scale     = pow(2, 25);
  uint32_t waterline = 25;

  // encode x & y with scale = 2^25
  Encode_at_level_with_scale(plain1, _encoder, vec1, 0 /*level*/, 0 /*slots*/,
                             scale, 0);
  Encode_at_level_with_scale(plain2, _encoder, vec2, 0 /*level*/, 0 /*slots*/,
                             scale, 0);
  Encrypt_msg(x, _encryptor, plain1);
  Encrypt_msg(y, _encryptor, plain2);
  // z = x^2 + y^2 (scale = 2^50)
  Mul_ciphertext(x2, x, x, _relin_key, _evaluator);
  Mul_ciphertext(y2, y, y, _relin_key, _evaluator);
  Add_ciphertext(z, x2, y2, _evaluator);
  // downscale z to waterline(2^25)
  Downscale_ciphertext(z, z, waterline, _evaluator);
  // z^3 = z^2 * z (scale = 2^75)
  Mul_ciphertext(z2, z, z, _relin_key, _evaluator);
  Mul_ciphertext(z3, z2, z, _relin_key, _evaluator);

  Decrypt(decrypted_prod, _decryptor, z3, NULL);
  Decode(decoded_prod, _encoder, decrypted_prod);
  Calculate_approx_max_error(plain_prod, decoded_prod);
  Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.001);

  Free_plaintext(plain1);
  Free_plaintext(plain2);
  Free_plaintext(decrypted_prod);
  Free_ciphertext(x);
  Free_ciphertext(y);
  Free_ciphertext(x2);
  Free_ciphertext(y2);
  Free_ciphertext(z);
  Free_ciphertext(z2);
  Free_ciphertext(z3);
  Free_value_list(decoded_prod);
  Free_value_list(plain_prod);
}

TEST_F(TEST_EVALUATOR, test_add_01) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  VALUE_LIST* vec2 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
  Run_test_add(vec1, vec2, false);
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_EVALUATOR, test_add_plain) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  VALUE_LIST* vec2 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
  Run_test_add_plain(vec1, vec2);
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_EVALUATOR, test_multiply_01) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  VALUE_LIST* vec2 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
  Run_test_mul_ciphertext(vec1, vec2, false);
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_EVALUATOR, test_multiply_plain_01) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  VALUE_LIST* vec2 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
  Run_test_multiply_plain(vec1, vec2);
  Free_value_list(vec1);
  Free_value_list(vec2);
}

TEST_F(TEST_EVALUATOR, test_mul_const) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(Get_dcmplx_values(vec1), len);
  Run_test_mul_const(vec1, 0.5);
  Free_value_list(vec1);
}

TEST_F(TEST_EVALUATOR, test_add_const) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(Get_dcmplx_values(vec1), len);
  Run_test_add_const(vec1, -7.75, Get_depth() + 1);
  Free_value_list(vec1);
}

TEST_F(TEST_EVALUATOR, test_add_const_at_level) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(Get_dcmplx_values(vec1), len);
  Run_test_add_const(vec1, -7.75, Get_depth() - 1);
  Free_value_list(vec1);
}

TEST_F(TEST_EVALUATOR, test_rescale) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  Run_test_rescale(vec1);
  Free_value_list(vec1);
}

TEST_F(TEST_EVALUATOR, test_downscale) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec1), len);
  VALUE_LIST* vec2 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec2), len);
  Run_test_downscale(vec1, vec2);
  Free_value_list(vec1);
  Free_value_list(vec2);
}

class TEST_EVALUATOR_EXTRA : public ::testing::Test {
protected:
  void SetUp() override {
    _degree = 16;
    _param  = Alloc_ckks_parameter();
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, 3, 60,
                                         20, 0);
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _relin_key = Get_relin_key(_keygen);
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _evaluator = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
  }

  void TearDown() override {
    Free_ckks_parameters(_param);
    Free_ckks_key_generator(_keygen);
    Free_ckks_encoder(_encoder);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_evaluator(_evaluator);
  }
  size_t Get_degree() { return _degree; }
  void   Run_test_mul_const(VALUE_LIST* msg1, double val,
                            uint32_t encode_sf_degree) {
    size_t msg1_len = LIST_LEN(msg1);
    size_t len      = msg1_len;

    PLAINTEXT*  plain1         = Alloc_plaintext();
    PLAINTEXT*  plain2         = Alloc_plaintext();
    CIPHERTEXT* ciph1          = Alloc_ciphertext();
    CIPHERTEXT* ciph_prod      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_prod = Alloc_plaintext();
    VALUE_LIST* decoded_prod   = Alloc_value_list(DCMPLX_TYPE, msg1_len);
    VALUE_LIST* plain_prod     = Alloc_value_list(DCMPLX_TYPE, len);

    Encode_at_level_with_sf(plain1, _encoder, msg1, 0, 0, encode_sf_degree);

    for (size_t i = 0; i < len; i++) {
      DCMPLX_VALUE_AT(plain_prod, i) = DCMPLX_VALUE_AT(msg1, i) * val;
    }
    Encrypt_msg(ciph1, _encryptor, plain1);

    Mul_const(ciph_prod, ciph1, val, _evaluator);

    Decrypt(decrypted_prod, _decryptor, ciph_prod, NULL);
    Decode(decoded_prod, _encoder, decrypted_prod);

    Check_complex_vector_approx_eq(plain_prod, decoded_prod, 0.001);
    Free_plaintext(plain1);
    Free_plaintext(plain2);
    Free_ciphertext(ciph1);
    Free_ciphertext(ciph_prod);
    Free_plaintext(decrypted_prod);
    Free_value_list(decoded_prod);
    Free_value_list(plain_prod);
  }

private:
  size_t              _degree;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_EVALUATOR*     _evaluator;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
  SWITCH_KEY*         _relin_key;
};

TEST_F(TEST_EVALUATOR_EXTRA, test_mul_const_01) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(Get_dcmplx_values(vec1), len);
  // In this case:
  // q_cnt = 2;     bit_counts_coeff = 60 + (3 - 1) * 20 = 100
  // sf_degree = 4; bit_counts_sf = (3 + 1) * 20 = 80
  // scaling factor is within bounds of coefficient modulus
  Run_test_mul_const(vec1, 0.5, 3);
  Free_value_list(vec1);
}

TEST_F(TEST_EVALUATOR_EXTRA, test_mul_const_02) {
  size_t      len  = Get_degree() / 2;
  VALUE_LIST* vec1 = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(Get_dcmplx_values(vec1), len);
  // In this case:
  // q_cnt = 2;     bit_counts_coeff = 60 + (3 - 1) * 20 = 100
  // sf_degree = 5; bit_counts_sf = (4 + 1) * 20 = 100
  // scaling factor out of bounds of coefficient modulus
  // should report assertion at Mul_plaintext or Decode
  EXPECT_DEATH(Run_test_mul_const(vec1, 0.5, 4), "");
  Free_value_list(vec1);
}