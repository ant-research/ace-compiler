//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_HANDLER_H
#define FHE_CKKS_HANDLER_H

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {

namespace ckks {

template <typename IMPL>
class HANDLER {
public:
  using NODE_PTR = air::base::NODE_PTR;

  explicit HANDLER(IMPL&& impl = IMPL()) : _impl(impl) {}

  explicit HANDLER(IMPL* impl) : _impl(*impl) {}

  static constexpr uint32_t ID = CKKS_DOMAIN::ID;

  template <typename RETV, typename VISITOR>
  RETV Handle(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Domain() == ID);
    switch (CKKS_OPERATOR(node->Operator())) {
#define CKKS_OPCODE(NAME, name, category, kid_num, fld_num, property) \
  case CKKS_OPERATOR::NAME:                                           \
    return Handle_##name<RETV, VISITOR>(visitor, node);               \
    break;
#include "fhe/ckks/opcode_def.inc"
      default:
        AIR_ASSERT(false);
        break;
    }
    return RETV();
  }

private:
#define CKKS_OPCODE(NAME, name, category, kid_num, fld_num, property)  \
  template <typename RETV, typename VISITOR>                           \
  RETV Handle_##name(VISITOR* visitor, NODE_PTR node) {                \
    return _impl.template Handle_##name<RETV, VISITOR>(visitor, node); \
  }
#include "fhe/ckks/opcode_def.inc"

  IMPL& _impl;
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_HANDLER_H
