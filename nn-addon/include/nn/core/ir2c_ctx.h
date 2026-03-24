//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_IR2C_CTX_H
#define NN_CORE_IR2C_CTX_H

#include "air/core/ir2c_ctx.h"

namespace nn {

namespace core {

class IR2C_CTX : public air::core::IR2C_CTX {
public:
  IR2C_CTX(std::ostream& os) : air::core::IR2C_CTX(os) {}
};  // IR2C_CTX

}  // namespace core

}  // namespace nn

#endif  // NN_CORE_IR2C_CTX_H
