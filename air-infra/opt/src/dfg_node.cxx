//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/dfg_node.h"

#include <string>

#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/meta_info.h"
#include "air/base/st_decl.h"
#include "air/opt/dfg_container.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

DFG_EDGE_PTR DFG_EDGE_ITER::operator*() const {
  if (_edge == air::base::Null_id) {
    return DFG_EDGE_PTR();
  }
  return Cntr()->Edge(_edge);
}

DFG_EDGE_PTR DFG_EDGE_ITER::operator->() const {
  if (_edge == air::base::Null_id) {
    return DFG_EDGE_PTR();
  }
  return Cntr()->Edge(_edge);
}

DFG_EDGE_ITER DFG_EDGE_ITER::operator++() {
  AIR_ASSERT_MSG(_edge != air::base::Null_id, "");
  DFG_EDGE_PTR edge = _cntr->Edge(_edge);
  _edge             = edge->Next_id();
  return DFG_EDGE_ITER(_cntr, _edge);
}

DFG_EDGE_ITER DFG_EDGE_ITER::operator++(int) {
  AIR_ASSERT_MSG(_edge != air::base::Null_id, "");
  DFG_EDGE_PTR edge = _cntr->Edge(_edge);
  _edge             = edge->Next_id();
  return DFG_EDGE_ITER(_cntr, edge->Id());
}

bool DFG_EDGE_ITER::operator==(const DFG_EDGE_ITER& other) const {
  return Cntr() == other.Cntr() && _edge == other->Id();
}

bool DFG_EDGE_ITER::operator!=(const DFG_EDGE_ITER& other) const {
  return !(*this == other);
}

DFG_EDGE_PTR DFG_EDGE::Next() const {
  AIR_ASSERT_MSG(Next_id() != air::base::Null_id, "Next edge is invalid");
  return Cntr()->Edge(Next_id());
}

DFG_NODE_PTR DFG_EDGE::Dst() const {
  DFG_NODE_ID dst_id = Dst_id();
  AIR_ASSERT_MSG(dst_id != air::base::Null_id, "Invalid dst node");
  return Cntr()->Node(dst_id);
}

DFG_NODE_PTR DFG_NODE::Opnd(uint32_t id) const {
  DFG_NODE_ID opnd_id = Data()->Opnd(id);
  if (opnd_id == base::Null_id) return DFG_NODE_PTR();

  DFG_NODE_PTR opnd_node = Cntr()->Node(opnd_id);
  return opnd_node;
}

air::opt::SSA_VER_PTR DFG_NODE::Ssa_ver(void) const {
  AIR_ASSERT(Is_ssa_ver());
  return Cntr()->Ssa_cntr()->Ver(Data()->Ssa_ver());
}

air::base::NODE_PTR DFG_NODE::Node(void) const {
  AIR_ASSERT(Is_node());
  return Cntr()->Cntr()->Node(Data()->Node());
}

DFG_EDGE_PTR DFG_NODE::Succ() const {
  if (Succ_id() == air::base::Null_id) {
    return DFG_EDGE_PTR();
  }
  return Cntr()->Edge(Succ_id());
}

bool DFG_NODE::Is_succ(DFG_NODE_ID node) const {
  for (DFG_EDGE_ITER iter = Begin_succ(); iter != End_succ(); ++iter) {
    DFG_EDGE_PTR edge = *iter;
    if (edge->Dst_id() == node) return true;
  }
  return false;
}

bool DFG_NODE::Is_opnd(uint32_t id, air::base::NODE_PTR node) const {
  AIR_ASSERT_MSG(id < Data()->Opnd_cnt(), "opnd id out of range");
  DFG_NODE_ID opnd_id = Data()->Opnd(id);
  if (opnd_id == air::base::Null_id) return false;

  DFG_NODE_PTR opnd = Cntr()->Node(opnd_id);
  return opnd->Is_node() && opnd->Node_id() == node->Id();
}

bool DFG_NODE::Is_opnd(uint32_t id, DFG_NODE_ID node) const {
  AIR_ASSERT_MSG(id < Data()->Opnd_cnt(), "opnd id out of range");
  DFG_NODE_ID opnd = Data()->Opnd(id);
  return opnd == node;
}

void DFG_NODE::Set_opnd(uint32_t id, DFG_NODE_PTR opnd) {
  bool is_opnd = Is_opnd(id, opnd->Id());
  if (is_opnd) return;
  Data()->Set_opnd(id, opnd->Id());

  // An edge already exists linking the current node to opnd.
  if (opnd->Is_succ(Id())) return;

  DFG_EDGE_PTR edge = Cntr()->New_edge(Id());
  DFG_EDGE_ID  succ = opnd->Succ_id();
  if (succ != base::Null_id) {
    edge->Set_next(succ);
  }
  opnd->Set_succ(edge->Id());
}

void DFG_NODE::Set_opnd(uint32_t id, DFG_NODE_ID opnd) {
  if (opnd == base::Null_id) return;
  Set_opnd(id, Cntr()->Node(opnd));
}

void DFG_NODE::Print(std::ostream& os, uint32_t indent = 0) const {
  std::string ind(indent, ' ');
  os << ind << To_str();
}

void DFG_NODE::Print() const { Print(std::cout, 0); }

std::string DFG_NODE::To_str() const {
  std::string str = "ID" + std::to_string(Id().Value()) + "_";
  if (Is_ssa_ver()) {
    air::opt::SSA_VER_PTR ver = Ssa_ver();
    str += ver->To_str();
  } else if (Is_node()) {
    air::base::NODE_PTR node = Node();
    str += "_IR" + std::to_string(node->Id().Value()) + "_";
    str += air::base::META_INFO::Op_name(node->Opcode());
  }
  str += "_Freq" + std::to_string(Freq());

  // to support dot file, replace '.' with '_'
  for (uint32_t id = 0; id < str.size(); ++id) {
    if (str[id] == '.') str[id] = '_';
  }
  return str;
}

void DFG_NODE::Print_dot(std::ostream& os, uint32_t indent) const {
  std::string name(To_str());
  std::string indent_str(indent, ' ');
  // 1. DFG node has no successor, print the name of itself.
  if (Succ_id() == base::Null_id) {
    os << indent_str << name << std::endl;
    return;
  }
  for (DFG_EDGE_ITER it = Begin_succ(); it != End_succ(); ++it) {
    DFG_NODE_PTR dst = it->Dst();
    os << indent_str << name << " -> " << dst->To_str() << std::endl;
  }
}

}  // namespace opt
}  // namespace air