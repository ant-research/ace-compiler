//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdbool.h>

//!< @brief The Bare system does not support file sytem
// So let's do that for now

bool Pt_mgr_init(const char* fname) { return false; }

void Pt_mgr_fini() {}

void Init_rtlib_timing() {}