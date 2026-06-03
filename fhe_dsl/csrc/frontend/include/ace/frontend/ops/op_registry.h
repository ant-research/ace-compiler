//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OP_REGISTRY_H
#define AIR_OP_REGISTRY_H

#include "ace/frontend/ops/op_schema.h"

namespace ace {
namespace frontend {

//=============================================================================
// Register All Operators (Call All Level Registration Functions)
//=============================================================================
void Register_all_ops();

}  // namespace frontend
}  // namespace ace

#endif  // AIR_OP_REGISTRY_H