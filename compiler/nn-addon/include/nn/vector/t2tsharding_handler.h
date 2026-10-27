//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_T2TSHARDING_HANDLER_H
#define NN_T2TSHARDING_HANDLER_H

#include "air/base/transform_util.h"
#include "nn/core/default_handler.h"
#include "nn/vector/sharding.h"
#include "nn/vector/t2tsharding_ctx.h"
#include "nn/vector/tensor2vector_util.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

class T2TSHARDING_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  T2TSHARDING_HANDLER() {}

  // TODO: Actyally every op needs sharding transformation.
  // Now the following ops falls backs to default Clone.
  // Assuming they don't need sharding now.
  // Handle_flatten, Handle_gemm

  // Handle_add: needed
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();
    SPOS             spos = node->Spos();

    SHARDING_MAP* shmap = ctx.Sharding_map();
    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      NODE_PTR new_node = cntr->Clone_node(node);
      NODE_PTR child0   = visitor->template Visit<RETV>(node->Child(0)).Node();
      NODE_PTR child1   = visitor->template Visit<RETV>(node->Child(1)).Node();
      AIR_ASSERT(child0 != air::base::Null_ptr &&
                 child1 != air::base::Null_ptr);
      new_node->Set_child(0, child0);
      new_node->Set_child(1, child1);
      return RETV(new_node);
    }

    OP_SHARDING shard = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    std::vector<int64_t> xyz = shard.Imap(0).Spec();
    int64_t              y = xyz[1], z = xyz[2];
    AIR_ASSERT(xyz[0] == 1);

    NODE_PTR new_input0 = visitor->template Visit<RETV>(node->Child(0)).Node();
    NODE_PTR new_input1 = visitor->template Visit<RETV>(node->Child(1)).Node();
    AIR_ASSERT(new_input0 != air::base::Null_ptr &&
               new_input0->Opcode() == air::core::OPC_LD);
    AIR_ASSERT(new_input1 != air::base::Null_ptr &&
               new_input1->Opcode() == air::core::OPC_LD);
    ADDR_DATUM_PTR input0_sharding_var = new_input0->Addr_datum();
    ADDR_DATUM_PTR input1_sharding_var = new_input1->Addr_datum();

    TYPE_PTR elem_type = node->Child(0)->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_shape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> output_block_shape{
        output_shape[0], output_shape[1] / y, output_shape[2] / z,
        output_shape[3]};
    ADDR_DATUM_PTR output_sharding_var =
        New_sharding_var(cntr, "out_add", ctx.Get_num_vloop(), y * z,
                         output_block_shape, elem_type, spos);
    shmap->Add_data_sharding(output_sharding_var);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, output_sharding_var->Type(),
                  "output_sharding_var[y*z]");
    TYPE_PTR output_block_type =
        output_sharding_var->Type()->Cast_to_arr()->Elem_type();

    // output_sharding_var[0:x] = 0
    NODE_PTR zero_node = cntr->New_zero(output_sharding_var->Type(), spos);
    STMT_PTR st0_output_sharding =
        cntr->New_st(zero_node, output_sharding_var, spos);
    ctx.Prepend(st0_output_sharding);

    std::vector<STMT_PTR> loops;
    loops         = ctx.New_ranged_loop(cntr, {y, z}, spos);
    NODE_PTR yiv  = cntr->New_ld(loops[0]->Node()->Iv(), spos);
    NODE_PTR ziv  = cntr->New_ld(loops[1]->Node()->Iv(), spos);
    STMT_PTR body = New_add_sharding_body(
        cntr, node, input0_sharding_var, input1_sharding_var,
        output_sharding_var, yiv, ziv, {y, y, z}, output_block_type, spos);

    STMT_LIST inner_sl = Get_loop_sl(loops[loops.size() - 1]);
    inner_sl.Append(body);
    ctx.Prepend(loops[0]);

    NODE_PTR ld_res = cntr->New_ld(output_sharding_var, spos);

    return RETV(ld_res);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_average_pool<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();
    SPOS             spos = node->Spos();

    SHARDING_MAP* shmap = ctx.Sharding_map();
    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      NODE_PTR new_node = cntr->Clone_node(node);
      NODE_PTR child    = visitor->template Visit<RETV>(node->Child(0)).Node();
      AIR_ASSERT(child != air::base::Null_ptr);
      new_node->Set_child(0, child);
      return RETV(new_node);
    }

    OP_SHARDING shard = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    std::vector<int64_t> xyz = shard.Imap(0).Spec();
    int64_t              y = xyz[1], z = xyz[2];
    int64_t              halosize = shard.Imap(0).Halosize();
    AIR_ASSERT_MSG(xyz[0] == 1, "average pool increasing output channel?");

    NODE_PTR new_input = visitor->template Visit<RETV>(node->Child(0)).Node();
    AIR_ASSERT(new_input != air::base::Null_ptr &&
               new_input->Opcode() == air::core::OPC_LD);
    ADDR_DATUM_PTR input_sharding_var = new_input->Addr_datum();

    // per-shard avgpool at first
    TYPE_PTR elem_type = node->Child(0)->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_shape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> output_block_shape{
        output_shape[0], output_shape[1] / y, output_shape[2] / z,
        output_shape[3]};
    ADDR_DATUM_PTR output_sharding_var =
        New_sharding_var(cntr, "out_avp", ctx.Get_num_vloop(), y * z,
                         output_block_shape, elem_type, spos);
    shmap->Add_data_sharding(output_sharding_var);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, output_sharding_var->Type(),
                  "output_sharding_var[y*z]");
    TYPE_PTR output_block_type =
        output_sharding_var->Type()->Cast_to_arr()->Elem_type();

    // output_sharding_var[0:x] = 0
    NODE_PTR zero_node = cntr->New_zero(output_sharding_var->Type(), spos);
    STMT_PTR st0_output_sharding =
        cntr->New_st(zero_node, output_sharding_var, spos);
    ctx.Prepend(st0_output_sharding);

    std::vector<STMT_PTR> loops;
    loops         = ctx.New_ranged_loop(cntr, {y, z}, spos);
    NODE_PTR yiv  = cntr->New_ld(loops[0]->Node()->Iv(), spos);
    NODE_PTR ziv  = cntr->New_ld(loops[1]->Node()->Iv(), spos);
    STMT_PTR body = New_average_pool_sharding_body(
        cntr, node, input_sharding_var, output_sharding_var, yiv, ziv,
        {y, y, z}, output_block_type, halosize, spos);

    STMT_LIST inner_sl = Get_loop_sl(loops[loops.size() - 1]);
    inner_sl.Append(body);
    ctx.Prepend(loops[0]);

    // shrink sharding number
    std::vector<int> strides = Get_attr_data<int>(node, nn::core::ATTR::STRIDE);
    AIR_ASSERT(strides[0] == strides[1]);
    NODE_PTR ld_res;
    if (strides[0] > 0) {
      int numtoone    = (y * z == 2) ? 2 : strides[0] * strides[1];
      int num_reshard = (y * z / numtoone);
      ctx.Trace(TF_SHARDING, "output resharding: stride=[", strides[0], ", ",
                strides[1], ")\n");
      ctx.Trace(TF_SHARDING, "num_reshard=", num_reshard,
                " numtoone=", numtoone, " y=", y, " z=", z, "\n");
      std::vector<int64_t> reshard_block_shape{
          output_shape[0], output_shape[1] / y * numtoone / z, output_shape[2],
          output_shape[3]};

      ADDR_DATUM_PTR reshard_var =
          ctx.Reshard(output_sharding_var, num_reshard, numtoone,
                      reshard_block_shape, {y, y, z}, spos);
      ctx.Trace_cmd(TF_SHARDING, Trace_array_type, reshard_var->Type(),
                    "output_reshard");
      ld_res = cntr->New_ld(reshard_var, spos);
    } else {
      ld_res = cntr->New_ld(output_sharding_var, spos);
    }

    return RETV(ld_res);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_concat(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    GLOB_SCOPE*      gscope = cntr->Glob_scope();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();
    SPOS             spos   = node->Spos();

    ctx.Incr_num_vloop();
    SHARDING_MAP* shmap = ctx.Sharding_map();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      NODE_PTR new_node = visitor->Context().Container()->Clone_node(node);
      NODE_PTR child0   = visitor->template Visit<RETV>(node->Child(0)).Node();
      NODE_PTR child1   = visitor->template Visit<RETV>(node->Child(1)).Node();
      AIR_ASSERT(child0 != air::base::Null_ptr &&
                 child1 != air::base::Null_ptr);
      new_node->Set_child(0, child0);
      new_node->Set_child(1, child1);
      return new_node;
    }

    OP_SHARDING shard = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    std::vector<int64_t> xyz = shard.Omap(0).Spec();
    int64_t              x = xyz[0], y = xyz[1], z = xyz[2];
    AIR_ASSERT_MSG(z == 1, "TODO: intra-channel concat");

    NODE_PTR new_input0 = visitor->template Visit<RETV>(node->Child(0)).Node();
    NODE_PTR new_input1 = visitor->template Visit<RETV>(node->Child(1)).Node();
    AIR_ASSERT(new_input0 != air::base::Null_ptr &&
               new_input0->Opcode() == air::core::OPC_LD);
    AIR_ASSERT(new_input1 != air::base::Null_ptr &&
               new_input1->Opcode() == air::core::OPC_LD);
    ADDR_DATUM_PTR input0_sharding_var = new_input0->Addr_datum();
    ADDR_DATUM_PTR input1_sharding_var = new_input1->Addr_datum();

    TYPE_PTR elem_type = node->Child(0)->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_shape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> output_block_shape{
        output_shape[0], output_shape[1] / y, output_shape[2] / z,
        output_shape[3]};
    ADDR_DATUM_PTR output_sharding_var =
        New_sharding_var(cntr, "out_cocat", ctx.Get_num_vloop(), y * z,
                         output_block_shape, elem_type, spos);
    shmap->Add_data_sharding(output_sharding_var);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, output_sharding_var->Type(),
                  "output_sharding_var[y*z]");
    TYPE_PTR output_block_type =
        output_sharding_var->Type()->Cast_to_arr()->Elem_type();

    std::vector<int64_t> in0 = shard.Imap(0).Spec();
    std::vector<int64_t> in1 = shard.Imap(1).Spec();
    AIR_ASSERT(in0[1] + in1[1] == xyz[1]);

    STMT_PTR loop0 =
        ctx.New_mesh_loop(cntr, "concat_op0", in0[1] * in0[2], spos);
    NODE_PTR iv0   = cntr->New_ld(loop0->Node()->Iv(), spos);
    STMT_PTR body0 = New_concat_sharding_body(cntr, 0, iv0, input0_sharding_var,
                                              output_sharding_var, spos);
    Get_loop_sl(loop0).Append(body0);
    ctx.Prepend(loop0);

    STMT_PTR loop1 =
        ctx.New_mesh_loop(cntr, "concat_op1", in1[1] * in1[2], spos);
    NODE_PTR iv1   = cntr->New_ld(loop1->Node()->Iv(), spos);
    STMT_PTR body1 = New_concat_sharding_body(cntr, in0[1] * in0[2], iv1,
                                              input1_sharding_var,
                                              output_sharding_var, spos);
    Get_loop_sl(loop1).Append(body1);
    ctx.Prepend(loop1);

    NODE_PTR ld_res = cntr->New_ld(output_sharding_var, spos);
    return RETV(ld_res);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    GLOB_SCOPE*      gscope = cntr->Glob_scope();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();
    SPOS             spos   = node->Spos();

    ctx.Incr_num_vloop();
    SHARDING_MAP* shmap = ctx.Sharding_map();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      NODE_PTR clone_node = visitor->Context().Container()->Clone_node(node);
      NODE_PTR new_input = visitor->template Visit<RETV>(node->Child(0)).Node();
      clone_node->Set_child(0, new_input);
      clone_node->Set_child(1, cntr->New_ldc(node->Child(1)->Const(), spos));
      clone_node->Set_child(2, cntr->New_ldc(node->Child(2)->Const(), spos));
      return clone_node;
    }

    // Verification & Get shapes
    std::vector<int> groups = Get_attr_data<int>(node, core::ATTR::GROUP);
    int              group  = groups[0];
    int64_t batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    int64_t channel_out = 0, kernel_height = 0, kernel_width = 0;
    Verify_conv(node, batch, channel_in, input_height, input_width, channel_out,
                kernel_height, kernel_width, group);

    NODE_PTR orig_input  = node->Child(0);
    NODE_PTR weight_node = node->Child(1);
    NODE_PTR bias_node   = node->Child(2);

    NODE_PTR new_input = visitor->template Visit<RETV>(node->Child(0)).Node();
    // Q: Is new_input always sharding var? A: No
    // if no sharding from previous operator output, new_input can be a PREG
    // Note: input_sharding_var shape is [y*z]

    // 1. Get sharding strategy: N-dimension partition
    OP_SHARDING conv_sharding = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, conv_sharding, node->Name());
    std::vector<int64_t> xyz = conv_sharding.Imap(1).Spec();
    int64_t              x = xyz[0], y = xyz[1];
    std::vector<int64_t> xyz_input = conv_sharding.Imap(0).Spec();
    int64_t              z         = xyz_input[2];
    int64_t              halosize  = conv_sharding.Imap(0).Halosize();

    std::vector<int> strides = Get_attr_data<int>(node, nn::core::ATTR::STRIDE);
    int              stride  = strides[0];

    // Note: Halo is added to height.
    TYPE_PTR etype = orig_input->Rtype()->Cast_to_arr()->Elem_type();

    // same as input shard
    TYPE_PTR             ib_type  = new_input->Opcode() == air::core::OPC_LDP
                                        ? new_input->Rtype()
                                        : ctx.Get_block_type(new_input->Addr_datum());
    std::vector<int64_t> ib_shape = ib_type->Cast_to_arr()->Shape();
    ADDR_DATUM_PTR       out_conv = New_sharding_var(
        cntr, "out_conv", ctx.Get_num_vloop(), x * z, ib_shape, etype, spos);
    shmap->Add_data_sharding(out_conv);

    // output block shape: [F/x, H/z, W]
    // Note that: Tensor IR's output type correctness.
    std::vector<int64_t> oshape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> ob_shape{oshape[0], oshape[1] / x, oshape[2] / z,
                                  oshape[3]};
    ADDR_DATUM_PTR       out_conv_halo =
        New_sharding_var(cntr, "out_conv_halo", ctx.Get_num_vloop(), x * z,
                         ob_shape, etype, spos);
    shmap->Add_data_sharding(out_conv_halo);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, out_conv_halo->Type(),
                  "output_sharding_var[x*z]");

    TYPE_PTR ob_type = ctx.Get_block_type(out_conv_halo);

    // 2. Const splitter: weight_node[F/x, C/y, K, K]
    ctx.Trace_cmd(TF_SHARDING, Trace_float_array, weight_node->Const(),
                  "conv_weight");

    if (group > 1) {
      ctx.Trace(TF_SHARDING, "group>1: adjust channel_in of kernel, and y\n");
      channel_in = 1;
      // no need to sum in channel_in dimension due to group
      y = 1;
    }
    std::vector<int64_t> weight_block_shape{channel_out / x, channel_in / y,
                                            kernel_height, kernel_width};
    CONSTANT_PTR         weight_sharding_const = shmap->Split_array_const(
        weight_node->Const(), x * y, weight_block_shape, etype, spos);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, weight_sharding_const->Type(),
                  "weight_sharding_const[x*y]");

    // Split bias[channel_out] -> [x*z][output_block_shape]
    // Broadcast is handled here because we don't want to push bias into
    // halo/stride_slice world.
    CONSTANT_PTR largebias_const = shmap->Broadcast_bias(
        bias_node->Const(), channel_out, oshape[2], oshape[3], spos);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, largebias_const->Type(),
                  "largebias_const");

    int oreshard    = conv_sharding.Omap(0).Reshard();
    int numtoone    = (x * z == 2) ? 2 : stride * stride;
    int num_reshard = (x * z / numtoone);
    if (!oreshard) {
      numtoone    = 1;
      num_reshard = x;
    }

    CONSTANT_PTR bias_sharding_const;
    if (num_reshard == 1)
      bias_sharding_const = largebias_const;
    else {
      // Note: Carafully compute the bias split: stride&no-stride. bias-add is
      // after reshard.
      // to 2D. TODO: need a beautiful shape deduction.

      bias_sharding_const = shmap->Split_array_const(
          largebias_const, num_reshard,
          {oshape[0], oshape[1] / num_reshard, oshape[2], oshape[3]}, etype,
          spos);
    }
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, bias_sharding_const->Type(),
                  "bias_sharding_const[x]");

    // zero bias for conv-shard
    int64_t              bias_block_size = channel_out / x;
    std::vector<int64_t> bias_block_shape{bias_block_size};
    TYPE_PTR             bias_block_type =
        New_array_type(gscope, "Tbias_block", ctx.Get_num_vloop(), etype,
                       bias_block_shape, spos);
    std::vector<float> bias_zero(bias_block_size, 0.0);
    CONSTANT_PTR       bias_zero_const = gscope->New_const(
        CONSTANT_KIND::ARRAY, bias_block_type, (void*)bias_zero.data(),
        bias_block_size * etype->Cast_to_prim()->Byte_size());

    // INIT loop: out_conv[0:x] = 0
    NODE_PTR zero_node    = cntr->New_zero(out_conv->Type(), spos);
    STMT_PTR st0_out_conv = cntr->New_st(zero_node, out_conv, spos);
    ctx.Prepend(st0_out_conv);

    NODE_PTR zero_node_halo    = cntr->New_zero(out_conv_halo->Type(), spos);
    STMT_PTR st0_out_conv_halo = cntr->New_st(zero_node, out_conv_halo, spos);
    ctx.Prepend(st0_out_conv_halo);

    ctx.New_rshard_conv2d(node, new_input, weight_sharding_const,
                          bias_zero_const, out_conv, {x, y, z}, halosize, spos);

    if (stride > 1 || halosize >= 1)
      ctx.New_eshard_slice(out_conv, out_conv_halo, strides, halosize, spos);
    else
      out_conv_halo = out_conv;

    // Handle output resharding in x&z dims: case
    // stride=2 x*z -> x*z/4
    NODE_PTR ld_result;
    if (oreshard) {
      ctx.Trace(TF_SHARDING, "output resharding: stride=", stride, "\n");

      ctx.Trace(TF_SHARDING, "num_reshard=", num_reshard,
                " numtoone=", numtoone, " x*z=", x * z, "\n");

      std::vector<int64_t> reshard_block_shape{
          oshape[0], oshape[1] / x * numtoone / z, oshape[2], oshape[3]};

      ADDR_DATUM_PTR reshard_var =
          ctx.Reshard(out_conv_halo, num_reshard, numtoone, reshard_block_shape,
                      {x, y, z}, spos);
      ctx.Trace_cmd(TF_SHARDING, Trace_array_type, reshard_var->Type(),
                    "output_reshard");
      ADDR_DATUM_PTR add_bias =
          ctx.Add_shard(reshard_var, bias_sharding_const, {x, z}, spos);
      ld_result = cntr->New_ld(add_bias, spos);
    } else {
      // add bias
      if (x == 1 && z == 1) {
        // output only contains 1 element, extract it and save to a PREG
        TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
        PREG_PTR res_preg = fscope->New_preg(node->Rtype());
        NODE_PTR res_ld   = New_sharding_loader(
            cntr, out_conv_halo, cntr->New_intconst(s32_type, 0, spos), spos);
        ctx.Prepend(cntr->New_stp(res_ld, res_preg, spos));
        ld_result = cntr->New_ldp(res_preg, spos);
        // TODO: handle bias_sharding_const. This is not handled in Add_shard()
      } else {
        ADDR_DATUM_PTR add_bias =
            ctx.Add_shard(out_conv_halo, bias_sharding_const, {x, z}, spos);
        ld_result = cntr->New_ld(add_bias, spos);
      }
    }

    return RETV(ld_result);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    GLOB_SCOPE*      gscope = cntr->Glob_scope();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();
    SPOS             spos   = node->Spos();

    SHARDING_MAP* shmap = ctx.Sharding_map();
    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      NODE_PTR new_node = cntr->Clone_node(node);
      NODE_PTR child    = visitor->template Visit<RETV>(node->Child(0)).Node();
      AIR_ASSERT(child != air::base::Null_ptr);
      new_node->Set_child(0, child);
      return RETV(new_node);
    }

    OP_SHARDING shard = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    std::vector<int64_t> xyz = shard.Imap(0).Spec();
    int64_t              y = xyz[1], z = xyz[2];
    int64_t              halosize = shard.Imap(0).Halosize();
    AIR_ASSERT_MSG(xyz[0] == 1 && z == 1 && halosize == 0,
                   "TODO: support intra channel sharding");

    NODE_PTR new_input = visitor->template Visit<RETV>(node->Child(0)).Node();
    AIR_ASSERT(new_input != air::base::Null_ptr &&
               new_input->Opcode() == air::core::OPC_LD);
    ADDR_DATUM_PTR input_sharding_var = new_input->Addr_datum();

    // per-shard global_avgpool at first
    TYPE_PTR elem_type = node->Child(0)->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_shape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> output_block_shape{
        output_shape[0], output_shape[1] / y, output_shape[2] / z,
        output_shape[3]};
    ADDR_DATUM_PTR output_sharding_var =
        New_sharding_var(cntr, "out_gap", ctx.Get_num_vloop(), y * z,
                         output_block_shape, elem_type, spos);
    shmap->Add_data_sharding(output_sharding_var);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, output_sharding_var->Type(),
                  "output_sharding_var[y*z]");
    TYPE_PTR output_block_type =
        output_sharding_var->Type()->Cast_to_arr()->Elem_type();

    // output_sharding_var[0:x] = 0
    NODE_PTR zero_node = cntr->New_zero(output_sharding_var->Type(), spos);
    STMT_PTR st0_output_sharding =
        cntr->New_st(zero_node, output_sharding_var, spos);
    ctx.Prepend(st0_output_sharding);

    std::vector<STMT_PTR> loops;
    loops         = ctx.New_ranged_loop(cntr, {y, z}, spos);
    NODE_PTR yiv  = cntr->New_ld(loops[0]->Node()->Iv(), spos);
    NODE_PTR ziv  = cntr->New_ld(loops[1]->Node()->Iv(), spos);
    STMT_PTR body = New_global_avgpool_sharding_body(
        cntr, node, input_sharding_var, output_sharding_var, yiv, ziv,
        {y, y, z}, output_block_type, spos);

    STMT_LIST inner_sl = Get_loop_sl(loops[loops.size() - 1]);
    inner_sl.Append(body);
    ctx.Prepend(loops[0]);

    // shrink sharding number
    // TODO: refine this part with T2TSHARDING_CTX::Reshard(). So far reshard
    // can't handle this case.
    AIR_ASSERT_MSG(y >= 2 && y <= 4, "TODO: y out of [2, 4]");
    TYPE_PTR s32_type   = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
    NODE_PTR gather_res = New_sharding_loader(
        cntr, output_sharding_var, cntr->New_intconst(s32_type, 0, spos), spos);
    for (int64_t i = 1; i < y; ++i) {
      TYPE_PTR rtype =
          (i != y - 1)
              ? New_array_type(gscope, "AVG_output_res", ctx.Get_num_vloop(),
                               elem_type,
                               {output_shape[0], output_shape[1] * i / y,
                                output_shape[2], output_shape[3]},
                               spos)
              : node->Rtype();
      gather_res = New_gather_node(
          cntr, gather_res,
          New_sharding_loader(cntr, output_sharding_var,
                              cntr->New_intconst(s32_type, i, spos), spos),
          rtype, 1, spos);
    }
    PREG_PTR reshard_preg = fscope->New_preg(node->Rtype());
    STMT_PTR gather_st    = cntr->New_stp(gather_res, reshard_preg, spos);
    ctx.Prepend(gather_st);
    NODE_PTR ld_res = cntr->New_ldp(reshard_preg, spos);
    return RETV(ld_res);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_relu(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();
    SPOS             spos = node->Spos();

    SHARDING_MAP* shmap = ctx.Sharding_map();
    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      NODE_PTR new_node = cntr->Clone_node(node);
      NODE_PTR child    = visitor->template Visit<RETV>(node->Child(0)).Node();
      AIR_ASSERT(child != air::base::Null_ptr);
      new_node->Set_child(0, child);
      return RETV(new_node);
    }

    OP_SHARDING shard = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    std::vector<int64_t> xyz = shard.Imap(0).Spec();
    int64_t              y = xyz[1], z = xyz[2];
    AIR_ASSERT(xyz[0] == 1);

    NODE_PTR input = visitor->template Visit<RETV>(node->Child(0)).Node();
    AIR_ASSERT(input != air::base::Null_ptr &&
               input->Opcode() == air::core::OPC_LD);
    ADDR_DATUM_PTR input_sharding_var = input->Addr_datum();

    TYPE_PTR elem_type = node->Child(0)->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_shape = node->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> output_block_shape{
        output_shape[0], output_shape[1] / y, output_shape[2] / z,
        output_shape[3]};
    ADDR_DATUM_PTR output_sharding_var =
        New_sharding_var(cntr, "out_relu", ctx.Get_num_vloop(), y * z,
                         output_block_shape, elem_type, spos);
    shmap->Add_data_sharding(output_sharding_var);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, output_sharding_var->Type(),
                  "output_sharding_var[y*z]");
    TYPE_PTR output_block_type =
        output_sharding_var->Type()->Cast_to_arr()->Elem_type();

    // output_sharding_var[0:x] = 0
    NODE_PTR zero_node = cntr->New_zero(output_sharding_var->Type(), spos);
    STMT_PTR st0_output_sharding =
        cntr->New_st(zero_node, output_sharding_var, spos);
    ctx.Prepend(st0_output_sharding);

    std::vector<STMT_PTR> loops;
    loops         = ctx.New_ranged_loop(cntr, {y, z}, spos);
    NODE_PTR yiv  = cntr->New_ld(loops[0]->Node()->Iv(), spos);
    NODE_PTR ziv  = cntr->New_ld(loops[1]->Node()->Iv(), spos);
    STMT_PTR body = New_relu_sharding_body(cntr, node, input_sharding_var,
                                           output_sharding_var, yiv, ziv,
                                           {y, y, z}, output_block_type, spos);

    STMT_LIST inner_sl = Get_loop_sl(loops[loops.size() - 1]);
    inner_sl.Append(body);
    ctx.Prepend(loops[0]);

    NODE_PTR ld_res = cntr->New_ld(output_sharding_var, spos);
    return RETV(ld_res);
  }
};

//! @brief T2TSHARDING_CORE handler
class T2TSHARDING_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  T2TSHARDING_CORE_HANDLER() {}
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(), "\n");
    ADDR_DATUM_PTR formal     = node->Addr_datum();
    ADDR_DATUM_PTR new_formal = ctx.Sharding_map()->Get_shard_datum(formal);
    NODE_PTR       new_idname = cntr->New_idname(new_formal, node->Spos());
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, new_formal->Type(),
                  "New_formal");
    return RETV(new_idname);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(), "\n");

    ADDR_DATUM_PTR old_sym = node->Addr_datum();
    ADDR_DATUM_PTR new_sym = ctx.Sharding_map()->Get_shard_datum(old_sym);
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, old_sym->Type(), "Old");
    ctx.Trace_cmd(TF_SHARDING, Trace_array_type, new_sym->Type(), "New");
    NODE_PTR new_ld = cntr->New_ld(new_sym, node->Spos());
    return RETV(new_ld);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(), "\n");

    ADDR_DATUM_PTR old_sym = node->Addr_datum();

    NODE_PTR new_child = visitor->template Visit<RETV>(node->Child(0)).Node();
    TYPE_PTR rtype     = new_child->Rtype();

    // Only add sym map: new_sym if from "ld
    // new_sym". No new st is needed.
    if (new_child->Opcode() ==
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD)) {
      ADDR_DATUM_PTR new_sym = new_child->Addr_datum();
      ctx.Sharding_map()->Set_shard_datum(old_sym, new_sym);
      return RETV();
    }

    // New st node, and set sym map.
    const char*    new_sym_name(old_sym->Name()->Char_str());
    ADDR_DATUM_PTR new_sym =
        fscope->New_var(rtype, new_sym_name, old_sym->Spos());
    ctx.Sharding_map()->Set_shard_datum(old_sym, new_sym);

    STMT_PTR new_st = cntr->New_st(new_child, new_sym, node->Spos());
    return RETV(new_st->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(), "\n");

    // Sharding pass: op return "ld sharding_sym"
    // If no sharing, still maintain preg-preg map.
    PREG_PTR old_sym = node->Preg();

    if (ctx.Sharding_map()->Is_sharded(old_sym)) {
      ADDR_DATUM_PTR new_sym =
          ctx.Sharding_map()->Get_shard_preg_datum(old_sym);
      NODE_PTR new_ld = cntr->New_ld(new_sym, node->Spos());
      return RETV(new_ld);
    } else {
      PREG_PTR new_sym = ctx.Sharding_map()->Get_shard_preg(old_sym);
      NODE_PTR new_ldp = cntr->New_ldp(new_sym, node->Spos());
      return RETV(new_ldp);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();

    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(), "\n");

    PREG_PTR old_preg = node->Preg();

    NODE_PTR new_child = visitor->template Visit<RETV>(node->Child(0)).Node();
    TYPE_PTR rtype     = new_child->Rtype();

    // Handle sharding mapping:
    // case 1: ld sharding_sym: preg->sharding_sym
    // case 2: sym: preg-> sym: preg->sym
    if (new_child->Opcode() ==
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD)) {
      ADDR_DATUM_PTR new_sym = new_child->Addr_datum();
      ctx.Sharding_map()->Set_shard_preg_datum(old_preg, new_sym);
      return RETV();
    }

    // Other cases: stp preg1 (NN.flatten ...)
    // map: preg1 -> new_preg1.
    PREG_PTR new_preg =
        fscope->New_preg(old_preg->Type(), old_preg->Home_sym());
    ctx.Sharding_map()->Set_shard_preg(old_preg, new_preg);

    STMT_PTR new_stp = cntr->New_stp(new_child, new_preg, node->Spos());
    return RETV(new_stp->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx  = visitor->Context();
    CONTAINER*       cntr = ctx.Container();
    SPOS             spos = node->Spos();

    SHARDING_MAP* shmap = ctx.Sharding_map();
    ctx.Trace(TF_SHARDING, ">> Sharding transformation: ", node->Name(),
              shmap->Is_op_sharding(node), "\n");
    ctx.Trace_cmd(TF_SHARDING, Trace_node, node);

    if (!shmap->Is_op_sharding(node)) {
      STMT_PTR new_stmt = cntr->Clone_stmt(node->Stmt());
      NODE_PTR new_node = new_stmt->Node();
      NODE_PTR child    = visitor->template Visit<RETV>(node->Child(0)).Node();
      AIR_ASSERT(child != air::base::Null_ptr);
      new_node->Set_child(0, child);
      return RETV(new_node);
    }

    OP_SHARDING shard = shmap->Get_op_sharding(node);
    ctx.Trace_cmd(TF_SHARDING, Trace_op_sharding, shard, node->Name());
    std::vector<int64_t> xyz = shard.Imap(0).Spec();
    int64_t              y = xyz[1], z = xyz[2];
    AIR_ASSERT(xyz[0] == 1);
    int64_t halosize = shard.Imap(0).Halosize();

    TYPE_PTR orig_type = node->Child(0)->Rtype();

    NODE_PTR input = visitor->template Visit<RETV>(node->Child(0)).Node();
    AIR_ASSERT(input != air::base::Null_ptr &&
               input->Opcode() == air::core::OPC_LD);
    STMT_PTR new_retv  = cntr->New_retv(input, input->Spos());
    NODE_PTR retv_node = new_retv->Node();

    nn::core::Set_scheme_attr(retv_node, orig_type, 1, xyz[1], xyz[2], 1,
                              halosize);

    return RETV(retv_node);
  }
};

}  // namespace vector
}  // namespace nn

#endif
