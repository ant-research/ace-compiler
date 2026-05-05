//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_HYPERFHE_RT_DEF_H
#define RTLIB_RT_HYPERFHE_RT_DEF_H

// NOLINTBEGIN (readability-identifier-naming)

//! @brief Forward declaration of HYPERFHE types
namespace hyperfhe{
class Ciphertext;
class Plaintext;
}

//! @brief Define CIPHERTEXT/CIPHER/PLAINTEXT/PLAIN for rt APIs
#ifdef GPU_BACKEND
typedef cuckks::DeviceCipher  CIPHERTEXT;
typedef cuckks::DeviceCipher  CIPHERTEXT3;
typedef cuckks::DeviceCipher* CIPHER;
typedef cuckks::DeviceCipher* CIPHER3;
typedef cuckks::DevicePlain   PLAINTEXT;
typedef cuckks::DevicePlain*  PLAIN;
#else
typedef seal::Ciphertext  CIPHERTEXT;
typedef seal::Ciphertext  CIPHERTEXT3;
typedef seal::Ciphertext* CIPHER;
typedef seal::Ciphertext* CIPHER3;
typedef seal::Plaintext   PLAINTEXT;
typedef seal::Plaintext*  PLAIN;
#endif

// NOLINTEND (readability-identifier-naming)

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED  1

#endif  // RTLIB_RT_SEAL_RT_DEF_H

