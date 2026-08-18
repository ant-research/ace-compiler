//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_expr.h"

#include <sstream>

#include "air/base/node.h"
#include "air/opt/hssa_container.h"
#include "air/opt/ssa_container.h"

using namespace air::base;
namespace air {
namespace opt {

bool VAR_HCR_DATA::Match(VAR_HCR_DATA_PTR other) const {
  AIR_ASSERT_MSG(false, "TO IMPL");
  return false;
}

bool VAR_HCR_DATA::Match_lex(VAR_HCR_DATA_PTR other) const {
  if (Var_kind() == other->Var_kind() && Var_id() == other->Var_id() &&
      Sub_idx() == other->Sub_idx()) {
    return true;
  }
  return false;
}

std::string VAR_HCR_DATA::Name(HSSA_CONTAINER* hssa_cont) const {
  std::string name_str = "";
  if (_var_kind == VAR_KIND::PREG) {
    name_str = "_preg." + std::to_string(_var_id);
  } else if (_var_kind == VAR_KIND::ADDR_DATUM) {
    FUNC_SCOPE* fs = hssa_cont->Air_cont()->Parent_func_scope();
    SYM_ID      sym_id(_var_id);
    SYM_PTR     sym = fs->Find_sym(sym_id);
    AIR_ASSERT(sym != air::base::Null_ptr);
    name_str = sym->Name()->Char_str();
  } else {
    AIR_ASSERT(false);
  }
  if (_sub_idx != SSA_SYM::NO_INDEX) {
    name_str = name_str + ".f" + std::to_string(_sub_idx);
  }
  return name_str;
}

void VAR_HCR_DATA::Print(HSSA_CONTAINER* hssa_cont, std::ostream& os,
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

CST_HCR_DATA::CST_HCR_DATA(NODE_PTR node) : HCR_DATA(node, CK_CONST) {
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

uint32_t CST_HCR_DATA::Hash_idx(void) {
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

bool CST_HCR_DATA::Match(CST_HCR_DATA_PTR other) const {
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

void CST_HCR_DATA::Print(HSSA_CONTAINER* hssa_cont, std::ostream& os,
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

uint32_t OP_HCR_DATA::Hash_idx(void) {
  uint32_t hash_idx = Opcode().operator unsigned int();
  for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
    HCR_ID kid = Kid(idx);
    hash_idx += kid.Value() << DEF_CR_BITS;
  }
  return hash_idx;
}

bool OP_HCR_DATA::Match(OP_HCR_DATA_PTR other) const {
  if (Opcode() != other->Opcode()) return false;
  if (Kid_cnt() != other->Kid_cnt()) return false;
  for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
    if (Kid(idx) != other->Kid(idx)) {
      return false;
    }
  }
  return true;
}

bool OP_HCR_DATA::Match_lex(OP_HCR_DATA_PTR other) const {
  //CMPLR_ASSERT(false, "TO IMPL");
  return false;
}

void OP_HCR_DATA::Print(HSSA_CONTAINER* hssa_cont, std::ostream& os,
                        uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  SSA_CONTAINER* ssa_cont = hssa_cont->Ssa_cont();
  for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
    os << std::endl;
    HCR_ID kid = Kid(idx);
    CMPLR_ASSERT(kid != HCR_ID(), "kid not set");
    if (kid != HCR_ID()) {
      HCR_PTR kid_ptr = hssa_cont->Cr_ptr(kid);
      kid_ptr->Print(os, indent + 1);
    }
  }
}

HCR_DATA::HCR_DATA(NODE_PTR node, CODEKIND k)
    : _opc(node->Opcode()),
      _kind(k),
      _usecnt(0),
      _flags(CF_EMPTY),
      _next(HCR_ID()) {
  _rtype   = node->Has_rtype() ? node->Rtype_id() : TYPE_ID();
  _dsctype = node->Has_access_type() ? node->Access_type_id() : TYPE_ID();
  _attr    = node->Attr_id();
  _spos    = node->Spos();
}

air::base::FUNC_SCOPE* HCR::Func_scope() const {
  return Hssa_cont()->Air_cont()->Parent_func_scope();
}

TYPE_PTR HCR::Rtype(void) const {
  return Hssa_cont()->Air_cont()->Glob_scope()->Type(Rtype_id());
}

uint32_t HCR::Kid_cnt(void) {
  if (Kind() == CK_OP) {
    return Cast_to_op_cr()->Kid_cnt();
  } else {
    return 0;
  }
}

int32_t HCR::Kid_idx(HCR_PTR expr) {
  if (Kind() == CK_OP) {
    for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
      HCR_PTR kid = Kid(idx);
      if (kid == expr) {
        return idx;
      }
    }
  }
  return -1;
}

HCR_PTR HCR::Kid(uint32_t idx) {
  AIR_ASSERT(Kind() == CK_OP);
  return Hssa_cont()->Cr_ptr(Cast_to_op_cr()->Kid(idx));
}

void HCR::Set_defphi(HPHI_PTR hphi) {
  AIR_ASSERT(Kind() == CK_VAR);
  VAR_HCR_DATA_PTR var_cr = air::base::Static_cast<VAR_HCR_DATA_PTR>(_data);
  var_cr->Set_def_phi(hphi->Id());
}

void HCR::Set_kid(uint32_t idx, HCR_ID id) {
  AIR_ASSERT(Kind() == CK_OP);
  OP_HCR_DATA_PTR op_cr = air::base::Static_cast<OP_HCR_DATA_PTR>(_data);
  op_cr->Set_kid(idx, id);
}

void HCR::Set_kid(uint32_t idx, HCR_PTR expr) { Set_kid(idx, expr->Id()); }

void HCR::Init_const(NODE_PTR node) {
  CMPLR_ASSERT(false, "TO IMPL Inist_const");
}

bool HCR::Match(HCR_PTR other, HSSA_CONTAINER* cont) const {
  if (Id() == other->Id()) return true;
  if (Kind() != other->Kind()) return false;
  switch (Kind()) {
    case CK_VAR: {
      VAR_HCR_DATA_PTR var_cr = Cast_to_var_cr();
      return var_cr->Match(other->Cast_to_var_cr());
    }
    case CK_OP: {
      OP_HCR_DATA_PTR op_cr = Cast_to_op_cr();
      return op_cr->Match(other->Cast_to_op_cr());
    }
    case CK_CONST: {
      CST_HCR_DATA_PTR cst_cr = Cast_to_cst_cr();
      return cst_cr->Match(other->Cast_to_cst_cr());
    }
    default:
      AIR_ASSERT(false);
  }
  return false;
}

bool HCR::Match_lex(HCR_PTR other) const {
  if (Id() == other->Id()) return true;
  if (Kind() != other->Kind()) return false;
  switch (Kind()) {
    case CK_VAR: {
      VAR_HCR_DATA_PTR var_cr = Cast_to_var_cr();
      return var_cr->Match_lex(other->Cast_to_var_cr());
    }
    case CK_OP: {
      OP_HCR_DATA_PTR op_cr = Cast_to_op_cr();
      return op_cr->Match_lex(other->Cast_to_op_cr());
    }
    case CK_CONST:
      // Constants must have the same CR node to match
      return false;
    default:
      AIR_ASSERT(false);
  }
  return false;
}

uint32_t HCR::Hash_idx() const {
  switch (Kind()) {
    case CK_VAR:
      CMPLR_ASSERT(false, "TO IMPL ");
      break;
    case CK_OP: {
      OP_HCR_DATA_PTR op_cr = Cast_to_op_cr();
      return op_cr->Hash_idx();
    }
    case CK_CONST: {
      CST_HCR_DATA_PTR cst_cr = Cast_to_cst_cr();
      return cst_cr->Hash_idx();
    }
    default:
      AIR_ASSERT(false);
  }
  return 0;
}

bool HCR::Replace_cr(HCR_ID cr, HCR_ID new_cr) {
  if (this->Id() == cr) return true;
  bool is_replaced = false;
  switch (Kind()) {
    case CK_OP: {
      for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
        HCR_PTR kid = Kid(idx);
        is_replaced |= kid->Replace_cr(cr, new_cr);
        Set_kid(idx, kid->Id() == cr ? new_cr : kid->Id());
      }
    } break;
    case CK_VAR:
    case CK_CONST:
      is_replaced = (Id() == cr);
      break;
    default:
      AIR_ASSERT_MSG(false, "TO IMPL:HCR::Replace_cr");
  }
  return is_replaced;
}

// returns true if all variable cr in current expression have the same version
// as phi sr, as we havn't created basic block, so replace bb with the scf stmt
bool HCR::Is_dominate(HSTMT_PTR sr) {
  AIR_ASSERT(!sr->Is_null() && sr->Is_scf());
  bool is_dom = true;
  switch (Kind()) {
    case CK_OP:
      for (uint32_t idx = 0; idx < Kid_cnt() && is_dom; idx++) {
        HCR_PTR kid = Kid(idx);
        is_dom &= kid->Is_dominate(sr);
      }
      break;
    case CK_VAR: {
      HSTMT_PTR def_stmt = Def_stmt();
      is_dom             = def_stmt->Is_dominate(sr);
    } break;
    case CK_CONST:
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected HCR::Is_dominate")
  }
  return is_dom;
}

// Check if expression have the same version
// recursive checking the child till var or const
bool HCR::Is_same_e_ver(HCR_PTR cr) {
  AIR_ASSERT(!cr->Is_null());
  bool is_same = true;
  switch (Kind()) {
    case CK_OP:
      if (Kid_cnt() != cr->Kid_cnt()) return false;
      for (uint32_t idx = 0; idx < Kid_cnt() && is_same; idx++) {
        HCR_PTR kid = Kid(idx);
        is_same &= kid->Is_same_e_ver(cr->Kid(idx));
      }
      break;
    case CK_VAR:
    case CK_CONST:
      is_same = (Id() == cr->Id());
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected HCR::Is_same_e_ver")
  }
  return is_same;
}

HSTMT_PTR HCR::Def_stmt(void) {
  AIR_ASSERT(Kind() == CK_VAR);
  HSTMT_ID         def_id = HSTMT_ID();
  VAR_HCR_DATA_PTR var_cr = Cast_to_var_cr();
  if (var_cr->Def_by_stmt()) {
    def_id = var_cr->Def_stmt();
  } else if (var_cr->Def_by_chi()) {
    def_id = Hssa_cont()->Chi_ptr(var_cr->Def_chi())->Stmt();
  } else if (var_cr->Def_by_phi()) {
    return Null_ptr;
  } else {
    //CMPLR_ASSERT(false, "TO IMPL");
    return Null_ptr;
  }
  return Hssa_cont()->Stmt_ptr(def_id);
}

BB_ID HCR::Def_bb() {
  AIR_ASSERT(Kind() == CK_VAR);
  VAR_HCR_DATA_PTR var_cr = Cast_to_var_cr();
  HPHI_PTR         phi    = Def_phi();
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

HPHI_PTR HCR::Def_phi() const {
  VAR_HCR_DATA_PTR var_cr = Cast_to_var_cr();
  if (var_cr->Def_by_phi()) {
    HPHI_ID phi = var_cr->Def_phi();
    return Hssa_cont()->Phi_ptr(phi);
  }
  return Null_ptr;
}

void HCR::Print_opcode(OPCODE opcode, std::ostream& os, uint32_t indent) {
  os << std::string(indent * INDENT_SPACE, ' ');
  if (opcode.Domain() > 0) {
    const char* domain_str = META_INFO::Domain_name(opcode.Domain());
    os << domain_str << ".";
  }
  const char* op_str = META_INFO::Op_name(opcode);
  os << op_str;
}

void HCR::Print(std::ostream& os, uint32_t indent) const {
  switch (_data->Kind()) {
    case CK_LDA: {
      LDA_HCR_DATA_PTR lda_cr = Static_cast<LDA_HCR_DATA_PTR>(_data);
      // lda_cr->Print(os, indent);
    } break;
    case CK_CONST: {
      CST_HCR_DATA_PTR cst_cr = Static_cast<CST_HCR_DATA_PTR>(_data);
      Print_opcode(cst_cr->Opcode(), os, indent);
      os << " cr" << Id().Value() << "(";
      cst_cr->Print(Hssa_cont(), os, indent);
      os << ")";
    } break;
    case CK_VAR: {
      VAR_HCR_DATA_PTR var_cr = Static_cast<VAR_HCR_DATA_PTR>(_data);
      var_cr->Print(Hssa_cont(), os, indent);
      os << "(cr" << Id().Value() << ")";
    }

    break;
    case CK_IVAR:
      AIR_ASSERT_MSG(false, "Not supported HCR kind %d", CK_IVAR);
      break;
    case CK_OP: {
      OP_HCR_DATA_PTR op_cr = Static_cast<OP_HCR_DATA_PTR>(_data);
      Print_opcode(op_cr->Opcode(), os, indent);
      os << "(cr" << Id().Value() << ")";
      op_cr->Print(Hssa_cont(), os, indent);
    } break;
    default:
      AIR_ASSERT_MSG(false, "Invalid HCR kind %d", _data->Kind());
  }
}

void HCR::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}
std::string HCR::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air
