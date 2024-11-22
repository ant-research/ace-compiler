//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_HANDLER_H
#define NN_CORE_HANDLER_H

#include "air/base/container.h"
#include "nn/core/opcode.h"

namespace nn {
namespace core {

template <typename IMPL>
class HANDLER {
public:
  HANDLER(IMPL&& impl = IMPL()) : _impl(impl) {}

  HANDLER(IMPL* impl) : _impl(*impl) {}

  template <typename RETV, typename VISITOR>
  RETV Handle(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Domain() == ID);
    switch (node->Operator()) {
      // case definition from opcode_def.inc
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE

      // default handler
      default:
        AIR_ASSERT(false);
        return RETV();
    }
  }

  static constexpr uint32_t ID = NN;

private:
  // private dispatcher to deliver to IMPL to handle the node
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_IMPL_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE

private:
  // pointer to concrete handler implementation
  IMPL& _impl;
};

}  // namespace core
}  // namespace nn

#endif  // NN_CORE_HANDLER_H
