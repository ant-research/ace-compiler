
//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "dfg_region.h"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_container.h"
#include "air/opt/dfg_data.h"
#include "air/opt/dfg_node.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_decl.h"
#include "air/opt/ssa_st.h"
#include "air/util/debug.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {
namespace ckks {

using namespace air::opt;

REGION_EDGE_PTR REGION_ELEM::EDGE_ITER::operator*() const {
  AIR_ASSERT(Edge() != air::base::Null_id);
  return Cntr()->Elem_edge(Edge());
}

REGION_EDGE_PTR REGION_ELEM::EDGE_ITER::operator->() const {
  AIR_ASSERT(Edge() != air::base::Null_id);
  return Cntr()->Elem_edge(Edge());
}

REGION_ELEM::EDGE_ITER& REGION_ELEM::EDGE_ITER::operator++() {
  AIR_ASSERT(Edge() != air::base::Null_id);
  _edge = (*this)->Next_id();
  return *this;
}

REGION_EDGE_PTR REGION_ELEM::Succ(void) const {
  AIR_ASSERT(Succ_id() != air::base::Null_id);
  return Cntr()->Elem_edge(Succ_id());
}

REGION_ELEM_PTR REGION_ELEM::Pred(uint32_t id) const {
  return Cntr()->Region_elem(Pred_id(id));
}

void REGION_ELEM::Set_pred(uint32_t id, CONST_REGION_ELEM_PTR pred) {
  _data->Set_pred(id, pred->Id());
  if (pred->Has_succ(Id())) return;
  pred->Add_succ(Id());
}

bool REGION_ELEM::Has_succ(REGION_ELEM_ID succ) const {
  for (EDGE_ITER iter = Begin_succ(); iter != End_succ(); ++iter) {
    if (iter->Dst_id() == succ) return true;
  }
  return false;
}

void REGION_ELEM::Add_succ(REGION_ELEM_ID dst) {
  if (Has_succ(dst)) return;
  REGION_EDGE_PTR new_edge = Cntr()->New_edge(dst);
  if (Succ_id() != air::base::Null_id) new_edge->Set_next(Succ_id());
  Set_succ(new_edge->Id());
}

air::opt::DFG_NODE_PTR REGION_ELEM::Dfg_node(void) const {
  AIR_ASSERT(Dfg_node_id() != air::base::Null_id);
  return Cntr()->Dfg_cntr(Parent_func())->Node(Dfg_node_id());
}

air::base::TYPE_PTR REGION_ELEM::Type(void) const {
  return _cntr->Glob_scope()->Type(Type_id());
}

air::base::NODE_PTR REGION_ELEM::Call_node(void) const {
  if (Data()->Call_node() == air::base::Null_id) return air::base::NODE_PTR();

  const DFG_CONTAINER* dfg_cntr = _cntr->Dfg_cntr(Parent_func());
  DFG_NODE_PTR         dfg_node = dfg_cntr->Node(Data()->Call_node());
  return dfg_node->Node();
}

uint32_t REGION_ELEM::Mul_depth(void) const {
  DFG_NODE_PTR dfg_node = Dfg_node();
  if (!dfg_node->Is_node()) return 0;
  air::base::OPCODE opc = dfg_node->Node()->Opcode();
  if (opc == OPC_MUL) return 1;
  AIR_ASSERT_MSG(opc != air::core::OPC_CALL, "not support call node in RESBM");
  return 0;
}

std::string REGION_ELEM::To_str() const {
  std::string str =
      "ELEM" + std::to_string(Id().Value()) + "_" + Dfg_node()->To_str();
  str += "_FUNC_" + std::to_string(Parent_func().Value());
  str += "_CS_" + (Data()->Call_node() == air::base::Null_id
                       ? "INV"
                       : std::to_string(Data()->Call_node().Value()));
  if (Region_id() != air::base::Null_id)
    str += "_REG" + std::to_string(Region_id().Value());
  if (Need_rescale()) str += "_RS";
  if (Need_bootstrap()) str += "_BTS";
  return str;
}

void REGION_ELEM::Print_dot(std::ostream& os, uint32_t indent) const {
  std::string indent_str(indent, ' ');
  for (EDGE_ITER edge_iter = Begin_succ(); edge_iter != End_succ();
       ++edge_iter) {
    os << indent_str << To_str() << " -> " << edge_iter->Dst()->To_str()
       << " [label= \"w=" << edge_iter->Weight() << "\""
       << "]" << std::endl;
  }
}

REGION_ELEM_PTR REGION_EDGE::Dst(void) const {
  REGION_ELEM_ID dst = Dst_id();
  AIR_ASSERT(dst != air::base::Null_id);
  return Cntr()->Region_elem(dst);
}

REGION_EDGE_PTR REGION_EDGE::Next(void) const {
  AIR_ASSERT(Next_id() != air::base::Null_id);
  return Cntr()->Elem_edge(Next_id());
}

REGION_ELEM_PTR REGION::ELEM_ITER::operator*() const {
  return Cntr()->Region_elem(*Iter());
}

REGION_ELEM_PTR REGION::ELEM_ITER::operator->() const {
  return Cntr()->Region_elem(*Iter());
}

void REGION::Print_dot(std::ostream& os, uint32_t indent) const {
  constexpr uint32_t INDENT = 4;
  std::string        indent_str0(indent, ' ');
  os << indent_str0 << "subgraph cluster_" << To_str() << "{" << std::endl;
  std::string indent_str1(indent + INDENT, ' ');
  os << indent_str1 << "label= \"" << To_str() << "\"" << std::endl;
  os << indent_str1 << "style=filled;" << std::endl;
  os << indent_str1 << "color=lightgray;" << std::endl;
  os << indent_str1 << "node [style=filled,color=white];" << std::endl;
  for (ELEM_ITER iter = Begin_elem(); iter != End_elem(); ++iter) {
    os << indent_str1 << iter->To_str() << std::endl;
  }
  os << indent_str0 << "}" << std::endl;
  for (ELEM_ITER iter = Begin_elem(); iter != End_elem(); ++iter) {
    iter->Print_dot(os, indent);
  }
}

REGION_DATA_PTR REGION_CONTAINER::New_region_data() {
  REGION_DATA_PTR data = air::base::Static_cast<REGION_DATA_PTR>(
      Region_tab()->Malloc(sizeof(REGION_DATA) >> 2));
  new (data) REGION_DATA(&Mem_pool());
  return data;
}

REGION_PTR REGION_CONTAINER::New_region() {
  REGION_DATA_PTR data = New_region_data();
  return REGION_PTR(REGION(this, data));
}

REGION_ELEM_DATA_PTR REGION_CONTAINER::New_elem_data(
    air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
    const CALLSITE_INFO& call_site) {
  uint32_t data_size =
      sizeof(REGION_ELEM_DATA) + (pred_cnt + 1) * sizeof(REGION_ELEM_ID);
  REGION_ELEM_DATA_PTR data = air::base::Static_cast<REGION_ELEM_DATA_PTR>(
      Elem_tab()->Malloc(data_size >> 2));
  new (data) REGION_ELEM_DATA(node, pred_cnt, call_site);
  return data;
}

REGION_EDGE_PTR REGION_CONTAINER::New_edge(REGION_ELEM_ID dst) const {
  REGION_EDGE_DATA_PTR data = air::base::Static_cast<REGION_EDGE_DATA_PTR>(
      Edge_tab()->Malloc(sizeof(REGION_EDGE_DATA) >> 2));
  new (data) REGION_EDGE_DATA(dst);
  return REGION_EDGE_PTR(REGION_EDGE(this, data));
}

REGION_ELEM_PTR REGION_CONTAINER::Get_elem(air::opt::DFG_NODE_ID node,
                                           uint32_t              pred_cnt,
                                           const CALLSITE_INFO&  call_site) {
  REGION_ELEM_DATA       data(node, 0, call_site);
  ELEM_DATA2ID::iterator iter = Elem_id().find(data);
  if (iter != Elem_id().end()) return Region_elem(iter->second);

  REGION_ELEM_DATA_PTR data_ptr = New_elem_data(node, pred_cnt, call_site);
  Elem_id()[data]               = data_ptr.Id();
  return REGION_ELEM_PTR(REGION_ELEM(this, data_ptr));
}

void REGION_CONTAINER::Print_dot(const char* file_name) const {
  std::ofstream dot_file(file_name);
  dot_file << "digraph {" << std::endl;
  constexpr uint32_t INDENT = 4;
  for (REGION_ITER iter = Begin_region(); iter != End_region(); ++iter) {
    iter->Print_dot(dot_file, INDENT);
  }
  dot_file << "}" << std::endl;
}

void REGION_CONTAINER::Print_dot(const char* file_name, REGION_ID id) const {
  AIR_ASSERT(id != air::base::Null_id);
  AIR_ASSERT(id.Value() < Region_cnt());

  std::ofstream dot_file(file_name);
  dot_file << "digraph {" << std::endl;
  constexpr uint32_t INDENT = 4;

  REGION_PTR region = Region(id);
  region->Print_dot(dot_file, INDENT);
  dot_file << "}" << std::endl;
}

}  // namespace ckks
}  // namespace fhe