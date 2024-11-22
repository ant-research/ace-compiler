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
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"
#include "util/plaintext.h"
#include "util/random_sample.h"

class TEST_CKKS_ENCRYPT_DECRYPTE : public ::testing::Test {
protected:
  void SetUp() override {
    _degree = 4;
    _param  = Alloc_ckks_parameter();
    Init_ckks_parameters_with_multiply_depth(_param, _degree, HE_STD_NOT_SET, 3,
                                             0);
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, _keygen->_public_key,
                                      _keygen->_secret_key);
    _decryptor = Alloc_ckks_decryptor(_param, _keygen->_secret_key);
  }

  size_t Get_degree() { return _degree; }

  void TearDown() override {
    Free_ckks_parameters(_param);
    Free_ckks_key_generator(_keygen);
    Free_ckks_encoder(_encoder);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_decryptor(_decryptor);
  }

  void Run_test_encrypt_decrypt(VALUE_LIST* message) {
    PLAINTEXT*  plain     = Alloc_plaintext();
    CIPHERTEXT* ciph      = Alloc_ciphertext();
    PLAINTEXT*  decrypted = Alloc_plaintext();
    VALUE_LIST* decoded   = Alloc_value_list(DCMPLX_TYPE, LIST_LEN(message));

    ENCODE(plain, _encoder, message);
    Encrypt_msg(ciph, _encryptor, plain);
    Decrypt(decrypted, _decryptor, ciph, NULL);
    Decode(decoded, _encoder, decrypted);
    Check_complex_vector_approx_eq(message, decoded, 0.001);
    Free_plaintext(plain);
    Free_ciphertext(ciph);
    Free_plaintext(decrypted);
    Free_value_list(decoded);
  }

private:
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_DECRYPTOR*     _decryptor;
  size_t              _degree;
};

TEST_F(TEST_CKKS_ENCRYPT_DECRYPTE, test_encrypt_decrypt_01) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_test_encrypt_decrypt(vec);
  Free_value_list(vec);
}
