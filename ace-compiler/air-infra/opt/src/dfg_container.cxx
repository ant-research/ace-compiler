//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/dfg_container.h"

#include <cstddef>
#include <string>

#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/meta_info.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st_decl.h"
#include "air/opt/dfg_node.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"

namespace air {
namespace opt {
using DFG_NODE_PTR = air::base::PTR<DFG_NODE>;
DFG_NODE_PTR DFG_CONTAINER::Formal(uint32_t id) const {
  return Node(Formal_id(id));
}
DFG_NODE_PTR DFG_CONTAINER::Entry(uint32_t id) const { return Formal(id); }

DFG_NODE_PTR DFG_CONTAINER::New_node(air::base::NODE_PTR node,
                                     air::base::TYPE_PTR type,
                                     uint32_t            opnd_num) {
  return New_node(node->Id(), type->Id(), opnd_num);
}

DFG_NODE_PTR DFG_CONTAINER::New_node(air::base::NODE_ID node,
                                     air::base::TYPE_ID type,
                                     uint32_t           opnd_num) {
  DFG_NODE_DATA_PTR data = New_node_data(node, type, opnd_num);
  return DFG_NODE_PTR(DFG_NODE(this, data));
}

DFG_NODE_PTR DFG_CONTAINER::New_node(air::opt::SSA_VER_PTR ver,
                                     air::base::TYPE_PTR   type,
                                     uint32_t              opnd_num) {
  return New_node(ver->Id(), type->Id(), opnd_num);
}

DFG_NODE_PTR DFG_CONTAINER::New_node(air::opt::SSA_VER_ID ver,
                                     air::base::TYPE_ID   type,
                                     uint32_t             opnd_num) {
  DFG_NODE_DATA_PTR data = New_node_data(ver, type, opnd_num);
  return DFG_NODE_PTR(DFG_NODE(this, data));
}

DFG_NODE_DATA_PTR DFG_CONTAINER::New_node_data(air::base::NODE_ID node,
                                               air::base::TYPE_ID type,
                                               uint32_t           opnd_num) {
  size_t data_size = sizeof(DFG_NODE_DATA) + opnd_num * sizeof(DFG_EDGE_ID);
  DFG_NODE_DATA_PTR data = air::base::Static_cast<DFG_NODE_DATA_PTR>(
      Node_tab()->Malloc(data_size >> 2));
  new (data) DFG_NODE_DATA(node, opnd_num, type);
  Record_node(node, data.Id());
  return data;
}

DFG_NODE_DATA_PTR DFG_CONTAINER::New_node_data(air::opt::SSA_VER_ID ver,
                                               air::base::TYPE_ID   type,
                                               uint32_t             opnd_num) {
  size_t data_size = sizeof(DFG_NODE_DATA) + opnd_num * sizeof(DFG_EDGE_ID);
  DFG_NODE_DATA_PTR data = air::base::Static_cast<DFG_NODE_DATA_PTR>(
      Node_tab()->Malloc(data_size >> 2));
  new (data) DFG_NODE_DATA(ver, opnd_num, type);
  Record_node(ver, data.Id());
  return data;
}

DFG_EDGE_PTR DFG_CONTAINER::New_edge(DFG_NODE_ID dst) {
  DFG_EDGE_DATA_PTR data = Edge_tab()->Allocate<DFG_EDGE_DATA>();
  new (data) DFG_EDGE_DATA(dst);
  return DFG_EDGE_PTR(DFG_EDGE(this, data));
}

DFG_NODE_PTR DFG_CONTAINER::Node(DFG_NODE_ID id) const {
  if (id == air::base::Null_id) return DFG_NODE_PTR();

  DFG_NODE_DATA_PTR data = Node_tab()->Find(id);
  return DFG_NODE_PTR(DFG_NODE(const_cast<DFG_CONTAINER*>(this), data));
}

DFG_EDGE_PTR DFG_CONTAINER::Edge(DFG_EDGE_ID id) const {
  if (id == air::base::Null_id) return DFG_EDGE_PTR();

  DFG_EDGE_DATA_PTR data = Edge_tab()->Find(id);
  return DFG_EDGE_PTR(DFG_EDGE(const_cast<DFG_CONTAINER*>(this), data));
}

DFG_NODE_ID DFG_CONTAINER::Node_id(base::NODE_ID expr) const {
  NODE_MAP::const_iterator iter = _air_node_dfg_node.find(expr.Value());
  if (iter == _air_node_dfg_node.end()) return DFG_NODE_ID();
  return DFG_NODE_ID(iter->second);
}

DFG_NODE_ID DFG_CONTAINER::Node_id(SSA_VER_ID ver) const {
  NODE_MAP::const_iterator iter = _ssa_ver_dfg_node.find(ver.Value());
  if (iter == _ssa_ver_dfg_node.end()) return DFG_NODE_ID();
  return DFG_NODE_ID(iter->second);
}

DFG_NODE_PTR DFG_CONTAINER::Get_node(air::base::NODE_ID node,
                                     air::base::TYPE_ID type,
                                     uint32_t           opnd_num) {
  DFG_NODE_ID dfg_node = this->Node_id(node);
  return (dfg_node != base::Null_id) ? Node(dfg_node)
                                     : New_node(node, type, opnd_num);
}

DFG_NODE_PTR DFG_CONTAINER::Get_node(SSA_VER_ID ver, base::TYPE_ID type,
                                     uint32_t opnd_num) {
  DFG_NODE_ID dfg_node = this->Node_id(ver);
  return (dfg_node != base::Null_id) ? Node(dfg_node)
                                     : New_node(ver, type, opnd_num);
}

DFG_EDGE_PTR DFG_CONTAINER::Get_edge(DFG_NODE_ID src, DFG_NODE_ID dst) {
  AIR_ASSERT(src != base::Null_id);
  AIR_ASSERT(dst != base::Null_id);

  DFG_NODE_PTR src_node = Node(src);
  for (DFG_EDGE_ITER iter = src_node->Begin_succ();
       iter != src_node->End_succ(); ++iter) {
    if (iter->Dst()->Id() == dst) return *iter;
  }

  return New_edge(dst);
}

DFG_NODE_PTR DFG_CONTAINER::NODE_ITER::operator*() const {
  DFG_NODE_ID id(*Iter());
  AIR_ASSERT_MSG(id != base::Null_id, "invalid DFG node idx");
  return _cntr->Node(id);
}

DFG_NODE_PTR DFG_CONTAINER::NODE_ITER::operator->() const {
  DFG_NODE_ID id(*Iter());
  AIR_ASSERT_MSG(id != base::Null_id, "invalid DFG node idx");
  return _cntr->Node(id);
}

DFG_EDGE_PTR DFG_CONTAINER::EDGE_ITER::operator*() const {
  DFG_EDGE_ID id(*Iter());
  AIR_ASSERT_MSG(id != base::Null_id, "invalid DFG node idx");
  return _cntr->Edge(id);
}

DFG_EDGE_PTR DFG_CONTAINER::EDGE_ITER::operator->() const {
  DFG_EDGE_ID id(*Iter());
  AIR_ASSERT_MSG(id != base::Null_id, "invalid DFG node idx");
  return _cntr->Edge(id);
}

}  // namespace opt
}  // namespace air