//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_HANDLER_H
#define FHE_CORE_HANDLER_H

#include "air/base/container.h"
#include "fhe/core/opcode.h"

namespace fhe {
namespace core {

template <typename IMPL>
class HANDLER {
public:
  HANDLER(IMPL* impl) : _impl(impl) {}

  template <typename RETV, typename VISITOR>
  RETV Handle(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Domain() == ID);
    switch (node->Operator()) {
      case OPCODE::ADD:
        return Handle_add(visitor, node);
      case OPCODE::SUB:
        return Handle_sub(visitor, node);
      case OPCODE::MUL:
        return Handle_mul(visitor, node);
      default:
        AIR_ASSERT(false);
        return RETV();
    }
  }

  static constexpr uint32_t ID = DOMAIN::FHE;

private:
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR node) {
    return _impl->Handle_add(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR node) {
    return _impl->Handle_sub(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR node) {
    return _impl->Handle_mul(visitor, node);
  }

private:
  // pointer to concrete handler implementation
  IMPL* _impl;
};

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_HANDLER_H
