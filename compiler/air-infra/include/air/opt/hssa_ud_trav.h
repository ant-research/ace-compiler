//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_UD_TRAV_H
#define AIR_OPT_HSSA_UD_TRAV_H
namespace air {
namespace opt {

template <typename CONTEXT, typename... HANDLERS>
class HSSA_UD_TRAV {
  using THIS_TYPE = HSSA_UD_TRAV<CONTEXT, HANDLERS...>;

public:
  //! @brief Construct a new VISITOR object with defult handler objects
  HSSA_UD_TRAV(CONTEXT& ctx) : _ctx(ctx) {}

  //! @brief Construct a new VISITOR object with external handlers tuple
  //! @param ctx visitor context object
  //! @param handlers tuple of external handler objects
  HSSA_UD_TRAV(CONTEXT& ctx, const std::tuple<HANDLERS...>&& handlers)
      : _ctx(ctx), _handlers(handlers) {}

  //! @brief Destruct VISITOR object
  ~HSSA_UD_TRAV() { AIR_ASSERT(_ctx.Empty()); }

  void Set_root_node(HEXPR_PTR node) { _root_node = node; }

  HEXPR_PTR Root_node(void) const { return _root_node; }

  template <typename RETV>
  RETV Start(HEXPR_PTR node) {
    Set_root_node(node);
    return Visit<RETV>(node);
  }

  //! @brief Visit node
  //! @param node node to be visited
  template <typename RETV>
  RETV Visit(HEXPR_PTR node) {
    RETV                    ret = RETV();
    typename CONTEXT::GUARD guard(_ctx, node);
    Pre_visit_node<RETV>(node);

    if constexpr (sizeof...(HANDLERS) == 0) {
      ret = Visit_node<RETV>(node);
    } else {
      ret = Forward<RETV, 0>(node->Opcode().Domain(), node);
    }

    Post_visit_node<RETV>(node);
    return ret;
  }

  //! @brief Visit node
  //! @param node node to be visited
  template <typename RETV>
  RETV Visit(HSTMT_PTR node) {
    RETV ret = RETV();
    Pre_visit_node<RETV>(node);

    if constexpr (sizeof...(HANDLERS) == 0) {
      ret = Visit_node<RETV>(node);
    } else {
      ret = Forward<RETV, 0>(node->Opcode().Domain(), node);
    }

    Post_visit_node<RETV>(node);
    return ret;
  }

  //! @brief Get nth parent node.
  //! @param nth index of parent node. 0 is current node. 1 is parent node, ...
  HEXPR_PTR Parent(size_t nth) const { return _ctx.Parent(nth); }

  //! @brief Get parent stmt
  HSTMT_PTR Parent_stmt() const { return _ctx.Parent_stmt(); }

  //! @brief manually push an expr to visiting stack
  //! @param node node to be pushed onto stack
  void Push(const HEXPR_PTR& expr) { _ctx.Push(expr); }

  //! @brief manually pop a node from visiting stack
  //! @param node node to be poped from stack
  void Pop(const HEXPR_PTR& expr) { _ctx.Pop(expr); }

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
  RETV Post_visit_node(HEXPR_PTR node) {
    return _ctx.template Post_handle_expr<RETV, THIS_TYPE>(this, node);
  }
  template <typename RETV>
  RETV Post_visit_node(HSTMT_PTR node) {
    return _ctx.template Post_handle_stmt<RETV, THIS_TYPE>(this, node);
  }

  // context for visitor
  CONTEXT& _ctx;

  // tuple of all handlers
  std::tuple<HANDLERS...> _handlers;
  HEXPR_PTR               _root_node;
};  // VISITOR

}  // namespace opt
}  // namespace air

#endif
