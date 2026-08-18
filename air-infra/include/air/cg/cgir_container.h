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
#include "air/cg/cgir_node.h"
#include "air/util/cfg_base.h"

namespace air {

namespace cg {

//! @brief CGIR_CONTAINER
//   Contains tables for operands, instructions, CFG nodes and edges. Also
//   provides iterators and access APIs for these objects.
class CGIR_CONTAINER {
  friend air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>;
  using CFG           = air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>;
  using NODE_ID       = CFG::NODE_ID;
  using EDGE_ID       = CFG::EDGE_ID;
  using NODE_DATA_PTR = CFG::NODE_DATA_PTR;
  using EDGE_DATA_PTR = CFG::EDGE_DATA_PTR;
  using OPND_TAB = air::base::ARENA<sizeof(OPND_DATA), sizeof(OPND_ID), false>;
  using INST_TAB = air::base::ARENA<sizeof(INST_ID), sizeof(INST_ID), false>;
  using NODE_TAB = air::base::ARENA<sizeof(NODE_DATA), sizeof(NODE_ID), false>;
  using EDGE_TAB = air::base::ARENA<sizeof(EDGE_DATA), sizeof(EDGE_ID), false>;

  static constexpr uint32_t OPND_TAB_KIND = 0x50001;
  static constexpr uint32_t INST_TAB_KIND = 0x50002;
  static constexpr uint32_t NODE_TAB_KIND = 0x50003;
  static constexpr uint32_t EDGE_TAB_KIND = 0x50004;

public:
  OPND_PTR Opnd(OPND_ID id) const { return OPND_PTR(OPND(this, Find(id))); }
  INST_PTR Inst(INST_ID id) const { return INST_PTR(INST(this, Find(id))); }
  NODE_PTR Node(NODE_ID id) const { return NODE_PTR(NODE(this, Find(id))); }
  EDGE_PTR Edge(EDGE_ID id) const { return EDGE_PTR(EDGE(this, Find(id))); }

  uint32_t Opnd_count() const { return _opnd_tab->Size(); }
  uint32_t Inst_count() const { return _inst_tab->Size(); }
  uint32_t Node_count() const { return _node_tab->Size(); }
  uint32_t Edge_count() const { return _edge_tab->Size(); }

public:
  OPND_PTR New_opnd(REG_CLASS reg_cls, uint8_t reg_num = REG_UNKNOWN);
  OPND_PTR New_opnd(int64_t val);
  OPND_PTR New_opnd(air::base::SYM_ID sym, int64_t ofst = 0);
  OPND_PTR New_opnd(air::base::CONSTANT_ID cst, int64_t ofst = 0);
  OPND_PTR New_opnd(air::base::LABEL_ID label);
  INST_PTR New_inst(air::base::SPOS spos, uint32_t isa, uint32_t opcode);
  INST_PTR New_inst(air::base::SPOS spos, uint32_t isa, uint32_t opcode,
                    uint8_t res_cnt, uint8_t opnd_cnt);
  INST_PTR New_inst(air::base::SPOS spos, uint32_t isa, uint32_t opcode,
                    OPND_ID res_opnd, ...);

public:
  CFG&     Cfg() { return _cfg; }
  NODE_ID  Entry_id() const { return _entry; }
  NODE_ID  Exit_id() const { return _exit; }
  NODE_PTR Entry() const { return Node(_entry); }
  NODE_PTR Exit() const { return Node(_exit); }
  NODE_PTR New_node();
  EDGE_PTR Connect(NODE_PTR pred, NODE_PTR succ);
  void     Set_entry(NODE_PTR node) { Connect(Entry(), node); }
  void     Set_exit(NODE_PTR node) { Connect(node, Exit()); }

public:
  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

public:
  CGIR_CONTAINER();
  ~CGIR_CONTAINER();

private:
  const OPND_TAB* Opnd_tab() const { return _opnd_tab; }
  const INST_TAB* Inst_tab() const { return _inst_tab; }
  const NODE_TAB* Node_tab() const { return _node_tab; }
  const EDGE_TAB* Edge_tab() const { return _edge_tab; }

  OPND_DATA_PTR New_opnd_data() { return _opnd_tab->Allocate<OPND_DATA>(); }
  INST_DATA_PTR New_inst_data(uint32_t res_opnd);
  NODE_DATA_PTR New_node_data() { return _node_tab->Allocate<NODE_DATA>(); }
  EDGE_DATA_PTR New_edge_data() { return _edge_tab->Allocate<EDGE_DATA>(); }

  OPND_DATA_PTR Find(OPND_ID id) const {
    return id != air::base::Null_id ? _opnd_tab->Find(id) : OPND_DATA_PTR();
  }
  INST_DATA_PTR Find(INST_ID id) const {
    return id != air::base::Null_id ? _inst_tab->Find(id) : INST_DATA_PTR();
  }
  NODE_DATA_PTR Find(NODE_ID id) const {
    return id != air::base::Null_id ? _node_tab->Find(id) : NODE_DATA_PTR();
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
  MEM_POOL  _mpool;     // memory pool for all tables
  CFG       _cfg;       // control flow graph
  OPND_TAB* _opnd_tab;  // operand table
  INST_TAB* _inst_tab;  // instruction table
  NODE_TAB* _node_tab;  // CFG node table
  EDGE_TAB* _edge_tab;  // CFG edge table
  NODE_ID   _entry;     // CFG entry node
  NODE_ID   _exit;      // CFG exit node

};  // CGIR_CONTAINER

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_CONTAINER_H
