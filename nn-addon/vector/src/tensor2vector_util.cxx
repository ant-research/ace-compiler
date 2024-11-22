//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/tensor2vector_util.h"

#include <cmath>

#include "air/core/opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"

namespace nn {
namespace vector {
using namespace air::base;

void TENSOR2VECTOR_UTIL::Gen_clear_data_stmt(ADDR_DATUM_PTR input_var,
                                             int64_t valid_len, TYPE_PTR etype,
                                             const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  // clear zero
  int64_t len_mask = valid_len;
  _ctx.Trace(TF_LOWER, "clean 0: len_mask=", len_mask, "\n");

  FPVEC                mask(len_mask, 1);
  std::vector<int64_t> mask_shape{len_mask};
  CONSTANT_PTR         mask_const =
      New_array_const(gscope, "clear_mask_n", _ctx.Get_num_vloop(), len_mask,
                      etype, mask_shape, (void*)mask.data(), spos);
  NODE_PTR mask_node = _cntr->New_ldc(mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, mask_node->Const(),
                 "clean 0 mask");

  NODE_PTR vmulc_node =
      New_mul(_cntr->New_ld(input_var, spos), mask_node, spos);
  STMT_PTR vmulc_stmt = _cntr->New_st(vmulc_node, input_var, spos);
  _ctx.Prepend(vmulc_stmt);
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_store_zero_to_var_stmt(
    std::string var_name, TYPE_PTR vtype, const SPOS& spos) {
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  std::string    var_str    = var_name + std::to_string(_ctx.Get_num_vloop());
  ADDR_DATUM_PTR result_var = fscope->New_var(vtype, var_str.c_str(), spos);

  NODE_PTR zero_node = _cntr->New_zero(vtype, spos);
  STMT_PTR st_stmt   = _cntr->New_st(zero_node, result_var, spos);
  _ctx.Prepend(st_stmt);

  return result_var;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_store_zero_to_var_stmt(
    std::string var_name, std::string ty_name, ARRAY_TYPE_PTR ty_arr,
    const std::vector<int64_t>& var_shape, const SPOS& spos) {
  TYPE_PTR arr_type = New_array_type(_cntr->Glob_scope(), ty_name,
                                     ty_arr->Elem_type(), var_shape, spos);
  return Gen_store_zero_to_var_stmt(var_name, arr_type, spos);
}

NODE_PTR TENSOR2VECTOR_UTIL::Gen_dup_input_node(NODE_PTR input_node,
                                                int64_t dup_num, int input_len,
                                                const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  NODE_PTR tmp_node = input_node;
  for (int i = 1; i < dup_num; i++) {
    std::vector<int> roll_num{-i * input_len};
    NODE_PTR         tmp_roll_node = New_roll(
        input_node, _cntr->New_intconst(s32_type, -i * input_len, spos),
        roll_num, spos);
    tmp_node = New_add(tmp_node, tmp_roll_node, spos);
  }
  return tmp_node;
}

void TENSOR2VECTOR_UTIL::Gen_dup_input_stmt(NODE_PTR input_node,
                                            int64_t dup_num, int input_len,
                                            ADDR_DATUM_PTR result_var,
                                            const SPOS&    spos) {
  NODE_PTR dup_node = Gen_dup_input_node(input_node, dup_num, input_len, spos);
  STMT_PTR input_dup_stmt = _cntr->New_st(dup_node, result_var, spos);
  _ctx.Prepend(input_dup_stmt);
}

void TENSOR2VECTOR_UTIL::Gen_loop_combine_stmt(
    const char* loop_name, int loop_ub, int elem_interval,
    ADDR_DATUM_PTR input_var, NODE_PTR mask_node, int64_t mask_slice_len,
    ADDR_DATUM_PTR result_var, const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  STMT_PTR  loop_stmt = New_loop(loop_name, 0, loop_ub, spos);
  STMT_LIST body_sl =
      STMT_LIST::Enclosing_list(loop_stmt->Node()->Child(3)->End_stmt());

  NODE_PTR roll_val_node = _cntr->New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::MUL),
      _cntr->New_ld(loop_stmt->Node()->Iv(), spos),
      _cntr->New_intconst(s32_type, elem_interval, spos), spos);

  std::vector<int> roll_num;
  for (int i = 0; i < loop_ub; i++) {
    roll_num.push_back(i * elem_interval);
  }
  NODE_PTR roll_node =
      New_roll(_cntr->New_ld(input_var, spos), roll_val_node, roll_num, spos);

  NODE_PTR mask_slice =
      New_slice(mask_node, _cntr->New_ld(loop_stmt->Node()->Iv(), spos),
                _cntr->New_intconst(s32_type, mask_slice_len, spos), spos);
  NODE_PTR mul_node = New_mul(roll_node, mask_slice, spos);

  NODE_PTR add_node  = New_add(_cntr->New_ld(result_var, spos), mul_node, spos);
  STMT_PTR add_store = _cntr->New_st(add_node, result_var, spos);
  body_sl.Append(add_store);
  _ctx.Prepend(loop_stmt);
}

STMT_PTR TENSOR2VECTOR_UTIL::New_loop(const char* index_str, int init,
                                      int upper, const SPOS& spos) {
  GLOB_SCOPE*    gscope   = _cntr->Glob_scope();
  FUNC_SCOPE*    fscope   = _cntr->Parent_func_scope();
  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // Generate a NULL loop
  std::string new_index_str(index_str);
  new_index_str += (std::string("_n") + std::to_string(_ctx.Get_num_vloop()));
  STR_PTR        index_name = gscope->New_str(new_index_str.c_str());
  ADDR_DATUM_PTR index_var  = fscope->New_var(s32_type, index_name, spos);
  NODE_PTR       index_node = _cntr->New_ld(index_var, spos);

  NODE_PTR init_node;
  init_node = _cntr->New_intconst(s32_type, init, spos);

  NODE_PTR upper_node = _cntr->New_intconst(s32_type, upper, spos);
  NODE_PTR cond_node =
      _cntr->New_bin_arith(air::core::OPCODE::LT, index_node, upper_node, spos);
  NODE_PTR incr_node =
      _cntr->New_bin_arith(air::core::OPCODE::ADD, index_node,
                           _cntr->New_intconst(s32_type, 1, spos), spos);

  NODE_PTR loop_body = _cntr->New_stmt_block(spos);
  STMT_PTR loop_stmt = _cntr->New_do_loop(index_var, init_node, cond_node,
                                          incr_node, loop_body, spos);

  AIR_ASSERT_MSG(loop_stmt->Node()->Child(3)->Is_block(),
                 "Loop body is not a block node!");
  return loop_stmt;
}

/**
 * @brief Generate vector IR for conv according to im2col strategy.
 * which maps conv to matrix multiply.
 */
NODE_PTR TENSOR2VECTOR_UTIL::New_conv_metakernel(
    NODE_PTR input, NODE_PTR weight, NODE_PTR bias, std::vector<int> ra,
    int channel_in, int channel_out, int output_height, int output_width,
    int kernel_hw, int stride, const SPOS& spos) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  // Get and check input type
  AIR_ASSERT_MSG(input->Rtype()->Is_array(), "conv input is not an array type");
  ARRAY_TYPE_PTR input_ty_arr = input->Rtype()->Cast_to_arr();

  std::vector<int64_t> input_shape = input_ty_arr->Shape();
  AIR_ASSERT_MSG(input_shape.size() == 1, "conv input dim %d is not 1.",
                 input_shape.size());

  // Get and check weight type
  CONSTANT_PTR weight_const = weight->Const();
  AIR_ASSERT_MSG(weight->Rtype()->Is_array(),
                 "conv weight is not an array type.");
  ARRAY_TYPE_PTR weight_ty_arr = weight->Rtype()->Cast_to_arr();

  std::vector<int64_t> weight_shape = weight_ty_arr->Shape();
  AIR_ASSERT_MSG(weight_shape.size() == 2, "conv weight const dim is not 2");

  std::vector<int64_t> shape{1, channel_out, output_height, output_width};
  TYPE_PTR vtype = New_array_type(gscope, "tmp_result_n", _ctx.Get_num_vloop(),
                                  input_ty_arr->Elem_type(), shape, spos);

  // VECTOR tmp_result = 0
  ADDR_DATUM_PTR tmp_result =
      Gen_store_zero_to_var_stmt("tmp_result_n", vtype, spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // input_dup = input + roll(input, -output_height*output_width)
  // duplicate input value channel_in times to make sure later roll works well
  int dup_num = static_cast<int>(ceil(1.0 * channel_out / channel_in)) + 1;
  if (this->_ctx.Improve_ss_insert() &&
      (this->_ctx.Get_num_op_ca_t2vsh() == 3)) {
    // TODO: hard code here!
    // For lenet FHE implement, vector length is 32768, valid vector length is
    // 16384 pad channel_in from 6 to 8 to avoid duplicate over 16384.
    dup_num = 2;
  }
  // TODO: hack here to make resnet conv pass. We process resnet with vector
  // len=65536.
  if (channel_in * output_height * output_width == 16384) {
    dup_num = 2;
  }
  AIR_ASSERT_MSG(channel_in * output_height * output_width * dup_num <= 32768,
                 "input shape dup number should <= 32768, current FHE "
                 "implement's N max is 65536");
  Gen_dup_input_stmt(input, dup_num, channel_in * output_height * output_width,
                     input_dup_var, spos);

  // Generate two-level LoopNest: level1 for channel_in, level2 for kernel_size
  STMT_PTR  loop1_stmt = New_loop("index_cin", 0, channel_in, spos);
  STMT_LIST body1_sl =
      STMT_LIST::Enclosing_list(loop1_stmt->Node()->Child(3)->End_stmt());

  STMT_PTR  loop2_stmt = New_loop("index_khw", 0, kernel_hw, spos);
  STMT_LIST body2_sl =
      STMT_LIST::Enclosing_list(loop2_stmt->Node()->Child(3)->End_stmt());

  // input_roll = ROLL(input_dup, ra[i])
  std::vector<int64_t> ra_shape(1, ra.size());

  for (int i = 0; i < ra.size(); i++) ra[i] *= stride;
  TYPE_PTR     ra_type  = New_array_type(gscope, "ra_int", _ctx.Get_num_vloop(),
                                         s32_type, ra_shape, spos);
  CONSTANT_PTR ra_const = gscope->New_const(CONSTANT_KIND::ARRAY, ra_type,
                                            (void*)(ra.data()), 4 * ra.size());
  _ctx.Trace_cmd(TF_LOWER, Trace_int_array, ra_const, "conv ra_const");
  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, weight->Const(),
                 "conv weight_im2col_const");
  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, bias->Const(), "conv bias");

  // roll(input, ra[i2])
  NODE_PTR ra_array = _cntr->New_array(
      _cntr->New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(ra_array, 0,
                       _cntr->New_ld(loop2_stmt->Node()->Iv(), spos));
  NODE_PTR ild_ra = _cntr->New_ild(ra_array, spos);

  //   std::vector<int> ra_roll_num;
  //   for (int i = 0; i < ra.size(); i++) ra_roll_num.push_back(ra[i] *
  //   stride);
  NODE_PTR vroll_node =
      New_roll(_cntr->New_ld(input_dup_var, spos), ild_ra, ra, spos);

  // weight_im2col[i1*kernel_hw+i2]
  NODE_PTR slice_index_node = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, _cntr->New_ld(loop2_stmt->Node()->Iv(), spos),
      _cntr->New_bin_arith(
          air::core::OPCODE::MUL, _cntr->New_ld(loop1_stmt->Node()->Iv(), spos),
          _cntr->New_intconst(s32_type, kernel_hw, spos), spos),
      spos);

  NODE_PTR weight_slice =
      New_slice(weight, slice_index_node,
                _cntr->New_intconst(
                    s32_type, output_height * output_width * channel_out, spos),
                spos);
  NODE_PTR vmul_node = New_mul(vroll_node, weight_slice, spos);

  NODE_PTR vadd_node =
      New_add(_cntr->New_ld(tmp_result, spos), vmul_node, spos);
  STMT_PTR vadd_store = _cntr->New_st(vadd_node, tmp_result, spos);

  body2_sl.Append(vadd_store);

  // roll input h*w for each iteration
  std::vector<int> vroll_cin_nums{output_height * output_width};
  NODE_PTR         vroll_cin_node = New_roll(
      _cntr->New_ld(input_dup_var, spos),
      _cntr->New_intconst(s32_type, output_height * output_width, spos),
      vroll_cin_nums, spos);
  STMT_PTR vroll_cin_st = _cntr->New_st(vroll_cin_node, input_dup_var, spos);

  body1_sl.Append(loop2_stmt);
  body1_sl.Append(vroll_cin_st);

  // TODO: for channel_in=1, only loop2 is needed.
  _ctx.Prepend(loop1_stmt);

  // add bias_const
  STMT_PTR vadd_bias_stmt = _cntr->New_st(
      New_add(_cntr->New_ld(tmp_result, spos), bias, spos), tmp_result, spos);
  _ctx.Prepend(vadd_bias_stmt);

  NODE_PTR ld_result = _cntr->New_ld(tmp_result, spos);

  return ld_result;
}

/**
 * @brief Generate vector IR for conv according to im2col strategy.
 * which maps conv to matrix multiply.
 */
NODE_PTR TENSOR2VECTOR_UTIL::New_conv_metakernel_fast(
    NODE_PTR input, NODE_PTR weight, NODE_PTR bias, std::vector<int> ra,
    int channel_in, int channel_out, int output_width, int output_height,
    int kernel_hw, const SPOS& spos) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  // Get and check input type
  AIR_ASSERT_MSG(input->Rtype()->Is_array(), "conv input is not an array type");
  ARRAY_TYPE_PTR input_ty_arr = input->Rtype()->Cast_to_arr();

  std::vector<int64_t> input_shape = input_ty_arr->Shape();
  AIR_ASSERT_MSG(input_shape.size() == 1, "conv input dim %d is not 1.",
                 input_shape.size());

  // Get and check weight type
  CONSTANT_PTR weight_const = weight->Const();
  AIR_ASSERT_MSG(weight->Rtype()->Is_array(),
                 "conv weight is not an array type.");
  ARRAY_TYPE_PTR weight_ty_arr = weight->Rtype()->Cast_to_arr();

  std::vector<int64_t> weight_shape = weight_ty_arr->Shape();
  AIR_ASSERT_MSG(weight_shape.size() == 2, "conv weight const dim is not 2");

  // Build var: result, result_cin, input_dup
  std::vector<int64_t> result_shape{1, channel_out, output_height,
                                    output_width};
  TYPE_PTR             vtype =
      New_array_type(gscope, "type_result_n", _ctx.Get_num_vloop(),
                     input_ty_arr->Elem_type(), result_shape, spos);

  // VECTOR result_var = 0
  ADDR_DATUM_PTR result_var =
      Gen_store_zero_to_var_stmt("result_n", vtype, spos);

  std::string result_cin_str =
      (std::string("result_cin_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR result_cin_var =
      fscope->New_var(vtype, result_cin_str.c_str(), spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // input_dup = input + roll(input, -output_height*output_width)
  // duplicate input value channel_in times to make sure later roll works well

  AIR_ASSERT_MSG(channel_out % channel_in == 0,
                 "channel_out % channel_in == 0 by padding in handle_conv");
  int dup_num = channel_out / channel_in;
  _ctx.Trace(TF_LOWER, "dup_num=", dup_num, "\n");

  Gen_dup_input_stmt(input, dup_num, channel_in * output_height * output_width,
                     input_dup_var, spos);

  // Generate roll loop
  // loop i 0:ra.size: input_roll[i] = roll(input_dup, ra[i]);
  STMT_PTR  loop_roll_stmt = New_loop("index_khw1", 0, kernel_hw, spos);
  STMT_LIST body_roll_sl =
      STMT_LIST::Enclosing_list(loop_roll_stmt->Node()->Child(3)->End_stmt());

  // Build input_roll[i] with array of array: [[vector0],[vector1]...]
  std::vector<int64_t> ra_shape(1, ra.size());

  TYPE_PTR input_vvtype =
      New_array_type(gscope, "type_input_roll_vv_n", _ctx.Get_num_vloop(),
                     vtype, ra_shape, spos);
  std::string input_roll_str =
      (std::string("input_roll_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_roll_var =
      fscope->New_var(input_vvtype, input_roll_str.c_str(), spos);

  NODE_PTR input_array = _cntr->New_array(
      _cntr->New_lda(input_roll_var, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(input_array, 0,
                       _cntr->New_ld(loop_roll_stmt->Node()->Iv(), spos));

  // input_roll[i] = ROLL(input_dup, ra[i])
  TYPE_PTR     ra_type  = New_array_type(gscope, "ra_int", _ctx.Get_num_vloop(),
                                         s32_type, ra_shape, spos);
  CONSTANT_PTR ra_const = gscope->New_const(CONSTANT_KIND::ARRAY, ra_type,
                                            (void*)(ra.data()), 4 * ra.size());
  _ctx.Trace_cmd(TF_LOWER, Trace_int_array, ra_const, "conv ra_const");

  NODE_PTR ra_array = _cntr->New_array(
      _cntr->New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(ra_array, 0,
                       _cntr->New_ld(loop_roll_stmt->Node()->Iv(), spos));
  NODE_PTR ild_ra = _cntr->New_ild(ra_array, spos);

  // st input_roll[i]
  NODE_PTR pre_roll_node =
      New_roll(_cntr->New_ld(input_dup_var, spos), ild_ra, ra, spos);
  STMT_PTR pre_roll_st = _cntr->New_ist(input_array, pre_roll_node, spos);
  body_roll_sl.Append(pre_roll_st);
  _ctx.Prepend(loop_roll_stmt);

  // Generate two-level LoopNest: level1 for channel_in, level2 for kernel_size
  STMT_PTR  loop1_stmt = New_loop("index_cin", 0, channel_in, spos);
  STMT_LIST body1_sl =
      STMT_LIST::Enclosing_list(loop1_stmt->Node()->Child(3)->End_stmt());

  // VECTOR result_cin = 0
  STMT_PTR st0_result_cin_stmt =
      _cntr->New_st(_cntr->New_zero(vtype, spos), result_cin_var, spos);
  body1_sl.Append(st0_result_cin_stmt);

  STMT_PTR  loop2_stmt = New_loop("index_khw2", 0, kernel_hw, spos);
  STMT_LIST body2_sl =
      STMT_LIST::Enclosing_list(loop2_stmt->Node()->Child(3)->End_stmt());

  // weight_im2col[i1*kernel_hw+i2]
  NODE_PTR slice_index_node = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, _cntr->New_ld(loop2_stmt->Node()->Iv(), spos),
      _cntr->New_bin_arith(
          air::core::OPCODE::MUL, _cntr->New_ld(loop1_stmt->Node()->Iv(), spos),
          _cntr->New_intconst(s32_type, kernel_hw, spos), spos),
      spos);

  NODE_PTR weight_slice =
      New_slice(weight, slice_index_node,
                _cntr->New_intconst(
                    s32_type, output_height * output_width * channel_out, spos),
                spos);

  NODE_PTR input_array2 = _cntr->New_array(
      _cntr->New_lda(input_roll_var, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(input_array2, 0,
                       _cntr->New_ld(loop2_stmt->Node()->Iv(), spos));
  NODE_PTR ild_input = _cntr->New_ild(input_array2, spos);

  NODE_PTR vmul_node = New_mul(ild_input, weight_slice, spos);

  NODE_PTR vadd_node =
      New_add(_cntr->New_ld(result_cin_var, spos), vmul_node, spos);
  STMT_PTR vadd_store = _cntr->New_st(vadd_node, result_cin_var, spos);

  body2_sl.Append(vadd_store);

  NODE_PTR cin_add_node;
  // TODO: hack here to make resnet conv pass. We process resnet with vector
  // len=65536.
  if (channel_out * output_height * output_width == 32768) {
    _ctx.Trace(TF_LOWER, "roll in loop2: data size=", 32768, "\n");
    cin_add_node = _cntr->New_ld(result_cin_var, spos);
  } else {
    cin_add_node =
        Gen_dup_input_node(_cntr->New_ld(result_cin_var, spos), 2,
                           channel_out * output_height * output_width, spos);
  }
  std::vector<int> roll_num_left;
  for (int i = 0; i < channel_in; i++)
    roll_num_left.push_back(i * output_height * output_width);
  NODE_PTR cin_roll_node2 = New_roll(
      cin_add_node,
      _cntr->New_bin_arith(
          air::core::OPCODE::MUL, _cntr->New_ld(loop1_stmt->Node()->Iv(), spos),
          _cntr->New_intconst(s32_type, output_height * output_width, spos),
          spos),
      roll_num_left, spos);
  NODE_PTR cin_add_node2 =
      New_add(_cntr->New_ld(result_var, spos), cin_roll_node2, spos);
  STMT_PTR cin_add_st = _cntr->New_st(cin_add_node2, result_var, spos);

  body1_sl.Append(loop2_stmt);
  body1_sl.Append(cin_add_st);

  // TODO: for channel_in=1, only loop2 is needed.
  _ctx.Prepend(loop1_stmt);

  // add bias_const
  STMT_PTR vadd_bias_stmt = _cntr->New_st(
      New_add(_cntr->New_ld(result_var, spos), bias, spos), result_var, spos);
  _ctx.Prepend(vadd_bias_stmt);

  // clear zero
  Gen_clear_data_stmt(result_var, channel_out * output_height * output_width,
                      input_ty_arr->Elem_type(), spos);

  NODE_PTR ld_result = _cntr->New_ld(result_var, spos);

  return ld_result;
}

/**
 * @brief Generate gemm metakernel which can be templated into conv2d and gemm
 * in Tensor IR.
 * TODO: params for template; metakernel as a funciton.
 *
 * @param weight_diag is ldc const weight after rotating
 * @param bias is ldc const bias
 */
NODE_PTR TENSOR2VECTOR_UTIL::New_gemm_metakernel_fast(NODE_PTR    input,
                                                      NODE_PTR    weight_diag,
                                                      NODE_PTR    bias,
                                                      const SPOS& spos,
                                                      bool        tiling) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  // Get and check input type
  ARRAY_TYPE_PTR       input_type  = input->Rtype()->Cast_to_arr();
  std::vector<int64_t> input_shape = input_type->Shape();
  AIR_ASSERT_MSG(((input_shape.size() == 2) && (input_shape[0] == 1)) ||
                     (input_shape.size() == 1),
                 "input: shape=%d dim[0]=%d. 2D input GEMM is work in progress",
                 input_shape.size(), input_shape[0]);

  // Get and check weight_diag type
  ARRAY_TYPE_PTR       weight_diag_type  = weight_diag->Rtype()->Cast_to_arr();
  std::vector<int64_t> weight_diag_shape = weight_diag_type->Shape();
  AIR_ASSERT_MSG(weight_diag_shape.size() == 2,
                 "weight_diag_type dim is not 2");
  int64_t              height = weight_diag_shape[0];
  int64_t              width  = weight_diag_shape[1];
  std::vector<int64_t> shape(1, width);

  _ctx.Trace(TF_LOWER, "New_gemm_metakernel_fast input shape: ",
             (input_shape.size() == 2) ? input_shape[1] : input_shape[0],
             ", weight_diag shape: ", height, "x", width, ", tiling:", tiling,
             "\n");
  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, weight_diag->Const(),
                 "gemm weight_diag_fast");

  TYPE_PTR vtype = New_array_type(gscope, "gemm_width", _ctx.Get_num_vloop(),
                                  input_type->Elem_type(), shape, spos);

  // VECTOR tmp_result = 0
  ADDR_DATUM_PTR tmp_result =
      Gen_store_zero_to_var_stmt("tmp_result_n", vtype, spos);

  std::string    tmp_block_str = (std::string("tmp_block_result_n") +
                               std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR tmp_block_result =
      fscope->New_var(vtype, tmp_block_str.c_str(), spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // input_dup = input + roll(input, -width). fast: height is width
  NODE_PTR input_dup    = input;
  NODE_PTR add_dup_node = input;

  // TODO: will change to use log version Gen_dup_input_node later.
  if (tiling) {
    _ctx.Trace(TF_LOWER, "Input replication: num=", width / height, "\n");
    for (int i = 1; i <= (int)log2(width / height); i++) {
      std::vector<int> roll_num{-(1 << (i - 1)) * (int)height};
      NODE_PTR         tmp_roll_node = New_roll(
          input_dup,
          _cntr->New_intconst(s32_type, -(1 << (i - 1)) * height, spos),
          roll_num, spos);
      add_dup_node = New_add(input_dup, tmp_roll_node, spos);
      add_dup_node->Set_rtype(vtype);
      input_dup = add_dup_node;
    }
  }

  STMT_PTR input_dup_stmt = _cntr->New_st(add_dup_node, input_dup_var, spos);
  _ctx.Prepend(input_dup_stmt);

  // height = h1*h2, h1 for block size.
  int h1 = 0, h2 = 0;
  Get_block_size(height, h1, h2);
  _ctx.Trace(TF_LOWER, "Get_block_size: h1=", h1, ", h2=", h2, "\n");

  // loop i 0:h1: input_roll[i] = roll(input_dup, i);
  STMT_PTR  loop_roll_stmt = New_loop("index_h1", 0, h1, spos);
  STMT_LIST body_roll_sl =
      STMT_LIST::Enclosing_list(loop_roll_stmt->Node()->Child(3)->End_stmt());

  // Build input_roll[i] with array of array: [[vector0],[vector1]...]
  std::vector<int64_t> ra_shape(1, h1);

  TYPE_PTR input_vvtype =
      New_array_type(gscope, "type_input_roll_vv_n", _ctx.Get_num_vloop(),
                     vtype, ra_shape, spos);
  std::string input_roll_str =
      (std::string("input_roll_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_roll_var =
      fscope->New_var(input_vvtype, input_roll_str.c_str(), spos);

  NODE_PTR input_array = _cntr->New_array(
      _cntr->New_lda(input_roll_var, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(input_array, 0,
                       _cntr->New_ld(loop_roll_stmt->Node()->Iv(), spos));

  // input_roll[i] = ROLL(input_dup, ra[i])
  std::vector<int> ra;
  for (int i = 0; i < h1; i++) ra.push_back(i);
  NODE_PTR pre_roll_node =
      New_roll(_cntr->New_ld(input_dup_var, spos),
               _cntr->New_ld(loop_roll_stmt->Node()->Iv(), spos), ra, spos);
  STMT_PTR pre_roll_st = _cntr->New_ist(input_array, pre_roll_node, spos);
  body_roll_sl.Append(pre_roll_st);
  _ctx.Prepend(loop_roll_stmt);

  // Generate a loop-nest(h2-h1) for GEMM.
  STMT_PTR  loop_stmt_h2 = New_loop("index_nest_h2", 0, h2, spos);
  STMT_LIST body_sl_h2 =
      STMT_LIST::Enclosing_list(loop_stmt_h2->Node()->Child(3)->End_stmt());

  STMT_PTR  loop_stmt_h1 = New_loop("index_nest_h1", 0, h1, spos);
  STMT_LIST body_sl_h1 =
      STMT_LIST::Enclosing_list(loop_stmt_h1->Node()->Child(3)->End_stmt());

  // tmp_block_result = 0
  NODE_PTR vzero_block_node = _cntr->New_zero(vtype, spos);
  STMT_PTR st0_block_stmt =
      _cntr->New_st(vzero_block_node, tmp_block_result, spos);
  body_sl_h2.Append(st0_block_stmt);

  // tmp_block_result += input_roll[i1] * weight_diag[i2*h1+i1]
  NODE_PTR slice_index_node = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, _cntr->New_ld(loop_stmt_h1->Node()->Iv(), spos),
      _cntr->New_bin_arith(air::core::OPCODE::MUL,
                           _cntr->New_ld(loop_stmt_h2->Node()->Iv(), spos),
                           _cntr->New_intconst(s32_type, h1, spos), spos),
      spos);

  NODE_PTR weight_slice =
      New_slice(weight_diag, slice_index_node,
                _cntr->New_intconst(s32_type, width, spos), spos);

  NODE_PTR input_array2 = _cntr->New_array(
      _cntr->New_lda(input_roll_var, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(input_array2, 0,
                       _cntr->New_ld(loop_stmt_h1->Node()->Iv(), spos));
  NODE_PTR ild_input = _cntr->New_ild(input_array2, spos);

  NODE_PTR vmul_node = New_mul(ild_input, weight_slice, spos);
  NODE_PTR vadd_node =
      New_add(_cntr->New_ld(tmp_block_result, spos), vmul_node, spos);
  STMT_PTR vadd_store = _cntr->New_st(vadd_node, tmp_block_result, spos);

  body_sl_h1.Append(vadd_store);

  // merge block: tmp_result += roll(tmp_block_result, i2*h1)
  STMT_PTR vadd_store2;
  if (tiling) {
    // mask1 and mask2: size h2
    int64_t old_height = width;

    FPVEC mask1_vec, mask2_vec;
    for (int k = 0; k < h2; k++) {
      FPVEC mask1(width, 0.0);
      FPVEC mask2(width, 1.0);
      for (size_t i = 0; i < width / height; i++) {
        for (size_t j = 0; j < height; j++) {
          if (j < (size_t)k * h1) {
            mask1[i * height + j] = 1.0;
            mask2[i * height + j] = 0.0;
          }
        }
      }
      mask1_vec = mask1_vec + mask1;
      mask2_vec = mask2_vec + mask2;
    }

    std::vector<int64_t> mask_shape{h2, width};
    CONSTANT_PTR         mask1_const =
        New_array_const(gscope, "mask1", h2 * width, input_type->Elem_type(),
                        mask_shape, (void*)mask1_vec.data(), spos);

    NODE_PTR mask1_slice =
        New_slice(_cntr->New_ldc(mask1_const, spos),
                  _cntr->New_ld(loop_stmt_h2->Node()->Iv(), spos),
                  _cntr->New_intconst(s32_type, width, spos), spos);

    CONSTANT_PTR mask2_const =
        New_array_const(gscope, "mask2", h2 * width, input_type->Elem_type(),
                        mask_shape, (void*)mask2_vec.data(), spos);

    NODE_PTR mask2_slice =
        New_slice(_cntr->New_ldc(mask2_const, spos),
                  _cntr->New_ld(loop_stmt_h2->Node()->Iv(), spos),
                  _cntr->New_intconst(s32_type, width, spos), spos);

    NODE_PTR block_mask_node1 =
        New_mul(_cntr->New_ld(tmp_block_result, spos), mask1_slice, spos);
    std::vector<int> roll_num_left1;
    for (int i = 0; i < h2; i++) roll_num_left1.push_back(i * h1 - height);
    // k*h1-h
    NODE_PTR block_roll_node1 =
        New_roll(block_mask_node1,
                 _cntr->New_bin_arith(
                     air::core::OPCODE::SUB,
                     _cntr->New_bin_arith(
                         air::core::OPCODE::MUL,
                         _cntr->New_ld(loop_stmt_h2->Node()->Iv(), spos),
                         _cntr->New_intconst(s32_type, h1, spos), spos),
                     _cntr->New_intconst(s32_type, height, spos), spos),
                 roll_num_left1, spos);

    NODE_PTR block_mask_node2 =
        New_mul(_cntr->New_ld(tmp_block_result, spos), mask2_slice, spos);
    std::vector<int> roll_num_left2;
    for (int i = 0; i < h2; i++) roll_num_left2.push_back(i * h1);
    // k*h1
    NODE_PTR block_roll_node2 = New_roll(
        block_mask_node2,
        _cntr->New_bin_arith(air::core::OPCODE::MUL,
                             _cntr->New_ld(loop_stmt_h2->Node()->Iv(), spos),
                             _cntr->New_intconst(s32_type, h1, spos), spos),
        roll_num_left2, spos);

    NODE_PTR vadd_node2 =
        New_add(_cntr->New_ld(tmp_result, spos),
                New_add(block_roll_node1, block_roll_node2, spos), spos);
    vadd_store2 = _cntr->New_st(vadd_node2, tmp_result, spos);
  } else {
    std::vector<int> roll_num_left;
    for (int i = 0; i < h2; i++) roll_num_left.push_back(i * h1);
    NODE_PTR block_roll_node = New_roll(
        _cntr->New_ld(tmp_block_result, spos),
        _cntr->New_bin_arith(air::core::OPCODE::MUL,
                             _cntr->New_ld(loop_stmt_h2->Node()->Iv(), spos),
                             _cntr->New_intconst(s32_type, h1, spos), spos),
        roll_num_left, spos);
    NODE_PTR vadd_node2 =
        New_add(_cntr->New_ld(tmp_result, spos), block_roll_node, spos);
    vadd_store2 = _cntr->New_st(vadd_node2, tmp_result, spos);
  }

  body_sl_h2.Append(loop_stmt_h1);
  body_sl_h2.Append(vadd_store2);
  _ctx.Prepend(loop_stmt_h2);

  if (!tiling) {
    _ctx.Trace(TF_LOWER,
               "tiling=0: block add=", (int)ceil(log2(width / height)), "\n");
    // Generate a loop for block add.
    STMT_PTR loop_blockadd_stmt =
        New_loop("index_add", 0, (int)ceil(log2(width / height)), spos);
    STMT_LIST body_blockadd_sl = STMT_LIST::Enclosing_list(
        loop_blockadd_stmt->Node()->Child(3)->End_stmt());
    // input_roll = ROLL(tmp_result, (1<<iv)*height)
    NODE_PTR shl_node = _cntr->New_bin_arith(
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::SHL),
        _cntr->New_intconst(s32_type, 1, spos),
        _cntr->New_ld(loop_blockadd_stmt->Node()->Iv(), spos), spos);
    NODE_PTR mul_node =
        _cntr->New_bin_arith(air::core::OPCODE::MUL, shl_node,
                             _cntr->New_intconst(s32_type, height, spos), spos);
    std::vector<int> roll_num_block;
    for (int i = 0; i < log2(width / height); i++) {
      roll_num_block.push_back((1U << i) * height);
    }
    NODE_PTR vroll_result_node = New_roll(_cntr->New_ld(tmp_result, spos),
                                          mul_node, roll_num_block, spos);
    NODE_PTR vadd1_node =
        New_add(_cntr->New_ld(tmp_result, spos), vroll_result_node, spos);
    STMT_PTR vadd1_store = _cntr->New_st(vadd1_node, tmp_result, spos);
    body_blockadd_sl.Append(vadd1_store);
    _ctx.Prepend(loop_blockadd_stmt);
  }

  // +C
  NODE_PTR vaddc_node = New_add(_cntr->New_ld(tmp_result, spos), bias, spos);
  STMT_PTR vaddc_stmt = _cntr->New_st(vaddc_node, tmp_result, spos);
  _ctx.Prepend(vaddc_stmt);

  // TODO: clean 0. suggest in FHE IR Level together with roll.
  Gen_clear_data_stmt(tmp_result, tiling ? width : height,
                      input_type->Elem_type(), spos);

  NODE_PTR ld_result = _cntr->New_ld(tmp_result, spos);

  return ld_result;
}

/**
 * @brief Generate gemm metakernel which can be templated into conv2d and gemm
 * in Tensor IR.
 * TODO: params for template; metakernel as a funciton.
 *
 * @param op1 is ldc const
 * @param op2 is ldc const
 */
NODE_PTR TENSOR2VECTOR_UTIL::New_gemm_metakernel(NODE_PTR op0, NODE_PTR op1,
                                                 NODE_PTR    op2,
                                                 const SPOS& spos) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  // Get and check op0 type
  AIR_ASSERT_MSG(op0->Rtype()->Is_array(), "op0 is not an array type");
  ARRAY_TYPE_PTR op0_ty_arr = op0->Rtype()->Cast_to_arr();

  std::vector<int64_t> op0_shape = op0_ty_arr->Shape();
  AIR_ASSERT_MSG(((op0_shape.size() == 2) && (op0_shape[0] == 1)) ||
                     (op0_shape.size() == 1),
                 "op0: shape=%d dim[0]=%d. 2D is work in progress",
                 op0_shape.size(), op0_shape[0]);

  // Get and check op1 type
  CONSTANT_PTR op1_const = op1->Const();
  AIR_ASSERT_MSG(op1->Rtype()->Is_array(), "op1 is not an array type");
  ARRAY_TYPE_PTR op1_ty_arr = op1->Rtype()->Cast_to_arr();

  std::vector<int64_t> op1_shape = op1_ty_arr->Shape();
  AIR_ASSERT_MSG(op1_shape.size() == 2, "op1 const dim is not 2");
  // TODO: enable assert after pad.
  // AIR_ASSERT_MSG(op1_shape[1] == width,
  //               "op1 const width should be equal to op0 widht");
  int64_t height = op1_shape[0];
  int64_t width  = op1_shape[1];
  // by roll, its wdith x2.
  std::vector<int64_t> shape(1, 2 * width);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, op1_const, "gemm op1_diag");

  TYPE_PTR vtype = New_array_type(gscope, "tmp", _ctx.Get_num_vloop(),
                                  op0_ty_arr->Elem_type(), shape, spos);

  // VECTOR tmp_result = 0
  ADDR_DATUM_PTR tmp_result =
      Gen_store_zero_to_var_stmt("tmp_result_n", vtype, spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // input_dup = input + roll(input, -width).
  Gen_dup_input_stmt(op0, 2, (int)width, input_dup_var, spos);

  // Generate a blocked loop for GEMM.
  STMT_PTR loop_stmt = New_loop("index_gemm", 0, height, spos);

  STMT_LIST body_sl =
      STMT_LIST::Enclosing_list(loop_stmt->Node()->Child(3)->End_stmt());

  // input_roll = ROLL(input_dup, i)
  std::vector<int> roll_num_height;
  for (int i = 0; i < height; i++) roll_num_height.push_back(i);
  NODE_PTR vroll_node = New_roll(_cntr->New_ld(input_dup_var, spos),
                                 _cntr->New_ld(loop_stmt->Node()->Iv(), spos),
                                 roll_num_height, spos);

  NODE_PTR op1_slice =
      New_slice(op1, _cntr->New_ld(loop_stmt->Node()->Iv(), spos),
                _cntr->New_intconst(s32_type, width, spos), spos);
  NODE_PTR vmul_node = New_mul(vroll_node, op1_slice, spos);

  NODE_PTR vadd_node =
      New_add(_cntr->New_ld(tmp_result, spos), vmul_node, spos);
  STMT_PTR vadd_store = _cntr->New_st(vadd_node, tmp_result, spos);
  body_sl.Append(vadd_store);

  _ctx.Prepend(loop_stmt);

  if (width / height > 1) {
    // Generate a loop for block add.
    STMT_PTR loop_blockadd_stmt =
        New_loop("index_add", 0, (int)ceil(log2(width / height)), spos);
    STMT_LIST body_blockadd_sl = STMT_LIST::Enclosing_list(
        loop_blockadd_stmt->Node()->Child(3)->End_stmt());

    // input_roll = ROLL(tmp_result, (1<<iv)*height)
    NODE_PTR shl_node = _cntr->New_bin_arith(
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::SHL),
        _cntr->New_intconst(s32_type, 1, spos),
        _cntr->New_ld(loop_blockadd_stmt->Node()->Iv(), spos), spos);
    NODE_PTR mul_node =
        _cntr->New_bin_arith(air::core::OPCODE::MUL, shl_node,
                             _cntr->New_intconst(s32_type, height, spos), spos);

    std::vector<int> roll_num_block;
    for (int i = 0; i < log2(width / height); i++) {
      roll_num_block.push_back((1U << i) * height);
    }
    NODE_PTR vroll_result_node = New_roll(_cntr->New_ld(tmp_result, spos),
                                          mul_node, roll_num_block, spos);

    NODE_PTR vadd1_node =
        New_add(_cntr->New_ld(tmp_result, spos), vroll_result_node, spos);
    STMT_PTR vadd1_store = _cntr->New_st(vadd1_node, tmp_result, spos);

    body_blockadd_sl.Append(vadd1_store);
    _ctx.Prepend(loop_blockadd_stmt);
  }

  // +C
  NODE_PTR vaddc_node = New_add(_cntr->New_ld(tmp_result, spos), op2, spos);
  STMT_PTR vaddc_stmt = _cntr->New_st(vaddc_node, tmp_result, spos);
  _ctx.Prepend(vaddc_stmt);

  // TODO: clean 0. suggest in FHE IR Level together with roll.
  Gen_clear_data_stmt(tmp_result, height, op0_ty_arr->Elem_type(), spos);

  NODE_PTR ld_result = _cntr->New_ld(tmp_result, spos);

  return ld_result;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Combine_cross_row(
    ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr, int64_t channel,
    int64_t ih, int64_t iw, int64_t ss_h, int64_t ss_w, int64_t ks,
    int64_t stride, int64_t padsize, const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  int64_t cin_len = channel * ih * iw;
  // 1.1 prepare combine row result, VECTOR result = 0
  std::vector<int64_t> comb_row_result_shape{cin_len};
  ADDR_DATUM_PTR       comb_row_result_var = Gen_store_zero_to_var_stmt(
      "comb_row_result", "comb_row_float", ty_arr, comb_row_result_shape, spos);

  // 1.2 prepare row combine mask
  FPVEC row_mask =
      Get_mask_for_row_combine(channel, ih, iw, ss_h, ss_w, stride);

  std::vector<int64_t> row_mask_shape{ss_w / stride, cin_len};
  CONSTANT_PTR         row_mask_const = New_array_const(
      gscope, "row_mask", (ss_w / stride) * cin_len, ty_arr->Elem_type(),
      row_mask_shape, (void*)row_mask.data(), spos);
  NODE_PTR row_mask_node = _cntr->New_ldc(row_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, row_mask_node->Const(),
                 "row mask");

  // 1.3 first loop to combine row
  Gen_loop_combine_stmt("combine_row_index", ss_w / stride, stride - 1,
                        input_var, row_mask_node, cin_len, comb_row_result_var,
                        spos);

  return comb_row_result_var;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Combine_cross_rc(
    ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr, int64_t channel,
    int64_t ih, int64_t iw, int64_t ss_h, int64_t ss_w, int64_t ks,
    int64_t stride, int64_t interval, const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  int64_t oh = ss_h / stride;
  int64_t ow = ss_w / stride;

  int64_t cin_len = channel * ih * iw;
  // 2. combine by rows and columns
  // 2.1 prepare combine row and column result, VECTOR result = 0
  std::vector<int64_t> comb_rc_result_shape{cin_len};
  ADDR_DATUM_PTR       comb_rc_result_var = Gen_store_zero_to_var_stmt(
      "comb_rc_result", "comb_rc_float", ty_arr, comb_rc_result_shape, spos);

  // 2.2 prepare row and column combine mask
  FPVEC rc_mask = Get_mask_for_rc_combine(channel, ih, iw, oh);

  std::vector<int64_t> rc_mask_shape{oh, cin_len};
  CONSTANT_PTR         rc_mask_const =
      New_array_const(gscope, "rc_mask", oh * cin_len, ty_arr->Elem_type(),
                      rc_mask_shape, (void*)rc_mask.data(), spos);
  NODE_PTR rc_mask_node = _cntr->New_ldc(rc_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, rc_mask_node->Const(),
                 "row column mask");

  // 2.3 second loop to combine row and column
  Gen_loop_combine_stmt("combine_rc_index", oh, interval, input_var,
                        rc_mask_node, cin_len, comb_rc_result_var, spos);

  return comb_rc_result_var;
}

void TENSOR2VECTOR_UTIL::Gen_combine_cross_channel(ADDR_DATUM_PTR input_var,
                                                   int64_t channel, int64_t ih,
                                                   int64_t iw, int64_t oh,
                                                   int64_t     ow,
                                                   const SPOS& spos) {
  AIR_ASSERT_MSG(
      ih * iw >= channel * oh * ow,
      "Gen_combine_cross_channel function only suitable for sparse data!")

  GLOB_SCOPE*    gscope   = _cntr->Glob_scope();
  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  NODE_PTR input_node = _cntr->New_ld(input_var, spos);

  STMT_PTR cc_loop_stmt =
      New_loop("combine_cc_index", 0, ceil(log2(channel)), spos);
  STMT_LIST cc_body_sl =
      STMT_LIST::Enclosing_list(cc_loop_stmt->Node()->Child(3)->End_stmt());

  // input_roll = ROLL(input, pow(2, i) * (ih * iw - oh * ow));
  // pow(2,i) == (1<< i)
  NODE_PTR shl_node = _cntr->New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::SHL),
      _cntr->New_intconst(s32_type, 1, spos),
      _cntr->New_ld(cc_loop_stmt->Node()->Iv(), spos), spos);
  NODE_PTR cc_roll_val_node = _cntr->New_bin_arith(
      air::core::OPCODE::MUL, shl_node,
      _cntr->New_intconst(s32_type, (ih * iw - oh * ow), spos), spos);

  std::vector<int> cc_roll_num;
  for (int i = 0; i < ceil(log2(channel)); i++) {
    cc_roll_num.push_back(pow(2, i) * (ih * iw - oh * ow));
  }
  NODE_PTR cc_roll_node =
      New_roll(input_node, cc_roll_val_node, cc_roll_num, spos);
  input_node = New_add(input_node, cc_roll_node, spos);

  STMT_PTR cc_add_store = _cntr->New_st(input_node, input_var, spos);
  cc_body_sl.Append(cc_add_store);
  _ctx.Prepend(cc_loop_stmt);
}

NODE_PTR TENSOR2VECTOR_UTIL::Combine_cross_channel(
    ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr, int64_t channel,
    int64_t ih, int64_t iw, int64_t ss_h, int64_t ss_w, int64_t ks,
    int64_t stride, const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  int64_t oh = ss_h / stride;
  int64_t ow = ss_w / stride;

  // 3.1 prepare combine cross channel result(final result), VECTOR result = 0
  // TODO: assume only 1 input here!
  std::vector<int64_t> final_result_shape{1, channel, oh, ow};
  ADDR_DATUM_PTR       final_result_var = Gen_store_zero_to_var_stmt(
      "final_result", "final_float", ty_arr, final_result_shape, spos);

  if (_ctx.Improve_ss_insert() && (ih * iw >= oh * ow * channel)) {
    // till now, this impl is only enabled when improve stride slice insert
    // option(-T2V:improve_ssi) is provided and all the output data can put into
    // one channel input. below implement does not cost level, and use less
    // rotate and add compared with implement in else statement block. later op
    // should pay attention to the dirty data out of 1 channel!

    // 3.2 only use rotate and add to combine and compact
    Gen_combine_cross_channel(input_var, channel, ih, iw, oh, ow, spos);

    STMT_PTR st_stmt =
        _cntr->New_st(_cntr->New_ld(input_var, spos), final_result_var, spos);
    _ctx.Prepend(st_stmt);
  } else {
    // 3.2 prepare channel combine mask
    FPVEC cc_mask =
        Get_mask_for_cross_channel_combine(channel, ih, iw, oh * ow);

    int64_t              cin_len = channel * ih * iw;
    std::vector<int64_t> cc_mask_shape{channel, cin_len};
    CONSTANT_PTR         cc_mask_const = New_array_const(
        gscope, "cc_mask", channel * cin_len, ty_arr->Elem_type(),
        cc_mask_shape, (void*)cc_mask.data(), spos);
    NODE_PTR cc_mask_node = _cntr->New_ldc(cc_mask_const, spos);

    _ctx.Trace_cmd(TF_LOWER, Trace_float_array, cc_mask_node->Const(),
                   "cross channel mask");

    // 3.3 third loop to combine cross channel
    Gen_loop_combine_stmt("combine_cc_index", channel, ih * iw - oh * ow,
                          input_var, cc_mask_node, cin_len, final_result_var,
                          spos);
  }
  NODE_PTR load_result_node = _cntr->New_ld(final_result_var, spos);
  load_result_node->Set_rtype(final_result_var->Type());
  return load_result_node;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Roll_valid_to_start(ADDR_DATUM_PTR input_var,
                                                       ARRAY_TYPE_PTR ty_arr,
                                                       int64_t ih, int64_t iw,
                                                       int64_t     padsize,
                                                       const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // 1. move valid data to the start
  std::string rolled_result_str =
      std::string("rolled_result") + std::to_string(_ctx.Get_num_vloop());
  ADDR_DATUM_PTR roll_result_var =
      fscope->New_var(ty_arr, rolled_result_str.c_str(), spos);

  int              roll_offset = padsize * (iw + 1);
  std::vector<int> roll_num    = {roll_offset};
  NODE_PTR         vroll_node  = New_roll(
      _cntr->New_ld(input_var, spos),
      _cntr->New_intconst(s32_type, roll_offset, spos), roll_num, spos);

  STMT_PTR roll_result_stmt = _cntr->New_st(vroll_node, roll_result_var, spos);
  _ctx.Prepend(roll_result_stmt);

  return roll_result_var;
}

NODE_PTR TENSOR2VECTOR_UTIL::New_extract_valid_data(
    NODE_PTR input, int64_t channel, int64_t padsize, int64_t ih, int64_t iw,
    int64_t ss_h, int64_t ss_w, int64_t ks, int64_t stride,
    bool is_start_pos_valid, const SPOS& spos) {
  _ctx.Incr_num_vloop();
  _ctx.Trace(TF_LOWER, "channel: ", channel, " padsize: ", padsize,
             "input height: ", ih, " input width:", iw,
             "stride slice size height: ss_h:", ss_h,
             "stride slice size width: ss_w:", ss_w, " kernel shape: ", ks,
             " stride: ", stride, "\n");
  AIR_ASSERT_MSG(iw == ih, "currently input height and width should be equal");
  AIR_ASSERT_MSG(ss_w == ss_h,
                 "currently stride slice height and width should be equal");

  ARRAY_TYPE_PTR op_ty_arr = input->Rtype()->Cast_to_arr();

  ADDR_DATUM_PTR comb_row_result_var;

  int64_t interval = 0;
  if (is_start_pos_valid) {
    AIR_ASSERT_MSG(padsize == 0,
                   "when the first element is valid, padsize should be 0");
    comb_row_result_var =
        Combine_cross_row(input->Addr_datum(), op_ty_arr, channel, ih, iw, ss_h,
                          ss_w, ks, stride, padsize, spos);
    interval = iw * (stride - 1) + (iw - ss_w / stride);
  } else {
    AIR_ASSERT_MSG(
        (padsize == 1) || (padsize == 2),
        "when the first element is invalid, padsize should be 1 or 2");
    comb_row_result_var = Roll_valid_to_start(input->Addr_datum(), op_ty_arr,
                                              ih, iw, padsize, spos);
    interval            = 2 * padsize;
    // stride is even number, for lenet should be conv2d + avg pool sequence
    // occur and no extract after conv2d immediately
    // TODO: later when resnet, need to improve!
    if ((stride & 1) == 0) {
      comb_row_result_var =
          Combine_cross_row(comb_row_result_var, op_ty_arr, channel, ih, iw,
                            ss_h, ss_w, ks, stride, padsize, spos);
      interval = iw + (iw - (iw - padsize * 2) /
                                stride);  // (iw-padsize*2)/stride number of
                                          // valid data in one row
    }
  }

  ADDR_DATUM_PTR comb_rc_result_var =
      Combine_cross_rc(comb_row_result_var, op_ty_arr, channel, ih, iw, ss_h,
                       ss_w, ks, stride, interval, spos);

  return Combine_cross_channel(comb_rc_result_var, op_ty_arr, channel, ih, iw,
                               ss_h, ss_w, ks, stride, spos);
}

}  // namespace vector
}  // namespace nn
