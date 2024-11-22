//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_HANDLER_H
#define FHE_POLY_HANDLER_H

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/core/opcode.h"
#include "fhe/poly/opcode.h"

namespace fhe {
namespace poly {

//! @brief Second level of handler to dispatch node to concrete handler
//! implementations
//! @tparam IMPL Concrete handler implementation
template <typename IMPL>
class HANDLER {
public:
  //! @brief Construct a new HANDLER object with default handler impl
  HANDLER(IMPL&& impl = IMPL()) : _impl(impl) {}

  //! @brief Construct a new HANDLER object with external handler impl
  HANDLER(IMPL* impl) : _impl(*impl) {}

  //! @brief Dispatch node to concrete handler implementation according to
  //! node's operator
  //! @tparam VISITOR Template class VISITOR
  //! @param visitor VISITOR object to visit node's kids
  //! @param node Node to dispatch
  template <typename RETV, typename VISITOR>
  RETV Handle(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Domain() == ID);
    switch (node->Operator()) {
      // case definition from opcode_def.inc
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE

      default:
        AIR_ASSERT_MSG(false, "unknown operator %s",
                       air::base::META_INFO::Op_name(node->Opcode()));
        return RETV();
    }
  }

  //! @brief Domain ID to handle
  static constexpr uint32_t ID = POLYNOMIAL_DID;

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

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_HANDLER_H
