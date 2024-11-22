//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_CONTAINER_H
#define AIR_BASE_CONTAINER_H

#include <cstdio>

#include "air/base/container_decl.h"
#include "air/base/node.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"

namespace air {
namespace base {

typedef ARENA<4, 4, true> CODE_ARENA;

class STMT_LIST {
public:
  STMT_LIST(NODE_PTR blk);

  CONTAINER* Container() const;
  NODE_PTR   Block_node() const { return _blk_node; }

  STMT_PTR Begin_stmt() const;
  STMT_ID  Begin_stmt_id() const;
  STMT_PTR End_stmt() const;
  STMT_ID  End_stmt_id() const;
  STMT_PTR Last_stmt() const;
  STMT_ID  Last_stmt_id() const;

  void Set_begin_stmt(STMT_PTR stmt);
  void Set_end_stmt(STMT_PTR stmt);

  bool Is_empty() const;

  STMT_PTR Prepend(STMT_PTR pos, STMT_PTR stmt);
  STMT_PTR Prepend(STMT_PTR stmt);
  STMT_PTR Remove(STMT_PTR pos);
  STMT_PTR Append(STMT_PTR pos, STMT_PTR stmt);
  STMT_PTR Append(STMT_PTR stmt);
  STMT_PTR Erase(STMT_PTR pos);

  //! Given a statement 'stmt', return the list encloses it
  static STMT_LIST Enclosing_list(STMT_PTR stmt);

  void Print(std::ostream& os, bool rot = true, uint32_t indent = 0) const;
  void Print() const;
  std::string To_str(bool rot = true) const;

private:
  void Set_begin_stmt(STMT_ID stmt);
  void Set_end_stmt(STMT_ID stmt);

  NODE_PTR _blk_node;
};

//! @brief Container that holds codes in AIR form as its data, provides
//! manipulation APIs like create, delete, find etc., can be an input
//! (and output if not changed in place) of lowering or optimzation
class CONTAINER {
  friend class FUNC_SCOPE;

public:
  FUNC_SCOPE* Parent_func_scope() const { return _func; }
  GLOB_SCOPE* Glob_scope() const { return _glob; }
  STMT_LIST   Stmt_list() const;
  STMT_PTR    Entry_stmt() const;
  NODE_PTR    Entry_node() const;
  NODE_PTR    Node(NODE_ID node) const;
  STMT_PTR    Stmt(STMT_ID stmt) const;

  NODE_PTR New_void(const SPOS& spos);
  //! Function entry node
  STMT_PTR New_func_entry(const SPOS& spos);
  //! Formal parameter of function entry
  NODE_PTR New_idname(CONST_ADDR_DATUM_PTR datum, const SPOS& spos);
  NODE_PTR New_stmt_block(const SPOS& spos);
  STMT_PTR New_if_then_else(CONST_NODE_PTR cond, NODE_PTR then_b,
                            NODE_PTR else_b, const SPOS& spos);
  STMT_PTR New_do_loop(CONST_ADDR_DATUM_PTR iv, CONST_NODE_PTR init,
                       NODE_PTR comp, NODE_PTR incr, NODE_PTR body,
                       const SPOS& spos);
  STMT_PTR New_while_loop(OPCODE op, CONST_NODE_PTR comp, NODE_PTR body,
                          const SPOS& spos);
  NODE_PTR New_una_arith(OPCODE op, NODE_PTR val, const SPOS& spos);
  NODE_PTR New_bin_arith(OPCODE op, NODE_PTR left, NODE_PTR right,
                         const SPOS& spos);
  NODE_PTR New_tern_arith(OPCODE op, NODE_PTR node1, NODE_PTR node2,
                          NODE_PTR node3, const SPOS& spos);
  //! Load value of a symbol
  NODE_PTR New_ld(CONST_ADDR_DATUM_PTR datum, const SPOS& spos);
  //! Load value of a symbol to rtype
  NODE_PTR New_ld(CONST_ADDR_DATUM_PTR datum, CONST_TYPE_PTR rtype,
                  const SPOS& spos);
  //! Load value from a pointer
  NODE_PTR New_ild(CONST_NODE_PTR ptr, const SPOS& spos);
  //! Load value from a pointer to rtype
  NODE_PTR New_ild(CONST_NODE_PTR ptr, CONST_TYPE_PTR rtype, const SPOS& spos);
  //! Load field of a structure symbol
  NODE_PTR New_ldf(CONST_ADDR_DATUM_PTR datum, CONST_FIELD_PTR fld,
                   const SPOS& spos);
  //! Load field of a structure symbol to rtype
  NODE_PTR New_ldf(CONST_ADDR_DATUM_PTR datum, CONST_FIELD_PTR fld,
                   CONST_TYPE_PTR rtype, const SPOS& spos);
  //! Load value offset to a symbol with type
  NODE_PTR New_ldo(CONST_ADDR_DATUM_PTR datum, CONST_TYPE_PTR type,
                   int64_t ofst, const SPOS& spos);
  //! Load value offset to a symbol with atype to rtype
  NODE_PTR New_ldo(CONST_ADDR_DATUM_PTR datum, CONST_TYPE_PTR atype,
                   CONST_TYPE_PTR rtype, int64_t ofst, const SPOS& spos);
  //! Load value of a preg
  NODE_PTR New_ldp(CONST_PREG_PTR preg, const SPOS& spos);
  //! Load value of a structure field of a preg
  NODE_PTR New_ldpf(CONST_PREG_PTR preg, CONST_FIELD_PTR fld, const SPOS& spos);
  //! Load constant value
  NODE_PTR New_ldc(CONST_CONSTANT_PTR cst, const SPOS& spos);
  //! Load integer constant value
  NODE_PTR New_intconst(CONST_TYPE_PTR rtype, uint64_t val, const SPOS& spos);
  //! Load integer constatn value
  NODE_PTR New_intconst(PRIMITIVE_TYPE rtype, uint64_t val, const SPOS& spos);
  //! New 0 for non-integer type
  NODE_PTR New_zero(CONST_TYPE_PTR rtype, const SPOS& spos);
  //! New 1 for non-integer type
  NODE_PTR New_one(CONST_TYPE_PTR rtype, const SPOS& spos);
  //! Load address of a symbol
  NODE_PTR New_lda(CONST_ADDR_DATUM_PTR datum, POINTER_KIND ptr_kind,
                   const SPOS& spos);
  //! Load address of a constant, only for array constant now
  NODE_PTR New_ldca(CONST_CONSTANT_PTR cst, POINTER_KIND ptr_kind,
                    const SPOS& spos);
  //! Store a value to a symbol
  STMT_PTR New_st(NODE_PTR val, CONST_ADDR_DATUM_PTR datum, const SPOS& spos);
  //! Store a value to an address
  STMT_PTR New_ist(CONST_NODE_PTR addr, CONST_NODE_PTR val, const SPOS& spos);
  //! Store a value to a field of a structure symbol
  STMT_PTR New_stf(NODE_PTR val, CONST_ADDR_DATUM_PTR datum,
                   CONST_FIELD_PTR fld, const SPOS& spos);
  //! Store a value to an address offset to a symbol
  STMT_PTR New_sto(NODE_PTR val, CONST_ADDR_DATUM_PTR datum, int64_t ofst,
                   CONST_TYPE_PTR type, const SPOS& spos);
  //! Store a value to a preg
  STMT_PTR New_stp(NODE_PTR val, CONST_PREG_PTR preg, const SPOS& spos);
  //! Store a value to a structure field of a preg
  STMT_PTR New_stpf(NODE_PTR val, CONST_PREG_PTR preg, CONST_FIELD_PTR fld,
                    const SPOS& spos);
  STMT_PTR New_entry(CONST_ENTRY_PTR entry, const SPOS& spos);
  STMT_PTR New_ret(const SPOS& spos);
  STMT_PTR New_retv(CONST_NODE_PTR retv, const SPOS& spos);
  //! New statement of a direct call
  STMT_PTR New_call(CONST_ENTRY_PTR entry, CONST_PREG_PTR retv,
                    uint32_t num_args, const SPOS& spos);
  void     New_arg(STMT_PTR call, uint32_t num, CONST_NODE_PTR arg);
  STMT_PTR New_begin_region(OPCODE op, CONST_REGION_INFO_PTR region,
                            const SPOS& spos);
  STMT_PTR New_end_region(OPCODE op, const SPOS& spos);
  STMT_PTR New_pragma(const SPOS& spos);

  //! @brief Clone node with validate_op to support VALIDATE
  NODE_PTR New_validate_node(CONST_NODE_PTR node, OPCODE validate_op);
  STMT_PTR New_validate_stmt(CONST_NODE_PTR val, CONST_NODE_PTR ref_val,
                             CONST_NODE_PTR len, CONST_NODE_PTR epsilon,
                             const SPOS& spos);

  //! @brief Support dump opcodes
  STMT_PTR New_dump_var(const char* msg, CONST_NODE_PTR val, uint64_t len,
                        const SPOS& spos);

  //! @brief Support timing opcodes
  STMT_PTR New_tm_start(CONST_CONSTANT_PTR msg, const SPOS& spos);
  STMT_PTR New_tm_taken(CONST_CONSTANT_PTR msg, const SPOS& spos);

  //! @brief New array element node indexed by num_dim dimensions
  //! @param base base address of the array
  //! @param num_dim number of dimension of the array being indexed
  NODE_PTR New_array(CONST_NODE_PTR base, uint32_t num_dim, const SPOS& spos);

  //! Set dim's dimension to be indexed by idx, both start from zero
  void Set_array_idx(NODE_PTR array, uint32_t dim, CONST_NODE_PTR idx);

  //! New array node with constant indexes
  NODE_PTR New_array(CONST_NODE_PTR base, std::vector<uint32_t>& idx,
                     const SPOS& spos);

  //! @brief New expression node according to its number of fields and
  //! number of children from meta info, and number of extra child
  //! (when PROP_EX_CHILD is set)
  //! @param op OPCODE of the expression
  //! @param rtype result type of the expression
  //! @param spos Source position of the expression
  //! @param num_ex Number of extra child
  NODE_PTR New_cust_node(OPCODE op, CONST_TYPE_PTR rtype, const SPOS& spos,
                         uint32_t num_ex = 0);

  /**
   * @brief New statement node according to its
   *  number of fields and number of children from meta info,
   *  and number of extra child (when PROP_EX_CHILD is set,
   *  i.e. number of arguments of a call)
   *
   * @param op OPCODE of the statement
   * @param spos Source position of the statement
   * @param num_ex Number of extra child
   * @return STMT_PTR representing the statement
   */
  STMT_PTR New_cust_stmt(OPCODE op, const SPOS& spos, uint32_t num_ex = 0);

  /**
   * @brief Attach an extra child node to a statement (PROP_EX_CHILD is set)
   *
   * @param stmt the statement node to add child to
   * @param num the num'th child of the statement to add child to
   * @param chld the child node to add
   */
  void Set_cust_stmt_ex_chld(STMT_PTR stmt, uint32_t num, CONST_NODE_PTR chld);

  /**
   * @brief Verify a customized node
   *
   * @param node the node to verify
   */
  bool Verify_cust_node(CONST_NODE_PTR node);

  //! Verify IRs in container
  bool Verify() const;
  //! Verify a statement
  bool Verify_stmt(STMT_PTR stmt) const;
  //! Verify a node
  bool Verify_node(NODE_PTR node) const;

  /**
   * @brief Make a clone node of the argument 'orig'
   *
   */
  NODE_PTR Clone_node(CONST_NODE_PTR orig);

  /**
   * @brief Make a clone node of statement block
   *
   */
  NODE_PTR Clone_stmt_blk(CONST_NODE_PTR orig);

  /**
   * @brief Make a clone of expression tree rooted at 'orig'
   *
   */
  NODE_PTR Clone_node_tree(CONST_NODE_PTR orig);

  /**
   * @brief Make a clone of the argument 'orig', children are not set in the
   * cloned statement
   *
   */
  STMT_PTR Clone_stmt(CONST_STMT_PTR orig);

  /**
   * @brief Make a clone of the statement and the whole tree
   *
   */
  STMT_PTR Clone_stmt_tree(CONST_STMT_PTR orig);

  CODE_ARENA* Code_arena() const { return _code_arena; }

  void Print(std::ostream& os, bool rot = true, uint32_t indent = 0) const;
  void Print() const;
  std::string To_str(bool rot = true) const;

private:
  static CONTAINER* New(FUNC_SCOPE* func, bool open = true);

  CONTAINER(FUNC_SCOPE* func, bool open);

  NODE_DATA_PTR New_node(OPCODE op, uint32_t num = 0);
  STMT_DATA_PTR New_stmt(OPCODE op, uint32_t num = 0);

  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type);
  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type, uint32_t num);
  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid);
  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2);
  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2, NODE_ID nid3);
  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2, NODE_ID nid3, NODE_ID nid4);
  NODE_PTR New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid,
                    uint32_t err, uint32_t dim);

  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, uint32_t num = 0);
  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid, uint32_t num = 0);
  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2);
  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3);
  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3, NODE_ID nid4);
  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3, NODE_ID nid4, NODE_ID nid5);
  STMT_PTR New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3, NODE_ID nid4, NODE_ID nid5, NODE_ID nid6);

  CODE_ARENA* _code_arena;
  FUNC_SCOPE* _func;
  GLOB_SCOPE* _glob;
};

}  // namespace base
}  // namespace air

#endif
