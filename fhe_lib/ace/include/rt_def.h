//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_ACE_LIBRARY_RT_DEF_H
#define RTLIB_RT_ACE_LIBRARY_RT_DEF_H

#include "Ciphertext.h"

// NOLINTBEGIN (readability-identifier-naming)

//! @brief Define CIPHERTEXT/CIPHER/PLAINTEXT/PLAIN for rt APIs
typedef cuckks::DeviceCipher  CIPHERTEXT;
typedef cuckks::DeviceCipher  CIPHERTEXT3;
typedef cuckks::DeviceCipher* CIPHER;
typedef cuckks::DeviceCipher* CIPHER3;
typedef cuckks::DevicePlain   PLAINTEXT;
typedef cuckks::DevicePlain*  PLAIN;

// NOLINTEND (readability-identifier-naming)

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED  1

#endif  // RTLIB_RT_ACE_LIBRARY_RT_DEF_H