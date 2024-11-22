//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RT_VERSION_H
#define RTLIB_COMMON_RT_VERSION_H

//! @brief rt_version.h
//! define runtime library version

#define RT_VERSION_MAJOR 0
#define RT_VERSION_MINOR 0
#define RT_VERSION_PATCH 0
#define RT_VERSION_BUILD 1

#define RT_VERSION_FULL                                  \
  ((RT_VERSION_MAJOR << 24) | (RT_VERSION_MINOR << 16) | \
   (RT_VERSION_PATCH << 8) | (RT_VERSION_BUILD))

#endif  // RTLIB_COMMON_RT_VERSION_H
