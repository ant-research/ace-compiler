//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "dfg_region_builder.h"

#include <cstdio>

#include "air/opt/dfg_builder.h"
#include "air/opt/dfg_node.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_build.h"
#include "air/util/debug.h"
#include "air/util/error.h"
#include "dfg_region.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {
namespace ckks {

#define BYPASS_EDGE_THRESHOLD 6

using namespace air::opt;

void REGION_BUILDER::Build_dfg() {
  for (GLOB_SCOPE::FUNC_SCOPE_ITER iter = Glob_scope()->Begin_func_scope();
       iter != Glob_scope()->End_func_scope(); ++iter) {
    FUNC_SCOPE* func_scope = &(*iter);
    if (func_scope->Owning_func()->Entry_point()->Is_program_entry()) {
      Region_cntr()->Set_entry_func(func_scope->Id());
    }

    // 1. build SSA
    SSA_CONTAINER*        ssa_cntr = Region_cntr()->Get_ssa_cntr(func_scope);
    air::opt::SSA_BUILDER ssa_builder(func_scope, ssa_cntr, Driver_ctx());
    ssa_builder.Perform();

    // 2. build DFG
    FUNC_ID        func     = func_scope->Id();
    DFG_CONTAINER* dfg_cntr = Region_cntr()->Get_dfg_cntr(func);
    dfg_cntr->Set_ssa_cntr(ssa_cntr);

    DFG                     dfg(dfg_cntr);
    air::opt::DFG_BUILDER<> dfg_builder(&dfg, func_scope, ssa_cntr, {});
    dfg_builder.Perform();
  }
}

void REGION_BUILDER::Link_formal(const CALLSITE_INFO& callsite) {
  FUNC_ID caller = callsite.Caller();
  AIR_ASSERT(caller == Region_cntr()->Entry_func());
  DFG_NODE_PTR call_node = Dfg_cntr(caller)->Node(callsite.Node());

  FUNC_ID              callee          = callsite.Callee();
  const DFG_CONTAINER* callee_dfg_cntr = Dfg_cntr(callee);
  // 1. link actual parameters with formals
  for (uint32_t id = 0; id < call_node->Opnd_cnt(); ++id) {
    if (call_node->Opnd_id(id) == air::base::Null_id) continue;
    DFG_NODE_PTR opnd_node = call_node->Opnd(id);
    if (!Is_cipher(opnd_node)) continue;

    REGION_ELEM_PTR opnd_elem = Region_cntr()->Get_elem(
        opnd_node->Id(), opnd_node->Opnd_cnt(), CALLSITE_INFO(caller));

    DFG_NODE_ID        formal_node     = callee_dfg_cntr->Formal_id(id);
    constexpr uint32_t FORMAL_PRED_CNT = 1;
    REGION_ELEM_PTR    formal_elem =
        Region_cntr()->Get_elem(formal_node, FORMAL_PRED_CNT, callsite);
    formal_elem->Set_pred(0, opnd_elem);
  }

  // 2. link ret node (retv) with returned register of call node (ret_preg)
  REGION_ELEM_PTR ret_preg_elem = Region_cntr()->Get_elem(
      call_node->Succ()->Dst_id(), 1, CALLSITE_INFO(caller));
  uint32_t retv_cnt = callee_dfg_cntr->Retv_cnt();
  if (retv_cnt == 0) {
    AIR_DEBUG("called function has no retv node");
    return;
  }
  AIR_ASSERT_MSG(retv_cnt == 1,
                 "currently, only support function with single retv node");
  for (DFG_CONTAINER::DFG_NODE_VEC_ITER iter = callee_dfg_cntr->Begin_retv();
       iter != callee_dfg_cntr->End_retv(); ++iter) {
    REGION_ELEM_PTR retv_elem = Region_cntr()->Get_elem(*iter, 1, callsite);
    ret_preg_elem->Set_pred(0, retv_elem);
  }
}

void REGION_BUILDER::Handle_callsite(const CALLSITE_INFO& callsite) {
  FUNC_ID                  func     = callsite.Callee();
  const DFG_CONTAINER*     dfg_cntr = Dfg_cntr(func);
  std::list<CALLSITE_INFO> callsite_info;
  for (DFG_CONTAINER::NODE_ITER node_iter = dfg_cntr->Begin_node();
       node_iter != dfg_cntr->End_node(); ++node_iter) {
    // 1. skip non-cipher nodes
    if (!Is_cipher(*node_iter)) continue;

    // 2. handle call nodes
    if (node_iter->Is_node() && node_iter->Node()->Is_call()) {
      FUNC_ID callee = node_iter->Node()->Entry()->Owning_func_id();
      Link_formal(CALLSITE_INFO(func, callee, node_iter->Id()));
      callsite_info.emplace_back(CALLSITE_INFO(func, callee, node_iter->Id()));
      continue;
    }

    // 3. handle non-call nodes
    REGION_ELEM_PTR elem = Region_cntr()->Get_elem(
        node_iter->Id(), node_iter->Opnd_cnt(), callsite);
    for (uint32_t id = 0; id < node_iter->Opnd_cnt(); ++id) {
      DFG_NODE_PTR opnd = node_iter->Opnd(id);
      if (opnd == DFG_NODE_PTR()) continue;
      if (!Is_cipher(opnd)) continue;
      REGION_ELEM_PTR opnd_elem =
          Region_cntr()->Get_elem(opnd->Id(), opnd->Opnd_cnt(), callsite);
      elem->Set_pred(id, opnd_elem);
    }
  }

  // Handle call sites in called function
  for (CALLSITE_INFO& call_info : callsite_info) {
    Handle_callsite(call_info);
  }
}

R_CODE REGION_BUILDER::Validate_scc_node() {
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  for (SCC_CONTAINER::NODE_ITER iter = scc_cntr->Begin_node();
       iter != scc_cntr->End_node(); ++iter) {
    AIR_ASSERT_MSG(iter->Elem_cnt() > 0, "SCC_NODE should not be empty");
    if (iter->Elem_cnt() == 1) continue;
    for (SCC_NODE::ELEM_ITER elem_iter = iter->Begin_elem();
         elem_iter != iter->End_elem(); ++elem_iter) {
      CONST_REGION_ELEM_PTR elem =
          Region_cntr()->Node(REGION_ELEM_ID(*elem_iter));
      uint32_t mul_depth = elem->Mul_depth();
      if (mul_depth == 0) continue;
      AIR_ASSERT_MSG(
          false, "CKKS::mul occurs in circle of data flow graph, ",
          "scale of resulting ciphertext is not statically determined");
      return R_CODE::INTERNAL;
    }
  }
  return R_CODE::NORMAL;
}

R_CODE REGION_BUILDER::Build_scc() {
  // 1. setup entry nodes
  FUNC_ID              entry_func = Region_cntr()->Entry_func();
  CALLSITE_INFO        entry_call_info(entry_func);
  const DFG_CONTAINER* dfg_cntr = Dfg_cntr(entry_func);
  for (uint32_t id = 0; id < dfg_cntr->Entry_cnt(); ++id) {
    DFG_NODE_ID     entry_id = dfg_cntr->Entry_id(id);
    REGION_ELEM_PTR entry_elem =
        Region_cntr()->Get_elem(entry_id, 0, entry_call_info);
    Region_cntr()->Add_entry(entry_elem->Id());
  }

  // 2. build interprocedural data flow graph from the entry function.
  Handle_callsite(entry_call_info);

  // 3. build SCC_NODEs
  SCC_CONTAINER*          scc_cntr = Region_cntr()->Scc_cntr();
  SCC_GRAPH               scc_graph(scc_cntr);
  REGION_CONTAINER::GRAPH graph(Region_cntr());
  SCC_BUILDER             scc_builder(&graph, &scc_graph);
  scc_builder.Perform();

  // 4. verify SCC_NODEs
  R_CODE r_code = Validate_scc_node();
  return r_code;
}

//! @brief Return mul_level consumed by operations in scc_node.
uint32_t REGION_BUILDER::Consume_mul_lev(CONST_SCC_NODE_PTR scc_node) {
  // A SCC_NODE containing multiple elements must not consume mul_level.
  // This is ensured by REGION_BUILDER::Validate_scc_node.
  if (scc_node->Elem_cnt() > 1) return 0;
  REGION_ELEM_ID elem_id(*scc_node->Begin_elem());
  return Region_cntr()->Region_elem(elem_id)->Mul_depth();
}

//! @brief Check if SCC node is retv.
bool REGION_BUILDER::Is_retv(CONST_SCC_NODE_PTR scc_node) {
  if (scc_node->Elem_cnt() > 1) return false;
  REGION_ELEM_ID         elem_id(*scc_node->Begin_elem());
  air::opt::DFG_NODE_PTR dfg_node =
      Region_cntr()->Region_elem(elem_id)->Dfg_node();
  return dfg_node->Is_node() &&
         dfg_node->Node()->Opcode() == air::core::OPC_RETV;
}

//! @brief Check if SCC/DFG node is ciphertext.
bool REGION_BUILDER::Is_cipher(CONST_DFG_NODE_PTR dfg_node) const {
  air::base::TYPE_ID  type_id = dfg_node->Type();
  air::base::TYPE_PTR type    = Glob_scope()->Type(dfg_node->Type());
  if (type->Is_array()) type_id = type->Cast_to_arr()->Elem_type_id();
  if (Lower_ctx()->Is_cipher_type(type_id) ||
      Lower_ctx()->Is_cipher3_type(type_id)) {
    return true;
  }
  return false;
}
bool REGION_BUILDER::Is_cipher(CONST_REGION_ELEM_PTR elem) const {
  return Is_cipher(elem->Dfg_node());
}
bool REGION_BUILDER::Is_cipher(CONST_SCC_NODE_PTR scc_node) const {
  AIR_ASSERT(scc_node->Elem_cnt() > 0);
  SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
  REGION_ELEM_ID      elem_id(*elem_iter);
  return Is_cipher(Region_cntr()->Region_elem(elem_id));
}

void REGION_BUILDER::Cal_mul_depth() {
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  // refresh trav_state of each SCC_NODE.
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
       scc_iter != scc_cntr->End_node(); ++scc_iter) {
    scc_iter->Set_trav_state(TRAV_STATE_RAW);
  }

  Init_scc_mul_depth();
  std::list<SCC_NODE_ID> worklist;
  for (uint32_t id = 0; id < Region_cntr()->Entry_cnt(); ++id) {
    REGION_ELEM_PTR entry_node = Region_cntr()->Entry(id);
    // filter non-cipher nodes
    if (!Is_cipher(entry_node)) continue;

    SCC_NODE_ID scc_node_id = scc_cntr->Scc_node(entry_node->Id());
    worklist.push_back(scc_node_id);

    Set_mul_depth(scc_node_id, 1);
    CONST_SCC_NODE_PTR scc_node = scc_cntr->Node(scc_node_id);
    scc_node->Set_trav_state(TRAV_STATE_PROCESSING);
  }

  while (!worklist.empty()) {
    SCC_NODE_ID scc_node_id = worklist.front();
    worklist.pop_front();
    uint32_t     mul_depth = Mul_depth(scc_node_id);
    SCC_NODE_PTR scc_node  = scc_cntr->Node(scc_node_id);
    // 1. skip nodes with invalid mul_depth, and revisit it later.
    if (mul_depth == INVALID_MUL_DEPTH) {
      worklist.push_back(scc_node_id);
      scc_node->Set_trav_state(TRAV_STATE_PROCESSING);
      continue;
    }

    // 2. update mul_depth of successor nodes.
    for (SCC_NODE::SCC_EDGE_ITER iter = scc_node->Begin_succ();
         iter != scc_node->End_succ(); ++iter) {
      SCC_NODE_PTR succ = (*iter)->Dst();
      // 2.1 skip nodes with no successor
      if (succ->Begin_succ() == succ->End_succ() && !Is_retv(succ)) continue;
      // 2.2 skip non-cipher nodes
      if (!Is_cipher(succ)) continue;

      uint32_t old_mul_depth = Mul_depth(succ->Id());
      uint32_t new_mul_depth = mul_depth + Consume_mul_lev(succ);
      if (old_mul_depth != INVALID_MUL_DEPTH &&
          new_mul_depth <= old_mul_depth) {
        continue;
      }
      Set_mul_depth(succ->Id(), new_mul_depth);

      if (!succ->At_trav_state(TRAV_STATE_PROCESSING)) {
        worklist.push_front(succ->Id());
      }
    }
    scc_node->Set_trav_state(TRAV_STATE_RAW);
  }
}

//! @brief Return true if scc_node contains a single CKKS::Mul node;
//! otherwise, return false.
static inline bool Is_mul_node(REGION_CONTAINER* reg_cntr,
                               SCC_NODE_PTR      scc_node) {
  if (scc_node->Elem_cnt() != 1) return false;
  REGION_ELEM_PTR elem =
      reg_cntr->Region_elem(REGION_ELEM_ID(*scc_node->Begin_elem()));
  DFG_NODE_PTR dfg_node = elem->Dfg_node();
  return (dfg_node->Is_node() && dfg_node->Node()->Opcode() == OPC_MUL);
}

//! @brief To ensure each region starts with MulCP/MulCC, maintain the
//! mul_depth of nodes consistent with their source MulCP/MulCC nodes within
//! each region.
void REGION_BUILDER::Unify_mul_depth(
    const std::list<air::opt::SCC_NODE_ID>& scc_list) {
  SCC_CONTAINER* scc_cntr = Scc_cntr();
  // 1. forward pass: set mul_depth of each successor node consistent with the
  // scc_node.
  for (SCC_NODE_LIST::const_reverse_iterator iter = scc_list.rbegin();
       iter != scc_list.rend(); ++iter) {
    SCC_NODE_PTR scc_node  = scc_cntr->Node(*iter);
    uint32_t     mul_depth = Mul_depth(scc_node->Id());
    for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
         edge_iter != scc_node->End_succ(); ++edge_iter) {
      SCC_NODE_PTR succ = edge_iter->Dst();
      // filter successors not in current region
      if (std::find(scc_list.begin(), scc_list.end(), succ->Id()) ==
          scc_list.end()) {
        continue;
      }
      uint32_t succ_mul_depth = Mul_depth(succ->Id());
      if (succ_mul_depth == INVALID_MUL_DEPTH || succ_mul_depth == mul_depth)
        continue;
      uint32_t succ_consume_lev = Consume_mul_lev(succ);
      if (succ_consume_lev > 0) {
        // in current implementation, SCC_NODE consuming mul depth must be
        // MulCC/MulCP.
        AIR_ASSERT(Is_mul_node(Region_cntr(), succ));
        continue;
      }

      AIR_ASSERT(succ_mul_depth > mul_depth);
      Set_mul_depth(succ->Id(), mul_depth);
    }
  }

  // 2. backward pass: set mul_depth of each predecessor node consistent with
  // scc_node.
  for (SCC_NODE_ID scc_node_id : scc_list) {
    SCC_NODE_PTR scc_node    = scc_cntr->Node(scc_node_id);
    uint32_t     consume_lev = Consume_mul_lev(scc_node);
    if (consume_lev > 0) {
      // in current implementation, SCC_NODE consuming mul depth must be
      // MulCC/MulCP.
      AIR_ASSERT(Is_mul_node(Region_cntr(), scc_node));
      continue;
    }

    uint32_t mul_depth = Mul_depth(scc_node->Id());
    for (SCC_NODE::SCC_NODE_ITER pred_iter = scc_node->Begin_pred();
         pred_iter != scc_node->End_pred(); ++pred_iter) {
      SCC_NODE_PTR pred = *pred_iter;
      // filter predecessors not in current region
      if (std::find(scc_list.begin(), scc_list.end(), pred->Id()) ==
          scc_list.end()) {
        continue;
      }
      uint32_t pred_mul_depth = Mul_depth(pred->Id());
      if (pred_mul_depth == INVALID_MUL_DEPTH || pred_mul_depth == mul_depth)
        continue;

      AIR_ASSERT(pred_mul_depth > mul_depth);
      Set_mul_depth(pred->Id(), mul_depth);
    }
  }
}

//! @brief Increase the mul_depth of each node to the lowest value among its
//! successors. If the successor is a multiplication node, set the mul_depth
//! to one less than that of the successor.
void REGION_BUILDER::Opt_mul_depth() {
  if (Max_mul_depth() <= 1) return;

  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  // 1. group SCC_NODEs based on mul_depth
  using SCC_NODE_LIST = std::list<air::opt::SCC_NODE_ID>;
  std::vector<SCC_NODE_LIST> scc_node_grp(Max_mul_depth() + 1);
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
       scc_iter != scc_cntr->End_node(); ++scc_iter) {
    uint32_t mul_depth = Mul_depth(scc_iter->Id());
    if (mul_depth == INVALID_MUL_DEPTH) continue;
    scc_node_grp[mul_depth].push_back(scc_iter->Id());
  }

  // 2. traverse SCC_NODE groups from greater mul_depth to lower.
  for (int32_t mul_depth = Max_mul_depth(); mul_depth > 0; --mul_depth) {
    SCC_NODE_LIST& scc_list = scc_node_grp[mul_depth];
    if (scc_list.empty()) continue;

    scc_list.sort();
    bool mul_depth_changed = false;
    for (SCC_NODE_ID scc_node_id : scc_list) {
      SCC_NODE_PTR scc_node = scc_cntr->Node(scc_node_id);
      // 2.1 compute minimal mul_depth of successor nodes
      uint32_t min_succ_mul_depth = UINT32_MAX;
      for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
           edge_iter != scc_node->End_succ(); ++edge_iter) {
        SCC_NODE_PTR succ           = edge_iter->Dst();
        uint32_t     succ_mul_depth = Mul_depth(succ->Id());
        if (succ_mul_depth == INVALID_MUL_DEPTH) continue;

        uint32_t succ_consume_lev = Consume_mul_lev(succ);
        if (succ_consume_lev > 0) {
          AIR_ASSERT(succ_mul_depth > succ_consume_lev);
          AIR_ASSERT(succ_consume_lev == 1);
          succ_mul_depth -= succ_consume_lev;
        }
        min_succ_mul_depth = std::min(min_succ_mul_depth, succ_mul_depth);
      }
      // 2.2 update mul_depth of scc_node
      if (min_succ_mul_depth > mul_depth &&
          min_succ_mul_depth < (mul_depth + BYPASS_EDGE_THRESHOLD)) {
        Set_mul_depth(scc_node->Id(), min_succ_mul_depth);
        mul_depth_changed = true;
      }
    }

    if (mul_depth_changed) {
      // To ensure each region starts with MulCP/MulCC, maintain the
      // mul_depth of nodes consistent with their source MulCP/MulCC nodes
      // within ach region.
      Unify_mul_depth(scc_list);
    }
  }

  // 3, traversal all scc node to 
  //   place input cipher(have no predecessor) at region 1
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
      scc_iter != scc_cntr->End_node(); ++scc_iter) {
    uint32_t mul_depth = Mul_depth(scc_iter->Id());
    if (mul_depth == INVALID_MUL_DEPTH) continue;
    if (scc_iter->Begin_pred() == scc_iter->End_pred()){
      Set_mul_depth(scc_iter->Id(), 1);
    }
  }
}

//! @brief Create regions. REGION_ELEMs with the same mul_depth are grouped
//! into the same region.
void REGION_BUILDER::Create_region() {
  const SCC_CONTAINER* scc_cntr = Scc_cntr();
  for (SCC_CONTAINER::NODE_ITER scc_iter = scc_cntr->Begin_node();
       scc_iter != scc_cntr->End_node(); ++scc_iter) {
    if (!Is_cipher(*scc_iter)) continue;
    uint32_t mul_depth = Mul_depth(scc_iter->Id());
    if (mul_depth == INVALID_MUL_DEPTH) continue;

    REGION_ID  region_id(mul_depth);
    REGION_PTR region = Region_cntr()->Get_region(region_id);
    for (air::opt::SCC_NODE::ELEM_ITER iter = scc_iter->Begin_elem();
         iter != scc_iter->End_elem(); ++iter) {
      REGION_ELEM_ID elem_id(*iter);
      region->Add_elem(elem_id);

      REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
      AIR_ASSERT(elem->Region_id() == air::base::Null_id);
      elem->Set_region(region_id);
    }
  }
}

void REGION_BUILDER::Verify_region() {
  if (Region_cntr()->Region_cnt() <= 1) return;
  for (uint32_t region_id = 1; region_id < Region_cntr()->Region_cnt();
       ++region_id) {
    REGION_PTR region = Region_cntr()->Region(REGION_ID(region_id));
    for (REGION::ELEM_ITER iter = region->Begin_elem();
         iter != region->End_elem(); ++iter) {
      bool is_src = true;
      for (uint32_t opnd_id = 0; opnd_id < iter->Pred_cnt(); ++opnd_id) {
        if (iter->Pred_id(opnd_id) == air::base::Null_id) continue;
        REGION_ID pred_region = iter->Pred(opnd_id)->Region_id();
        if (pred_region == air::base::Null_id) continue;
        if (pred_region.Value() == region_id) {
          is_src = false;
          break;
        }
      }
      if (is_src && region_id != 1) {
        DFG_NODE_PTR dfg_node = iter->Dfg_node();
        AIR_ASSERT_MSG(
            dfg_node->Is_node() && dfg_node->Node()->Opcode() == OPC_MUL,
            "Each region must start with MulCC/MulCP");
      } else {
        DFG_NODE_PTR dfg_node = iter->Dfg_node();
        AIR_ASSERT_MSG(
            !dfg_node->Is_node() || dfg_node->Node()->Opcode() != OPC_MUL,
            "MulCC/MulCP must be positioned at the beginning of region");
      }
    }
  }
}

R_CODE REGION_BUILDER::Perform() {
  // 1.1 build DFG of each function
  Build_dfg();
  // 1.2 build link DFG of each function and build SCC for the interprocedural
  // DFG
  R_CODE ret = Build_scc();
  if (ret != R_CODE::NORMAL) return ret;

  // 2. Compute mul_depth of each node.
  Cal_mul_depth();
  // 3. Optimize mul_depth of each node.
  Opt_mul_depth();
  // 4. Create region based on pre-computed mul_depth.
  Create_region();
  // 5. Verify each region.
  Verify_region();
  return R_CODE::NORMAL;
}

}  // namespace ckks
}  // namespace fhe
