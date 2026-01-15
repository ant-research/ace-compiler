//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CKKS_DECRYPTOR_H
#define RTLIB_INCLUDE_CKKS_DECRYPTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckks/ciphertext.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "ckks/secret_key.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief An object that can Decrypt data using CKKS given a secret key.
typedef struct {
  CKKS_PARAMETER* _params;
  SECRET_KEY*     _secret_key;
} CKKS_DECRYPTOR;

//! @brief Initializes decryptor for CKKS scheme.
//! @param param parameters including polynomial degree,
//! plaintext modulus, and ciphertext modulus.
//! @param secret_key secret key used for Decryption
static inline CKKS_DECRYPTOR* Alloc_ckks_decryptor(CKKS_PARAMETER* param,
                                                   SECRET_KEY*     secret_key) {
  CKKS_DECRYPTOR* decryptor = (CKKS_DECRYPTOR*)malloc(sizeof(CKKS_DECRYPTOR));
  decryptor->_params        = param;
  decryptor->_secret_key    = secret_key;
  return decryptor;
}

//! @brief Cleanup ckks decryptor
static inline void Free_ckks_decryptor(CKKS_DECRYPTOR* decryptor) {
  if (decryptor == NULL) return;
  free(decryptor);
  decryptor = NULL;
}

//! @brief Decrypts the ciphertext and returns the corresponding plaintext
//! @param res the plaintext corresponding to the Decrypted ciphertext
//! @param decryptor decryptor with a secret key
//! @param ciphertext CIPHERTEXT to be Decrypted.
//! @param c2 optional additional parameter for a ciphertext that
//! has not been relinearized.
void Decrypt(PLAINTEXT* res, CKKS_DECRYPTOR* decryptor, CIPHERTEXT* ciphertext,
             POLYNOMIAL* c2);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CKKS_DECRYPTOR_H