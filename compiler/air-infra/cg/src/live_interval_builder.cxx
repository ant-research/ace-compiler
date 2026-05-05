//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/live_interval_builder.h"

#include "air/cg/cgir_util.h"
#include "air/cg/config.h"
#include "air/cg/isa_core.h"

namespace air {
namespace cg {

void LIVE_INTERVAL_BUILDER::Cal_inst_slot(void) {
  uint32_t slot_idx = SLOT::DIST;
  for (BB_PTR bb : LAYOUT_ITER(Container())) {
    // reserve the first SLOT idx for BB entry.
    uint32_t start = slot_idx;
    slot_idx += SLOT::DIST;
    // 1. calculate SLOT for each instruction.
    for (INST_PTR inst : INST_ITER(bb)) {
      Trace(TRACE_LI_BUILD, "\nMap inst: ");
      Trace_obj(TRACE_LI_BUILD, inst);
      // Current implementation does not create a new SLOT for PHI and CHI,
      // as they are not real instructions.
      if (inst->Opcode() == CORE_PHI || inst->Opcode() == CORE_CHI) {
        Set_inst_slot(SLOT(inst->Id(), bb->Id(), start, BLOCK_SLOT));
        Trace(TRACE_LI_BUILD, " to SLOT_", start);
      } else {
        Set_inst_slot(SLOT(inst->Id(), bb->Id(), slot_idx, REGISTER_SLOT));
        Trace(TRACE_LI_BUILD, " to SLOT_", slot_idx);
        slot_idx += SLOT::DIST;
      }
    }
    // 2. set up range of current basic block.
    RANGE bb_range(SLOT(bb->Id(), start), SLOT(bb->Id(), slot_idx));
    Set_bb_range(bb->Id(), bb_range);
  }
}

void LIVE_INTERVAL_BUILDER::Handle_live_out_value(BB_PTR bb) {
  // 1. Collect live out of current bb from phi operands in successors.
  std::set<PREG_INFO>&      live_out  = Live(bb->Id());
  CFG::NODE_ITER<CFG::SUCC> succ_iter = bb->Begin_succ();
  for (; succ_iter != bb->End_succ(); ++succ_iter) {
    // number of bb in current successor's predecessors.
    uint32_t id   = succ_iter->Pos<CFG::PRED>(bb->Id());
    BB_PTR   succ = _cont->Bb(succ_iter->Id());
    for (const INST_PTR& inst : INST_ITER(succ)) {
      if (inst->Opcode() != CORE_PHI) continue;
      OPND_PTR opnd = inst->Opnd(id);
      // skip opnds not defined within function body
      if (opnd->Def_inst()->Opcode() == CORE_CHI) continue;
      AIR_ASSERT(opnd->Is_preg());
      PREG_INFO vi(opnd->Preg(), opnd->Def_inst_id(), opnd->Reg_class());
      live_out.insert(vi);
    }
  }
  if (live_out.empty()) return;
  // 2. Create an initial live range for each live-out PREG value that spans
  // the entire basic block.
  RANGE range = Bb_range(bb->Id());
  for (const PREG_INFO& v : live_out) {
    LIVE_RANGE lr(range.Start(), range.End(), {});
    Live_interval(v).Add_range(lr);
  }
}

//! @brief Collect live range for opnds used in non-phi inst.
void LIVE_INTERVAL_BUILDER::Handle_inst(BB_PTR bb) {
  if (bb->Empty()) return;

  const SLOT&                   bb_start  = Bb_range(bb->Id()).Start();
  INST_ITER::REVERSE_ITERATOR&& inst_iter = INST_ITER(bb).rbegin();
  for (; inst_iter != INST_ITER(bb).rend(); ++inst_iter) {
    // skip PHI and CHI list
    if (inst_iter->Opcode() == CORE_PHI || inst_iter->Opcode() == CORE_CHI)
      break;

    const SLOT& inst_slot = Inst_slot(inst_iter->Id());
    // 1. handle defined PREGs
    for (uint32_t id = 0; id < inst_iter->Res_count(); ++id) {
      OPND_PTR res = inst_iter->Res(id);
      if (!res->Is_preg()) continue;
      base::PREG_ID preg = res->Preg();
      PREG_INFO     vi(preg, inst_iter->Id(), res->Reg_class());
      Live_interval(vi).Update_start_slot(inst_slot);
      Del_live(bb->Id(), preg, inst_iter->Id());
    }
    // 2. handle used PREGs
    for (uint32_t id = 0; id < inst_iter->Opnd_count(); ++id) {
      OPND_PTR opnd = inst_iter->Opnd(id);
      if (!opnd->Is_preg()) continue;
      base::PREG_ID  preg = opnd->Preg();
      PREG_INFO      vi(preg, opnd->Def_inst_id(), opnd->Reg_class());
      LIVE_INTERVAL& li = Live_interval(vi);
      LIVE_RANGE     lr(bb_start, inst_slot,
                        {OCCUR_INFO(inst_slot, id, OCC_INST_OPND)});
      li.Add_range(lr);
      Add_live(bb->Id(),
               PREG_INFO(preg, opnd->Def_inst_id(), opnd->Reg_class()));
    }
  }
}

void LIVE_INTERVAL_BUILDER::Handle_phi_res(BB_PTR bb) {
  if (bb->Empty()) return;
  if (bb->Pred_cnt() <= 1) return;

  INST_ITER::ITER_IMPL<>&& inst_iter = INST_ITER(bb).begin();
  for (; inst_iter != INST_ITER(bb).end(); ++inst_iter) {
    if (inst_iter->Opcode() != CORE_PHI) continue;
    OPND_PTR res = inst_iter->Res(0);
    AIR_ASSERT(res->Is_preg());
    Del_live(bb->Id(), res->Preg(), inst_iter->Id());
  }
}

void LIVE_INTERVAL_BUILDER::Gen_live_interval() {
  // create live interval for each PREG value.
  LAYOUT_ITER::REVERSE_ITERATOR&& bb_iter  = LAYOUT_ITER(Container()).rbegin();
  LAYOUT_ITER::REVERSE_ITERATOR&& end_iter = LAYOUT_ITER(Container()).rend();
  for (; bb_iter != end_iter; ++bb_iter) {
    BB_PTR bb = *bb_iter;
    // 1. create initial live range for values live out bb.
    Handle_live_out_value(bb);

    // 2. create live range for values used/defined in non-phi instructions.
    Handle_inst(bb);

    // 3. remove phi result from living values
    Handle_phi_res(bb);

    // 4. create live range for values live through loop.
    if (bb->Has_attr(base::BB_LOOP_HEADER)) {
      // TODO: extend live range of each live in at loop-header to
      // the end of loop-back.
      Trace(TRACE_LI_BUILD, "Loop is not supported in current implementation");
      /* std::set<PREG_INFO>& live  = Live(bb->Id());
      const SLOT&          start = Bb_range(bb->Id()).Start();
      LOOP_INFO_PTR        loop  = Loop_info()->Parent_loop(bb->Id());
      const SLOT&          end   = Bb_range(loop->Tail_id()).Start();
      for (const PREG_INFO& v : live) {
        Live_interval(v).Add_range(LIVE_RANGE(start, end, {}));
      } */
    }

    // 5. add live-in of current basic-block into live-out of its predecessors
    const std::set<PREG_INFO>& live_out = Live(bb->Id());
    for (uint32_t id = 0; id < bb->Pred_cnt(); ++id) {
      BB_ID pred = bb->Pred_id(id);
      for (const PREG_INFO& v : live_out) {
        Add_live(pred, v);
      }
    }
  }
}

}  // namespace cg
}  // namespace air