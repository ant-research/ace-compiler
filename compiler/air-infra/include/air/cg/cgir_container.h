//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_CONTAINER_H
#define AIR_CG_CGIR_CONTAINER_H

#include "air/base/container.h"
#include "air/base/loop_info.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_node.h"
#include "air/util/cfg_base.h"
#include "air/util/dom.h"

namespace air {

namespace cg {

//! @brief CGIR_CONTAINER
//   Contains tables for operands, instructions, CFG nodes and edges. Also
//   provides iterators and access APIs for these objects.
class CGIR_CONTAINER {
  friend air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>;
  friend air::util::DOM_BUILDER<CGIR_CONTAINER>;
  using CFG           = air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>;
  using FUNC_SCOPE    = base::FUNC_SCOPE;
  using BB_DATA_PTR   = CFG::NODE_DATA_PTR;
  using EDGE_DATA_PTR = CFG::EDGE_DATA_PTR;
  using OPND_TAB = air::base::ARENA<sizeof(OPND_DATA), sizeof(OPND_ID), false>;
  using INST_TAB = air::base::ARENA<sizeof(INST_ID), sizeof(INST_ID), false>;
  using BB_TAB   = air::base::ARENA<sizeof(BB_DATA), sizeof(BB_ID), false>;
  using EDGE_TAB = air::base::ARENA<sizeof(EDGE_DATA), sizeof(EDGE_ID), false>;

  static constexpr uint32_t OPND_TAB_KIND = 0x50001;
  static constexpr uint32_t INST_TAB_KIND = 0x50002;
  static constexpr uint32_t BB_TAB_KIND   = 0x50003;
  static constexpr uint32_t EDGE_TAB_KIND = 0x50004;

  // for DOM_BUILDER
  BB_PTR        Node(BB_ID id) const { return BB_PTR(BB(this, Find(id))); }
  uint32_t      Node_count() const { return _bb_tab->Size(); }
  const BB_TAB* Node_tab() const { return _bb_tab; }

public:
  OPND_PTR Opnd(OPND_ID id) const { return OPND_PTR(OPND(this, Find(id))); }
  INST_PTR Inst(INST_ID id) const { return INST_PTR(INST(this, Find(id))); }
  BB_PTR   Bb(BB_ID id) const { return BB_PTR(BB(this, Find(id))); }
  EDGE_PTR Edge(EDGE_ID id) const { return EDGE_PTR(EDGE(this, Find(id))); }

  uint32_t Opnd_count() const { return _opnd_tab->Size(); }
  uint32_t Inst_count() const { return _inst_tab->Size(); }
  uint32_t Bb_count() const { return _bb_tab->Size(); }
  uint32_t Edge_count() const { return _edge_tab->Size(); }

public:
  OPND_PTR New_opnd(uint8_t reg_cls, uint8_t reg_num, uint32_t ver = 0);
  OPND_PTR New_opnd(int64_t val);
  OPND_PTR New_opnd(uint8_t reg_cls, base::PREG_ID preg);
  OPND_PTR New_opnd(air::base::SYM_ID sym, int64_t ofst = 0, uint16_t flag = 0);
  OPND_PTR New_opnd(air::base::CONSTANT_ID cst, int64_t ofst = 0,
                    uint16_t flag = 0);
  OPND_PTR New_opnd(BB_ID bb);
  OPND_PTR New_preg(uint8_t reg_cls, uint32_t ver = 0) {
    return New_opnd(reg_cls, REG_UNKNOWN, ver);
  }
  INST_PTR New_inst(air::base::SPOS spos, uint32_t isa, uint32_t opcode);
  INST_PTR New_inst(air::base::SPOS spos, uint32_t isa, uint32_t opcode,
                    uint8_t res_cnt, uint8_t opnd_cnt);
  INST_PTR New_inst(air::base::SPOS spos, uint32_t isa, uint32_t opcode,
                    OPND_ID res_opnd, ...);

  INST_PTR New_chi(air::base::SPOS spos, OPND_ID res, OPND_ID opnd);
  INST_PTR New_phi(air::base::SPOS spos, OPND_ID res, uint32_t preds);

public:
  CFG&        Cfg() { return _cfg; }
  BB_ID       Entry_id() const { return _entry; }
  BB_ID       Exit_id() const { return _exit; }
  BB_PTR      Entry() const { return Bb(_entry); }
  BB_PTR      Exit() const { return Bb(_exit); }
  BB_PTR      New_bb();
  EDGE_PTR    Connect(BB_PTR pred, BB_PTR succ);
  void        Create_fake_entry_exit();
  void        Set_entry(BB_PTR node) { Connect(Entry(), node); }
  void        Set_exit(BB_PTR node) { Connect(node, Exit()); }
  void        Set_bb_order(BB_PTR prev, BB_PTR next);
  FUNC_SCOPE* Func_scope(void) const { return _func_scope; }

  using BB_PRED_ITER = CFG::NODE_ITER<CFG::PRED>;
  using BB_SUCC_ITER = CFG::NODE_ITER<CFG::SUCC>;

public:
  OPND_PTR Clone(OPND_PTR opnd);
  INST_PTR Clone(INST_PTR inst);
  BB_PTR   Clone(BB_PTR bb, bool cp_bb = true, bool cp_inst = true);

public:
  void Build_ssa();

public:
  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

public:
  CGIR_CONTAINER(FUNC_SCOPE* func_scope, bool fake_entry_exit = false);
  ~CGIR_CONTAINER();

private:
  const OPND_TAB* Opnd_tab() const { return _opnd_tab; }
  const INST_TAB* Inst_tab() const { return _inst_tab; }
  const BB_TAB*   Bb_tab() const { return _bb_tab; }
  const EDGE_TAB* Edge_tab() const { return _edge_tab; }

  OPND_DATA_PTR New_opnd_data() { return _opnd_tab->Allocate<OPND_DATA>(); }
  INST_DATA_PTR New_inst_data(uint32_t res_opnd);
  BB_DATA_PTR   New_node_data() { return _bb_tab->Allocate<BB_DATA>(); }
  EDGE_DATA_PTR New_edge_data() { return _edge_tab->Allocate<EDGE_DATA>(); }

  OPND_DATA_PTR Find(OPND_ID id) const {
    return id != air::base::Null_id ? _opnd_tab->Find(id) : OPND_DATA_PTR();
  }
  INST_DATA_PTR Find(INST_ID id) const {
    return id != air::base::Null_id ? _inst_tab->Find(id) : INST_DATA_PTR();
  }
  BB_DATA_PTR Find(BB_ID id) const {
    return id != air::base::Null_id ? _bb_tab->Find(id) : BB_DATA_PTR();
  }
  EDGE_DATA_PTR Find(EDGE_ID id) const {
    return id != air::base::Null_id ? _edge_tab->Find(id) : EDGE_DATA_PTR();
  }

  template <typename TAB, typename F, typename... ARGS>
  void For_each(const TAB* tab, F&& f, ARGS&&... args) const {
    for (auto it = tab->Begin(); it != tab->End(); ++it) {
      f(this, *it, args...);
    }
  }

private:
  MEM_POOL    _mpool;                 // memory pool for all tables
  CFG         _cfg;                   // control flow graph
  FUNC_SCOPE* _func_scope = nullptr;  // parent function scope
  OPND_TAB*   _opnd_tab;              // operand table
  INST_TAB*   _inst_tab;              // instruction table
  BB_TAB*     _bb_tab;                // CFG node table
  EDGE_TAB*   _edge_tab;              // CFG edge table
  BB_ID       _entry;                 // CFG entry node
  BB_ID       _exit;                  // CFG exit node

};  // CGIR_CONTAINER

using LOOP_INFO_MGR = air::base::LOOP_INFO_MGR<CGIR_CONTAINER, BB_ID, BB_PTR>;
using LOOP_INFO_PTR = LOOP_INFO_MGR::LOOP_INFO_PTR;
using LOOP_INFO_ID  = LOOP_INFO_MGR::LOOP_INFO_ID;

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_CONTAINER_H
