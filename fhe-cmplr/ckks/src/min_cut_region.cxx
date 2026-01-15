//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "min_cut_region.h"

#include <cfloat>

#include "air/base/id_wrapper.h"
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

void CUT_TYPE::Print(std::ostream& os, uint32_t indent) {
  std::string indent_str(indent, ' ');
  os << indent_str << "Cut with " << Cut_elem().size() << " elements, "
     << "total cost increase= " << Cost_incr() << " ms." << std::endl;
  os << indent_str << "REGION_ELEM: {" << std::endl;
  for (REGION_ELEM_ID id : Cut_elem()) {
    if (id == air::base::Null_id) continue;
    REGION_ELEM_PTR elem = Region_cntr()->Region_elem(id);
    elem->Print(os, indent + 4);
  }
  os << indent_str << "}" << std::endl;
  os << indent_str << "REGION_EDGE: {" << std::endl;
  for (REGION_ELEM_ID id : Cut_elem()) {
    if (id == air::base::Null_id) continue;
    REGION_ELEM_PTR elem = Region_cntr()->Region_elem(id);
    elem->Print_dot(os, indent + 4);
  }
  os << indent_str << "}" << std::endl;
}

double MIN_CUT_REGION::Cut_op_cost(void) const {
  uint32_t poly_deg = Lower_ctx()->Get_ctx_param().Get_poly_degree();
  if (Kind() == MIN_CUT_BTS) {
    return Operation_cost(OPC_BOOTSTRAP, Level(), poly_deg);
  } else if (Kind() == MIN_CUT_RESCALE) {
    return Operation_cost(OPC_RESCALE, Level(), poly_deg);
  }
  AIR_ASSERT_MSG(false, "not support min cut kind");
  return DBL_MAX;
}

double MIN_CUT_REGION::Node_op_cost(const DFG_NODE_PTR& dfg_node) const {
  if (!dfg_node->Is_node()) return 0.;
  air::base::OPCODE opc = dfg_node->Node()->Opcode();
  // ignore cost of non-FHE operation
  if (opc.Domain() != CKKS_DOMAIN::ID) return 0.;

  uint32_t freq = Freq(dfg_node, Lower_ctx(), Glob_scope());

  uint32_t poly_deg  = Lower_ctx()->Get_ctx_param().Get_poly_degree();
  double   cost_diff = 0.;
  if (Kind() == MIN_CUT_RESCALE) {
    cost_diff = Operation_cost(opc, Level(), poly_deg) -
                Operation_cost(opc, Level() - 1, poly_deg);
  } else if (Kind() == MIN_CUT_BTS) {
    cost_diff = Operation_cost(opc, 0, poly_deg) -
                Operation_cost(opc, Level(), poly_deg);
  } else {
    AIR_ASSERT_MSG(false, "not supported min cut kind");
  }

  return freq * cost_diff;
}

double MIN_CUT_REGION::Scc_cut_op_cost(const SCC_NODE_PTR& scc_node) const {
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

double MIN_CUT_REGION::Scc_node_op_cost(const SCC_NODE_PTR& scc_node) const {
  double cost = 0.;
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID id(*elem_iter);
    AIR_ASSERT(id != air::base::Null_id);
    cost += Node_op_cost(Region_cntr()->Node(id)->Dfg_node());
  }
  return cost;
}

void MIN_CUT_REGION::Set_weight(const SCC_NODE_PTR& scc_node, double val) {
  std::list<REGION_ELEM::EDGE_ITER> outter_edge;
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID id(*elem_iter);
    AIR_ASSERT(id != air::base::Null_id);
    REGION_ELEM_PTR elem = Region_cntr()->Node(id);
    for (REGION_ELEM::EDGE_ITER edge_iter = elem->Begin_succ();
         edge_iter != elem->End_succ(); ++edge_iter) {
      // handle internal edge
      if (Scc_cntr()->Scc_node(edge_iter->Dst_id()) == scc_node->Id()) {
        edge_iter->Set_weight(val);
      } else {
        outter_edge.push_back(edge_iter);
      }
    }
  }
  if (outter_edge.empty()) return;
  double avg_weight = val / outter_edge.size();
  for (REGION_ELEM::EDGE_ITER edge_iter : outter_edge) {
    edge_iter->Set_weight(avg_weight);
  }
}

SCC_NODE_ID MIN_CUT_REGION::Next_src_node(const CUT_TYPE& cur_cut) {
  SCC_NODE_ID src_node(air::base::Null_id);
  double      max_cost_incr = -1.E100;
  for (REGION_ELEM_ID elem_id : cur_cut.Cut_elem()) {
    CONST_REGION_ELEM_PTR node = Region_cntr()->Node(elem_id);
    SCC_NODE_PTR scc_node  = Scc_cntr()->Node(Scc_cntr()->Scc_node(elem_id));
    bool         candidate = true;

    // skip elements link to other regions
    for (REGION_ELEM::EDGE_ITER edge_iter = node->Begin_succ();
         edge_iter != node->End_succ(); ++edge_iter) {
      REGION_ID dst_region = edge_iter->Dst()->Region_id();
      if (dst_region != air::base::Null_id && dst_region != Region()->Id()) {
        candidate = false;
        break;
      }
    }
    if (!candidate) continue;

    for (REGION_ELEM::EDGE_ITER edge_iter = node->Begin_succ();
         edge_iter != node->End_succ(); ++edge_iter) {
      CONST_REGION_ELEM_PTR succ = edge_iter->Dst();
      if (succ->Region_id() != Region()->Id()) continue;
      if (succ->Trav_state() != TRAV_STATE_RAW) {
        continue;
      }

      SCC_NODE_PTR succ_scc_node =
          Scc_cntr()->Node(Scc_cntr()->Scc_node(succ->Id()));
      bool all_pred_visited = true;
      for (SCC_NODE::SCC_NODE_ITER iter = succ_scc_node->Begin_pred();
           iter != succ_scc_node->End_pred(); ++iter) {
        REGION_ID pred_region = Region_cntr()->Scc_region(iter->Id());
        if (pred_region == air::base::Null_id) continue;
        // For min_cut_rescale, the cut must be positioned before any bypass
        // edge connecting to the current region.
        if (Kind() == MIN_CUT_RESCALE && pred_region != Region_id()) {
          all_pred_visited = false;
          break;
        }
        if (iter->Trav_state() != TRAV_STATE_VISITED) {
          all_pred_visited = false;
          break;
        }
      }
      if (!all_pred_visited) continue;

      double cost_incr =
          Scc_cut_op_cost(succ_scc_node) + Scc_node_op_cost(succ_scc_node);
      Set_weight(succ_scc_node, cost_incr);
      cost_incr += Scc_cut_op_cost(scc_node);
      if (cost_incr > max_cost_incr) {
        src_node      = succ_scc_node->Id();
        max_cost_incr = cost_incr;
        Trace(TD_RESBM_MIN_CUT, "    Next candidate src node ",
              succ_scc_node->To_str(), " with cost_incr= ", cost_incr, "\n");
      }
    }
  }
  return src_node;
}

double MIN_CUT_REGION::Cut_cost(const CUT_TYPE& cut) {
  double freq = 0;
  for (REGION_ELEM_ID node_id : cut.Cut_elem()) {
    REGION_ELEM_PTR     elem     = Region_cntr()->Node(node_id);
    const DFG_NODE_PTR& dfg_node = elem->Dfg_node();
    freq += Freq(dfg_node, Lower_ctx(), Glob_scope()) *
            Poly_num(Lower_ctx(), elem->Type()) / 2.;
  }
  double cut_value = Cut_op_cost();
  return freq * cut_value;
}

double MIN_CUT_REGION::Init_src_node(void) {
  const REGION_PTR& region = Region();
  double            cost   = 0.;
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    REGION_ID region = elem_iter->Region_id();
    bool      is_src = true;
    for (uint32_t id = 0; id < elem_iter->Pred_cnt(); ++id) {
      if (elem_iter->Pred_id(id) == air::base::Null_id) continue;
      if (region == elem_iter->Pred(id)->Region_id()) {
        is_src = false;
        break;
      }
    }
    if (is_src) {
      SCC_NODE_ID  scc_id      = Scc_cntr()->Scc_node(elem_iter->Id());
      SCC_NODE_PTR scc_node    = Scc_cntr()->Node(scc_id);
      double       cut_op_cost = Scc_cut_op_cost(scc_node);
      cost += cut_op_cost;
      Set_weight(scc_node, cut_op_cost);

      Src().insert(scc_id);
      Min_cut().Add_elem(elem_iter->Id());
      elem_iter->Set_trav_state(TRAV_STATE_VISITED);
      scc_node->Set_trav_state(TRAV_STATE_VISITED);
      DFG_NODE_PTR dfg_node = elem_iter->Dfg_node();
    }
  }

  Min_cut().Set_cost_incr(cost);
  // trace initial source node cut
  Trace(TD_RESBM_MIN_CUT, "    Initial cut:\n");
  Trace_obj(TD_RESBM_MIN_CUT, &Min_cut());
  return cost;
}

void MIN_CUT_REGION::Update_cut(const SCC_NODE_PTR& scc_node, CUT_TYPE& cut) {
  // 1. add element node in SCC_NODE into cut
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID elem_id(*elem_iter);
    cut.Add_elem(elem_id);
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
      // opnd is not in current cut
      if (cut.Find(pred_id) == cut.End()) continue;

      // check if current opnd
      bool            is_cut = false;
      REGION_ELEM_PTR pred   = Region_cntr()->Node(pred_id);
      for (REGION_ELEM::EDGE_ITER edge_iter = pred->Begin_succ();
           edge_iter != pred->End_succ(); ++edge_iter) {
        REGION_ELEM_PTR dst = edge_iter->Dst();
        if (dst->Trav_state() == TRAV_STATE_RAW ||
            (dst->Region_id() != air::base::Null_id &&
             dst->Region_id() != pred->Region_id())) {
          is_cut = true;
          break;
        }
      }
      if (!is_cut) cut.Erase_elem(pred_id);
    }
  }
}

void MIN_CUT_REGION::Min_cut_phase() {
  const uint32_t level = Level();
  for (REGION::ELEM_ITER elem_iter = Region()->Begin_elem();
       elem_iter != Region()->End_elem(); ++elem_iter) {
    elem_iter->Set_trav_state(TRAV_STATE_RAW);
    elem_iter->Clear_attr();
    SCC_NODE_ID scc_id = Scc_cntr()->Scc_node(elem_iter->Id());
    Scc_cntr()->Node(scc_id)->Set_trav_state(TRAV_STATE_RAW);
  }
  // 1. collect src nodes for min_cut
  double   min_cost_incr = Init_src_node();
  CUT_TYPE cur_cut       = Min_cut();
  double   op_cost_incr  = 0.;  // cost incr from FHE operations
  while (true) {
    // 2. get the most tightly connected src node
    SCC_NODE_ID next_src_node = Next_src_node(cur_cut);
    if (next_src_node == air::base::Null_id) {
      break;
    }

    // 3. update current cut
    Src().insert(next_src_node);
    SCC_NODE_PTR next_src = Scc_cntr()->Node(next_src_node);
    // trace next source node
    Trace(TD_RESBM_MIN_CUT, "    Next src node: ", next_src->To_str(), "\n");

    op_cost_incr += Scc_node_op_cost(next_src);
    Update_cut(next_src, cur_cut);
    if (cur_cut.Empty()) continue;
    // cost increase of rescale/bootstrap and FHE operations.
    double tot_cost_incr = op_cost_incr + Cut_cost(cur_cut);
    cur_cut.Set_cost_incr(tot_cost_incr);
    // trace current cut
    Trace(TD_RESBM_MIN_CUT, "    Current cut:\n");
    Trace_obj(TD_RESBM_MIN_CUT, &cur_cut);

    // 4. update min_cut
    if (tot_cost_incr < min_cost_incr) {
      min_cost_incr = tot_cost_incr;
      Min_cut()     = cur_cut;
      // trace current min cost incr cut
      Trace(TD_RESBM_MIN_CUT, "    Update cut with lower cost increase:\n");
      Trace_obj(TD_RESBM_MIN_CUT, &Min_cut());
    }
  }
  // trace min_cut result
  Trace(TD_RESBM_MIN_CUT, "    Min_cut res:\n");
  Trace_obj(TD_RESBM_MIN_CUT, &Min_cut());

  for (REGION_ELEM_ID elem_id : Min_cut().Cut_elem()) {
    if (Kind() == MIN_CUT_RESCALE) {
      Region_cntr()->Region_elem(elem_id)->Set_need_rescale(true);
    } else if (Kind() == MIN_CUT_BTS) {
      Region_cntr()->Region_elem(elem_id)->Set_need_bootstrap(true);
    } else {
      AIR_ASSERT_MSG(false, "not supported min cut kind");
    }
  }
}

void MIN_CUT_REGION::Perform() {
  AIR_ASSERT_MSG(Region_id().Value() > 0, "Invalid region index");
  AIR_ASSERT_MSG(Region_id().Value() < Region_cntr()->Region_cnt(),
                 "Region index out of range.");
  Trace(TD_RESBM_MIN_CUT, "START Min_cut region_", Region_id().Value(),
        " at level= ", Level(), " for ",
        (Kind() == MIN_CUT_BTS ? "bootstrap" : "rescale"), "\n");

  Min_cut_phase();

  // Print region in a dot file in case of tracing min_cut
  if (Is_trace(TD_RESBM_MIN_CUT)) {
    std::string file_name =
        "Region_" + std::to_string(Region_id().Value()) + ".dot";
    Region_cntr()->Print_dot(file_name.c_str(), Region_id());
  }
  Trace(TD_RESBM_MIN_CUT, "END Min_cut region_", Region_id().Value(), "\n");
}

}  // namespace ckks
}  // namespace fhe
