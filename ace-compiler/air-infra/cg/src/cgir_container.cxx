//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_container.h"

#include <stdarg.h>

#include <sstream>

#include "air/base/st.h"
#include "air/cg/cgir_util.h"
#include "air/cg/isa_core.h"
#include "air/cg/targ_info.h"
#include "air/util/debug.h"

namespace air {

namespace cg {

OPND_PTR CGIR_CONTAINER::New_opnd(uint8_t reg_cls, uint8_t reg_num,
                                  uint32_t ver) {
  AIR_ASSERT(reg_cls <= UINT8_MAX);
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(reg_cls, (uint8_t)reg_num, ver);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(int64_t val) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(val);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(air::base::SYM_ID sym, int64_t ofst,
                                  uint16_t flag) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(sym, ofst, flag);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(air::base::CONSTANT_ID cst, int64_t ofst,
                                  uint16_t flag) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(cst, ofst, flag);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(BB_ID bb) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(bb);
  return OPND_PTR(OPND(this, data));
}

INST_PTR CGIR_CONTAINER::New_inst(air::base::SPOS spos, uint32_t isa,
                                  uint32_t opcode) {
  const ISA_INFO_META* ti = TARG_INFO_MGR::Isa_info(isa);
  AIR_ASSERT(ti != nullptr);
  return New_inst(spos, isa, opcode, ti->Res_num(opcode), ti->Opnd_num(opcode));
}

INST_PTR CGIR_CONTAINER::New_inst(air::base::SPOS spos, uint32_t isa,
                                  uint32_t opcode, uint8_t res_cnt,
                                  uint8_t opnd_cnt) {
  INST_DATA_PTR data = New_inst_data(res_cnt + opnd_cnt);
  new (data) INST_DATA(spos, isa, opcode, res_cnt, opnd_cnt);
  return INST_PTR(INST(this, data));
}

INST_PTR CGIR_CONTAINER::New_inst(air::base::SPOS spos, uint32_t isa,
                                  uint32_t opcode, OPND_ID res_opnd, ...) {
  const ISA_INFO_META* ti = TARG_INFO_MGR::Isa_info(isa);
  AIR_ASSERT(ti != nullptr);
  uint32_t res_count      = ti->Res_num(opcode);
  uint32_t opnd_count     = ti->Opnd_num(opcode);
  INST_PTR inst           = New_inst(spos, isa, opcode, res_count, opnd_count);
  uint32_t res_opnd_count = res_count + opnd_count;
  AIR_ASSERT(res_opnd_count > 0);
  inst->Set_res_opnd(0, res_opnd);
  va_list ap;
  va_start(ap, res_opnd);
  for (uint32_t i = 1; i < res_opnd_count; ++i) {
    OPND_ID opnd = va_arg(ap, OPND_ID);
    AIR_ASSERT(opnd != air::base::Null_id);
    inst->Set_res_opnd(i, opnd);
  }
  va_end(ap);
  return inst;
}

INST_PTR CGIR_CONTAINER::New_chi(air::base::SPOS spos, OPND_ID res,
                                 OPND_ID opnd) {
  INST_PTR inst = New_inst(spos, ISA_CORE, CHI, 1, 1);
  inst->Set_res(0, res);
  inst->Set_opnd(0, opnd);
  return inst;
}

INST_PTR CGIR_CONTAINER::New_phi(air::base::SPOS spos, OPND_ID res,
                                 uint32_t preds) {
  INST_PTR inst = New_inst(spos, ISA_CORE, PHI, 1, preds);
  inst->Set_res(0, res);
  return inst;
}

BB_PTR CGIR_CONTAINER::New_bb() {
  CFG::NODE_PTR node = _cfg.New_node();
  return BB_PTR(BB(this, node->Data()));
}

EDGE_PTR CGIR_CONTAINER::Connect(BB_PTR pred, BB_PTR succ) {
  CFG::EDGE_PTR edge = _cfg.Connect(pred, succ);
  return EDGE_PTR(EDGE(this, edge->Data()));
}

void CGIR_CONTAINER::Create_fake_entry_exit() {
  AIR_ASSERT(_entry == air::base::Null_id);
  AIR_ASSERT(_exit == air::base::Null_id);
  _entry = New_bb()->Id();
  _exit  = New_bb()->Id();
}

void CGIR_CONTAINER::Set_bb_order(BB_PTR prev, BB_PTR next) {
  air::util::LAYOUT_INFO* prev_info = prev->Layout_info();
  air::util::LAYOUT_INFO* next_info = next->Layout_info();
  AIR_ASSERT(prev_info->Next() == air::base::Null_prim_id);
  AIR_ASSERT(next_info->Prev() == air::base::Null_prim_id);
  prev_info->Set_next(next->Id().Value());
  next_info->Set_prev(prev->Id().Value());
}

OPND_PTR CGIR_CONTAINER::Clone(OPND_PTR opnd) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(opnd->Data().Addr());
  return OPND_PTR(OPND(this, data));
}

INST_PTR CGIR_CONTAINER::Clone(INST_PTR inst) {
  INST_DATA_PTR orig = inst->Data();
  INST_DATA_PTR data = New_inst_data(orig->Res_opnd_count());
  new (data) INST_DATA(orig.Addr());
  for (uint32_t i = 0; i < orig->Res_opnd_count(); ++i) {
    OPND_PTR opnd = Clone(inst->Res_opnd(i));
    data->Set_res_opnd(i, opnd->Id());
  }
  return INST_PTR(INST(this, data));
}

BB_PTR CGIR_CONTAINER::Clone(BB_PTR bb, bool cp_bb, bool cp_inst) {
  CFG::NODE_PTR node = _cfg.New_node();
  if (cp_bb) {
    new (node->Data().Addr()) BB_DATA(bb->Data().Addr());
  }
  BB_PTR ptr(BB(this, node->Data()));
  if (cp_inst) {
    for (auto&& inst : INST_ITER(bb)) {
      ptr->Append(Clone(inst));
    }
  }
  return ptr;
}

void CGIR_CONTAINER::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ') << "CGIR CONTAINER:" << std::endl;
  For_each(
      Bb_tab(),
      +[](const CGIR_CONTAINER* cont, uint32_t id, std::ostream& os,
          uint32_t idt) {
        BB_PTR bb = cont->Bb(BB_ID(id));
        bb->Print(os, idt);
      },
      os, indent);
}

void CGIR_CONTAINER::Print() const { Print(std::cout, 0); }

std::string CGIR_CONTAINER::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

INST_DATA_PTR CGIR_CONTAINER::New_inst_data(uint32_t res_opnd) {
  size_t sz = sizeof(INST_DATA) + res_opnd * sizeof(OPND_ID);
  AIR_ASSERT(_inst_tab->Unit_size() == 4);
  AIR_ASSERT(sz % _inst_tab->Unit_size() == 0);
  INST_DATA_PTR data =
      air::base::Static_cast<INST_DATA_PTR>(_inst_tab->Malloc(sz >> 2));
  return data;
}

CGIR_CONTAINER::CGIR_CONTAINER(base::FUNC_SCOPE* func_scope,
                               bool              fake_entry_exit)
    : _cfg(this), _func_scope(func_scope) {
  // create opnd & inst tables
  air::util::CXX_MEM_ALLOCATOR<OPND_TAB, MEM_POOL> opnd_a(&_mpool);
  _opnd_tab = opnd_a.Allocate(&_mpool, OPND_TAB_KIND, "opnd_tab", true);
  air::util::CXX_MEM_ALLOCATOR<INST_TAB, MEM_POOL> inst_a(&_mpool);
  _inst_tab = inst_a.Allocate(&_mpool, INST_TAB_KIND, "inst_tab", true);
  // create bb & edge tables
  air::util::CXX_MEM_ALLOCATOR<BB_TAB, MEM_POOL> bb_a(&_mpool);
  _bb_tab = bb_a.Allocate(&_mpool, BB_TAB_KIND, "bb_tab", true);
  air::util::CXX_MEM_ALLOCATOR<EDGE_TAB, MEM_POOL> edge_a(&_mpool);
  _edge_tab = edge_a.Allocate(&_mpool, EDGE_TAB_KIND, "edge_tab", true);
  // create fake entry & exit
  if (fake_entry_exit) {
    Create_fake_entry_exit();
  }
}

CGIR_CONTAINER::~CGIR_CONTAINER() {}

}  // namespace cg

}  // namespace air
