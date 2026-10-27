//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file rt_ace/rt_ace.h
//! @brief Forwarding shim — compiler-generated kernel.cu emits
//!        `#include "rt_ace/rt_ace.h"`.  This header redirects to the
//!        canonical location under fhe/vendor/ace/.
//!
//! NOTE: No include guard here — the real header at fhe/vendor/ace/rt_ace.h
//! provides its own guard (RTLIB_RT_ACE_RT_ACE_H).  Adding a guard here
//! would shadow the real header's guard and skip its entire contents.

#include "fhe/vendor/ace/rt_ace.h"
