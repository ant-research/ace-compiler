//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_PRAGMA_H
#define AIR_BASE_PRAGMA_H

//! Define a macro to generate domain specific pragma id
#define MAKE_PRAGMA_ID(domain, id) (((uint32_t)domain << 16) + (uint32_t)id)

#endif  // AIR_BASE_PRAGMA_H
