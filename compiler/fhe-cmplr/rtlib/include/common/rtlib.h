//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RTLIB_H
#define RTLIB_COMMON_RTLIB_H

//! @brief rtlib.h
//! included by user application

#include <stdlib.h>

#include "rt_api.h"
#include "rt_stat.h"
#include "tensor.h"

//! TODO put it here for now
static void Abort_location() { exit(1); }

#endif  // RTLIB_COMMON_RTLIB_H
