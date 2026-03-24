//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_STRIDED_SLICE_CTX_H
#define NN_VECTOR_STRIDED_SLICE_CTX_H

#include <queue>

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

using namespace air::base;
using namespace air::driver;

using NODE_SHAPE_MAP =
    std::map<NODE_ID,
             std::vector<int64_t>>;  // strided_slice node and input shape pair

using SS_SFT_MAP =
    std::map<NODE_ID, NODE_ID>;  // strided_slice source and fusion target map

using SS_FC_PAIR =
    std::pair<NODE_ID, std::vector<NODE_ID>>;  // fusion candidate pair

class SS_FUSION_CTX : public ANALYZE_CTX {
public:
  SS_FUSION_CTX(FUNC_SCOPE* func_scope, VECTOR_CTX& ctx,
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

  //! @brief whehter this op can fused with strided_slice
  bool Is_fusion_op(NODE_PTR node) {
    if ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::GEMM)) ||
        (node->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE))) {
      return true;
    }
    return false;
  }

  //! @brief whehter this op will enlarge data size
  bool Is_enlarge_data_size_op(NODE_PTR node) {
    if ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONCAT))) {
      return true;
    }
    return false;
  }

  //! @brief whehter this is binary and enlarge data size op
  //! TODO: not considerting multiply now, need to improve later.
  bool Is_binary_op(NODE_PTR node) {
    if (((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::ADD)) &&
         (node->Child(1)->Opcode() != air::core::OPC_LDC))) {
      return true;
    }
    return false;
  }

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

  air::opt::DFG_NODE_PTR Get_dfg_node(NODE_ID node) {
    air::opt::DFG_CONTAINER* dfg_cntr = this->Dfg_cntr();
    air::opt::DFG_NODE_PTR   dfg_node = dfg_cntr->Node(dfg_cntr->Node_id(node));
    return dfg_node;
  }

  std::vector<int64_t> Get_input_shape(NODE_ID node_id) {
    NODE_PTR ss_node = this->Container()->Node(node_id);
    NODE_PTR input   = ss_node->Child(0);
    AIR_ASSERT_MSG(input->Rtype()->Is_array(), "operand is not an array type");
    ARRAY_TYPE_PTR       op_ty_arr = input->Rtype()->Cast_to_arr();
    std::vector<int64_t> op_shape  = op_ty_arr->Shape();
    return op_shape;
  }

  // TODO: need to generalize
  int Compute_concat_output_size(NODE_PTR node) {
    NODE_PTR opnd1 = node->Child(0);
    NODE_PTR opnd2 = node->Child(1);

    if ((opnd1->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) &&
        (opnd2->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE))) {
      NODE_PTR input = opnd1->Child(0);
      AIR_ASSERT_MSG(input->Rtype()->Is_array(),
                     "operand is not an array type");
      ARRAY_TYPE_PTR       ty_arr      = input->Rtype()->Cast_to_arr();
      std::vector<int64_t> input_shape = ty_arr->Shape();

      std::vector<int> channel = Get_attr_data<int>(node, core::ATTR::CHANNEL);
      int output_size = (int64_t)channel[0] * input_shape[2] * input_shape[3];
      return output_size * 2;
    } else if ((opnd1->Opcode() ==
                OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) &&
               (opnd2->Opcode() ==
                OPCODE(nn::core::NN, nn::core::OPCODE::CONV))) {
      NODE_PTR input = opnd1->Child(0);
      AIR_ASSERT_MSG(input->Rtype()->Is_array(),
                     "operand is not an array type");
      ARRAY_TYPE_PTR       ty_arr      = input->Rtype()->Cast_to_arr();
      std::vector<int64_t> input_shape = ty_arr->Shape();

      NODE_PTR weight_node = opnd1->Child(1);
      int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
              kernel_width = 0;
      Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                     kernel_height, kernel_width);
      int output_size = (int64_t)channel_out * input_shape[2] * input_shape[3];
      return output_size * 2;
    }
    return 0;
  }

  SS_FC_PAIR Find_possible_fusion_candidate(
      NODE_ID node_id, const std::vector<int64_t>& input_shape) {
    NODE_PTR ss_node = this->Container()->Node(node_id);

    SS_FC_PAIR           result_pair;
    NODE_ID              df_cand = air::base::Null_id;
    std::vector<NODE_ID> mf_cands;
    std::queue<NODE_ID>  bfs_que;
    bfs_que.push(node_id);

    while (!bfs_que.empty()) {
      NODE_ID                cur_node_id = bfs_que.front();
      air::opt::DFG_NODE_PTR dfg_node    = this->Get_dfg_node(cur_node_id);
      // get def node
      air::opt::DFG_NODE_PTR dfg_def_node = dfg_node->Succ()->Dst();

      // traverse all successor(use site) and if there is strided_slice fusion
      // candidate.
      for (auto iter = dfg_def_node->Begin_succ();
           iter != dfg_def_node->End_succ(); ++iter) {
        air::opt::DFG_EDGE_PTR edge      = *iter;
        air::opt::DFG_NODE_PTR dst_node  = edge->Dst();
        NODE_PTR               succ_node = dst_node->Node();
        if (Is_binary_op(succ_node)) {
          if (std::find(mf_cands.begin(), mf_cands.end(), succ_node->Id()) ==
              mf_cands.end()) {
            mf_cands.push_back(succ_node->Id());
          }
        }

        // check succ node:
        // will enlarge data size, such as some conv, gemm, concat: compare with
        // slot. is dest node? such as ss, gemm not change data size: add, mul,
        // relu, flatten, reshape
        if (Is_fusion_op(succ_node)) {
          df_cand = succ_node->Id();
          break;
        } else if (Is_enlarge_data_size_op(succ_node)) {
          // analyze whether it will enlarge data size > max slot
          // if > slot, stop
          // else, pushed into mf_cands vec, later will adjust rtype and
          // attribute.
          if ((succ_node->Opcode() ==
               OPCODE(nn::core::NN, nn::core::OPCODE::CONV))) {
            NODE_PTR weight_node = succ_node->Child(1);
            int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
                    kernel_width = 0;
            Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                           kernel_height, kernel_width);
            int output_size = channel_out * input_shape[2] * input_shape[3];
            if (output_size > Get_slot()) {
              break;
            }
          } else if (succ_node->Opcode() ==
                     OPCODE(nn::core::NN, nn::core::OPCODE::CONCAT)) {
            int output_size = Compute_concat_output_size(succ_node);
            if (output_size > Get_slot()) {
              break;
            }
          }
        }

        if (dst_node->Trav_state() != air::opt::TRAV_STATE_VISITED) {
          bfs_que.push(succ_node->Id());
          dst_node->Set_trav_state(air::opt::TRAV_STATE_VISITED);
        } else {
          if (std::find(mf_cands.begin(), mf_cands.end(), succ_node->Id()) ==
              mf_cands.end()) {
            mf_cands.push_back(succ_node->Id());
          }
        }
      }
      bfs_que.pop();
    }

    result_pair = std::make_pair(df_cand, mf_cands);
    return result_pair;
  }

  //! @brief get node shape map
  NODE_SHAPE_MAP Get_ns_map() {
    NODE_SHAPE_MAP ss_ns_map;
    SS_WORKLIST    ss_worklist = _ctx.Get_ss_worklist();
    for (NODE_ID ss_node_id : ss_worklist) {
      std::vector<int64_t> input_shape = Get_input_shape(ss_node_id);
      ss_ns_map.insert(
          std::pair<NODE_ID, std::vector<int64_t>>(ss_node_id, input_shape));
    }
    return ss_ns_map;
  }

  SS_SFT_MAP Find_fusion_target() {
    SS_SFT_MAP ss_sft_map;
    NODE_ID    last_ss_id   = air::base::Null_id;  // last ss node
    NODE_ID    last_df_cand = air::base::Null_id;  // definite fuse candidate
    std::vector<NODE_ID> last_mf_cand;             // may fuse candidate
    std::vector<int64_t> last_input_shape;

    NODE_SHAPE_MAP           ss_ns_map = Get_ns_map();
    NODE_SHAPE_MAP::iterator it        = ss_ns_map.begin();
    for (; it != ss_ns_map.end(); ++it) {
      std::vector<int64_t> input_shape = it->second;
      // input shape need to be adjusted when doesn't do strided_slice
      // immediately
      if (it->first == last_df_cand) {
        input_shape = last_input_shape;
      }
      SS_FC_PAIR fc_pair =
          Find_possible_fusion_candidate(it->first, input_shape);
      NODE_ID              df_cand = fc_pair.first;   // definite fuse candidate
      std::vector<NODE_ID> mf_cand = fc_pair.second;  // may fuse candidate

      // analyze and determine the fusion target
      ss_sft_map.insert(std::pair<NODE_ID, NODE_ID>(it->first, df_cand));
      if (df_cand == air::base::Null_id && (mf_cand.size() > 0)) {
        if ((mf_cand.size() == 1) &&
            std::find(last_mf_cand.begin(), last_mf_cand.end(), mf_cand[0]) !=
                last_mf_cand.end()) {
          if (ss_sft_map[last_ss_id] != air::base::Null_id) {
            ss_sft_map[it->first] = ss_sft_map[last_ss_id];
          } else {
            ss_sft_map[it->first]  = mf_cand[0];
            ss_sft_map[last_ss_id] = mf_cand[0];
          }
        }
      }

      last_df_cand = df_cand;
      last_mf_cand = mf_cand;
      last_ss_id   = it->first;
      // NODE_PTR dest_node = this->Container()->Node(dest_node_id);
      last_input_shape = input_shape;
    }
    return ss_sft_map;
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

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_STRIDED_SLICE_CTX_H
