//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_TENSOR2VECTOR_HANDLER_H
#define NN_VECTOR_TENSOR2VECTOR_HANDLER_H

#include "air/base/transform_util.h"
#include "nn/core/attr.h"
#include "nn/core/null_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/tensor2vector_util.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

class TENSOR2VECTOR_HANDLER : public nn::core::NULL_HANDLER {
public:
  TENSOR2VECTOR_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::add", false);
    TENSOR2VECTOR_UTIL vgen(ctx);
    NODE_PTR           new_ld0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR           new_ld1 = visitor->template Visit<RETV>(node->Child(1));
    NODE_PTR           new_add = vgen.New_add(new_ld0, new_ld1, node->Spos());
    return new_add;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::mul", false);
    TENSOR2VECTOR_UTIL vgen(ctx);

    NODE_PTR new_mul;
    if (ctx.Mask_fuse() && Has_attr<uint32_t>(node, core::ATTR::MASK)) {
      std::vector<uint32_t> vdn =
          Get_attr_data<uint32_t>(node, core::ATTR::MASK);
      AIR_ASSERT(vdn.size() == 1 && vdn[0] > 0);
      NODE_PTR opnd0 = node->Child(0);
      NODE_PTR opnd1 = node->Child(1);
      AIR_ASSERT(
          (opnd0->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RELU)) ||
          ((opnd0->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MUL)) &&
           opnd0->Child(1)->Opcode() == air::core::OPC_LDC));
      AIR_ASSERT(opnd1->Opcode() == air::core::ONE);

      if (opnd0->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MUL)) {
        // get float type constant value
        NODE_PTR scalar_operand = opnd0->Child(1);
        float    scalar_val     = vgen.Get_scalar<float>(scalar_operand);

        // opnd0->Child(0) is the variable
        TYPE_PTR etype = opnd0->Child(0)->Rtype()->Cast_to_arr()->Elem_type();
        NODE_PTR masked_node =
            vgen.Gen_mask_node(vdn[0], etype, scalar_val, node->Spos());

        NODE_PTR new_opnd0 = visitor->template Visit<RETV>(opnd0->Child(0));
        new_mul            = vgen.New_mul(new_opnd0, masked_node, node->Spos());
      } else if (opnd0->Opcode() ==
                 OPCODE(nn::core::NN, nn::core::OPCODE::RELU)) {
        NODE_PTR new_ld0 = visitor->template Visit<RETV>(node->Child(0));
        NODE_PTR new_ld1 = visitor->template Visit<RETV>(node->Child(1));
        new_mul          = vgen.New_mul(new_ld0, new_ld1, node->Spos());
        new_mul->Set_attr(core::ATTR::MASK, vdn.data(), vdn.size());
      } else {
        AIR_ASSERT_MSG(false, "should not be here");
      }
      return new_mul;
    }
    NODE_PTR new_ld0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR new_ld1 = visitor->template Visit<RETV>(node->Child(1));
    new_mul          = vgen.New_mul(new_ld0, new_ld1, node->Spos());

    return new_mul;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_relu(VISITOR* visitor, air::base::NODE_PTR node) {
    NODE_PTR new_ld = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR op     = visitor->Context().Container()->Clone_node(node);
    op->Set_child(0, new_ld->Id());
    return op;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_flatten(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::flatten", false);
    CONTAINER*         cntr = ctx.Container();
    TENSOR2VECTOR_UTIL vgen(ctx);
    GLOB_SCOPE*        gscope     = cntr->Glob_scope();
    SPOS               spos       = node->Spos();
    NODE_PTR           orig_input = node->Child(0);
    NODE_PTR           new_op0    = visitor->template Visit<RETV>(orig_input);
    // TODO: handle flatten axis. Now flatten to [1,x].
    // Only set load. Other op rtype is conistent.
    std::vector<int64_t> shape = orig_input->Rtype()->Cast_to_arr()->Shape();
    if (shape.size() > 1) {
      int64_t size = 1;
      for (auto s : shape) size *= s;
      ctx.Trace(TF_LOWER, "WARN: flatten new_input ", shape.size(),
                " is not 1D! \n");
      ctx.Trace_cmd(TF_LOWER, Trace_node, new_op0);
      std::vector<int64_t> shape1d{size};
      NODE_PTR new_op0_reshape = vgen.New_reshape(new_op0, shape1d, spos);
      return new_op0_reshape;
    }
    return new_op0;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_reshape(VISITOR* visitor, air::base::NODE_PTR node) {
    // TODO: use handle_flatten now, will improve later!
    return Handle_flatten<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::conv", false);
    CONTAINER*         cntr = ctx.Container();
    TENSOR2VECTOR_UTIL vgen(ctx);
    GLOB_SCOPE*        gscope   = cntr->Glob_scope();
    SPOS               spos     = node->Spos();
    CONST_TYPE_PTR     s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
    // Get original conv2d input shape. Assuming NCHW&padding now.
    NODE_PTR orig_input = node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, input_height,
                   input_width);

    int64_t output_height = input_height;
    int64_t output_width  = input_width;

    ctx.Trace(TF_LOWER, "conv orig_input shape: [", batch, ", ", channel_in,
              ", ", input_height, ", ", input_width, "]\n");
    AIR_ASSERT_MSG(batch == 1, "Conv only supports batch=1");

    NODE_PTR weight_node = node->Child(1);
    int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
            kernel_width = 0;
    Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                   kernel_height, kernel_width);
    std::vector<int> groups = Get_attr_data<int>(node, core::ATTR::GROUP);
    int              group  = groups[0];
    // Only support depthwise convolution for group>1
    if (group > 1) {
      AIR_ASSERT_MSG((channel_in == group) && ctx.Conv_fast(),
                     "depthwise convolution: channel_in == group && Conv_fast");
    } else {
      AIR_ASSERT_MSG(channel_in == channel_in_kernel,
                     "channel_in == channel_in_kernel");
    }
    ctx.Trace(TF_LOWER, "conv kernel shape: [", channel_out, ", ",
              channel_in_kernel, ", ", kernel_height, ", ", kernel_width,
              "], group=", group, "\n");

    NODE_PTR new_input   = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR new_input1d = new_input;
    AIR_ASSERT_MSG(new_input->Rtype()->Is_array(),
                   "conv new_input is not an array type");
    if (new_input->Rtype()->Cast_to_arr()->Shape().size() > 1) {
      ctx.Trace(TF_LOWER, "conv new_input is not 1D! \n");
      ctx.Trace_cmd(TF_LOWER, Trace_node, new_input);
      //  Insert reshape input to 1D
      std::vector<int64_t> input1d_shape(
          1, channel_in * input_height * input_height);
      new_input1d = vgen.New_reshape(new_input, input1d_shape, spos);
    }

    if (new_input1d->Opcode() != air::core::LD &&
        new_input1d->Opcode() != air::core::LDP) {
      // avoid duplicate computation
      new_input1d = ctx.Store_temp_result_to_preg(new_input1d);
    }

    // transpose_im2col weight
    ctx.Trace_cmd(TF_LOWER, Trace_float_array, weight_node->Const(),
                  "conv_weight");
    const float* cptr = weight_node->Const()->Array_ptr<float>();
    FPVEC weight(cptr, cptr + channel_out * channel_in_kernel * kernel_height *
                                  kernel_width);

    int stride      = 1;  // by default, conv works at stride=1
    int fhe_padsize = 0;
    if (ctx.Selective_ss()) {
      std::vector<int> tmp_real_strides =
          Get_attr_data<int>(node, core::ATTR::STRIDE);
      stride = tmp_real_strides[0];

      if (Has_attr<int>(node, core::ATTR::KEEP_SHAPE_PAD)) {
        std::vector<int> fhe_pads =
            Get_attr_data<int>(node, core::ATTR::KEEP_SHAPE_PAD);
        AIR_ASSERT_MSG(fhe_pads.size() > 0, "fhe padsize must exist");
        fhe_padsize = fhe_pads[0];
      }
    }

    int64_t kernel_size = kernel_height * kernel_width;
    // Handle case where channel_out%channel_in != 0
    // weight cannot mul all input channel. So expand input and weight.
    if ((channel_out >= channel_in) && (channel_out % channel_in != 0)) {
      ctx.Trace(TF_LOWER, "channel_out%channel_in != 0 -> padding channel_in=",
                channel_in, "\n");
      int channel_in_new = channel_in;
      while (channel_out % channel_in_new != 0) channel_in_new++;
      ctx.Trace(TF_LOWER, "padding channel_in_new=", channel_in_new, "\n");
      FPVEC weight_pad(channel_out * channel_in_new * kernel_size, 0);
      for (int i = 0; i < channel_out; i++)
        for (int j = 0; j < channel_in; j++)
          for (int k = 0; k < kernel_size; k++)
            weight_pad[i * channel_in_new * kernel_size + j * kernel_size + k] =
                weight[i * channel_in * kernel_size + j * kernel_size + k];
      channel_in        = channel_in_new;
      channel_in_kernel = channel_in_new;
      weight            = std::move(weight_pad);
      ctx.Trace(TF_LOWER, "padding weight.size()=", weight.size(), "\n");
    }

    FPMAT conv1_im2col_kernel(
        channel_in_kernel * kernel_height * kernel_width,
        FPVEC(channel_out * input_height * input_width, 0.0));
    std::vector<int> ra(kernel_height * kernel_width, 0);
    Get_im2col_kernel(weight, channel_in_kernel, input_height, input_width,
                      channel_out, kernel_height, kernel_width, 1, stride, ra,
                      conv1_im2col_kernel);

    std::vector<int> orig_strides = {stride, stride};
    if (Has_attr<int>(node, core::ATTR::ORIG_STRIDE)) {
      orig_strides = Get_attr_data<int>(node, core::ATTR::ORIG_STRIDE);
    }
    AIR_ASSERT(orig_strides.size() == 2);
    AIR_ASSERT(orig_strides[0] == orig_strides[1]);
    std::vector<int> real_pads = Get_attr_data<int>(node, core::ATTR::PAD);
    AIR_ASSERT_MSG(real_pads.size() == 4, "conv padding size only support 4");
    ctx.Trace(TF_LOWER, "conv stride is ", orig_strides[0], "\n");
    ctx.Trace(TF_LOWER, "conv padding is ", real_pads[0], "\n");
    bool mask_stride_data = false;
    if (((orig_strides[0] > 1) && (real_pads[0] != 0)) ||
        ((orig_strides[0] > 1) && (real_pads[0] == 0) && (kernel_width == 1))) {
      mask_stride_data = true;
    }
    if (mask_stride_data) {
      Masking_padding_stride_data_in_mat(
          channel_in_kernel * kernel_height * kernel_width, input_height,
          input_width, channel_out, real_pads[0], orig_strides[0],
          kernel_height, conv1_im2col_kernel);
    } else if ((real_pads[0] == 0) && (kernel_width != 1)) {
      Masking_no_padding_stride_data_in_mat(
          channel_in_kernel * kernel_height * kernel_width, input_height,
          input_width, kernel_height, kernel_width, channel_out, real_pads[0],
          orig_strides[0], conv1_im2col_kernel, fhe_padsize);
    }

    // Here conv is transformed into:
    // conv1 = input[input_size] "X" conv1_im2col_kernel[channel_in*kernel_size,
    // output_size] with "ra" rotation alignment.

    bool    flag_conv_fast = (ctx.Conv_fast() && (channel_out >= channel_in));
    int64_t output_size    = channel_out * input_height * input_width;
    int64_t input_size     = channel_in * input_height * input_width;

    ctx.Trace(TF_LOWER, "Handle_conv: flag_conv_fast=", flag_conv_fast,
              ", Conv_parallel=", ctx.Conv_parallel(), "\n");
    // Data blocks in a vector for parallelization.
    int num_block = 1;
    if (flag_conv_fast && ctx.Conv_parallel())
      num_block = Get_costmodel_fusion(channel_in, channel_out, kernel_size,
                                       output_size, group, ctx.Get_slot());

    int width_block_pad = (num_block == 1) ? 0 : input_size / num_block;
    // Here channel_in_kernel considers depwise.
    int num_grid  = channel_in_kernel;
    int cap_block = kernel_size;

    int   position_block = output_height * output_width;
    int   num_dup_input  = channel_out / channel_in;
    FPVEC block_pad(width_block_pad, 0.0);

    vgen.Trace_conv_params(num_grid, num_block, output_size, width_block_pad,
                           cap_block, num_dup_input, "initial");

    // 1) Improve cap_block for less rotations and KSKs.
    // 2) block-level parallization in a grid.

    // Hold weight data by transposing
    FPVEC weight_im2col_vec;
    if (flag_conv_fast) {
      if (kernel_size == 1) {
        // Only cap_block. TODO: 1x1 for block-level parallization.
        cap_block = Get_costmodel_mini_factor1(channel_in_kernel);
        for (int i = 0; i < channel_in_kernel; i++) {
          // Form block for metakernel:
          rotate(conv1_im2col_kernel[i].begin(),
                 conv1_im2col_kernel[i].begin() +
                     conv1_im2col_kernel[i].size() -
                     cap_block * (i / cap_block) * input_height * input_width,
                 conv1_im2col_kernel[i].end());
          weight_im2col_vec.insert(weight_im2col_vec.end(),
                                   conv1_im2col_kernel[i].begin(),
                                   conv1_im2col_kernel[i].end());
        }
        // Adjust rotation aligment.
        for (int i = 1; i < cap_block; i++)
          ra.push_back(i * output_height * output_width);
        // Adjust input: as rotation > channel_size.
        if (cap_block > 1) num_dup_input += 1;
        num_grid /= cap_block;
        position_block *= cap_block;
        vgen.Trace_conv_params(num_grid, num_block, output_size,
                               width_block_pad, cap_block, num_dup_input,
                               "block: 1x1");
      } else {
        // Improve block-level parallization for less rotations and KSKs.
        // Rearrange conv1_im2col_kernel in a "grid" level.
        // conv1_im2col_kernel[channel_in*kernel_size, output_size] ->
        // conv1_im2col_kernel[channel_in*kernel_size/num_block,
        // (output_size+width_block_pad)*num_block]
        int offset = channel_in * kernel_size / num_block;
        for (int i = 0; i < channel_in_kernel / num_block; i++) {
          for (int j = 0; j < kernel_size; j++) {
            // Arrange num_block weights in a grid, ADD block_pad for resource
            // isolation.
            for (int b = 0; b < num_block; b++) {
              int index = i * kernel_size + j + b * offset;
              rotate(conv1_im2col_kernel[index].begin(),
                     conv1_im2col_kernel[index].begin() +
                         conv1_im2col_kernel[index].size() -
                         i * input_height * input_width,
                     conv1_im2col_kernel[index].end());
              weight_im2col_vec.insert(weight_im2col_vec.end(),
                                       conv1_im2col_kernel[index].begin(),
                                       conv1_im2col_kernel[index].end());
              if (num_block > 1)
                weight_im2col_vec.insert(weight_im2col_vec.end(),
                                         block_pad.begin(), block_pad.end());
            }
          }
        }
        if (num_block > 1) {
          num_dup_input *= (num_block + 1);
          num_grid /= num_block;
        }
        vgen.Trace_conv_params(num_grid, num_block, output_size,
                               width_block_pad, cap_block, num_dup_input,
                               "block: non-1x1");
      }
    } else {
      for (int i = 0; i < channel_in * kernel_height * kernel_width; i++)
        weight_im2col_vec.insert(weight_im2col_vec.end(),
                                 conv1_im2col_kernel[i].begin(),
                                 conv1_im2col_kernel[i].end());
    }

    // New weight_im2col_const
    int64_t weight_im2col_size = channel_in_kernel * kernel_size * output_size;
    if (num_block > 1)
      weight_im2col_size =
          channel_in_kernel * kernel_size * (output_size + width_block_pad);

    AIR_ASSERT_MSG(weight_im2col_vec.size() == weight_im2col_size,
                   "verify weight_im2col_size");
    std::vector<int64_t> weight_im2col_shape{
        channel_in_kernel * kernel_size / num_block,
        (num_block > 1) ? num_block * (output_size + width_block_pad)
                        : output_size};

    std::string weight_im2col_str =
        New_array_name("weight_im2col_float", weight_im2col_shape);
    CONSTANT_PTR weight_im2col_const = New_array_const(
        gscope, weight_im2col_str.c_str(), weight_im2col_size,
        weight_node->Rtype()->Cast_to_arr()->Elem_type(), weight_im2col_shape,
        (void*)weight_im2col_vec.data(), spos);
    NODE_PTR new_weight = cntr->New_ldc(weight_im2col_const, spos);

    // Expand bias const: TODO: add has broadcast, to sihe?
    NODE_PTR     bias_node = node->Child(2);
    const float* bias_ptr  = bias_node->Const()->Array_ptr<float>();
    FPVEC        bias_expand(channel_out * output_height * output_width);
    for (int i = 0; i < channel_out; i++) {
      for (int j = 0; j < output_height * output_width; j++) {
        bias_expand[i * output_height * output_width + j] = bias_ptr[i];
      }
    }

    if (mask_stride_data) {
      Masking_stride_data_in_vec(output_height, output_width, channel_out,
                                 real_pads[0], orig_strides[0], kernel_height,
                                 bias_expand);
    } else if (real_pads[0] == 0) {
      Masking_no_padding_stride_data_in_vec(
          output_height, output_width, channel_out, real_pads[0],
          orig_strides[0], kernel_height, kernel_width, bias_expand,
          fhe_padsize);
    }

    int64_t              bias_expand_size = output_size;
    std::vector<int64_t> bias_expand_shape{bias_expand_size};
    CONSTANT_PTR         bias_expand_const =
        New_array_const(gscope, "bias_expand", bias_expand_size,
                        bias_node->Rtype()->Cast_to_arr()->Elem_type(),
                        bias_expand_shape, (void*)bias_expand.data(), spos);
    NODE_PTR new_bias = cntr->New_ldc(bias_expand_const, spos);

    // Here all parameters for parallization are ready.
    // We can estimate the total number of rotations and KSKs here.
    SHARD_MAP_PARAMS params;
    params._num_grid  = num_grid;
    params._num_block = num_block;

    params._width_block      = output_size + width_block_pad;
    params._width_block_data = output_size;
    params._width_block_pad  = width_block_pad;
    params._position_block   = position_block;
    params._cap_block        = cap_block;
    params._num_dup_input    = num_dup_input;

    bool need_mask = true;
    if (ctx.Mask_fuse()) {
      if (Has_attr<uint32_t>(node, core::ATTR::MASK)) {
        std::vector<uint32_t> vdn =
            Get_attr_data<uint32_t>(node, core::ATTR::MASK);
        AIR_ASSERT(vdn.size() == 1 && vdn[0] > 0);
        need_mask = true;
      } else {
        need_mask = false;
      }
    }

    NODE_PTR new_node;
    if (flag_conv_fast)
      new_node = vgen.New_conv_metakernel_fast(
          new_input1d, new_weight, new_bias, ra, channel_in, channel_out,
          output_height, output_width, kernel_height * kernel_width, group,
          stride, &params, need_mask, spos);
    else
      new_node = vgen.New_conv_metakernel(
          new_input1d, new_weight, new_bias, ra, channel_in, channel_out,
          output_height, output_width, kernel_height * kernel_width, stride,
          spos);
    return new_node;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_gemm(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::gemm", false);
    CONTAINER*         cntr = ctx.Container();
    TENSOR2VECTOR_UTIL vgen(ctx);
    // TODO: pad for shape consistence.

    SPOS     spos    = node->Spos();
    NODE_PTR new_ld0 = visitor->template Visit<RETV>(node->Child(0));

    if (new_ld0->Opcode() != air::core::LD &&
        new_ld0->Opcode() != air::core::LDP) {
      // avoid duplicate computation
      new_ld0 = ctx.Store_temp_result_to_preg(new_ld0);
    }

    CONSTANT_PTR op1_const = node->Child(1)->Const();
    AIR_ASSERT_MSG(node->Child(1)->Rtype()->Is_array(),
                   "operand1 is not an array type");
    ARRAY_TYPE_PTR op1_ty_arr = node->Child(1)->Rtype()->Cast_to_arr();

    std::vector<int64_t> op1_shape = Get_array_dim_ubs(op1_ty_arr);
    AIR_ASSERT_MSG(op1_shape.size() == 2, "operand1_const dim %d != 2",
                   op1_shape.size());

    int          height = op1_shape[0];
    int          width  = op1_shape[1];
    const float* cptr   = op1_const->Array_ptr<float>();

    // Read the op1_const
    ctx.Trace_cmd(TF_LOWER, Trace_float_array, op1_const, "gemm weight");

    FPMAT op1_mat(height);
    for (int i = 0; i < height; i++) {
      op1_mat[i].resize(width);
      for (int j = 0; j < width; j++) {
        op1_mat[i][j] = cptr[i * width + j];
      }
    }

    // when width is equal to max available slots, try to expand height to
    // make width multiple of height otherwise if max available slots is
    // enough, expand width to make width multiple of height.
    if (width == ctx.Get_slot() || width == ctx.Get_slot() / 2) {
      int padh;
      for (padh = height; padh <= height * width; padh++) {
        if (width % padh == 0) break;
      }

      op1_mat.resize(padh);

      for (int i = 0; i < padh - height; i++) {
        FPVEC current_row(width, 0);
        op1_mat[height + i] = current_row;
      }

      height = padh;
    }

    // Assuming GEMM:: transB = 1
    // transpose_diag(op1_const)
    FPVEC diag_vec;
    int   padw;
    for (padw = width; padw <= height * width; padw++) {
      if (padw % height == 0) break;
    }

    bool flag_gemm_fast = ctx.Gemm_fast() && Is_power_of_two(height);

    int64_t old_height = height;
    if (flag_gemm_fast) {
      AIR_ASSERT_MSG(((height & (height - 1)) == 0) &&
                         (((height / width) & (height / width - 1)) == 0),
                     "TODO: gemm_fast only supports height=%d=2^n now. "
                     "Example: 8x4, 2048x1024",
                     height);
      if (old_height > width) {
        padw   = height;
        height = width;
      } else {
        padw = width;
      }
    }

    int h1 = 0, h2 = 0;
    Get_block_size(height, h1, h2);
    ctx.Trace(TF_LOWER, "Handle_gemm Get_block_size: height=", height,
              ", h1=", h1, ", h2=", h2, "\n");

    diag_vec.reserve(static_cast<size_t>(padw) * height);

    if (flag_gemm_fast) {
      // weight tiling
      for (int i = 0; i < height; i++) {
        FPVEC diag;
        if (old_height > width) {
          FPMAT slice_weight0(op1_mat.begin(), op1_mat.begin() + height);
          diag = Transpose_diagonal(slice_weight0, i, height);
          rotate(diag.begin(), diag.begin() + diag.size() - (i / h1) * h1,
                 diag.end());
          for (size_t slice = 1; slice < old_height / height; slice++) {
            FPMAT slice_weight(op1_mat.begin() + height * slice,
                               op1_mat.begin() + height * (slice + 1));
            FPVEC diag2 = Transpose_diagonal(slice_weight, i, height);
            rotate(diag2.begin(), diag2.begin() + diag2.size() - (i / h1) * h1,
                   diag2.end());
            diag.insert(diag.end(), diag2.begin(), diag2.end());
          }
        } else {
          diag = Transpose_diagonal(op1_mat, i, padw);
          rotate(diag.begin(), diag.begin() + diag.size() - (i / h1) * h1,
                 diag.end());
        }
        diag_vec.insert(diag_vec.end(), diag.begin(), diag.end());
      }
    } else {
      for (int i = 0; i < height; i++) {
        FPVEC diag = Transpose_diagonal(op1_mat, i, padw);
        diag_vec.insert(diag_vec.end(), diag.begin(), diag.end());
      }
    }
    AIR_ASSERT_MSG(diag_vec.size() == padw * height,
                   "operand1_diag size != height*padw  %d != %d*%d",
                   diag_vec.size(), height, padw);

    // new 2d diag_array const: height*padw
    GLOB_SCOPE*          gscope = cntr->Glob_scope();
    std::vector<int64_t> weight_diag_shape{height, padw};
    CONSTANT_PTR         diag_const = New_array_const(
        gscope, "weight_diag", height * padw, op1_ty_arr->Elem_type(),
        weight_diag_shape, (void*)diag_vec.data(), spos);
    // new_ld1 is LD operand_diag
    NODE_PTR new_weight = cntr->New_ldc(diag_const, spos);
    NODE_PTR new_bias   = visitor->template Visit<RETV>(node->Child(2));

    bool need_mask = true;
    if (ctx.Mask_fuse()) {
      if (Has_attr<uint32_t>(node, core::ATTR::MASK)) {
        std::vector<uint32_t> vdn =
            Get_attr_data<uint32_t>(node, core::ATTR::MASK);
        AIR_ASSERT(vdn.size() == 1 && vdn[0] > 0);
        need_mask = true;
      } else {
        need_mask = false;
      }
    }

    NODE_PTR new_node;
    if (flag_gemm_fast) {
      new_node = vgen.New_gemm_metakernel_fast(new_ld0, new_weight, new_bias,
                                               spos, old_height > width);
    } else
      new_node = vgen.New_gemm_metakernel(new_ld0, new_weight, new_bias,
                                          need_mask, spos);
    return new_node;
  }

  //! @deprecated: will delete later
  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::avg_pool", false);
    CONTAINER*         cntr = ctx.Container();
    TENSOR2VECTOR_UTIL vgen(ctx);
    // TODO: pad for shape consistence.
    AIR_ASSERT_MSG(node->Num_child() == 1,
                   "average pool operator only support 1 child");
    NODE_PTR input = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR new_input =
        ctx.Store_and_load_new_preg(input);  // avoid modifying input directly
    SPOS spos = node->Spos();

    AIR_ASSERT_MSG(new_input->Rtype()->Is_array(),
                   "operand is not an array type");
    ARRAY_TYPE_PTR       op_ty_arr = new_input->Rtype()->Cast_to_arr();
    std::vector<int64_t> op_shape  = op_ty_arr->Shape();
    AIR_ASSERT_MSG(op_shape.size() == 4, "input shape should be 4");
    // std::cout << ">> input shape size: " << op_shape.size() << std::endl;
    // for (auto dim : op_shape) {
    //   std::cout << dim << std::endl;
    // }
    std::vector<int> kernel_shape =
        Get_attr_data<int>(node, core::ATTR::KSHAPE);
    std::vector<int> strides = Get_attr_data<int>(node, core::ATTR::STRIDE);

    int64_t stride = strides[0];
    int64_t ks     = kernel_shape[0];
    int64_t c_in   = op_shape[1];
    int64_t h      = op_shape[2];
    int64_t w      = op_shape[3];

    AIR_ASSERT_MSG(stride == 2 || stride == 4,
                   "average pool only support stride and ks equals 2 or 4!");

    GLOB_SCOPE* gscope = cntr->Glob_scope();
    FUNC_SCOPE* fscope = cntr->Parent_func_scope();

    std::string add_row_str =
        std::string("tmp_row_add") + std::to_string(ctx.Get_num_vloop());
    ADDR_DATUM_PTR add_row_var =
        fscope->New_var(op_ty_arr, add_row_str.c_str(), spos);

    CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

    NODE_PTR extra_roll_node = new_input;

    if (ctx.Selective_ss()) {
      if (Has_attr<int>(node, core::ATTR::PROLL)) {
        std::vector<int> roll_num = Get_attr_data<int>(node, core::ATTR::PROLL);
        AIR_ASSERT_MSG(stride == 4,
                       "enable new selective_ss option, pre_roll attr has "
                       "value, stride should equal 4");
        AIR_ASSERT_MSG(ks == 4,
                       "enable new selective_ss option, pre_roll attr has "
                       "value, ks should equal 4");
        extra_roll_node = vgen.New_roll(
            new_input, cntr->New_intconst(s32_type, roll_num[0], spos),
            roll_num, spos);
      }
    }

    // calculate sum of element in kernel shape
    std::vector<int> roll_num1{(int)ks / 2 * (int)w};
    NODE_PTR         tmp_roll_node1 = vgen.New_roll(
        extra_roll_node, cntr->New_intconst(s32_type, ks / 2 * w, spos),
        roll_num1, spos);

    NODE_PTR add_row_node = vgen.New_add(extra_roll_node, tmp_roll_node1, spos);
    STMT_PTR add_row_stmt = cntr->New_st(add_row_node, add_row_var, spos);

    ctx.Prepend(add_row_stmt);
    NODE_PTR ld_row_add = cntr->New_ld(add_row_var, spos);

    std::vector<int> roll_num2{(int)ks / 2};
    NODE_PTR         tmp_roll_node2 =
        vgen.New_roll(ld_row_add, cntr->New_intconst(s32_type, ks / 2, spos),
                      roll_num2, spos);
    NODE_PTR add_col_node = vgen.New_add(ld_row_add, tmp_roll_node2, spos);

    // masking [1,0,1,0..., 0,0,0,0..., 1,0,1,0..., 0,0,0,0...]
    FPVEC avg_value_mask = Get_avg_value_mask(c_in, h, w, ks);

    std::vector<int64_t> avg_mask_shape{c_in * h * w};
    std::string          mask_name =
        std::string("avg_value_mask") + std::to_string(ctx.Get_num_vloop());
    CONSTANT_PTR mask_const =
        New_array_const(gscope, mask_name, c_in * h * w, op_ty_arr->Elem_type(),
                        avg_mask_shape, (void*)avg_value_mask.data(), spos);
    // new_ldc is LD avg value mask
    NODE_PTR avg_value_mask_node = cntr->New_ldc(mask_const, spos);

    ctx.Trace_cmd(TF_LOWER, Trace_float_array, avg_value_mask_node->Const(),
                  "avg_pool_mask");

    // intermediate average pool result which not considerring stride
    NODE_PTR avgpool_inter_node =
        vgen.New_mul(add_col_node, avg_value_mask_node, spos);

    return avgpool_inter_node;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_average_pool<RETV, VISITOR>(visitor, node);
  }

  //! @deprecated: will delete later
  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    TIMING_UTIL timing(ctx, node->Spos(), "Tensor::global_avg_pool", false);
    CONTAINER*  cntr = ctx.Container();
    TENSOR2VECTOR_UTIL vgen(ctx);

    AIR_ASSERT_MSG(node->Num_child() == 1,
                   "global average pool operator only support 1 child");
    NODE_PTR input_node = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR new_input  = ctx.Store_and_load_new_preg(
        input_node);  // avoid modifying input directly
    SPOS spos = node->Spos();

    AIR_ASSERT_MSG(new_input->Rtype()->Is_array(),
                   "operand is not an array type");
    ARRAY_TYPE_PTR       op_ty_arr = new_input->Rtype()->Cast_to_arr();
    std::vector<int64_t> op_shape  = op_ty_arr->Shape();
    AIR_ASSERT_MSG(op_shape.size() == 4, "input shape should be 4");

    int64_t c_in = op_shape[1];
    int64_t h    = op_shape[2];
    int64_t w    = op_shape[3];

    int loop_ub = log2(h * w);
    vgen.Gen_loop_roll_add_stmt("gap_sum_index", loop_ub, 1, new_input, spos);

    GLOB_SCOPE* gscope = cntr->Glob_scope();
    FUNC_SCOPE* fscope = cntr->Parent_func_scope();
    // 2. calculate global average value with gap
    // 2.1 generate average const
    int64_t vdn_in_ks = h * w;
    if (ctx.Selective_ss()) {
      if (Has_attr<int>(node, core::ATTR::VDN_IN_KS)) {
        std::vector<int> real_vdn_in_ks =
            Get_attr_data<int>(node, core::ATTR::VDN_IN_KS);
        AIR_ASSERT_MSG(real_vdn_in_ks.size() == 1,
                       "vdn_in_ks only contains one value");
        vdn_in_ks = real_vdn_in_ks[0];
      }
    }
    CONSTANT_PTR average_const =
        gscope->New_const(CONSTANT_KIND::FLOAT, op_ty_arr->Elem_type(),
                          (long double)1.0 / vdn_in_ks);
    NODE_PTR average_node = cntr->New_ldc(average_const, spos);

    // 2.2 caculate global average value with gap
    std::string result_str =
        std::string("gap_result") + std::to_string(ctx.Get_num_vloop());
    ADDR_DATUM_PTR gap_result_var =
        fscope->New_var(op_ty_arr, result_str.c_str(), spos);

    NODE_PTR ld_sum_node;
    if (new_input->Opcode() ==
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::LDP)) {
      ld_sum_node = cntr->New_ldp(new_input->Preg(), spos);
    } else {
      ld_sum_node = cntr->New_ld(new_input->Addr_datum(), spos);
    }

    NODE_PTR cz_result_node = vgen.New_mul(ld_sum_node, average_node, spos);
    STMT_PTR cz_result_stmt =
        cntr->New_st(cz_result_node, gap_result_var, spos);
    ctx.Prepend(cz_result_stmt);

    return cntr->New_ld(gap_result_var, spos);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_strided_slice(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = ctx.Container();
    TENSOR2VECTOR_UTIL vgen(ctx);
    AIR_ASSERT_MSG(node->Num_child() == 4,
                   "strided_slice operator only support 4 child");
    NODE_PTR input = visitor->template Visit<RETV>(node->Child(0));
    SPOS     spos  = node->Spos();

    AIR_ASSERT_MSG(input->Rtype()->Is_array(), "operand is not an array type");
    ARRAY_TYPE_PTR       op_ty_arr = input->Rtype()->Cast_to_arr();
    std::vector<int64_t> op_shape  = op_ty_arr->Shape();

    if ((node->Child(0)->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE)) ||
        (node->Child(0)->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN))) {
      // till now, reshape/flatten op make shape size to 1, so use input
      // original shape
      op_shape = node->Child(0)->Rtype()->Cast_to_arr()->Shape();
    }

    AIR_ASSERT_MSG(op_shape.size() == 4, "input shape should be 4");
    ctx.Trace(TF_LOWER, ">> input shape size: ", op_shape.size(), "\n");

    CONSTANT_PTR start_indice_const = node->Child(1)->Const();
    int64_t      start_row = 0, start_column = 0;
    Get_const_array_value(start_indice_const, start_row, start_column);
    AIR_ASSERT_MSG(start_row == start_column, "only support same start indice");

    CONSTANT_PTR slice_size_const = node->Child(2)->Const();
    int64_t      ss_height = 0, ss_width = 0;
    Get_const_array_value(slice_size_const, ss_height, ss_width);

    CONSTANT_PTR stride_size_const = node->Child(3)->Const();
    int64_t      stride_row = 0, stride_col = 0;
    Get_const_array_value(stride_size_const, stride_row, stride_col);
    AIR_ASSERT_MSG(stride_row == stride_col, "only support same stride size");

    std::vector<int> channel = Get_attr_data<int>(node, core::ATTR::CHANNEL);
    AIR_ASSERT_MSG(channel.size() == 1,
                   "strided slice only contains 1 channel attribute");

    // TODO: kernel shape, height, width should also get from attributes
    // or child node?
    int padsize      = start_row;
    int kernal_shape = stride_row;
    if (start_row != 0) {
      kernal_shape = 2 * padsize + 1;
    }
    int64_t actual_height = op_shape[2];
    int64_t actual_width  = op_shape[3];

    FUNC_SCOPE* fscope = cntr->Parent_func_scope();

    std::string orig_result_str =
        std::string("orig_result") + std::to_string(ctx.Get_num_vloop());
    ADDR_DATUM_PTR orig_result_var =
        fscope->New_var(op_ty_arr, orig_result_str.c_str(), spos);
    STMT_PTR st_stmt = cntr->New_st(input, orig_result_var, spos);
    ctx.Prepend(st_stmt);

    NODE_PTR ld_result = cntr->New_ld(orig_result_var, spos);

    NODE_PTR extraced_node = vgen.New_extract_valid_data(
        ld_result, channel[0], padsize, actual_height, actual_width, ss_height,
        ss_width, kernal_shape, stride_row, (start_row == 0), spos);

    return extraced_node;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_concat(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = ctx.Container();
    TIMING_UTIL        timing(ctx, node->Spos(), "Tensor::concat", false);
    TENSOR2VECTOR_UTIL vgen(ctx);
    AIR_ASSERT_MSG(node->Num_child() == 2,
                   "only support concat with two children");
    NODE_PTR input0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR input1 = visitor->template Visit<RETV>(node->Child(1));
    SPOS     spos   = node->Spos();

    AIR_ASSERT_MSG(input0->Rtype()->Is_array() && input1->Rtype()->Is_array(),
                   "concat operands must be array type");

    std::vector<int> axis_att_dim = Get_attr_data<int>(node, core::ATTR::AXIS);
    AIR_ASSERT_MSG(axis_att_dim.size() == 1 && axis_att_dim[0] == 1,
                   "only support concat whose axis attr value is 1");

    std::vector<int64_t> input0_dim = input0->Rtype()->Cast_to_arr()->Shape();
    std::vector<int64_t> input1_dim = input1->Rtype()->Cast_to_arr()->Shape();
    AIR_ASSERT_MSG(
        input0_dim.size() == input1_dim.size() && input0_dim.size() == 4,
        "input shape dimensions should be 4");
    int64_t input0_size =
        input0_dim[0] * input0_dim[1] * input0_dim[2] * input0_dim[3];
    int64_t input1_size =
        input1_dim[0] * input1_dim[1] * input1_dim[2] * input1_dim[3];
    AIR_ASSERT_MSG((input0_size == input1_size) &&
                       ((input0_size + input1_size) <= ctx.Get_slot()),
                   "output size %d of concat must < %d",
                   (input0_size + input1_size), ctx.Get_slot());

    int64_t c_out = input0_dim[0];
    int64_t c_in  = input0_dim[1];
    int64_t h     = input0_dim[2];
    int64_t w     = input0_dim[3];

    AIR_ASSERT_MSG(c_out == 1, "only support c_out=1 till now");

    // when axis=1, c_out=1.
    // input0 shape:1x64x32x32, input1 shape:1x64x32x32, output
    // shape:1x128x32x32

    // concat: right roll input1 using size of input0 as roll offset, then add
    // input0 with right rolled input1
    CONST_TYPE_PTR s32_type =
        cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
    std::vector<int> roll_num{(int)(-c_in * h * w)};
    NODE_PTR         right_roll_input1 =
        vgen.New_roll(input1, cntr->New_intconst(s32_type, -c_in * h * w, spos),
                      roll_num, spos);
    NODE_PTR concat_node = vgen.New_add(input0, right_roll_input1, spos);

    return concat_node;
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_HANDLER_H
