//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RT_ENV_H
#define RTLIB_COMMON_RT_ENV_H

//! @brief rt_env.h
//! Define environment variables used by rtlib

//! environment variable to control rtlib timing output
//! RTLIB_TIMING_OUTPUT=string: stdout, stderr or file name. default: NULL
#define ENV_RTLIB_TIMING_OUTPUT "RTLIB_TIMING_OUTPUT"

//! environment variable to control trace file name
//! RTLIB_TRACE_FILE=string: stdout, stderr or file name. default: fhe_trace.t
#define ENV_RTLIB_TRACE_FILE "RTLIB_TRACE_FILE"

//! environment variable to control plaintext manager (PT_MGR)
//! PT_ENTRY_COUNT=int: number of pt kept in memory. default: 8
#define ENV_PT_ENTRY_COUNT "PT_ENTRY_COUNT"
//! PT_PREFETCH_COUNT: number of pt for prefetching. default: 2
#define ENV_PT_PREFETCH_COUNT "PT_PREFETCH_COUNT"

//! environment variable to control rt data file reader (RT_DATA_FILE)
//! RT_DATA_ASYNC_READ=0|1: use asynchronous read. default: 0
#define ENV_RT_DATA_ASYNC_READ "RT_DATA_ASYNC_READ"

//! environment variable to control using even polynomial
//! in mod_reduce of bootstrapping
#define ENV_BOOTSTRAP_EVEN_POLY "RTLIB_BTS_EVEN_POLY"

//! environment variable to control fusion for decompose and modup op
#define ENV_OP_FUSION_DECOMP_MODUP "OP_FUSION_DECOMP_MODUP"

//! environment variable to control clear imaginary part at the end of bootstrap
#define ENV_BOOTSTRAP_CLEAR_IMAG "RT_BTS_CLEAR_IMAG"
#endif  // RTLIB_COMMON_RT_ENV_H
