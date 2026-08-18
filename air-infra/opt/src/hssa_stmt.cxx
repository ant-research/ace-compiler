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

#include "air/opt/bb.h"
#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"

using namespace air::base;

namespace air {

namespace opt {

void ASSIGN_HSTMT_DATA::Print(HSSA_CONTAINER* cont, std::ostream& os,
                              uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  HMU_ID  hmu  = Mu();
  HCHI_ID hchi = Chi();
  if (hmu != Null_id) {
    HMU_LIST hmu_list(cont, hmu);
    hmu_list.Print(os, indent);
  }

  cont->Cr_ptr(Lhs())->Print(os);
  os << std::endl;

  if (Rhs() != Null_id) cont->Cr_ptr(Rhs())->Print(os, indent + 1);

  if (hchi != Null_id) {
    os << std::endl;
    HCHI_LIST hchi_list(cont, hchi);
    hchi_list.Print(os, indent);
  }
}

DO_LOOP_HSTMT_DATA::DO_LOOP_HSTMT_DATA(air::base::NODE_PTR node, HSTMT_PTR init,
                                       HCR_PTR cond, HSTMT_PTR body,
                                       HCR_PTR incr, HPHI_PTR hphi)
    : DO_LOOP_HSTMT_DATA(node, init->Id(), cond->Id(), body->Id(), incr->Id()) {
}

void DO_LOOP_HSTMT_DATA::Print(HSSA_CONTAINER* cont, std::ostream& os,
                               uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  os << std::endl;
  HSTMT_LIST entry(cont, Entry());
  entry.Print(os, indent + 1);

  os << std::string((indent + 1) * INDENT_SPACE, ' ');
  os << "loop cond:" << std::endl;
  cont->Cr_ptr(Cond())->Print(os, indent + 2);

  // print body
  os << std::string((indent + 1) * INDENT_SPACE, ' ');
  os << "body:";
  HSTMT_LIST body(cont, Body());
  body.Print(os, indent + 2);
  os << std::string((indent + 1) * INDENT_SPACE, ' ');
  os << "end_body" << std::endl;

  // print incr
  cont->Cr_ptr(Incr())->Print(os, indent + 1);

  // print exit
  if (Exit() != HSTMT_ID()) {
    os << std::string(indent * INDENT_SPACE, ' ');
    os << "loop_exit:" << std::endl;
    HSTMT_LIST exit(cont, Exit());
    exit.Print(os, indent + 1);
    os << std::string(indent * INDENT_SPACE, ' ');
    os << "end_loop_exit" << std::endl;
  }

  os << std::string(indent * INDENT_SPACE, ' ');
  os << "end_do_loop" << std::endl;
}

void IF_HSTMT_DATA::Print(HSSA_CONTAINER* cont, std::ostream& os,
                          uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  CMPLR_ASSERT(false, "TODO: IF::Print");
}

void OP_HSTMT_DATA::Print(HSSA_CONTAINER* cont, std::ostream& os,
                          uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  // TO FIX
  if (Opcode() == air::core::FUNC_ENTRY) {
    return;
  }
  for (uint32_t i = 0; i < Kid_cnt(); i++) {
    HCR_PTR kid = cont->Cr_ptr(Kid(i));
    kid->Print(os, indent);
  }
}

void CALL_HSTMT_DATA::Print(HSSA_CONTAINER* cont, std::ostream& os,
                            uint32_t indent) const {
  HSTMT_DATA::Print(cont, os, indent);
  if (Retv() != HCR_ID()) {
    cont->Cr_ptr(Retv())->Print(os, indent);
  }

  for (uint32_t i = 0; i < Kid_cnt(); i++) {
    HCR_PTR kid = cont->Cr_ptr(Kid(i));
    kid->Print(os, indent + 1);
  }
}

BB_PTR HSTMT::Bb(CFG* cfg) { return cfg->Bb_ptr(Bb_id()); }
HCR_ID HSTMT::Lhs_id() const { return Cast_to_assign_sr()->Lhs(); }

HCR_PTR HSTMT::Lhs() const { return _cont->Cr_ptr(Lhs_id()); }

HCR_ID HSTMT::Rhs_id() const { return Cast_to_assign_sr()->Rhs(); }

HCR_PTR HSTMT::Rhs() const { return _cont->Cr_ptr(Rhs_id()); }

void HSTMT::Set_chi(HCHI_ID chi) {
  switch (Kind()) {
    case SK_ASSIGN:
      Cast_to_assign_sr()->Set_chi(chi);
      break;
    case SK_OP:
      Cast_to_op_sr()->Set_chi(chi);
      break;
    case SK_CALL:
      Cast_to_call_sr()->Set_chi(chi);
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected stmt kind for chi");
  }
}

HCHI_ID HSTMT::Chi() const {
  switch (Kind()) {
    case SK_ASSIGN:
      return Cast_to_assign_sr()->Chi();
    case SK_OP:
      return Cast_to_op_sr()->Chi();
    case SK_CALL:
      return Cast_to_call_sr()->Chi();
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected stmt kind for chi");
  }
  return HCHI_ID();
}

void HSTMT_DATA::Print(HSSA_CONTAINER* cont, std::ostream& os,
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
      Cast_to_assign_sr()->Print(_cont, os, indent);
      break;
    case SK_OP:
      Cast_to_op_sr()->Print(_cont, os, indent);
      break;
    case SK_DO_LOOP:
      Cast_to_do_loop_sr()->Print(_cont, os, indent);
      break;
    case SK_IF:
      Cast_to_if_sr()->Print(_cont, os, indent);
      break;
    case SK_CALL:
      Cast_to_call_sr()->Print(_cont, os, indent);
      break;
    default:
      AIR_ASSERT(false);
  }
}

bool HSTMT::Replace_cr(HCR_ID cr, HCR_ID new_cr) {
  bool is_replaced = false;
  switch (Kind()) {
    case SK_OP: {
      OP_HSTMT_DATA_PTR op_sr = Cast_to_op_sr();
      for (uint32_t idx = 0; idx < op_sr->Kid_cnt(); idx++) {
        HCR_ID  kid_id = op_sr->Kid(idx);
        HCR_PTR kid    = _cont->Cr_ptr(kid_id);
        is_replaced |= kid->Replace_cr(cr, new_cr);
        op_sr->Set_kid(idx, kid_id == cr ? new_cr : kid_id);
      }
      break;
    }
    case SK_ASSIGN: {
      HCR_PTR lhs = Lhs();
      HCR_PTR rhs = Rhs();
      is_replaced |= lhs->Replace_cr(cr, new_cr);
      is_replaced |= rhs->Replace_cr(cr, new_cr);
      Set_lhs(lhs->Id() == cr ? new_cr : lhs->Id());
      Set_rhs(rhs->Id() == cr ? new_cr : rhs->Id());
      break;
    }
    case SK_DO_LOOP: {
      DO_LOOP_HSTMT_DATA_PTR do_loop_sr = Cast_to_do_loop_sr();

      auto replace_cr = [](HSTMT_PTR stmt, HCR_ID cr, HCR_ID new_cr,
                           bool& is_replaced) {
        is_replaced |= stmt->Replace_cr(cr, new_cr);
      };
      HSTMT_LIST entry(_cont, do_loop_sr->Entry());
      entry.For_each(replace_cr, cr, new_cr, is_replaced);

      HCR_PTR cond = _cont->Cr_ptr(do_loop_sr->Cond());
      is_replaced |= cond->Replace_cr(cr, new_cr);

      HSTMT_LIST body(_cont, do_loop_sr->Body());
      body.For_each(replace_cr, cr, new_cr, is_replaced);

      HSTMT_LIST exit(_cont, do_loop_sr->Exit());
      exit.For_each(replace_cr, cr, new_cr, is_replaced);

      HCR_PTR incr = _cont->Cr_ptr(do_loop_sr->Incr());
      is_replaced |= incr->Replace_cr(cr, new_cr);
      break;
    }
    case SK_BLK_ENTRY:
      break;
    default:
      AIR_ASSERT_MSG(false, "Replace cr unsupported kind");
  }
  return is_replaced;
}

// this is an tempoary based on stmt order
bool HSTMT::Is_dominate(HSTMT_PTR stmt) {
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