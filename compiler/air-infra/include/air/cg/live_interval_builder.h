//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_LIVE_INTERVAL_BUILDER_H
#define AIR_CG_LIVE_INTERVAL_BUILDER_H

#include "air/cg/cgir_container.h"
#include "air/cg/config.h"
#include "air/driver/driver_ctx.h"
#include "air/util/error.h"
#include "live_interval.h"

namespace air {
namespace cg {

//! @brief Implementation of LIVE_INTERVAL builder which is implemented
//! according to Fig.4 in follow article:
//! Wimmer C, Franz M. Linear scan register allocation on SSA form.
//! CGO 2010: 170-179.
class LIVE_INTERVAL_BUILDER {
  using DRIVER_CTX    = driver::DRIVER_CTX;
  using BB_LIVE_VALUE = std::vector<std::set<PREG_INFO>>;

public:
  LIVE_INTERVAL_BUILDER(const DRIVER_CTX*      driver_ctx,
                        const CODE_GEN_CONFIG* cfg, CGIR_CONTAINER* cntr,
                        LIVE_INTERVAL_INFO* li_info)
      : _driver_ctx(driver_ctx), _cfg(cfg), _cont(cntr), _li_info(li_info) {}

  R_CODE Run(void) {
    // 1. calculate slot for each instruction.
    Cal_inst_slot();
    // 2. create live interval for each PREG.
    Gen_live_interval();
    // 3. update LIVE_INTERVAL_INFO to create a sorted live interval list.
    _li_info->Update();

    Trace(TRACE_LI_BUILD, "\n Created follow LIVE_INTERVALs:\n");
    Trace_obj(TRACE_LI_BUILD, _li_info);

    return R_CODE::NORMAL;
  }

private:
  //! @brief Calculate the slot for each instruction by traversing the basic
  //! blocks in forward order, increasing the SLOT index by SLOT::DIST with
  //! each instruction and basic block entry. The SLOT index of each PHI is
  //! set to the same as its basic block entry.
  void Cal_inst_slot(void);

  //! @brief Collect live-out PREG values of block bb and create initial live
  //! ranges for them. There are two types of PREG values set as live-out from
  //! the current basic block. The first type, used in non-PHI instructions of
  //! successor BBs, is handled in LIVE_INTERVAL_BUILDER::Gen_live_interval.
  //! The second type, used in PHI operands, is managed at the beginning of
  //! processing each basic block in Handle_live_out_value. It is designed to
  //! handle loop-back BBs, which are processed before their predecessor, the
  //! loop header.
  void Handle_live_out_value(BB_PTR bb);
  //! @brief Create live ranges for operands of non-phi instructions.
  void Handle_inst(BB_PTR bb);
  //! @brief Handle PHIs which remove values defined by phi from live values.
  void Handle_phi_res(BB_PTR bb);
  //! @brief Create live intervals for each PREG by traversing the CFG in
  //! reverse order. Within each loop, prioritize traversing the loop-back
  //! basic block (BB) before other BBs in the loop.
  void Gen_live_interval();

  //! @brief Set up the slot for instruction inst
  void Set_inst_slot(const SLOT& slot) {
    _li_info->Set_inst_slot(slot.Inst(), slot);
  }
  //! @brief Return the slot of inst
  const SLOT& Inst_slot(INST_ID inst) const {
    return _li_info->Inst_slot(inst);
  }

  //! @brief Set up the range of BB.
  void Set_bb_range(BB_ID bb, const RANGE& rg) {
    _li_info->Set_bb_range(bb, rg);
  }
  //! @brief Return the range of BB
  const RANGE& Bb_range(BB_ID bb) const { return _li_info->Bb_range(bb); }

  //! @brief Add a live value for the basic block bb. Return true if the value
  //! is set live for the first time; otherwise, return false.
  bool Add_live(BB_ID bb, const PREG_INFO& v) {
    if (_live.size() <= bb.Value()) _live.resize(bb.Value() + 1);
    return _live[bb.Value()].insert(v).second;
  }
  //! @brief Remove a value from live values.
  void Del_live(BB_ID bb, base::PREG_ID preg, INST_ID def_inst) {
    if (_live.size() <= bb.Value()) return;
    // Reg_class is not used in differing VALUEs.
    _live[bb.Value()].erase(PREG_INFO(preg, def_inst, 0));
  }
  //! @brief Return live values of basic block.
  std::set<PREG_INFO>& Live(BB_ID bb) {
    if (_live.size() <= bb.Value()) {
      _live.resize(bb.Value() + 1);
    }
    return _live[bb.Value()];
  }

  //! @brief Return the live interval of PREG val.
  LIVE_INTERVAL& Live_interval(const PREG_INFO& val) {
    return _li_info->Get_live_interval(val);
  }

  CGIR_CONTAINER* Container(void) const { return _cont; }

  DECLARE_TRACE_DETAIL_API((*_cfg), _driver_ctx)

  const DRIVER_CTX*      _driver_ctx = nullptr;
  const CODE_GEN_CONFIG* _cfg        = nullptr;
  CGIR_CONTAINER*        _cont       = nullptr;
  LIVE_INTERVAL_INFO*    _li_info;  //< live interval of each PREG
  BB_LIVE_VALUE          _live;     //< live var of each BB
};

}  // namespace cg
}  // namespace air

#endif  // AIR_CG_LIVE_INTERVAL_BUILDER_H
