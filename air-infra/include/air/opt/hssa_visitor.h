//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_VISITOR_H
#define AIR_OPT_HSSA_VISITOR_H

#include <set>

#include "air/opt/cfg_decl.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_decl.h"
namespace air {

namespace opt {

enum TRAV_ORDER {
  TOR_PRE,   // pre order
  TOR_POST,  // post order
  TOR_DOM,   // dom order
  TOR_PDOM   // post dom order
};

template <typename CONTEXT, typename... HANDLERS>
class HSSA_VISITOR {
  using THIS_TYPE = HSSA_VISITOR<CONTEXT, HANDLERS...>;

public:
  //! @brief Construct a new VISITOR object with defult handler objects
  HSSA_VISITOR(CONTEXT& ctx, TRAV_ORDER order = TOR_PRE)
      : _ctx(ctx), _trav_order(order) {}

  //! @brief Construct a new VISITOR object with external handlers tuple
  //! @param ctx visitor context object
  //! @param handlers tuple of external handler objects
  HSSA_VISITOR(CONTEXT& ctx, const std::tuple<HANDLERS...>&& handlers)
      : _ctx(ctx), _handlers(handlers) {}

  //! @brief Destruct VISITOR object
  ~HSSA_VISITOR() { AIR_ASSERT(_ctx.Empty()); }

  template <typename RETV>
  void Trav(BB_PTR bb) {
    Pre_visit_bb<RETV>(bb);
    if (_visited.find(bb->Id()) != _visited.end()) return;
    _visited.insert(bb->Id());
    CFG&       cfg = _ctx.Cfg();
    HSTMT_LIST stmt_list(&(_ctx.Hssa_cont()), bb->Begin_stmt_id());

    auto trav = [](HSTMT_PTR stmt, THIS_TYPE& visitor) {
      visitor.Visit<RETV>(stmt);
    };
    stmt_list.For_each(trav, *this);

    for (auto succ_id : bb->Succ()) {
      BB_PTR succ_bb = cfg.Bb_ptr(succ_id);
      Trav<RETV>(succ_bb);
    }
    Post_visit_bb<RETV>(bb);
  }

  //! @brief Visit node
  //! @param node node to be visited
  template <typename RETV, typename NODE_PTR>
  RETV Visit(NODE_PTR node) {
    typename CONTEXT::GUARD guard(_ctx, node);
    Pre_visit_node<RETV>(node);

    if constexpr (sizeof...(HANDLERS) == 0) {
      Visit_node<RETV, NODE_PTR>(node);
    } else {
      Forward<RETV, 0>(node->Opcode().Domain(), node);
    }

    Post_visit_node<RETV>(node);
    return RETV();
  }

  //! @brief Get nth parent node.
  //! @param nth index of parent node. 0 is current node. 1 is parent node, ...
  NODE_INFO Parent(size_t nth) const { return _ctx.Parent(nth); }

  //! @brief manually push an expr to visiting stack
  //! @param node node to be pushed onto stack
  void Push(const HEXPR_PTR& expr, const HSTMT_PTR& stmt) {
    _ctx.Push(expr, stmt);
  }

  //! @brief manually pop a node from visiting stack
  //! @param node node to be poped from stack
  void Pop(const HEXPR_PTR& expr, const HSTMT_PTR& stmt) {
    _ctx.Pop(expr, stmt);
  }

  //! @brief get traverse order
  TRAV_ORDER Trav_order() { return _trav_order; }

  //! @brief get CONTEXT object
  CONTEXT& Context() { return _ctx; }

private:
  // forward traverse handlers tuple and dispatch node to handler with correct
  // domain id
  template <typename RETV, uint32_t I, typename NODE_PTR>
  RETV Forward(uint32_t domain, NODE_PTR node) {
    if (domain == std::get<I>(_handlers).ID) {
      return std::get<I>(_handlers).template Handle<RETV, THIS_TYPE>(this,
                                                                     node);
    } else if constexpr (I + 1 < sizeof...(HANDLERS)) {
      return Forward<RETV, I + 1>(domain, node);
    } else {
      return Visit_node<RETV, NODE_PTR>(node);
    }
  }

  // visit node directly without domain handler
  template <typename RETV, typename NODE_PTR>
  RETV Visit_node(NODE_PTR node) {
    return _ctx.template Handle_unknown_domain<RETV, THIS_TYPE>(this, node);
  }

  template <typename RETV>
  RETV Pre_visit_node(HEXPR_PTR node) {
    return _ctx.template Pre_handle_expr<RETV, THIS_TYPE>(this, node);
  }

  template <typename RETV>
  RETV Pre_visit_node(HSTMT_PTR node) {
    return _ctx.template Pre_handle_stmt<RETV, THIS_TYPE>(this, node);
  }

  template <typename RETV>
  RETV Pre_visit_bb(BB_PTR bb) {
    return _ctx.template Pre_handle_bb<RETV, THIS_TYPE>(this, bb);
  }

  template <typename RETV>
  RETV Post_visit_bb(BB_PTR bb) {
    return _ctx.template Post_handle_bb<RETV, THIS_TYPE>(this, bb);
  }

  template <typename RETV>
  RETV Post_visit_node(HEXPR_PTR node) {
    return _ctx.template Post_handle_expr<RETV, THIS_TYPE>(this, node);
  }

  template <typename RETV>
  RETV Post_visit_node(HSTMT_PTR node) {
    return _ctx.template Post_handle_stmt<RETV, THIS_TYPE>(this, node);
  }

  // traverse order
  TRAV_ORDER _trav_order;

  // context for visitor
  CONTEXT& _ctx;

  // tuple of all handlers
  std::tuple<HANDLERS...> _handlers;

  BBID_SET _visited;
};  // VISITOR

}  // namespace opt
}  // namespace air

#endif
