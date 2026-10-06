//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_PRAGMA_H
#define NN_CORE_PRAGMA_H

#include "air/base/pragma.h"
#include "nn/core/opcode.h"

namespace nn {

namespace core {

enum class PRAGMA : uint32_t {
  OP_START,  //<! Indicate OP start. arg0 is OPCODE, arg1 is OP name
  OP_END,    //<! Indicate OP end. arg0 is OPCODE, arg1 is OP name
};

static constexpr uint32_t PRAGMA_OP_START =
    MAKE_PRAGMA_ID(NN, PRAGMA::OP_START);
static constexpr uint32_t PRAGMA_OP_END = MAKE_PRAGMA_ID(NN, PRAGMA::OP_END);

}  // namespace core

}  // namespace nn

#endif  // NN_CORE_PRAGMA_H
