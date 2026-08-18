//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_ERROR_H
#define RTLIB_COMMON_ERROR_H

//! @brief error.h
//! for error handling

#include <assert.h>

#if !defined(NDEBUG)
#define IS_TRUE(cond, msg) assert((cond) && (msg))
#else
#define IS_TRUE(cond, msg) ((void)1)
#endif

#define FMT_ASSERT(cond, ...)              \
  if (!(cond)) {                           \
    printf("%s:%d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__);                   \
    printf("\n");                          \
    abort();                               \
  }

#define DEV_WARN(fmt, ...) printf(fmt, ##__VA_ARGS__)

#endif
