//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_DOM_UTIL_H
#define AIR_UTIL_DOM_UTIL_H

#include "air/util/dom.h"

namespace air {
namespace util {

//! @brief VISITOR to help walk DOM-tree and dispatch to CONTEXT or HANDLER
//  If no handler, all callbacks will be dispatched to CONTEXT. Otherwise
//  all callbacks will be dispatched to HANDLER.
template <typename CONTEXT, typename... HANDLERS>
class DOM_VISITOR {
  using THIS_TYPE  = DOM_VISITOR<CONTEXT, HANDLERS...>;
  using CHILD_ITER = DOM_INFO::ID_VEC;

  const CHILD_ITER& Children(uint32_t id) {
    return _dom_info->Get_dom_children(id);
  }

public:
  //! @brief Construct a new VISITOR object with defult handler objects
  DOM_VISITOR(CONTEXT& ctx, const DOM_INFO* dom) : _ctx(ctx), _dom_info(dom) {}

  //! @brief Construct a new VISITOR object with external handlers tuple
  //! @param ctx visitor context object
  //! @param handlers tuple of external handler objects
  DOM_VISITOR(CONTEXT& ctx, const std::tuple<HANDLERS...>&& handlers)
      : _ctx(ctx), _handlers(handlers) {}

  //! @brief Visit dom node and recursive visiting its child
  void Visit(uint32_t id) {
    // call Context's Push_mark(id) with following working items:
    //  - Push a mark to renaming stack
    //  - Handle PHI list and push result to stack
    //  - Handle STMT list and:
    //      * Handle rhs and update with top of stack
    //      * Handle lhs and push result to stack
    //  - Handle PHI list of successors and update PHI operands with top of
    //  stack
    if constexpr (sizeof...(HANDLERS) == 0) {
      _ctx.Push_mark(id);
    } else {
      std::get<0>(_handlers).template Push_mark<THIS_TYPE>(this, id);
    }
    for (auto&& child : Children(id)) {
      // Visit dom children recursively
      Visit(child);
    }
    // Call Context's Pop_mark(id) with following working items:
    //  - Pop mark from renaming stack
    if constexpr (sizeof...(HANDLERS) == 0) {
      _ctx.Pop_mark(id);
    } else {
      std::get<0>(_handlers).template Pop_mark<THIS_TYPE>(this, id);
    }
  }

  //! @brief get CONTEXT object
  CONTEXT& Context() { return _ctx; }

private:
  CONTEXT&                _ctx;       // visitor context
  const DOM_INFO*         _dom_info;  // DOM info
  std::tuple<HANDLERS...> _handlers;  // Handlers
};

}  // namespace util
}  // namespace air

#endif
