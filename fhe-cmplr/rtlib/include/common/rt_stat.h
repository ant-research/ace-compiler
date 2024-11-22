//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RT_STAT_H
#define RTLIB_COMMON_RT_STAT_H

//! @brief rt_stat.h
//! runtime statistics

#ifdef __cplusplus
extern "C" {
#endif

//! @brief start a time/memory statistics
void Tm_start(const char* msg);

//! @brief end current time/memory statistics
void Tm_taken(const char* msg);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RT_STAT_H
