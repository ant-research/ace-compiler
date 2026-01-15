//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_expr.h"

#if ENABLED_HSSA
#include <sstream>

#include "air/base/node.h"
#include "air/opt/hssa_container.h"
#include "air/opt/ssa_container.h"

using namespace air::base;

namespace air {
namespace opt {

bool VAR_DATA::Match(VAR_DATA_PTR other) const {
  if (Var_kind() == other->Var_kind() && Var_id() == other->Var_id() &&
      Sub_idx() == other->Sub_idx() && Ver() == other->Ver()) {
    return true;
  }
  return false;
}

bool VAR_DATA::Match_lex(VAR_DATA_PTR other) const {
  if (Var_kind() == other->Var_kind() && Var_id() == other->Var_id() &&
      Sub_idx() == other->Sub_idx()) {
    return true;
  }
  return false;
}

std::string VAR_DATA::Name(HCONTAINER* hssa_cont) const {
  std::string name_str = "";
  if (Var_kind() == VAR_KIND::PREG) {
    name_str = "_preg." + std::to_string(Var_id());
  } else if (Var_kind() == VAR_KIND::ADDR_DATUM) {
    FUNC_SCOPE* fs = hssa_cont->Air_cont()->Parent_func_scope();
    SYM_ID      sym_id(Var_id());
    SYM_PTR     sym = fs->Find_sym(sym_id);
    AIR_ASSERT(sym != air::base::Null_ptr);
    name_str = sym->Name()->Char_str();
  } else {
    AIR_ASSERT(false);
  }
  if (Sub_idx() != SSA_SYM::NO_INDEX) {
    name_str = name_str + ".f" + std::to_string(_sub_idx);
  }
  return name_str;
}

void VAR_DATA::Print(HCONTAINER* hssa_cont, std::ostream& os,
                     uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  if (_ver_id != SSA_VER_ID()) {
    SSA_CONTAINER* ssa_cont = hssa_cont->Ssa_cont();
    SSA_SYM_PTR    sym_ptr  = ssa_cont->Ver_sym(_ver_id);
    sym_ptr->Print(os);
  } else {
    os << Name(hssa_cont);
  }
}

CST_DATA::CST_DATA(NODE_PTR node) : HEXPR_DATA(node, EK_CONST) {
  AIR_ASSERT(node->Domain() == air::core::CORE);
  switch (node->Operator()) {
    case air::core::OPCODE::INTCONST:
      Set_cst_kind(CSTKIND_INT);
      Set_value(node->Intconst());
      break;
    case air::core::OPCODE::LDC:
    case air::core::OPCODE::LDCA:
      Set_cst_kind(CSTKIND_ID);
      Set_value(node->Const_id());
      break;
    case air::core::OPCODE::ONE:
      Set_cst_kind(CSTKIND_INT);
      Set_value(1);
      break;
    case air::core::OPCODE::ZERO:
      Set_cst_kind(CSTKIND_INT);
      Set_value(0);
      break;
    default:
      AIR_ASSERT_MSG(false, "invalid const operator");
  }
}

uint32_t CST_DATA::Hash_idx(void) const {
  switch (Cst_kind()) {
    case CSTKIND_INT:
      return (uint32_t)Cst_val();
    case CSTKIND_ID:
      return Cst_id().Value();
    default:
      AIR_ASSERT_MSG(false, "invalid const operator");
  }
  return 0;
}

bool CST_DATA::Match(CST_DATA_PTR other) const {
  if (Cst_kind() != other->Cst_kind()) return false;
  switch (Cst_kind()) {
    case CSTKIND_INT:
      return Cst_val() == other->Cst_val();
    case CSTKIND_ID:
      return Cst_id() == other->Cst_id();
    default:
      AIR_ASSERT_MSG(false, "invalid const operator");
  }
  return false;
}

void CST_DATA::Print(HCONTAINER* hssa_cont, std::ostream& os,
                     uint32_t indent) const {
  switch (Cst_kind()) {
    case CSTKIND_INT:
      os << "#" << Cst_val();
      break;
    case CSTKIND_ID:
      os << "CST" << Cst_id().Value();
      break;
    default:
      AIR_ASSERT_MSG(false, "cst kind not supported");
  }
}

uint32_t OP_DATA::Hash_idx(void) const {
  uint32_t hash_idx = Opcode().operator unsigned int();
  for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
    HEXPR_ID kid = Kid(idx);
    hash_idx += kid.Value() << DEF_EXPR_BITS;
  }
  return hash_idx;
}

bool OP_DATA::Match(OP_DATA_PTR other) const {
  if (Opcode() != other->Opcode()) return false;
  if (Kid_cnt() != other->Kid_cnt()) return false;
  for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
    if (Kid(idx) != other->Kid(idx)) {
      return false;
    }
  }
  return true;
}

void OP_DATA::Print(HCONTAINER* hssa_cont, std::ostream& os,
                    uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  SSA_CONTAINER* ssa_cont = hssa_cont->Ssa_cont();
  for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
    os << std::endl;
    HEXPR_ID kid = Kid(idx);
    AIR_ASSERT_MSG(kid != HEXPR_ID(), "kid not set");
    HEXPR_PTR kid_ptr = hssa_cont->Cr_ptr(kid);
    kid_ptr->Print(os, indent + 1);
  }
}

HEXPR_DATA::HEXPR_DATA(NODE_PTR node, CODEKIND k)
    : _opc(node->Opcode()),
      _kind(k),
      _usecnt(0),
      _flags(CF_EMPTY),
      _next(HEXPR_ID()) {
  _rtype   = node->Has_rtype() ? node->Rtype_id() : TYPE_ID();
  _dsctype = node->Has_access_type() ? node->Access_type_id() : TYPE_ID();
  _attr    = node->Attr_id();
  _spos    = node->Spos();
}

air::base::FUNC_SCOPE* HEXPR::Func_scope() const {
  return Hssa_cont()->Air_cont()->Parent_func_scope();
}

TYPE_PTR HEXPR::Rtype(void) const {
  return Hssa_cont()->Air_cont()->Glob_scope()->Type(Rtype_id());
}

uint32_t HEXPR::Kid_cnt(void) const {
  if (Kind() == EK_OP) {
    return Cast_to_op_expr()->Kid_cnt();
  } else {
    return 0;
  }
}

int32_t HEXPR::Kid_idx(HEXPR_PTR expr) const {
  if (Kind() == EK_OP) {
    for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
      HEXPR_PTR kid = Kid(idx);
      if (kid == expr) {
        return idx;
      }
    }
  }
  return -1;
}

HEXPR_PTR HEXPR::Kid(uint32_t idx) const {
  return Hssa_cont()->Cr_ptr(Cast_to_op_expr()->Kid(idx));
}

void HEXPR::Set_defphi(HPHI_PTR hphi) {
  Cast_to_var_expr()->Set_def_phi(hphi->Id());
}

void HEXPR::Set_kid(uint32_t idx, HEXPR_ID id) {
  Cast_to_op_expr()->Set_kid(idx, id);
}

void HEXPR::Set_kid(uint32_t idx, HEXPR_PTR expr) { Set_kid(idx, expr->Id()); }

bool HEXPR::Match(HEXPR_PTR other, HCONTAINER* cont) const {
  if (Id() == other->Id()) return true;
  if (Kind() != other->Kind()) return false;
  switch (Kind()) {
    case EK_VAR: {
      VAR_DATA_PTR var_expr = Cast_to_var_expr();
      return var_expr->Match(other->Cast_to_var_expr());
    }
    case EK_OP: {
      OP_DATA_PTR op_expr = Cast_to_op_expr();
      return op_expr->Match(other->Cast_to_op_expr());
    }
    case EK_CONST: {
      CST_DATA_PTR cst_expr = Cast_to_cst_expr();
      return cst_expr->Match(other->Cast_to_cst_expr());
    }
    default:
      AIR_ASSERT(false);
  }
  return false;
}

bool HEXPR::Match_lex(HEXPR_PTR other) const {
  if (Id() == other->Id()) return true;
  if (Kind() != other->Kind()) return false;
  switch (Kind()) {
    case EK_VAR: {
      VAR_DATA_PTR var_expr = Cast_to_var_expr();
      return var_expr->Match_lex(other->Cast_to_var_expr());
    }
    case EK_CONST:
      // Constants must have the same EXPR node to match
      return false;
    default:
      AIR_ASSERT(false);
  }
  return false;
}

uint32_t HEXPR::Hash_idx() const {
  switch (Kind()) {
    case EK_VAR:
      CMPLR_ASSERT(false, "TO IMPL ");
      break;
    case EK_OP: {
      OP_DATA_PTR op_expr = Cast_to_op_expr();
      return op_expr->Hash_idx();
    }
    case EK_CONST: {
      CST_DATA_PTR cst_expr = Cast_to_cst_expr();
      return cst_expr->Hash_idx();
    }
    default:
      AIR_ASSERT(false);
  }
  return 0;
}

bool HEXPR::Replace_expr(HEXPR_ID expr, HEXPR_ID new_expr) {
  if (this->Id() == expr) return true;
  bool is_replaced = false;
  switch (Kind()) {
    case EK_OP: {
      for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
        HEXPR_PTR kid = Kid(idx);
        is_replaced |= kid->Replace_expr(expr, new_expr);
        Set_kid(idx, kid->Id() == expr ? new_expr : kid->Id());
      }
    } break;
    case EK_VAR:
    case EK_CONST:
      is_replaced = (Id() == expr);
      break;
    default:
      AIR_ASSERT_MSG(false, "TO IMPL:HEXPR::Replace_expr");
  }
  return is_replaced;
}

bool HEXPR::Is_same_e_ver(HEXPR_PTR expr) const {
  AIR_ASSERT(!expr->Is_null());
  bool is_same = true;
  switch (Kind()) {
    case EK_OP:
      if (Kid_cnt() != expr->Kid_cnt()) return false;
      for (uint32_t idx = 0; idx < Kid_cnt() && is_same; idx++) {
        HEXPR_PTR kid = Kid(idx);
        is_same &= kid->Is_same_e_ver(expr->Kid(idx));
      }
      break;
    case EK_VAR:
    case EK_CONST:
      is_same = (Id() == expr->Id());
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected HEXPR::Is_same_e_ver")
  }
  return is_same;
}

HSTMT_PTR HEXPR::Def_stmt(void) const {
  HSTMT_ID     def_id   = HSTMT_ID();
  VAR_DATA_PTR var_expr = Cast_to_var_expr();
  if (var_expr->Def_by_stmt()) {
    def_id = var_expr->Def_stmt();
  } else if (var_expr->Def_by_chi()) {
    def_id = Hssa_cont()->Chi_ptr(var_expr->Def_chi())->Stmt();
  } else if (var_expr->Def_by_phi()) {
    return Null_ptr;
  } else {
    CMPLR_ASSERT(false, "TO IMPL");
    return Null_ptr;
  }
  return Hssa_cont()->Stmt_ptr(def_id);
}

BB_ID HEXPR::Def_bb() const {
  VAR_DATA_PTR var_expr = Cast_to_var_expr();
  HPHI_PTR     phi      = Def_phi();
  if (phi != Null_ptr) {
    return phi->Bb_id();
  } else {
    HSTMT_PTR def_stmt = Def_stmt();
    if (def_stmt->Is_null()) {
      CMPLR_ASSERT(false, "TO IMPL");
      return BB_ID();
    }
    return def_stmt->Bb_id();
  }
}

HPHI_PTR HEXPR::Def_phi() const {
  VAR_DATA_PTR var_expr = Cast_to_var_expr();
  if (var_expr->Def_by_phi()) {
    HPHI_ID phi = var_expr->Def_phi();
    return Hssa_cont()->Phi_ptr(phi);
  }
  return Null_ptr;
}

void HEXPR::Print_opcode(OPCODE opcode, std::ostream& os, uint32_t indent) {
  os << std::string(indent * INDENT_SPACE, ' ');
  if (opcode.Domain() > 0) {
    const char* domain_str = META_INFO::Domain_name(opcode.Domain());
    os << domain_str << ".";
  }
  const char* op_str = META_INFO::Op_name(opcode);
  os << op_str;
}

void HEXPR::Print(std::ostream& os, uint32_t indent) const {
  switch (Data()->Kind()) {
    case EK_CONST: {
      CST_DATA_PTR cst_expr = Static_cast<CST_DATA_PTR>(Data());
      Print_opcode(cst_expr->Opcode(), os, indent);
      os << " expr" << Id().Value() << "(";
      cst_expr->Print(Hssa_cont(), os, indent);
      os << ")";
    } break;
    case EK_VAR: {
      VAR_DATA_PTR var_expr = Static_cast<VAR_DATA_PTR>(Data());
      var_expr->Print(Hssa_cont(), os, indent);
      os << "(expr" << Id().Value() << ")";
    } break;
    case EK_OP: {
      OP_DATA_PTR op_expr = Static_cast<OP_DATA_PTR>(Data());
      Print_opcode(op_expr->Opcode(), os, indent);
      os << "(expr" << Id().Value() << ")";
      op_expr->Print(Hssa_cont(), os, indent);
    } break;
    default:
      AIR_ASSERT_MSG(false, "Invalid HEXPR kind %d", Data()->Kind());
  }
}

void HEXPR::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HEXPR::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air
#endif