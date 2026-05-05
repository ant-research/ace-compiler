//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CKKS_ENCRYPTOR_H
#define RTLIB_INCLUDE_CKKS_ENCRYPTOR_H

#include "ckks/ciphertext.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "ckks/public_key.h"
#include "ckks/secret_key.h"
#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief An object that can encrypt data using CKKS given a public key.
typedef struct {
  CKKS_PARAMETER* _params;
  PUBLIC_KEY*     _public_key;
  SECRET_KEY*     _secret_key;
} CKKS_ENCRYPTOR;

//! @brief Generates private/public key pair for CKKS scheme.
//! @param params parameters including polynomial degree,
//! ciphertext modulus, etc.
//! @param public_key public key used for encryption.
//! @param secret_key optionally passed for secret key encryption.
static inline CKKS_ENCRYPTOR* Alloc_ckks_encryptor(CKKS_PARAMETER* params,
                                                   PUBLIC_KEY*     public_key,
                                                   SECRET_KEY*     secret_key) {
  CKKS_ENCRYPTOR* encryptor = (CKKS_ENCRYPTOR*)malloc(sizeof(CKKS_ENCRYPTOR));
  encryptor->_params        = params;
  encryptor->_public_key    = public_key;
  encryptor->_secret_key    = secret_key;
  return encryptor;
}

//! @brief Cleanup ckks encryptor
//! @param encryptor encryptor to be cleanup
static inline void Free_ckks_encryptor(CKKS_ENCRYPTOR* encryptor) {
  if (encryptor == NULL) return;
  free(encryptor);
  encryptor = NULL;
}

//! @brief Encrypts the message and returns the corresponding ciphertext
//! @param res a ciphertext consisting of a pair of polynomials in the
//! ciphertext space
//! @param encryptor encryptor including public key & secret key
//! @param plain input plaintext
void Encrypt_msg(CIPHERTEXT* res, CKKS_ENCRYPTOR* encryptor, PLAINTEXT* plain);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CKKS_ENCRYPTOR_H
