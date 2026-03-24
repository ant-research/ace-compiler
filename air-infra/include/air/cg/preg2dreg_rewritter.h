//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_PREG2DREG_REWRITTER_H
#define AIR_CG_PREG2DREG_REWRITTER_H

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "air/base/st_decl.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_util.h"
#include "air/cg/config.h"
#include "air/cg/isa_core.h"
#include "air/cg/live_interval_builder.h"
#include "air/cg/targ_info.h"
#include "air/driver/driver_ctx.h"
#include "air/driver/pass.h"
#include "air/util/debug.h"
#include "air/util/error.h"

namespace air {
namespace cg {

//! @brief Implementation of the PREG to physical register rewriter:
//! PREG2DREG_REWRITTER is the final pass of register allocation, resetting
//! PREGs to the selected physical registers.
class PREG2DREG_REWRITTER {
  using DREG2LI    = std::vector<LIVE_INTERVAL::LIST>;  // index is DREG index.
  using RC_DREG2LI = std::vector<DREG2LI>;  // index is register class.
public:
  PREG2DREG_REWRITTER(const driver::DRIVER_CTX* driver_ctx, const CODE_GEN_CONFIG* config,
                      CGIR_CONTAINER* cont, LIVE_INTERVAL_INFO* li_info,
                      RC_DREG2LI* dreg2li, uint8_t isa)
      : _driver_ctx(driver_ctx), _config(config), _cont(cont), _li_info(li_info), _dreg2li(dreg2li), _isa(isa) {}
  ~PREG2DREG_REWRITTER() {}

  template <typename IR_GEN>
  R_CODE Run(IR_GEN* ir_gen);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  PREG2DREG_REWRITTER(void);
  PREG2DREG_REWRITTER(const PREG2DREG_REWRITTER&);
  PREG2DREG_REWRITTER& operator=(const PREG2DREG_REWRITTER&);

  LIVE_INTERVAL_INFO* Live_info(void) const { return _li_info; }
  RC_DREG2LI*         Dreg2li(void) const { return _dreg2li; }
  CGIR_CONTAINER*     Container(void) const { return _cont; }
  uint32_t            Isa(void) const { return _isa; }

  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)

  const driver::DRIVER_CTX* _driver_ctx = nullptr;
  const CODE_GEN_CONFIG* _config =nullptr;
  CGIR_CONTAINER*     _cont;
  LIVE_INTERVAL_INFO* _li_info;
  RC_DREG2LI*         _dreg2li;
  uint8_t             _isa;
};

template <typename IR_GEN>
R_CODE PREG2DREG_REWRITTER::Run(IR_GEN* ir_gen) {
  // 1. replace pseudo-register in non-PHI instructions
  LIVE_INTERVAL_INFO::ITER li_iter = Live_info()->Begin_li();
  LIVE_INTERVAL_INFO::ITER end_li  = Live_info()->End_li();
  for (; li_iter != end_li; ++li_iter) {
    LIVE_INTERVAL* li       = (*li_iter);
    INST_PTR       def_inst = li->Def_inst();
    const char* reg_name = TARG_INFO_MGR::Reg_name(li->Reg_cls(), li->Reg_num());
    Trace(TRACE_LSRA, "\nPREG2DREG_REWRITTER substitue PREG_",
          li->Preg_id().Index(),
          " with physical register, ", reg_name, ", which is define at:\n");
    Trace_obj(TRACE_LSRA, def_inst);
    OPND_PTR       res      = def_inst->Res(0);
    AIR_ASSERT(res->Preg() == li->Preg_id());
    res->Set_reg_num(li->Reg_num());

    Trace(TRACE_LSRA, "After substitution:\n");
    Trace_obj(TRACE_LSRA, def_inst);
  }

  // 2. remove PHI nodes, and insert move instruction for PHI operands allocated
  // in different physical register with the result.
  LAYOUT_ITER::ITERATOR bb_iter  = LAYOUT_ITER(Container()).begin();
  LAYOUT_ITER::ITERATOR end_iter = LAYOUT_ITER(Container()).end();
  for (; bb_iter != end_iter; ++bb_iter) {
    BB_PTR bb = *bb_iter;
    for (INST_PTR inst = bb->First(); inst != bb->Last(); inst = inst->Next()) {
      if (inst->Opcode() != CORE_PHI) continue;
      OPND_PTR res     = inst->Res(0);
      uint32_t res_reg = res->Reg_num();
      if (res_reg == REG_UNKNOWN) continue;

      for (uint32_t id = 0; id < inst->Opnd_count(); ++id) {
        OPND_PTR opnd     = inst->Opnd(id);
        uint8_t  opnd_reg = opnd->Reg_num();
        AIR_ASSERT(opnd_reg != REG_UNKNOWN);
        if (opnd_reg == res_reg) continue;
        // create move instruction for non-consistent PHI operands.
        OPND_PTR src  = Container()->New_opnd(opnd->Reg_class(), opnd_reg);
        res           = Container()->New_opnd(res->Reg_class(), res->Reg_num());
        INST_PTR mv   = ir_gen->Gen_mv(res, src, inst->Spos());
        BB_PTR   pred = Container()->Bb(bb->Pred_id(id));
        pred->Append(mv);
      }
    }
    for (INST_PTR inst = bb->First(); inst != bb->Last(); inst = inst->Next()) {
      if (inst->Opcode() != CORE_PHI) continue;
      bb->Remove(inst);
      Trace(TRACE_LSRA, "\nPREG2DREG_REWRITTER Remove PHI:");
      Trace_obj(TRACE_LSRA, inst);
    }
  }
  return R_CODE::NORMAL;
}

}  // namespace cg
}  // namespace air
#endif  // PREG2DREG_REWRITTER