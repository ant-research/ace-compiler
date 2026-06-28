//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_ACE_RT_DEF_H
#define RTLIB_RT_ACE_RT_DEF_H

// NOLINTBEGIN (readability-identifier-naming)

//! @brief Forward declaration of Ace types
namespace ace_library{
class Ciphertext;
class Plaintext;
}

//! @brief Define CIPHERTEXT/CIPHER/PLAINTEXT/PLAIN for rt APIs
typedef acelib::DeviceCipher  CIPHERTEXT;
typedef acelib::DeviceCipher  CIPHERTEXT3;
typedef acelib::DeviceCipher* CIPHER;
typedef acelib::DeviceCipher* CIPHER3;
typedef acelib::DevicePlain   PLAINTEXT;
typedef acelib::DevicePlain*  PLAIN;

// NOLINTEND (readability-identifier-naming)

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED  1

#endif  // RTLIB_RT_ACE_RT_DEF_H
