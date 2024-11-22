//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ANALYZE_CTX_H
#define AIR_BASE_ANALYZE_CTX_H

#include <vector>

#include "air/base/container.h"

namespace air {
namespace base {

//! @brief context for analyze pass used by VISITOR
class ANALYZE_CTX {
public:
  //! @brief construct a new analyze context
  ANALYZE_CTX() {}

  //! @brief destruct TRANSFORM object
  ~ANALYZE_CTX() { AIR_ASSERT(_stack.empty()); }

  //! @brief get nth parent node.
  //! @param nth index of parent node. 0 is current node. 1 is parent node, ...
  NODE_PTR Parent(size_t nth) const {
    AIR_ASSERT(!_stack.empty());
    size_t sz = _stack.size();
    return sz > nth ? _stack[sz - 1 - nth] : Null_ptr;
  }

  //! @brief get parent block node
  NODE_PTR Parent_block() const {
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
      if ((*it)->Is_block()) {
        return *it;
      }
    }
    return Null_ptr;
  }

  //! @brief get parent stmt
  STMT_PTR Parent_stmt() const {
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
      if ((*it)->Is_root()) {
        return (*it)->Stmt();
      }
    }
    return Null_ptr;
  }

  //! @brief manually push a node to visiting stack
  //! @param node node to be pushed onto stack
  void Push(const NODE_PTR& node) { _stack.push_back(node); }

  //! @brief manually pop a node from visiting stack
  //! @param node node to be poped from stack
  void Pop(const NODE_PTR& node) {
    AIR_ASSERT(!_stack.empty() && _stack.back() == node);
    _stack.pop_back();
  }

  //! @brief check if node stack is empty
  bool Empty() const { return _stack.empty(); }

  //! @brief default BLOCK handler
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, NODE_PTR node) {
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      visitor->template Visit<RETV>(stmt->Node());
    }
    return RETV();
  }

  //! @brief default NODE handler
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    return RETV();
  }

  //! @brief unknown DOMAIN handler
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT_MSG(false, "Internal error: unknown domain");
    return RETV();
  }

  //! @brief define type for NODE_STACK
  using NODE_STACK = std::vector<NODE_PTR>;

  //! @brief guard to push/pop node automatically
  class GUARD {
  public:
    //! @brief constructor, push node onto stack
    GUARD(ANALYZE_CTX& ctx, const NODE_PTR& node)
        : _stack(ctx.Node_stack()), _node(node) {
      _stack.push_back(node);
    }

    //! @brief destructor, pop node from stack
    ~GUARD() {
      AIR_ASSERT(!_stack.empty() && _stack.back() == _node);
      _stack.pop_back();
    }

  private:
    NODE_STACK&     _stack;  // node stack
    const NODE_PTR& _node;   // current node
  };                         // GUARD

private:
  ANALYZE_CTX(const ANALYZE_CTX&)            = delete;
  ANALYZE_CTX(const ANALYZE_CTX&&)           = delete;
  ANALYZE_CTX& operator=(const ANALYZE_CTX&) = delete;

protected:
  // enable GUARD access _stack
  friend class GUARD;

  // Get stack for all parents of current node
  NODE_STACK& Node_stack() { return _stack; }

  // stack for parent node in old container
  NODE_STACK _stack;

};  // ANALYZE_CTX

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_ANALYZE_CTX_H
