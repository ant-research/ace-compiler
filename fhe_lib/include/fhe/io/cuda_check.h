//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief cuda_check.h
//! Shared CUDA error-checking utility for FHE GPU backends.
//!
//! This header provides the FHE_CHECK_CUDA macro used across GPU I/O
//! and key serialization code. It is part of the common I/O layer
//! (fhe_lib/common/gds/) and can be included by any GPU backend
//! (phantom, ace, etc.).

#ifndef FHE_CUDA_CHECK_H
#define FHE_CUDA_CHECK_H

#include <cstdio>
#include <cuda_runtime.h>
#include <stdexcept>
#include <string>

//=============================================================================
// CUDA error check macro
//=============================================================================

//! Check CUDA return value and throw std::runtime_error on failure.
//! Used throughout FHE GPU backends for consistent error handling.
#define FHE_CHECK_CUDA(expr)                                                                                           \
    do {                                                                                                               \
        cudaError_t err = (expr);                                                                                      \
        if (err != cudaSuccess) {                                                                                      \
            throw std::runtime_error(std::string("CUDA error at ") + __FILE__ + ":" + std::to_string(__LINE__) +       \
                                     ": " + cudaGetErrorString(err));                                                  \
        }                                                                                                              \
    } while (0)

// Backward compatibility alias for phantom code
// Guarded to avoid redefinition when Phantom's cuda_wrapper.cuh is included first
#ifndef PHANTOM_CHECK_CUDA
#define PHANTOM_CHECK_CUDA FHE_CHECK_CUDA
#endif

#endif  // FHE_CUDA_CHECK_H
