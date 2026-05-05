//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_stmt.h"

#include <iomanip>
#include <sstream>

#include "air/core/opcode.h"
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

DO_LOOP_DATA::DO_LOOP_DATA(air::base::NODE_PTR node, HSTMT_PTR init,
                           HEXPR_PTR cond, HSTMT_PTR body, HEXPR_PTR incr,
                           HPHI_PTR hphi)
    : HSTMT_DATA(node, SK_DO_LOOP) {
  AIR_ASSERT_MSG(node->Opcode() == air::core::OPC_DO_LOOP,
                 "DO_LOOP_DATA: node must be a do_loop");
  AIR_ASSERT_MSG(init != Null_ptr, "DO_LOOP_DATA: init stmt is required");
  AIR_ASSERT_MSG(cond != Null_ptr, "DO_LOOP_DATA: cond expr is required");
  AIR_ASSERT_MSG(body != Null_ptr, "DO_LOOP_DATA: body stmt is required");
  AIR_ASSERT_MSG(incr != Null_ptr, "DO_LOOP_DATA: incr expr is required");
  _entry = init->Id();
  _exit  = HSTMT_ID();
  _cond  = cond->Id();
  _body  = body->Id();
  _incr  = incr->Id();
}

void DO_LOOP_DATA::Print(HCONTAINER* cont, std::ostream& os,
                         uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  os << std::endl;
  os << std::string((indent + 1) * INDENT_SPACE, ' ')
     << "entry: " << Entry().Value();
  os << " cond: " << Cond().Value();
  os << " body: " << Body().Value();
  os << " incr: " << Incr().Value();
}

void IF_DATA::Print(HCONTAINER* cont, std::ostream& os, uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  os << std::endl;
  os << std::string((indent + 1) * INDENT_SPACE, ' ')
     << "cond: " << Cond().Value();
}

BB_PTR HSTMT::Bb(CFG* cfg) const { return cfg->Bb_ptr(Bb_id()); }

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

HMU_ID HSTMT::Mu(void) const {
  switch (Kind()) {
    case SK_ASSIGN:
      return Cast_to_assign()->Mu();
    case SK_NARY:
      return Cast_to_nary()->Mu();
    case SK_CALL:
      return Cast_to_call()->Mu();
    default:
      AIR_ASSERT_MSG(false, "unexpected stmt kind for mu");
  }
  return HMU_ID();
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
    case SK_DO_LOOP:
      Cast_to_do_loop()->Print(_cont, os, indent);
      break;
    case SK_IF:
      Cast_to_if()->Print(_cont, os, indent);
      break;
    case SK_BLK_ENTRY:
      _data->Print(_cont, os, indent);
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
    case SK_CALL: {
      CALL_DATA_PTR call = Cast_to_call();
      for (uint32_t idx = 0; idx < call->Arg_cnt(); idx++) {
        HEXPR_ID  arg_id = call->Arg(idx);
        HEXPR_PTR arg    = _cont->Expr_ptr(arg_id);
        is_replaced |= arg->Replace_expr(expr, new_expr);
        call->Set_arg(idx, arg_id == expr ? new_expr : arg_id);
      }
      break;
    }
    case SK_DO_LOOP:
    case SK_BLK_ENTRY:
      break;
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