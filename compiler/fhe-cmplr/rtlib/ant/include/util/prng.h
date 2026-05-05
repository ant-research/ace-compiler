//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef RTLIB_ANT_INCLUDE_UTIL_PRNG_H
#define RTLIB_ANT_INCLUDE_UTIL_PRNG_H

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Free global BLAKE2_PRNG
void Free_prng();

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_INCLUDE_UTIL_PRNG_H