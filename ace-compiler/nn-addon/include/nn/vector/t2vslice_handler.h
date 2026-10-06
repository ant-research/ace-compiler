//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_T2VSLICE_HANDLER_H
#define NN_VECTOR_T2VSLICE_HANDLER_H

#include "air/base/transform_util.h"
#include "nn/core/default_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

class T2VSLICE_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  T2VSLICE_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    // decompose NN.conv to NN.conv, NN.add and NN.strided_slice,
    // and also generate NN.mul when fast_conv option is enabled.
    TENSOR2VECTOR_CTX& ctx    = visitor->Context();
    CONTAINER*         cntr   = ctx.Container();
    GLOB_SCOPE*        gscope = cntr->Glob_scope();
    SPOS               spos   = node->Spos();
    NODE_PTR new_node = visitor->template Handle_node<RETV>(node).Node();
    // Get original conv2d input shape. Assuming NCHW&padding now.
    NODE_PTR orig_input = new_node->Child(0);

    int64_t batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG(batch == 1, "Conv only supports batch=1");

    NODE_PTR weight_node = new_node->Child(1);
    int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
            kernel_width = 0;
    Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                   kernel_height, kernel_width);
    AIR_ASSERT_MSG(kernel_height == kernel_width,
                   "Conv only supports kernel_height = kernel_width");

    std::vector<int64_t> rtype_shape =
        orig_input->Rtype()->Cast_to_arr()->Shape();
    // channel_out
    rtype_shape[1] = channel_out;

    TYPE_PTR conv_rtype = New_array_type(
        gscope, "conv", orig_input->Rtype()->Cast_to_arr()->Elem_type(),
        rtype_shape, spos);
    new_node->Set_rtype(conv_rtype);

    std::vector<int> pads    = Get_attr_data<int>(new_node, core::ATTR::PAD);
    std::vector<int> strides = Get_attr_data<int>(new_node, core::ATTR::STRIDE);
    AIR_ASSERT_MSG(pads.size() == 4, "conv pad size only support 4");
    AIR_ASSERT_MSG(strides.size() == 2, "conv stride size only support 2");

    // When strided_slice needs to be inserted:
    // kernel_height should be equal to kernel_width
    // 1. stride > 1
    //   A. padding value=0, kernel_height > 1 (both keep shape and stride
    //   introduce gaps) B. padding value=0, kernel_height = 1         (only
    //   stride introduce gaps) C. padding value>0, ASSERT(kernel_height > 1)
    //   (only stride introduce gaps)
    // 2. stride = 1
    //   A. padding value=0, kernel_height > 1 (only keep shape introduce
    //   gaps) B. padding value=0, kernel_height = 1         (no gaps) no need
    //   strided_slice C. padding value>0, ASSERT(kernel_height > 1) (no gaps)
    //   no need strided_slice

    if (pads[0] > 0) {
      AIR_ASSERT_MSG(kernel_height > 1, "no supported this conv yet");
    }
    bool need_insert_ss = false;
    if ((strides[0] > 1) || ((pads[0] == 0) && (kernel_height > 1))) {
      need_insert_ss = true;
    }

    if (need_insert_ss) {
      int64_t start_index0 = 0;
      int64_t start_index1 = 0;
      int64_t slicesize0   = input_height;
      int64_t slicesize1   = input_width;

      if ((pads[0] == 0) && (kernel_height > 1)) {
        int64_t padsize = (kernel_height - 1) / 2;
        start_index0    = padsize;
        start_index1    = padsize;
        slicesize0      = input_height - 2 * padsize;
        slicesize1      = input_width - 2 * padsize;
      }

      std::vector<int64_t> start_indiex{start_index0, start_index1};
      std::vector<int64_t> slice_size{slicesize0, slicesize1};
      std::vector<int64_t> stride_size{strides[0], strides[1]};

      std::vector<int> work_stride = {1, 1};
      new_node->Set_attr(core::ATTR::STRIDE, work_stride.data(),
                         work_stride.size());
      new_node->Set_attr(core::ATTR::ORIG_STRIDE, strides.data(),
                         strides.size());
      // seperate strided_slice with conv op, to simplify later optimization.
      NODE_PTR ld_gap_result = ctx.Store_temp_result_to_preg(new_node);

      VECTOR_GEN vgen(cntr);
      NODE_PTR   strided_slice_node =
          vgen.New_strided_slice(ld_gap_result, start_indiex, slice_size,
                                 stride_size, channel_out, spos);
      ctx.Register_ss_node(strided_slice_node->Id());
      return RETV(strided_slice_node);
    }
    return RETV(new_node);  // no need to insert strided_slice
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_average_pool<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = ctx.Container();
    SPOS               spos = node->Spos();
    VECTOR_GEN         vgen(cntr);

    NODE_PTR new_node = visitor->template Handle_node<RETV>(node).Node();
    new_node->Set_rtype(new_node->Child(0)->Rtype());
    // Get original conv2d input shape. Assuming NCHW&padding now.
    NODE_PTR orig_input = new_node->Child(0);
    int64_t  batch = 0, channel_in = 0, ih = 0, iw = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, ih, iw);
    AIR_ASSERT_MSG(batch == 1, "average_pool only supports batch=1");

    std::vector<int> strides = Get_attr_data<int>(new_node, core::ATTR::STRIDE);
    std::vector<int> ks      = Get_attr_data<int>(new_node, core::ATTR::KSHAPE);
    std::vector<int> pads    = Get_attr_data<int>(new_node, core::ATTR::PAD);

    if ((strides.size() > 0) && (strides[0] == 2)) {
      std::vector<int64_t> start_indiex{0, 0};
      std::vector<int64_t> slice_size{ih, iw};
      std::vector<int64_t> stride_size{strides[0], strides[1]};

      if ((ks[0] == 3) && (pads[0] > 0)) {
        // use group conv to compute 3x3 pooling
        new_node = vgen.New_group_conv(new_node);
      }
      // seperate strided_slice with avg pool op, to simplify later
      // optimizations.
      NODE_PTR ld_ap_result = ctx.Store_temp_result_to_preg(new_node);

      NODE_PTR strided_slice_node =
          vgen.New_strided_slice(ld_ap_result, start_indiex, slice_size,
                                 stride_size, channel_in, spos);
      ctx.Register_ss_node(strided_slice_node->Id());
      return RETV(strided_slice_node);
    } else {
      return RETV(new_node);  // no need to insert strided_slice
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = ctx.Container();
    SPOS               spos = node->Spos();
    NODE_PTR new_node       = visitor->template Handle_node<RETV>(node).Node();
    new_node->Set_rtype(new_node->Child(0)->Rtype());
    // Get original conv2d input shape. Assuming NCHW&padding now.
    NODE_PTR orig_input = new_node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG((batch == 1) && (input_height == input_width),
                   "global_average_pool only supports batch=1");

    std::vector<int64_t> start_indiex{0, 0};
    std::vector<int64_t> slice_size{input_height, input_width};
    std::vector<int64_t> stride_size{input_height, input_width};
    VECTOR_GEN           vgen(cntr);

    // seperate strided_slice with global avg pool op, to simplify later
    // optimizations.
    NODE_PTR ld_gap_result = ctx.Store_temp_result_to_preg(new_node);

    NODE_PTR strided_slice_node = vgen.New_strided_slice(
        ld_gap_result, start_indiex, slice_size, stride_size, channel_in, spos);
    ctx.Register_ss_node(strided_slice_node->Id());
    return RETV(strided_slice_node);
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_HANDLER_H
