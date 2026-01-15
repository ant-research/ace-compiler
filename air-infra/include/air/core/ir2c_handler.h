//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CORE_IR2C_HANDLER_H
#define AIR_CORE_IR2C_HANDLER_H

#include "air/base/ir2c_ctx.h"
#include "air/core/invalid_handler.h"
#include "air/core/opcode.h"

namespace air {
namespace core {

//! @brief IR2C handler which handler all kinds of nodes in CORE and generate
//  C code for those nodes.
class IR2C_HANDLER : public INVALID_HANDLER {
public:
  //! @brief Construct a new ir2c handler object
  IR2C_HANDLER() {}

  //! @brief Handle BLOCK for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Is_block());
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx.Begin_block(node);
    for (air::base::STMT_PTR stmt       = node->Begin_stmt();
         stmt != node->End_stmt(); stmt = stmt->Next()) {
      air::base::NODE_PTR snode = stmt->Node();
      ctx.Begin_stmt(snode);
      visitor->template Visit<RETV>(snode);
      ctx.End_stmt(snode);
    }
    ctx.End_block(node);
    return RETV();
  }

  //! @brief Handle comment for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_comment(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx << "// " << node->Comment() << "\n";
    return RETV();
  }

  //! @brief Handle pragma for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_pragma(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx << "// pragma: " << node->Pragma_id() << ", " << node->Pragma_arg0()
        << ", " << node->Pragma_arg1() << "\n";
  }

  //! @brief Handle FUNC_ENTRY for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX&   ctx    = visitor->Context();
    air::base::FUNC_SCOPE* fscope = node->Func_scope();
    ctx.Emit_func_def(fscope);
    ctx.Begin_func_body(node);
    ctx.Emit_local_var(fscope);
    visitor->template Visit<RETV>(node->Last_child());
    ctx.End_func_body(node);
    ctx << "\n";
    return RETV();
  }

  //! @brief Handle DO_LOOP for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx << "for (";
    ctx.Emit_var(node->Iv());
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << "; ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << "; ";
    ctx.Emit_var(node->Iv());
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(2));
    ctx << ") ";
    visitor->template Visit<RETV>(node->Child(3));
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_if(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx << "if (";
    visitor->template Visit<RETV>(node->Child(0));  // condition
    ctx << ") ";
    visitor->template Visit<RETV>(node->Child(1));  // then block
    if (node->Num_child() > 2 && node->Child(2) != air::base::Null_ptr) {
      ctx << " else ";
      visitor->template Visit<RETV>(node->Child(2));  // else block
    }
    return RETV();
  }

  //! @brief Handle binary operator for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_binop(VISITOR* visitor, air::base::NODE_PTR node,
                    const char* op_str) {
    air::base::IR2C_CTX& ctx    = visitor->Context();
    air::base::NODE_PTR  parent = visitor->Parent(1);
    ctx.Begin_expr(node, parent);
    visitor->template Visit<RETV>(node->Child(0));
    ctx << op_str;
    visitor->template Visit<RETV>(node->Child(1));
    ctx.End_expr(node, parent);
    return RETV();
  }

  //! @brief Handle unary operator for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_unaop(VISITOR* visitor, air::base::NODE_PTR node,
                    const char* op_str) {
    air::base::IR2C_CTX& ctx    = visitor->Context();
    air::base::NODE_PTR  parent = visitor->Parent(1);
    ctx.Begin_expr(node, parent);
    ctx << op_str;
    visitor->template Visit<RETV>(node->Child(0));
    ctx.End_expr(node, parent);
    return RETV();
  }

  //! @brief Handle ADD for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " + ");
  }

  //! @brief Handle SUB for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " - ");
  }

  //! @brief Handle MUL for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " * ");
  }

  //! @brief Handle SHL for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_shl(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " << ");
  }

  //! @brief Handle LT for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_lt(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " < ");
  }

  //! @brief Handle LE for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_le(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " <= ");
  }

  //! @brief Handle GT for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_gt(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " > ");
  }

  //! @brief Handle GE for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ge(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " >= ");
  }

  //! @brief Handle EQ for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_eq(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " == ");
  }

  //! @brief Handle NE for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ne(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " != ");
  }

  //! @brief Handle BAND (bitwise AND) for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_band(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " & ");
  }

  //! @brief Handle BOR (bitwise OR) for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_bor(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " | ");
  }

  //! @brief Handle LAND (logical AND) for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_land(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " && ");
  }

  //! @brief Handle LOR (logical OR) for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_lor(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " || ");
  }

  //! @brief Handle LNOT (logical NOT) for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_lnot(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_unaop<RETV>(visitor, node, "!");
  }

  //! @brief Handle BNOT (bitwise NOT) for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_bnot(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_unaop<RETV>(visitor, node, "~");
  }

  //! @brief Handle MOD for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_mod(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " % ");
  }

  //! @brief Handle DIV for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_div(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " / ");
  }

  //! @brief Handle FLOORDIV for IR2C (integer division)
  template <typename RETV, typename VISITOR>
  RETV Handle_floordiv(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_binop<RETV>(visitor, node, " / ");
  }

  //! @brief handle LDCA for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ldca(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX&          ctx = visitor->Context();
    air::base::CONST_CONSTANT_PTR cst = node->Const();
    if (cst->Kind() != air::base::CONSTANT_KIND::ARRAY) {
      ctx << "&";
    }
    ctx.Emit_constant_name(cst->Id());
    return RETV();
  }

  //! @brief Handle LDC for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX&    ctx = visitor->Context();
    air::base::CONSTANT_PTR cst = node->Const();
    if (cst->Kind() == air::base::CONSTANT_KIND::SIGNED_INT) {
      ctx << cst->Integer_literal().Val_as_int64();
    } else if (cst->Kind() == air::base::CONSTANT_KIND::UNSIGNED_INT) {
      ctx << cst->Integer_literal().Val_as_uint64();
    } else if (cst->Kind() == air::base::CONSTANT_KIND::FLOAT) {
      ctx << cst->Float_literal().Val_as_double();
    } else if (cst->Kind() == air::base::CONSTANT_KIND::ARRAY) {
      ctx.Emit_constant_name(cst->Id());
    } else if (cst->Kind() == air::base::CONSTANT_KIND::STR_ARRAY) {
      ctx.Emit_constant_str_init(cst);
    } else {
      AIR_ASSERT(false);
    }
    return RETV();
  }

  //! @brief Handle INTCONST for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_intconst(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    if (node->Rtype()->Is_signed_int()) {
      ctx << (int64_t)node->Intconst();
    } else {
      ctx << node->Intconst();
    }
    return RETV();
  }

  //! @brief Handle ARRAY for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_array(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    visitor->template Visit<RETV>(node->Child(0));
    for (uint32_t i = 1; i < node->Num_child(); ++i) {
      ctx << "[";
      visitor->template Visit<RETV>(node->Child(i));
      ctx << "]";
    }
  }

  //! @brief Handle LDP for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Emit_preg_id(node->Preg_id());
    return RETV();
  }

  //! @brief Handle LOAD for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Emit_var(node);
    return RETV();
  }

  //! @brief Handle LDA for IR2C
  template <typename RETV, typename VISITOR>
  void Handle_lda(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Emit_var(node);
    return RETV();
  }

  //! @brief Handle ILOAD for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR addr       = node->Child(0);
    bool                need_star  = (addr->Opcode() != OPC_ARRAY);
    bool                need_paren = false;
    if (need_star) {
      need_paren = (addr->Opcode() != OPC_LD);
      visitor->Context() << (need_paren ? "*(" : "*");
    }
    visitor->template Visit<RETV>(addr);
    if (need_paren) {
      visitor->Context() << ")";
    }
    return RETV();
  }

  //! @brief Handle CALL for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    if (node->Has_ret_var() && node->Ret_preg_id() != air::base::Null_ptr) {
      ctx.Emit_preg_id(node->Ret_preg_id());
      ctx << " = ";
    }
    ctx.Emit_sym(node->Entry());
    Emit_param_list<RETV>(visitor, node);
    return RETV();
  }

  //! @brief Handle FUNC for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_func(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT_MSG(false, "TODO: handle func");
    return RETV();
  }

  //! @brief Handle STORE for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx.Emit_var(node);
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(0));
    return RETV();
  }

  //! @brief Handle IST for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    // 1. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    bool           need_star  = (addr_child->Opcode() != OPC_ARRAY);
    bool           need_paren = false;
    if (need_star) {
      need_paren = (addr_child->Opcode() != OPC_LD);
      ctx << (need_paren ? "*(" : "*");
    }
    visitor->template Visit<RETV>(addr_child);
    ctx << (need_paren ? ") = " : " = ");

    // 2. handle rhs node
    visitor->template Visit<RETV>(node->Child(1));
    return RETV();
  }

  //! @brief Handle RETV for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context() << "return ";
    visitor->template Visit<RETV>(node->Child(0));
    return RETV();
  }

  //! @brief Handle RET for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_ret(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context() << "return";
    return RETV();
  }

  //! @brief Handle INTRN_CALL for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_intrn_call(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    if (node->Has_ret_var() && node->Ret_preg_id() != air::base::Null_ptr) {
      ctx.Emit_preg_id(node->Ret_preg_id());
      ctx << " = ";
    }
    ctx << node->Intrn_name();
    Emit_param_list<RETV>(visitor, node);
    return RETV();
  }

  //! @brief Handle INTRN_OP for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_intrn_op(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context() << node->Intrn_name();
    Emit_param_list<RETV>(visitor, node);
    return RETV();
  }

private:
  template <typename RETV, typename VISITOR>
  void Emit_param_list(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx << "(";
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      if (i > 0) {
        ctx << ", ";
      }
      visitor->template Visit<RETV>(node->Child(i));
    }
    ctx << ")";
  }
};

}  // namespace core
}  // namespace air

#endif  // AIR_CORE_IR2C_HANDLER_H
