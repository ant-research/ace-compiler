//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY2C_MFREE_H
#define FHE_POLY_POLY2C_MFREE_H

#include <unordered_set>

#include "air/base/analyze_ctx.h"
#include "air/core/opcode.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/opcode.h"

namespace fhe {

namespace poly {

//! @brief Utility to help insert Free_poly
class MFREE_UTIL {
public:
  MFREE_UTIL(air::base::STMT_PTR pos) : _pos(pos) {
    AIR_ASSERT(pos != air::base::Null_ptr);
  }

  template <typename VAR_TYPE>
  void Insert_mfree(VAR_TYPE var) {
    air::base::TYPE_PTR type = var->Type();
    if (type->Is_array()) {
      Gen_free_array(var);
    } else if (type->Is_record()) {
      Gen_free_record(var);
    } else {
      AIR_ASSERT(false);
    }
  }

private:
  template <typename VAR_TYPE>
  void Gen_free_array(VAR_TYPE var) {
    AIR_ASSERT(var->Type()->Is_array());
    air::base::ARRAY_TYPE_PTR type = var->Type()->Cast_to_arr();
    AIR_ASSERT(type->Dim() == 1);
    Append_free_stmt(Gen_ld(var));
  }

  template <typename VAR_TYPE>
  void Gen_free_record(VAR_TYPE var) {
    AIR_ASSERT(var->Type()->Is_record());
    air::base::FIELD_ITER fld_iter = var->Type()->Cast_to_rec()->Begin();
    air::base::FIELD_ITER fld_end  = var->Type()->Cast_to_rec()->End();
    for (; fld_iter != fld_end; ++fld_iter) {
      Append_free_stmt(Gen_ldf(var, *fld_iter));
    }
  }

  void Append_free_stmt(air::base::NODE_PTR var) {
    AIR_ASSERT(var->Opcode() == air::core::OPC_LD ||
               var->Opcode() == air::core::OPC_LDP ||
               var->Opcode() == air::core::OPC_LDF ||
               var->Opcode() == air::core::OPC_LDPF);
    air::base::STMT_PTR stmt =
        Container()->New_cust_stmt(fhe::poly::OPC_FREE, Spos());
    stmt->Node()->Set_child(0, var);
    air::base::STMT_LIST list(_pos->Parent_node());
    list.Append(_pos, stmt);
  }

  air::base::NODE_PTR Gen_ld(air::base::ADDR_DATUM_PTR var) {
    return Container()->New_ld(var, Spos());
  }

  air::base::NODE_PTR Gen_ld(air::base::PREG_PTR var) {
    return Container()->New_ldp(var, Spos());
  }

  air::base::NODE_PTR Gen_ldf(air::base::ADDR_DATUM_PTR var,
                              air::base::FIELD_PTR      fld) {
    return Container()->New_ldf(var, fld, Spos());
  }

  air::base::NODE_PTR Gen_ldf(air::base::PREG_PTR  var,
                              air::base::FIELD_PTR fld) {
    return Container()->New_ldpf(var, fld, Spos());
  }

  air::base::CONTAINER* Container() { return _pos->Container(); }
  air::base::SPOS       Spos() { return _pos->Spos(); }

  air::base::STMT_PTR _pos;
};

//! @brief Pass to analyze and insert Free_poly
class MFREE_PASS {
public:
  MFREE_PASS(const fhe::core::LOWER_CTX& ctx) : _lower_ctx(ctx) {}

  void Perform(air::base::NODE_PTR body);

  template <typename VAR_TYPE>
  bool Need_mfree(VAR_TYPE var) {
    auto& var_set = Var_set(var);
    if (var_set.find(var->Id().Value()) == var_set.end()) {
      var_set.insert(var->Id().Value());
      return true;
    }
    return false;
  }

  template <typename VAR_TYPE>
  void Mark_var_freed(VAR_TYPE var) {
    auto& var_set = Var_set(var);
    var_set.insert(var->Id().Value());
  }

  bool Type_need_mfree(air::base::TYPE_PTR type) {
    if (_lower_ctx.Is_plain_type(type->Id()) ||
        _lower_ctx.Is_cipher_type(type->Id()) ||
        _lower_ctx.Is_cipher3_type(type->Id())) {
      return true;
    } else if (type->Is_array()) {
      return Type_need_mfree(type->Cast_to_arr()->Elem_type());
    } else {
      return false;
    }
  }

  const fhe::core::LOWER_CTX& Lower_ctx() const { return _lower_ctx; }

private:
  MFREE_PASS(const MFREE_PASS&)            = delete;
  MFREE_PASS& operator=(const MFREE_PASS&) = delete;

  std::unordered_set<uint32_t>& Var_set(air::base::ADDR_DATUM_PTR var) {
    return _sym_set;
  }
  std::unordered_set<uint32_t>& Var_set(air::base::PREG_PTR var) {
    return _preg_set;
  }

  const fhe::core::LOWER_CTX&  _lower_ctx;
  std::unordered_set<uint32_t> _sym_set;
  std::unordered_set<uint32_t> _preg_set;
};

//! @brief Context to forward traverse IR tree
//!  - mark variables won't be freed
//!  - free return from Rotate in the same block (may be inside a loop)
class MFREE_FWD_CTX : public air::base::ANALYZE_CTX {
public:
  //! @brief Construct a new MFREE_FWD_CTX object
  MFREE_FWD_CTX(MFREE_PASS& pass) : _pass(pass) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    if (node->Opcode() == air::core::OPC_ST &&
        node->Child(0)->Opcode() == air::core::OPC_ILD &&
        node->Child(0)->Child(0)->Opcode() == air::core::OPC_ARRAY) {
      // we have loop to free whole array, so won't free individual
      _pass.Mark_var_freed(node->Addr_datum());
    } else if (node->Opcode() == air::core::OPC_CALL &&
               strcmp(node->Entry()->Name()->Char_str(), "Rotate") == 0 &&
               Parent(2) != air::base::Null_ptr) {
      // rotate return value should be freed in end of the same level
      // but if rotate is in top-level, still after last use
      AIR_ASSERT(node->Has_ret_var());
      air::base::PREG_PTR retv = node->Ret_preg();
      AIR_ASSERT(_pass.Type_need_mfree(retv->Type()));
      AIR_ASSERT(_pass.Need_mfree(retv));
      _pass.Mark_var_freed(retv);

      air::base::STMT_PTR pos = Find_insert_pos();
      AIR_ASSERT(pos != air::base::Null_ptr);

      MFREE_UTIL util(pos);
      util.Insert_mfree(retv);
    } else if (node->Is_do_loop()) {
      visitor->template Visit<RETV>(node->Body_blk());
    }
  }

  //! @brief Handle unknown domain node
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_node<RETV>(visitor, node);
  }

private:
  air::base::STMT_PTR Find_insert_pos() {
    air::base::NODE_PTR blk = Parent_block();
    AIR_ASSERT(blk != air::base::Null_ptr);
    air::base::STMT_LIST list(blk);
    AIR_ASSERT(!list.Is_empty());
    air::base::STMT_PTR pos = list.Last_stmt();
    if (pos->Node()->Opcode() == air::core::OPC_RET ||
        pos->Node()->Opcode() == air::core::OPC_RETV) {
      return pos->Prev();
    }
    return pos;
  }

  MFREE_PASS& _pass;
};  // MFREE_FWD_CTX

//! @brief Context to backward traverse IR tree and instrument free operator
class MFREE_BWD_CTX : public air::base::ANALYZE_CTX {
public:
  //! @brief Construct a new MFREE_BWD_CTX object
  MFREE_BWD_CTX(MFREE_PASS& pass) : _pass(pass) {}

  //! @biref Backword traverse the stmt list in block
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Is_block());
    air::base::STMT_LIST list(node);
    if (list.Is_empty()) {
      return RETV();
    }
    air::base::STMT_PTR stmt = node->End_stmt();
    while (stmt->Has_prev()) {
      stmt = stmt->Prev();
      visitor->template Visit<RETV>(stmt->Node());
    }
    return RETV();
  }

  //! @brief Traverse the kids of the node to handle all variables
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    if (node->Has_sym()) {
      air::base::ADDR_DATUM_PTR var = node->Addr_datum();
      bool is_entry = node->Func_scope()->Owning_func()->Entry_point()->Is_program_entry();
      if ((!is_entry && var->Is_formal()) || Parent(1)->Opcode() == air::core::OPC_RETV) {
        // don't free formal excpet main and return value
        _pass.Mark_var_freed(var);
        return;
      }
      Handle_variable(node->Addr_datum());
    } else if (node->Has_preg()) {
      Handle_variable(node->Preg());
    }
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    return RETV();
  }

  //! @brief Handle unknown domain node
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_node<RETV>(visitor, node);
  }

private:
  template <typename VAR_TYPE>
  void Handle_variable(VAR_TYPE var) {
    air::base::TYPE_PTR type = var->Type();
    if (_pass.Type_need_mfree(type) && _pass.Need_mfree(var)) {
      air::base::STMT_PTR pos = Find_insert_pos();
      AIR_ASSERT(pos != air::base::Null_ptr);
      MFREE_UTIL util(pos);
      util.Insert_mfree(var);
    }
  }

  air::base::STMT_PTR Find_insert_pos() {
    const auto& stack = Node_stack();
    for (auto it = stack.begin(); it != stack.end(); ++it) {
      if ((*it)->Is_block()) {
        continue;
      }
      AIR_ASSERT((*it)->Is_root());
      return (*it)->Stmt();
    }
    AIR_ASSERT(false);
    return air::base::Null_ptr;
  }

  MFREE_PASS& _pass;
};  // MFREE_BWD_CTX

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY2C_MFREE_H
