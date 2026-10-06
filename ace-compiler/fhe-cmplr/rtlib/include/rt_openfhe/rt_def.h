//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_OPENFHE_RT_DEF_H
#define RTLIB_RT_OPENFHE_RT_DEF_H

// NOLINTBEGIN (readability-identifier-naming)

//! @brief Forward declaration of OpenFHE types
namespace lbcrypto {}  // namespace lbcrypto

//! @brief Define CIPHERTEXT/CIPHER/PLAINTEXT/PLAIN for rt APIs
typedef lbcrypto::Ciphertext<lbcrypto::DCRTPoly>  CIPHERTEXT;
typedef lbcrypto::Ciphertext<lbcrypto::DCRTPoly>  CIPHERTEXT3;
typedef lbcrypto::Ciphertext<lbcrypto::DCRTPoly>* CIPHER;
typedef lbcrypto::Ciphertext<lbcrypto::DCRTPoly>* CIPHER3;
typedef lbcrypto::Plaintext                       PLAINTEXT;
typedef lbcrypto::Plaintext*                      PLAIN;

// NOLINTEND (readability-identifier-naming)

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED  1

#endif  // RTLIB_RT_OPENFHE_RT_DEF_H
