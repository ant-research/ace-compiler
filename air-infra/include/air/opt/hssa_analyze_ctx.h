//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_ANALYZE_CTX_H
#define AIR_OPT_HSSA_ANALYZE_CTX_H

#include <vector>

#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/hssa_stmt.h"
#include "air/opt/hssa_visitor.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

//! @brief context for analyze pass used by VISITOR
class HSSA_ANALYZE_CTX {
public:
  //! @brief construct a new analyze context
  HSSA_ANALYZE_CTX(CFG& cfg) : _cfg(cfg) {}

  //! @brief destruct TRANSFORM object
  ~HSSA_ANALYZE_CTX() { AIR_ASSERT(_stack.empty()); }

  CFG&        Cfg() const { return _cfg; }
  HCONTAINER& Hssa_cont() const { return *_cfg.Hssa_cont(); }

  NODE_INFO Parent(size_t nth) const {
    AIR_ASSERT(!_stack.empty());
    size_t sz = _stack.size();
    return sz > nth ? _stack[sz - 1 - nth]
                    : std::make_pair(air::base::Null_ptr, air::base::Null_ptr);
  }

  //! @brief get parent stmt
  HSTMT_PTR Parent_stmt() const {
    if (_stack.empty()) return air::base::Null_ptr;
    return Parent(0).second;
  }

  //! @brief manually push a node to visiting stack
  //! @param node node to be pushed onto stack
  void Push(const HEXPR_PTR& expr, const HSTMT_PTR& stmt) {
    _stack.push_back(std::make_pair(expr, stmt));
  }

  //! @brief manually pop a node from visiting stack
  //! @param node node to be poped from stack
  void Pop(const HEXPR_PTR& expr, const HSTMT_PTR& stmt) {
    AIR_ASSERT(!_stack.empty() && _stack.back().first == expr &&
               _stack.back().second == stmt);
    _stack.pop_back();
  }

  //! @brief check if node stack is empty
  bool Empty() const { return _stack.empty(); }

  template <typename RETV, typename VISITOR>
  RETV Handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    switch (node->Kind()) {
      case EK_VAR:
      case EK_CONST:
        break;
      case EK_OP: {
        OP_DATA_PTR op_expr = node->Cast_to_op_expr();
        for (size_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
          visitor->template Visit<RETV>(node->Kid(idx));
        }
      } break;
      default:
        CMPLR_ASSERT(false, "node not handled");
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stmt(VISITOR* visitor, HSTMT_PTR node) {
    switch (node->Kind()) {
      case SK_ASSIGN: {
        ASSIGN_DATA_PTR ass_stmt = node->Cast_to_assign();
        visitor->template Visit<RETV>(Hssa_cont().Expr_ptr(ass_stmt->Rhs()));
        break;
      }
      case SK_NARY: {
        NARY_DATA_PTR op_stmt = node->Cast_to_nary();
        uint32_t      kid_cnt = op_stmt->Kid_cnt();
        // special handle for func entry
        if (node->Opcode() == air::core::FUNC_ENTRY) {
          kid_cnt--;
        }
        for (size_t idx = 0; idx < kid_cnt; idx++) {
          visitor->template Visit<RETV>(
              Hssa_cont().Expr_ptr(op_stmt->Kid(idx)));
        }
        break;
      }
      case SK_CALL: {
        CALL_DATA_PTR call_stmt = node->Cast_to_call();
        uint32_t      kid_cnt   = call_stmt->Arg_cnt();
        for (size_t idx = 0; idx < kid_cnt; idx++) {
          visitor->template Visit<RETV>(
              Hssa_cont().Expr_ptr(call_stmt->Arg(idx)));
        }
        break;
      }
      case SK_IF: {
        IF_DATA_PTR if_stmt = node->Cast_to_if();
        visitor->template Visit<RETV>(Hssa_cont().Expr_ptr(if_stmt->Cond()));
        break;
      }
      default:
        AIR_ASSERT_MSG(false, "not yet implemented");
    }
    return RETV();
  }

  //! @brief unknown DOMAIN handler
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, HSTMT_PTR node) {
    return Handle_stmt<RETV, VISITOR>(visitor, node);
    // AIR_ASSERT_MSG(false, "Internal error: unknown domain");
    // return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, HEXPR_PTR node) {
    return Handle_expr<RETV, VISITOR>(visitor, node);
    // AIR_ASSERT_MSG(false, "Internal error: unknown domain");
    // return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_stmt(VISITOR* visitor, HSTMT_PTR node) {
    // std::cout << "======================\n";
    // std::cout << "Enter stmt[" << node->Id().Value() << "]" << std::endl;
    // node->Print();
    // std::cout << std::endl;

    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    // std::cout << "======================\n";
    // std::cout << "Enter expr[" << node->Id().Value() << "]" << std::endl;
    // node->Print();
    // std::cout << std::endl;

    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_stmt(VISITOR* visitor, HSTMT_PTR node) {
    // std::cout << "Exit stmt[" << node->Id().Value() << "]" << std::endl;
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    // std::cout << "Exit expr[" << node->Id().Value() << "]" << std::endl;
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_bb(VISITOR* visitor, BB_PTR bb) {
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_bb(VISITOR* visitor, BB_PTR bb) {
    return RETV();
  }

  //! @brief define type for NODE_STACK
  using NODE_STACK = std::vector<NODE_INFO>;

  //! @brief guard to push/pop node automatically
  class GUARD {
  public:
    //! @brief constructor, push node onto stack
    GUARD(HSSA_ANALYZE_CTX& ctx, const HEXPR_PTR& node, const HSTMT_PTR& stmt)
        : _stack(ctx.Node_stack()), _node_info(node, stmt) {
      _stack.push_back(std::make_pair(node, stmt));
    }

    //! @brief constructor, push node onto stack
    GUARD(HSSA_ANALYZE_CTX& ctx, const HEXPR_PTR& node)
        : _stack(ctx.Node_stack()) {
      HSTMT_PTR cur_stmt = _stack.empty() ? Null_ptr : _stack.back().second;
      _node_info         = std::make_pair(node, cur_stmt);
      _stack.push_back(_node_info);
    }

    //! @brief constructor, push node onto stack
    GUARD(HSSA_ANALYZE_CTX& ctx, const HSTMT_PTR& stmt)
        : _stack(ctx.Node_stack()) {
      _node_info = std::make_pair(Null_ptr, stmt);
      _stack.push_back(_node_info);
    }

    //! @brief destructor, pop node from stack
    ~GUARD() {
      AIR_ASSERT(!_stack.empty() && _stack.back() == _node_info);
      _stack.pop_back();
    }

  private:
    NODE_STACK& _stack;      // node stack
    NODE_INFO   _node_info;  // current node info
  };                         // GUARD

private:
  HSSA_ANALYZE_CTX(const HSSA_ANALYZE_CTX&)            = delete;
  HSSA_ANALYZE_CTX(const HSSA_ANALYZE_CTX&&)           = delete;
  HSSA_ANALYZE_CTX& operator=(const HSSA_ANALYZE_CTX&) = delete;

protected:
  // enable GUARD access _stack
  friend class GUARD;

  // Get stack for all parents of current node
  NODE_STACK& Node_stack() { return _stack; }

  CFG& _cfg;

  // stack for parent node in old container
  NODE_STACK _stack;

};  // HSSA_ANALYZE_CTX

}  // namespace opt
}  // namespace air

#endif  // AIR_BASE_ANALYZE_CTX_H
