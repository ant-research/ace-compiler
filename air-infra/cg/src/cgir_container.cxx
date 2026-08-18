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

#include "air/cg/targ_info.h"

namespace air {

namespace cg {

OPND_PTR CGIR_CONTAINER::New_opnd(REG_CLASS reg_cls, uint8_t reg_num) {
  AIR_ASSERT((uint32_t)reg_cls <= UINT8_MAX);
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA((uint8_t)reg_cls, (uint8_t)reg_num);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(int64_t val) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(val);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(air::base::SYM_ID sym, int64_t ofst) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(sym, ofst);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(air::base::CONSTANT_ID cst, int64_t ofst) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(cst, ofst);
  return OPND_PTR(OPND(this, data));
}

OPND_PTR CGIR_CONTAINER::New_opnd(air::base::LABEL_ID label) {
  OPND_DATA_PTR data = _opnd_tab->template Allocate<OPND_DATA>();
  new (data) OPND_DATA(label);
  return OPND_PTR(OPND(this, data));
}

INST_PTR CGIR_CONTAINER::New_inst(air::base::SPOS spos, uint32_t isa,
                                  uint32_t opcode) {
  const TARG_INFO* ti = TARG_INFO_MGR::Targ_info(isa);
  AIR_ASSERT(ti != nullptr);
  return New_inst(spos, isa, opcode, ti->Res_count(opcode),
                  ti->Opnd_count(opcode));
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
  const TARG_INFO* ti = TARG_INFO_MGR::Targ_info(isa);
  AIR_ASSERT(ti != nullptr);
  uint32_t res_count      = ti->Res_count(opcode);
  uint32_t opnd_count     = ti->Opnd_count(opcode);
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

NODE_PTR CGIR_CONTAINER::New_node() {
  CFG::NODE_PTR node = _cfg.New_node();
  return NODE_PTR(NODE(this, node->Data()));
}

EDGE_PTR CGIR_CONTAINER::Connect(NODE_PTR pred, NODE_PTR succ) {
  CFG::EDGE_PTR edge = _cfg.Connect(pred, succ);
  return EDGE_PTR(EDGE(this, edge->Data()));
}

void CGIR_CONTAINER::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ') << "CGIR CONTAINER:" << std::endl;
  For_each(
      Node_tab(),
      +[](const CGIR_CONTAINER* cont, uint32_t id, std::ostream& os,
          uint32_t idt) {
        NODE_PTR node = cont->Node(NODE_ID(id));
        node->Print(os, idt);
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

CGIR_CONTAINER::CGIR_CONTAINER() : _cfg(this) {
  // create opnd & inst tables
  air::util::CXX_MEM_ALLOCATOR<OPND_TAB, MEM_POOL> opnd_a(&_mpool);
  _opnd_tab = opnd_a.Allocate(&_mpool, OPND_TAB_KIND, "opnd_tab", true);
  air::util::CXX_MEM_ALLOCATOR<INST_TAB, MEM_POOL> inst_a(&_mpool);
  _inst_tab = inst_a.Allocate(&_mpool, INST_TAB_KIND, "inst_tab", true);
  // create node & edge tables
  air::util::CXX_MEM_ALLOCATOR<NODE_TAB, MEM_POOL> node_a(&_mpool);
  _node_tab = node_a.Allocate(&_mpool, NODE_TAB_KIND, "node_tab", true);
  air::util::CXX_MEM_ALLOCATOR<EDGE_TAB, MEM_POOL> edge_a(&_mpool);
  _edge_tab = edge_a.Allocate(&_mpool, EDGE_TAB_KIND, "edge_tab", true);
  // create fake entry & exit
  _entry = New_node()->Id();
  _exit  = New_node()->Id();
}

CGIR_CONTAINER::~CGIR_CONTAINER() {}

}  // namespace cg

}  // namespace air
