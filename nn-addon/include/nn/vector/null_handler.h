//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_NULL_HANDLER_H
#define NN_VECTOR_NULL_HANDLER_H

#include "air/base/container.h"
#include "nn/vector/vector_opcode.h"

namespace nn {
namespace vector {

class NULL_HANDLER {
public:
  // null handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_NULL_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "nn/vector/opcode_def.inc"
#undef DEF_OPCODE
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_NULL_HANDLER_H
