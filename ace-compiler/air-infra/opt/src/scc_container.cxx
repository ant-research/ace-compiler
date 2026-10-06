//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/scc_container.h"

#include "air/base/ptr_wrapper.h"
#include "air/opt/scc_node.h"

namespace air {
namespace opt {

SCC_NODE_PTR SCC_CONTAINER::Node(SCC_NODE_ID node) const {
  if (node == air::base::Null_id) return SCC_NODE_PTR();

  SCC_NODE_DATA_PTR data = Node_tab()->Find(node);
  return SCC_NODE_PTR(SCC_NODE(const_cast<SCC_CONTAINER*>(this), data));
}

SCC_EDGE_PTR SCC_CONTAINER::Edge(SCC_EDGE_ID edge) const {
  if (edge == air::base::Null_id) return SCC_EDGE_PTR();

  SCC_EDGE_DATA_PTR data = Edge_tab()->Find(edge);
  return SCC_EDGE_PTR(SCC_EDGE(const_cast<SCC_CONTAINER*>(this), data));
}

SCC_NODE_PTR SCC_CONTAINER::New_scc() {
  SCC_NODE_DATA_PTR data = air::base::Static_cast<SCC_NODE_DATA_PTR>(
      Node_tab()->Malloc(sizeof(SCC_NODE_DATA) >> 2));
  new (data) SCC_NODE_DATA(&_mem_pool);
  return SCC_NODE_PTR(SCC_NODE(this, data));
}

SCC_EDGE_PTR SCC_CONTAINER::New_edge(SCC_NODE_ID dst) {
  SCC_EDGE_DATA_PTR data = air::base::Static_cast<SCC_EDGE_DATA_PTR>(
      Edge_tab()->Malloc(sizeof(SCC_EDGE_DATA) >> 2));
  new (data) SCC_EDGE_DATA(dst);
  return SCC_EDGE_PTR(SCC_EDGE(this, data));
}

SCC_NODE_PTR SCC_CONTAINER::NODE_ITER::operator*() const {
  AIR_ASSERT(_iter != _cntr->_node_tab->End());
  return _cntr->Node(SCC_NODE_ID(*_iter));
}

SCC_NODE_PTR SCC_CONTAINER::NODE_ITER::operator->() const {
  AIR_ASSERT(_iter != _cntr->_node_tab->End());
  return _cntr->Node(SCC_NODE_ID(*_iter));
}

SCC_CONTAINER::NODE_ITER& SCC_CONTAINER::NODE_ITER::operator++() {
  AIR_ASSERT(_iter != _cntr->Node_tab()->End());
  ++_iter;
  return *this;
}

bool SCC_CONTAINER::NODE_ITER::operator==(const NODE_ITER& other) const {
  return _cntr == other._cntr && _iter == other._iter;
}

bool SCC_CONTAINER::NODE_ITER::operator!=(const NODE_ITER& other) const {
  return !(*this == other);
}

SCC_EDGE_PTR SCC_CONTAINER::EDGE_ITER::operator*() const {
  AIR_ASSERT(_iter != _cntr->_edge_tab->End());
  return _cntr->Edge(SCC_EDGE_ID(*_iter));
}

SCC_EDGE_PTR SCC_CONTAINER::EDGE_ITER::operator->() const {
  AIR_ASSERT(_iter != _cntr->_edge_tab->End());
  return _cntr->Edge(SCC_EDGE_ID(*_iter));
}

SCC_CONTAINER::EDGE_ITER& SCC_CONTAINER::EDGE_ITER::operator++() {
  AIR_ASSERT(_iter != _cntr->Edge_tab()->End());
  ++_iter;
  return *this;
}

bool SCC_CONTAINER::EDGE_ITER::operator==(const EDGE_ITER& other) const {
  return _cntr == other._cntr && _iter == other._iter;
}

bool SCC_CONTAINER::EDGE_ITER::operator!=(const EDGE_ITER& other) const {
  return !(*this == other);
}

}  // namespace opt
}  // namespace air