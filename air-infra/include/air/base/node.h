//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_NODE_H
#define AIR_BASE_NODE_H

#include "air/base/node_data.h"
#include "air/base/st_attr.h"
#include "air/base/st_decl.h"

namespace air {
namespace base {

class NODE {
  friend class CONTAINER;
  friend class STMT;
  PTR_FRIENDS(NODE);

public:
  NODE() : _cont(0), _data() {}
  NODE(const NODE& node) : _cont(node._cont), _data(node._data) {}
  NODE(CONTAINER* c, NODE_DATA_PTR node) : _cont(c), _data(node) {}

  NODE_ID     Id() const { return _data.Id(); }
  CONTAINER*  Container() const { return _cont; }
  FUNC_SCOPE* Func_scope() const;
  GLOB_SCOPE& Glob_scope() const;
  const char* Name() const;
  uint32_t    Num_child() const;
  size_t      Size() const;
  STMT_PTR    Stmt() const;

  OPCODE   Opcode() const;
  uint32_t Domain() const { return Opcode().Domain(); }
  uint32_t Operator() const { return Opcode().Operator(); }
  uint64_t Intconst() const;

  STMT_ID  Begin_stmt_id() const;
  STMT_PTR Begin_stmt() const;
  STMT_ID  End_stmt_id() const;
  STMT_PTR End_stmt() const;
  TYPE_PTR Rtype() const;
  TYPE_ID  Rtype_id() const;
  SPOS     Spos() const { return _data->_comm._spos; }
  NODE_PTR Child(uint32_t n) const;
  NODE_ID  Child_id(uint32_t n) const;
  NODE_PTR Last_child() const;
  NODE_ID  Last_child_id() const;
  NODE_PTR Addr() const;
  NODE_ID  Addr_id() const;
  //! Get node of loop init expr
  NODE_PTR Loop_init() const;
  //! Get node ID of loop init expr
  NODE_ID Loop_init_id() const;
  //! Get node of loop compare expr
  NODE_PTR Compare() const;
  //! Get node ID of loop compare expr
  NODE_ID Compare_id() const;
  //! Get node of loop increment expr
  NODE_PTR Loop_incr() const;
  //! Get node ID of loop increment expr
  NODE_ID Loop_incr_id() const;
  //! Get node of loop/function body
  NODE_PTR Body_blk() const;
  //! Get node ID of loop/function body
  NODE_ID Body_blk_id() const;
  //! Get node of then block of if statement
  NODE_PTR Then_blk() const;
  //! Get node ID of then block of if statement
  NODE_ID Then_blk_id() const;
  //! Get node of else block of if statement
  NODE_PTR Else_blk() const;
  //! Get node ID of else block of if statement
  NODE_ID  Else_blk_id() const;
  NODE_PTR Index() const;
  NODE_ID  Index_id() const;
  //! Get return preg
  PREG_PTR Ret_preg() const;
  //! Get ID of return preg
  PREG_ID Ret_preg_id() const;
  //! Number of indexed dimensions of an array element node
  uint32_t Array_dim() const;
  //! Base address of an array element node
  NODE_PTR Array_base() const;
  //! Get index node of dim's dimension, start from zero
  NODE_PTR Array_idx(uint32_t dim) const;

  TYPE_PTR       Access_type() const;
  TYPE_ID        Access_type_id() const;
  ADDR_DATUM_PTR Addr_datum() const;
  ADDR_DATUM_ID  Addr_datum_id() const;
  ADDR_DATUM_PTR Iv() const;
  ADDR_DATUM_ID  Iv_id() const;
  BLOCK_PTR      Block() const;
  BLOCK_ID       Block_id() const;
  ENTRY_PTR      Entry() const;
  ENTRY_ID       Entry_id() const;
  FIELD_PTR      Field() const;
  FIELD_ID       Field_id() const;
  STMT_PTR       Parent_stmt() const;
  STMT_ID        Parent_stmt_id() const;
  CONSTANT_PTR   Const() const;
  CONSTANT_ID    Const_id() const;
  //! Get operand preg of the node
  PREG_PTR Preg() const;
  //! Get operand preg ID of the node
  PREG_ID Preg_id() const;
  //! Get offset value
  int64_t Ofst() const;

  uint32_t Num_arg() const;

  ATTR_ITER Begin_attr() const;
  ATTR_ITER End_attr() const;
  void      Copy_attr(CONST_NODE_PTR node);
  DECLATR_ATTR_ACCESS_API(Attr_id(), (SCOPE_BASE*)Func_scope())

  bool Is_root() const;
  bool Is_leaf() const;
  bool Is_call() const;
  bool Is_dir_call() const;
  bool Is_icall() const;
  bool Is_intrn_call() const;
  bool Is_block() const;
  bool Is_entry() const;
  bool Is_ret() const;
  bool Is_const_ld() const;
  bool Is_ld() const;
  bool Is_ild() const;
  bool Is_dir_ld() const;
  bool Is_st() const;
  bool Is_una_arith_op() const;
  bool Is_bin_arith_op() const;
  bool Is_idx_op() const;
  bool Is_type_cast_op() const;
  bool Is_if() const;
  bool Is_do_loop() const;
  bool Is_while_do() const;
  bool Is_do_while() const;
  bool Is_empty_blk() const;
  bool Is_preg_op() const;
  bool Is_relational_op() const;
  bool Is_lib_call() const;

  bool Has_added_chld() const;
  bool Has_entry() const;
  bool Has_fld() const;
  bool Has_ret_var() const;
  bool Has_rtype() const;
  bool Has_sym() const;
  bool Has_preg() const;
  bool Has_const_id() const;
  bool Has_access_type() const;
  bool Has_ofst() const;

  void Set_child(uint32_t n, CONST_NODE_PTR code);
  void Set_child(uint32_t n, NODE_ID cid);
  void Set_spos(const SPOS& s) { _data->_comm._spos = s; }
  void Set_rtype(CONST_TYPE_PTR type);
  void Set_rtype(TYPE_ID tid);
  void Set_begin_stmt(CONST_STMT_PTR stmt);
  void Set_begin_stmt(STMT_ID stmt);
  void Set_addr_datum(CONST_ADDR_DATUM_PTR datum);
  void Set_addr_datum(ADDR_DATUM_ID datum);
  void Set_arg(uint32_t num, CONST_NODE_PTR arg);
  void Set_arg(uint32_t num, NODE_ID arg);
  void Set_access_type(CONST_TYPE_PTR type);
  void Set_access_type(TYPE_ID type);
  void Set_block(CONST_BLOCK_PTR blk);
  void Set_block(BLOCK_ID blk);
  void Set_end_stmt(CONST_STMT_PTR stmt);
  void Set_end_stmt(STMT_ID stmt);
  void Set_entry(CONST_ENTRY_PTR ent);
  void Set_entry(ENTRY_ID);
  //! Set return value of a function to retv
  void Set_ret_preg(CONST_PREG_PTR retv);
  void Set_ret_preg(PREG_ID retv);
  void Set_field(CONST_FIELD_PTR fld);
  void Set_field(FIELD_ID fld);
  void Set_parent_stmt(CONST_STMT_PTR stmt);
  void Set_parent_stmt(STMT_ID stmt);
  void Set_num_arg(uint32_t n);
  void Set_const(CONST_CONSTANT_PTR cst);
  void Set_const(CONSTANT_ID id);
  void Set_preg(CONST_PREG_PTR preg);
  void Set_preg(PREG_ID id);
  void Set_iv(CONST_ADDR_DATUM_PTR iv);
  void Set_iv(ADDR_DATUM_ID id);
  void Set_intconst(uint64_t val);
  void Set_ofst(int64_t ofst);

  void Print_tree(std::ostream& os, bool rot = true, uint32_t indent = 0) const;
  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print() const;
  std::string To_str(bool rot = true) const;

private:
  NODE(const CONTAINER* c, NODE_DATA_PTR node)
      : _cont(const_cast<CONTAINER*>(c)), _data(node) {}

  void Set_opcode(OPCODE op) {
    _data->_comm._core._opcode = static_cast<uint32_t>(op);
  }
  bool     Is_null() const { return _data.Is_null(); }
  uint32_t Num_fld() const;
  uint32_t Num_added_chld() const;

  NODE_DATA_PTR Data() const { return _data; }

  const ATTR_ID& Attr_id() const;

  CONTAINER*    _cont;
  NODE_DATA_PTR _data;
};

class STMT {
  friend class CONTAINER;
  friend class NODE;
  PTR_FRIENDS(STMT)

public:
  STMT_ID     Id() const { return _data.Id(); }
  CONTAINER*  Container() const { return _cont; }
  FUNC_SCOPE* Func_scope() const;
  GLOB_SCOPE& Glob_scope() const;
  STMT_PTR    Prev() const;
  STMT_ID     Prev_id() const;
  STMT_PTR    Next() const;
  STMT_ID     Next_id() const;
  NODE_PTR    Parent_node() const;
  NODE_ID     Parent_node_id() const;
  STMT_PTR    Enclosing_stmt() const;
  STMT_ID     Enclosing_stmt_id() const;
  NODE_PTR    Node() const;
  SPOS        Spos() const { return _data->_data._comm._spos; }

  OPCODE   Opcode() const { return (OPCODE)_data->_data._comm._core._opcode; }
  uint32_t Domain() const { return Opcode().Domain(); }
  uint32_t Operator() const { return Opcode().Operator(); }

  bool Has_prev() const;
  bool Has_next() const;
  bool Has_parent_node() const;
  bool Has_enclosing_stmt() const;

  void Set_prev(CONST_STMT_PTR stmt);
  void Set_prev(STMT_ID stmt);
  void Set_next(CONST_STMT_PTR stmt);
  void Set_next(STMT_ID stmt);
  void Set_parent_node(CONST_NODE_PTR node);
  void Set_parent_node(NODE_ID node);

  void Print(std::ostream& os, bool rot = true, uint32_t indent = 0) const;
  void Print() const;
  std::string To_str(bool rot = true) const;

private:
  STMT() : _cont(0), _data() {}
  STMT(const STMT& stmt) : _cont(stmt._cont), _data(stmt._data) {}
  STMT(CONTAINER* c, STMT_DATA_PTR stmt) : _cont(c), _data(stmt) {}
  STMT(const CONTAINER* c, STMT_DATA_PTR stmt)
      : _cont(const_cast<CONTAINER*>(c)), _data(stmt) {}

  bool Is_null() const { return _data.Is_null(); }

  STMT_DATA_PTR Data() const { return _data; }

  CONTAINER*    _cont;
  STMT_DATA_PTR _data;
};

}  // namespace base
}  // namespace air

#endif
