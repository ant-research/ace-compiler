//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_T2TSHARDING_ANALYSIS_HANDLER_H
#define NN_T2TSHARDING_ANALYSIS_HANDLER_H

#include "air/base/transform_util.h"
#include "air/core/default_handler.h"
#include "nn/core/attr.h"
#include "nn/core/default_handler.h"
#include "nn/core/null_handler.h"
#include "nn/vector/sharding.h"
#include "nn/vector/t2tsharding_analysis_ctx.h"
#include "nn/vector/tensor2vector_util.h"

namespace nn {
namespace vector {

// Start from input to drive the initial sharding analysis for each op.
// Sharding propagation across the model.
class T2TSHARDING_ANALYSIS_HANDLER : public nn::core::NULL_HANDLER {
public:
  T2TSHARDING_ANALYSIS_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_elem_wise_op<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_concat(VISITOR* visitor, air::base::NODE_PTR node) {
    // validate types
    AIR_ASSERT(node->Rtype()->Is_array());
    AIR_ASSERT(
        node->Child(0)->Rtype()->Is_compatible_type(node->Child(1)->Rtype()));

    // get shape and count from Rtype
    T2TSHARDING_ANALYSIS_CTX& ctx   = visitor->Context();
    int64_t                   slots = ctx.Max_slots();
    int64_t in0_count = node->Child(0)->Rtype()->Cast_to_arr()->Elem_count();
    int64_t in1_count = node->Child(1)->Rtype()->Cast_to_arr()->Elem_count();
    int64_t out_count = node->Rtype()->Cast_to_arr()->Elem_count();

    std::vector<int64_t> in0_shape =
        node->Child(0)->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> in1_shape =
        node->Child(1)->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> out_shape = node->Rtype()->Cast_to_arr()->Shape();
    AIR_ASSERT(in0_shape[0] == out_shape[0] && in1_shape[0] == out_shape[0]);
    AIR_ASSERT(in0_shape[1] + in1_shape[1] == out_shape[1]);
    AIR_ASSERT(in0_shape[2] == out_shape[2] && in1_shape[2] == out_shape[2]);
    AIR_ASSERT(in0_shape[3] == out_shape[3] && in1_shape[3] == out_shape[3]);

    ctx.Trace(TF_SHARDING, ">> Sharding analysis: ", node->Name(),
              " slot=", slots, " in0_count=", in0_count,
              " in1_count=", in1_count, " out_count=", out_count, "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, node->Child(0)->Rtype(),
                  "Input 0");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, node->Child(1)->Rtype(),
                  "Input 1");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, node->Rtype(), "Output");

    SHARDING_MAP* shmap = ctx.Sharding_map();
    if (out_count < slots) {
      ctx.Trace(TF_SHARDING, "No sharding is needed!\n");

      ARRAY_SHARDING output_shard;
      output_shard.Set_spec({1, 1, 1, 1});

      FUNC_ID  scope  = node->Container()->Parent_func_scope()->Id();
      NODE_PTR output = ctx.Parent(1);
      AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
                 output->Opcode() == air::core::OPC_STP);
      (ctx.Parent(1)->Opcode() == air::core::OPC_ST)
          ? shmap->Set_data_sharding(scope, output->Addr_datum_id(),
                                     output_shard)
          : shmap->Set_data_sharding(scope, output->Preg_id(), output_shard);
      return RETV();
    }

    std::vector<int64_t> in0_pspec = shmap->Analyze_conv(
        slots, in0_shape[1], in0_shape[1], in0_shape[2], in0_shape[3]);
    ARRAY_SHARDING in0_shard(DIM3{in0_pspec[0], in0_pspec[1], in0_pspec[2]});
    in0_shard.Set_spec({1, in0_pspec[1], in0_pspec[2], 1});

    std::vector<int64_t> in1_pspec = shmap->Analyze_conv(
        slots, in1_shape[1], in1_shape[1], in1_shape[2], in1_shape[3]);
    ARRAY_SHARDING in1_shard(DIM3{in1_pspec[0], in1_pspec[1], in1_pspec[2]});
    in1_shard.Set_spec({1, in1_pspec[1], in1_pspec[2], 1});

    std::vector<int64_t> out_pspec = shmap->Analyze_conv(
        slots, out_shape[1], out_shape[1], out_shape[2], out_shape[3]);
    ARRAY_SHARDING out_shard(DIM3{out_pspec[0], out_pspec[1], out_pspec[2]});
    out_shard.Set_spec({1, out_pspec[1], out_pspec[2], 1});

    OP_SHARDING shard(DIM3{out_pspec[0], out_pspec[1], out_pspec[2]});
    shard.Set_imap(0, in0_shard);
    shard.Set_imap(1, in1_shard);
    shard.Set_omap(0, out_shard);
    shmap->Set_op_sharding(node, shard);
    std::vector<int64_t> shard_info{out_pspec[0], out_pspec[1], out_pspec[2],
                                    0};
    node->Set_attr(core::ATTR::SHARDING, shard_info.data(), shard_info.size());
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());

    // check and validate expected and actual sharding on input
    FUNC_ID scope = node->Container()->Parent_func_scope()->Id();
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      AIR_ASSERT(node->Child(i)->Opcode() == air::core::OPC_LD ||
                 node->Child(i)->Opcode() == air::core::OPC_LDP);
      const ARRAY_SHARDING* actual =
          (node->Child(i)->Opcode() == air::core::OPC_LD)
              ? shmap->Get_data_sharding(scope, node->Child(i)->Addr_datum_id())
              : shmap->Get_data_sharding(scope, node->Child(i)->Preg_id());
      AIR_ASSERT(actual != nullptr);
      ctx.Trace(TF_SHARDING, "Input ", i,
                " expected: ", i == 0 ? in0_shard : in1_shard);
      ctx.Trace(TF_SHARDING, "Input ", i, " actual: ", *actual);
    }

    NODE_PTR output = ctx.Parent(1);
    AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
               output->Opcode() == air::core::OPC_STP);
    (ctx.Parent(1)->Opcode() == air::core::OPC_ST)
        ? shmap->Set_data_sharding(scope, output->Addr_datum_id(), out_shard)
        : shmap->Set_data_sharding(scope, output->Preg_id(), out_shard);

    return RETV(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR child = node->Child(0);
    AIR_ASSERT(node->Rtype()->Is_array());
    AIR_ASSERT(child->Rtype()->Is_array());
    AIR_ASSERT((child->Opcode() != air::core::OPC_LD) ||
               (!child->Addr_datum()->Is_formal()));

    T2TSHARDING_ANALYSIS_CTX& ctx   = visitor->Context();
    int64_t                   slots = ctx.Max_slots();
    int64_t in_count  = child->Rtype()->Cast_to_arr()->Elem_count();
    int64_t out_count = node->Rtype()->Cast_to_arr()->Elem_count();

    std::vector<int64_t> in_shape  = child->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> out_shape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int>     strides = Get_attr_data<int>(node, core::ATTR::STRIDE);
    std::vector<int>     pads    = Get_attr_data<int>(node, core::ATTR::PAD);
    std::vector<int>     kshape  = Get_attr_data<int>(node, core::ATTR::KSHAPE);

    AIR_ASSERT(strides[0] == strides[1]);
    AIR_ASSERT(pads[0] == pads[1] && pads[0] == pads[2] && pads[0] == pads[3]);
    AIR_ASSERT(in_shape[0] == out_shape[0] && in_shape[1] == out_shape[1]);
    AIR_ASSERT(out_shape[2] =
                   (in_shape[2] + 2 * pads[0] - kshape[0]) / strides[0] + 1);
    AIR_ASSERT(out_shape[3] =
                   (in_shape[3] + 2 * pads[1] - kshape[1]) / strides[1] + 1);

    ctx.Trace(TF_SHARDING, ">> Sharding analysis: ", node->Name(),
              " slot=", slots, " in_count=", in_count, " out_count=", out_count,
              "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, child->Rtype(), "Input ");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, node->Rtype(), "Output");

    SHARDING_MAP* shmap = ctx.Sharding_map();
    if (in_count < slots) {
      ctx.Trace(TF_SHARDING, "No sharding is needed!\n");

      ARRAY_SHARDING output_shard;
      output_shard.Set_spec({1, 1, 1, 1});

      FUNC_ID  scope  = node->Container()->Parent_func_scope()->Id();
      NODE_PTR output = ctx.Parent(1);
      AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
                 output->Opcode() == air::core::OPC_STP);
      (output->Opcode() == air::core::OPC_ST)
          ? shmap->Set_data_sharding(scope, output->Addr_datum_id(),
                                     output_shard)
          : shmap->Set_data_sharding(scope, output->Preg_id(), output_shard);

      return RETV();
    }

    // Get op sharding strategy: N-dimension partition in C/H
    // input  [C/y, H/z, W]
    // output [C/y, H/z, W]
    // use same sharding strategy as Conv2D (channel_in == channel_out, x == y)
    std::vector<int64_t> pspec = shmap->Analyze_conv(
        slots, out_shape[1], in_shape[1], in_shape[2], in_shape[3]);

    DIM3           mesh = {pspec[0], pspec[1], pspec[2]};
    OP_SHARDING    shard(mesh);
    ARRAY_SHARDING input_shard(mesh);
    input_shard.Set_spec({1, pspec[1], pspec[2], 1});
    // use same halo strategy as Conv2D (kernel_height / 2)
    Compute_conv_halo(input_shard, kshape[0], kshape[1]);
    AIR_ASSERT((in_shape[2] / pspec[2] + input_shard.Halosize()) *
                   in_shape[3] <=
               slots);

    ARRAY_SHARDING output_shard(mesh);
    int            stride = strides[0];
    if (stride > 1) {
      AIR_ASSERT_MSG((stride * stride) % pspec[2] == 0,
                     "TODO: multi-level intra-channel average pool");
      AIR_ASSERT_MSG(pspec[2] == 1 || pspec[2] == stride,
                     "TODO: not 2 to 1 intra-channel average pool");
      output_shard.Set_reshard(1);
      int64_t nz = pspec[2] / stride;
      int64_t ny = (pspec[1] * pspec[2]) / (nz ? stride : stride * stride);
      output_shard.Set_spec({1, ny, nz, 1});
    } else {
      output_shard.Set_spec({1, pspec[1], pspec[2], 1});
    }

    shard.Set_imap(0, input_shard);
    shard.Set_omap(0, output_shard);

    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    shmap->Set_op_sharding(node, shard);
    std::vector<int64_t> shard_info{pspec[0], pspec[1], pspec[2],
                                    input_shard.Halosize()};
    node->Set_attr(core::ATTR::SHARDING, shard_info.data(), shard_info.size());

    FUNC_ID scope = node->Container()->Parent_func_scope()->Id();
    AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_LD ||
               node->Child(0)->Opcode() == air::core::OPC_LDP);
    const ARRAY_SHARDING* actual =
        (node->Child(0)->Opcode() == air::core::OPC_LD)
            ? shmap->Get_data_sharding(scope, node->Child(0)->Addr_datum_id())
            : shmap->Get_data_sharding(scope, node->Child(0)->Preg_id());
    AIR_ASSERT(actual != nullptr);

    ctx.Trace(TF_SHARDING, "Input expected: ", input_shard);
    ctx.Trace(TF_SHARDING, "Input actual: ", *actual);

    NODE_PTR output = ctx.Parent(1);
    AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
               output->Opcode() == air::core::OPC_STP);
    (output->Opcode() == air::core::OPC_ST)
        ? shmap->Set_data_sharding(scope, output->Addr_datum_id(), output_shard)
        : shmap->Set_data_sharding(scope, output->Preg_id(), output_shard);

    return RETV(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_average_pool<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_ANALYSIS_CTX& ctx  = visitor->Context();
    CONTAINER*                cntr = ctx.Container();

    std::vector<int> pads   = Get_attr_data<int>(node, core::ATTR::PAD);
    std::vector<int> groups = Get_attr_data<int>(node, core::ATTR::GROUP);
    int              group  = groups[0];

    // Verification & Get shapes
    int64_t batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    int64_t channel_out = 0, kernel_height = 0, kernel_width = 0;
    Verify_conv(node, batch, channel_in, input_height, input_width, channel_out,
                kernel_height, kernel_width, group);

    AIR_ASSERT_MSG(pads[0] > 0 || (kernel_height == 1 && kernel_width == 1),
                   "only support SAME padding now");

    visitor->template Visit<RETV>(node->Child(0));

    NODE_PTR input  = node->Child(0);
    NODE_PTR weight = node->Child(1);

    // Local sharding decision
    int64_t max_slots    = ctx.Max_slots();
    int64_t input_numel  = input->Rtype()->Cast_to_arr()->Elem_count();
    int64_t output_numel = node->Rtype()->Cast_to_arr()->Elem_count();

    ctx.Trace(TF_SHARDING, ">> Sharding analysis: ", node->Name(),
              " slots=", max_slots, "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, input->Rtype(), "Input ");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, weight->Rtype(), "Weight");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, node->Rtype(), "Output");

    // Note: if (stride=2 and 2*output_numel>max_slots): still need sharing
    // because we cannot handle stride inplace?
    if ((input_numel <= max_slots) && (output_numel <= max_slots)) {
      ctx.Trace(TF_SHARDING, "No sharding is needed!\n");
      return RETV();
    }

    // Get op sharding strategy: N-dimension partition in F/C/H
    // input  [C/y, H/z, W]
    // weight [F/x, C/y, K, K]
    // output [F/x, H/z, W]
    SHARDING_MAP*        shmap = ctx.Sharding_map();
    std::vector<int64_t> pspec = shmap->Analyze_conv(
        max_slots, channel_out, channel_in, input_height, input_width);

    // Construct mesh for conv
    DIM3        conv_mesh = {pspec[0], pspec[1], pspec[2]};
    OP_SHARDING conv_shard(conv_mesh);

    ARRAY_SHARDING input_shard(conv_mesh);
    input_shard.Set_spec({1, pspec[1], pspec[2], 1});
    Compute_conv_halo(input_shard, kernel_height, kernel_width);
    AIR_ASSERT_MSG(
        (input_height / pspec[2] + input_shard.Halosize()) * input_width <=
            max_slots,
        "conv with halo <= max_slots");

    std::vector<int> strides = Get_attr_data<int>(node, nn::core::ATTR::STRIDE);
    AIR_ASSERT(strides[0] == strides[1]);
    int stride = strides[0];
    AIR_ASSERT_MSG(!((input_height / pspec[2] % 2 != 0) && (stride == 2)),
                   "Never handle odd input_block_height and stride=2");

    ARRAY_SHARDING weight_shard(conv_mesh);
    weight_shard.Set_spec({pspec[0], pspec[1], 1, 1});

    ARRAY_SHARDING output_shard(conv_mesh);
    if (stride > 1) {
      output_shard.Set_reshard(1);
      if (pspec[2] >= (stride * stride)) {
        // still have intra-channel sharding
        AIR_ASSERT_MSG(false,
                       "TODO: output halo size may incorrect for this case");
        AIR_ASSERT(pspec[2] % (stride * stride) == 0);
        output_shard.Set_spec({1, pspec[0], pspec[2] / (stride * stride), 1});
      } else if ((pspec[0] * pspec[2]) >= (stride * stride)) {
        // no intra-channel shar
        AIR_ASSERT((pspec[0] * pspec[2]) % (stride * stride) == 0);
        output_shard.Set_spec(
            {1, (pspec[0] * pspec[2]) / (stride * stride), 1, 1});
      } else {
        output_shard.Set_spec({1, 1, 1, 1});
      }
    } else {
      output_shard.Set_spec({1, pspec[0], pspec[2], 1});
    }

    conv_shard.Set_imap(0, input_shard);
    conv_shard.Set_imap(1, weight_shard);
    conv_shard.Set_omap(0, output_shard);

    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, conv_shard, node->Name());
    shmap->Set_op_sharding(node, conv_shard);
    std::vector<int64_t> shard_info{pspec[0], pspec[1], pspec[2],
                                    input_shard.Halosize()};
    node->Set_attr(core::ATTR::SHARDING, shard_info.data(), shard_info.size());

    // Match sharding info with predecessor
    AIR_ASSERT(input->Opcode() == air::core::OPC_LD ||
               input->Opcode() == air::core::OPC_LDP);
    if (input->Opcode() == air::core::OPC_LD) {
      ADDR_DATUM_PTR input_data = input->Addr_datum();
      if (input_data->Is_formal()) {
        ctx.Trace(TF_SHARDING,
                  "Formal sharding: ", input_data->Name()->Char_str(), "\n");
        shmap->Set_data_sharding(input_data->Defining_func_scope()->Id(),
                                 input_data->Id(), input_shard);
      } else {
        AIR_ASSERT_MSG(false, "TODO: LD addr_datum");
      }
    } else {
      PREG_PTR input_data = input->Preg();
      if (input_numel > max_slots) {  // below is useless?
        const ARRAY_SHARDING* actual = shmap->Get_data_sharding(
            input_data->Defining_func_scope()->Id(), input_data->Id());
        AIR_ASSERT(actual != nullptr);
        ctx.Trace(TF_SHARDING, "Input expected: ", input_shard);
        ctx.Trace(TF_SHARDING, "Input actual: ", *actual);
      } else {
        // no sharding for input from previous operator output, set it
        shmap->Set_data_sharding(input_data->Defining_func_scope()->Id(),
                                 input_data->Id(), input_shard);
        ctx.Trace(TF_SHARDING, "Input expected: ", input_shard);
      }
    }

    // Set sharding info for successor
    NODE_PTR output = ctx.Parent(1);
    AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
               output->Opcode() == air::core::OPC_STP);
    if (output->Opcode() == air::core::OPC_ST) {
      ADDR_DATUM_PTR output_data = output->Addr_datum();
      AIR_ASSERT(!output_data->Is_formal());
      shmap->Set_data_sharding(output_data->Defining_func_scope()->Id(),
                               output_data->Id(), output_shard);
    } else {
      PREG_PTR output_data = output->Preg();
      shmap->Set_data_sharding(output_data->Defining_func_scope()->Id(),
                               output_data->Id(), output_shard);
    }

    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR child = node->Child(0);
    AIR_ASSERT(node->Rtype()->Is_array());
    AIR_ASSERT(child->Rtype()->Is_array());
    AIR_ASSERT((child->Opcode() != air::core::OPC_LD) ||
               (!child->Addr_datum()->Is_formal()));

    T2TSHARDING_ANALYSIS_CTX& ctx   = visitor->Context();
    int64_t                   slots = ctx.Max_slots();
    int64_t in_count  = child->Rtype()->Cast_to_arr()->Elem_count();
    int64_t out_count = node->Rtype()->Cast_to_arr()->Elem_count();

    std::vector<int64_t> in_shape  = child->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> out_shape = node->Rtype()->Cast_to_arr()->Shape();

    AIR_ASSERT(in_shape[0] == out_shape[0] && in_shape[1] == out_shape[1]);
    AIR_ASSERT(in_shape[2] == in_shape[3]);
    AIR_ASSERT(out_shape[2] == 1 && out_shape[3] == 1);
    AIR_ASSERT_MSG(in_shape[2] * in_shape[3] <= slots,
                   "TODO: global_avg_pool h*w > slots");

    ctx.Trace(TF_SHARDING, ">> Sharding analysis: ", node->Name(),
              " slot=", slots, " in_count=", in_count, " out_count=", out_count,
              "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, child->Rtype(), "Input ");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, node->Rtype(), "Output");

    SHARDING_MAP* shmap = ctx.Sharding_map();
    if (in_count < slots) {
      ctx.Trace(TF_SHARDING, "No sharding is needed!\n");

      ARRAY_SHARDING output_shard;
      output_shard.Set_spec({1, 1, 1, 1});

      FUNC_ID  scope  = node->Container()->Parent_func_scope()->Id();
      NODE_PTR output = ctx.Parent(1);
      AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
                 output->Opcode() == air::core::OPC_STP);
      (output->Opcode() == air::core::OPC_ST)
          ? shmap->Set_data_sharding(scope, output->Addr_datum_id(),
                                     output_shard)
          : shmap->Set_data_sharding(scope, output->Preg_id(), output_shard);

      return RETV();
    }

    int64_t x = (out_count % slots == 0) ? (out_count / slots)
                                         : (out_count / slots + 1);
    int64_t y =
        (in_count % slots == 0) ? (in_count / slots) : (in_count / slots + 1);
    int64_t z = 1;

    DIM3           mesh = {x, y, z};
    OP_SHARDING    shard(mesh);
    ARRAY_SHARDING input_shard(mesh);
    input_shard.Set_spec({1, y, z, 1});
    ARRAY_SHARDING output_shard(mesh);
    output_shard.Set_spec({1, x, z, 1});
    shard.Set_imap(0, input_shard);
    shard.Set_omap(0, output_shard);

    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    shmap->Set_op_sharding(node, shard);
    std::vector<int64_t> shard_info{x, y, z, 0};
    node->Set_attr(core::ATTR::SHARDING, shard_info.data(), shard_info.size());

    FUNC_ID scope = node->Container()->Parent_func_scope()->Id();
    AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_LD ||
               node->Child(0)->Opcode() == air::core::OPC_LDP);
    const ARRAY_SHARDING* actual =
        (node->Child(0)->Opcode() == air::core::OPC_LD)
            ? shmap->Get_data_sharding(scope, node->Child(0)->Addr_datum_id())
            : shmap->Get_data_sharding(scope, node->Child(0)->Preg_id());
    AIR_ASSERT(actual != nullptr);

    ctx.Trace(TF_SHARDING, "Input expected: ", input_shard);
    ctx.Trace(TF_SHARDING, "Input actual: ", *actual);

    NODE_PTR output = ctx.Parent(1);
    AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
               output->Opcode() == air::core::OPC_STP);
    (output->Opcode() == air::core::OPC_ST)
        ? shmap->Set_data_sharding(scope, output->Addr_datum_id(), output_shard)
        : shmap->Set_data_sharding(scope, output->Preg_id(), output_shard);

    return RETV(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_relu(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_elem_wise_op<RETV, VISITOR>(visitor, node);
  }

private:
  // handle all elemnent-wize op like add/mul/relu/etc
  template <typename RETV, typename VISITOR>
  RETV Handle_elem_wise_op(VISITOR* visitor, air::base::NODE_PTR node) {
    // validate types
    AIR_ASSERT(node->Rtype()->Is_array());
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      AIR_ASSERT(node->Rtype()->Is_compatible_type(node->Child(i)->Rtype()));
    }

    // get shape and count from Rtype
    T2TSHARDING_ANALYSIS_CTX& ctx   = visitor->Context();
    int64_t                   slots = ctx.Max_slots();
    std::vector<int64_t>      shape = node->Rtype()->Cast_to_arr()->Shape();

    // When the reshape op appears in the middle of the model, the shape reduces
    // to 2 dims in all current models. The temporary solution is to insert two
    // 1 in the front of shapes to make shape size to 4.
    AIR_ASSERT((shape.size() == 4 || shape.size() == 2) && shape[0] == 1);
    if (shape.size() == 2) {
      shape.insert(shape.begin(), {1, 1});
    }
    int64_t count = node->Rtype()->Cast_to_arr()->Elem_count();
    AIR_ASSERT(count == shape[0] * shape[1] * shape[2] * shape[3]);
    ctx.Trace(TF_SHARDING, ">> Sharding analysis: ", node->Name(),
              " slot=", slots, " count=", count, " shape=[", shape[0], ", ",
              shape[1], ", ", shape[2], ", ", shape[3], "]\n");

    // check if sharding is needed
    if (count < slots) {
      ctx.Trace(TF_SHARDING, "No sharding is needed!\n");
      return RETV(node);
    }

    // analyze shape to get suitable sharding scheme
    SHARDING_MAP*        shmap = ctx.Sharding_map();
    std::vector<int64_t> pspec =
        shmap->Analyze_conv(slots, shape[1], shape[1], shape[2], shape[3]);
    DIM3           mesh = {pspec[0], pspec[1], pspec[2]};
    ARRAY_SHARDING input_shard(mesh);
    input_shard.Set_spec({1, pspec[1], pspec[2], 1});
    ARRAY_SHARDING output_shard(mesh);
    output_shard.Set_spec({1, pspec[0], pspec[2], 1});
    OP_SHARDING shard(mesh);
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      shard.Set_imap(i, input_shard);
    }
    shard.Set_omap(0, output_shard);
    shmap->Set_op_sharding(node, shard);
    std::vector<int64_t> shard_info{pspec[0], pspec[1], pspec[2], 0};
    node->Set_attr(core::ATTR::SHARDING, shard_info.data(), shard_info.size());
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());

    // check and validate expected and actual sharding on input
    FUNC_ID scope = node->Container()->Parent_func_scope()->Id();
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      AIR_ASSERT(node->Child(i)->Opcode() == air::core::OPC_LD ||
                 node->Child(i)->Opcode() == air::core::OPC_LDP);
      const ARRAY_SHARDING* actual =
          (node->Child(i)->Opcode() == air::core::OPC_LD)
              ? shmap->Get_data_sharding(scope, node->Child(i)->Addr_datum_id())
              : shmap->Get_data_sharding(scope, node->Child(i)->Preg_id());
      AIR_ASSERT(actual != nullptr);

      ctx.Trace(TF_SHARDING, "Input ", i, " expected: ", input_shard);
      ctx.Trace(TF_SHARDING, "Input ", i, " actual: ", *actual);
    }

    NODE_PTR output = ctx.Parent(1);
    AIR_ASSERT(output->Opcode() == air::core::OPC_ST ||
               output->Opcode() == air::core::OPC_STP);
    (output->Opcode() == air::core::OPC_ST)
        ? shmap->Set_data_sharding(scope, output->Addr_datum_id(), output_shard)
        : shmap->Set_data_sharding(scope, output->Preg_id(), output_shard);

    return RETV(node);
  }
};

//! @brief T2TSHARDING_ANA_CORE_HANDLER handler
class T2TSHARDING_ANALYSIS_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  T2TSHARDING_ANALYSIS_CORE_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    // validate types
    AIR_ASSERT(node->Num_child() == 1);
    NODE_PTR child_node = node->Child(0);
    AIR_ASSERT(child_node->Rtype()->Is_array());

    // get shape and count from Rtype
    T2TSHARDING_ANALYSIS_CTX& ctx   = visitor->Context();
    int64_t                   slots = ctx.Max_slots();
    std::vector<int64_t> shape = child_node->Rtype()->Cast_to_arr()->Shape();

    // When the reshape op appears in the middle of the model, the shape reduces
    // to 2 dims in all current models. The temporary solution is to insert two
    // 1 in the front of shapes to make shape size to 4.
    AIR_ASSERT((shape.size() == 4 || shape.size() == 2) && shape[0] == 1);
    if (shape.size() == 2) {
      shape.insert(shape.begin(), {1, 1});
    }
    int64_t count = child_node->Rtype()->Cast_to_arr()->Elem_count();
    AIR_ASSERT(count == shape[0] * shape[1] * shape[2] * shape[3]);
    ctx.Trace(TF_SHARDING, ">> Sharding analysis: ", node->Name(),
              " slot=", slots, " count=", count, " shape=[", shape[0], ", ",
              shape[1], ", ", shape[2], ", ", shape[3], "]\n");

    // check if sharding is needed
    if (count < slots) {
      ctx.Trace(TF_SHARDING, "No sharding is needed!\n");
      return RETV(node);
    }

    // analyze shape to get suitable sharding scheme
    SHARDING_MAP*        shmap = ctx.Sharding_map();
    std::vector<int64_t> pspec =
        shmap->Analyze_conv(slots, shape[1], shape[1], shape[2], shape[3]);
    DIM3           mesh = {pspec[0], pspec[1], pspec[2]};
    ARRAY_SHARDING input_shard(mesh);
    input_shard.Set_spec({1, pspec[1], pspec[2], 1});
    ARRAY_SHARDING output_shard(mesh);
    output_shard.Set_spec({1, pspec[0], pspec[2], 1});
    OP_SHARDING shard(mesh);
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      shard.Set_imap(i, input_shard);
    }
    shard.Set_omap(0, output_shard);
    shmap->Set_op_sharding(node, shard);
    std::vector<int64_t> shard_info{pspec[0], pspec[1], pspec[2], 0};
    node->Set_attr(core::ATTR::SHARDING, shard_info.data(), shard_info.size());
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());

    // check and validate expected and actual sharding on input
    FUNC_ID scope = node->Container()->Parent_func_scope()->Id();
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      AIR_ASSERT(node->Child(i)->Opcode() == air::core::OPC_LD ||
                 node->Child(i)->Opcode() == air::core::OPC_LDP);
      const ARRAY_SHARDING* actual =
          (node->Child(i)->Opcode() == air::core::OPC_LD)
              ? shmap->Get_data_sharding(scope, node->Child(i)->Addr_datum_id())
              : shmap->Get_data_sharding(scope, node->Child(i)->Preg_id());
      AIR_ASSERT(actual != nullptr);

      ctx.Trace(TF_SHARDING, "Input ", i, " expected: ", input_shard);
      ctx.Trace(TF_SHARDING, "Input ", i, " actual: ", *actual);
    }

    shmap->Set_data_sharding(scope, child_node->Addr_datum_id(), output_shard);

    return RETV(node);
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_HANDLER_H
