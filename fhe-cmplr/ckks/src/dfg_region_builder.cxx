//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "dfg_region_builder.h"

#include <algorithm>
#include <cstdint>
#include <list>
#include <vector>

#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/node.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/dfg_data.h"
#include "air/opt/dfg_node.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_build.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace ckks {
#define BYPASS_EDGE_THRESHOLD 11

using namespace air::opt;

R_CODE REGION_BUILDER::Validate_scc_node(FUNC_ID func) {
  const DFG_CONTAINER* dfg_cntr = Dfg_cntr(func);
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  for (auto iter = scc_cntr->Begin_node(); iter != scc_cntr->End_node();
       ++iter) {
    AIR_ASSERT(iter->Elem_cnt() > 0);
    if (iter->Elem_cnt() == 1) continue;
    for (SCC_NODE::ELEM_ITER elem_iter = iter->Begin_elem();
         elem_iter != iter->End_elem(); ++elem_iter) {
      CONST_DFG_NODE_PTR dfg_node  = dfg_cntr->Node(DFG_NODE_ID(*elem_iter));
      uint32_t           mul_depth = Consume_mul_depth(dfg_node);
      AIR_ASSERT_MSG(
          mul_depth == 0, "CKKS::mul occurs in circle of data flow graph, ",
          "scale of resulting ciphertext is not statically determined");
    }
    return R_CODE::INTERNAL;
  }
  return R_CODE::NORMAL;
}

void REGION_BUILDER::Build_dfg() {
  air::base::FUNC_SCOPE* entry_func = nullptr;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER iter = Glob_scope()->Begin_func_scope();
       iter != Glob_scope()->End_func_scope(); ++iter) {
    air::base::FUNC_SCOPE& func_scope = *iter;
    if (func_scope.Owning_func()->Entry_point()->Is_program_entry()) {
      entry_func = &func_scope;
      Region_cntr()->Set_entry_func(func_scope.Id());
    }

    // 1. build SSA
    SSA_CONTAINER*        ssa_cntr = Region_cntr()->Get_ssa_cntr(&func_scope);
    air::opt::SSA_BUILDER ssa_builder(&func_scope, ssa_cntr, Driver_ctx());
    ssa_builder.Perform();

    // 3. build DFG
    FUNC_ID        func     = func_scope.Id();
    DFG_CONTAINER* dfg_cntr = Region_cntr()->Get_dfg_cntr(func);
    dfg_cntr->Set_ssa_cntr(ssa_cntr);

    DFG                     dfg(dfg_cntr);
    air::opt::DFG_BUILDER<> dfg_builder(&dfg, &func_scope, ssa_cntr, {});
    dfg_builder.Perform();
  }
}

void REGION_BUILDER::Link_param(const CALLSITE_INFO& callsite) {
  FUNC_ID caller = callsite.Caller();
  AIR_ASSERT(caller == Region_cntr()->Entry_func());
  DFG_NODE_PTR call_node = Dfg_cntr(caller)->Node(callsite.Node());

  FUNC_ID              callee          = callsite.Callee();
  const DFG_CONTAINER* callee_dfg_cntr = Dfg_cntr(callee);
  // 1. link actual parameters with formals
  for (uint32_t id = 0; id < call_node->Opnd_cnt(); ++id) {
    DFG_NODE_PTR opnd_node = call_node->Opnd(id);
    if (!Is_cipher(caller, opnd_node)) continue;
    REGION_ELEM_ID opnd_elem_id = Region_cntr()->Get_elem(
        opnd_node->Id(), opnd_node->Opnd_cnt(), CALLSITE_INFO(caller));
    REGION_ELEM_PTR opnd_elem = Region_cntr()->Region_elem(opnd_elem_id);

    DFG_NODE_ID    formal_node = callee_dfg_cntr->Formal_id(id);
    REGION_ELEM_ID formal_elem_id =
        Region_cntr()->Get_elem(formal_node, 1, callsite);
    REGION_ELEM_PTR formal_elem = Region_cntr()->Region_elem(formal_elem_id);
    formal_elem->Set_pred(0, opnd_elem);
  }

  // 2. link retv with ret_preg
  REGION_ELEM_ID ret_preg_elem_id = Region_cntr()->Get_elem(
      call_node->Succ()->Dst_id(), 1, CALLSITE_INFO(caller));
  REGION_ELEM_PTR ret_preg_elem = Region_cntr()->Region_elem(ret_preg_elem_id);
  uint32_t        retv_cnt      = callee_dfg_cntr->Retv_cnt();
  AIR_ASSERT(retv_cnt == 1);
  for (DFG_CONTAINER::DFG_NODE_VEC_ITER iter = callee_dfg_cntr->Begin_retv();
       iter != callee_dfg_cntr->End_retv(); ++iter) {
    REGION_ELEM_ID  retv_elem_id = Region_cntr()->Get_elem(*iter, 1, callsite);
    REGION_ELEM_PTR retv_elem    = Region_cntr()->Region_elem(retv_elem_id);
    ret_preg_elem->Set_pred(0, retv_elem);
  }
}

void REGION_BUILDER::Handle_callsite(const CALLSITE_INFO& callsite) {
  FUNC_ID                  func     = callsite.Callee();
  const DFG_CONTAINER*     dfg_cntr = Dfg_cntr(func);
  std::list<CALLSITE_INFO> callsite_info;
  for (DFG_CONTAINER::NODE_ITER node_iter = dfg_cntr->Begin_node();
       node_iter != dfg_cntr->End_node(); ++node_iter) {
    if (!Is_cipher(func, *node_iter)) continue;
    if (node_iter->Is_node() && node_iter->Node()->Is_call()) {
      FUNC_ID callee = node_iter->Node()->Entry()->Owning_func_id();
      Link_param(CALLSITE_INFO(func, callee, node_iter->Id()));
      callsite_info.emplace_back(CALLSITE_INFO(func, callee, node_iter->Id()));
      continue;
    }

    REGION_ELEM_ID elem_id = Region_cntr()->Get_elem(
        node_iter->Id(), node_iter->Opnd_cnt(), callsite);
    REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
    for (uint32_t id = 0; id < node_iter->Opnd_cnt(); ++id) {
      DFG_NODE_PTR opnd = node_iter->Opnd(id);
      if (opnd == DFG_NODE_PTR()) continue;
      if (!Is_cipher(func, opnd)) continue;
      REGION_ELEM_ID opnd_elem_id =
          Region_cntr()->Get_elem(opnd->Id(), opnd->Opnd_cnt(), callsite);
      REGION_ELEM_PTR opnd_elem = Region_cntr()->Region_elem(opnd_elem_id);
      elem->Set_pred(id, opnd_elem);
    }
  }

  for (CALLSITE_INFO& call_info : callsite_info) {
    Handle_callsite(call_info);
  }
}

void REGION_BUILDER::Build_scc() {
  // 1. setup entry nodes
  FUNC_ID              entry_func = Region_cntr()->Entry_func();
  CALLSITE_INFO        entry_call_info(entry_func);
  const DFG_CONTAINER* dfg_cntr = Dfg_cntr(entry_func);
  for (uint32_t id = 0; id < dfg_cntr->Entry_cnt(); ++id) {
    DFG_NODE_ID    entry_id = dfg_cntr->Entry_id(id);
    REGION_ELEM_ID entry_elem_id =
        Region_cntr()->Get_elem(entry_id, 0, entry_call_info);
    Region_cntr()->Add_entry(entry_elem_id);
  }

  // 2. build graph
  Handle_callsite(entry_call_info);
  //

  SCC_CONTAINER*          scc_cntr = Region_cntr()->Scc_cntr();
  SCC_GRAPH               scc_graph(scc_cntr);
  REGION_CONTAINER::GRAPH graph(Region_cntr());
  SCC_BUILDER             scc_builder(&graph, &scc_graph);
  scc_builder.Perform();
  scc_cntr->Print_dot(Region_cntr(), "whole_proc_scc.dot");

  // 5. verify SCC
  // Validate_scc_node(func);
}

//! @brief check if SCC_NODE contains node of CKKS::mul
bool REGION_BUILDER::Has_mul(CONST_SCC_NODE_PTR scc_node) {
  for (auto elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID         elem_id(*elem_iter);
    air::opt::DFG_NODE_PTR dfg_node =
        Region_cntr()->Region_elem(elem_id)->Dfg_node();
    if (dfg_node->Is_node() && dfg_node->Node()->Opcode() == OPC_MUL) {
      return true;
    }
  }
  return false;
}

uint32_t REGION_BUILDER::Consume_mul_depth(CONST_DFG_NODE_PTR dfg_node) {
  if (!dfg_node->Is_node()) return 0;
  switch (dfg_node->Node()->Opcode()) {
    case OPC_MUL:
      return 1;
    case air::core::OPC_CALL: {
      air::base::NODE_PTR node           = dfg_node->Node();
      const uint32_t*     mul_depth_attr = node->Attr<uint32_t>(
          Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::MUL_DEPTH));
      AIR_ASSERT_MSG(mul_depth_attr != nullptr,
                     "Encounter call without mul_depth attr");
      return *mul_depth_attr;
    }
    default:
      return 0;
  }
}

//! @brief Return mul_depth of scc_node.
uint32_t REGION_BUILDER::Consume_mul_depth(CONST_SCC_NODE_PTR scc_node) {
  if (scc_node->Elem_cnt() > 1) return 0;
  REGION_ELEM_ID         elem_id(*scc_node->Begin_elem());
  air::opt::DFG_NODE_PTR dfg_node =
      Region_cntr()->Region_elem(elem_id)->Dfg_node();
  return dfg_node->Is_node() && dfg_node->Node()->Opcode() == OPC_MUL;
}

//! @brief Check if SCC node is retv
bool REGION_BUILDER::Is_retv(CONST_SCC_NODE_PTR scc_node) {
  if (scc_node->Elem_cnt() > 1) return false;
  REGION_ELEM_ID         elem_id(*scc_node->Begin_elem());
  air::opt::DFG_NODE_PTR dfg_node =
      Region_cntr()->Region_elem(elem_id)->Dfg_node();
  return dfg_node->Is_node() &&
         dfg_node->Node()->Opcode() == air::core::OPC_RETV;
}

//! @brief Check if SCC node is call
bool REGION_BUILDER::Is_call(CONST_SCC_NODE_PTR scc_node) {
  if (scc_node->Elem_cnt() > 1) return false;
  REGION_ELEM_ID         elem_id(*scc_node->Begin_elem());
  air::opt::DFG_NODE_PTR dfg_node =
      Region_cntr()->Region_elem(elem_id)->Dfg_node();
  return dfg_node->Is_node() &&
         dfg_node->Node()->Opcode() == air::core::OPC_CALL;
}

bool REGION_BUILDER::Is_cipher(CONST_REGION_ELEM_PTR elem) const {
  air::base::TYPE_ID type_id = elem->Dfg_node()->Type();

  air::base::TYPE_PTR type = Glob_scope()->Type(type_id);
  if (type->Is_array()) type_id = type->Cast_to_arr()->Elem_type_id();

  return Lower_ctx()->Is_cipher3_type(type_id) ||
         Lower_ctx()->Is_cipher_type(type_id);
}

bool REGION_BUILDER::Is_cipher(CONST_SCC_NODE_PTR scc_node) const {
  AIR_ASSERT(scc_node->Elem_cnt() > 0);
  SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
  REGION_ELEM_ID      elem_id(*elem_iter);
  return Is_cipher(Region_cntr()->Region_elem(elem_id));
}

bool REGION_BUILDER::Is_cipher(FUNC_ID            func,
                               CONST_DFG_NODE_PTR dfg_node) const {
  air::base::TYPE_ID  type_id = dfg_node->Type();
  air::base::TYPE_PTR type    = Glob_scope()->Type(dfg_node->Type());
  if (type->Is_array()) type_id = type->Cast_to_arr()->Elem_type_id();
  if (Lower_ctx()->Is_cipher_type(type_id) ||
      Lower_ctx()->Is_cipher3_type(type_id)) {
    return true;
  }
  return false;
}

//! @brief Check if SCC node contains ciphertext value
bool REGION_BUILDER::Is_cipher(FUNC_ID            func,
                               CONST_SCC_NODE_PTR scc_node) const {
  for (auto elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    air::opt::DFG_NODE_ID  dfg_node_id(*elem_iter);
    air::opt::DFG_NODE_PTR dfg_node = Dfg_cntr(func)->Node(dfg_node_id);
    if (Is_cipher(func, dfg_node)) return true;
  }
  return false;
}

void REGION_BUILDER::Cal_mul_depth() {
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
       scc_iter != scc_cntr->End_node(); ++scc_iter) {
    scc_iter->Set_trav_state(air::opt::TRAV_STATE_RAW);
  }

  _scc_mul_depth.clear();
  _scc_mul_depth.resize(scc_cntr->Node_cnt(), INVALID_MUL_DEPTH);
  std::list<SCC_NODE_ID> worklist;
  for (uint32_t id = 0; id < Region_cntr()->Entry_cnt(); ++id) {
    REGION_ELEM_PTR formal = Region_cntr()->Entry(id);
    if (!Is_cipher(formal)) continue;

    air::opt::SCC_NODE_ID scc_node_id = scc_cntr->Scc_node(formal->Id());
    // filter non-cipher DFG nodes
    worklist.push_back(scc_node_id);

    Set_mul_depth(scc_node_id, 1);
    CONST_SCC_NODE_PTR scc_node = scc_cntr->Node(scc_node_id);
    scc_node->Set_trav_state(air::opt::TRAV_STATE_PROCESSING);
  }

  while (!worklist.empty()) {
    air::opt::SCC_NODE_ID scc_node_id = worklist.front();
    worklist.pop_front();
    uint32_t               mul_depth = Mul_depth(scc_node_id);
    air::opt::SCC_NODE_PTR scc_node  = scc_cntr->Node(scc_node_id);
    if (mul_depth == INVALID_MUL_DEPTH) {
      worklist.push_back(scc_node_id);
      scc_node->Set_trav_state(air::opt::TRAV_STATE_PROCESSING);
      continue;
    }

    for (air::opt::SCC_NODE::SCC_EDGE_ITER iter = scc_node->Begin_succ();
         iter != scc_node->End_succ(); ++iter) {
      air::opt::SCC_NODE_PTR succ = (*iter)->Dst();
      // 1. skip dead nodes which has no use site
      if (succ->Begin_succ() == succ->End_succ() && !Is_retv(succ)) continue;
      // 2. skip nodes of non-cipher values
      if (!Is_cipher(succ)) continue;

      uint32_t pre_mul_depth = Mul_depth(succ->Id());
      uint32_t mul_depth_inc = Consume_mul_depth(succ);
      uint32_t cur_mul_depth = mul_depth + mul_depth_inc;
      if (pre_mul_depth != INVALID_MUL_DEPTH &&
          cur_mul_depth <= pre_mul_depth) {
        continue;
      }
      Set_mul_depth(succ->Id(), cur_mul_depth);

      if (succ->Trav_state() != air::opt::TRAV_STATE_PROCESSING) {
        worklist.push_back(succ->Id());
      }
    }
    scc_node->Set_trav_state(air::opt::TRAV_STATE_RAW);
  }
}

void REGION_BUILDER::Downward_merge() {
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  using SCC_MUL_DEPTH           = std::vector<std::list<air::opt::SCC_NODE_ID>>;
  SCC_MUL_DEPTH region_scc_node(Max_mul_depth() + 1);
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
       scc_iter != scc_cntr->End_node(); ++scc_iter) {
    uint32_t mul_depth = Mul_depth(scc_iter->Id());
    if (mul_depth == INVALID_MUL_DEPTH) continue;
    region_scc_node[mul_depth].push_back(scc_iter->Id());
  }

  if (region_scc_node.size() <= 1) return;

  for (int32_t mul_depth = Max_mul_depth(); mul_depth > 0; --mul_depth) {
    std::list<SCC_NODE_ID> scc_list = region_scc_node[mul_depth];
    if (scc_list.empty()) continue;

    scc_list.sort();
    uint32_t init_mul_depth = mul_depth;
    while (!scc_list.empty()) {
      SCC_NODE_PTR scc_node = scc_cntr->Node(scc_list.front());
      scc_list.pop_front();
      uint32_t min_succ_region = UINT32_MAX;
      for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
           edge_iter != scc_node->End_succ(); ++edge_iter) {
        SCC_NODE_PTR succ           = edge_iter->Dst();
        uint32_t     succ_mul_depth = Mul_depth(succ->Id());
        if (succ_mul_depth == INVALID_MUL_DEPTH) continue;

        if (Has_mul(succ)) {
          AIR_ASSERT(succ_mul_depth > 1);
          succ_mul_depth -= 1;
        }
        min_succ_region = std::min(min_succ_region, succ_mul_depth);
      }
      if (min_succ_region > init_mul_depth &&
          min_succ_region < (init_mul_depth + BYPASS_EDGE_THRESHOLD)) {
        Set_mul_depth(scc_node->Id(), min_succ_region);
      }
    }
  }
}

void REGION_BUILDER::Collect_region_node() {
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
       scc_iter != scc_cntr->End_node(); ++scc_iter) {
    if (!Is_cipher(*scc_iter)) continue;
    uint32_t mul_depth = Mul_depth(scc_iter->Id());
    if (mul_depth == INVALID_MUL_DEPTH) continue;

    REGION_ID region_id(mul_depth);
    Region_cntr()->Set_scc_region(scc_iter->Id(), region_id);

    REGION_PTR region = Region_cntr()->Get_region(region_id);
    for (air::opt::SCC_NODE::ELEM_ITER iter = scc_iter->Begin_elem();
         iter != scc_iter->End_elem(); ++iter) {
      REGION_ELEM_ID elem_id(*iter);
      region->Add_elem(elem_id);

      REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
      AIR_ASSERT(elem->Region() == air::base::Null_id);
      elem->Set_region(region_id);
    }
  }
}

void REGION_BUILDER::Perform() {
  // 1. build DFG and SCC
  Build_dfg();
  Build_scc();
  // 2.
  Cal_mul_depth();
  Downward_merge();
  //
  Collect_region_node();
  Region_cntr()->Print_dot("whole_proc_region.dot");

  /*
  // 2. cal mul_depth of DFG nodes of entry function
  FUNC_ID entry_func = Region_cntr()->Entry_func();
  Cal_mul_depth(entry_func, CALLSITE_INFO(entry_func));
  Collect_dfg_node(entry_func, FUNC_ID(), DFG_NODE_ID());

  // 2. cal mul_depth of DFG nodes of calleed function
  for (CALL_INFO_MAP::const_iterator iter = Call_info().begin();
       iter != Call_info().end(); ++iter) {
    const CALLSITE_INFO& call_info = iter->first;
    Cal_mul_depth(call_info.Callee(), call_info);
    Downward_merge(call_info.Callee());
    Collect_dfg_node(call_info.Callee(), entry_func, call_info.Node());
  }
  // 2. calculate mul_depth of each DFG node.
  Cal_mul_depth(Region_cntr()->Entry_func(), CALLSITE_INFO());

  Region_cntr()->Print_dot("Region.dot"); */
}

}  // namespace ckks
}  // namespace fhe