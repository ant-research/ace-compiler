//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "air/opt/ssapre.h"

#include "air/opt/bb.h"
#include "air/opt/cfg.h"
#include "air/opt/hssa_analyze_ctx.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_visitor.h"
#include "air/opt/pre_container.h"
using namespace air::base;

namespace air {
namespace opt {
class WL_BUILDER_CTX : public HSSA_ANALYZE_CTX {
public:
  WL_BUILDER_CTX(PRE_CONTAINER& pre_cont)
      : HSSA_ANALYZE_CTX(pre_cont.Cfg()), _pre_cont(pre_cont) {}

  PRE_CONTAINER& Pre_cont() const { return _pre_cont; }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    if (node->Kind() == EK_OP) {
      Pre_cont().Append_real_occ(node, Parent_stmt());
    }
    HSSA_ANALYZE_CTX::Post_handle_expr<RETV, VISITOR>(visitor, node);
    return RETV();
  }

private:
  PRE_CONTAINER& _pre_cont;
};

void SSAPRE::Run() {
  Trace(TRACE_IR_BEFORE_PRE, "\n=========HIR before SSAPRE======\n");
  Trace_obj(TRACE_IR_BEFORE_PRE, &Cfg());
  Trace(TRACE_PRE_FLOW, "\n@@Step 1: Collect worklist\n");
  Create_worklist();

  const WORKLIST* wl = Pre_cont().Worklist();
  for (size_t idx = 0; idx < wl->size(); idx++) {
    PRE_CAND_ID  cand_id = wl->at(idx);
    PRE_CAND_PTR cand    = Pre_cont().Pre_cand_ptr(cand_id);
    Init_cand(cand);
    if (Is_skip_cand(cand)) continue;
    Trace(TRACE_PRE_FLOW, "\n-----Processing [", idx, "]th candidate------\n");
    Trace_obj(TRACE_PRE_FLOW, cand);

    Insert_phis();
    // Skip if occurance less than 2
    if (Cur_all_occs().size() < 2) {
      continue;
    }

    Trace(TRACE_PRE_FLOW, "\n@@Step 3: Rename\n");
    Rename();
    Trace_obj(TRACE_PRE_FLOW, cand);

    Trace(TRACE_PRE_FLOW, "\n@@Step 5: Finalize\n");
    Finalize();
  }

  Trace(TRACE_IR_AFTER_PRE, "\n=========HIR after SSAPRE======\n");
  Trace_obj(TRACE_IR_AFTER_PRE, &Cfg());
}

// Step 1: Create worklist
void SSAPRE::Create_worklist() {
  PRE_CONTAINER&               pre_cont = Pre_cont();
  CFG&                         cfg      = Cfg();
  WL_BUILDER_CTX               ctx(pre_cont);
  HSSA_VISITOR<WL_BUILDER_CTX> wk_builder(ctx, TOR_DOM);
  wk_builder.Trav<void>(cfg.Entry_bb());
  // pre_cont.Print_worklist();
}

void SSAPRE::Init_cand(PRE_CAND_PTR cand) {
  Set_cur_cand(cand);
  Set_cur_tmp(air::base::Null_ptr);
  Init_cur_ver();
  Var_phi_set().clear();
  Df_phi_set().clear();
  Cur_all_occs().clear();
}

// a fast turn around to skip candidates if the expression's opnds is not saved
// to tmp
bool SSAPRE::Is_skip_cand(PRE_CAND_PTR cand) {
  HEXPR_PTR expr = cand->Expr();
#if 0
  if (expr->Kind() == EK_OP) {
    OP_DATA_PTR op_expr = expr->Cast_to_op_expr();
    for (size_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
      if (Hssa_cont().Expr_ptr(op_expr->Kid(idx))->Kind() != EK_VAR) return true;
    }
  }
#endif
  if (!Pre_config().Is_cand_op(expr->Opcode())) {
    return true;
  }
  return false;
}

// Step 2: Insert phi
void SSAPRE::Insert_phis() {
  Trace(TRACE_PRE_FLOW, "\n@@Step 2: Insert phi\n");
  Collect_phi_sets();
  Create_phi_occs();
  Create_phi_opnd_occs();
  Build_all_occs();

  Trace(TRACE_PRE_FLOW, "\nAfter Insert phi\n");
  Trace_obj(TRACE_PRE_FLOW, Cur_cand());
  Trace_cmd(TRACE_PRE_FLOW, SSAPRE::Print_all_occs, Pre_cont(), Cur_all_occs());
}

void SSAPRE::Collect_phi_sets() {
  PRE_CONTAINER& pre_cont = Pre_cont();
  PRE_CAND_PTR   pre_cand = Cur_cand();
  AIR_ASSERT(!pre_cand->Is_null());
  BBID_CSET& var_phi_set = Var_phi_set();
  BBID_CSET& df_phi_set  = Df_phi_set();
  var_phi_set.clear();
  df_phi_set.clear();

  OCC_LIST real_occs = OCC_LIST(&Pre_cont(), pre_cand->Real_occs_id());

  auto insert_phi_for_expr = [](OCC_PTR real_occ, SSAPRE* ssapre) {
    HEXPR_PTR expr = real_occ->Expr();
    AIR_ASSERT(expr->Kind() == EK_OP);
    for (uint32_t idx = 0; idx < expr->Kid_cnt(); idx++) {
      ssapre->Gen_var_phi_list(expr->Kid(idx));
    }
    ssapre->Get_domfrontier(real_occ->Bb(), ssapre->Df_phi_set());
  };
  real_occs.For_each(insert_phi_for_expr, this);

  BBID_SET tmp_set(var_phi_set.begin(), var_phi_set.end());
  for (BB_ID id : tmp_set) {
    Get_domfrontier(Cfg().Bb_ptr(id), var_phi_set);
  }

  // merge the two sets
  df_phi_set.insert(var_phi_set.begin(), var_phi_set.end());
}

void SSAPRE::Gen_var_phi_list(HEXPR_PTR expr) {
  BBID_CSET&  var_phi_set = Var_phi_set();
  HCONTAINER& hssa_cont   = Hssa_cont();
  if (expr->Kind() != EK_VAR) return;
  VAR_DATA_PTR var_expr = expr->Cast_to_var_expr();
  if (var_expr->Def_by_phi()) {
    HPHI_PTR phi = hssa_cont.Phi_ptr(var_expr->Def_phi());
    if (var_phi_set.find(phi->Bb_id()) == var_phi_set.end()) {
      var_phi_set.insert(phi->Bb_id());
      for (uint32_t idx = 0; idx < phi->Size(); idx++) {
        HEXPR_PTR opnd = hssa_cont.Expr_ptr(phi->Opnd_id(idx));
        Gen_var_phi_list(opnd);
      }
    }
  }
}

void SSAPRE::Get_domfrontier(BB_PTR bb, BBID_CSET& bb_set) {
  AIR_ASSERT(!bb->Is_null());
  for (auto df_id : Dom_info().Get_iter_dom_frontiers(bb)) {
    bb_set.insert(df_id);
  }
}

void SSAPRE::Create_phi_occs() {
  PRE_CONTAINER& pre_cont = Pre_cont();
  for (BB_ID id : Df_phi_set()) {
    BB_PTR bb = Cfg().Bb_ptr(id);
    pre_cont.Append_phi_occ(Cur_cand()->Expr(), bb, 2);
  }
}

void SSAPRE::Create_phi_opnd_occs() {
  PRE_CAND_PTR pre_cand = Cur_cand();
  OCC_LIST     phi_occs = OCC_LIST(&Pre_cont(), pre_cand->Phi_occs_id());

  auto create_phi_opnd_occs = [](OCC_PTR occ, SSAPRE* ssapre) {
    PHI_OCC_DATA_PTR phi_occ = occ->Cast_to_phi_occ();
    for (uint32_t phi_idx = 0; phi_idx < phi_occ->Num_opnds(); phi_idx++) {
      ssapre->Pre_cont().Append_phi_opnd_occ(occ, phi_idx);
    }
  };
  phi_occs.For_each(create_phi_opnd_occs, this);
}

// all occ are composed by PHI_OCC, REAL_OCC and PHI_OPND_OCC
// Insertion follow order: phi->real->phi_opnd
void SSAPRE::Build_all_occs() {
  PRE_CONTAINER& pre_cont = Pre_cont();
  PRE_CAND_PTR   pre_cand = Cur_cand();

  OCC_PTR phi_occ      = pre_cand->Phi_occs();
  OCC_PTR real_occ     = pre_cand->Real_occs();
  OCC_PTR phi_opnd_occ = pre_cand->Phi_opnd_occs();

  OCC_PTR picked_occ = phi_occ;

  do {
    picked_occ = phi_occ;
    if (picked_occ->Is_null() ||
        (!real_occ->Is_null() && real_occ->Is_dpo_less_than(picked_occ))) {
      picked_occ = real_occ;
    }
    if (picked_occ->Is_null() || (!phi_opnd_occ->Is_null() &&
                                  phi_opnd_occ->Is_dpo_less_than(picked_occ))) {
      picked_occ = phi_opnd_occ;
    }
    if (!picked_occ->Is_null()) {
      Append_all_occ(picked_occ);
      switch (picked_occ->Kind()) {
        case OCC_REAL:
          real_occ = real_occ->Next();
          break;
        case OCC_PHI:
          phi_occ = phi_occ->Next();
          break;
        case OCC_PHI_OPND:
          phi_opnd_occ = phi_opnd_occ->Next();
          break;
        default:
          AIR_ASSERT(false);
      }
    }
  } while (picked_occ != Null_ptr);
}

// Step 3: Rename
void SSAPRE::Rename() {
  std::stack<OCC_PTR> occ_stack;
  OCC_PTR_SET         pending_occs;
  Init_cur_ver();

  // Pass 1: connect use-def of real occ, phi occ, phi opnd occ
  for (OCC_ID occ_id : Cur_all_occs()) {
    OCC_PTR occ = Pre_cont().Occ_ptr(occ_id);
    while (!occ_stack.empty() && !occ_stack.top()->Dominates(occ)) {
      occ_stack.pop();
    }
    switch (occ->Kind()) {
      case OCC_REAL:
        Rename_for_real_occ(occ, occ_stack, pending_occs);
        break;
      case OCC_PHI:
        Create_new_version(occ, occ_stack);
        break;
      case OCC_PHI_OPND: {
        if (occ_stack.empty()) {
          occ->Set_def(Null_ptr);
        } else {
          OCC_PTR top_occ = occ_stack.top();
          occ->Set_def(top_occ);
          occ->Set_ver(top_occ->Ver());
          if (top_occ->Kind() == OCC_REAL) {
            occ->Cast_to_phi_opnd_occ()->Set_has_real_use();
          }
        }
      } break;
      default:
        AIR_ASSERT(false);
    }
  }

  // Pass 2: process pending occs, rename for phi opnd and phi result
  while (!pending_occs.empty()) {
    OCC_PTR_SET::iterator it  = pending_occs.begin();
    OCC_PTR               occ = *it;
    pending_occs.erase(it);
    OCC_PTR def_occ = occ->Def();
    AIR_ASSERT(occ->Kind() == OCC_REAL);
    AIR_ASSERT(def_occ != Null_ptr);
    PHI_OCC_DATA_PTR phi_occ = def_occ->Cast_to_phi_occ();
    for (uint32_t idx = 0; idx < phi_occ->Num_opnds(); idx++) {
      OCC_PTR opnd = Pre_cont().Occ_ptr(phi_occ->Opnd(idx));
      Rename_for_phi_opnd(opnd, occ, pending_occs, idx);
    }
  }
}

// This is a quick fixing of wrong PRE of case conv2d.onnx
// LOOP1
//   cr3 = phi(cr0, cr1)
//   LOOP2
//   {cr1 <- }
//   PRECOMP(cr3) is moved to LOOP2 head, as SSA think cr3 same version
//   as LOOP2 result.
//   there need an extra phi before PRECOMP, in current design
//   we only generate one phi for each phi,
bool SSAPRE::Need_fixing_phi(OCC_PTR def, OCC_PTR use) {
  AIR_ASSERT(def->Kind() == OCC_PHI);
  HEXPR_PTR use_expr = use->Expr();
  AIR_ASSERT(!use_expr->Is_null());
  BB_PTR        def_bb   = def->Bb();
  LOOP_INFO_PTR def_loop = def_bb->Loop_info();
  LOOP_INFO_PTR use_loop = use->Bb()->Loop_info();
  if (def_loop == use_loop && use->Bb()->Kind() == BB_LOOP_EXIT) {
    return true;
  }
  return false;
}

void SSAPRE::Rename_for_real_occ(OCC_PTR& occ, std::stack<OCC_PTR>& occ_stack,
                                 OCC_PTR_SET& pending_occs) {
  AIR_ASSERT(occ->Kind() == OCC_REAL);

  if (occ_stack.empty()) {
    Create_new_version(occ, occ_stack);
    return;
  }
  OCC_PTR top_occ = occ_stack.top();
  switch (top_occ->Kind()) {
    case OCC_REAL: {
      if (Is_same_ver_for_real_def(top_occ, occ)) {
        occ->Set_ver(top_occ->Ver());
        occ->Set_def(top_occ->Def()->Is_null() ? top_occ : top_occ->Def());
      } else {
        Create_new_version(occ, occ_stack);
      }
    } break;
    case OCC_PHI: {
      if (Is_same_ver_for_phi_def(top_occ, occ)) {
        occ->Set_ver(top_occ->Ver());
        occ->Set_def(top_occ);
        occ_stack.push(occ);
        pending_occs.insert(occ);
        if (!Need_fixing_phi(top_occ, occ)) {
          top_occ->Cast_to_phi_occ()->Set_is_down_safe();
        } else {
          top_occ->Cast_to_phi_occ()->Reset_is_down_safe();
        }
      } else {
        top_occ->Cast_to_phi_occ()->Reset_is_down_safe();
        Create_new_version(occ, occ_stack);
      }
    } break;
    default:
      AIR_ASSERT_MSG(false, "unexpected OCC kind:", top_occ->Kind());
  }
}

// Rename for phi opnd at phi index
void SSAPRE::Rename_for_phi_opnd(OCC_PTR& occ, OCC_PTR real_occ,
                                 OCC_PTR_SET& pending_occs, uint32_t phi_idx) {
  PHI_OPND_OCC_DATA_PTR phi_opnd = occ->Cast_to_phi_opnd_occ();
  if (phi_opnd->Is_processed()) return;
  phi_opnd->Set_is_processed();

  HEXPR_PTR cur_ver_expr = Phi_opnd_with_cur_ver(real_occ, phi_idx);
  occ->Set_expr(cur_ver_expr);
  OCC_PTR opnd_def = occ->Def();
  if (opnd_def == Null_ptr) return;

  switch (opnd_def->Kind()) {
    case OCC_REAL:
      if (!Is_same_ver_for_real_def(opnd_def, occ)) {
        phi_opnd->Set_def(Null_ptr);
        phi_opnd->Reset_has_real_use();
        phi_opnd->Set_ver(Cur_ver());
        Inc_cur_ver();
        // temp set to avoid redef of phi opnd
        Pre_cont()
            .Occ_ptr(phi_opnd->Owning_phi_occ())
            ->Cast_to_phi_occ()
            ->Reset_is_down_safe();
      }
      break;
    case OCC_PHI:
      if (Is_same_ver_for_phi_def(opnd_def, occ)) {
        // OCC_PTR new_occ = Pre_cont().Append_real_occ(cur_ver_expr, Null_ptr);
        OCC_PTR new_occ = Pre_cont().New_real_occ(cur_ver_expr, Null_ptr);
        new_occ->Set_def(opnd_def);
        new_occ->Set_ver(opnd_def->Ver());
        new_occ->Set_bb(occ->Bb());
        pending_occs.insert(new_occ);
      } else {
        phi_opnd->Set_def(Null_ptr);
        phi_opnd->Set_has_real_use();
        opnd_def->Cast_to_phi_occ()->Reset_is_down_safe();
      }
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected opnd def kind ", opnd_def->Kind());
  }
}

bool SSAPRE::Is_same_ver_for_real_def(OCC_PTR def, OCC_PTR use) {
  AIR_ASSERT(def->Kind() == OCC_REAL);

  if (use->Kind() == OCC_REAL) {
    if (use->Expr_id() == def->Expr_id()) return true;
    return false;
  } else if (use->Kind() == OCC_PHI_OPND) {
    return use->Expr()->Is_same_e_ver(def->Expr());
  }
  return false;
  // TODO: process for Strength reduction
}

bool SSAPRE::Is_same_ver_for_phi_def(OCC_PTR def, OCC_PTR use) {
  AIR_ASSERT(def->Kind() == OCC_PHI);
  HEXPR_PTR use_expr = use->Expr();
  AIR_ASSERT(!use_expr->Is_null());
  if (Is_expr_modify_phi_res(use_expr, def->Bb())) {
    return false;
  }
  return true;
}

bool SSAPRE::Is_expr_modify_phi_res(HEXPR_PTR expr, BB_PTR phi_bb) {
  AIR_ASSERT(phi_bb->Is_phi());
  switch (expr->Kind()) {
    case EK_VAR: {
      BB_ID  def_id = expr->Def_bb();
      BB_PTR def_bb = Cfg().Entry_bb();
      if (def_id != BB_ID()) {
        def_bb = Cfg().Bb_ptr(expr->Def_bb());
      }
      if (def_bb != phi_bb) {
        if (def_bb->Dominates(phi_bb)) {
          return false;
        }
      } else {
        if (expr->Cast_to_var_expr()->Def_by_phi()) {
          return false;
        }
      }
      return true;
    }
    case EK_OP: {
      OP_DATA_PTR op_expr = expr->Cast_to_op_expr();
      for (uint32_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
        if (Is_expr_modify_phi_res(Hssa_cont().Expr_ptr(op_expr->Kid(idx)),
                                   phi_bb)) {
          return true;
        }
      }
      return false;
    }
    default:
      AIR_ASSERT_MSG(false, "unexpected kind");
  }
  return true;
}

HEXPR_PTR SSAPRE::Get_cur_ver(HEXPR_PTR expr, OCC_PTR phi_occ,
                              uint32_t phi_idx) {
  AIR_ASSERT(phi_occ->Kind() == OCC_PHI);
  HCONTAINER& cont = Hssa_cont();
  BB_PTR      bb   = phi_occ->Bb();
  AIR_ASSERT(bb->Is_phi());
  AIR_ASSERT(expr->Kind() == EK_VAR);

  uint32_t  var_id      = expr->Cast_to_var_expr()->Var_id();
  HPHI_PTR  matched_phi = Null_ptr;
  HPHI_ID   phi_head    = bb->Begin_phi_id();
  HPHI_ID   cur         = phi_head;
  HPHI_LIST phi_list(&cont, phi_head);
  while (cur != HPHI_ID()) {
    HPHI_PTR  phi = cont.Phi_ptr(cur);
    HEXPR_PTR res = phi->Result();
    AIR_ASSERT(res->Kind() == EK_VAR);
    if (res->Match_lex(expr)) {
      matched_phi = phi;
      break;
    }
    cur = phi->Next_id();
  }
  if (!matched_phi->Is_null()) {
    return cont.Expr_ptr(matched_phi->Opnd_id(phi_idx));
  } else {
    return Null_ptr;
  }
}

HEXPR_PTR SSAPRE::Phi_opnd_with_cur_ver(OCC_PTR real_occ, uint32_t phi_idx) {
  AIR_ASSERT(real_occ->Kind() == OCC_REAL);
  OCC_PTR def_phi = real_occ->Def();
  AIR_ASSERT(!def_phi->Is_null() && def_phi->Kind() == OCC_PHI);

  HEXPR_PTR expr = real_occ->Expr();
  switch (expr->Kind()) {
    case EK_OP: {
      OP_DATA* op_expr = OP_DATA::Alloc(expr->Cast_to_op_expr()->Kid_cnt());
      new (op_expr) OP_DATA(expr->Cast_to_op_expr());
      for (uint32_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
        HEXPR_PTR kid     = Hssa_cont().Expr_ptr(op_expr->Kid(idx));
        HEXPR_PTR cur_ver = Get_cur_ver(kid, def_phi, phi_idx);
        if (!cur_ver->Is_null()) {
          op_expr->Set_kid(idx, cur_ver->Id());
        }
      }
      HEXPR_DATA_PTR op_ptr(op_expr, HEXPR_ID());
      HEXPR_PTR      new_expr =
          Hssa_cont().Find_or_new_expr(HEXPR_PTR(HEXPR(&Hssa_cont(), op_ptr)));
      free(op_expr);
      return new_expr;
    }
    default:
      AIR_ASSERT_MSG(false, "unexpected expr kind");
  }
  return Null_ptr;
}

void SSAPRE::Create_new_version(OCC_PTR& occ, std::stack<OCC_PTR>& occ_stack) {
  occ->Set_ver(Cur_ver());
  Inc_cur_ver();
  occ_stack.push(occ);
}

// Step 5: Finalize
void SSAPRE::Finalize() {
  Compute_save_reload_inserts();
  Trace_obj(TRACE_PRE_FLOW, Cur_cand());
  Gen_save_reload();
}

bool SSAPRE::Need_insert(PHI_OPND_OCC_DATA_PTR phi_opnd) {
  if (phi_opnd->Def_id() == OCC_ID()) {
    return true;
  }
  if (!phi_opnd->Has_real_use()) {
    OCC_PTR def_occ = Pre_cont().Occ_ptr(phi_opnd->Def_id());
    if (def_occ->Kind() == OCC_PHI &&
        !def_occ->Cast_to_phi_occ()->Will_be_avail()) {
      return true;
    }
  }
  return false;
}

void SSAPRE::Compute_save_reload_inserts() {
  PRE_CONTAINER& pre_cont = Pre_cont();
  PRE_CAND_PTR   pre_cand = Cur_cand();
  AIR_ASSERT(!pre_cand->Is_null());

  ALL_OCC_LIST&        all_occs = Cur_all_occs();
  std::vector<OCC_PTR> def_occs;
  // TODO: allocate with real versions
  def_occs.resize(10, air::base::Null_ptr);
  for (auto occ_id : all_occs) {
    OCC_PTR  occ = pre_cont.Occ_ptr(occ_id);
    uint32_t ver = occ->Ver();
    switch (occ->Kind()) {
      case OCC_REAL: {
        REAL_OCC_DATA_PTR real_occ = occ->Cast_to_real_occ();
        if (def_occs[ver] != air::base::Null_ptr &&
            def_occs[ver]->Dominates(occ)) {
          Trace(TRACE_PRE_FLOW, "* Mark OCC[", occ->Id().Value(),
                "] reload by OCC[", def_occs[ver]->Id().Value(), "]\n");
          real_occ->Set_reload();
          AIR_ASSERT(!real_occ->Is_save());
          real_occ->Set_def(def_occs[ver]);
          def_occs[ver]->Set_save();
        } else {
          real_occ->Reset_reload();
          def_occs[ver] = occ;
        }
        break;
      }
      case OCC_PHI: {
        PHI_OCC_DATA_PTR phi_occ = occ->Cast_to_phi_occ();
        if (phi_occ->Will_be_avail()) {
          def_occs[ver] = occ;
        }
        break;
      }
      case OCC_PHI_OPND: {
        PHI_OPND_OCC_DATA_PTR opnd_occ = occ->Cast_to_phi_opnd_occ();
        OCC_PTR phi_occ = pre_cont.Occ_ptr(opnd_occ->Owning_phi_occ());
        if (!phi_occ->Cast_to_phi_occ()->Will_be_avail()) {
          break;
        }

        if (Need_insert(opnd_occ)) {
          opnd_occ->Set_inserted();
          opnd_occ->Set_save();
          opnd_occ->Set_def(OCC_ID());
        } else {
          opnd_occ->Set_def(def_occs[ver]);
        }
      } break;
      default:
        CMPLR_ASSERT(false, "Compute_save_reload_inserts: not supported");
    }
  }
}

HEXPR_PTR SSAPRE::Get_or_new_temp_var_exp(HEXPR_PTR expr) {
  HEXPR_PTR cur_tmp = Cur_tmp();
  if (!cur_tmp->Is_null()) {
    // create a new tmp expr with new version from cur_tmp
    HEXPR_PTR new_tmp = Hssa_cont().New_var_with_ver(cur_tmp, Cur_ver());
    Inc_cur_ver();
    return new_tmp;
  } else {
    // create a new tmp
    cur_tmp = Hssa_cont().New_preg_expr(expr);
    Set_cur_tmp(cur_tmp);
    return cur_tmp;
  }
}

void SSAPRE::Gen_save_for_occ(OCC_PTR occ) {
  PRE_CONTAINER& pre_cont  = Pre_cont();
  HCONTAINER&    hssa_cont = pre_cont.Hssa_cont();
  HEXPR_PTR      tmp_expr  = Get_or_new_temp_var_exp(occ->Expr());
  if (occ->Kind() == OCC_REAL && occ->Cast_to_real_occ()->Is_lhs()) {
    CMPLR_ASSERT(false, "TO IMPL");
  } else {
    if (occ->Kind() == OCC_REAL) {
      // generate save statment
      HSTMT_PTR save_stmt = hssa_cont.New_assign_stmt(tmp_expr, occ->Expr());
      HSTMT_PTR stmt      = occ->Stmt();
      BB_PTR    bb        = Cfg().Bb_ptr(stmt->Bb_id());
      bb->Insert_stmt_before(save_stmt, stmt);
      // this is a temp change to replace real_occ's owing stmt
      // the right way is to perform copy prop in HSSA construction
      // then there will be no extra store stmt
      HSTMT_PTR orig_stmt = occ->Stmt();
      orig_stmt->Replace_expr(occ->Expr_id(), tmp_expr->Id());
      Trace(TRACE_PRE_FLOW, "\n* Gen save stmt for OCC[", occ->Id().Value(),
            "]\n");
      Trace_obj(TRACE_PRE_FLOW, save_stmt);
    } else if (occ->Kind() == OCC_PHI_OPND) {
      // generate save statment
      HSTMT_PTR save_stmt = hssa_cont.New_assign_stmt(tmp_expr, occ->Expr());
      BB_PTR    bb        = occ->Bb();
      bb->Append_stmt(save_stmt);
      Trace(TRACE_PRE_FLOW, "\n* Gen save stmt for phi opnd OCC[",
            occ->Id().Value(), "]\n");
      Trace_obj(TRACE_PRE_FLOW, save_stmt);
    } else if (occ->Kind() == OCC_PHI) {
      PHI_OCC_DATA_PTR phi_occ = occ->Cast_to_phi_occ();
      HPHI_PTR hphi = Hssa_cont().New_phi(occ->Bb(), phi_occ->Num_opnds());
      hphi->Set_result(tmp_expr->Id());
      phi_occ->Set_saved_phi(hphi->Id());
      Trace(TRACE_PRE_FLOW, "\n* Gen save phi for phi OCC[", occ->Id().Value(),
            "]\n");
      Trace_obj(TRACE_PRE_FLOW, hphi);
      // TODO: update phi opnds
    }
  }
  occ->Set_saved_expr(tmp_expr);
}

bool SSAPRE::Replace_occurs(OCC_PTR occ, HEXPR_ID saved_expr) {
  HEXPR_PTR expr = occ->Expr();
  HSTMT_PTR stmt = occ->Stmt();
  return stmt->Replace_expr(expr->Id(), saved_expr);
}

void SSAPRE::Gen_reload_for_realocc(OCC_PTR occ) {
  AIR_ASSERT(occ->Kind() == OCC_REAL);
  HCONTAINER& hssa_cont  = Hssa_cont();
  OCC_PTR     def_occ    = occ->Def();
  HEXPR_ID    saved_expr = HEXPR_ID();
  switch (def_occ->Kind()) {
    case OCC_REAL:
      saved_expr = def_occ->Cast_to_real_occ()->Saved_expr();
      break;
    case OCC_PHI: {
      PHI_OCC_DATA_PTR phi_occ = def_occ->Cast_to_phi_occ();
      HPHI_ID          phi_id  = phi_occ->Saved_phi();
      AIR_ASSERT(phi_id != HPHI_ID());
      HPHI_PTR saved_phi = hssa_cont.Phi_ptr(phi_id);
      saved_expr         = saved_phi->Result_id();
      break;
    }
    default:
      CMPLR_ASSERT(false, "TO IMPL");
  }
  AIR_ASSERT(saved_expr != HEXPR_ID());

  bool is_replaced = Replace_occurs(occ, saved_expr);
  // update worklist
  if (is_replaced) {
    // Build work list for stmt
    CMPLR_ASSERT(false, "TO IMPL");
  }
  Trace(TRACE_PRE_FLOW, "\n* Replace OCC[", occ->Id().Value(), "] with ");
  Trace_obj(TRACE_PRE_FLOW, hssa_cont.Expr_ptr(saved_expr));
}

void SSAPRE::Gen_save_reload() {
  PRE_CONTAINER& pre_cont = Pre_cont();
  PRE_CAND_PTR   pre_cand = Cur_cand();
  AIR_ASSERT(!pre_cand->Is_null());

  ALL_OCC_LIST& all_occs = Cur_all_occs();
  for (auto occ_id : all_occs) {
    OCC_PTR occ = pre_cont.Occ_ptr(occ_id);
    switch (occ->Kind()) {
      case OCC_REAL: {
        REAL_OCC_DATA_PTR real_occ = occ->Cast_to_real_occ();
        if (real_occ->Is_save()) {
          AIR_ASSERT(!real_occ->Is_reload());
          Gen_save_for_occ(occ);
        } else if (real_occ->Is_reload()) {
          Gen_reload_for_realocc(occ);
        }
        break;
      }
      case OCC_PHI_OPND: {
        PHI_OPND_OCC_DATA_PTR opnd_occ = occ->Cast_to_phi_opnd_occ();
        if (opnd_occ->Is_save()) {
          // at a save location, def occ should be null
          AIR_ASSERT(occ->Def()->Is_null());
          Gen_save_for_occ(occ);
        }
        break;
        // TODO: udpate phi opnds
      }
      case OCC_PHI: {
        PHI_OCC_DATA_PTR phi_occ = occ->Cast_to_phi_occ();
        if (!phi_occ->Will_be_avail()) break;
        Gen_save_for_occ(occ);
        break;
      }
      default:
        CMPLR_ASSERT(false, "Gen_save_reload: not supported");
    }
  }
}

void SSAPRE::Print_all_occs(std::ostream& os, PRE_CONTAINER& cont,
                            ALL_OCC_LIST& occ_list) {
  os << "ALL occs" << std::endl;
  for (auto occ_id : occ_list) {
    OCC_PTR occ = cont.Occ_ptr(occ_id);
    occ->Print(os);
    os << std::endl;
  }
}
}  // namespace opt
}  // namespace air
