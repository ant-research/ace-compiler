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
#include "nn/vector/tensor2vector_py_airgen.h"
#include "nn/vector/vector_opcode.h"

namespace nn {
namespace vector {
using namespace air::base;

// Pad gemm: increasing the n or k so that one is a multiple of the other
// TODO: pad to num_slots & adjust k_pad...
void Compute_pad_gemm(int64_t& n, int64_t& k, int64_t num_slots) {
  int64_t orig_n = n;
  int64_t orig_k = k;

  // make sure n is less than k
  if (n > k) {
    std::swap(n, k);
  }

  if (Is_power_of_two(k) && !Is_power_of_two(n)) {
    int limit = Next_power_of_two(n);
    int padn;
    for (padn = n; padn <= limit; padn++) {
      if (k % padn == 0) break;
    }
    n = padn;
  }

  if (k == num_slots || k == num_slots / 2) {
    int padn;
    for (padn = n; padn <= n * k; padn++) {
      if (k % padn == 0) break;
    }
    n = padn;
  }

  int64_t incre_n = (k - n % k) % k;
  int64_t incre_k = (n - k % n) % n;

  if (incre_n <= incre_k) {
    n += incre_n;
  } else {
    k += incre_k;
  }

  if (orig_n > orig_k) {
    std::swap(n, k);
  }

  if ((k > (num_slots / 2)) && (k < num_slots)) {
    k = num_slots;
    while (!((n % k == 0) || (k % n == 0))) n++;
  }

  AIR_ASSERT_MSG((k <= (num_slots / 2)) || (k == num_slots),
                 "k_pad size constraint (TODO)")
}

// cost model for 1x1 conv: c1*c2=c, and c1+c2 is minimum.
// c is the input channel of conv input.
int Get_costmodel_mini_factor1(int c) {
  // Only support the condition which is common in AI.
  if ((c < 32) || !Is_power_of_two(c)) {
    return 1;
  }
  int min_sum = c;
  int factor1 = 1;
  for (int i = 2; i <= sqrt(c); i++) {
    if ((c % i == 0) && ((i + c / i) < min_sum)) {
      factor1 = i;
    }
  }
  return factor1;
}

// cost model for kernel fusion. Goal: parallelism by subblock strucutre in
// vector. channel_in/bank + 2*bank <  channel_in
int Get_costmodel_fusion(int channel_in, int channel_out, int kernel_size,
                         int output_size, int group, int slots) {
  int bank = 1;

  // no need for 1x1 because it is done by cap_block "blocking".
  // no need for depthwise because is is parallized naturally.
  if (Is_power_of_two(output_size) && (channel_out % channel_in == 0) &&
      (kernel_size > 1) && (group == 1) && (slots / output_size >= 4) &&
      channel_out >= channel_in) {
    bank = slots / output_size / 2;
    // cannot exceed the number of MetaKernels.
    if (bank > channel_in) bank = channel_in;
    if ((bank >= 2) && (channel_in / bank + 2 * bank) >= channel_in) bank /= 2;
    if (bank > 8) bank = 8;
  }

  return bank;
}

// Reduce_add_intra: input is vector with rep blocks each size kd.
// [kd;sf; || kd;sf; || ...]
// Add rep blocks, and return ADDR_DATUM_PTR result.
ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Reduce_add_intra(ADDR_DATUM_PTR input,
                                                    int rep, int kd, int sf,
                                                    const SPOS& spos) {
  _ctx.Trace(TF_LOWER, "Reduce_add_intra: rep=", rep, " kd=", kd, " sf=", sf,
             "\n");
  FUNC_SCOPE*    fscope = _cntr->Parent_func_scope();
  CONST_TYPE_PTR s32t = _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  TYPE_PTR       vtype      = input->Type();
  ADDR_DATUM_PTR var_reduce = fscope->New_var(vtype, "reduce_intra", spos);
  STMT_PTR init = _cntr->New_st(_cntr->New_ld(input, spos), var_reduce, spos);
  _ctx.Prepend(init);

  if (Is_power_of_two(rep)) {
    // Reduce add by log based loop(0...log2(rep)): add rotate(input,
    // 2^i*(bsize+sf))
    // This method uses less rotations. e.g gemv_n10_k4096, the number of
    // rotations reduces from 410 to 8 compared with else branch.
    Gen_loop_roll_add_stmt("ri", log2(rep), (kd + sf),
                           _cntr->New_ld(var_reduce, spos), spos);
  } else {
    // Reduce add by loop(1...rep): add rotate(input, i*(bsize+sf))
    STMT_PTR  loop_reduce = New_loop("ri", 1, rep, spos);
    STMT_LIST reduce_sl   = Get_loop_sl(loop_reduce);

    std::vector<int> roll_num;
    for (int i = 1; i < rep; i++) {
      roll_num.push_back(i * (kd + sf));
    }

    NODE_PTR shift = _cntr->New_bin_arith(
        air::core::OPCODE::MUL, s32t, Get_loop_iv(loop_reduce),
        _cntr->New_intconst(s32t, kd + sf, spos), spos);

    NODE_PTR input_roll_left =
        New_roll(_cntr->New_ld(input, spos), shift, roll_num, spos);

    NODE_PTR add_roll =
        New_add(_cntr->New_ld(var_reduce, spos), input_roll_left, spos);

    STMT_PTR store_reduce = _cntr->New_st(add_roll, var_reduce, spos);
    reduce_sl.Append(store_reduce);
    _ctx.Prepend(loop_reduce);
  }
  return var_reduce;
}

// cyclic: input{a,b,c,d} -> {b,c,d,a}
// n: grids, how many rotations are needed.
// so total size=n*len masks, each grid loop need 1.
NODE_PTR TENSOR2VECTOR_UTIL::Roll_cyclic(ADDR_DATUM_PTR input, int len,
                                         int block_size, int n,
                                         ADDR_DATUM_PTR iv, const SPOS& spos) {
  //  Gen mask1 [shift=1, len-shift=0], [n,len]
  //  Gen mask2 [shift=0, len-shift=1]  [n,len]
  FPVEC mask1(n * len, 0.0), mask2(n * len, 1.0);
  for (int i = 0; i < n; i++) {
    for (int s = 0; s < i * block_size; s++) {
      mask1[i * len + s] = 1.0;
      mask2[i * len + s] = 0.0;
    }
  }

  CONST_TYPE_PTR s32t = _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  CONST_TYPE_PTR f32t =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  CONSTANT_PTR mask1_const =
      New_array_const(_cntr->Glob_scope(), "roll_mask1", n * len, f32t,
                      {n, len}, (void*)mask1.data(), spos);
  CONSTANT_PTR mask2_const =
      New_array_const(_cntr->Glob_scope(), "roll_mask2", n * len, f32t,
                      {n, len}, (void*)mask2.data(), spos);
  NODE_PTR mask1_slice =
      New_slice(_cntr->New_ldc(mask1_const, spos), _cntr->New_ld(iv, spos),
                _cntr->New_intconst(s32t, len, spos), spos);
  NODE_PTR mask2_slice =
      New_slice(_cntr->New_ldc(mask2_const, spos), _cntr->New_ld(iv, spos),
                _cntr->New_intconst(s32t, len, spos), spos);
  NODE_PTR roll_val_node1 = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32t, _cntr->New_intconst(s32t, -len, spos),
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32t,
                           _cntr->New_ld(iv, spos),
                           _cntr->New_intconst(s32t, block_size, spos), spos),
      spos);
  std::vector<int> roll_num1;
  for (int i = 0; i < n; i++) roll_num1.push_back(i * block_size - len);
  NODE_PTR value1 =
      New_roll(New_mul(_cntr->New_ld(input, spos), mask1_slice, spos),
               roll_val_node1, roll_num1, spos);
  std::vector<int> roll_num2;
  for (int i = 0; i < n; i++) roll_num2.push_back(i * block_size);
  NODE_PTR value2 =
      New_roll(New_mul(_cntr->New_ld(input, spos), mask2_slice, spos),
               _cntr->New_bin_arith(
                   air::core::OPCODE::MUL, s32t, _cntr->New_ld(iv, spos),
                   _cntr->New_intconst(s32t, block_size, spos), spos),
               roll_num2, spos);
  NODE_PTR add_node = New_add(value1, value2, spos);

  return add_node;
}

// Collective result of blocks in a grid:
// a common "reduce" computation pattern in vector
// |block0: data-pad|block1: data-pad|block2: data-pad| ... tail|
//           \   \            /   /            /    /         /
//            ================================================
//                          += result

PREG_PTR TENSOR2VECTOR_UTIL::Gen_collective_reduce_stmt(
    ADDR_DATUM_PTR result_var, TYPE_PTR etype, const SPOS& spos, int num_block,
    int width_block_data, int width_block_pad, int output_size,
    bool need_mask) {
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();

  // corner case
  if (output_size == _ctx.Get_slot()) {
    _ctx.Trace(TF_LOWER,
               "Gen_collective_reduce_stmt: output_size == _ctx.Get_slot() ",
               output_size, "\n");
    AIR_ASSERT_MSG(num_block == 1, "if output_size == _ctx.Get_slot()");
    if (need_mask) {
      FPVEC                mask1(width_block_data, 1.0);
      std::vector<int64_t> mask_shape{width_block_data};
      CONSTANT_PTR         mask1_const =
          New_array_const(gscope, "mask1", width_block_data, etype, mask_shape,
                          (void*)mask1.data(), spos);
      NODE_PTR init_m1 = New_mul(_cntr->New_ld(result_var, spos),
                                 _cntr->New_ldc(mask1_const, spos), spos);

      PREG_PTR epi_preg   = fscope->New_preg(result_var->Type());
      STMT_PTR init_store = _cntr->New_stp(init_m1, epi_preg, spos);
      _ctx.Prepend(init_store);
      return epi_preg;
    } else {
      PREG_PTR epi_preg = fscope->New_preg(result_var->Type());
      STMT_PTR init_store =
          _cntr->New_stp(_cntr->New_ld(result_var, spos), epi_preg, spos);
      _ctx.Prepend(init_store);
      return epi_preg;
    }
  }

  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  int width_block = width_block_data + width_block_pad;

  FPVEC mask1(width_block_data, 1.0);
  FPVEC mask2(width_block_data, 0.0);
  // if (num_block == 1) width_block_pad = width_block_data;
  std::cout << "width_block_data=" << width_block_data
            << " width_block_pad=" << width_block_pad << std::endl;
  for (int i = width_block_data - width_block_pad; i < width_block_data; i++)
    mask2[i] = 1.0;

  std::vector<int64_t> mask_shape{width_block_data};
  CONSTANT_PTR         mask1_const =
      New_array_const(gscope, "mask1", width_block_data, etype, mask_shape,
                      (void*)mask1.data(), spos);
  CONSTANT_PTR mask2_const =
      New_array_const(gscope, "mask2", width_block_data, etype, mask_shape,
                      (void*)mask2.data(), spos);

  NODE_PTR init_m1;
  if (need_mask) {
    // reduce style across "data" of blocks in a grid
    init_m1 = New_mul(_cntr->New_ld(result_var, spos),
                      _cntr->New_ldc(mask1_const, spos), spos);
  } else {
    init_m1 = _cntr->New_ld(result_var, spos);
  }

  PREG_PTR epi_preg   = fscope->New_preg(init_m1->Rtype());
  STMT_PTR init_store = _cntr->New_stp(init_m1, epi_preg, spos);

  _ctx.Prepend(init_store);
  if (num_block > 1) {
    STMT_PTR collecitve_data_loop =
        New_loop("num_block_collecitve_data", 1, num_block, spos);
    STMT_LIST collecitve_data_sl = STMT_LIST::Enclosing_list(
        collecitve_data_loop->Node()->Child(3)->End_stmt());

    std::vector<int> roll_num_m1;
    for (int i = 1; i < num_block; i++) roll_num_m1.push_back(i * width_block);
    NODE_PTR roll_val_node_m1 = _cntr->New_bin_arith(
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::MUL), s32_type,
        _cntr->New_ld(collecitve_data_loop->Node()->Iv(), spos),
        _cntr->New_intconst(s32_type, width_block, spos), spos);

    NODE_PTR roll_node_m1 = New_roll(_cntr->New_ld(result_var, spos),
                                     roll_val_node_m1, roll_num_m1, spos);
    NODE_PTR vmul_node_m1 =
        New_mul(roll_node_m1, _cntr->New_ldc(mask1_const, spos), spos);
    NODE_PTR add_node_m1 =
        New_add(_cntr->New_ldp(epi_preg, spos), vmul_node_m1, spos);
    STMT_PTR add_store_m1 = _cntr->New_stp(add_node_m1, epi_preg, spos);
    collecitve_data_sl.Append(add_store_m1);
    _ctx.Prepend(collecitve_data_loop);

    // allreduce style across "gap" of blocks in a grid
    STMT_PTR collecitve_gap_loop =
        New_loop("num_block_collecitve_gap", 0, num_block - 1, spos);
    STMT_LIST collecitve_gap_sl = STMT_LIST::Enclosing_list(
        collecitve_gap_loop->Node()->Child(3)->End_stmt());
    std::vector<int> roll_num_m2;
    for (int i = 0; i < num_block - 1; i++)
      roll_num_m2.push_back(width_block_pad + i * width_block);
    NODE_PTR roll_val_node_m2 = _cntr->New_bin_arith(
        air::core::OPCODE::ADD, s32_type,
        _cntr->New_bin_arith(
            air::base::OPCODE(air::core::CORE, air::core::OPCODE::MUL),
            s32_type, _cntr->New_ld(collecitve_gap_loop->Node()->Iv(), spos),
            _cntr->New_intconst(s32_type, width_block, spos), spos),
        _cntr->New_intconst(s32_type, width_block_pad, spos), spos);

    NODE_PTR roll_node_m2 = New_roll(_cntr->New_ld(result_var, spos),
                                     roll_val_node_m2, roll_num_m2, spos);
    NODE_PTR vmul_node_m2 =
        New_mul(roll_node_m2, _cntr->New_ldc(mask2_const, spos), spos);
    NODE_PTR add_node_m2 =
        New_add(_cntr->New_ldp(epi_preg, spos), vmul_node_m2, spos);
    STMT_PTR add_store_m2 = _cntr->New_stp(add_node_m2, epi_preg, spos);
    collecitve_gap_sl.Append(add_store_m2);
    _ctx.Prepend(collecitve_gap_loop);
  }

  // tail block
  std::vector<int> roll_num_tail(1, -width_block_data);
  NODE_PTR         roll_result =
      New_roll(_cntr->New_ld(result_var, spos),
               _cntr->New_intconst(s32_type, -width_block_data, spos),
               roll_num_tail, spos);

  NODE_PTR fini_add2;
  if (need_mask) {
    NODE_PTR fini_m2 =
        New_mul(roll_result, _cntr->New_ldc(mask2_const, spos), spos);
    fini_add2 = New_add(_cntr->New_ldp(epi_preg, spos), fini_m2, spos);
  } else {
    fini_add2 = New_add(_cntr->New_ldp(epi_preg, spos), roll_result, spos);
  }

  STMT_PTR init_store2 = _cntr->New_stp(fini_add2, epi_preg, spos);
  _ctx.Prepend(init_store2);
  return epi_preg;
}

PREG_PTR TENSOR2VECTOR_UTIL::Gen_collective_reduce_stmt(
    ADDR_DATUM_PTR result_var, TYPE_PTR etype, const SPOS& spos,
    int width_block_data, int output_size, bool need_mask) {
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();

  // corner case
  if (output_size == _ctx.Get_slot()) {
    _ctx.Trace(TF_LOWER,
               "Gen_collective_reduce_stmt: output_size == _ctx.Get_slot() ",
               output_size, "\n");
    if (need_mask) {
      FPVEC                mask(width_block_data, 1.0);
      std::vector<int64_t> mask_shape{width_block_data};
      CONSTANT_PTR         mask_const =
          New_array_const(gscope, "mask", width_block_data, etype, mask_shape,
                          (void*)mask.data(), spos);
      NODE_PTR init_m1 = New_mul(_cntr->New_ld(result_var, spos),
                                 _cntr->New_ldc(mask_const, spos), spos);

      PREG_PTR epi_preg   = fscope->New_preg(result_var->Type());
      STMT_PTR init_store = _cntr->New_stp(init_m1, epi_preg, spos);
      _ctx.Prepend(init_store);
      return epi_preg;
    } else {
      PREG_PTR epi_preg = fscope->New_preg(result_var->Type());
      STMT_PTR init_store =
          _cntr->New_stp(_cntr->New_ld(result_var, spos), epi_preg, spos);
      _ctx.Prepend(init_store);
      return epi_preg;
    }
  }

  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  std::vector<int> roll_num_tail(1, -width_block_data);
  NODE_PTR         roll_result =
      New_roll(_cntr->New_ld(result_var, spos),
               _cntr->New_intconst(s32_type, -width_block_data, spos),
               roll_num_tail, spos);

  NODE_PTR fini_add =
      New_add(_cntr->New_ld(result_var, spos), roll_result, spos);

  NODE_PTR init_m1;
  if (need_mask) {
    FPVEC mask(width_block_data, 1.0);
    std::cout << "width_block_data=" << width_block_data << std::endl;

    std::vector<int64_t> mask_shape{width_block_data};
    CONSTANT_PTR         mask_const =
        New_array_const(gscope, "mask", width_block_data, etype, mask_shape,
                        (void*)mask.data(), spos);

    init_m1 = New_mul(fini_add, _cntr->New_ldc(mask_const, spos), spos);
  } else {
    init_m1 = fini_add;
  }

  PREG_PTR epi_preg   = fscope->New_preg(init_m1->Rtype());
  STMT_PTR init_store = _cntr->New_stp(init_m1, epi_preg, spos);

  _ctx.Prepend(init_store);

  return epi_preg;
}

NODE_PTR TENSOR2VECTOR_UTIL::Gen_mask_node(int64_t valid_len, TYPE_PTR etype,
                                           float val, const SPOS& spos) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  _ctx.Trace(TF_LOWER, "clean 0: len_mask=", valid_len, "\n");
  FPVEC                mask(valid_len, val);
  std::vector<int64_t> mask_shape{valid_len};
  CONSTANT_PTR         mask_const =
      New_array_const(gscope, "clear_mask_n", _ctx.Get_num_vloop(), valid_len,
                      etype, mask_shape, (void*)mask.data(), spos);
  NODE_PTR mask_node = _cntr->New_ldc(mask_const, spos);
  return mask_node;
}

void TENSOR2VECTOR_UTIL::Gen_clear_data_stmt(ADDR_DATUM_PTR input_var,
                                             int64_t valid_len, TYPE_PTR etype,
                                             const SPOS& spos) {
  NODE_PTR mask_node = Gen_mask_node(valid_len, etype, 1, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, mask_node->Const(),
                 "clean 0 mask");

  NODE_PTR vmulc_node =
      New_mul(_cntr->New_ld(input_var, spos), mask_node, spos);
  STMT_PTR vmulc_stmt = _cntr->New_st(vmulc_node, input_var, spos);
  _ctx.Prepend(vmulc_stmt);
}

void TENSOR2VECTOR_UTIL::Gen_clear_data_stmt(NODE_PTR    input_node,
                                             const SPOS& spos) {
  ARRAY_TYPE_PTR       input_type = input_node->Rtype()->Cast_to_arr();
  std::vector<int64_t> shape      = input_type->Shape();
  int64_t              channel    = shape[1];
  int64_t              ih         = shape[2];
  int64_t              iw         = shape[3];
  FPVEC                cz_mask    = Get_gap_mask(channel, ih, iw, false);

  std::vector<int64_t> cz_mask_shape{channel * ih * iw};
  CONSTANT_PTR         cz_mask_const = New_array_const(
      _cntr->Glob_scope(), "clear_zero_mask", channel * ih * iw,
      input_type->Elem_type(), cz_mask_shape, (void*)cz_mask.data(), spos);
  NODE_PTR cz_mask_node = _cntr->New_ldc(cz_mask_const, spos);

  NODE_PTR cz_result_node = New_mul(input_node, cz_mask_node, spos);
  STMT_PTR cz_result_stmt =
      _cntr->New_st(cz_result_node, input_node->Addr_datum(), spos);
  _ctx.Prepend(cz_result_stmt);
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_var(std::string var_name, TYPE_PTR vtype,
                                           const SPOS& spos) {
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  std::string    var_str    = var_name + std::to_string(_ctx.Get_num_vloop());
  ADDR_DATUM_PTR result_var = fscope->New_var(vtype, var_str.c_str(), spos);
  return result_var;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_st_0_to_var_stmt(std::string var_name,
                                                        TYPE_PTR    vtype,
                                                        const SPOS& spos) {
  NODE_PTR       zero_node  = _cntr->New_zero(vtype, spos);
  ADDR_DATUM_PTR result_var = Gen_var(var_name, vtype, spos);
  STMT_PTR       st_stmt    = _cntr->New_st(zero_node, result_var, spos);
  _ctx.Prepend(st_stmt);

  return result_var;
}

void TENSOR2VECTOR_UTIL::Gen_st_0_to_var_stmt(ADDR_DATUM_PTR var,
                                              const SPOS&    spos) {
  NODE_PTR zero_node = _cntr->New_zero(var->Type(), spos);
  STMT_PTR st_stmt   = _cntr->New_st(zero_node, var, spos);
  _ctx.Prepend(st_stmt);
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_var(
    std::string var_name, std::string ty_name, ARRAY_TYPE_PTR ty_arr,
    const std::vector<int64_t>& var_shape, const SPOS& spos) {
  TYPE_PTR arr_type = New_array_type(_cntr->Glob_scope(), ty_name,
                                     ty_arr->Elem_type(), var_shape, spos);
  return Gen_var(var_name, arr_type, spos);
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_st_0_to_var_stmt(
    std::string var_name, std::string ty_name, ARRAY_TYPE_PTR ty_arr,
    const std::vector<int64_t>& var_shape, const SPOS& spos) {
  TYPE_PTR arr_type = New_array_type(_cntr->Glob_scope(), ty_name,
                                     ty_arr->Elem_type(), var_shape, spos);
  return Gen_st_0_to_var_stmt(var_name, arr_type, spos);
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
                                            const SPOS& spos, int outer) {
  NODE_PTR dup_node = Gen_dup_input_node(input_node, dup_num, input_len, spos);
  STMT_PTR input_dup_stmt = _cntr->New_st(dup_node, result_var, spos);
  if (outer)
    _ctx.Get_insertpoint(outer).Append(input_dup_stmt);
  else
    _ctx.Prepend(input_dup_stmt);
}

void TENSOR2VECTOR_UTIL::Gen_loop_roll_add_stmt(const char* loop_name,
                                                int loop_ub, int elem_interval,
                                                NODE_PTR    input_node,
                                                const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  STMT_PTR  loop_stmt = New_loop(loop_name, 0, loop_ub, spos);
  STMT_LIST body_sl =
      STMT_LIST::Enclosing_list(loop_stmt->Node()->Child(3)->End_stmt());

  // input_roll = roll(input, pow(2, i)); pow(2,i) -> (1<< i)
  NODE_PTR shl_node = _cntr->New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::SHL), s32_type,
      _cntr->New_intconst(s32_type, 1, spos),
      _cntr->New_ld(loop_stmt->Node()->Iv(), spos), spos);

  NODE_PTR roll_val_node;
  if (elem_interval != 1) {
    roll_val_node = _cntr->New_bin_arith(
        air::core::OPCODE::MUL, s32_type, shl_node,
        _cntr->New_intconst(s32_type, elem_interval, spos), spos);
  } else {
    roll_val_node = shl_node;
  }

  std::vector<int> roll_num;
  for (int i = 0; i < loop_ub; i++) {
    roll_num.push_back(pow(2, i) * elem_interval);
  }
  NODE_PTR roll_node = New_roll(input_node, roll_val_node, roll_num, spos);
  NODE_PTR add_node  = New_add(input_node, roll_node, spos);
  STMT_PTR add_store;
  if (input_node->Opcode() ==
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::LDP)) {
    add_store = _cntr->New_stp(add_node, input_node->Preg(), spos);
  } else {
    add_store = _cntr->New_st(add_node, input_node->Addr_datum(), spos);
  }
  body_sl.Append(add_store);
  _ctx.Prepend(loop_stmt);
}

void TENSOR2VECTOR_UTIL::Gen_loop_combine_stmt(
    const char* loop_name, int loop_ub, int elem_interval,
    ADDR_DATUM_PTR input_var, NODE_PTR mask_node, int64_t mask_slice_len,
    ADDR_DATUM_PTR result_var, const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  Gen_st_0_to_var_stmt(result_var, spos);

  STMT_PTR  loop_stmt = New_loop(loop_name, 0, loop_ub, spos);
  STMT_LIST body_sl =
      STMT_LIST::Enclosing_list(loop_stmt->Node()->Child(3)->End_stmt());

  NODE_PTR roll_val_node = _cntr->New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::MUL), s32_type,
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

void TENSOR2VECTOR_UTIL::Gen_loops_combine_stmt(
    std::string loop_name, int loop_ub1, int loop_ub2, int iw, int stride,
    ADDR_DATUM_PTR input_var, NODE_PTR mask_node, int64_t mask_slice_len,
    ADDR_DATUM_PTR result_var, const SPOS& spos) {
  int            ow = iw / stride;
  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  std::string out_ln        = loop_name + "_out";
  STMT_PTR    out_loop_stmt = New_loop(out_ln.c_str(), 0, loop_ub1, spos);
  STMT_LIST   out_body_sl =
      STMT_LIST::Enclosing_list(out_loop_stmt->Node()->Child(3)->End_stmt());
  std::string in_ln        = loop_name + "_in";
  STMT_PTR    in_loop_stmt = New_loop(in_ln.c_str(), 0, loop_ub2, spos);
  STMT_LIST   in_body_sl =
      STMT_LIST::Enclosing_list(in_loop_stmt->Node()->Child(3)->End_stmt());

  NODE_PTR outl_iv = _cntr->New_ld(out_loop_stmt->Node()->Iv(), spos);
  NODE_PTR inl_iv  = _cntr->New_ld(in_loop_stmt->Node()->Iv(), spos);

  // compute roll val, i*(iw + ow) + j*stride
  NODE_PTR out_mul_node =
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type, outl_iv,
                           _cntr->New_intconst(s32_type, iw + ow, spos), spos);
  NODE_PTR in_mul_node =
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type, inl_iv,
                           _cntr->New_intconst(s32_type, stride, spos), spos);
  NODE_PTR roll_val_node = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32_type, out_mul_node, in_mul_node, spos);

  std::vector<int> roll_num;
  for (int i = 0; i < loop_ub1; i++) {
    for (int j = 0; j < loop_ub2; j++) {
      roll_num.push_back(i * (iw + ow) + j * stride);
    }
  }
  NODE_PTR roll_node =
      New_roll(_cntr->New_ld(input_var, spos), roll_val_node, roll_num, spos);

  // compute mask slice index
  // i*loop_ub2 + j
  NODE_PTR si_out_node =
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type, outl_iv,
                           _cntr->New_intconst(s32_type, loop_ub2, spos), spos);
  NODE_PTR slice_index_node = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32_type, si_out_node, inl_iv, spos);
  //
  NODE_PTR mask_slice =
      New_slice(mask_node, slice_index_node,
                _cntr->New_intconst(s32_type, mask_slice_len, spos), spos);
  NODE_PTR mul_node = New_mul(roll_node, mask_slice, spos);

  NODE_PTR add_node  = New_add(_cntr->New_ld(result_var, spos), mul_node, spos);
  STMT_PTR add_store = _cntr->New_st(add_node, result_var, spos);
  in_body_sl.Append(add_store);
  out_body_sl.Append(in_loop_stmt);
  _ctx.Prepend(out_loop_stmt);
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
  NODE_PTR cond_node  = _cntr->New_bin_arith(air::core::OPCODE::LT, s32_type,
                                             index_node, upper_node, spos);
  NODE_PTR incr_node =
      _cntr->New_bin_arith(air::core::OPCODE::ADD, s32_type, index_node,
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
  ADDR_DATUM_PTR tmp_result = Gen_st_0_to_var_stmt("tmp_result_n", vtype, spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // input_dup = input + roll(input, -output_height*output_width)
  // duplicate input value several times to make sure later roll works well
  int     dup_num = static_cast<int>(ceil(1.0 * channel_out / channel_in)) + 1;
  int64_t input_size = channel_in * output_height * output_width;
  if (Is_power_of_two(input_size)) {
    int t = this->_ctx.Get_slot() / input_size;
    if (dup_num > t) dup_num = t;
  }

  if (channel_in * output_height * output_width * 2 > this->_ctx.Get_slot()) {
    dup_num = 1;  // no need duplicate
  }

  AIR_ASSERT_MSG(channel_in * output_height * output_width * dup_num <=
                     this->_ctx.Get_slot(),
                 "after duplicate input, size should <= slot");
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
      air::core::OPCODE::ADD, s32_type,
      _cntr->New_ld(loop2_stmt->Node()->Iv(), spos),
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type,
                           _cntr->New_ld(loop1_stmt->Node()->Iv(), spos),
                           _cntr->New_intconst(s32_type, kernel_hw, spos),
                           spos),
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
 * @brief IRMA blocking: ra alignment
 */

NODE_PTR TENSOR2VECTOR_UTIL::Blocking_rot(NODE_PTR input, std::vector<int> ra,
                                          int input_size, int num_dup, int bs,
                                          const SPOS& spos, int outer) {
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
  // Get and check input type
  AIR_ASSERT_MSG((ra.size() == bs), "ra.size() == bs");
  AIR_ASSERT_MSG(input->Rtype()->Is_array(), "Input is not an array type");
  ARRAY_TYPE_PTR input_ty_arr = input->Rtype()->Cast_to_arr();

  // Build var: result, result_grid, input_dup
  TYPE_PTR vtype = input_ty_arr;

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // 1. Generate input data in a grid:
  // Array: input_grid[bs][width_block * num_block]
  // input_dup_var = replicate(input, num_dup_input)
  STMT_PTR comment_stmt = _cntr->New_comment(
      (std::string("BlockingRot replicate=") + std::to_string(num_dup)).c_str(),
      spos);
  _ctx.Prepend(comment_stmt);
  Gen_dup_input_stmt(input, num_dup, input_size, input_dup_var, spos, outer);
  // 2) IR Gen for Code:
  // for i in range(0, bs):
  //   input_grid_var[i] = roll(input_dup_var, ra[i])
  comment_stmt = _cntr->New_comment(
      (std::string("BlockingRot HRot=bs=") + std::to_string(bs)).c_str(), spos);
  _ctx.Prepend(comment_stmt);
  STMT_PTR  input_grid_loop = New_loop("bs_input", 0, bs, spos);
  STMT_LIST input_grid_sl =
      STMT_LIST::Enclosing_list(input_grid_loop->Node()->Child(3)->End_stmt());

  std::vector<int64_t> input_grid_shape(1, bs);
  TYPE_PTR             input_grid_type =
      New_array_type(gscope, "type_input_grid", _ctx.Get_num_vloop(), vtype,
                     input_grid_shape, spos);

  std::string input_grid_str =
      (std::string("input_block_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_grid_var =
      fscope->New_var(input_grid_type, input_grid_str.c_str(), spos);

  // input_grid_var[i]
  NODE_PTR input_grid_i = _cntr->New_array(
      _cntr->New_lda(input_grid_var, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(input_grid_i, 0,
                       _cntr->New_ld(input_grid_loop->Node()->Iv(), spos));
  // ra[i]
  TYPE_PTR     ra_type  = New_array_type(gscope, "ra_int", _ctx.Get_num_vloop(),
                                         s32_type, input_grid_shape, spos);
  CONSTANT_PTR ra_const = gscope->New_const(CONSTANT_KIND::ARRAY, ra_type,
                                            (void*)(ra.data()), 4 * ra.size());
  _ctx.Trace_cmd(TF_LOWER, Trace_int_array, ra_const, "conv ra_const");

  NODE_PTR ra_array = _cntr->New_array(
      _cntr->New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(ra_array, 0,
                       _cntr->New_ld(input_grid_loop->Node()->Iv(), spos));
  NODE_PTR ild_ra = _cntr->New_ild(ra_array, spos);

  // input_grid_var[i] = roll(input_dup_var, ra[i])
  NODE_PTR input_grid_roll_i =
      New_roll(_cntr->New_ld(input_dup_var, spos), ild_ra, ra, spos);
  STMT_PTR input_grid_assign =
      _cntr->New_ist(input_grid_i, input_grid_roll_i, spos);

  input_grid_sl.Append(input_grid_assign);
  if (outer)
    _ctx.Get_insertpoint(outer).Append(input_grid_loop);
  else
    _ctx.Prepend(input_grid_loop);

  NODE_PTR lda_node =
      _cntr->New_lda(input_grid_var, POINTER_KIND::FLAT32, spos);
  return lda_node;
}

/**
 * @brief Generate vector IR for conv according to im2col strategy.
 * which maps conv to matrix multiply.
 */
NODE_PTR TENSOR2VECTOR_UTIL::New_conv_metakernel_fast(
    NODE_PTR input2d, NODE_PTR weight, NODE_PTR bias, std::vector<int> ra,
    int channel_in, int channel_out, int output_height, int output_width,
    int kernel_hw, int group, int stride, const SHARD_MAP_PARAMS* params,
    bool need_mask, NODE_PTR new_offset, const SPOS& spos) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  int input_size  = channel_in * output_height * output_width;
  int output_size = channel_out * output_height * output_width;
  int num_grid    = params->_num_grid;
  int num_block   = params->_num_block;

  int width_block      = params->_width_block;
  int width_block_data = params->_width_block_data;
  int width_block_pad  = params->_width_block_pad;
  int cap_block        = params->_cap_block;
  int position_block   = params->_position_block;
  int num_dup_input    = params->_num_dup_input;

  int64_t num_slots = _ctx.Get_slot();

  // Get and check input type: lda input2d -> vector of vector
  AIR_ASSERT_MSG((input2d->Opcode() == air::core::OPC_LDA),
                 "Input is not an LDA node");
  ARRAY_TYPE_PTR       type2d  = input2d->Addr_datum()->Type()->Cast_to_arr();
  std::vector<int64_t> shape2d = type2d->Shape();
  AIR_ASSERT_MSG(((shape2d.size() == 1) && (shape2d[0] == cap_block)),
                 "input2d shape check");

  ARRAY_TYPE_PTR type2d_elem = type2d->Elem_type()->Cast_to_arr();

  CONSTANT_PTR weight_const = weight->Const();

  // Build var: result, result_grid, input_dup
  std::vector<int64_t> result_shape{1, channel_out, output_height,
                                    output_width};
  TYPE_PTR vtype = New_array_type(gscope, "type_result_n", _ctx.Get_num_vloop(),
                                  type2d_elem->Elem_type(), result_shape, spos);

  // VECTOR result_var = 0
  ADDR_DATUM_PTR result_var = Gen_st_0_to_var_stmt("result_n", vtype, spos);

  std::string result_grid_str =
      (std::string("result_grid_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR result_grid_var =
      fscope->New_var(vtype, result_grid_str.c_str(), spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // 3) IR Gen for Code: grid-block two-level loop
  // Here MetaKernel is block-level computation.
  // for i in range(0, num_grid):
  //   result_grid = zeros(num_slots)
  //   for j in range(0, cap_block):
  //     result_grid += input_grid_var[j] * weight[i*cap_block+j]
  //   result_var += roll(result_grid, i*position_block)
  STMT_PTR  grid_loop = New_loop("num_grid", 0, num_grid, spos);
  STMT_LIST grid_sl =
      STMT_LIST::Enclosing_list(grid_loop->Node()->Child(3)->End_stmt());

  // result_grid = zeros(num_slots)
  STMT_PTR st0_result_grid_stmt =
      _cntr->New_st(_cntr->New_zero(vtype, spos), result_grid_var, spos);
  grid_sl.Append(st0_result_grid_stmt);

  STMT_PTR  cap_block_loop = New_loop("cap_block", 0, cap_block, spos);
  STMT_LIST cap_block_sl =
      STMT_LIST::Enclosing_list(cap_block_loop->Node()->Child(3)->End_stmt());

  // weight[i*cap_block+j]
  NODE_PTR slice_index_node = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32_type,
      _cntr->New_ld(cap_block_loop->Node()->Iv(), spos),
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type,
                           _cntr->New_ld(grid_loop->Node()->Iv(), spos),
                           _cntr->New_intconst(s32_type, cap_block, spos),
                           spos),
      spos);

  if (new_offset != air::base::Null_ptr) {
    // sharding
    slice_index_node = _cntr->New_bin_arith(air::core::OPCODE::ADD, s32_type,
                                            new_offset, slice_index_node, spos);
  }
  NODE_PTR weight_grid = New_slice(
      weight, slice_index_node,
      _cntr->New_intconst(s32_type, num_block * width_block, spos), spos);

  // input_grid_var[j]
  NODE_PTR input_array2 = _cntr->New_array(input2d, 1, spos);
  _cntr->Set_array_idx(input_array2, 0,
                       _cntr->New_ld(cap_block_loop->Node()->Iv(), spos));
  NODE_PTR ild_input = _cntr->New_ild(input_array2, spos);

  // result_var += input_grid_var[j] * weight[i*cap_block+j]
  NODE_PTR vmul_node = New_mul(ild_input, weight_grid, spos);
  NODE_PTR vadd_node =
      New_add(_cntr->New_ld(result_grid_var, spos), vmul_node, spos);
  STMT_PTR vadd_store = _cntr->New_st(vadd_node, result_grid_var, spos);

  cap_block_sl.Append(vadd_store);

  NODE_PTR cin_add_node = _cntr->New_ld(result_grid_var, spos);

  // special case: introcude Roll_cyclic. the left-roll destroy existed data
  // The condition is strict. TODO :)
  bool is_cyclic = (num_block == 1) && (output_size > num_slots / 2) &&
                   (output_size < num_slots) && (group != channel_in);
  _ctx.Trace(TF_LOWER, "is_cyclic=", is_cyclic, " output_size=", output_size,
             "\n");

  NODE_PTR cin_roll_node2;
  if (is_cyclic) {
    _ctx.Trace(TF_LOWER, "Roll_cyclic: output_size=", output_size,
               " position_block=", position_block, "\n");
    cin_roll_node2 = Roll_cyclic(result_grid_var, output_size, position_block,
                                 num_grid, grid_loop->Node()->Iv(), spos);
  } else {
    // result_var += roll(result_grid, i*position_block)
    std::vector<int> roll_num_left;
    for (int i = 0; i < num_grid; i++)
      roll_num_left.push_back(i * position_block);
    cin_roll_node2 =
        New_roll(cin_add_node,
                 _cntr->New_bin_arith(
                     air::core::OPCODE::MUL, s32_type,
                     _cntr->New_ld(grid_loop->Node()->Iv(), spos),
                     _cntr->New_intconst(s32_type, position_block, spos), spos),
                 roll_num_left, spos);
  }

  NODE_PTR cin_add_node2 =
      New_add(_cntr->New_ld(result_var, spos), cin_roll_node2, spos);
  STMT_PTR cin_add_st = _cntr->New_st(cin_add_node2, result_var, spos);

  grid_sl.Append(cap_block_loop);
  grid_sl.Append(cin_add_st);
  _ctx.Prepend(grid_loop);

  STMT_PTR vadd_bias_stmt;
  if (((channel_in > 1) && (group != channel_in) && !is_cyclic) ||
      (num_slots == output_size)) {
    // Here we get results of all blocks in a vecotr. Reduce it.
    // num_block == 1: 2xslots for rotate-left. so width_block_pad:
    PREG_PTR epi_preg;
    if ((!_ctx.Conv_parallel()) && (!_ctx.Sharding()) && (num_block == 1)) {
      epi_preg =
          Gen_collective_reduce_stmt(result_var, type2d_elem->Elem_type(), spos,
                                     width_block_data, output_size, need_mask);
    } else {
      if (num_block == 1) {
        width_block_pad = (channel_in - 1) * output_height * output_width;
      }
      epi_preg = Gen_collective_reduce_stmt(
          result_var, type2d_elem->Elem_type(), spos, num_block,
          width_block_data, width_block_pad, output_size, need_mask);
    }
    vadd_bias_stmt = _cntr->New_st(
        New_add(_cntr->New_ldp(epi_preg, spos), bias, spos), result_var, spos);
  } else {
    vadd_bias_stmt = _cntr->New_st(
        New_add(_cntr->New_ld(result_var, spos), bias, spos), result_var, spos);
  }

  _ctx.Prepend(vadd_bias_stmt);

  NODE_PTR ld_result = _cntr->New_ld(result_var, spos);
  return ld_result;
}

/**
 * @brief Generate gemm metakernel which can be templated into conv2d and
 * gemm in Tensor IR.
 * TODO: params for template; metakernel as a funciton.
 *
 * @param weight_diag is ldc const weight after rotating
 * @param bias is ldc const bias
 */
ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::New_gemm_metakernel_fast(
    NODE_PTR input2d, NODE_PTR weight_diag, NODE_PTR bias, int64_t np,
    int64_t kp, int bs, int gs, const SPOS& spos) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  int64_t num_slots = _ctx.Get_slot();

  // Get and check input type: lda input2d -> vector of vector
  AIR_ASSERT_MSG((input2d->Opcode() == air::core::OPC_LDA),
                 "input2d is not an LDA node");
  ARRAY_TYPE_PTR       type2d  = input2d->Addr_datum()->Type()->Cast_to_arr();
  std::vector<int64_t> shape2d = type2d->Shape();
  AIR_ASSERT_MSG(((shape2d.size() == 1) && (shape2d[0] == bs)),
                 "input2d shape check");

  TYPE_PTR etype = type2d->Elem_type()->Cast_to_arr()->Elem_type();

  // Get and check weight_diag type
  ARRAY_TYPE_PTR       weight_diag_type  = weight_diag->Rtype()->Cast_to_arr();
  std::vector<int64_t> weight_diag_shape = weight_diag_type->Shape();
  AIR_ASSERT_MSG(weight_diag_shape.size() == 2,
                 "weight_diag_type dim is not 2");
  int64_t              height = weight_diag_shape[0];
  int64_t              width  = weight_diag_shape[1];
  std::vector<int64_t> shape(1, width);

  _ctx.Trace(TF_LOWER, "m_irma shape: ", height, "x", width, "\n");

  TYPE_PTR vtype = New_array_type(gscope, "gemm_width", _ctx.Get_num_vloop(),
                                  etype, shape, spos);

  // VECTOR result = 0
  ADDR_DATUM_PTR result = Gen_st_0_to_var_stmt("gemm_result_n", vtype, spos);

  ADDR_DATUM_PTR block_result = fscope->New_var(vtype, "block_result", spos);

  CONST_TYPE_PTR s32t = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  STMT_PTR comment_stmt = _cntr->New_comment(
      (std::string("IMRA Metakernel: gs=") + std::to_string(gs)).c_str(), spos);
  _ctx.Prepend(comment_stmt);
  // Generate a 2-level loop for GEMM: blocks.
  STMT_PTR  loop_h2 = New_loop("gs", 0, gs, spos);
  STMT_LIST h2_sl   = Get_loop_sl(loop_h2);

  STMT_PTR  loop_h1 = New_loop("bs", 0, bs, spos);
  STMT_LIST h1_sl   = Get_loop_sl(loop_h1);

  // block_result = [0]*width
  NODE_PTR vzero_node     = _cntr->New_zero(vtype, spos);
  STMT_PTR st0_block_stmt = _cntr->New_st(vzero_node, block_result, spos);
  h2_sl.Append(st0_block_stmt);

  // block_result += input2d[0:h1] * weight_diag[i2*h1:i2*h1+h1]
  // weight_diag[i2*h1:i2*h1+h1]
  NODE_PTR slice_index = _cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32t, Get_loop_iv(loop_h1),
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32t, Get_loop_iv(loop_h2),
                           _cntr->New_intconst(s32t, bs, spos), spos),
      spos);

  NODE_PTR weight_slice = New_slice(
      weight_diag, slice_index, _cntr->New_intconst(s32t, width, spos), spos);

  // input2d[0:h1]
  NODE_PTR input_array = _cntr->New_array(input2d, 1, spos);
  _cntr->Set_array_idx(input_array, 0, Get_loop_iv(loop_h1));
  NODE_PTR ild_input = _cntr->New_ild(input_array, spos);

  NODE_PTR vmul_node = New_mul(ild_input, weight_slice, spos);
  NODE_PTR block_add =
      New_add(_cntr->New_ld(block_result, spos), vmul_node, spos);
  STMT_PTR block_add_st = _cntr->New_st(block_add, block_result, spos);

  h1_sl.Append(block_add_st);

  // result += roll(block_result, i2*h1)
  std::vector<int> roll_num_left;
  for (int i = 0; i < gs; i++) roll_num_left.push_back(i * bs);

  NODE_PTR block_roll = New_roll(
      _cntr->New_ld(block_result, spos),
      _cntr->New_bin_arith(air::core::OPCODE::MUL, s32t, Get_loop_iv(loop_h2),
                           _cntr->New_intconst(s32t, bs, spos), spos),
      roll_num_left, spos);
  NODE_PTR vadd_node2  = New_add(_cntr->New_ld(result, spos), block_roll, spos);
  STMT_PTR vadd_store2 = _cntr->New_st(vadd_node2, result, spos);

  h2_sl.Append(loop_h1);
  h2_sl.Append(vadd_store2);
  _ctx.Prepend(loop_h2);

  return result;
}

/**
 * @brief Generate gemm metakernel which can be templated into conv2d and
 * gemm in Tensor IR.
 * TODO: params for template; metakernel as a funciton.
 *
 * @param op1 is ldc const
 * @param op2 is ldc const
 */
NODE_PTR TENSOR2VECTOR_UTIL::New_gemm_metakernel(NODE_PTR op0, NODE_PTR op1,
                                                 NODE_PTR op2, bool need_mask,
                                                 const SPOS& spos) {
  _ctx.Incr_num_vloop();
  GLOB_SCOPE* gscope = _cntr->Glob_scope();
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

  // Get and check op0 type
  AIR_ASSERT_MSG(op0->Rtype()->Is_array(), "op0 is not an array type");
  ARRAY_TYPE_PTR op0_ty_arr = op0->Rtype()->Cast_to_arr();

  // std::vector<int64_t> op0_shape = op0_ty_arr->Shape();
  // AIR_ASSERT_MSG(((op0_shape.size() == 2) && (op0_shape[0] == 1)) ||
  //                    (op0_shape.size() == 1),
  //                "op0: shape=%d dim[0]=%d. 2D is work in progress",
  //                op0_shape.size(), op0_shape[0]);

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
  // may need duplicate
  // input_dup = input + roll(input, -width)
  int dup_num = 2;
  if (width == _ctx.Get_slot()) {
    dup_num = 1;  // no need duplicate
  }
  std::vector<int64_t> shape(1, dup_num * width);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, op1_const, "gemm op1_diag");

  TYPE_PTR vtype = New_array_type(gscope, "tmp", _ctx.Get_num_vloop(),
                                  op0_ty_arr->Elem_type(), shape, spos);

  // VECTOR tmp_result = 0
  ADDR_DATUM_PTR tmp_result = Gen_st_0_to_var_stmt("tmp_result_n", vtype, spos);

  std::string dup_str =
      (std::string("input_dup_n") + std::to_string(_ctx.Get_num_vloop()));
  ADDR_DATUM_PTR input_dup_var = fscope->New_var(vtype, dup_str.c_str(), spos);

  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  Gen_dup_input_stmt(op0, dup_num, (int)width, input_dup_var, spos);

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
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::SHL), s32_type,
        _cntr->New_intconst(s32_type, 1, spos),
        _cntr->New_ld(loop_blockadd_stmt->Node()->Iv(), spos), spos);
    NODE_PTR mul_node =
        _cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type, shl_node,
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

  if (need_mask) {
    Gen_clear_data_stmt(tmp_result, height, op0_ty_arr->Elem_type(), spos);
  }

  NODE_PTR ld_result = _cntr->New_ld(tmp_result, spos);

  return ld_result;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Gen_comb_in_channel(ADDR_DATUM_PTR input_var,
                                                       ARRAY_TYPE_PTR ty_arr,
                                                       int64_t        stride,
                                                       const SPOS&    spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  int64_t              oh      = ih / stride;
  int64_t              ow      = iw / stride;
  int64_t              cin_len = channel * ih * iw;
  // 1. combine data in channel
  // 1.1 prepare result, vector result = 0
  std::vector<int64_t> comb_dic_shape{cin_len};
  ADDR_DATUM_PTR       comb_dic_var = Gen_st_0_to_var_stmt(
      "comb_dic_result", "comb_dic_float", ty_arr, comb_dic_shape, spos);

  // 1.2 first log2 loop to make elements consecutive, no need masking
  int loop_ub  = log2(stride);
  int interval = stride - 1;
  Gen_loop_roll_add_stmt("comb_dic_index", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // 1.3 prepare compact all valid data in channel mask
  FPVEC dic_mask = Get_dic_comb_mask(channel, ih, iw, stride);

  int64_t              data_blocks = oh * ceil(double(ow) / stride);
  std::vector<int64_t> dic_mask_shape{data_blocks, cin_len};
  CONSTANT_PTR         dic_mask_const = New_array_const(
      _cntr->Glob_scope(), "dic_mask", data_blocks * cin_len,
      ty_arr->Elem_type(), dic_mask_shape, (void*)dic_mask.data(), spos);
  NODE_PTR dic_mask_node = _cntr->New_ldc(dic_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, dic_mask_node->Const(),
                 "dic mask");
  // 1.4 second loop to combine data in channel
  Gen_loops_combine_stmt("comb_dic_index", oh, ceil(double(ow) / stride), iw,
                         stride, input_var, dic_mask_node, cin_len,
                         comb_dic_var, spos);
  return comb_dic_var;
}

void TENSOR2VECTOR_UTIL::Gen_comb_row_base(ADDR_DATUM_PTR input_var,
                                           ADDR_DATUM_PTR result_var,
                                           ARRAY_TYPE_PTR ty_arr, int64_t ow,
                                           int64_t stride, const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  int64_t              cin_len = channel * ih * iw;

  // 1.2 prepare row combine mask
  FPVEC row_mask = Get_row_combine_mask(channel, ih, iw, 1, ow);

  std::vector<int64_t> row_mask_shape{ow, cin_len};
  CONSTANT_PTR         row_mask_const = New_array_const(
      _cntr->Glob_scope(), "row_mask", ow * cin_len, ty_arr->Elem_type(),
      row_mask_shape, (void*)row_mask.data(), spos);
  NODE_PTR row_mask_node = _cntr->New_ldc(row_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, row_mask_node->Const(),
                 "row mask");

  // 1.3 loop to combine row
  Gen_loop_combine_stmt("comb_row_index", ow, stride - 1, input_var,
                        row_mask_node, cin_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_comb_large_row(ADDR_DATUM_PTR input_var,
                                            ADDR_DATUM_PTR result_var,
                                            ARRAY_TYPE_PTR ty_arr,
                                            int64_t stride, const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  int64_t              ow      = iw / stride;
  int64_t              cin_len = channel * ih * iw;

  // 1.2 first log2 loop to make elements consecutive, no need masking
  int loop_ub  = log2(stride);
  int interval = stride - 1;
  Gen_loop_roll_add_stmt("comb_lrow_lf_index1", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // Make a mask to eliminate invalid data, preparing for 1.3
  FPVEC                lrow_mask = Get_large_row_mask(channel, ih, iw, stride);
  std::vector<int64_t> lrow_mask_shape{cin_len};
  CONSTANT_PTR         lrow_mask_const = New_array_const(
      _cntr->Glob_scope(), "lrow_mask", cin_len, ty_arr->Elem_type(),
      lrow_mask_shape, (void*)lrow_mask.data(), spos);
  NODE_PTR lrow_mask_node = _cntr->New_ldc(lrow_mask_const, spos);

  NODE_PTR mul_node =
      New_mul(_cntr->New_ld(input_var, spos), lrow_mask_node, spos);
  STMT_PTR mul_stmt = _cntr->New_st(mul_node, input_var, spos);
  _ctx.Prepend(mul_stmt);

  // 1.3 second log2 loop to make elements consecutive, no need masking
  loop_ub  = log2(stride);
  interval = stride;
  Gen_loop_roll_add_stmt("comb_row_lf_index2", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // 1.4 prepare combine in rows mask
  int   cvdc = stride * 2;               // cvdc is consecutive valid data count
  int   vdgn = ceil(double(ow) / cvdc);  // vdgn is the valid data group number
  FPVEC row_mask = Get_row_combine_mask(channel, ih, iw, cvdc, vdgn);

  std::vector<int64_t> row_mask_shape{vdgn, cin_len};
  CONSTANT_PTR         row_mask_const = New_array_const(
      _cntr->Glob_scope(), "row_mask", vdgn * cin_len, ty_arr->Elem_type(),
      row_mask_shape, (void*)row_mask.data(), spos);
  NODE_PTR row_mask_node = _cntr->New_ldc(row_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, row_mask_node->Const(),
                 "row mask");

  // 1.5 loop to combine row
  Gen_loop_combine_stmt("comb_row_index", vdgn, stride * 2, input_var,
                        row_mask_node, cin_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_comb_row_fast(ADDR_DATUM_PTR input_var,
                                           ADDR_DATUM_PTR result_var,
                                           ARRAY_TYPE_PTR ty_arr,
                                           int64_t stride, const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  int64_t              ow      = iw / stride;
  int64_t              cin_len = channel * ih * iw;

  // 1.2 first log2 loop to make elements consecutive, no need masking
  int loop_ub  = log2(stride);
  int interval = stride - 1;
  Gen_loop_roll_add_stmt("comb_row_lf_index", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // 1.3 prepare combine in rows mask
  int   cvdc = stride;                   // cvdc is consecutive valid data count
  int   vdgn = ceil(double(ow) / cvdc);  // vdgn is the valid data group number
  FPVEC row_mask = Get_row_combine_mask(channel, ih, iw, cvdc, vdgn);

  std::vector<int64_t> row_mask_shape{vdgn, cin_len};
  CONSTANT_PTR         row_mask_const = New_array_const(
      _cntr->Glob_scope(), "row_mask", vdgn * cin_len, ty_arr->Elem_type(),
      row_mask_shape, (void*)row_mask.data(), spos);
  NODE_PTR row_mask_node = _cntr->New_ldc(row_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, row_mask_node->Const(),
                 "row mask");

  // 1.4 loop to combine row
  Gen_loop_combine_stmt("comb_row_index", vdgn, stride, input_var,
                        row_mask_node, cin_len, result_var, spos);
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Comb_in_row(
    ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr, int64_t ow, int64_t stride,
    int64_t padsize, int64_t& interval, bool is_start_pos_valid,
    bool enb_row_fast, const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];

  ADDR_DATUM_PTR temp = input_var;
  if (is_start_pos_valid) {
    AIR_ASSERT(padsize == 0);
    interval = iw * (stride - 1) + (iw - iw / stride);
  } else {
    AIR_ASSERT((padsize == 1) || (padsize == 2) || (padsize == 3));
    interval = 2 * padsize;
    // lenet can generate strided_slice due to padding
    if ((stride & 1) == 0) {
      // (iw-padsize*2)/stride computes the number of valid data in one row
      interval = iw + (iw - (iw - padsize * 2) / stride);
    }
    temp = Mv_valid_to_start(input_var, ty_arr, padsize, spos);
  }

  // lenet can generate strided_slice due to padding
  ADDR_DATUM_PTR comb_row_ret = temp;
  if ((stride & 1) == 0) {
    // 1. combine in row
    // 1.1 prepare combine row result variaable, make it = 0
    std::vector<int64_t> ret_shape{channel * ih * iw};
    ADDR_DATUM_PTR       ret_var =
        Gen_var("comb_row_result", "comb_row_float", ty_arr, ret_shape, spos);

    if (enb_row_fast) {
      if (_ctx.Stride_slice_exp()) {
        Gen_comb_large_row(input_var, ret_var, ty_arr, stride, spos);
      } else {
        // TODO: when mask fuse optimization is enabled, fast implement may need
        // to do masking first.
        Gen_comb_row_fast(input_var, ret_var, ty_arr, stride, spos);
      }
    } else {
      Gen_comb_row_base(input_var, ret_var, ty_arr, ow, stride, spos);
    }
    comb_row_ret = ret_var;
  }

  return comb_row_ret;
}

void TENSOR2VECTOR_UTIL::Gen_comb_rc_base(ADDR_DATUM_PTR input_var,
                                          ADDR_DATUM_PTR result_var,
                                          ARRAY_TYPE_PTR ty_arr, int64_t oh,
                                          int64_t ow, int64_t interval,
                                          const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  int64_t              cin_len = channel * ih * iw;

  // 2.2 prepare row and column combine mask
  FPVEC rc_mask = Get_rc_combine_mask(channel, ih, iw, ow, oh, 0);

  std::vector<int64_t> rc_mask_shape{oh, cin_len};
  CONSTANT_PTR         rc_mask_const = New_array_const(
      _cntr->Glob_scope(), "rc_mask", oh * cin_len, ty_arr->Elem_type(),
      rc_mask_shape, (void*)rc_mask.data(), spos);
  NODE_PTR rc_mask_node = _cntr->New_ldc(rc_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, rc_mask_node->Const(),
                 "row column mask");

  // 2.3 second loop to combine row and column
  Gen_loop_combine_stmt("comb_rc_index", oh, interval, input_var, rc_mask_node,
                        cin_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_comb_rc_fast(ADDR_DATUM_PTR input_var,
                                          ADDR_DATUM_PTR result_var,
                                          ARRAY_TYPE_PTR ty_arr, int64_t stride,
                                          int64_t interval, const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];

  int64_t oh      = ih / stride;
  int64_t ow      = iw / stride;
  int64_t cin_len = channel * ih * iw;

  _ctx.Trace(TF_LOWER, "Gen_comb_rc_fast, channel: ", channel,
             "input height: ", ih, " input width:", iw, " interval: ", interval,
             " stride: ", stride, "\n");

  // 2.2 first log2 loop to make elements consecutive, no need masking
  int loop_ub = Get_log_rc_lub(interval, oh, ow);
  Gen_loop_roll_add_stmt("comb_rc_lf_index", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // 2.3 prepare row and column combine mask
  // cvdn is the number of consecutive valid data
  int cvdn      = ow + interval;
  int last_cvdn = (oh * ow) % cvdn;
  // vdgn is the number of valid data group
  int vdgn = ceil(1.0 * oh * ow / cvdn);
  if (loop_ub == 1) {
    cvdn = iw;
    vdgn = 1;  // all valid data is in the first row
  }
  FPVEC rc_mask = Get_rc_combine_mask(channel, ih, iw, cvdn, vdgn, last_cvdn);

  std::vector<int64_t> rc_mask_shape{vdgn, cin_len};
  CONSTANT_PTR         rc_mask_const = New_array_const(
      _cntr->Glob_scope(), "rc_mask", vdgn * cin_len, ty_arr->Elem_type(),
      rc_mask_shape, (void*)rc_mask.data(), spos);
  NODE_PTR rc_mask_node = _cntr->New_ldc(rc_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, rc_mask_node->Const(),
                 "row column mask");

  // 2.4 second loop to combine row and column
  int new_interval = (Next_power_of_two(ih / vdgn) - pow(2, loop_ub - 1)) * iw;
  Gen_loop_combine_stmt("comb_rc_index", vdgn, new_interval, input_var,
                        rc_mask_node, cin_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_comb_cc_base(ADDR_DATUM_PTR input_var,
                                          ADDR_DATUM_PTR result_var,
                                          ARRAY_TYPE_PTR ty_arr, int64_t oh,
                                          int64_t ow, const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  // 3.2 prepare channel combine mask
  FPVEC cc_mask = Get_cc_combine_mask(channel, ih, iw, oh * ow, channel, 0);

  int64_t              cin_len = channel * ih * iw;
  std::vector<int64_t> cc_mask_shape{channel, cin_len};
  CONSTANT_PTR         cc_mask_const = New_array_const(
      _cntr->Glob_scope(), "cc_mask", channel * cin_len, ty_arr->Elem_type(),
      cc_mask_shape, (void*)cc_mask.data(), spos);
  NODE_PTR cc_mask_node = _cntr->New_ldc(cc_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, cc_mask_node->Const(),
                 "cross channel mask");

  // 3.3 loop to combine cross channel
  Gen_loop_combine_stmt("comb_cc_index", channel, ih * iw - oh * ow, input_var,
                        cc_mask_node, cin_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_comb_large_channel(ADDR_DATUM_PTR input_var,
                                                ADDR_DATUM_PTR result_var,
                                                ARRAY_TYPE_PTR ty_arr,
                                                int64_t        stride,
                                                const SPOS&    spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  AIR_ASSERT(channel > 32 && Is_power_of_two(channel));
  if (stride > 2) {
    _ctx.Trace(TF_LOWER,
               "Gen_comb_large_channel is not worked for global average "
               "pool(stride > 2)\n");
    AIR_ASSERT((ih != stride) && (iw != stride));
  }
  // 3.2 first log2 loop to make elements consecutive, no need masking
  int     loop_ub   = Get_log_cc_lub(channel, ih, iw, stride);
  int64_t oh        = ih / stride;
  int64_t ow        = iw / stride;
  int64_t input_len = channel * ih * iw;

  int interval = ih * iw - oh * ow;
  Gen_loop_roll_add_stmt("comb_cc_lf_index1", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // Make a mask to eliminate invalid data, preparing for 3.3
  FPVEC lc_mask = Get_large_channel_mask(channel, ih, iw, stride);
  std::vector<int64_t> lc_mask_shape{input_len};
  CONSTANT_PTR         lc_mask_const = New_array_const(
      _cntr->Glob_scope(), "lcc_mask", input_len, ty_arr->Elem_type(),
      lc_mask_shape, (void*)lc_mask.data(), spos);
  NODE_PTR lc_mask_node = _cntr->New_ldc(lc_mask_const, spos);

  NODE_PTR mul_node =
      New_mul(_cntr->New_ld(input_var, spos), lc_mask_node, spos);
  STMT_PTR mul_stmt = _cntr->New_st(mul_node, input_var, spos);
  _ctx.Prepend(mul_stmt);

  // 3.3 second log2 loop to make elements consecutive, no need masking
  loop_ub  = stride;
  interval = ih * iw * (stride * stride - 1);
  Gen_loop_roll_add_stmt("comb_cc_lf_index2", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // 3.4 prepare cross channel combine mask
  int output_len = channel * oh * ow;
  // cvdn stands for the number of consecutive valid data number
  int cvdn      = ih * iw * (stride * stride);
  int last_cvdn = output_len % cvdn;
  // vdgn stands for the valid data group number
  int vdgn = ceil(1.0 * output_len / cvdn);
  if (loop_ub == 1) {
    cvdn = (ih * iw) / 2;
    vdgn = 1;  // all valid data is in the first channel
  }

  _ctx.Trace(TF_LOWER, "Gen_comb_large_channel, cvdn: ", cvdn,
             " last_cvdn: ", last_cvdn, " vdgn: ", vdgn, "\n");

  FPVEC cc_mask = Get_cc_combine_mask(channel, ih, iw, cvdn, vdgn, last_cvdn);

  std::vector<int64_t> cc_mask_shape{vdgn, input_len};
  CONSTANT_PTR         cc_mask_const = New_array_const(
      _cntr->Glob_scope(), "cc_mask", vdgn * input_len, ty_arr->Elem_type(),
      cc_mask_shape, (void*)cc_mask.data(), spos);
  NODE_PTR cc_mask_node = _cntr->New_ldc(cc_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, cc_mask_node->Const(),
                 "cross channel mask");

  int new_interval =
      (Next_power_of_two(channel / vdgn) - (stride * stride)) * ih * iw;
  _ctx.Trace(TF_LOWER, "Gen_comb_large_channel, new_interval: ", new_interval,
             "\n");
  // 3.5 second loop to combine cross channel
  Gen_loop_combine_stmt("comb_cc_index", vdgn, new_interval, input_var,
                        cc_mask_node, input_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_comb_cc_fast(ADDR_DATUM_PTR input_var,
                                          ADDR_DATUM_PTR result_var,
                                          ARRAY_TYPE_PTR ty_arr, int64_t stride,
                                          const SPOS& spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];

  // 3.2 first log2 loop to make elements consecutive, no need masking
  int loop_ub  = Get_log_cc_lub(channel, ih, iw, stride);
  int oh       = ih / stride;
  int ow       = iw / stride;
  int interval = ih * iw - oh * ow;
  Gen_loop_roll_add_stmt("comb_cc_lf_index", loop_ub, interval,
                         _cntr->New_ld(input_var, spos), spos);

  // 3.3 prepare cross channel combine mask
  int output_len = channel * oh * ow;
  // cvdn stands for the number of consecutive valid data number
  int cvdn      = ih * iw;
  int last_cvdn = output_len % cvdn;
  // vdgn stands for the valid data group number
  int vdgn = ceil(1.0 * output_len / cvdn);
  if (loop_ub == 1) {
    cvdn = (ih * iw) / 2;
    vdgn = 1;  // all valid data is in the first channel
  }

  _ctx.Trace(TF_LOWER, "Gen_comb_cc_fast, cvdn: ", cvdn,
             " last_cvdn: ", last_cvdn, " vdgn: ", vdgn, "\n");

  FPVEC cc_mask = Get_cc_combine_mask(channel, ih, iw, cvdn, vdgn, last_cvdn);

  int64_t              input_len = channel * ih * iw;
  std::vector<int64_t> cc_mask_shape{vdgn, input_len};
  CONSTANT_PTR         cc_mask_const = New_array_const(
      _cntr->Glob_scope(), "cc_mask", vdgn * input_len, ty_arr->Elem_type(),
      cc_mask_shape, (void*)cc_mask.data(), spos);
  NODE_PTR cc_mask_node = _cntr->New_ldc(cc_mask_const, spos);

  _ctx.Trace_cmd(TF_LOWER, Trace_float_array, cc_mask_node->Const(),
                 "cross channel mask");

  int new_interval = (Next_power_of_two(channel / vdgn) - 1) * ih * iw;
  _ctx.Trace(TF_LOWER, "Gen_comb_cc_fast, new_interval: ", new_interval, "\n");
  // 3.4 second loop to combine cross channel
  Gen_loop_combine_stmt("comb_cc_index", vdgn, new_interval, input_var,
                        cc_mask_node, input_len, result_var, spos);
}

void TENSOR2VECTOR_UTIL::Gen_log_based_comb_cc(ADDR_DATUM_PTR input_var,
                                               ADDR_DATUM_PTR result_var,
                                               ARRAY_TYPE_PTR ty_arr,
                                               int64_t        stride,
                                               const SPOS&    spos) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];
  int64_t              oh      = ih / stride;
  int64_t              ow      = iw / stride;
  // 3.2 only use rotate and add to combine and compact
  int      loop_ub       = ceil(log2(channel));
  int      elem_interval = ih * iw - oh * ow;
  NODE_PTR ld_input      = _cntr->New_ld(input_var, spos);
  Gen_loop_roll_add_stmt("comb_cc_index", loop_ub, elem_interval, ld_input,
                         spos);

  // 3.3 clear invalid data to avoid affecting subsequent computations.
  // TODO: consider how to eliminating this masking op correctly!
  if (!_ctx.Improve_ss_insert()) {
    // when enable improve_ss_insert, for lenet model, this masking is
    // useless. because later gemm duplicate width 1 time and junk data are
    // placed at index 1025: width = oh*ow*channel = 400; after expand, width
    // = 480; after duplicate 1 time: 480 * 2 = 960 < 1024
    Gen_clear_data_stmt(input_var, oh * ow * channel, ty_arr->Elem_type(),
                        spos);
  }

  STMT_PTR st_stmt = _cntr->New_st(ld_input, result_var, spos);
  _ctx.Prepend(st_stmt);
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Comb_cross_row_col(
    ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr, int64_t oh, int64_t ow,
    int64_t stride, int64_t interval, const SPOS& spos, bool enable_rc_fast) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];

  // 2. combine by rows and columns
  // 2.1 prepare combine row and column result, VECTOR result = 0
  std::vector<int64_t> ret_shape{channel * ih * iw};
  ADDR_DATUM_PTR       ret_var =
      Gen_var("comb_rc_result", "comb_rc_float", ty_arr, ret_shape, spos);

  if (enable_rc_fast) {
    Gen_comb_rc_fast(input_var, ret_var, ty_arr, stride, interval, spos);
  } else {
    Gen_comb_rc_base(input_var, ret_var, ty_arr, oh, ow, interval, spos);
  }
  return ret_var;
}

NODE_PTR TENSOR2VECTOR_UTIL::Comb_cross_channel(
    ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr, int64_t oh, int64_t ow,
    int64_t stride, const SPOS& spos, bool enable_cc_fast) {
  std::vector<int64_t> shape   = ty_arr->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];

  bool need_comb_cc = true;
  if (channel == 1) {
    _ctx.Trace(TF_LOWER, "no need to combine cross channel");
    need_comb_cc = false;
  }

  // 3.1 prepare combine cross channel result(final result), result = 0
  std::vector<int64_t> ret_shape{1, channel, oh, ow};
  TYPE_PTR             ret_type =
      _cntr->Glob_scope()->New_arr_type(ty_arr->Elem_type(), ret_shape, spos);

  NODE_PTR load_ret_node;
  if (need_comb_cc) {
    ADDR_DATUM_PTR ret_var = Gen_var("ss_result", ret_type, spos);

    if (Enable_log_cc_comb(channel, ih, iw, oh, ow)) {
      Gen_log_based_comb_cc(input_var, ret_var, ty_arr, stride, spos);
    } else if (enable_cc_fast) {
      if (_ctx.Stride_slice_exp() ||
          Enable_large_channel_comb(channel, stride)) {
        Gen_comb_large_channel(input_var, ret_var, ty_arr, stride, spos);
      } else {
        Gen_comb_cc_fast(input_var, ret_var, ty_arr, stride, spos);
      }
    } else {
      Gen_comb_cc_base(input_var, ret_var, ty_arr, oh, ow, spos);
    }
    load_ret_node = _cntr->New_ld(ret_var, spos);
    load_ret_node->Set_rtype(ret_var->Type());
  } else {
    AIR_ASSERT(channel == 1);
    input_var->Set_type(ret_type);
    load_ret_node = _cntr->New_ld(input_var, spos);
  }
  return load_ret_node;
}

ADDR_DATUM_PTR TENSOR2VECTOR_UTIL::Mv_valid_to_start(ADDR_DATUM_PTR input_var,
                                                     ARRAY_TYPE_PTR ty_arr,
                                                     int64_t        padsize,
                                                     const SPOS&    spos) {
  std::vector<int64_t> shape = ty_arr->Shape();
  int64_t              iw    = shape[3];

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
    NODE_PTR input, int64_t padsize, int64_t ss_h, int64_t ss_w, int64_t ks,
    int64_t stride, bool is_start_pos_valid, const SPOS& spos) {
  _ctx.Incr_num_vloop();
  std::vector<int64_t> shape   = input->Rtype()->Cast_to_arr()->Shape();
  int64_t              channel = shape[1];
  int64_t              ih      = shape[2];
  int64_t              iw      = shape[3];

  _ctx.Trace(TF_LOWER, "channel: ", channel, " padsize: ", padsize,
             "input height: ", ih, " input width:", iw,
             "stride slice size height: ss_h:", ss_h,
             "stride slice size width: ss_w:", ss_w, " kernel shape: ", ks,
             " stride: ", stride, " slots: ", _ctx.Max_slots(), "\n");

  ADDR_DATUM_PTR input_var = input->Addr_datum();
  ARRAY_TYPE_PTR ty_arr    = input->Rtype()->Cast_to_arr();
  int64_t        oh        = ss_h / stride;
  int64_t        ow        = ss_w / stride;

  if (stride == ss_h) {
    // global average pool, only need combine cross channel since only 1 valid
    // element in 1 channel
    AIR_ASSERT(channel > 1);
    if (!_ctx.Selective_ss()) {
      // TODO: Here make an assumption that all gap will followed with gemm,
      // if no selective ss option is provided, then need to clear zero before
      // compact. move it to gap implementation?
      Gen_clear_data_stmt(input, spos);
    }
    return Comb_cross_channel(input_var, ty_arr, oh, ow, stride, spos, true);
  }

  bool enb_row_fast = false;
  bool enb_rc_fast  = false;
  bool enb_cc_fast  = false;
  if (_ctx.Stride_slice_fast() || _ctx.Two_level_ss()) {
    // after first log loop: group=ow/stride
    if (stride > 1) {
      enb_row_fast = Is_even(ss_w);
      enb_rc_fast  = Is_even(ss_h);
      enb_cc_fast  = true;
    }

    // TODO: later can be improved to remove this constraint
    if ((_ctx.Max_slots() - channel * ih * iw) < (oh * ow)) {
      enb_cc_fast = false;
    }
  }

  _ctx.Trace(TF_LOWER, "row_fast=", enb_row_fast, ", rc_fast=", enb_rc_fast,
             ", cc_fast=", enb_cc_fast, "\n");

  ADDR_DATUM_PTR comb_rc_ret;
  if ((stride > 1) &&
      (_ctx.Two_level_ss() || Enable_1level_channel_comb(ih, iw))) {
    // consume 1 level to combine valid elements in channels
    comb_rc_ret = Gen_comb_in_channel(input_var, ty_arr, stride, spos);
  } else {
    int64_t interval = 0;
    // consume 1 level to combine valid elements in rows
    ADDR_DATUM_PTR comb_row_ret =
        Comb_in_row(input_var, ty_arr, ow, stride, padsize, interval,
                    is_start_pos_valid, enb_row_fast, spos);

    // consume 1 level to combine valid elements in rows and columns
    comb_rc_ret = Comb_cross_row_col(comb_row_ret, ty_arr, oh, ow, stride,
                                     interval, spos, enb_rc_fast);
  }

  return Comb_cross_channel(comb_rc_ret, ty_arr, oh, ow, stride, spos,
                            enb_cc_fast);
}

NODE_PTR TENSOR2VECTOR_UTIL::Gen_roll_sum_no_loop_body(
    NODE_PTR input, std::vector<int> kernel_shape) {
  FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
  SPOS        spos   = input->Spos();

  ARRAY_TYPE_PTR       input_ty  = input->Rtype()->Cast_to_arr();
  std::vector<int64_t> input_dim = input_ty->Shape();
  AIR_ASSERT(input_dim.size() == 4);
  int64_t h = input_dim[2];
  int64_t w = input_dim[3];

  std::string add_row_str =
      std::string("tmp_row_add") + std::to_string(_ctx.Get_num_vloop());
  ADDR_DATUM_PTR add_row_var =
      fscope->New_var(input_ty, add_row_str.c_str(), spos);

  CONST_TYPE_PTR s32_type =
      _cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  int64_t ks = kernel_shape[0];
  // calculate sum of elements in kernel shape
  std::vector<int> roll_num1{(int)ks / 2 * (int)w};
  NODE_PTR         tmp_roll_node1 = New_roll(
      input, _cntr->New_intconst(s32_type, ks / 2 * w, spos), roll_num1, spos);

  NODE_PTR add_row_node = New_add(input, tmp_roll_node1, spos);
  STMT_PTR add_row_stmt = _cntr->New_st(add_row_node, add_row_var, spos);

  _ctx.Prepend(add_row_stmt);
  NODE_PTR ld_row_add = _cntr->New_ld(add_row_var, spos);

  std::vector<int> roll_num2{(int)ks / 2};
  NODE_PTR         tmp_roll_node2 = New_roll(
      ld_row_add, _cntr->New_intconst(s32_type, ks / 2, spos), roll_num2, spos);
  NODE_PTR add_col_node = New_add(ld_row_add, tmp_roll_node2, spos);
  return add_col_node;
}

NODE_PTR TENSOR2VECTOR_UTIL::Gen_roll_sum_body(NODE_PTR         input,
                                               std::vector<int> kernel_shape) {
  SPOS                 spos      = input->Spos();
  ARRAY_TYPE_PTR       input_ty  = input->Rtype()->Cast_to_arr();
  std::vector<int64_t> input_dim = input_ty->Shape();
  AIR_ASSERT(input_dim.size() == 4);
  int64_t h = input_dim[2];
  int64_t w = input_dim[3];

  int loop_ub = log2(h * w);
  Gen_loop_roll_add_stmt("gap_sum_index", loop_ub, 1, input, spos);
  NODE_PTR ld_sum_node;
  if (input->Opcode() ==
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::LDP)) {
    ld_sum_node = _cntr->New_ldp(input->Preg(), spos);
  } else {
    ld_sum_node = _cntr->New_ld(input->Addr_datum(), spos);
  }
  return ld_sum_node;
}

NODE_PTR Handle_gemm_base(TENSOR2VECTOR_CTX& ctx, air::base::NODE_PTR node,
                          NODE_PTR new_ld0, NODE_PTR new_bias, SPOS spos) {
  CONTAINER*         cntr = ctx.Container();
  TENSOR2VECTOR_UTIL vgen(ctx);

  CONSTANT_PTR         op1_const  = node->Child(1)->Const();
  ARRAY_TYPE_PTR       op1_ty_arr = node->Child(1)->Rtype()->Cast_to_arr();
  std::vector<int64_t> op1_shape  = Get_array_dim_ubs(op1_ty_arr);

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
  FPVEC diag_vec;
  int   padw;
  for (padw = width; padw <= height * width; padw++) {
    if (padw % height == 0) break;
  }

  for (int i = 0; i < height; i++) {
    FPVEC diag = Transpose_diagonal(op1_mat, i, padw);
    diag_vec   = diag_vec + diag;
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

  if (ctx.Config().Python_dsl()) {
    TENSOR2VECTOR_PY_IMPL py_gen(ctx);
    return py_gen.New_py_gemm(node, new_ld0, new_weight, new_bias,
                              node->Spos());
  }

  NODE_PTR new_node =
      vgen.New_gemm_metakernel(new_ld0, new_weight, new_bias, need_mask, spos);
  return new_node;
}

// Trace the analysis params.
void TENSOR2VECTOR_UTIL::Trace_conv_params(int num_grid, int num_block,
                                           int width_block_data,
                                           int width_block_pad, int cap_block,
                                           int num_dup_input, std::string msg) {
  _ctx.Trace(TF_LOWER, "Trace_conv_params: ", msg, "\nslots=", _ctx.Get_slot(),
             "\nnum_grid=", num_grid, "\nnum_block=", num_block,
             "\nwidth_block_data=", width_block_data,
             "\nwidth_block_pad=", width_block_pad, "\ncap_block=", cap_block,
             "\nnum_dup_input=", num_dup_input, "\n========\n");
}

}  // namespace vector
}  // namespace nn
