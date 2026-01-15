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
#include "ckks/evaluator.h"
#include "ckks/key_gen.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "ckks/secret_key.h"
#include "common/rt_api.h"
#include "context/ckks_context.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "rns_poly_impl.h"
#include "util/matrix_operation.h"
#include "util/random_sample.h"

class TEST_ROTATION : public ::testing::Test {
protected:
  void SetUp() override {
    _degree             = 32;
    int32_t rot_idxs[5] = {0, 1, 2, 3, 4};
    Set_context_params(_degree, 3, 33, 30, 0 /*qpart*/, 0 /*input_level*/,
                       0 /*hw*/, 5, rot_idxs);
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

  size_t Get_degree() { return _degree; }

  void Run_test_simple_rotate(VALUE_LIST* vec, uint32_t rot) {
    size_t      vec_length = LIST_LEN(vec);
    VALUE_LIST* rot_msg    = Alloc_value_list(DCMPLX_TYPE, vec_length);
    for (size_t idx = 0; idx < vec_length; idx++) {
      DCMPLX_VALUE_AT(rot_msg, idx) =
          Get_dcmplx_value_at(vec, (idx + rot) % (_degree / 2));
    }

    PLAINTEXT*  plain    = Alloc_plaintext();
    CIPHERTEXT* ciph     = Alloc_ciphertext();
    CIPHERTEXT* ciph_rot = Alloc_ciphertext();
    SECRET_KEY* rot_key =
        Alloc_secret_key(_degree, _param->_num_primes, _param->_num_p_primes);
    CKKS_DECRYPTOR* rot_decryptor = Alloc_ckks_decryptor(_param, rot_key);
    PLAINTEXT*      decrypted_rot = Alloc_plaintext();
    VALUE_LIST*     decoded_rot   = Alloc_value_list(DCMPLX_TYPE, vec_length);

    ENCODE(plain, _encoder, vec);
    Encrypt_msg(ciph, _encryptor, plain);
    Init_ciphertext_from_ciph(ciph_rot, ciph, Get_ciph_sfactor(ciph),
                              Get_ciph_sf_degree(ciph));
    Rotate_poly_with_rotation_idx(Get_c0(ciph_rot), Get_c0(ciph), rot);
    Rotate_poly_with_rotation_idx(Get_c1(ciph_rot), Get_c1(ciph), rot);
    Rotate_poly_with_rotation_idx(Get_sk_poly(rot_key),
                                  Get_sk_poly(Get_sk(_keygen)), rot);
    Conv_poly2ntt(Get_ntt_sk(rot_key), Get_sk_poly(rot_key));
    Decrypt(decrypted_rot, rot_decryptor, ciph_rot, NULL);
    Decode(decoded_rot, _encoder, decrypted_rot);

    Check_complex_vector_approx_eq(rot_msg, decoded_rot, 0.001);

    Free_value_list(decoded_rot);
    Free_plaintext(decrypted_rot);
    Free_ckks_decryptor(rot_decryptor);
    Free_secretkey(rot_key);
    Free_ciphertext(ciph_rot);
    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_value_list(rot_msg);
  }

  void Run_test_rotate(VALUE_LIST* vec, int64_t rot) {
    size_t      vec_length = LIST_LEN(vec);
    VALUE_LIST* rot_msg    = Alloc_value_list(DCMPLX_TYPE, vec_length);
    for (size_t idx = 0; idx < vec_length; idx++) {
      DCMPLX_VALUE_AT(rot_msg, idx) =
          Get_dcmplx_value_at(vec, (idx + rot) % (_degree / 2));
    }

    PLAINTEXT*  plain         = Alloc_plaintext();
    CIPHERTEXT* ciph          = Alloc_ciphertext();
    CIPHERTEXT* ciph_rot      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_rot = Alloc_plaintext();
    VALUE_LIST* decoded_rot   = Alloc_value_list(DCMPLX_TYPE, vec_length);

    ENCODE(plain, _encoder, vec);
    Encrypt_msg(ciph, _encryptor, plain);
    Eval_fast_rotate(ciph_rot, ciph, rot, _evaluator);
    Decrypt(decrypted_rot, _decryptor, ciph_rot, NULL);
    Decode(decoded_rot, _encoder, decrypted_rot);
    Check_complex_vector_approx_eq(rot_msg, decoded_rot, 0.005);

    Free_value_list(decoded_rot);
    Free_plaintext(decrypted_rot);
    Free_ciphertext(ciph_rot);
    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_value_list(rot_msg);
  }

  void Run_test_rotate_after_reset_slots(VALUE_LIST* vec, int64_t rot,
                                         size_t dup_cnt) {
    size_t vec_length = LIST_LEN(vec);
    IS_TRUE(dup_cnt * vec_length == _degree / 2, "invalid length of vector");
    VALUE_LIST* rot_msg = Alloc_value_list(DCMPLX_TYPE, vec_length);
    for (size_t idx = 0; idx < vec_length; idx++) {
      DCMPLX_VALUE_AT(rot_msg, idx) =
          Get_dcmplx_value_at(vec, (idx + rot) % vec_length);
    }

    PLAINTEXT*  plain         = Alloc_plaintext();
    CIPHERTEXT* ciph          = Alloc_ciphertext();
    CIPHERTEXT* ciph_rot      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_rot = Alloc_plaintext();

    // encode with default slots = N / 2
    ENCODE(plain, _encoder, vec);
    Encrypt_msg(ciph, _encryptor, plain);

    // reset slots to vec_length
    Set_ciph_slots(ciph, vec_length);

    // ciph = ciph * dup_cnt
    Mul_const(ciph, ciph, dup_cnt, _evaluator);

    // fast rotate with rot
    Eval_fast_rotate(ciph_rot, ciph, rot, _evaluator);

    Decrypt(decrypted_rot, _decryptor, ciph_rot, NULL);
    VALUE_LIST* decoded_rot =
        Alloc_value_list(DCMPLX_TYPE, Get_ciph_slots(ciph_rot));
    Decode(decoded_rot, _encoder, decrypted_rot);

    Check_complex_vector_approx_eq(rot_msg, decoded_rot, 0.005);

    Free_value_list(decoded_rot);
    Free_plaintext(decrypted_rot);
    Free_ciphertext(ciph_rot);
    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_value_list(rot_msg);
  }

  void Run_test_conjugate(VALUE_LIST* vec) {
    size_t      vec_length   = LIST_LEN(vec);
    VALUE_LIST* conj_message = Alloc_value_list(DCMPLX_TYPE, vec_length);
    for (size_t idx = 0; idx < vec_length; idx++) {
      DCMPLX_VALUE_AT(conj_message, idx) = conj(Get_dcmplx_value_at(vec, idx));
    }

    PLAINTEXT*  plain          = Alloc_plaintext();
    CIPHERTEXT* ciph           = Alloc_ciphertext();
    SWITCH_KEY* conj_key       = Alloc_switch_key();
    CIPHERTEXT* ciph_conj      = Alloc_ciphertext();
    PLAINTEXT*  decrypted_conj = Alloc_plaintext();
    VALUE_LIST* decoded_conj   = Alloc_value_list(DCMPLX_TYPE, _degree >> 1);

    ENCODE(plain, _encoder, vec);
    Encrypt_msg(ciph, _encryptor, plain);
    Generate_conj_key(conj_key, _keygen);
    Insert_auto_key(_keygen, 2 * _degree - 1, conj_key);

    Conjugate(ciph_conj, ciph, _evaluator);
    Decrypt(decrypted_conj, _decryptor, ciph_conj, NULL);
    Decode(decoded_conj, _encoder, decrypted_conj);

    Check_complex_vector_approx_eq(conj_message, decoded_conj, 0.005);

    Free_value_list(decoded_conj);
    Free_plaintext(decrypted_conj);
    Free_ciphertext(ciph_conj);
    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_value_list(conj_message);
  }

private:
  size_t              _degree;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_EVALUATOR*     _evaluator;
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
};

TEST_F(TEST_ROTATION, simple_rotate) {
  size_t      length = Get_degree() / 2;
  VALUE_LIST* dc_vec = Alloc_value_list(DCMPLX_TYPE, length);
  Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), length);
  int32_t rot = 1;
  Run_test_simple_rotate(dc_vec, rot);
  Free_value_list(dc_vec);
}

TEST_F(TEST_ROTATION, conjugate) {
  size_t      length = Get_degree() / 2;
  VALUE_LIST* dc_vec = Alloc_value_list(DCMPLX_TYPE, length);
  Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), length);
  Run_test_conjugate(dc_vec);
  Free_value_list(dc_vec);
}

TEST_F(TEST_ROTATION, rotate_01) {
  size_t      length = Get_degree() / 2;
  VALUE_LIST* dc_vec = Alloc_value_list(DCMPLX_TYPE, length);
  Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), length);
  Run_test_rotate(dc_vec, 0);
  Run_test_rotate(dc_vec, 1);
  Run_test_rotate(dc_vec, 2);
  Run_test_rotate(dc_vec, 3);
  Run_test_rotate(dc_vec, 4);
  Free_value_list(dc_vec);
}

TEST_F(TEST_ROTATION, rotate_02) {
  size_t      default_slot_size = Get_degree() / 2;
  size_t      dup_cnt           = 2;
  size_t      len1              = default_slot_size / dup_cnt;
  VALUE_LIST* dc_vec            = Alloc_value_list(DCMPLX_TYPE, len1);
  Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), len1);
  Run_test_rotate_after_reset_slots(dc_vec, 2 /*rot_idx*/, dup_cnt);
  Free_value_list(dc_vec);
}

TEST_F(TEST_ROTATION, rotate_03) {
  size_t      default_slot_size = Get_degree() / 2;
  size_t      dup_cnt           = 4;
  size_t      len1              = default_slot_size / dup_cnt;
  VALUE_LIST* dc_vec            = Alloc_value_list(DCMPLX_TYPE, len1);
  Sample_random_complex_vector(DCMPLX_VALUES(dc_vec), len1);
  Run_test_rotate_after_reset_slots(dc_vec, 2 /*rot_idx*/, dup_cnt);
  Free_value_list(dc_vec);
}
