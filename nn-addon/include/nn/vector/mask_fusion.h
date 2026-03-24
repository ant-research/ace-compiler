//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_MASK_FUSION_CTX_H
#define NN_VECTOR_MASK_FUSION_CTX_H

#include "air/core/default_handler.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/dfg_container.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "nn/core/attr.h"
#include "nn/core/default_handler.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

#define SUCC_MAX 3

using namespace air::base;
using namespace air::driver;

class MASK_FUSION_CTX : public ANALYZE_CTX {
public:
  MASK_FUSION_CTX(FUNC_SCOPE* func_scope, VECTOR_CTX& ctx,
                  const DRIVER_CTX* driver_ctx, const VECTOR_CONFIG& cfg)
      : _func_scope(func_scope),
        _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _ssa_cntr(&_func_scope->Container()),
        _dfg_cntr(&_ssa_cntr),
        _dfg(&_dfg_cntr) {}

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  void Build_ssa() {
    air::opt::SSA_BUILDER ssa_builder(_func_scope, &_ssa_cntr, _driver_ctx);
    ssa_builder.Perform();
  }

  void Build_dfg() {
    air::opt::DFG_BUILDER<> dfg_builder(&_dfg, _func_scope, &_ssa_cntr, {});
    dfg_builder.Perform();
  }

  air::opt::SSA_CONTAINER* Ssa_cntr() { return &_ssa_cntr; }

  air::opt::DFG_CONTAINER* Dfg_cntr() { return &_dfg_cntr; }

  air::base::CONTAINER* Container() { return _ssa_cntr.Container(); }

  NODE_PTR Get_def_node(NODE_PTR node) {
    AIR_ASSERT(node->Opcode() == air::core::OPC_LD ||
               node->Opcode() == air::core::OPC_LDP)
    air::opt::SSA_CONTAINER* ssa_cntr = this->Ssa_cntr();
    air::opt::SSA_VER_PTR    ssa_ver  = ssa_cntr->Node_ver(node->Id());
    AIR_ASSERT_MSG(ssa_ver->Kind() == air::opt::VER_DEF_KIND::STMT,
                   "must define by stmt");

    CONTAINER* cntr     = ssa_cntr->Container();
    NODE_PTR   def_node = cntr->Stmt(ssa_ver->Def_stmt_id())->Node();
    return def_node;
  }

  // @brief set mask attibute value to node
  void Set_mask_attr(NODE_PTR node, uint32_t mask_len) {
    std::vector<uint32_t> vdn{mask_len};
    node->Set_attr(core::ATTR::MASK, vdn.data(), vdn.size());
  }

  // @brief generate mul scalar 1 with mask=valid_len attribute as parent node
  void Gen_mult_mask_node(NODE_PTR node, uint32_t mask_len) {
    AIR_ASSERT(
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RELU)) ||
        ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MUL)) &&
         node->Child(1)->Opcode() == air::core::OPC_LDC));

    CONTAINER*  cntr   = this->Container();
    GLOB_SCOPE* gscope = cntr->Glob_scope();

    // prepare load op node as mul's first child
    air::opt::DFG_NODE_PTR dfg_node = this->Get_dfg_node(node->Id());
    // get def node
    air::opt::DFG_NODE_PTR dfg_def_node = dfg_node->Succ()->Dst();
    STMT_PTR def_stmt = cntr->Stmt(dfg_def_node->Ssa_ver()->Def_stmt_id());
    NODE_PTR def_node = def_stmt->Node();

    // prepare mask 1 node as mul's second child
    TYPE_PTR type          = gscope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
    NODE_PTR mask_val_node = cntr->New_one(type, node->Spos());

    // generate mul node
    CMPLR_ASSERT(0, "Fix rtype for New_bin_arith.");
    NODE_PTR mul_mask_node = cntr->New_bin_arith(
        air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MUL), node->Rtype(),
        node, mask_val_node, node->Spos());

    // set mask length as mul node's attribute
    Set_mask_attr(mul_mask_node, mask_len);

    def_node->Set_child(0, mul_mask_node);
  }

  air::opt::DFG_NODE_PTR Get_dfg_node(NODE_ID node) {
    air::opt::DFG_CONTAINER* dfg_cntr = this->Dfg_cntr();
    air::opt::DFG_NODE_PTR   dfg_node = dfg_cntr->Node(dfg_cntr->Node_id(node));
    return dfg_node;
  }

  // @brief when node can propagate mask generate by predecessor node, return
  // true
  bool Can_prop_mask(NODE_PTR node) {
    if ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::GEMM)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONCAT))) {
      return false;
    }
    return true;
  }

  // @brief when node is an operator which can eliminate mask directly, return
  // true
  bool Is_eliminate_mask_node(NODE_PTR node) {
    if (node->Opcode() ==
        OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
      return true;
    }
    return false;
  }

  // @brief when node is an operator that can fuse mask, return true
  bool Is_fuse_mask_node(NODE_PTR node) {
    if ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RELU)) ||
        ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MUL)) &&
         node->Child(1)->Opcode() == air::core::OPC_LDC)) {
      return true;
    }
    return false;
  }

  //! first version, when find fuse/eliminate mask candidate, don't do DFS
  //! further. this may result in a non-optimal fusion point.
  std::vector<NODE_ID> Find_fusion_candidate(NODE_ID node_id) {
    NODE_PTR               debug_node = this->Container()->Node(node_id);
    air::opt::DFG_NODE_PTR dfg_node   = this->Get_dfg_node(node_id);
    // get def node
    air::opt::DFG_NODE_PTR dfg_def_node = dfg_node->Succ()->Dst();

    std::vector<NODE_ID> candidate;
    // traverse all successor(use site) and analyze whether there is mask fusion
    // opportunity, if yes, add to candidate.
    uint32_t succ_count = 0;
    for (auto iter = dfg_def_node->Begin_succ();
         iter != dfg_def_node->End_succ(); ++iter) {
      succ_count++;
      air::opt::DFG_EDGE_PTR edge      = *iter;
      air::opt::DFG_NODE_PTR dst_node  = edge->Dst();
      NODE_PTR               succ_node = dst_node->Node();

      // indicate this is the last node
      if (succ_node->Opcode() == air::core::RET ||
          succ_node->Opcode() == air::core::RETV) {
        AIR_ASSERT(succ_count == 1);
        candidate.push_back(succ_node->Id());
        break;
      }

      if (!Can_prop_mask(succ_node)) {
        // if any successor node fails to propagate the mask, stop immediately.
        break;
      } else if (Is_eliminate_mask_node(succ_node) ||
                 Is_fuse_mask_node(succ_node)) {
        candidate.push_back(succ_node->Id());
      } else {
        // prop to next succ node
        std::vector<NODE_ID> sub_candidate =
            Find_fusion_candidate(succ_node->Id());
        if (sub_candidate.size() > 0) {
          candidate.insert(std::end(candidate), std::begin(sub_candidate),
                           std::end(sub_candidate));
        }
      }
    }

    // successor node should not exceed SUCC_MAX till now
    AIR_ASSERT(succ_count < SUCC_MAX);

    // profit analysis
    // when there are multiple successors, mask fused/eliminated in one
    // successor branch but not all successor branch, there is no profit. when
    // the number of candidate is less than the number of successor, there is no
    // profit, so clear the candidate vector.
    if (candidate.size() < succ_count) {
      candidate.clear();
    }

    return candidate;
  }

  void Process_mask(const std::vector<NODE_ID>& candidates,
                    NODE_MASK_PAIR              mask_node) {
    // no candidate means set mask attr to op who generates it.
    if (candidates.size() == 0) {
      NODE_PTR orig_node = Container()->Node(mask_node.first);
      Set_mask_attr(orig_node, mask_node.second);
    }
    for (NODE_ID node_id : candidates) {
      NODE_PTR node = Container()->Node(node_id);
      if (Is_fuse_mask_node(node)) {
        Gen_mult_mask_node(node, mask_node.second);
      }
    }
  }

  void Mask_fusion() {
    MF_WORKLIST mf_worklist = _ctx.Get_mf_worklist();
    for (const NODE_MASK_PAIR mask_node : mf_worklist) {
      std::vector<NODE_ID> candidates = Find_fusion_candidate(mask_node.first);
      // analyze and process candidates.
      Process_mask(candidates, mask_node);
    }
  }

  int Get_slot() const {
    // slot option value has high priority
    // TODO: need to align slot type later.
    if (_config.Max_slots() != 0) {
      return _config.Max_slots();
    } else {
      return _ctx.Slot();
    }
  }

private:
  FUNC_SCOPE*                  _func_scope;
  VECTOR_CTX&                  _ctx;
  const DRIVER_CTX*            _driver_ctx;
  const VECTOR_CONFIG&         _config;
  air::opt::SSA_CONTAINER      _ssa_cntr;
  air::opt::DFG_CONTAINER      _dfg_cntr;
  air::opt::DFG_CONTAINER::DFG _dfg;
};

//! @brief mask fusion handler
class MASK_FUSION_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    MASK_FUSION_CTX& ctx = visitor->Context();

    NODE_PTR input = node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG(batch == 1, "Conv only supports batch=1");

    NODE_PTR weight_node = node->Child(1);
    int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
            kernel_width = 0;
    Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                   kernel_height, kernel_width);
    AIR_ASSERT_MSG(kernel_height == kernel_width,
                   "Conv only supports kernel_height = kernel_width");

    int64_t output_size = channel_out * input_height * input_width;

    // generate worklist which contains all op that generate mask
    if ((channel_out >= channel_in) && (ctx.Get_slot() != output_size)) {
      ctx.Register_node_mask_len(node->Id(), (uint32_t)output_size);
    }

    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_gemm(VISITOR* visitor, air::base::NODE_PTR node) {
    MASK_FUSION_CTX& ctx = visitor->Context();

    AIR_ASSERT(node->Child(1)->Rtype()->Is_array());
    ARRAY_TYPE_PTR       ty_arr   = node->Child(1)->Rtype()->Cast_to_arr();
    std::vector<int64_t> ty_shape = Get_array_dim_ubs(ty_arr);
    AIR_ASSERT_MSG(ty_shape.size() == 2, "operand1_const dim %d != 2",
                   ty_shape.size());

    int64_t height = ty_shape[0];

    // generate worklist which contains all op that generate mask
    if (ctx.Get_slot() != height) {
      ctx.Register_node_mask_len(node->Id(), (uint32_t)height);
    }

    return RETV();
  }

};  // MASK_FUSION_CORE_HANDLER

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_MASK_FUSION_CTX_H
