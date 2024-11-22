//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_INVALID_HANDLER_H
#define NN_CORE_INVALID_HANDLER_H

#include "air/base/container.h"
#include "nn/core/opcode.h"

namespace nn {
namespace core {

class INVALID_HANDLER {
public:
  // invalid handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_INVALID_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE
};

}  // namespace core
}  // namespace nn

#endif  // NN_CORE_INVALID_HANDLER_H
