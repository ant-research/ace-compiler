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
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    if (ctx.Improve_ss_insert()) {
      ctx.Incr_num_op_ca_th();
    }
    CONTAINER*  cntr     = visitor->Context().Container();
    GLOB_SCOPE* gscope   = cntr->Glob_scope();
    SPOS        spos     = node->Spos();
    NODE_PTR    new_node = visitor->template Handle_node<RETV>(node).Node();
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
    std::vector<int64_t> rtype_shape =
        orig_input->Rtype()->Cast_to_arr()->Shape();
    // channel_out
    rtype_shape[1] = channel_out;

    TYPE_PTR conv_rtype = New_array_type(
        gscope, "conv", orig_input->Rtype()->Cast_to_arr()->Elem_type(),
        rtype_shape, spos);
    new_node->Set_rtype(conv_rtype);

    std::vector<int> pads    = Get_attr_int(new_node, "pads");
    std::vector<int> strides = Get_attr_int(new_node, "strides");
    AIR_ASSERT_MSG(strides.size() == 2, "conv stride size only support 2");
    // when nopadding or with padding but stride is not 1 need to insert
    // strided_slice
    if (!ctx.Improve_ss_insert()) {
      if ((pads.size() > 0) && (pads[0] == 0)) {
        int                  padsize   = (kernel_height - 1) / 2;
        int                  slicesize = input_height - 2 * padsize;
        std::vector<int64_t> start_indiex{padsize, padsize};
        std::vector<int64_t> slice_size{slicesize, slicesize};
        std::vector<int64_t> stride_size{strides[0], strides[1]};
        VECTOR_GEN           vgen(cntr);
        NODE_PTR             strided_slice_node = vgen.New_strided_slice(
            new_node, start_indiex, slice_size, stride_size, channel_out, spos);
        return RETV(strided_slice_node);
      } else if (((pads.size() > 0) && (pads[0] == 1)) &&
                 ((strides.size() > 0) && (strides[0] == 2))) {
        std::vector<int64_t> start_indiex{0, 0};
        std::vector<int64_t> slice_size{input_height, input_width};
        std::vector<int64_t> stride_size{2, 2};
        VECTOR_GEN           vgen(cntr);
        NODE_PTR             strided_slice_node = vgen.New_strided_slice(
            new_node, start_indiex, slice_size, stride_size, channel_out, spos);
        return RETV(strided_slice_node);
      } else {
        return RETV(new_node);
      }
    } else {
      return RETV(new_node);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_average_pool<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx = visitor->Context();
    if (ctx.Improve_ss_insert()) {
      ctx.Incr_num_op_ca_th();
    }
    CONTAINER*  cntr     = visitor->Context().Container();
    GLOB_SCOPE* gscope   = cntr->Glob_scope();
    SPOS        spos     = node->Spos();
    NODE_PTR    new_node = visitor->template Handle_node<RETV>(node).Node();
    new_node->Set_rtype(new_node->Child(0)->Rtype());
    // Get original conv2d input shape. Assuming NCHW&padding now.
    NODE_PTR orig_input = new_node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG((batch == 1) && (input_height == input_width),
                   "average_pool only supports batch=1");

    // TODO: ONLY stride, assuming the same stride
    std::vector<int> strides = Get_attr_int(new_node, "strides");
    if (!ctx.Improve_ss_insert()) {
      if ((strides.size() > 0) && (strides[0] == 2)) {
        std::vector<int64_t> start_indiex{0, 0};
        std::vector<int64_t> slice_size{input_height, input_width};
        std::vector<int64_t> stride_size{2, 2};
        VECTOR_GEN           vgen(cntr);
        NODE_PTR             strided_slice_node = vgen.New_strided_slice(
            new_node, start_indiex, slice_size, stride_size, channel_in, spos);
        return RETV(strided_slice_node);
      } else {
        return RETV(new_node);
      }
    } else if (ctx.Improve_ss_insert() && (ctx.Get_num_op_ca_th() == 4)) {
      // TODO: hard code here!
      std::vector<int64_t> start_indiex{0, 0};
      std::vector<int64_t> slice_size{20, 20};
      std::vector<int64_t> stride_size{4, 4};
      VECTOR_GEN           vgen(cntr);
      NODE_PTR             strided_slice_node = vgen.New_strided_slice(
          new_node, start_indiex, slice_size, stride_size, channel_in, spos);
      return RETV(strided_slice_node);
    } else {
      return RETV(new_node);
    }
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_HANDLER_H
