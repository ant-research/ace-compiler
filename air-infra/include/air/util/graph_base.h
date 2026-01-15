//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_GRAPH_BASE_H
#define AIR_UTIL_GRAPH_BASE_H

#include <fstream>

#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
namespace air {
namespace util {

//! @brief A generic graph impl stores node and edge data within a CONTAINER.
//! GRAPH offers APIs for traversal and access to its nodes and edges.
//! The Print_dot method outputs the graph to a dot file.
template <typename CONTAINER>
class GRAPH {
public:
  using NODE_ID   = typename CONTAINER::NODE_ID;
  using EDGE_ID   = typename CONTAINER::EDGE_ID;
  using NODE_PTR  = typename CONTAINER::NODE_PTR;
  using EDGE_PTR  = typename CONTAINER::EDGE_PTR;
  using NODE_ITER = typename CONTAINER::NODE_ITER;
  using EDGE_ITER = typename CONTAINER::EDGE_ITER;

  explicit GRAPH(CONTAINER* cntr) : _cntr(cntr){};
  ~GRAPH() {}

  NODE_PTR  Node(NODE_ID id) const { return _cntr->Node(id); }
  uint32_t  Node_cnt(void) const { return _cntr->Node_cnt(); }
  EDGE_PTR  Edge(EDGE_ID id) const { return _cntr->Edge(id); }
  uint32_t  Edge_cnt(void) const { return _cntr->Edge_cnt(); }
  NODE_ITER Begin_node(void) const { return _cntr->Begin_node(); }
  NODE_ITER End_node(void) const { return _cntr->End_node(); }
  EDGE_ITER Begin_edge(void) const { return _cntr->Begin_edge(); }
  EDGE_ITER End_edge(void) const { return _cntr->End_edge(); }

  //! @brief Output current graph into a dot file named file_name.
  void Print_dot(const char* file_name) const {
    std::ofstream file(file_name, std::ios_base::out);
    file << "digraph {" << std::endl;
    constexpr uint32_t INDENT = 4;
    for (NODE_ITER it = Begin_node(); it != End_node(); ++it) {
      it->Print_dot(file, INDENT);
    }
    file << "}" << std::endl;
  }

  CONTAINER&       Container() { return *_cntr; }
  const CONTAINER& Container() const { return *_cntr; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  GRAPH(void);
  GRAPH(const GRAPH&);
  GRAPH operator=(const GRAPH&);

  CONTAINER* _cntr;
};

}  // namespace util
}  // namespace air
#endif  // AIR_UTIL_GRAPH_BASE_H