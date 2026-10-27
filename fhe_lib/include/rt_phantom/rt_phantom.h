//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file rt_phantom/rt_phantom.h
//! @brief Forwarding shim — compiler-generated kernel.cu emits
//!        `#include "rt_phantom/rt_phantom.h"`.  This header redirects
//!        to the canonical location under fhe/vendor/phantom/.
//!
//! NOTE: No include guard here — the real header at
//! fhe/vendor/phantom/rt_phantom.h provides its own guard
//! (RTLIB_RT_PHANTOM_RT_PHANTOM_H).  Adding a guard here would shadow
//! the real header's guard and skip its entire contents.

#include "fhe/vendor/phantom/rt_phantom.h"
