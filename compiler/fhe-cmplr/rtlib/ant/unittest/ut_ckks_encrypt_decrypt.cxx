//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/ciphertext.h"
#include "ckks/decryptor.h"
#include "ckks/encoder.h"
#include "ckks/encryptor.h"
#include "ckks/key_gen.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "common/rt_api.h"
#include "context/ckks_context.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "util/random_sample.h"

class TEST_CKKS_ENCRYPT_DECRYPTE : public ::testing::Test {
protected:
  void SetUp() override {
    _degree = 4;
    Set_context_params(_degree, 3, 33, 30);
    Prepare_context();
    // TODO: should be removed after refactor CKKS APIs
    _encoder   = (CKKS_ENCODER*)Encoder();
    _decryptor = (CKKS_DECRYPTOR*)Decryptor();
    _encryptor = (CKKS_ENCRYPTOR*)Encryptor();
  }

  size_t Get_degree() { return _degree; }

  void TearDown() override { Finalize_context(); }

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
  CKKS_ENCODER*   _encoder;
  CKKS_ENCRYPTOR* _encryptor;
  CKKS_DECRYPTOR* _decryptor;
  size_t          _degree;
};

TEST_F(TEST_CKKS_ENCRYPT_DECRYPTE, test_encrypt_decrypt_01) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_test_encrypt_decrypt(vec);
  Free_value_list(vec);
}
