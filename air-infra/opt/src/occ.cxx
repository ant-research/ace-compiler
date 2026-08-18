//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/occ.h"

#include <sstream>

#include "air/opt/cfg.h"
#include "air/opt/pre_container.h"

using namespace air::base;

namespace air {
namespace opt {

void OCC_DATA::Set_def(OCC_PTR def) { _def = def->Id(); }

void OCC_DATA::Print(PRE_CONTAINER* pre_cont, std::ostream& os,
                     uint32_t indent) const {
  os << " kind: " << Kind() << " cr: " << Coderep_id().Value();
  os << " ver:" << _e_ver;
  os << " bb:" << Bb_id().Value();
}

void PHI_OCC_DATA::Print_flag(std::ostream& os) const {
  if (_flags == PHI_OCC_NONE) return;
  os << " flags:[";
  if (Is_set_flag(PHI_OCC_IS_DOWN_SAFE)) os << "downsafe ";
  if (Is_set_flag(PHI_OCC_IS_LIVE)) os << "live ";
  if (Is_set_flag(PHI_OCC_CANT_BE_AVAIL)) os << "can_be_avail ";
  if (Is_set_flag(PHI_OCC_STOPS)) os << "stops ";
  if (Is_set_flag(PHI_OCC_IDENTITY)) os << "identity ";
  if (Is_set_flag(PHI_OCC_IDENT_INJURED)) os << "injured ";
  os << "]";
}

void PHI_OCC_DATA::Print(std::ostream& os, uint32_t indent) const {
  Print_flag(os);
  os << " opnds:[";
  for (uint32_t idx = 0; idx < Num_opnds(); idx++) {
    os << Opnd(idx).Value() << " ";
  }
  os << "]";
}

void PHI_OPND_OCC_DATA::Print_flag(std::ostream& os) const {
  if (_flags == PHI_OPND_OCC_NONE) return;
  os << " flags:[";
  if (Is_set_flag(PHI_OPND_OCC_IS_SAVE)) os << "saved ";
  if (Is_set_flag(PHI_OPND_OCC_IS_INSERTED)) os << "inserted ";
  if (Is_set_flag(PHI_OPND_OCC_HAS_REAL_USE)) os << "has_real_use ";
  if (Is_set_flag(PHI_OPND_OCC_IS_PROCESSED)) os << "processed ";
  os << "]";
}

void PHI_OPND_OCC_DATA::Print(std::ostream& os, uint32_t indent) const {
  Print_flag(os);
  os << " phi:" << _phi_occ.Value();
  if (_saved_cr != HCR_ID()) {
    os << " saved_expr:" << _saved_cr.Value();
  }
}

void REAL_OCC_DATA::Print_flag(std::ostream& os) const {
  if (_flags == ROCC_NONE) return;
  os << " flags:[";
  if (Is_set_flag(ROCC_IS_RELOAD)) os << "reload ";
  if (Is_set_flag(ROCC_IS_SAVE)) os << "saved ";
  if (Is_set_flag(ROCC_IS_LHS)) os << "lhs ";
  if (Is_set_flag(ROCC_IS_FORMAL)) os << "formal ";
  os << "]";
}

void REAL_OCC_DATA::Print(std::ostream& os, uint32_t indent) const {
  Print_flag(os);
  if (_saved_cr != HCR_ID()) {
    os << " saved_expr:" << _saved_cr.Value();
  }
  HSTMT_ID stmt_id = Stmt_id();
  os << " stmt:" << ((stmt_id == HSTMT_ID()) ? -1 : stmt_id.Value());
}

CFG& OCC::Cfg(void) const { return Cont()->Cfg(); }

bool OCC::Has_next(void) const { return Next_id() != OCC_ID(); }

OCC_PTR OCC::Next(void) const {
  return (Has_next() ? _cont->Occ_ptr(Next_id()) : Null_ptr);
}

HCR_PTR OCC::Coderep(void) const {
  return _cont->Hssa_cont().Cr_ptr(_data->Coderep_id());
}

HSTMT_PTR OCC::Stmtrep(void) const {
  AIR_ASSERT(Kind() == OCC_REAL_OCCUR);
  HSTMT_ID stmt_id = Cast_to_real_occ()->Stmt_id();
  return _cont->Hssa_cont().Stmt_ptr(stmt_id);
}

BB_PTR OCC::Bb(void) const {
  return Bb_id() == BB_ID() ? Null_ptr : Cfg().Bb_ptr(Bb_id());
}

void OCC::Set_save() {
  switch (Kind()) {
    case OCC_REAL_OCCUR:
      Cast_to_real_occ()->Set_save();
      break;
    case OCC_PHI_OCCUR: {
      PHI_OCC_DATA_PTR phi_occ = Cast_to_phi_occ();
      for (size_t idx = 0; idx < phi_occ->Num_opnds(); idx++) {
        OCC_ID  opnd_id = phi_occ->Opnd(idx);
        OCC_PTR opnd    = Cont()->Occ_ptr(opnd_id);
        if (!opnd->Cast_to_phi_opnd_occ()->Is_processed()) {
          opnd->Cast_to_phi_opnd_occ()->Set_is_processed();
          opnd->Def()->Set_save();
        }
      }
    } break;
    default:
      CMPLR_ASSERT(false, "TO IMPL OCC::Set_save");
  }
}

void OCC::Set_saved_cr(HCR_PTR cr) {
  AIR_ASSERT(cr->Kind() == CK_VAR);
  switch (Kind()) {
    case OCC_REAL_OCCUR:
      Cast_to_real_occ()->Set_saved_cr(cr->Id());
      break;
    case OCC_PHI_OPND_OCCUR:
      Cast_to_phi_opnd_occ()->Set_saved_cr(cr->Id());
      break;
    default:
      ;
      //CMPLR_ASSERT(false, "TO IMPL OCC::Set_save");
  }
}

OCC_PTR OCC::Def(void) const {
  return ((_data->Def_id() != OCC_ID()) ? Cont()->Occ_ptr(_data->Def_id())
                                        : Null_ptr);
}
bool OCC::Is_dpo_less_than(OCC_PTR other) {
  BB_ID     bb_id       = Bb()->Id();
  BB_ID     other_bb_id = other->Bb()->Id();
  DOM_INFO& dom_info    = Cfg().Dom_info();
  return dom_info.Get_dom_tree_pre_idx(bb_id) <
         dom_info.Get_dom_tree_pre_idx(other_bb_id);
}

void OCC::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  os << "[" << Id().Value() << "]";
  _data->Print(Cont(), os, indent);
  switch (Kind()) {
    case OCC_REAL_OCCUR:
      Cast_to_real_occ()->Print(os, indent);
      break;
    case OCC_PHI_OCCUR:
      Cast_to_phi_occ()->Print(os, indent);
      break;
    case OCC_PHI_OPND_OCCUR:
      Cast_to_phi_opnd_occ()->Print(os, indent);
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpeted occ kind");
  }
}

void OCC::Print() const { Print(std::cout, 0); }

std::string OCC::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

CFG&    PRE_CAND::Cfg(void) const { return Cont()->Cfg(); }
HCR_PTR PRE_CAND::Coderep(void) const {
  return _pre_cont->Hssa_cont().Cr_ptr(Coderep_id());
}

OCC_PTR PRE_CAND::Real_occs(void) {
  if (_data->Real_occs_id() != OCC_ID()) {
    return _pre_cont->Occ_ptr(_data->Real_occs_id());
  } else {
    return air::base::Null_ptr;
  }
}
OCC_PTR PRE_CAND::Phi_occs(void) {
  if (_data->Phi_occs_id() != OCC_ID()) {
    return _pre_cont->Occ_ptr(_data->Phi_occs_id());
  } else {
    return air::base::Null_ptr;
  }
}

OCC_PTR PRE_CAND::Phi_opnd_occs(void) {
  if (_data->Phi_opnd_occs_id() != OCC_ID()) {
    return _pre_cont->Occ_ptr(_data->Phi_opnd_occs_id());
  } else {
    return air::base::Null_ptr;
  }
}

void PRE_CAND_DATA::Append_occ(PRE_CONTAINER* pre_cont, OCC_PTR occ) {
  OCC_ID head = OCC_ID();
  switch (occ->Kind()) {
    case OCC_REAL_OCCUR: {
      if (_real_occ_head == OCC_ID())
        _real_occ_head = occ->Id();
      else
        head = _real_occ_head;
    } break;
    case OCC_PHI_OCCUR: {
      if (_phi_occ_head == OCC_ID())
        _phi_occ_head = occ->Id();
      else
        head = _phi_occ_head;
    } break;
    case OCC_PHI_OPND_OCCUR: {
      if (_phi_opnd_occ_head == OCC_ID())
        _phi_opnd_occ_head = occ->Id();
      else
        head = _phi_opnd_occ_head;
    } break;
    default:
      AIR_ASSERT(false);
  }

  OCC_LIST occ_list(pre_cont, head);
  CMPLR_ASSERT(Check_dpo_order(occ_list, occ),
               "PRE_CAND_DATA::Append_occ: violates dpo order");
  occ_list.Append(occ->Id());
}

bool PRE_CAND_DATA::Match(PRE_CONTAINER* pre_cont, PRE_CAND_PTR other) const {
  HSSA_CONTAINER& hssa_cont = pre_cont->Hssa_cont();
  HCR_PTR         cr        = hssa_cont.Cr_ptr(Coderep_id());
  HCR_PTR         other_cr  = hssa_cont.Cr_ptr(other->Coderep_id());
  return cr->Match_lex(other_cr);
}

bool PRE_CAND::Match(PRE_CAND_PTR other) const {
  return Data()->Match(Cont(), other);
}

void PRE_CAND_DATA::Print(PRE_CONTAINER* pre_cont, std::ostream& os,
                          uint32_t indent) const {

  OCC_LIST real_occ_list(pre_cont, Real_occs_id());
  if (real_occ_list.Size() < 2) {
    return;
  }
  os << std::string(indent * INDENT_SPACE, ' ');
  HCR_PTR cr = pre_cont->Hssa_cont().Cr_ptr(Coderep_id());
  cr->Print(os, indent);

  if (Real_occs_id() != OCC_ID()) {
    os << std::endl;
    os << std::string(indent * INDENT_SPACE, ' ');
    os << "Real occs:" << std::endl;
    OCC_LIST real_occ_list(pre_cont, Real_occs_id());
    real_occ_list.Print(os, indent + 1);
  }

  if (Phi_occs_id() != OCC_ID()) {
    os << std::string(indent * INDENT_SPACE, ' ');
    os << "Phi occs:" << std::endl;
    OCC_LIST phi_occ_list(pre_cont, Phi_occs_id());
    phi_occ_list.Print(os, indent + 1);
  }

  if (Phi_opnd_occs_id() != OCC_ID()) {
    os << std::string(indent * INDENT_SPACE, ' ');
    os << "Phi opnd occs:" << std::endl;
    OCC_LIST phi_opnd_occ_list(pre_cont, Phi_opnd_occs_id());
    phi_opnd_occ_list.Print(os, indent + 1);
  }
}

void PRE_CAND::Print(std::ostream& os, uint32_t indent) const {
  Data()->Print(Cont(), os, indent);
  os << std::endl;
}

void PRE_CAND::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string PRE_CAND::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}
}  // namespace opt
}  // namespace air
