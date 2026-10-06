//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "air/opt/pre_container.h"

#include "air/opt/pre_decl.h"

using namespace air::base;

namespace air {

namespace opt {

uint32_t PRE_CONTAINER::Hash_exp(HEXPR_PTR expr) {
  uint32_t hash_idx = expr->Hash_idx();
  return hash_idx % Htable_size();
}

PRE_CAND_PTR PRE_CONTAINER::New_pre_cand(HEXPR_PTR expr) {
  PRE_CAND_DATA_PTR data_ptr = _cand_tab->Allocate<PRE_CAND_DATA>();
  new (data_ptr) PRE_CAND_DATA(expr);
  PRE_CAND_PTR cand_ptr = PRE_CAND_PTR(PRE_CAND(this, data_ptr));
  return cand_ptr;
}

OCC_PTR PRE_CONTAINER::New_real_occ(HEXPR_PTR expr, HSTMT_PTR stmt) {
  REAL_OCC_DATA_PTR data_ptr = _occ_tab->Allocate<REAL_OCC_DATA>();
  new (data_ptr) REAL_OCC_DATA(expr, stmt);
  OCC_PTR occ_ptr = OCC_PTR(OCC(this, data_ptr));
  return occ_ptr;
}

void PRE_CONTAINER::Add_to_htable(uint32_t hash_idx, PRE_CAND_PTR cand) {
  PRE_CAND_ID bucket_head = _cand_htable[hash_idx];
  if (bucket_head == PRE_CAND_ID()) {
    _cand_htable[hash_idx] = cand->Id();
  } else {
    PRE_CAND_LIST cand_list(this, bucket_head);
    cand_list.Append(cand->Id());
  }
}

void PRE_CONTAINER::Add_to_worklist(PRE_CAND_PTR cand) {
  _worklist.push_back(cand->Id());
}

PRE_CAND_PTR PRE_CONTAINER::Find_or_new_pre_cand(HEXPR_PTR expr) {
  PRE_CAND_DATA     data(expr);
  PRE_CAND_DATA_PTR data_ptr(&data, PRE_CAND_ID());
  PRE_CAND_PTR      cand_ptr(PRE_CAND(this, data_ptr));

  uint32_t hash_idx = Hash_exp(expr);
  AIR_ASSERT(hash_idx < Htable_size());
  PRE_CAND_ID   bucket_head = _cand_htable[hash_idx];
  PRE_CAND_LIST cand_list(this, bucket_head);
  PRE_CAND_PTR  cand = cand_list.Find(cand_ptr);
  if (cand->Is_null()) {
    cand = New_pre_cand(expr);
    Add_to_htable(hash_idx, cand);
    Add_to_worklist(cand);
  }
  // TO DO: handle second order effect
  return cand;
}

OCC_PTR PRE_CONTAINER::Append_real_occ(HEXPR_PTR expr, HSTMT_PTR stmt) {
  PRE_CAND_PTR pre_cand = Find_or_new_pre_cand(expr);
  AIR_ASSERT(!pre_cand->Is_null());

  OCC_PTR exp_occ = New_real_occ(expr, stmt);
  pre_cand->Append_occ(exp_occ);
  return exp_occ;
}

void PRE_CONTAINER::Append_phi_occ(HEXPR_PTR expr, BB_PTR bb,
                                   uint32_t num_opnd) {
  PRE_CAND_PTR pre_cand = Find_or_new_pre_cand(expr);
  AIR_ASSERT(!pre_cand->Is_null());

  PHI_OCC_DATA_PTR data_ptr = Static_cast<PHI_OCC_DATA_PTR>(
      _occ_tab->Malloc(PHI_OCC_DATA::Size(num_opnd)));
  new (data_ptr) PHI_OCC_DATA(expr, bb, num_opnd);
  OCC_PTR phi_occ = OCC_PTR(OCC(this, data_ptr));
  pre_cand->Append_occ(phi_occ);
}

void PRE_CONTAINER::Append_phi_opnd_occ(OCC_PTR phi_occ, uint32_t phi_idx) {
  AIR_ASSERT(phi_occ->Kind() == OCC_PHI);

  HEXPR_PTR    expr     = phi_occ->Expr();
  BB_PTR       bb       = phi_occ->Bb();
  BB_PTR       pred_bb  = bb->Pred(phi_idx);
  PRE_CAND_PTR pre_cand = Find_or_new_pre_cand(expr);
  AIR_ASSERT(!pre_cand->Is_null());

  PHI_OPND_OCC_DATA_PTR data_ptr = _occ_tab->Allocate<PHI_OPND_OCC_DATA>();
  new (data_ptr) PHI_OPND_OCC_DATA(expr, pred_bb);
  data_ptr->Set_owning_phi_occ(phi_occ->Id());
  phi_occ->Cast_to_phi_occ()->Set_opnd(phi_idx, data_ptr.Id());
  OCC_PTR opnd_occ = OCC_PTR(OCC(this, data_ptr));
  pre_cand->Append_occ(opnd_occ);
}

#if 0
void PRE_CONTAINER::Insert_new_operand(OCC_PTR phi_opnd_occ) {
  AIR_ASSERT(phi_occ->Kind() == OCC_PHI);
  AIR_ASSERT(phi_opnd_occ->Kind() == OCC_PHI_OPND);

  PHI_OPND_OCC_DATA_PTR opnd_occ = phi_opnd_occ->Cast_to_phi_opnd_occ();
  HEXPR_ID insert_expr = opnd_occ->Cur_ver_expr();

  opnd_occ->Set_inserted();
  opnd_occ->Set_save();
}
#endif

void PRE_CONTAINER::Print_worklist(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  os << "PRE working list:" << std::endl;
  const WORKLIST* wl = Worklist();
  for (size_t idx = 0; idx < wl->size(); idx++) {
    os << "CAND[" << idx << "]";
    PRE_CAND_ID  cand_id = wl->at(idx);
    PRE_CAND_PTR cand    = Pre_cand_ptr(cand_id);
    cand->Print(os, indent + 2);
  }
}

void PRE_CONTAINER::Print_worklist() const { Print_worklist(std::cout, 0); }

}  // namespace opt
}  // namespace air
