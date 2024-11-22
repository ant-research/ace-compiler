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
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using namespace air::base;

NODE_PTR VECTOR_GEN::New_add(NODE_PTR op0, NODE_PTR op1, const SPOS& spos) {
  NODE_PTR vadd_node = _cntr->New_bin_arith(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ADD), op0, op1,
      spos);
  return vadd_node;
}

NODE_PTR VECTOR_GEN::New_strided_slice(NODE_PTR             op0,
                                       std::vector<int64_t> start_indices,
                                       std::vector<int64_t> slice_size,
                                       std::vector<int64_t> stride_size,
                                       int channel, const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  // TODO: all size is 2.
  int64_t size = slice_size.size();
  AIR_ASSERT_MSG((size == 2) && (slice_size[0] % stride_size[0] == 0),
                 "strided_slize only support size=2!");

  // rtype
  std::vector<int64_t> rtype_shape = op0->Rtype()->Cast_to_arr()->Shape();
  rtype_shape[2]                   = slice_size[0] / stride_size[0];
  rtype_shape[3]                   = slice_size[0] / stride_size[0];

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
  Set_attr_int(slice_node, "channel", channel_num);

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
                              const SPOS& spos) {
  NODE_PTR vroll_node = _cntr->New_cust_node(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL), op0->Rtype(),
      spos);
  vroll_node->Set_child(0, op0);
  vroll_node->Set_child(1, op1);
  Set_attr_int(vroll_node, "nums", attr);
  return vroll_node;
}

NODE_PTR VECTOR_GEN::New_mul(NODE_PTR op0, NODE_PTR op1, const SPOS& spos) {
  NODE_PTR vmul_node = _cntr->New_bin_arith(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::MUL), op0, op1,
      spos);
  return vmul_node;
}

}  // namespace vector
}  // namespace nn
