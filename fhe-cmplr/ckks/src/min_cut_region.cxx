//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "min_cut_region.h"

#include <algorithm>
#include <cfloat>
#include <list>
#include <string>

#include "air/base/id_wrapper.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/opt/dfg_data.h"
#include "air/opt/dfg_node.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/util/debug.h"
#include "ckks_cost_model.h"
#include "dfg_region.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace ckks {

using namespace air::opt;

//! @brief Return number of polynomials contained in ciphertext type
static uint32_t Poly_num(const core::LOWER_CTX* lower_ctx,
                         air::base::TYPE_PTR    type) {
  air::base::TYPE_ID type_id =
      (type->Is_array()) ? type->Cast_to_arr()->Elem_type_id() : type->Id();
  if (lower_ctx->Is_cipher_type(type_id))
    return 2;
  else if (lower_ctx->Is_cipher3_type(type_id))
    return 3;
  AIR_ASSERT_MSG(false, "not supported type");
  return 0;
}

//! @brief Return the required count of bootstrap/rescale operations for
//! dfg_node: if dfg_node is a ciphertext, the count equals its frequency;
//! if dfg_node is an array of ciphertexts, the count is the product of its
//! frequency and the array's element count.
static uint32_t Freq(const DFG_NODE_PTR&          dfg_node,
                     const core::LOWER_CTX*       lower_ctx,
                     const air::base::GLOB_SCOPE* glob_scope) {
  uint32_t freq = dfg_node->Freq();
  if (freq == 0) return 0;
  air::base::TYPE_ID  type_id = dfg_node->Type();
  air::base::TYPE_PTR type    = glob_scope->Type(type_id);
  if (type->Is_array()) {
    air::base::ARRAY_TYPE_PTR arry_type = type->Cast_to_arr();
    freq *= arry_type->Elem_count();
    type_id = arry_type->Elem_type_id();
  }
  AIR_ASSERT_MSG(
      lower_ctx->Is_cipher3_type(type_id) || lower_ctx->Is_cipher_type(type_id),
      "not supported type");
  return freq;
}

double MIN_CUT::Cut_op_cost(void) const {
  if (Kind() == MIN_CUT_BTS) {
    return Operation_cost(OPC_BOOTSTRAP, Level());
  } else if (Kind() == MIN_CUT_RESCALE) {
    return Operation_cost(OPC_RESCALE, Level());
  }
  AIR_ASSERT_MSG(false, "not support min cut kind");
  return DBL_MAX;
}

double MIN_CUT::Node_op_cost(const DFG_NODE_PTR& dfg_node) const {
  if (!dfg_node->Is_node()) return 0.;
  air::base::OPCODE opc = dfg_node->Node()->Opcode();
  // ignore cost of non-FHE operation
  if (opc.Domain() != CKKS_DOMAIN::ID) return 0.;

  uint32_t freq = Freq(dfg_node, Lower_ctx(), Glob_scope());

  double cost_diff = 0.;
  if (Kind() == MIN_CUT_RESCALE) {
    cost_diff = Operation_cost(opc, Level()) - Operation_cost(opc, Level() - 1);
  } else if (Kind() == MIN_CUT_BTS) {
    cost_diff = Operation_cost(opc, 0) - Operation_cost(opc, Level());
  } else {
    AIR_ASSERT_MSG(false, "not supported min cut kind");
  }

  return freq * cost_diff;
}

double MIN_CUT::Scc_cut_op_cost(const SCC_NODE_PTR& scc_node) const {
  double freq = 0;
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID        elem_id(*elem_iter);
    CONST_REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);

    bool scc_internal_node = true;
    for (REGION_ELEM::EDGE_ITER edge_iter = elem->Begin_succ();
         edge_iter != elem->End_succ(); ++edge_iter) {
      if (Scc_cntr()->Scc_node(edge_iter->Dst_id()) != scc_node->Id()) {
        scc_internal_node = false;
        break;
      }
    }
    if (scc_internal_node) continue;
    freq += Freq(elem->Dfg_node(), Lower_ctx(), Glob_scope()) *
            Poly_num(Lower_ctx(), elem->Type()) / 2.;
  }
  return freq * Cut_op_cost();
}

double MIN_CUT::Scc_node_op_cost(const SCC_NODE_PTR& scc_node) const {
  double weight = 0.;
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID id(*elem_iter);
    AIR_ASSERT(id != air::base::Null_id);
    weight += Node_op_cost(Region_cntr()->Node(id)->Dfg_node());
  }
  return weight;
}

MIN_CUT::NODE_INFO MIN_CUT::Next_src_node(const CUT_TYPE& cur_cut) const {
  SCC_NODE_ID src_node(air::base::Null_id);
  double      min_cost_incr = 1.E100;
  for (REGION_ELEM_ID elem_id : cur_cut) {
    CONST_REGION_ELEM_PTR node      = Region_cntr()->Node(elem_id);
    bool                  candidate = true;
    for (REGION_ELEM::EDGE_ITER edge_iter = node->Begin_succ();
         edge_iter != node->End_succ(); ++edge_iter) {
      if (edge_iter->Dst()->Region() != Region()->Id()) {
        candidate = false;
        break;
      }
    }
    // bool res = std::find(node->Begin_succ(), node->End_succ(), [=](const
    // REGION_EDGE_PTR& edge) {
    //   return edge->Dst()->Region() != Region()->Id(); }) != node->End_succ();
    if (!candidate) continue;
    for (REGION_ELEM::EDGE_ITER edge_iter = node->Begin_succ();
         edge_iter != node->End_succ(); ++edge_iter) {
      CONST_REGION_ELEM_PTR succ = edge_iter->Dst();
      if (succ->Region() != Region()->Id()) continue;
      if (succ->Trav_state() != TRAV_STATE_RAW) {
        continue;
      }

      SCC_NODE_PTR succ_scc_node =
          Scc_cntr()->Node(Scc_cntr()->Scc_node(succ->Id()));
      bool all_pred_visited = true;
      for (SCC_NODE::SCC_NODE_ITER iter = succ_scc_node->Begin_pred();
           iter != succ_scc_node->End_pred(); ++iter) {
        if (Region_cntr()->Scc_region(iter->Id()) == air::base::Null_id)
          continue;
        if (iter->Trav_state() != TRAV_STATE_VISITED) {
          all_pred_visited = false;
          break;
        }
      }
      if (!all_pred_visited) continue;

      double cost_incr =
          Scc_cut_op_cost(succ_scc_node) - Scc_node_op_cost(succ_scc_node);
      if (cost_incr < min_cost_incr) {
        src_node      = succ_scc_node->Id();
        min_cost_incr = cost_incr;
      }
    }
  }
  return NODE_INFO(src_node, min_cost_incr);
}

MIN_CUT::NODE_INFO MIN_CUT::Next_sink_node(void) const {
  SCC_NODE_ID sink_node(air::base::Null_id);
  double      weight = -1.0E100;
  for (SCC_NODE_ID scc_id : Snk()) {
    SCC_NODE_PTR scc = Scc_cntr()->Node(scc_id);
    for (SCC_NODE::SCC_NODE_ITER node_iter = scc->Begin_pred();
         node_iter != scc->End_pred(); ++node_iter) {
      if (node_iter->Trav_state() != TRAV_STATE_RAW) {
        continue;
      }
      double n_weight = Scc_node_op_cost(*node_iter);
      if (n_weight > weight) {
        sink_node = node_iter->Id();
        weight    = n_weight;
      }
    }
  }
  return NODE_INFO(sink_node, weight);
}

double MIN_CUT::Cut_value(const CUT_TYPE& cut) {
  double freq = 0;
  for (REGION_ELEM_ID node_id : cut) {
    REGION_ELEM_PTR     elem     = Region_cntr()->Node(node_id);
    const DFG_NODE_PTR& dfg_node = elem->Dfg_node();
    freq += Freq(dfg_node, Lower_ctx(), Glob_scope()) *
            Poly_num(Lower_ctx(), elem->Type()) / 2.;
  }
  double cut_value = Cut_op_cost();
  return freq * cut_value;
}

double MIN_CUT::Init_src_node(void) {
  const REGION_PTR& region     = Region();
  double            min_weight = 0.;
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    REGION_ID region = elem_iter->Region();
    bool      is_src = true;
    for (uint32_t id = 0; id < elem_iter->Pred_cnt(); ++id) {
      if (elem_iter->Pred_id(id) == air::base::Null_id) continue;
      if (region == elem_iter->Pred(id)->Region()) {
        is_src = false;
        break;
      }
    }
    if (is_src) {
      SCC_NODE_ID scc_id = Scc_cntr()->Scc_node(elem_iter->Id());
      Src().insert(scc_id);
      Min_cut().insert(elem_iter->Id());
      elem_iter->Set_trav_state(TRAV_STATE_VISITED);
      Scc_cntr()->Node(scc_id)->Set_trav_state(TRAV_STATE_VISITED);
      DFG_NODE_PTR dfg_node = elem_iter->Dfg_node();
      // min_weight += Node_op_cost(dfg_node);
    }
  }
  min_weight += Cut_value(Min_cut());
  return min_weight;
}

void MIN_CUT::Update_cut(const SCC_NODE_PTR& scc_node, CUT_TYPE& cut) {
  // 1. add element node in SCC_NODE into cut
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID elem_id(*elem_iter);
    cut.insert(elem_id);
    REGION_ELEM_PTR elem_node = Region_cntr()->Node(elem_id);
    elem_node->Set_trav_state(TRAV_STATE_VISITED);
  }
  scc_node->Set_trav_state(TRAV_STATE_VISITED);

  // 2. remove redundant node in cut
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID  elem_id(*elem_iter);
    REGION_ELEM_PTR elem_node = Region_cntr()->Node(elem_id);
    for (uint32_t id = 0; id < elem_node->Pred_cnt(); ++id) {
      REGION_ELEM_ID pred_id = elem_node->Pred_id(id);
      if (pred_id == air::base::Null_id) continue;
      // opnd is not in
      if (cut.find(pred_id) == cut.end()) continue;

      // check if current opnd
      bool            is_cut = false;
      REGION_ELEM_PTR pred   = Region_cntr()->Node(pred_id);
      for (REGION_ELEM::EDGE_ITER edge_iter = pred->Begin_succ();
           edge_iter != pred->End_succ(); ++edge_iter) {
        REGION_ELEM_PTR dst = edge_iter->Dst();
        if (dst->Trav_state() == TRAV_STATE_RAW) {
          is_cut = true;
          break;
        }
      }
      if (!is_cut) cut.erase(pred_id);
    }
  }
}

void MIN_CUT::Min_cut_phase() {
  const uint32_t level = Level();
  // 1. collect src/snk nodes for min_cut
  for (REGION::ELEM_ITER elem_iter = Region()->Begin_elem();
       elem_iter != Region()->End_elem(); ++elem_iter) {
    elem_iter->Set_trav_state(TRAV_STATE_RAW);
  }
  double   min_weight  = Init_src_node();
  CUT_TYPE cur_cut     = Min_cut();
  bool     finish      = false;
  double   base_weight = 0.;  // cost incr from delay rescale
  while (!finish) {
    // 1. get the most tightly connected src node
    NODE_INFO next_src_info = Next_src_node(cur_cut);
    if (next_src_info.Node() == air::base::Null_id) {
      finish = true;
      break;
    }

    // 2. get the most tightly connected sink node
    NODE_INFO next_snk_info = Next_sink_node();

    // 3. add snk or src for next iteration.
    if (next_snk_info.Node() != air::base::Null_id &&
        next_snk_info.Weight() > next_src_info.Weight()) {
      Scc_cntr()->Node(next_snk_info.Node())->Set_trav_state(TRAV_STATE_RAW);
      Snk().insert(next_snk_info.Node());
      continue;
    } else {
      Src().insert(next_src_info.Node());
      base_weight += Scc_node_op_cost(Scc_cntr()->Node(next_src_info.Node()));
    }

    // 4. cal total weight of current cut
    SCC_NODE_PTR next_src = Scc_cntr()->Node(next_src_info.Node());
    Update_cut(next_src, cur_cut);

    // 5. update min_cut
    if (cur_cut.empty()) continue;
    double cut_weight = base_weight + Cut_value(cur_cut);
    if (cut_weight < min_weight) {
      min_weight = cut_weight;
      Min_cut()  = cur_cut;
    }
  }
  AIR_ASSERT(finish);
  for (REGION_ELEM_ID elem_id : Min_cut()) {
    if (Kind() == MIN_CUT_RESCALE) {
      Region_cntr()->Region_elem(elem_id)->Set_need_rescale(true);
    } else if (Kind() == MIN_CUT_BTS) {
      Region_cntr()->Region_elem(elem_id)->Set_need_bootstrap(true);
    } else {
      AIR_ASSERT_MSG(false, "not supported min cut kind");
    }
  }
}

void MIN_CUT::Perform() {
  if (Region()->Elem_cnt() == 0) return;

  Min_cut_phase();

  std::string region_name("region_");
  region_name += std::to_string(Region()->Id().Value()) + ".dot";
  Reg_cntr()->Print_dot(region_name.c_str(), Region()->Id());
}

}  // namespace ckks
}  // namespace fhe