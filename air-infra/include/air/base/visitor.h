//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_VISITOR_H
#define AIR_BASE_VISITOR_H

#include <tuple>
#include <vector>

#include "air/base/container.h"

namespace air {
namespace base {

//! @brief Top level visitor to dispatch node to individual handler for each
//  domain according to node's domain id
//! @tparam CONTEXT Context for the visitor
//! @tparam HANDLERS Handlers for all domains
template <typename CONTEXT, typename... HANDLERS>
class VISITOR {
  using THIS_TYPE = VISITOR<CONTEXT, HANDLERS...>;

public:
  //! @brief Construct a new VISITOR object with defult handler objects
  VISITOR(CONTEXT& ctx) : _ctx(ctx) {}

  //! @brief Construct a new VISITOR object with external handlers tuple
  //! @param ctx visitor context object
  //! @param handlers tuple of external handler objects
  VISITOR(CONTEXT& ctx, const std::tuple<HANDLERS...>&& handlers)
      : _ctx(ctx), _handlers(handlers) {}

  //! @brief Destruct VISITOR object
  ~VISITOR() { AIR_ASSERT(_ctx.Empty()); }

  //! @brief Visit node
  //! @param node Node to be visited
  template <typename RETV>
  RETV Visit(NODE_PTR node) {
    typename CONTEXT::GUARD guard(_ctx, node);
    if constexpr (sizeof...(HANDLERS) == 0) {
      return Visit_node<RETV>(node);
    } else {
      return Forward<RETV, 0>(node->Domain(), node);
    }
  }

  //! @brief Get nth parent node.
  //! @param nth index of parent node. 0 is current node. 1 is parent node, ...
  NODE_PTR Parent(size_t nth) const { return _ctx.Parent(nth); }

  //! @brief Get parent stmt
  STMT_PTR Parent_stmt() const { return _ctx.Parent_stmt(); }

  //! @brief manually push a node to visiting stack
  //! @param node node to be pushed onto stack
  void Push(const NODE_PTR& node) { _ctx.Push(node); }

  //! @brief manually pop a node from visiting stack
  //! @param node node to be poped from stack
  void Pop(const NODE_PTR& node) { _ctx.Pop(node); }

  //! @brief default BLOCK handler
  template <typename RETV>
  RETV Handle_block(NODE_PTR node) {
    return _ctx.template Handle_block<RETV, THIS_TYPE>(this, node);
  }

  //! @brief default NODE handler
  template <typename RETV>
  RETV Handle_node(NODE_PTR node) {
    return _ctx.template Handle_node<RETV, THIS_TYPE>(this, node);
  }

  //! @brief get CONTEXT object
  CONTEXT& Context() { return _ctx; }

private:
  // forward traverse handlers tuple and dispatch node to handler with correct
  // domain id
  template <typename RETV, uint32_t I>
  RETV Forward(uint32_t domain, NODE_PTR node) {
    if (domain == std::get<I>(_handlers).ID) {
      return std::get<I>(_handlers).template Handle<RETV, THIS_TYPE>(this,
                                                                     node);
    } else if constexpr (I + 1 < sizeof...(HANDLERS)) {
      return Forward<RETV, I + 1>(domain, node);
    } else {
      return Visit_node<RETV>(node);
    }
  }

  // visit node directly without domain handler
  template <typename RETV>
  RETV Visit_node(NODE_PTR node) {
    if (node->Is_lib_call()) {
      return _ctx.template Handle_node<RETV, THIS_TYPE>(this, node);
    } else if (node->Is_block()) {
      return _ctx.template Handle_block<RETV, THIS_TYPE>(this, node);
    } else {
      return _ctx.template Handle_unknown_domain<RETV, THIS_TYPE>(this, node);
    }
  }

  // context for visitor
  CONTEXT& _ctx;

  // tuple of all handlers
  std::tuple<HANDLERS...> _handlers;
};  // VISITOR

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_VISITOR_H
