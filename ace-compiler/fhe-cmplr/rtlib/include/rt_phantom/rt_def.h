//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_PHANTOM_RT_DEF_H
#define RTLIB_RT_PHANTOM_RT_DEF_H

// NOLINTBEGIN (readability-identifier-naming)
#include "phantom.h"
//! @brief Forward declaration of phantom types
namespace phantom {}  // namespace phantom

//! @brief Define CIPHERTEXT/CIPHER/PLAINTEXT/PLAIN for rt APIs
typedef PhantomCiphertext  CIPHERTEXT;
typedef PhantomCiphertext  CIPHERTEXT3;
typedef PhantomCiphertext* CIPHER;
typedef PhantomCiphertext* CIPHER3;
typedef PhantomPlaintext   PLAINTEXT;
typedef PhantomPlaintext*  PLAIN;

// NOLINTEND (readability-identifier-naming)

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED  1

#endif  // RTLIB_RT_OPENFHE_RT_DEF_H
