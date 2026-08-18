//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_IR2C_HANDLER_H
#define NN_CORE_IR2C_HANDLER_H

#include "nn/core/ir2c_ctx.h"
#include "nn/core/null_handler.h"
#include "nn/core/opcode.h"

namespace nn {
namespace core {

class IR2C_HANDLER : public NULL_HANDLER {
public:
  IR2C_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    nn::core::IR2C_CTX& ctx = visitor->Context();
    ctx << "nn_add(";
    visitor->template Visit<void>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<void>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, air::base::NODE_PTR node) {
    nn::core::IR2C_CTX& ctx = visitor->Context();
    ctx << "nn_sub(";
    visitor->template Visit<void>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<void>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_gemm(VISITOR* visitor, air::base::NODE_PTR node) {
    nn::core::IR2C_CTX& ctx = visitor->Context();
    ctx << "nn_gemm(";
    visitor->template Visit<void>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<void>(node->Child(1));
    ctx << ")";
  }

};  // IR2C_HANDLER

}  // namespace core
}  // namespace nn

#endif  // NN_CORE_IR2C_HANDLER_H
