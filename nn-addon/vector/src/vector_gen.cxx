//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vector_gen.h"

#include <cmath>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "nn/core/attr.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using namespace air::base;

NODE_PTR VECTOR_GEN::New_roll_sum(NODE_PTR op0, const SPOS& spos) {
  CMPLR_ASSERT(0, "Fix rtype for New_una_arith.");
  NODE_PTR roll_sum_node = _cntr->New_una_arith(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL_SUM),
      op0->Rtype(), op0, spos);
  return roll_sum_node;
}

NODE_PTR VECTOR_GEN::New_add(NODE_PTR op0, NODE_PTR op1, const SPOS& spos) {
  // semantic: vector add. no explicit broadcast now, pad is reasonable.
  int64_t op0_numel = op0->Rtype()->Cast_to_arr()->Elem_count();
  int64_t op1_numel = op1->Rtype()->Cast_to_arr()->Elem_count();

  TYPE_PTR rtype = (op0_numel >= op1_numel) ? op0->Rtype() : op1->Rtype();

  NODE_PTR vadd_node = _cntr->New_bin_arith(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ADD), rtype, op0,
      op1, spos);
  return vadd_node;
}

NODE_PTR VECTOR_GEN::New_group_conv(NODE_PTR node) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  const SPOS& spos   = node->Spos();

  // Get original conv2d input shape. Assuming NCHW&padding now.
  NODE_PTR input = node->Child(0);
  int64_t  batch = 0, channel = 0, ih = 0, iw = 0;
  Get_array_nchw(input->Rtype(), batch, channel, ih, iw);

  std::vector<int> strides = Get_attr_data<int>(node, core::ATTR::STRIDE);
  std::vector<int> ks      = Get_attr_data<int>(node, core::ATTR::KSHAPE);
  AIR_ASSERT((ks[0] == 3) && (ks[1] == 3));

  // new weight const
  std::vector<int64_t> wt_shape = {channel, 1, ks[0], ks[1]};
  int                  wt_size  = channel * 1 * ks[0] * ks[1];
  std::vector<float>   weight(wt_size, 1.0 / (ks[0] * ks[1]));
  CONST_TYPE_PTR       f32_type = gscope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  CONSTANT_PTR wt_const = New_array_const(gscope, wt_size, f32_type, wt_shape,
                                          (void*)weight.data(), spos);

  // new bias const whose values are all zero
  std::vector<int64_t> bias_shape = {channel};
  std::vector<float>   bias(channel, 0.0);
  CONSTANT_PTR         bias_const = New_array_const(
      gscope, channel, f32_type, bias_shape, (void*)bias.data(), spos);

  NODE_PTR conv_node = _cntr->New_cust_node(
      OPCODE(nn::core::NN, nn::core::OPCODE::CONV), node->Rtype(), spos);
  conv_node->Set_child(0, node->Child(0));
  conv_node->Set_child(1, _cntr->New_ldc(wt_const, spos));
  conv_node->Set_child(2, _cntr->New_ldc(bias_const, spos));

  // set attributes
  conv_node->Copy_attr(node);
  std::vector<int> group{(int)channel};
  conv_node->Set_attr(core::ATTR::GROUP, group.data(), group.size());
  std::vector<int> work_stride = {1, 1};
  conv_node->Set_attr(core::ATTR::STRIDE, work_stride.data(),
                      work_stride.size());
  conv_node->Set_attr(core::ATTR::ORIG_STRIDE, strides.data(), strides.size());

  return conv_node;
}

NODE_PTR VECTOR_GEN::New_strided_slice(NODE_PTR             op0,
                                       std::vector<int64_t> start_indices,
                                       std::vector<int64_t> slice_size,
                                       std::vector<int64_t> stride_size,
                                       int channel, const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  // TODO: all size is 2.
  int64_t size = slice_size.size();
  AIR_ASSERT((size == 2));

  // rtype
  std::vector<int64_t> rtype_shape = op0->Rtype()->Cast_to_arr()->Shape();
  rtype_shape[2]                   = slice_size[0] / stride_size[0];
  rtype_shape[3]                   = slice_size[1] / stride_size[1];

  TYPE_PTR slice_rtype = New_array_type(
      gscope, "strided_slice", op0->Rtype()->Cast_to_arr()->Elem_type(),
      rtype_shape, spos);

  std::vector<int64_t> size_shape{size};

  CONST_TYPE_PTR s64_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S64);
  CONSTANT_PTR   start_indices_const =
      New_array_const(gscope, "size_shape", size, s64_type, size_shape,
                      (void*)start_indices.data(), spos);

  CONSTANT_PTR slice_size_const =
      New_array_const(gscope, "size_shape", size, s64_type, size_shape,
                      (void*)slice_size.data(), spos);

  CONSTANT_PTR stride_size_const =
      New_array_const(gscope, "size_shape", size, s64_type, size_shape,
                      (void*)stride_size.data(), spos);

  std::vector<int> channel_num{channel};
  NODE_PTR         slice_node = _cntr->New_cust_node(
      OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE), slice_rtype, spos);
  slice_node->Set_child(0, op0);
  slice_node->Set_child(1, _cntr->New_ldc(start_indices_const, spos));
  slice_node->Set_child(2, _cntr->New_ldc(slice_size_const, spos));
  slice_node->Set_child(3, _cntr->New_ldc(stride_size_const, spos));
  slice_node->Set_attr(core::ATTR::CHANNEL, channel_num.data(),
                       channel_num.size());
  return slice_node;
}

NODE_PTR VECTOR_GEN::New_slice(NODE_PTR op0, NODE_PTR start_indices,
                               NODE_PTR slice_size, const SPOS& spos) {
  GLOB_SCOPE*          gscope = _cntr->Glob_scope();
  std::vector<int64_t> slice_shape{(int64_t)slice_size->Intconst()};
  AIR_ASSERT_MSG(op0->Rtype()->Cast_to_arr()->Shape().size() == 2,
                 "Only support slice input shape is 2D now.")
  AIR_ASSERT_MSG(op0->Rtype()->Cast_to_arr()->Shape()[1] == slice_shape[0],
                 "slice size is not correct");
  TYPE_PTR slice_rtype = New_array_type(
      gscope, "slice_float", op0->Rtype()->Cast_to_arr()->Elem_type(),
      slice_shape, spos);

  NODE_PTR slice_node = _cntr->New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::SLICE), slice_rtype,
      spos);
  slice_node->Set_child(0, op0);
  slice_node->Set_child(1, start_indices);
  // TODO: Verify slice_size <= op0 shape
  slice_node->Set_child(2, slice_size);

  return slice_node;
}

NODE_PTR VECTOR_GEN::New_reshape(NODE_PTR op0, std::vector<int64_t> shape,
                                 const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  // TODO: 1)other dims; 2)Verify shape consistency.
  AIR_ASSERT_MSG(shape.size() == 1, "Only support reshape to 1D now.");

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR       shape_type =
      New_array_type(gscope, "reshape_float",
                     op0->Rtype()->Cast_to_arr()->Elem_type(), shape, spos);

  std::vector<int64_t> const_shape{(int64_t)shape.size()};
  CONSTANT_PTR         shape_const =
      New_array_const(gscope, "shape_int", shape.size(), s32_type, const_shape,
                      (void*)shape.data(), spos);

  NODE_PTR reshape_node = _cntr->New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::RESHAPE),
      shape_type, spos);
  reshape_node->Set_child(0, op0);
  reshape_node->Set_child(1, _cntr->New_ldc(shape_const, spos));
  return reshape_node;
}

NODE_PTR VECTOR_GEN::New_roll(NODE_PTR op0, NODE_PTR op1, const SPOS& spos) {
  NODE_PTR vroll_node = _cntr->New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL), op0->Rtype(),
      spos);
  vroll_node->Set_child(0, op0);
  vroll_node->Set_child(1, op1);
  return vroll_node;
}

NODE_PTR VECTOR_GEN::New_roll(NODE_PTR op0, NODE_PTR op1, std::vector<int> attr,
                              const SPOS& spos, TYPE_PTR rtype) {
  AIR_ASSERT_MSG(attr.size() > 0, "roll num attribute should not be empty");
  TYPE_PTR vroll_rtype = (rtype == air::base::Null_ptr) ? op0->Rtype() : rtype;
  NODE_PTR vroll_node  = _cntr->New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL), vroll_rtype,
      spos);
  vroll_node->Set_child(0, op0);
  vroll_node->Set_child(1, op1);
  vroll_node->Set_attr(core::ATTR::RNUM, attr.data(), attr.size());
  return vroll_node;
}

NODE_PTR VECTOR_GEN::New_mul(NODE_PTR op0, NODE_PTR op1, const SPOS& spos) {
  NODE_PTR vmul_node = _cntr->New_bin_arith(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::MUL), op0->Rtype(),
      op0, op1, spos);
  return vmul_node;
}

}  // namespace vector
}  // namespace nn
