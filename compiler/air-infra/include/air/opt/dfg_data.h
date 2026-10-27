//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DFG_DATA_H
#define AIR_OPT_DFG_DATA_H

#include <climits>

#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st_decl.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

class DFG_NODE;
using DFG_NODE_PTR = air::base::PTR<DFG_NODE>;
class DFG_EDGE;
using DFG_EDGE_PTR = air::base::PTR<DFG_EDGE>;

class DFG_NODE_DATA;
using DFG_NODE_ID       = air::base::ID<DFG_NODE_DATA>;
using DFG_NODE_DATA_PTR = air::base::PTR_FROM_DATA<DFG_NODE_DATA>;

class DFG_EDGE_DATA;
using DFG_EDGE_ID       = air::base::ID<DFG_EDGE_DATA>;
using DFG_EDGE_DATA_PTR = air::base::PTR_FROM_DATA<DFG_EDGE_DATA>;
class DFG_REGION;
using DFG_REGION_ID = air::base::ID<DFG_REGION>;

//! @brief TRAV_STATE_RAW indicates an unvisited node under analysis;
//! TRAV_STATE_PROCESSING indicates a node currently in processing;
//! TRAV_STATE_VISITED indicates a node that has been processed and does not
//! require revisiting.
enum TRAV_STATE {
  TRAV_STATE_RAW        = 0,
  TRAV_STATE_PROCESSING = 1,
  TRAV_STATE_VISITED    = 2,
};

//! @brief DATA_NODE_EXPR signifies a data flow node representing an expression;
//! DATA_NODE_SSA_VER signifies a data flow node representing a SSA version.
enum DFG_NODE_KIND {
  DATA_NODE_EXPR    = 0,
  DATA_NODE_SSA_VER = 1,
};

//! @brief DFG_EDGE_DATA contains the ID of dst node and the ID of the next
//! edge. The next edge links the src node and its next use-site.
class DFG_EDGE_DATA {
public:
  friend class DFG_CONTAINER;
  friend class DFG_EDGE;
  explicit DFG_EDGE_DATA(DFG_NODE_ID dst)
      : _dst(dst), _next_edge(air::base::Null_id) {}
  ~DFG_EDGE_DATA() {}

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_EDGE_DATA(void);
  DFG_EDGE_DATA(const DFG_EDGE_DATA&);
  DFG_EDGE_DATA operator=(const DFG_EDGE_DATA);

  DFG_NODE_ID Dst(void) const { return _dst; }
  DFG_EDGE_ID Next(void) const { return _next_edge; }
  void        Set_next(DFG_EDGE_ID next) { _next_edge = next; }

  DFG_NODE_ID _dst;
  DFG_EDGE_ID _next_edge;
};

//! @brief DFG_NODE_DATA contains ID of the recorded data(_data), ID of
//! pred(_opnd) nodes, ID of edges(_succ_edge) linking succ nodes, and
//! frequency(_freq). In current impl, the DFG node is constructed with SSA form
//! IR, allowing the data ID to be NODE_ID and SSA_VER_ID. DFG_NODE_DATA
//! provides accessing API for each field.
class DFG_NODE_DATA {
public:
  friend class DFG_CONTAINER;
  friend class DFG_NODE;

  using NODE_PTR    = air::base::NODE_PTR;
  using NODE_ID     = air::base::NODE_ID;
  using SSA_VER_PTR = air::opt::SSA_VER_PTR;
  using SSA_VER_ID  = air::opt::SSA_VER_ID;
  using TYPE_ID     = air::base::TYPE_ID;

  DFG_NODE_DATA(NODE_ID node, uint32_t opnd_cnt, TYPE_ID type)
      : _data(node),
        _node_kind(DFG_NODE_KIND::DATA_NODE_EXPR),
        _opnd_cnt(opnd_cnt),
        _type(type),
        _succ_edge(air::base::Null_id) {
    for (uint32_t id = 0; id < _opnd_cnt; ++id) {
      _opnd[id] = air::base::Null_id;
    }
  }

  DFG_NODE_DATA(SSA_VER_ID ver, uint32_t opnd_cnt, TYPE_ID type)
      : _data(ver),
        _node_kind(DFG_NODE_KIND::DATA_NODE_SSA_VER),
        _opnd_cnt(opnd_cnt),
        _type(type),
        _succ_edge(air::base::Null_id) {
    for (uint32_t id = 0; id < _opnd_cnt; ++id) {
      _opnd[id] = air::base::Null_id;
    }
  }
  ~DFG_NODE_DATA() {}

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_NODE_DATA(void);
  DFG_NODE_DATA(const DFG_NODE_DATA&);
  DFG_NODE_DATA operator=(const DFG_NODE_DATA&);

  const air::base::TYPE_ID Type() const { return _type; }
  DFG_NODE_ID*             Begin_opnd() { return _opnd; }
  DFG_NODE_ID*             End_opnd() { return _opnd + _opnd_cnt; }

  DFG_EDGE_ID Succ() { return _succ_edge; }
  void        Set_succ(DFG_EDGE_ID succ) { _succ_edge = succ; }

  void     Set_freq(uint32_t freq) { _freq = freq; }
  uint32_t Freq(void) const { return _freq; }

  bool Is_ssa_ver() const {
    return _node_kind == DFG_NODE_KIND::DATA_NODE_SSA_VER;
  }
  bool Is_node() const { return _node_kind == DFG_NODE_KIND::DATA_NODE_EXPR; }
  NODE_ID Node() const {
    AIR_ASSERT(Is_node());
    return _data._node;
  }
  SSA_VER_ID Ssa_ver() const {
    AIR_ASSERT(Is_ssa_ver());
    return _data._ssa_ver;
  }

  void       Set_trav_state(TRAV_STATE state) { _trav_state = state; }
  TRAV_STATE Trav_state(void) const { return _trav_state; }

  uint32_t Opnd_cnt(void) const { return _opnd_cnt; }
  void     Set_opnd(uint32_t opnd_id, DFG_NODE_ID opnd) {
    AIR_ASSERT_MSG(opnd_id < Opnd_cnt(), "opnd id out of range");
    _opnd[opnd_id] = opnd;
  }
  DFG_NODE_ID Opnd(uint32_t opnd_id) const {
    AIR_ASSERT_MSG(opnd_id < Opnd_cnt(), "opnd id out of range");
    return _opnd[opnd_id];
  }
  bool Is_opnd(uint32_t id, air::base::NODE_PTR node) const;
  bool Is_opnd(uint32_t id, air::opt::SSA_VER_PTR ver) const;
  void Print_dot(std::ostream& out) const;

  union DATA {
    SSA_VER_ID _ssa_ver;
    NODE_ID    _node;
    DATA(NODE_ID node) : _node(node) {}
    DATA(SSA_VER_ID ver) : _ssa_ver(ver) {}
  } _data;
  DFG_NODE_KIND      _node_kind;
  air::base::TYPE_ID _type;
  uint32_t           _freq       = 0;
  uint32_t           _opnd_cnt   = 0;
  TRAV_STATE         _trav_state = TRAV_STATE_RAW;
  DFG_EDGE_ID        _succ_edge;
  DFG_NODE_ID        _opnd[0];
};

}  // namespace opt
}  // namespace air

#endif  // AIR_OPT_DFG_DATA_H