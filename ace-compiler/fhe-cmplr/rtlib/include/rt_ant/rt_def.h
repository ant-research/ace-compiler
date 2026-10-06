//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_ANT_RT_DEF_H
#define RTLIB_RT_ANT_RT_DEF_H

//! @brief Declaration of ANT types

#include "ckks/ciphertext.h"
#include "ckks/plaintext.h"

typedef CIPHERTEXT* CIPHER;
typedef PLAINTEXT*  PLAIN;

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED  1

#endif  // RTLIB_RT_ANT_RT_DEF_H
