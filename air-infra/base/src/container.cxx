//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"

#include <iostream>
#include <sstream>

#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/core/opcode.h"

namespace air {
namespace base {

//=============================================================================
// class STMT_LIST member functions
//=============================================================================

STMT_LIST::STMT_LIST(NODE_PTR blk) {
  AIR_ASSERT(blk->Is_block());
  _blk_node = blk;
}

CONTAINER* STMT_LIST::Container() const { return _blk_node->Container(); }

STMT_ID
STMT_LIST::Begin_stmt_id() const { return _blk_node->Begin_stmt_id(); }

STMT_PTR
STMT_LIST::Begin_stmt() const { return _blk_node->Begin_stmt(); }

STMT_ID
STMT_LIST::End_stmt_id() const { return _blk_node->End_stmt_id(); }

STMT_PTR
STMT_LIST::End_stmt() const { return _blk_node->End_stmt(); }

STMT_ID
STMT_LIST::Last_stmt_id() const { return End_stmt()->Prev_id(); }

STMT_PTR
STMT_LIST::Last_stmt() const { return End_stmt()->Prev(); }

void STMT_LIST::Set_begin_stmt(STMT_ID stmt) {
  _blk_node->Set_begin_stmt(stmt);
}

void STMT_LIST::Set_begin_stmt(STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  _blk_node->Set_begin_stmt(stmt);
  stmt->Set_prev(Null_ptr);
}

void STMT_LIST::Set_end_stmt(STMT_ID stmt) { _blk_node->Set_end_stmt(stmt); }

void STMT_LIST::Set_end_stmt(STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  _blk_node->Set_end_stmt(stmt);
  stmt->Set_next(Null_ptr);
}

bool STMT_LIST::Is_empty() const { return _blk_node->Is_empty_blk(); }

STMT_PTR
STMT_LIST::Prepend(STMT_PTR pos, STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  AIR_ASSERT(Container() == pos->Container());
  AIR_ASSERT(pos->Parent_node_id() == _blk_node->Id());
  stmt->Set_next(pos);
  stmt->Set_parent_node(pos->Parent_node_id());

  if (pos->Id() == Begin_stmt_id()) {
    pos->Set_prev(stmt);
    Set_begin_stmt(stmt);
  } else {
    STMT_PTR prev = pos->Prev();
    stmt->Set_prev(prev);
    prev->Set_next(stmt);
    pos->Set_prev(stmt);
  }
  return stmt;
}

STMT_PTR
STMT_LIST::Prepend(STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  stmt->Set_parent_node(Begin_stmt()->Parent_node_id());
  return Prepend(Begin_stmt(), stmt);
}

STMT_PTR
STMT_LIST::Prepend(STMT_LIST stmt_list) {
  NODE_PTR blk = stmt_list.Block_node();
  AIR_ASSERT(blk != Null_ptr);
  AIR_ASSERT(Container() == blk->Container());
  if (blk->Begin_stmt_id() == blk->End_stmt_id()) return Begin_stmt();
  // Set owner block
  STMT_PTR blk_begin = blk->Begin_stmt();
  STMT_PTR blk_end   = blk->End_stmt();
  STMT_PTR stmt      = blk_begin;
  while (stmt != blk_end) {
    stmt->Set_parent_node(Block_node());
    stmt = stmt->Next();
  }
  // Prepend list
  Begin_stmt()->Set_prev(blk_end->Prev_id());
  blk_end->Prev()->Set_next(Begin_stmt_id());
  Set_begin_stmt(blk_begin);
  // Clean up source block
  blk->Set_begin_stmt(blk->End_stmt_id());
  blk->Begin_stmt()->Set_next(Null_id);
  blk->End_stmt()->Set_prev(Null_id);
  return blk_begin;
}

STMT_PTR
STMT_LIST::Append(STMT_PTR pos, STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  AIR_ASSERT(Container() == pos->Container());
  AIR_ASSERT(pos->Parent_node_id() == _blk_node->Id());
  if ((pos->Id() == End_stmt_id()) ||
      (pos->Opcode() == core::OPC_END_STMT_LIST)) {
    Prepend(pos, stmt);
  }
  STMT_PTR next = pos->Next();
  pos->Set_next(stmt);
  stmt->Set_prev(pos);
  stmt->Set_next(next);
  next->Set_prev(stmt);
  stmt->Set_parent_node(pos->Parent_node_id());
  return stmt;
}

STMT_PTR
STMT_LIST::Append(STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  stmt->Set_parent_node(End_stmt()->Parent_node_id());
  return Prepend(End_stmt(), stmt);
}

STMT_PTR
STMT_LIST::Append(STMT_LIST stmt_list) {
  NODE_PTR blk = stmt_list.Block_node();
  AIR_ASSERT(blk != Null_ptr);
  AIR_ASSERT(Container() == blk->Container());
  if (blk->Begin_stmt_id() == blk->End_stmt_id()) return Begin_stmt();
  // Set owner block
  STMT_PTR blk_begin = blk->Begin_stmt();
  STMT_PTR blk_end   = blk->End_stmt();
  STMT_PTR stmt      = blk_begin;
  while (stmt != blk_end) {
    stmt->Set_parent_node(Block_node());
    stmt = stmt->Next();
  }
  // Append list
  if (Begin_stmt_id() == End_stmt_id()) {
    Set_begin_stmt(blk_begin);
  } else {
    End_stmt()->Prev()->Set_next(blk_begin);
    blk_begin->Set_prev(End_stmt()->Prev_id());
  }
  End_stmt()->Set_prev(blk_end->Prev_id());
  blk_end->Prev()->Set_next(End_stmt_id());
  // Clean up source block
  blk->Set_begin_stmt(blk->End_stmt_id());
  blk->Begin_stmt()->Set_next(Null_id);
  blk->End_stmt()->Set_prev(Null_id);
  return Begin_stmt();
}

STMT_PTR
STMT_LIST::Remove(STMT_PTR pos) {
  AIR_ASSERT(Container() == pos->Container());
  AIR_ASSERT(pos->Parent_node_id() == _blk_node->Id());
  AIR_ASSERT(pos->Id() != End_stmt_id());
  STMT_PTR next = pos->Next();

  if (pos->Id() == Begin_stmt_id()) {
    Set_begin_stmt(next);
    next->Set_prev(Null_id);
  } else {
    STMT_PTR prev = pos->Prev();
    next->Set_prev(prev);
    prev->Set_next(next);
  }
  pos->Set_prev(Null_id);
  pos->Set_next(Null_id);
  pos->Set_parent_node(Null_id);
  return next;
}

STMT_LIST
STMT_LIST::Enclosing_list(STMT_PTR stmt) {
  AIR_ASSERT(stmt->Id() != Null_id);
  AIR_ASSERT(stmt->Has_parent_node());
  return STMT_LIST(stmt->Parent_node());
}

void STMT_LIST::Print(std::ostream& os, bool rot, uint32_t indent) const {
  for (CONST_STMT_PTR stmt = Begin_stmt(); stmt != End_stmt();
       stmt                = stmt->Next()) {
    stmt->Print(os, rot, indent);
  }
}

void STMT_LIST::Print() const { Print(std::cout, true, 0); }

std::string STMT_LIST::To_str(bool rot) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, rot);
  return buf.str();
}

//=============================================================================
// class CONTAINER member functions
//=============================================================================

CONTAINER::CONTAINER(FUNC_SCOPE* func, bool open) : _func(func) {
  _glob       = &_func->Glob_scope();
  _mem_pool   = new ARENA_ALLOCATOR;
  _code_arena = new CODE_ARENA(_mem_pool, AK_CODE, "code_container", open);
}

void CONTAINER::Delete() {
  if (_code_arena != nullptr) {
    delete _code_arena;
    _code_arena = nullptr;
  }
  if (_mem_pool != nullptr) {
    delete _mem_pool;
    _mem_pool = nullptr;
  }
}

CONTAINER::~CONTAINER() { Delete(); }

CONTAINER* CONTAINER::New(FUNC_SCOPE* func, bool open) {
  CONTAINER* ret = (CONTAINER*)func->Mem_pool().Allocate(sizeof(CONTAINER));
  ::new (ret) CONTAINER(func, open);
  return ret;
}

STMT_PTR
CONTAINER::Entry_stmt() const {
  AIR_ASSERT(_func->Owning_func()->Is_defined());
  return Stmt(STMT_ID(_func->Entry_stmt_id()));
}

NODE_PTR
CONTAINER::Entry_node() const { return Entry_stmt()->Node(); }

STMT_LIST CONTAINER::Stmt_list() const {
  NODE_PTR func_entry_node = Entry_stmt()->Node();
  NODE_PTR blk_node = func_entry_node->Child(func_entry_node->Num_child() - 1);
  return STMT_LIST(blk_node);
}

NODE_PTR
CONTAINER::Node(NODE_ID id) const {
  return NODE_PTR(NODE(this, _code_arena->Find(id)));
}

STMT_PTR
CONTAINER::Stmt(STMT_ID id) const {
  unsigned long stmt_addr =
      (unsigned long)_code_arena->Find(id.Value()) - OFFSETOF(STMT_DATA, _data);

  return STMT_PTR(STMT(this, PTR_FROM_DATA<STMT_DATA>(
                                 reinterpret_cast<STMT_DATA*>(stmt_addr), id)));
}

STMT_PTR
CONTAINER::New_func_entry(const SPOS& spos) {
  FUNC_PTR func_sym = _func->Owning_func();
  // Set up entry statement
  ENTRY_PTR          entry    = func_sym->Entry_point();
  SIGNATURE_TYPE_PTR func_sig = entry->Type()->Cast_to_sig();
  uint32_t           num_args = func_sig->Num_param();
  STMT_PTR stmt = New_stmt(core::OPC_FUNC_ENTRY, spos, NODE_ID(), num_args);
  NODE_PTR node = stmt->Node();
  node->Set_entry(entry->Id());
  node->Set_num_arg(num_args);
  // Set up formal parameters and idname nodes
  PARAM_ITER iter  = func_sig->Begin_param();
  PARAM_ITER end   = func_sig->End_param();
  uint32_t   count = 0;
  for (; iter != end; ++iter) {
    PARAM_PTR param = (*iter);
    if (param->Is_ret()) continue;
    ADDR_DATUM_PTR formal =
        _func->New_formal(param->Type()->Id(), param->Name()->Id(), spos);
    NODE_PTR formal_node = New_idname(formal, spos);
    node->Set_child(count, formal_node->Id());
    count++;
  }
  AIR_ASSERT(num_args == count);
  _func->Set_entry_stmt(stmt->Id());
  // Set up block node for statements of the function
  NODE_PTR blk_node = New_stmt_block(spos);
  blk_node->Set_parent_stmt(stmt);
  node->Set_child(num_args, blk_node->Id());
  return stmt;
}

STMT_PTR CONTAINER::New_func_entry(const SPOS& spos, uint32_t formal_cnt) {
  FUNC_PTR func_sym = _func->Owning_func();
  // Set up entry statement
  ENTRY_PTR entry = func_sym->Entry_point();
  STMT_PTR  stmt  = New_stmt(core::OPC_FUNC_ENTRY, spos, NODE_ID(), formal_cnt);
  NODE_PTR  node  = stmt->Node();
  node->Set_entry(entry->Id());
  node->Set_num_arg(formal_cnt);
  _func->Set_entry_stmt(stmt->Id());

  // Set up block node for statements of the function
  NODE_PTR blk_node = New_stmt_block(spos);
  blk_node->Set_parent_stmt(stmt);
  node->Set_child(formal_cnt, blk_node->Id());
  return stmt;
}

NODE_PTR
CONTAINER::New_idname(CONST_ADDR_DATUM_PTR datum, const SPOS& spos) {
  AIR_ASSERT(datum != Null_ptr);
  NODE_PTR node = New_node(core::OPC_IDNAME, spos, datum->Type_id());
  node->Set_addr_datum(datum->Id());
  return node;
}

NODE_PTR
CONTAINER::New_stmt_block(const SPOS& spos) {
  STMT_PTR last = New_stmt(core::OPC_END_STMT_LIST, SPOS());
  NODE_PTR node = New_node(core::OPC_BLOCK, spos,
                           _glob->Prim_type(PRIMITIVE_TYPE::VOID)->Id());
  node->Set_parent_stmt(STMT_ID());
  node->Set_begin_stmt(last->Id());
  node->Set_end_stmt(last->Id());
  last->Set_parent_node(node);
  return node;
}

STMT_PTR
CONTAINER::New_if_then_else(CONST_NODE_PTR cond, NODE_PTR then_b,
                            NODE_PTR else_b, const SPOS& spos) {
  AIR_ASSERT((cond != Null_ptr) && (then_b != Null_ptr) &&
             (else_b != Null_ptr));
  AIR_ASSERT(this == cond->Container());
  AIR_ASSERT(this == then_b->Container());
  AIR_ASSERT(this == else_b->Container());
  AIR_ASSERT(cond->Is_relational_op());
  AIR_ASSERT(then_b->Is_block());
  AIR_ASSERT(else_b->Is_block());
  STMT_PTR stmt =
      New_stmt(core::OPC_IF, spos, cond->Id(), then_b->Id(), else_b->Id());
  then_b->Set_parent_stmt(stmt);
  else_b->Set_parent_stmt(stmt);
  return stmt;
}

STMT_PTR
CONTAINER::New_do_loop(CONST_ADDR_DATUM_PTR iv, CONST_NODE_PTR init,
                       NODE_PTR comp, NODE_PTR incr, NODE_PTR body,
                       const SPOS& spos) {
  AIR_ASSERT((iv != Null_ptr) && (init != Null_ptr) && (comp != Null_ptr) &&
             (incr != Null_ptr) && (body != Null_ptr));
  AIR_ASSERT(this == init->Container());
  AIR_ASSERT(this == comp->Container());
  AIR_ASSERT(this == incr->Container());
  AIR_ASSERT(this == body->Container());
  AIR_ASSERT(comp->Is_relational_op());
  STMT_PTR stmt = New_stmt(core::OPC_DO_LOOP, spos, init->Id(), comp->Id(),
                           incr->Id(), body->Id());
  body->Set_parent_stmt(stmt);
  NODE_PTR node = stmt->Node();
  node->Set_iv(iv->Id());
  return stmt;
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, uint32_t num) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op, num);
  new (stmt_ptr) STMT_DATA(op, spos);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid, uint32_t num) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op, num);
  new (stmt_ptr) STMT_DATA(op, spos, nid);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op);
  new (stmt_ptr) STMT_DATA(op, spos, nid1, nid2);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op);
  new (stmt_ptr) STMT_DATA(op, spos, nid1, nid2, nid3);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3, NODE_ID nid4) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op);
  new (stmt_ptr) STMT_DATA(op, spos, nid1, nid2, nid3, nid4);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3, NODE_ID nid4, NODE_ID nid5) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op);
  new (stmt_ptr) STMT_DATA(op, spos, nid1, nid2, nid3, nid4, nid5);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_PTR
CONTAINER::New_stmt(OPCODE op, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
                    NODE_ID nid3, NODE_ID nid4, NODE_ID nid5, NODE_ID nid6) {
  STMT_DATA_PTR stmt_ptr = New_stmt(op);
  new (stmt_ptr) STMT_DATA(op, spos, nid1, nid2, nid3, nid4, nid5, nid6);
  return STMT_PTR(STMT(this, stmt_ptr));
}

STMT_DATA_PTR
CONTAINER::New_stmt(OPCODE op, uint32_t num) {
  AIR_ASSERT(META_INFO::Valid_opcode(op));
  size_t bytes = NODE_DATA::Size(op, num);

  PTR_FROM_DATA<void> stmt_ptr =
      _code_arena->Malloc((OFFSETOF(STMT_DATA, _data) + bytes) >> 2);
  _code_arena->Adjust_addr(stmt_ptr.Id().Value(), OFFSETOF(STMT_DATA, _data));
  return Static_cast<STMT_DATA_PTR>(stmt_ptr);
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, uint32_t num) {
  NODE_DATA_PTR node_ptr = New_node(op, num);
  new (node_ptr) NODE_DATA(op, spos, type);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_DATA_PTR
CONTAINER::New_node(OPCODE op, uint32_t num) {
  AIR_ASSERT(META_INFO::Valid_opcode(op));
  size_t bytes = NODE_DATA::Size(op, num);
  return Static_cast<NODE_DATA_PTR>(_code_arena->Malloc(bytes >> 2));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type, nid);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type, nid1, nid2);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2, NODE_ID nid3) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type, nid1, nid2, nid3);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2, NODE_ID nid3, NODE_ID nid4) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type, nid1, nid2, nid3, nid4);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2, NODE_ID nid3, NODE_ID nid4, NODE_ID nid5) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type, nid1, nid2, nid3, nid4, nid5);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_node(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                    NODE_ID nid2, NODE_ID nid3, NODE_ID nid4, NODE_ID nid5,
                    NODE_ID nid6) {
  NODE_DATA_PTR node_ptr = New_node(op);
  new (node_ptr) NODE_DATA(op, spos, type, nid1, nid2, nid3, nid4, nid5, nid6);
  return NODE_PTR(NODE(this, node_ptr));
}

NODE_PTR
CONTAINER::New_una_arith(OPCODE op, CONST_TYPE_PTR rtype, NODE_PTR val,
                         const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (val != Null_ptr));
  AIR_ASSERT(this == val->Container());
  NODE_PTR node_ptr = New_node(op, spos, rtype->Id(), val->Id());
  return node_ptr;
}

NODE_PTR
CONTAINER::New_bin_arith(OPCODE op, CONST_TYPE_PTR rtype, NODE_PTR left,
                         NODE_PTR right, const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (left != Null_ptr) && (right != Null_ptr));
  AIR_ASSERT(this == left->Container());
  AIR_ASSERT(this == right->Container());
  NODE_PTR node = New_node(op, spos, rtype->Id(), left->Id(), right->Id());
  return node;
}

NODE_PTR CONTAINER::New_tern_arith(OPCODE op, CONST_TYPE_PTR rtype,
                                   NODE_PTR node1, NODE_PTR node2,
                                   NODE_PTR node3, const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (node1 != Null_ptr) &&
             (node2 != Null_ptr) && (node3 != Null_ptr));
  AIR_ASSERT(this == node1->Container());
  AIR_ASSERT(this == node2->Container());
  AIR_ASSERT(this == node3->Container());
  NODE_PTR node =
      New_node(op, spos, rtype->Id(), node1->Id(), node2->Id(), node3->Id());
  return node;
}

NODE_PTR CONTAINER::New_quad_arith(OPCODE op, CONST_TYPE_PTR rtype,
                                   NODE_PTR node1, NODE_PTR node2,
                                   NODE_PTR node3, NODE_PTR node4,
                                   const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (node1 != Null_ptr) &&
             (node2 != Null_ptr) && (node3 != Null_ptr) && (node4 != Null_ptr));
  AIR_ASSERT(this == node1->Container());
  AIR_ASSERT(this == node2->Container());
  AIR_ASSERT(this == node3->Container());
  AIR_ASSERT(this == node4->Container());
  NODE_PTR node = New_node(op, spos, rtype->Id(), node1->Id(), node2->Id(),
                           node3->Id(), node4->Id());
  return node;
}

NODE_PTR CONTAINER::New_quint_arith(OPCODE op, CONST_TYPE_PTR rtype,
                                    NODE_PTR node1, NODE_PTR node2,
                                    NODE_PTR node3, NODE_PTR node4,
                                    NODE_PTR node5, const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (node1 != Null_ptr) &&
             (node2 != Null_ptr) && (node3 != Null_ptr) &&
             (node4 != Null_ptr) && (node5 != Null_ptr));
  AIR_ASSERT(this == node1->Container());
  AIR_ASSERT(this == node2->Container());
  AIR_ASSERT(this == node3->Container());
  AIR_ASSERT(this == node4->Container());
  AIR_ASSERT(this == node5->Container());
  NODE_PTR node = New_node(op, spos, rtype->Id(), node1->Id(), node2->Id(),
                           node3->Id(), node4->Id(), node5->Id());
  return node;
}

NODE_PTR CONTAINER::New_sext_arith(OPCODE op, CONST_TYPE_PTR rtype,
                                   NODE_PTR node1, NODE_PTR node2,
                                   NODE_PTR node3, NODE_PTR node4,
                                   NODE_PTR node5, NODE_PTR node6,
                                   const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (node1 != Null_ptr) &&
             (node2 != Null_ptr) && (node3 != Null_ptr) &&
             (node4 != Null_ptr) && (node5 != Null_ptr) && (node6 != Null_ptr));
  AIR_ASSERT(this == node1->Container());
  AIR_ASSERT(this == node2->Container());
  AIR_ASSERT(this == node3->Container());
  AIR_ASSERT(this == node4->Container());
  AIR_ASSERT(this == node5->Container());
  AIR_ASSERT(this == node6->Container());
  NODE_PTR node = New_node(op, spos, rtype->Id(), node1->Id(), node2->Id(),
                           node3->Id(), node4->Id(), node5->Id(), node6->Id());
  return node;
}

NODE_PTR CONTAINER::New_invalid(OPCODE op, const SPOS& spos) {
  NODE_PTR node =
      New_node(op, spos, _glob->Prim_type(PRIMITIVE_TYPE::VOID)->Id());
  return node;
}

NODE_PTR
CONTAINER::New_ld(CONST_ADDR_DATUM_PTR datum, const SPOS& spos) {
  AIR_ASSERT(datum != Null_ptr);
  return New_ld(datum, datum->Type(), spos);
}

NODE_PTR
CONTAINER::New_ld(CONST_ADDR_DATUM_PTR datum, CONST_TYPE_PTR rtype,
                  const SPOS& spos) {
  AIR_ASSERT((datum != Null_ptr) && (rtype != Null_ptr));
  NODE_PTR node  = New_node(core::OPC_LD, spos, rtype->Id());
  TYPE_PTR atype = datum->Type();
  AIR_ASSERT((atype->Is_int() && rtype->Is_int()) || (atype == rtype));
  node->Set_addr_datum(datum);
  node->Set_access_type(atype->Id());
  return node;
}

NODE_PTR
CONTAINER::New_ild(CONST_NODE_PTR ptr, const SPOS& spos) {
  AIR_ASSERT((ptr != Null_ptr) && (ptr->Rtype()->Is_ptr()));
  return New_ild(ptr, ptr->Rtype()->Cast_to_ptr()->Domain_type()->Base_type(),
                 spos);
}

NODE_PTR
CONTAINER::New_ild(CONST_NODE_PTR ptr, CONST_TYPE_PTR rtype, const SPOS& spos) {
  AIR_ASSERT((ptr != Null_ptr) && (rtype != Null_ptr));
  AIR_ASSERT(ptr->Rtype()->Is_ptr());
  CONST_POINTER_TYPE_PTR ptr_type = ptr->Rtype()->Cast_to_ptr();
  TYPE_PTR               atype    = ptr_type->Domain_type()->Base_type();
  AIR_ASSERT((atype->Is_int() && rtype->Is_int()) || (atype == rtype));
  NODE_PTR node = New_node(core::OPC_ILD, spos, rtype->Id(), ptr->Id());
  node->Set_access_type(atype->Id());
  return node;
}

NODE_PTR
CONTAINER::New_ldf(CONST_ADDR_DATUM_PTR datum, CONST_FIELD_PTR fld,
                   const SPOS& spos) {
  AIR_ASSERT((datum != Null_ptr) && (fld != Null_ptr));
  return New_ldf(datum, fld, fld->Type()->Base_type(), spos);
}

NODE_PTR
CONTAINER::New_ldf(CONST_ADDR_DATUM_PTR datum, CONST_FIELD_PTR fld,
                   CONST_TYPE_PTR rtype, const SPOS& spos) {
  AIR_ASSERT((datum != Null_ptr) && (fld != Null_ptr) && (rtype != Null_ptr));
  TYPE_PTR atype = fld->Type()->Base_type();
  AIR_ASSERT((atype->Is_int() && rtype->Is_int()) || (atype == rtype));
  NODE_PTR node = New_node(core::OPC_LDF, spos, rtype->Id());
  node->Set_addr_datum(datum);
  node->Set_access_type(atype->Id());
  node->Set_field(fld);
  return node;
}

NODE_PTR
CONTAINER::New_ldo(CONST_ADDR_DATUM_PTR datum, CONST_TYPE_PTR type,
                   int64_t ofst, const SPOS& spos) {
  AIR_ASSERT((datum != Null_ptr) && (type != Null_ptr));
  return New_ldo(datum, type, type, ofst, spos);
}

NODE_PTR
CONTAINER::New_ldo(CONST_ADDR_DATUM_PTR datum, CONST_TYPE_PTR atype,
                   CONST_TYPE_PTR rtype, int64_t ofst, const SPOS& spos) {
  AIR_ASSERT((datum != Null_ptr) && (atype != Null_ptr) && (rtype != Null_ptr));
  AIR_ASSERT((atype->Is_int() && rtype->Is_int()) || (atype == rtype));
  NODE_PTR node = New_node(core::OPC_LDO, spos, rtype->Id());
  node->Set_addr_datum(datum);
  node->Set_access_type(atype->Id());
  node->Set_ofst(ofst);
  return node;
}

NODE_PTR
CONTAINER::New_ldp(CONST_PREG_PTR preg, const SPOS& spos) {
  AIR_ASSERT(preg != Null_ptr);
  NODE_PTR node = New_node(core::OPC_LDP, spos, preg->Type_id());
  node->Set_preg(preg);
  node->Set_access_type(preg->Type_id());
  return node;
}

NODE_PTR
CONTAINER::New_ldpf(CONST_PREG_PTR preg, CONST_FIELD_PTR fld,
                    const SPOS& spos) {
  AIR_ASSERT((preg != Null_ptr) && (fld != Null_ptr));
  AIR_ASSERT(preg->Type()->Is_record());
  TYPE_ID  rtype = fld->Type()->Base_type_id();
  NODE_PTR node  = New_node(core::OPC_LDPF, spos, rtype);
  node->Set_preg(preg);
  node->Set_access_type(rtype);
  node->Set_field(fld);
  return node;
}

NODE_PTR
CONTAINER::New_ldc(CONST_CONSTANT_PTR cst, const SPOS& spos) {
  AIR_ASSERT(cst != Null_ptr);
  NODE_PTR node = New_node(core::OPC_LDC, spos, cst->Type()->Base_type_id());
  node->Set_const(cst);
  return node;
}

NODE_PTR
CONTAINER::New_intconst(CONST_TYPE_PTR rtype, uint64_t val, const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (rtype->Is_int()));
  NODE_PTR node = New_node(core::OPC_INTCONST, spos, rtype->Id());
  node->Set_intconst(val);
  return node;
}

NODE_PTR
CONTAINER::New_intconst(PRIMITIVE_TYPE rtype, uint64_t val, const SPOS& spos) {
  TYPE_PTR rtype_ptr = _glob->Prim_type(rtype);
  return New_intconst(rtype_ptr, val, spos);
}

NODE_PTR
CONTAINER::New_zero(CONST_TYPE_PTR rtype, const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (!rtype->Is_int()));
  NODE_PTR node = New_node(core::OPC_ZERO, spos, rtype->Id());
  return node;
}

NODE_PTR
CONTAINER::New_one(CONST_TYPE_PTR rtype, const SPOS& spos) {
  AIR_ASSERT((rtype != Null_ptr) && (!rtype->Is_int()));
  NODE_PTR node = New_node(core::OPC_ONE, spos, rtype->Id());
  return node;
}

NODE_PTR
CONTAINER::New_lda(CONST_ADDR_DATUM_PTR datum, POINTER_KIND ptr_kind,
                   const SPOS& spos) {
  AIR_ASSERT(datum != Null_ptr);
  CONST_POINTER_TYPE_PTR rtype = _glob->Ptr_type(datum->Type_id(), ptr_kind);
  NODE_PTR               node  = New_node(core::OPC_LDA, spos, rtype->Id());
  node->Set_addr_datum(datum);
  return node;
}

NODE_PTR
CONTAINER::New_ldca(CONST_CONSTANT_PTR cst, POINTER_KIND ptr_kind,
                    const SPOS& spos) {
  AIR_ASSERT(cst != Null_ptr);
  CONST_POINTER_TYPE_PTR rtype =
      (cst->Type()->Is_array())
          ? _glob->Ptr_type(cst->Type()->Cast_to_arr()->Elem_type_id(),
                            ptr_kind)
          : _glob->Ptr_type(cst->Type(), ptr_kind);
  NODE_PTR node = New_node(core::OPC_LDCA, spos, rtype->Id());
  node->Set_const(cst);
  return node;
}

STMT_PTR
CONTAINER::New_st(NODE_PTR val, CONST_ADDR_DATUM_PTR datum, const SPOS& spos) {
  AIR_ASSERT((val != Null_ptr) && (datum != Null_ptr));
  AIR_ASSERT(this == val->Container());
  STMT_PTR stmt = New_stmt(core::OPC_ST, spos, val->Id());
  NODE_PTR node = stmt->Node();
  node->Set_addr_datum(datum);
  node->Set_access_type(datum->Type_id());
  return stmt;
}

STMT_PTR
CONTAINER::New_ist(CONST_NODE_PTR addr, CONST_NODE_PTR val, const SPOS& spos) {
  AIR_ASSERT((addr != Null_ptr) && (val != Null_ptr));
  AIR_ASSERT(addr->Rtype()->Is_ptr());
  STMT_PTR stmt = New_stmt(core::OPC_IST, spos, addr->Id(), val->Id());
  NODE_PTR node = stmt->Node();
  node->Set_access_type(
      addr->Rtype()->Cast_to_ptr()->Domain_type()->Base_type_id());
  return stmt;
}

STMT_PTR
CONTAINER::New_stf(NODE_PTR val, CONST_ADDR_DATUM_PTR datum,
                   CONST_FIELD_PTR fld, const SPOS& spos) {
  AIR_ASSERT((val != Null_ptr) && (datum != Null_ptr) && (fld != Null_ptr));
  AIR_ASSERT(this == val->Container());
  STMT_PTR stmt = New_stmt(core::OPC_STF, spos, val->Id());
  NODE_PTR node = stmt->Node();
  node->Set_addr_datum(datum);
  node->Set_access_type(fld->Type()->Base_type_id());
  node->Set_field(fld);
  return stmt;
}

STMT_PTR
CONTAINER::New_sto(NODE_PTR val, CONST_ADDR_DATUM_PTR datum, int64_t ofst,
                   CONST_TYPE_PTR type, const SPOS& spos) {
  AIR_ASSERT((val != Null_ptr) && (datum != Null_ptr) && (type != Null_ptr));
  AIR_ASSERT(this == val->Container());
  STMT_PTR stmt = New_stmt(core::OPC_STO, spos, val->Id());
  NODE_PTR node = stmt->Node();
  node->Set_addr_datum(datum);
  node->Set_access_type(type->Id());
  node->Set_ofst(ofst);
  return stmt;
}

STMT_PTR
CONTAINER::New_stp(NODE_PTR val, CONST_PREG_PTR preg, const SPOS& spos) {
  AIR_ASSERT((val != Null_ptr) && (preg != Null_ptr));
  AIR_ASSERT(this == val->Container());
  STMT_PTR stmt = New_stmt(core::OPC_STP, spos, val->Id());
  NODE_PTR node = stmt->Node();
  node->Set_preg(preg);
  node->Set_access_type(preg->Type_id());
  return stmt;
}

STMT_PTR
CONTAINER::New_stpf(NODE_PTR val, CONST_PREG_PTR preg, CONST_FIELD_PTR fld,
                    const SPOS& spos) {
  AIR_ASSERT((val != Null_ptr) && (preg != Null_ptr) && (fld != Null_ptr));
  AIR_ASSERT(this == val->Container());
  AIR_ASSERT(preg->Type()->Is_record());
  AIR_ASSERT(preg->Type() == fld->Owning_rec_type());
  AIR_ASSERT(val->Rtype() == fld->Type()->Base_type());
  STMT_PTR stmt = New_stmt(core::OPC_STPF, spos, val->Id());
  NODE_PTR node = stmt->Node();
  node->Set_preg(preg);
  node->Set_access_type(preg->Type_id());
  node->Set_field(fld);
  return stmt;
}

STMT_PTR
CONTAINER::New_entry(CONST_ENTRY_PTR entry, const SPOS& spos) {
  AIR_ASSERT(entry != Null_ptr);
  STMT_PTR stmt = New_stmt(core::OPC_ENTRY, spos);
  NODE_PTR node = stmt->Node();
  node->Set_entry(entry);
  return stmt;
}

STMT_PTR
CONTAINER::New_ret(const SPOS& spos) {
  STMT_PTR stmt = New_stmt(core::OPC_RET, spos);
  return stmt;
}

STMT_PTR
CONTAINER::New_retv(CONST_NODE_PTR retv, const SPOS& spos) {
  AIR_ASSERT(retv != Null_ptr);
  AIR_ASSERT(this == retv->Container());
  STMT_PTR stmt = New_stmt(core::OPC_RETV, spos, retv->Id());
  return stmt;
}

STMT_PTR
CONTAINER::New_call(CONST_ENTRY_PTR entry, CONST_PREG_PTR retv,
                    uint32_t num_args, const SPOS& spos) {
  AIR_ASSERT(entry != Null_ptr);
  AIR_ASSERT(
      retv == Null_ptr ||
      retv->Type()->Is_compatible_type(
          entry->Type()->Base_type()->Cast_to_sig()->Ret_param()->Type()));
  STMT_PTR stmt = New_stmt(core::OPC_CALL, spos, num_args);
  NODE_PTR node = stmt->Node();
  node->Set_entry(entry);
  node->Set_ret_preg(retv);
  node->Set_num_arg(num_args);
  return stmt;
}

NODE_PTR CONTAINER::New_intrn_op(const char* fname, TYPE_ID rtype,
                                 uint32_t num_args, const SPOS& spos) {
  AIR_ASSERT(fname != nullptr);
  NODE_PTR node = New_node(core::OPC_INTRN_OP, spos, rtype, num_args);
  STR_PTR  str  = Glob_scope()->New_str(fname);
  node->Set_intrn_name(str->Id());
  node->Set_num_arg(num_args);
  return node;
}

STMT_PTR CONTAINER::New_intrn_call(const char* fname, CONST_PREG_PTR retv,
                                   uint32_t num_args, const SPOS& spos) {
  AIR_ASSERT(fname != nullptr);
  STMT_PTR stmt = New_stmt(core::OPC_INTRN_CALL, spos, num_args);
  NODE_PTR node = stmt->Node();
  STR_PTR  str  = Glob_scope()->New_str(fname);
  node->Set_intrn_name(str->Id());
  node->Set_ret_preg(retv);
  node->Set_num_arg(num_args);
  return stmt;
}

void CONTAINER::New_arg(NODE_PTR call_node, uint32_t num, CONST_NODE_PTR arg) {
  AIR_ASSERT((call_node != Null_ptr) && (arg != Null_ptr));
  AIR_ASSERT(this == call_node->Container());
  AIR_ASSERT(this == arg->Container());
  call_node->Set_arg(num, arg);
}

void CONTAINER::New_arg(STMT_PTR call_stmt, uint32_t num, CONST_NODE_PTR arg) {
  NODE_PTR call_node = call_stmt->Node();
  New_arg(call_node, num, arg);
}

STMT_PTR
CONTAINER::New_pragma(uint32_t id, uint32_t arg0, uint32_t arg1,
                      const SPOS& spos) {
  STMT_PTR stmt = New_stmt(core::OPC_PRAGMA, spos);
  NODE_PTR node = stmt->Node();
  node->Set_pragma(id, arg0, arg1);
  return stmt;
}

NODE_PTR
CONTAINER::New_validate_node(CONST_NODE_PTR node, OPCODE validate_op) {
  AIR_ASSERT(node != Null_ptr);
  OPCODE op = node->Opcode();
  AIR_ASSERT(META_INFO::Op_num_child(op) ==
             META_INFO::Op_num_child(validate_op));
  AIR_ASSERT(META_INFO::Op_num_field(op) ==
             META_INFO::Op_num_field(validate_op));
  NODE_PTR clone_node = Clone_node_tree(node);
  clone_node->Set_opcode(validate_op);
  return clone_node;
}

STMT_PTR
CONTAINER::New_validate_stmt(CONST_NODE_PTR val, CONST_NODE_PTR ref_val,
                             CONST_NODE_PTR len, CONST_NODE_PTR epsilon,
                             const SPOS& spos) {
  AIR_ASSERT((val != Null_ptr) && (ref_val != Null_ptr) && (len != Null_ptr) &&
             (epsilon != Null_ptr));
  STMT_PTR stmt = New_stmt(core::OPC_VALIDATE, spos, val->Id(), ref_val->Id(),
                           len->Id(), epsilon->Id());
  return stmt;
}

STMT_PTR CONTAINER::New_dump_var(const char* msg, CONST_NODE_PTR val,
                                 uint64_t len, const SPOS& spos) {
  AIR_ASSERT((msg != nullptr) && (val != Null_ptr));
  CONSTANT_PTR msg_cst =
      Glob_scope()->New_const(CONSTANT_KIND::STR_ARRAY, msg, strlen(msg));
  NODE_PTR name    = New_ldc(msg_cst, spos);
  NODE_PTR len_cst = New_intconst(PRIMITIVE_TYPE::INT_U64, len, spos);
  STMT_PTR stmt =
      New_stmt(core::OPC_DUMP_VAR, spos, name->Id(), val->Id(), len_cst->Id());
  return stmt;
}

NODE_PTR
CONTAINER::New_array(CONST_NODE_PTR base, uint32_t num_dim, const SPOS& spos) {
  AIR_ASSERT(base != Null_ptr);
  AIR_ASSERT(num_dim > 0);
  TYPE_ID elem_type_id;
  if (base->Has_sym()) {
    TYPE_PTR sym_type = base->Addr_datum()->Type();
    AIR_ASSERT(sym_type->Is_array());
    AIR_ASSERT(sym_type->Cast_to_arr()->Dim() == num_dim);
    elem_type_id = sym_type->Cast_to_arr()->Elem_type_id();
  } else {
    AIR_ASSERT(base->Opcode() == core::OPC_LDCA);
    TYPE_PTR cst_type = base->Const()->Type();
    AIR_ASSERT(cst_type->Is_array());
    AIR_ASSERT(cst_type->Cast_to_arr()->Dim() == num_dim);
    elem_type_id = cst_type->Cast_to_arr()->Elem_type_id();
  }
  TYPE_PTR rtype =
      Glob_scope()->New_ptr_type(elem_type_id, POINTER_KIND::FLAT32);
  OPCODE        op                       = core::OPC_ARRAY;
  NODE_DATA_PTR node_data                = New_node(op, num_dim + 1);
  node_data->_comm._core._opcode         = static_cast<uint32_t>(op);
  node_data->_comm._core._num_added_chld = num_dim;
  node_data->_comm._spos                 = spos;
  node_data->_comm._rtype                = rtype->Id().Value();
  node_data->_comm._attr                 = Null_id;
  NODE_PTR node(NODE(this, node_data));
  uint32_t fixed_fn = META_INFO::Op_num_field(op);
  for (uint32_t i = 0; i < num_dim; i++) {
    node_data->_uu._fields[fixed_fn + i] = Null_prim_id;
  }
  node->Set_child(0, base->Id());
  return node;
}

void CONTAINER::Set_array_idx(NODE_PTR array, uint32_t dim,
                              CONST_NODE_PTR idx) {
  AIR_ASSERT(array != Null_ptr);
  AIR_ASSERT(array->Opcode() == core::OPC_ARRAY);
  AIR_ASSERT(array->Num_child() > dim);
  array->Set_child(dim + 1, idx->Id());
}

NODE_PTR
CONTAINER::New_array(CONST_NODE_PTR base, std::vector<uint32_t>& idx,
                     const SPOS& spos) {
  AIR_ASSERT(base != Null_ptr);
  uint32_t size  = idx.size();
  NODE_PTR node  = New_array(base, size, spos);
  TYPE_PTR itype = _glob->Prim_type(PRIMITIVE_TYPE::INT_U32);
  for (uint32_t i = 0; i < size; i++) {
    NODE_PTR inode = New_intconst(itype, idx[i], spos);
    Set_array_idx(node, i, inode);
  }
  return node;
}

STMT_PTR
CONTAINER::New_comment(const char* comment, const SPOS& spos) {
  STMT_PTR stmt = New_stmt(core::OPC_COMMENT, spos, 0);
  NODE_PTR node = stmt->Node();
  STR_PTR  str  = Glob_scope()->New_str(comment);
  node->Set_comment(str->Id());
  return stmt;
}

NODE_PTR
CONTAINER::New_cust_node(OPCODE op, CONST_TYPE_PTR rtype, const SPOS& spos,
                         uint32_t num_ex) {
  AIR_ASSERT(rtype != Null_ptr);
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::EXPR>(op));
  NODE_DATA_PTR node_ptr  = New_node(op, num_ex);
  uint32_t      num_child = META_INFO::Op_num_child(op);
  TYPE_ID       rtype_id  = TYPE_ID();
  if (rtype != Null_ptr) {
    rtype_id = rtype->Id();
  }
  switch (num_child) {
    case 0: {
      new (node_ptr) NODE_DATA(op, spos, rtype_id);
      break;
    }
    case 1: {
      new (node_ptr) NODE_DATA(op, spos, rtype_id, NODE_ID());
      break;
    }
    case 2: {
      new (node_ptr) NODE_DATA(op, spos, rtype_id, NODE_ID(), NODE_ID());
      break;
    }
    case 3: {
      new (node_ptr)
          NODE_DATA(op, spos, rtype_id, NODE_ID(), NODE_ID(), NODE_ID());
      break;
    }
    case 4: {
      new (node_ptr) NODE_DATA(op, spos, rtype_id, NODE_ID(), NODE_ID(),
                               NODE_ID(), NODE_ID());
      break;
    }
    case 5: {
      new (node_ptr) NODE_DATA(op, spos, rtype_id, NODE_ID(), NODE_ID(),
                               NODE_ID(), NODE_ID(), NODE_ID());
      break;
    }
    case 6: {
      new (node_ptr) NODE_DATA(op, spos, rtype_id, NODE_ID(), NODE_ID(),
                               NODE_ID(), NODE_ID(), NODE_ID(), NODE_ID());
      break;
    }
    default: {
      AIR_ASSERT(0);
      new (node_ptr) NODE_DATA(op, spos, rtype_id);
      break;
    }
  }
  return NODE_PTR(NODE(this, node_ptr));
}

STMT_PTR
CONTAINER::New_cust_stmt(OPCODE op, const SPOS& spos, uint32_t num_ex) {
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::STMT>(op));
  STMT_DATA_PTR stmt_ptr  = New_stmt(op, num_ex);
  uint32_t      num_child = META_INFO::Op_num_child(op);
  switch (num_child) {
    case 0: {
      new (stmt_ptr) STMT_DATA(op, spos);
      break;
    }
    case 1: {
      new (stmt_ptr) STMT_DATA(op, spos, NODE_ID());
      break;
    }
    case 2: {
      new (stmt_ptr) STMT_DATA(op, spos, NODE_ID(), NODE_ID());
      break;
    }
    case 3: {
      new (stmt_ptr) STMT_DATA(op, spos, NODE_ID(), NODE_ID(), NODE_ID());
      break;
    }
    case 4: {
      new (stmt_ptr)
          STMT_DATA(op, spos, NODE_ID(), NODE_ID(), NODE_ID(), NODE_ID());
      break;
    }
    case 5: {
      new (stmt_ptr) STMT_DATA(op, spos, NODE_ID(), NODE_ID(), NODE_ID(),
                               NODE_ID(), NODE_ID());
      break;
    }
    case 6: {
      new (stmt_ptr) STMT_DATA(op, spos, NODE_ID(), NODE_ID(), NODE_ID(),
                               NODE_ID(), NODE_ID(), NODE_ID());
      break;
    }
    default: {
      AIR_ASSERT(0);
      new (stmt_ptr) STMT_DATA(op, spos);
      break;
    }
  }
  return STMT_PTR(STMT(this, stmt_ptr));
}

void CONTAINER::Set_cust_stmt_ex_chld(STMT_PTR stmt, uint32_t num,
                                      CONST_NODE_PTR chld) {
  AIR_ASSERT((stmt != Null_ptr) && (chld != Null_ptr));
  AIR_ASSERT(this == stmt->Container());
  AIR_ASSERT(this == chld->Container());
  NODE_PTR node = stmt->Node();
  node->Set_child(META_INFO::Op_num_child(node->Opcode()) + num, chld);
}

bool CONTAINER::Verify_cust_node(CONST_NODE_PTR node) {
  if (node == Null_ptr) {
    CMPLR_DEV_WARN("Null node ptr");
    return false;
  }
  uint32_t num_chld = node->Num_child();
  uint32_t num_fld  = node->Num_fld();         // to figure out how to verify
  uint32_t num_ex   = node->Num_added_chld();  // to figure out how to verify
  for (uint32_t i = 0; i < num_chld; i++) {
    if (!Verify_node(node->Child(i))) {
      return false;
    }
  }
  if (node->Rtype()->Id().Is_null()) {
    CMPLR_DEV_WARN("Null rtype");
    return false;
  }
  return true;
}

bool CONTAINER::Verify() const {
  if (Parent_func_scope()->Entry_stmt_id() == Null_id) {
    CMPLR_DEV_WARN("Null entry stmt");
    return false;
  }
  NODE_PTR enode = Entry_node();
  if (enode == Null_ptr) {
    CMPLR_DEV_WARN("Null entry node");
    return false;
  }
  if (enode->Opcode() != core::OPC_FUNC_ENTRY) {
    CMPLR_DEV_WARN("Wrong entry node opcode");
    return false;
  }
  return Verify_stmt(Entry_stmt());
}

bool CONTAINER::Verify_stmt(STMT_PTR stmt) const {
  if (stmt == Null_ptr) {
    CMPLR_DEV_WARN("Null stmt ptr");
    return false;
  }
  if (stmt->Container() != this) {
    CMPLR_DEV_WARN("Wrong container");
    return false;
  }
  NODE_PTR node = stmt->Node();
  if (!node->Is_root()) {
    CMPLR_DEV_WARN("Not root stmt");
    return false;
  }
  return Verify_node(node);
}

bool CONTAINER::Verify_node(NODE_PTR node) const {
  if (node == Null_ptr) {
    CMPLR_DEV_WARN("Null node ptr");
    return false;
  }
  if (node->Container() != this) {
    CMPLR_DEV_WARN("Wrong container");
    return false;
  }
  OPCODE op = node->Opcode();
  // Verify OPCODE specific properties
  if (op == core::OPC_FUNC_ENTRY) {
    uint32_t num_child = node->Num_child();
    if (num_child < 1) return false;
    for (uint32_t id = 0; id < num_child - 1; ++id) {
      if (node->Child(id)->Opcode() != core::OPC_IDNAME) {
        CMPLR_DEV_WARN("Wrong opcode for entry first n-1 kid");
        return false;
      }
    }
    if (!node->Last_child()->Is_block()) {
      CMPLR_DEV_WARN("Wrong opcode for entry last kid");
      return false;
    }
    if (node->Last_child()->Parent_stmt() != node->Stmt()) {
      CMPLR_DEV_WARN("Wrong parent stmt");
      return false;
    }
  }
  if (node->Is_block()) {
    if ((node->Begin_stmt_id() == Null_id) ||
        (node->End_stmt_id() == Null_id) ||
        (node->Parent_stmt_id() == Null_id)) {
      CMPLR_DEV_WARN("Null block begin or end or parent stmt");
      return false;
    }
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      if (stmt->Parent_node_id() != node->Id()) {
        CMPLR_DEV_WARN("Wrong parent stmt");
        return false;
      }
      if (!Verify_stmt(stmt)) return false;
    }
  }
  if (node->Is_do_loop()) {
    if (node->Iv_id().Is_null()) {
      CMPLR_DEV_WARN("Null loop iv");
      return false;
    }
    if (node->Iv()->Scope_level() == 0) {
      if (&node->Iv()->Glob_scope() != Glob_scope()) {
        CMPLR_DEV_WARN("Wrong loop iv scope");
        return false;
      }
    } else {
      if (node->Iv()->Defining_func_scope() != Parent_func_scope()) {
        CMPLR_DEV_WARN("Wrong loop iv defining scope");
        return false;
      }
    }
    if (!node->Child(3)->Is_block()) {
      CMPLR_DEV_WARN("Wrong loop body kid");
      return false;
    }
    if (node->Child(3)->Parent_stmt() != node->Stmt()) {
      CMPLR_DEV_WARN("Wrong parent stmt");
      return false;
    }
  }
  if (node->Is_if()) {
    if ((!node->Child(1)->Is_block()) || (!node->Child(2)->Is_block()) ||
        (node->Child(1)->Parent_stmt() != node->Stmt()) ||
        (node->Child(2)->Parent_stmt() != node->Stmt())) {
      CMPLR_DEV_WARN("Wrong parent stmt for if kids");
      return false;
    }
  }
  // Verify common properties
  if (META_INFO::Has_prop<OPR_PROP::EXPR>(op)) {
    if (node->Rtype_id().Is_null()) {
      CMPLR_DEV_WARN("Null expr rtype");
      return false;
    }
    if (&node->Rtype()->Glob_scope() != Glob_scope()) {
      CMPLR_DEV_WARN("Wrong expr rtype scope");
      return false;
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::SYM>(op)) {
    if (node->Addr_datum_id().Is_null()) {
      CMPLR_DEV_WARN("Null addr_datum");
      return false;
    }
    if (node->Addr_datum()->Scope_level() == 0) {
      if (&node->Addr_datum()->Glob_scope() != Glob_scope()) {
        CMPLR_DEV_WARN("Wrong addr_datum scope");
        return false;
      }
    } else {
      if (node->Addr_datum()->Defining_func_scope() != Parent_func_scope()) {
        CMPLR_DEV_WARN("Wrong addr_datum scope");
        return false;
      }
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::ACC_TYPE>(op)) {
    if (node->Access_type_id().Is_null()) {
      CMPLR_DEV_WARN("Null access type");
      return false;
    }
    if (&node->Access_type()->Glob_scope() != Glob_scope()) {
      CMPLR_DEV_WARN("Wrong access type scope");
      return false;
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::FIELD_ID>(op)) {
    if (node->Field_id().Is_null()) {
      CMPLR_DEV_WARN("Null field id");
      return false;
    }
    if (&node->Field()->Glob_scope() != Glob_scope()) {
      CMPLR_DEV_WARN("Wrong field scope");
      return false;
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::PREG>(op)) {
    if (node->Preg_id().Is_null()) {
      CMPLR_DEV_WARN("Null preg");
      return false;
    }
    if (node->Preg()->Defining_func_scope() != Parent_func_scope()) {
      CMPLR_DEV_WARN("Wrong preg scope");
      return false;
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::CONST_ID>(op)) {
    if (node->Const_id().Is_null()) {
      CMPLR_DEV_WARN("Null constant");
      return false;
    }
    if (&node->Const()->Glob_scope() != Glob_scope()) {
      CMPLR_DEV_WARN("Wrong constant scope");
      return false;
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::ENTRY>(op)) {
    if (node->Entry_id().Is_null()) {
      CMPLR_DEV_WARN("Null entry id");
      return false;
    }
    if (node->Entry()->Scope_level() == 0) {
      if (&node->Entry()->Glob_scope() != Glob_scope()) {
        CMPLR_DEV_WARN("Wrong entry scope");
        return false;
      }
    } else {
      if (node->Entry()->Defining_func_scope() != Parent_func_scope()) {
        CMPLR_DEV_WARN("Wrong entry defining scope");
        return false;
      }
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::RET_VAR>(op)) {
    if (!node->Ret_preg_id().Is_null()) {
      if (node->Ret_preg()->Defining_func_scope() != Parent_func_scope()) {
        CMPLR_DEV_WARN("Wrong ret preg defining scope");
        return false;
      }
    }
  }
  // Verify children
  for (uint32_t i = 0; i < node->Num_child(); i++) {
    if (!Verify_node(node->Child(i))) return false;
  }
  return true;
}

NODE_PTR
CONTAINER::Clone_node(CONST_NODE_PTR orig) {
  AIR_ASSERT(orig != Null_ptr);
  AIR_ASSERT(!orig->Is_root());

  OPCODE   op  = orig->Opcode();
  uint32_t num = 0;
  if (orig->Has_added_chld()) {
    num = orig->Num_added_chld();
  }
  NODE_DATA_PTR node_ptr = New_node(op, num);
  new (node_ptr) NODE_DATA(*orig->Data().Addr());

  NODE_PTR node(NODE(this, node_ptr));
  return node;
}

NODE_PTR
CONTAINER::Clone_stmt_blk(CONST_NODE_PTR orig) {
  AIR_ASSERT(orig != Null_ptr);
  AIR_ASSERT(orig->Opcode() == core::OPC_BLOCK);
  STMT_DATA_PTR last = New_stmt(core::OPC_END_STMT_LIST);
  new (last) STMT_DATA(*(orig->End_stmt())->Data().Addr());

  NODE_DATA_PTR node_ptr = New_node(core::OPC_BLOCK);
  new (node_ptr) NODE_DATA(*orig->Data().Addr());
  NODE_PTR node(NODE(this, node_ptr));
  node->Set_begin_stmt(last.Id());
  node->Set_end_stmt(last.Id());
  return node;
}

NODE_PTR
CONTAINER::Clone_node_tree(CONST_NODE_PTR orig) {
  AIR_ASSERT(orig != Null_ptr);
  AIR_ASSERT(!orig->Is_root());
  NODE_PTR node;
  if (orig->Is_block()) {
    node = Clone_stmt_blk(orig);
    STMT_LIST sl(node);
    STMT_PTR  cloned;

    node->End_stmt()->Set_parent_node(node);
    for (STMT_PTR stmt = orig->Begin_stmt(); stmt != orig->End_stmt();
         stmt          = stmt->Next()) {
      cloned = Clone_stmt_tree(stmt);
      cloned->Set_parent_node(node);
      sl.Append(cloned);
    }
  } else {
    node = Clone_node(orig);
    for (uint32_t num = 0; num < orig->Num_child(); ++num) {
      node->Set_child(num, Clone_node_tree(orig->Child(num)));
    }
  }
  return node;
}

STMT_PTR
CONTAINER::Clone_stmt(CONST_STMT_PTR orig) {
  AIR_ASSERT(orig != Null_ptr);
  CONST_NODE_PTR orig_node = orig->Node();
  OPCODE         op        = orig_node->Opcode();

  STMT_DATA_PTR stmt_ptr;
  if (orig_node->Has_added_chld()) {
    stmt_ptr = New_stmt(op, orig_node->Num_arg());
  } else {
    stmt_ptr = New_stmt(op);
  }
  new (stmt_ptr) STMT_DATA(*orig->Data().Addr());
  STMT_PTR stmt(STMT(this, stmt_ptr));
  NODE_PTR node = stmt->Node();
  return stmt;
}

STMT_PTR
CONTAINER::Clone_stmt_tree(CONST_STMT_PTR orig) {
  AIR_ASSERT(orig != Null_ptr);
  CONST_NODE_PTR orig_root = orig->Node();
  STMT_PTR       stmt      = Clone_stmt(orig);
  NODE_PTR       root      = stmt->Node();

  for (uint32_t num = 0; num < root->Num_child(); ++num) {
    root->Set_child(num, Clone_node_tree(orig_root->Child(num)));
    if (root->Child(num)->Is_block()) {
      NODE_PTR  body = root->Child(num);
      STMT_LIST sl(body);
      body->Set_parent_stmt(stmt);
    }
  }
  return root->Stmt();
}

void CONTAINER::Print(std::ostream& os, bool rot, uint32_t indent) const {
  if (_func->Owning_func()->Is_defined()) {
    Entry_stmt()->Print(os, rot, indent);
  }
}

void CONTAINER::Print() const { Print(std::cout, true, 0); }

std::string CONTAINER::To_str(bool rot) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, rot);
  return buf.str();
}

}  // namespace base
}  // namespace air
