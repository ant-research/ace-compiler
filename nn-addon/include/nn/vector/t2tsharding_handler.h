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

  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    GLOB_SCOPE*      gscope = cntr->Glob_scope();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();
    SPOS             spos   = node->Spos();

    // NODE_PTR new_input   = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR new_input =
        visitor->template Handle_node<RETV>(node->Child(0)).Node();

    //  Verification
    NODE_PTR orig_input = node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG(batch == 1, "Conv only supports batch=1");

    NODE_PTR weight_node = node->Child(1);
    int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
            kernel_width = 0;
    Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                   kernel_height, kernel_width);
    AIR_ASSERT_MSG(channel_in == channel_in_kernel,
                   "channel_in == channel_in_kernel");

    NODE_PTR bias_node = node->Child(2);

    // NODE_PTR new_input = visitor->template Visit<RETV>(node->Child(0));
    // new_input->Print();

    // 1. Get sharding strategy: N-dimension partition
    // input  [channel_in/y, input_height/z, input_width]
    // weight [channel_out/(x=channel_out), channel_in/y, kernel_height,
    // kernel_width]
    // output [channel_out/(x=channel_out), input_height/z,
    // input_width]
    ARRAY_SHARDING* shmap = ctx.Sharding();

    // ARRAY_SHARDING shmap(cntr);
    //  dim3[x,y,z]
    std::vector<int64_t> xyz = shmap->Get_node_map(node);
    //    shmap.Analyze_conv(channel_out, channel_in, input_height,
    //    input_width);
    int64_t x = xyz[0], y = xyz[1], z = xyz[2];

    // output block shape or computation shape: each partitioned conv works on
    // the shape.
    TYPE_PTR elem_type = orig_input->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_block_shape{1, channel_out / x,
                                            input_height / z, input_width};
    TYPE_PTR             output_block_type =
        New_array_type(gscope, "Tblock_output", ctx.Get_num_vloop(), elem_type,
                       output_block_shape, spos);

    // output sharding type
    TYPE_PTR output_sharding_type = shmap->New_sharding_type(
        "output", ctx.Get_num_vloop(), x, output_block_type, spos);

    std::string output_sharding_str =
        (std::string("output_sharding") + std::to_string(ctx.Get_num_vloop()));

    // output_sharding_var[x]
    ADDR_DATUM_PTR output_sharding_var = fscope->New_var(
        output_sharding_type, output_sharding_str.c_str(), spos);


    ADDR_DATUM_PTR orig_data = orig_input->Addr_datum();
    ADDR_DATUM_ID  new_did = ctx.Get_addr_datum_map(orig_data->Id());    
    ADDR_DATUM_PTR input_sharding_var = fscope->Addr_datum(new_did);

    // shmap->Set_map(output_sharding_type->Id().Value(), xyz);

    // 2. Split weight_node[channel_out, channel_in, kernel_height,
    // kernel_width]: num = x*y:
    // sharding_weight_const[channel_out/(x=channel_out), channel_in/y,
    // kernel_height, kernel_width]
    ctx.Trace_cmd(TF_LOWER, Trace_float_array, weight_node->Const(),
                  "conv_weight");
    std::vector<int64_t> weight_block_shape{1, 1, kernel_height, kernel_width};
    std::vector<CONSTANT_PTR> weight_blocks_const = shmap->Split_array(
        weight_node->Const(), x * y, weight_block_shape, elem_type, spos);

    // Split bias[channel_out]: num=x
    std::vector<int64_t>      bias_block_shape{1};
    std::vector<CONSTANT_PTR> bias_blocks_const = shmap->Split_array(
        bias_node->Const(), x, bias_block_shape, elem_type, spos);
    TYPE_PTR bias_block_type =
        New_array_type(gscope, "Tbias_block", ctx.Get_num_vloop(), elem_type,
                       bias_block_shape, spos);
    std::vector<float> bias_zero{0.0};
    CONSTANT_PTR       bias_zero_const = gscope->New_const(
        CONSTANT_KIND::ARRAY, bias_block_type, (void*)bias_zero.data(),
        elem_type->Cast_to_prim()->Byte_size());

    // INIT loop: output_sharding_var[0:x] = 0
    NODE_PTR zero_node = cntr->New_zero(output_sharding_type, spos);
    STMT_PTR st0_output_sharding =
        cntr->New_st(zero_node, output_sharding_var, spos);
    ctx.Prepend(st0_output_sharding);

    // 3. Sharding computation for conv
    // input:  0:y/cin           0:y/cin  -> replicaiton
    // weight: 0:y/cin/cout(x=0) 0:y/cin/cout(x=1)
    // reduce: add
    // output: 0                 1
    for (int i = 0; i < x; i++) {    // out
      for (int j = 0; j < y; j++) {  // in
        // output_sharding[i] += conv(input[j], weight_blocks_const[i][j],
        // bias_zero)
        NODE_PTR pconv_node = New_conv_node(
            cntr, shmap->New_sharding_loader(input_sharding_var, j, spos),
            cntr->New_ldc(weight_blocks_const[i * y + j], spos),
            cntr->New_ldc(bias_zero_const, spos), output_block_type, spos);
        pconv_node->Copy_attr(node);

        STMT_PTR conv_add_output_sharding = shmap->New_sharding_update_store(
            output_sharding_var, i, pconv_node, spos);
        ctx.Prepend(conv_add_output_sharding);
      }
      // output[i] += bias_blocks_const[i];
      STMT_PTR bias_add_output_sharding = shmap->New_sharding_update_store(
          output_sharding_var, i, cntr->New_ldc(bias_blocks_const[i], spos),
          spos);
      ctx.Prepend(bias_add_output_sharding);
    }

    NODE_PTR ld_result = cntr->New_ld(output_sharding_var, spos);

    return RETV(ld_result);
  }
};

//! @brief T2TSHARDING_CORE handler
class T2TSHARDING_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  T2TSHARDING_CORE_HANDLER() {}
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_CTX& ctx    = visitor->Context();
    CONTAINER*       cntr   = ctx.Container();
    GLOB_SCOPE*      gscope = cntr->Glob_scope();
    FUNC_SCOPE*      fscope = cntr->Parent_func_scope();
    
    ADDR_DATUM_PTR formal = node->Addr_datum();
    ADDR_DATUM_ID  new_fid = ctx.Get_addr_datum_map(formal->Id());
    ADDR_DATUM_PTR new_formal =  fscope->Addr_datum(new_fid);
    NODE_PTR new_idname = cntr->New_idname(new_formal, node->Spos());
    return RETV(new_idname);
  }

};  // COPY_PROP_CORE_HANDLER


}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_HANDLER_H
