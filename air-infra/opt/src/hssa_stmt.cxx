//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_stmt.h"

#if ENABLED_HSSA
#include <iomanip>
#include <sstream>

#include "air/opt/bb.h"
#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"

using namespace air::base;

namespace air {
namespace opt {

void ASSIGN_DATA::Print(HCONTAINER* cont, std::ostream& os,
                        uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  HMU_ID  hmu  = Mu();
  HCHI_ID hchi = Chi();

  // print mu list
  if (hmu != Null_id) {
    HMU_LIST hmu_list(cont, hmu);
    hmu_list.Print(os, indent);
  }

  // print left hand side
  cont->Expr_ptr(Lhs())->Print(os);
  os << std::endl;

  // print right hand side
  if (Rhs() != Null_id) cont->Expr_ptr(Rhs())->Print(os, indent + 1);

  // print chi list
  if (hchi != Null_id) {
    os << std::endl;
    HCHI_LIST hchi_list(cont, hchi);
    hchi_list.Print(os, indent);
  }
}

void NARY_DATA::Print(HCONTAINER* cont, std::ostream& os,
                      uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);

  if (Opcode() == air::core::FUNC_ENTRY) {
    AIR_ASSERT_MSG(false, "unsupported func entry");
    return;
  }
  for (uint32_t i = 0; i < Kid_cnt(); i++) {
    HEXPR_PTR kid = cont->Expr_ptr(Kid(i));
    kid->Print(os, indent);
  }
}

void CALL_DATA::Print(HCONTAINER* cont, std::ostream& os,
                      uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  if (Retv() != HEXPR_ID()) {
    cont->Expr_ptr(Retv())->Print(os, indent);
  }

  for (uint32_t i = 0; i < Arg_cnt(); i++) {
    HEXPR_PTR arg = cont->Expr_ptr(Arg(i));
    arg->Print(os, indent + 1);
  }
}

BB_PTR HSTMT::Bb() const { return Hssa_cont()->Cfg()->Bb_ptr(Bb_id()); }

HEXPR_ID HSTMT::Lhs_id() const { return Cast_to_assign()->Lhs(); }

HEXPR_PTR HSTMT::Lhs() const { return _cont->Expr_ptr(Lhs_id()); }

HEXPR_ID HSTMT::Rhs_id() const { return Cast_to_assign()->Rhs(); }

HEXPR_PTR HSTMT::Rhs() const { return _cont->Expr_ptr(Rhs_id()); }

void HSTMT::Set_chi(HCHI_ID chi) {
  switch (Kind()) {
    case SK_ASSIGN:
      Cast_to_assign()->Set_chi(chi);
      break;
    case SK_NARY:
      Cast_to_nary()->Set_chi(chi);
      break;
    case SK_CALL:
      Cast_to_call()->Set_chi(chi);
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected stmt kind for chi");
  }
}

HCHI_ID HSTMT::Chi() const {
  switch (Kind()) {
    case SK_ASSIGN:
      return Cast_to_assign()->Chi();
    case SK_NARY:
      return Cast_to_nary()->Chi();
    case SK_CALL:
      return Cast_to_call()->Chi();
    default:
      AIR_ASSERT_MSG(false, "unexpected stmt kind for chi");
  }
  return HCHI_ID();
}

void HSTMT_DATA::Print(HCONTAINER* cont, std::ostream& os,
                       uint32_t indent) const {
  OPCODE opcode = Opcode();
  if (opcode.Domain() > 0) {
    const char* domain_str = META_INFO::Domain_name(opcode.Domain());
    os << domain_str << ".";
  }
  const char* op_str = META_INFO::Op_name(opcode);
  os << op_str << " ";
}

void HSTMT::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  os << "[" << Id().Value() << "]";
  switch (Kind()) {
    case SK_ASSIGN:
      Cast_to_assign()->Print(_cont, os, indent);
      break;
    case SK_NARY:
      Cast_to_nary()->Print(_cont, os, indent);
      break;
    case SK_CALL:
      Cast_to_call()->Print(_cont, os, indent);
      break;
    default:
      AIR_ASSERT(false);
  }
}

bool HSTMT::Replace_expr(HEXPR_ID expr, HEXPR_ID new_expr) {
  bool is_replaced = false;
  switch (Kind()) {
    case SK_NARY: {
      NARY_DATA_PTR op_sr = Cast_to_nary();
      for (uint32_t idx = 0; idx < op_sr->Kid_cnt(); idx++) {
        HEXPR_ID  kid_id = op_sr->Kid(idx);
        HEXPR_PTR kid    = _cont->Expr_ptr(kid_id);
        is_replaced |= kid->Replace_expr(expr, new_expr);
        op_sr->Set_kid(idx, kid_id == expr ? new_expr : kid_id);
      }
      break;
    }
    case SK_ASSIGN: {
      HEXPR_PTR lhs = Lhs();
      HEXPR_PTR rhs = Rhs();
      is_replaced |= lhs->Replace_expr(expr, new_expr);
      is_replaced |= rhs->Replace_expr(expr, new_expr);
      Set_lhs(lhs->Id() == expr ? new_expr : lhs->Id());
      Set_rhs(rhs->Id() == expr ? new_expr : rhs->Id());
      break;
    }
    default:
      AIR_ASSERT_MSG(false, "Replace expression unsupported");
  }
  return is_replaced;
}

// this is an tempoary based on stmt order
bool HSTMT::Is_dominate(HSTMT_PTR stmt) const {
  AIR_ASSERT(false);
  return false;
}

void HSTMT::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HSTMT::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air

#endif