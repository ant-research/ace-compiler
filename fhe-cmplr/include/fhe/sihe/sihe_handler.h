//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_HANDLER_H
#define FHE_SIHE_HANDLER_H

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/util/debug.h"
#include "fhe/sihe/sihe_opcode.h"

namespace fhe {

namespace sihe {
using namespace air::base;

template <typename IMPL>
class HANDLER {
public:
  explicit HANDLER(IMPL&& impl = IMPL()) : _impl(impl) {}

  explicit HANDLER(IMPL* impl) : _impl(*impl) {}

  static constexpr uint32_t ID = SIHE_DOMAIN::ID;

  template <typename RETV, typename VISITOR>
  RETV Handle(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Domain() == ID);
    switch (SIHE_OPERATOR(node->Operator())) {
#define SIHE_OPCODE(NAME, name, category, kid_num, fld_num, property) \
  case SIHE_OPERATOR::NAME:                                           \
    return Handle_##name<RETV, VISITOR>(visitor, node);
#include "fhe/sihe/opcode_def.inc"
      default:
        CMPLR_ASSERT(false, "not supported operator");
        return RETV();
    }
  }

private:
#define SIHE_OPCODE(NAME, name, category, kid_num, fld_num, property)  \
  template <typename RETV, typename VISITOR>                           \
  RETV Handle_##name(VISITOR* visitor, NODE_PTR node) {                \
    return _impl.template Handle_##name<RETV, VISITOR>(visitor, node); \
  }
#include "fhe/sihe/opcode_def.inc"

  IMPL& _impl;
};
}  // namespace sihe
}  // namespace fhe

#endif  // FHE_CORE_HANDLER_H
