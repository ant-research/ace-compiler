//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstring>
#include <iostream>
#include <sstream>

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/base/st_misc.h"
#include "air/core/opcode.h"

using namespace air::core;

namespace air {
namespace base {

//=============================================================================
// class NODE member functions
//=============================================================================

FUNC_SCOPE* NODE::Func_scope() const { return _cont->Parent_func_scope(); }

GLOB_SCOPE& NODE::Glob_scope() const { return Func_scope()->Glob_scope(); }

uint32_t NODE::Num_child() const {
  uint32_t num = META_INFO::Op_num_child(Opcode());
  return (Has_added_chld() ? num + Num_added_chld() : num);
}
const char* NODE::Name() const { return META_INFO::Op_name(Opcode()); }

size_t NODE::Size() const {
  OPCODE op = Opcode();

  if (Has_added_chld()) {
    return NODE_DATA::Size(op, Num_added_chld());
  } else {
    return NODE_DATA::Size(op, 0);
  }
}

STMT_PTR
NODE::Stmt() const {
  AIR_ASSERT(Is_root());
  unsigned long addr = (unsigned long)_cont->Code_arena()->Find(Id().Value()) -
                       OFFSETOF(STMT_DATA, _data);

  return STMT_PTR(
      STMT(_cont, PTR_FROM_DATA<STMT_DATA>(reinterpret_cast<STMT_DATA*>(addr),
                                           ID<STMT_DATA>(Id().Value()))));
}

uint64_t NODE::Intconst() const {
  AIR_ASSERT(Opcode() == core::OPC_INTCONST);
  return reinterpret_cast<uint64_t&>(_data->_uu._u2._int_const);
}

bool NODE::Is_root() const {
  return !META_INFO::Has_prop<OPR_PROP::EXPR>(Opcode());
}

bool NODE::Is_leaf() const {
  return META_INFO::Has_prop<OPR_PROP::LEAF>(Opcode());
}

bool NODE::Is_ld() const {
  return META_INFO::Has_prop<OPR_PROP::LOAD>(Opcode());
}

bool NODE::Is_st() const {
  return META_INFO::Has_prop<OPR_PROP::STORE>(Opcode());
}

bool NODE::Is_call() const {
  return META_INFO::Has_prop<OPR_PROP::CALL>(Opcode());
}

bool NODE::Is_empty_blk() const { return Begin_stmt_id() == End_stmt_id(); }

bool NODE::Is_entry() const { return Opcode() == core::OPC_FUNC_ENTRY; }

bool NODE::Is_block() const { return Opcode() == core::OPC_BLOCK; }

bool NODE::Is_if() const { return Opcode() == core::OPC_IF; }

bool NODE::Is_do_loop() const { return Opcode() == core::OPC_DO_LOOP; }

bool NODE::Is_preg_op() const {
  return (Opcode() == core::OPC_LDP || Opcode() == core::OPC_STP ||
          Opcode() == core::OPC_LDPF || Opcode() == core::OPC_STPF);
}

bool NODE::Is_relational_op() const {
  return META_INFO::Has_prop<OPR_PROP::COMPARE>(Opcode());
}

bool NODE::Is_lib_call() const {
  return META_INFO::Has_prop<OPR_PROP::LIB_CALL>(Opcode());
}

bool NODE::Has_added_chld() const {
  return META_INFO::Has_prop<OPR_PROP::EX_CHILD>(Opcode());
}

bool NODE::Has_rtype() const {
  return META_INFO::Has_prop<OPR_PROP::EXPR>(Opcode());
}

bool NODE::Has_fld() const {
  return META_INFO::Has_prop<OPR_PROP::FIELD_ID>(Opcode());
}

bool NODE::Has_entry() const {
  return META_INFO::Has_prop<OPR_PROP::ENTRY>(Opcode());
}

bool NODE::Has_ret_var() const {
  return META_INFO::Has_prop<OPR_PROP::RET_VAR>(Opcode());
}

bool NODE::Has_sym() const {
  return META_INFO::Has_prop<OPR_PROP::SYM>(Opcode());
}

bool NODE::Has_preg() const {
  return META_INFO::Has_prop<OPR_PROP::PREG>(Opcode());
}

bool NODE::Has_const_id() const {
  return META_INFO::Has_prop<OPR_PROP::CONST_ID>(Opcode());
}

bool NODE::Has_access_type() const {
  return META_INFO::Has_prop<OPR_PROP::ACC_TYPE>(Opcode());
}

bool NODE::Has_ofst() const {
  return META_INFO::Has_prop<OPR_PROP::OFFSET>(Opcode());
}

TYPE_ID
NODE::Rtype_id() const {
  AIR_ASSERT(Has_rtype());
  AIR_ASSERT_MSG(_data->_comm._rtype != Null_st_id, "invalid rtype");
  return TYPE_ID(_data->_comm._rtype);
}

TYPE_PTR
NODE::Rtype() const { return _cont->Glob_scope()->Type(Rtype_id()); }

ATTR_ITER NODE::Begin_attr() const {
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::ATTR>(Opcode()));
  ATTR_LIST list(_data->_comm._attr, _cont->Parent_func_scope());
  return list.Begin();
}

ATTR_ITER NODE::End_attr() const {
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::ATTR>(Opcode()));
  ATTR_LIST list(_data->_comm._attr, _cont->Parent_func_scope());
  return list.End();
}

void NODE::Copy_attr(CONST_NODE_PTR node) {
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::ATTR>(Opcode()));
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::ATTR>(node->Opcode()));
  _data->_comm._attr = node->_data->_comm._attr;
}

const ATTR_ID& NODE::Attr_id() const {
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::ATTR>(Opcode()));
  return _data->_comm._attr;
}

NODE_ID
NODE::Child_id(uint32_t num) const {
  AIR_ASSERT(Num_child() > num);
  return NODE_ID(_data->_uu._fields[Num_fld() + num]);
}

NODE_PTR
NODE::Child(uint32_t num) const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Child_id(num))));
}

NODE_ID
NODE::Last_child_id() const {
  AIR_ASSERT(Num_child() > 0);
  return Child_id(Num_child() - 1);
}

NODE_PTR
NODE::Last_child() const {
  AIR_ASSERT(Num_child() > 0);
  return Child(Num_child() - 1);
}

NODE_ID
NODE::Loop_init_id() const {
  AIR_ASSERT(Is_do_loop());
  return Child_id(0);
}

NODE_PTR
NODE::Loop_init() const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Loop_init_id())));
}

NODE_ID
NODE::Compare_id() const {
  if (Is_do_loop()) {
    return Child_id(1);
  } else if (Is_if()) {
    return Child_id(0);
  } else {
    return NODE_ID();
  }
}

NODE_PTR
NODE::Compare() const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Compare_id())));
}

NODE_ID
NODE::Loop_incr_id() const {
  AIR_ASSERT(Is_do_loop())
  return Child_id(2);
}

NODE_PTR
NODE::Loop_incr() const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Loop_incr_id())));
}

NODE_ID
NODE::Body_blk_id() const {
  if (Is_do_loop()) {
    return Child_id(3);
  } else if (Opcode() == core::OPC_FUNC_ENTRY) {
    return Last_child_id();
  } else {
    return NODE_ID();
  }
}

NODE_PTR
NODE::Body_blk() const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Body_blk_id())));
}

NODE_ID
NODE::Then_blk_id() const {
  AIR_ASSERT(Is_if());
  return Child_id(1);
}

NODE_PTR
NODE::Then_blk() const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Then_blk_id())));
}

NODE_ID
NODE::Else_blk_id() const {
  AIR_ASSERT(Is_if());
  return Child_id(2);
}

NODE_PTR
NODE::Else_blk() const {
  return NODE_PTR(NODE(_cont, _cont->Code_arena()->Find(Else_blk_id())));
}

PREG_ID
NODE::Ret_preg_id() const {
  AIR_ASSERT(Has_ret_var());
  return PREG_ID(_data->_uu._u3._call._retv);
}

PREG_PTR
NODE::Ret_preg() const {
  return _cont->Parent_func_scope()->Preg(Ret_preg_id());
}

uint32_t NODE::Array_dim() const {
  AIR_ASSERT(Opcode() == core::OPC_ARRAY);
  return (Num_child() - 1);
}

NODE_PTR
NODE::Array_base() const {
  AIR_ASSERT(Opcode() == core::OPC_ARRAY);
  return Child(0);
}

NODE_PTR
NODE::Array_idx(uint32_t dim) const {
  AIR_ASSERT(Opcode() == core::OPC_ARRAY);
  AIR_ASSERT(Array_dim() > dim);
  return Child(dim + 1);
}

STMT_ID
NODE::Begin_stmt_id() const {
  AIR_ASSERT(Is_block());
  return STMT_ID(_data->_uu._u2._block._begin_stmt);
}

STMT_PTR
NODE::Begin_stmt() const { return _cont->Stmt(Begin_stmt_id()); }

STMT_ID
NODE::End_stmt_id() const {
  AIR_ASSERT(Is_block());
  return STMT_ID(_data->_uu._u2._block._end_stmt);
}

STMT_PTR
NODE::End_stmt() const { return _cont->Stmt(End_stmt_id()); }

STMT_ID
NODE::Parent_stmt_id() const {
  AIR_ASSERT(Is_block());
  return STMT_ID(_data->_comm._parent_stmt);
}

STMT_PTR
NODE::Parent_stmt() const { return _cont->Stmt(Parent_stmt_id()); }

TYPE_ID
NODE::Access_type_id() const {
  AIR_ASSERT(Has_access_type());
  return TYPE_ID(_data->_uu._u2._mem_acc._atype);
}

TYPE_PTR
NODE::Access_type() const {
  return _cont->Glob_scope()->Type(Access_type_id());
}

ADDR_DATUM_ID
NODE::Addr_datum_id() const {
  AIR_ASSERT(Has_sym());
  return ADDR_DATUM_ID(_data->_uu._addr_datum);
}

ADDR_DATUM_PTR
NODE::Addr_datum() const {
  return _cont->Parent_func_scope()->Addr_datum(Addr_datum_id());
}

ADDR_DATUM_ID
NODE::Iv_id() const {
  AIR_ASSERT(Is_do_loop());
  return ADDR_DATUM_ID(_data->_uu._iv);
}

ADDR_DATUM_PTR
NODE::Iv() const { return _cont->Parent_func_scope()->Addr_datum(Iv_id()); }

FIELD_ID
NODE::Field_id() const {
  AIR_ASSERT(Has_fld());
  return FIELD_ID(_data->_uu._u3._fld_acc._fld);
}

FIELD_PTR
NODE::Field() const { return _cont->Glob_scope()->Field(Field_id()); }

uint32_t NODE::Num_arg() const {
  AIR_ASSERT(Has_added_chld());
  return _data->_comm._core._num_added_chld;
}

int64_t NODE::Ofst() const {
  AIR_ASSERT(Has_ofst());
  return reinterpret_cast<int64_t&>(_data->_uu._u4._ofst_acc._ofst);
}

uint32_t NODE::Num_fld() const {
  OPCODE   op  = Opcode();
  uint32_t num = META_INFO::Op_num_field(op);
  // TODO op_array
  return num;
}

uint32_t NODE::Num_added_chld() const {
  AIR_ASSERT(Has_added_chld());
  return _data->_comm._core._num_added_chld;
}

ENTRY_ID
NODE::Entry_id() const {
  AIR_ASSERT(Has_entry());
  return ENTRY_ID(_data->_uu._entry);
}

ENTRY_PTR
NODE::Entry() const { return _cont->Glob_scope()->Entry_point(Entry_id()); }

CONSTANT_ID
NODE::Const_id() const {
  AIR_ASSERT(Has_const_id());
  return CONSTANT_ID(_data->_uu._const);
}

CONSTANT_PTR
NODE::Const() const { return _cont->Glob_scope()->Constant(Const_id()); }

PREG_ID
NODE::Preg_id() const {
  AIR_ASSERT(Has_preg());
  return PREG_ID(_data->_uu._preg);
}

PREG_PTR
NODE::Preg() const { return _cont->Parent_func_scope()->Preg(Preg_id()); }

void NODE::Set_access_type(CONST_TYPE_PTR type) { Set_access_type(type->Id()); }

void NODE::Set_access_type(TYPE_ID id) {
  AIR_ASSERT(Has_access_type());
  _data->_uu._u2._mem_acc._atype = id.Value();
}

void NODE::Set_field(FIELD_ID fld) {
  AIR_ASSERT(Has_fld());
  _data->_uu._u3._fld_acc._fld = fld.Value();
}

void NODE::Set_field(CONST_FIELD_PTR fld) { Set_field(fld->Id()); }

void NODE::Set_begin_stmt(STMT_ID stmt) {
  AIR_ASSERT(Is_block());
  _data->_uu._u2._block._begin_stmt = stmt.Value();
}

void NODE::Set_begin_stmt(CONST_STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  Set_begin_stmt(stmt->Id());
}

void NODE::Set_end_stmt(STMT_ID stmt) {
  AIR_ASSERT(Is_block());
  _data->_uu._u2._block._end_stmt = stmt.Value();
}

void NODE::Set_end_stmt(CONST_STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  Set_end_stmt(stmt->Id());
}

void NODE::Set_parent_stmt(STMT_ID stmt) {
  AIR_ASSERT(Is_block());
  _data->_comm._parent_stmt = stmt.Value();
}

void NODE::Set_parent_stmt(CONST_STMT_PTR stmt) {
  AIR_ASSERT(Container() == stmt->Container());
  Set_parent_stmt(stmt->Id());
}

void NODE::Set_child(uint32_t num, NODE_ID id) {
  AIR_ASSERT(Num_child() > num);
  _data->_uu._fields[Num_fld() + num] = id.Value();
}

void NODE::Set_child(uint32_t num, CONST_NODE_PTR node) {
  AIR_ASSERT(Container() == node->Container());
  Set_child(num, node->Id());
}

void NODE::Set_rtype(TYPE_ID type) {
  AIR_ASSERT(Has_rtype());
  _data->_comm._rtype = type.Value();
  AIR_ASSERT((!Has_access_type()) ||
             ((Rtype()->Is_int() && Access_type()->Is_int()) ||
              (Rtype() == Access_type())));
}

void NODE::Set_rtype(CONST_TYPE_PTR type) { Set_rtype(type->Id()); }

void NODE::Set_addr_datum(ADDR_DATUM_ID id) {
  AIR_ASSERT(Has_sym());
  _data->_uu._addr_datum = id.Value();
}

void NODE::Set_addr_datum(CONST_ADDR_DATUM_PTR datum) {
  Set_addr_datum(datum->Id());
}

void NODE::Set_entry(ENTRY_ID id) {
  // AIR_ASSERT(Has_entry());
  _data->_uu._entry = id.Value();
}

void NODE::Set_entry(CONST_ENTRY_PTR entry) { Set_entry(entry->Id()); }

void NODE::Set_ret_preg(PREG_ID id) {
  AIR_ASSERT(Has_ret_var());
  _data->_uu._u3._call._retv = id.Value();
}

void NODE::Set_ret_preg(CONST_PREG_PTR preg) {
  if (preg == Null_ptr) {
    Set_ret_preg(PREG_ID());
  } else {
    Set_ret_preg(preg->Id());
  }
}

void NODE::Set_arg(uint32_t num, NODE_ID node) {
  AIR_ASSERT(Num_arg() > num);
  Set_child(META_INFO::Op_num_child(Opcode()) + num, node);
}

void NODE::Set_arg(uint32_t num, CONST_NODE_PTR node) {
  AIR_ASSERT(Container() == node->Container());
  Set_arg(num, node->Id());
}

void NODE::Set_num_arg(uint32_t num) {
  AIR_ASSERT(Has_added_chld());
  _data->_comm._core._num_added_chld = num;
}

OPCODE NODE::Opcode() const { return OPCODE(_data->_comm._core._opcode); }

void NODE::Set_const(CONST_CONSTANT_PTR cst) { Set_const(cst->Id()); }

void NODE::Set_const(CONSTANT_ID id) {
  AIR_ASSERT(Has_const_id());
  _data->_uu._const = id.Value();
}

void NODE::Set_preg(CONST_PREG_PTR preg) { Set_preg(preg->Id()); }

void NODE::Set_preg(PREG_ID id) {
  AIR_ASSERT(Has_preg());
  _data->_uu._preg = id.Value();
}

void NODE::Set_iv(CONST_ADDR_DATUM_PTR iv) { Set_iv(iv->Id()); }

void NODE::Set_iv(ADDR_DATUM_ID id) {
  AIR_ASSERT(Is_do_loop());
  _data->_uu._iv = id.Value();
}

void NODE::Set_intconst(uint64_t val) {
  AIR_ASSERT(Opcode() == core::OPC_INTCONST);
  AIR_ASSERT(Rtype()->Is_int());
  reinterpret_cast<uint64_t&>(_data->_uu._u2._int_const) = val;
}

void NODE::Set_ofst(int64_t ofst) {
  AIR_ASSERT(Has_ofst());
  reinterpret_cast<int64_t&>(_data->_uu._u4._ofst_acc._ofst) = ofst;
}

void NODE::Print_tree(std::ostream& os, bool rot, uint32_t indent) const {
  os << std::hex << std::showbase;

  if (Is_block()) {
    // print block
    Print(os, indent);
    os << std::endl;
    // print block statements
    for (CONST_STMT_PTR stmt = Begin_stmt(); stmt != End_stmt();
         stmt                = stmt->Next()) {
      stmt->Print(os, rot, indent + 1);
    }
    // print end block
    os << std::string(indent * INDENT_SPACE, ' ') << "end_block ID("
       << Id().Value() << ")" << std::endl;
  } else if (Is_entry() || rot) {
    // print self
    Print(os, indent);
    os << std::endl;
    // print child
    for (uint32_t i = 0; i < Num_child(); i++) {
      Child(i)->Print_tree(os, rot, indent + 1);
    }
  } else {
    // print child
    for (uint32_t i = 0; i < Num_child(); i++) {
      Child(i)->Print_tree(os, rot, indent + 1);
    }
    // print self
    Print(os, indent);
    os << std::endl;
  }
}

void NODE::Print(std::ostream& os, uint32_t indent) const {
  FUNC_SCOPE* func = Container()->Parent_func_scope();
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  if (Domain() > 0) {
    const char* domain_str = META_INFO::Domain_name(Domain());
    os << domain_str << ".";
  }
  const char* op_str = Name();
  os << op_str;

  OPCODE op = Opcode();
  if (Has_preg()) {
    os << " PREG[" << Preg_id().Value() << "]";
  }
  if (Has_sym()) {
    CONST_ADDR_DATUM_PTR datum = Addr_datum();
    os << " \"" << datum->Name()->Char_str();
    if (datum->Name()->Is_undefined()) {
      os << "_" << datum->Id().Value();
    }
    os << "\" " << (datum->Is_var() ? "VAR" : "FML") << "["
       << datum->Id().Value() << "]";
  }
  if (Has_entry()) {
    CONST_ENTRY_PTR entry = Entry();
    os << " \"" << entry->Name()->Char_str() << "\" ENT[" << entry->Id().Value()
       << "]";
  }
  if (Has_const_id()) {
    CONST_CONSTANT_PTR cst = Const();
    if (cst->Is_float()) {
      PRIMITIVE_TYPE ctype = cst->Type()->Cast_to_prim()->Encoding();
      if (ctype == PRIMITIVE_TYPE::FLOAT_32) {
        os << " #" << cst->Float_literal().Val_as_float();
      } else if (ctype == PRIMITIVE_TYPE::FLOAT_64) {
        os << " #" << cst->Float_literal().Val_as_double();
      } else {
        os << " #" << cst->Float_literal().Val_as_long_double();
      }
    } else {
      os << " CST[" << cst->Id().Value() << "]";
    }
  }
  if (op == core::OPC_INTCONST) {
    if (Rtype()->Is_signed_int()) {
      os << " #" << static_cast<int64_t>(Intconst());
    } else {
      os << " #" << Intconst();
    }
  }
  if (META_INFO::Has_prop<OPR_PROP::ATTR>(op) &&
      _data->_comm._attr != Null_id) {
    os << " ATTR[";
    ATTR_LIST list(_data->_comm._attr, _cont->Parent_func_scope());
    list.Print(os, 0);
    os << "]";
  }
  if (Has_rtype()) {
    os << " RTYPE[" << Rtype_id().Value() << "](" << Rtype()->Name()->Char_str()
       << ")";
  }
  if (Has_fld()) {
    os << " FLD[" << Field_id().Value() << "](" << Field()->Name()->Char_str()
       << ")";
  }
  if (Is_root()) {
    os << " ID(" << Id().Value() << ") LINE(";
    os << std::dec << Spos().Line() << ":" << Spos().Col() << ":"
       << Spos().Count() << ")";
  }
}

void NODE::Print() const { Print_tree(std::cout, true, 0); }

std::string NODE::To_str(bool rot) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print_tree(os, rot);
  return buf.str();
}

//=============================================================================
// class STMT member functions
//=============================================================================

FUNC_SCOPE* STMT::Func_scope() const { return _cont->Parent_func_scope(); }

GLOB_SCOPE& STMT::Glob_scope() const { return Func_scope()->Glob_scope(); }

STMT_ID
STMT::Prev_id() const { return _data->_prev; }

STMT_PTR
STMT::Prev() const {
  if (!Has_prev()) return Null_ptr;
  return _cont->Stmt(Prev_id());
}

STMT_ID
STMT::Next_id() const { return _data->_next; }

STMT_PTR
STMT::Next() const {
  if (!Has_next()) return Null_ptr;
  return _cont->Stmt(Next_id());
}

NODE_ID
STMT::Parent_node_id() const {
  return (NODE_ID)_data->_data._comm._parent_node;
}

NODE_PTR
STMT::Parent_node() const {
  if (!Has_parent_node()) return Null_ptr;
  return _cont->Node(Parent_node_id());
}

STMT_ID
STMT::Enclosing_stmt_id() const {
  NODE_PTR parent = Parent_node();
  if (parent == Null_ptr) return Null_id;
  return parent->Parent_stmt_id();
}

STMT_PTR
STMT::Enclosing_stmt() const {
  if (!Has_enclosing_stmt()) return Null_ptr;
  return _cont->Stmt(Enclosing_stmt_id());
}

bool STMT::Has_prev() const { return (Prev_id()) != Null_id; }

bool STMT::Has_next() const { return (Next_id()) != Null_id; }

bool STMT::Has_parent_node() const { return (Parent_node_id()) != Null_id; }

bool STMT::Has_enclosing_stmt() const {
  return (Enclosing_stmt_id()) != Null_id;
}

void STMT::Set_prev(STMT_ID prev) { _data->_prev = prev; }

void STMT::Set_prev(CONST_STMT_PTR prev) {
  AIR_ASSERT((prev == Null_ptr) || (Container() == prev->Container()));
  Set_prev((prev == Null_ptr) ? Null_id : prev->Id());
}

void STMT::Set_next(STMT_ID next) { _data->_next = next; }

void STMT::Set_next(CONST_STMT_PTR next) {
  AIR_ASSERT((next == Null_ptr) || (Container() == next->Container()));
  Set_next((next == Null_ptr) ? Null_id : next->Id());
}

void STMT::Set_parent_node(CONST_NODE_PTR node) {
  AIR_ASSERT(Container() == node->Container());
  Set_parent_node(node->Id());
}

void STMT::Set_parent_node(NODE_ID node) {
  _data->_data._comm._parent_node = node.Value();
}

NODE_PTR
STMT::Node() const {
  return NODE_PTR(
      NODE(_cont, _cont->Code_arena()->Find(Reinterpret_cast<NODE_ID>(Id()))));
}

void STMT::Print(std::ostream& os, bool rot, uint32_t indent) const {
  CONST_NODE_PTR exp = Node();
  exp->Print_tree(os, rot, indent);
}

void STMT::Print() const { Print(std::cout, true, 0); }

std::string STMT::To_str(bool rot) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, rot);
  return buf.str();
}

}  // namespace base
}  // namespace air
