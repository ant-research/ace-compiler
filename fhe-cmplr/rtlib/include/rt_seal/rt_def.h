//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_SEAL_RT_DEF_H
#define RTLIB_RT_SEAL_RT_DEF_H

// NOLINTBEGIN (readability-identifier-naming)

//! @brief forward declaration of SEAL types
namespace seal {
class Ciphertext;
class Plaintext;
}  // namespace seal

//! @brief Define CIPHERTEXT/CIPHER/PLAINTEXT/PLAIN for rt APIs
typedef seal::Ciphertext  CIPHERTEXT;
typedef seal::Ciphertext* CIPHER;
typedef seal::Plaintext   PLAINTEXT;
typedef seal::Plaintext*  PLAIN;

// NOLINTEND (readability-identifier-naming)

#endif  // RTLIB_RT_SEAL_RT_DEF_H
