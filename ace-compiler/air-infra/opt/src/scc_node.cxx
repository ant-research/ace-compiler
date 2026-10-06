//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/scc_node.h"

#include "air/base/id_wrapper.h"
#include "air/opt/scc_container.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

SCC_NODE_PTR SCC_NODE::SCC_NODE_ITER::operator*() const {
  AIR_ASSERT(Node() != base::Null_id);
  return Cntr()->Node(Node());
}

SCC_NODE_PTR SCC_NODE::SCC_NODE_ITER::operator->() const {
  AIR_ASSERT(Node() != base::Null_id);
  return Cntr()->Node(Node());
}

SCC_EDGE_PTR SCC_NODE::SCC_EDGE_ITER::operator*() const {
  AIR_ASSERT(Edge() != base::Null_id);
  return Cntr()->Edge(Edge());
}

SCC_EDGE_PTR SCC_NODE::SCC_EDGE_ITER::operator->() const {
  AIR_ASSERT(Edge() != base::Null_id);
  return Cntr()->Edge(Edge());
}

SCC_NODE_PTR SCC_EDGE::Dst() {
  AIR_ASSERT(Dst_id() != base::Null_id);
  return Cntr()->Node(Dst_id());
}

bool SCC_NODE::Has_succ(SCC_NODE_ID succ) {
  for (SCC_EDGE_LIST::const_iterator iter = Data()->Begin_succ();
       iter != Data()->End_succ(); ++iter) {
    if ((*iter) == base::Null_id) continue;
    if (Cntr()->Edge(*iter)->Dst_id() == succ) return true;
  }
  return false;
}

void SCC_NODE::Add_succ(SCC_NODE_ID succ) {
  AIR_ASSERT(succ != base::Null_id);
  if (Has_succ(succ)) return;

  SCC_EDGE_PTR edge = Cntr()->New_edge(succ);
  Data()->Add_succ(edge->Id());
  Cntr()->Node(succ)->Add_pred(Id());
}

void SCC_NODE::Print_dot(std::ostream& os, uint32_t indent) {
  std::string name(To_str());
  for (SCC_EDGE_ITER it = Begin_succ(); it != End_succ(); ++it) {
    SCC_NODE_PTR dst = (*it)->Dst();
    os << std::string(indent, ' ') << name << " -> " << dst->To_str()
       << std::endl;
  }
}

}  // namespace opt
}  // namespace air