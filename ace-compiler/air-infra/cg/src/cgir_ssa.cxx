//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "cgir_ssa.h"

#include "air/cg/visitor.h"
#include "air/util/dom_builder.h"
#include "air/util/dom_util.h"

namespace air {

namespace cg {

void SSA_BUILD_CTX::Insert_phi() {
  // step 1: iterate each bb and insert preg/dreg to all bbs in its DF set
  for (uint32_t i = 0; i < _cntr->Bb_count(); ++i) {
    U32_SET* rkey_set = (*_def_vec)[i];
    if (rkey_set != nullptr) {
      AIR_ASSERT(rkey_set->size() > 0);
      const air::util::DOM_INFO::ID_CSET& df = _dom->Get_dom_frontiers(i);
      for (uint32_t id : df) {
        U32_SET*& df_set = (*_phi_map)[id];
        if (df_set == nullptr) {
          df_set = New_u32_set();
        }
        df_set->merge(*rkey_set);
      }
    }
  }

  if (_trace_insert) {
  }

  // step 2: iterate DF set and insert PHI INST
  for (U32_SET_MAP::iterator it = _phi_map->begin(); it != _phi_map->end();
       ++it) {
    BB_PTR          bb         = _cntr->Bb(BB_ID(it->first));
    air::base::SPOS spos       = bb->Spos();
    uint32_t        pred_count = bb->Pred_cnt();
    for (uint32_t rkey : *(it->second)) {
      INST_PTR inst = New_phi_inst(spos, Zero_opnd(rkey), pred_count);
      bb->Prepend(inst);
    }
  }
}

void SSA_BUILD_CTX::Handle_bb(uint32_t id) {
  // 1. push mark
  Push_stk_mark(id);
  // 2. handle body insts
  //   2.1 rename rhs for normal inst
  //   2.2 push lhs for all inst
  BB_PTR bb = _cntr->Bb(BB_ID(id));
  for (INST_PTR&& inst : INST_ITER(bb)) {
    if (inst->Opcode() != CORE_PHI) {
      for (uint32_t i = 0; i < inst->Opnd_count(); ++i) {
        OPND_PTR opnd = inst->Opnd(i);
        if (opnd->Is_register()) {
          OPND_ID opnd_id = Top_ver(opnd);
          inst->Set_opnd(i, opnd_id);
        }
      }
    }
    for (uint32_t i = 0; i < inst->Res_count(); ++i) {
      OPND_PTR res = inst->Res(i);
      AIR_ASSERT(res->Is_register());
      OPND_ID res_id = Push_ver(res, inst->Id());
      inst->Set_res(i, res_id);
    }
  }
  // 2.3 rename succ phi opnds
  for (BB_PTR succ : SUCC_ITER(bb)) {
    if (_phi_map->find(succ->Id().Value()) != _phi_map->end()) {
      uint32_t pos = succ->Pred_pos(BB_ID(id));
      Rename_succ_opnd(succ, pos);
    }
  }
}

SSA_BUILD_CTX::SSA_BUILD_CTX(CGIR_CONTAINER*            cntr,
                             const air::util::DOM_INFO* dom)
    : _cntr(cntr), _dom(dom) {
  air::util::CXX_MEM_ALLOCATOR<U32_SET_VEC, MEM_POOL> vec_a(&_mpool);
  _def_vec = vec_a.Allocate(cntr->Bb_count(), U32_SET_ALLOCATOR(&_mpool));
  air::util::CXX_MEM_ALLOCATOR<U32_STK_MAP, MEM_POOL> stk_a(&_mpool);
  _stk_map =
      stk_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                     U32_STK_PAIR_ALLOCATOR(&_mpool));
  air::util::CXX_MEM_ALLOCATOR<U32_SET_MAP, MEM_POOL> phi_a(&_mpool);
  _phi_map =
      phi_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                     U32_SET_PAIR_ALLOCATOR(&_mpool));
}

void CGIR_CONTAINER::Build_ssa() {
  // build DOM
  air::util::DOM_INFO    dom_info;
  air::util::DOM_BUILDER dom_bldr(this, &dom_info);
  dom_bldr.Run();

  // traverse CGIR and insert PHI instruction
  SSA_BUILD_CTX                                       ssa_ctx(this, &dom_info);
  air::cg::VISITOR<SSA_BUILD_CTX, PHI_INSERT_HANDLER> phi_trav(ssa_ctx);
  phi_trav.Visit<LAYOUT_ITER, INST_ITER>(LAYOUT_ITER(this));
  ssa_ctx.Insert_phi();

  // rename
  ssa_ctx.Init_rename();
  air::util::DOM_VISITOR<SSA_BUILD_CTX, RENAME_HANDLER> ren_trav(ssa_ctx,
                                                                 &dom_info);
  ren_trav.Visit(this->Entry_id().Value());
  ssa_ctx.Fini_rename();

  // verify
  // VERIFY_CTX verify_ctx(this);
  // verify_ctx.Initialize();
  // air::util::DOM_VISITOR<VERIFY_CTX, VERIFY_HANDLER> verify_trav(verify_ctx);
  // verify_trav.Visit(this->Entry_id().Value());
  // verify_ctx.Finalize();
}

}  // namespace cg

}  // namespace air
