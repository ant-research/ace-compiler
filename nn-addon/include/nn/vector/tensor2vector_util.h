//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_TENSOR2VECTOR_UTIL_H
#define NN_VECTOR_TENSOR2VECTOR_UTIL_H

#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/vector_gen.h"

namespace nn {
namespace vector {

//! @brief shard_map params:
// How metakernel and data are organized.
typedef struct {
  // partition: control outer loop.
  int _num_grid;
  int _num_block;

  // resource for a block: data packed in the block
  int _width_block;
  int _width_block_data;
  int _width_block_pad;
  int _position_block;

  int _cap_block;
  int _num_dup_input;
} SHARD_MAP_PARAMS;

// Cost Model for MetaKernel
int Get_costmodel_mini_factor1(int c);
int Get_costmodel_fusion(int channel_in, int channel_out, int kernel_size,
                         int output_size, int group, int slots);

class TENSOR2VECTOR_UTIL : public VECTOR_GEN {
public:
  TENSOR2VECTOR_UTIL(TENSOR2VECTOR_CTX& ctx)
      : VECTOR_GEN(ctx.Container()), _ctx(ctx) {}

  NODE_PTR New_gemm_metakernel(NODE_PTR op0, NODE_PTR op1, NODE_PTR op2,
                               bool need_mask, const SPOS& spos);
  NODE_PTR New_gemm_metakernel_fast(NODE_PTR op0, NODE_PTR op1, NODE_PTR op2,
                                    const SPOS& spos, bool tiling);
  NODE_PTR New_conv_metakernel(NODE_PTR input, NODE_PTR weight, NODE_PTR bias,
                               std::vector<int> ra, int channel_in,
                               int channel_out, int output_height,
                               int output_width, int kernel_hw, int stride,
                               const SPOS& spos);
  NODE_PTR New_conv_metakernel_fast(NODE_PTR input, NODE_PTR weight,
                                    NODE_PTR bias, std::vector<int> ra,
                                    int channel_in, int channel_out,
                                    int output_width, int output_height,
                                    int kernel_hw, int group, int stride,
                                    const SHARD_MAP_PARAMS* params,
                                    bool need_mask, const SPOS& spos);

  //! use this function to extract valid data, to make valid data together
  //! till now used after conv(add pad to keep output same) and average pool
  //! ih/iw: actually input height and width
  //! ss_h/ss_w: stride slice size height and width
  NODE_PTR New_extract_valid_data(NODE_PTR input, int64_t channel,
                                  int64_t padsize, int64_t ih, int64_t iw,
                                  int64_t ss_h, int64_t ss_w, int64_t ks,
                                  int64_t stride, bool is_start_pos_valid,
                                  const SPOS& spos);

  // New simple LT+1 const loop used for Vector. TODO: move to infra if it is
  // general.
  STMT_PTR New_loop(const char* index_str, int init, int upper,
                    const SPOS& spos);

  template <typename T>
  T Get_scalar(NODE_PTR node) {
    AIR_ASSERT_MSG(node->Rtype()->Cast_to_arr()->Elem_count() == 1,
                   "input node's type must only contain 1 value");
    T val = node->Const()->Array_elem<T>(0);
    return val;
  }

  NODE_PTR Gen_mask_node(int64_t valid_len, TYPE_PTR etype, float val,
                         const SPOS& spos);

  //! @brief only used for input that all invalid data are
  //! placed at last.
  void Gen_clear_data_stmt(ADDR_DATUM_PTR input_var, int64_t valid_len,
                           TYPE_PTR etype, const SPOS& spos);

  //! @brief till now only used for global average pool
  void Gen_clear_data_stmt(NODE_PTR input_node, int64_t channel, int64_t ih,
                           int64_t iw, const SPOS& spos);

  void Trace_conv_params(int num_grid, int num_block, int width_block,
                         int width_block_pad, int cap_block, int num_dup_input,
                         std::string msg);

private:
  ADDR_DATUM_PTR Roll_valid_to_start(ADDR_DATUM_PTR input_var,
                                     ARRAY_TYPE_PTR ty_arr, int64_t ih,
                                     int64_t iw, int64_t padsize,
                                     const SPOS& spos);

  //! compact all valid data in channel only using 1 level
  ADDR_DATUM_PTR One_level_comb_in_channel(ADDR_DATUM_PTR input_var,
                                           ARRAY_TYPE_PTR ty_arr,
                                           int64_t channel, int64_t ih,
                                           int64_t iw, int64_t ss_w,
                                           int64_t ss_h, int64_t stride,
                                           const SPOS& spos);

  NODE_PTR Two_level_comb(ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr,
                          int64_t channel, int64_t ih, int64_t iw, int64_t ss_h,
                          int64_t ss_w, int64_t stride, const SPOS& spos);

  ADDR_DATUM_PTR Combine_cross_row(ADDR_DATUM_PTR input_var,
                                   ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                   int64_t ih, int64_t iw, int64_t ss_w,
                                   int64_t stride, bool enable_row_fast,
                                   const SPOS& spos);

  ADDR_DATUM_PTR Combine_cross_rc(ADDR_DATUM_PTR input_var,
                                  ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                  int64_t ih, int64_t iw, int64_t ss_h,
                                  int64_t stride, int64_t interval,
                                  const SPOS& spos);

  ADDR_DATUM_PTR Combine_cross_rc_fast(ADDR_DATUM_PTR input_var,
                                       ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                       int64_t ih, int64_t iw, int64_t ss_h,
                                       int64_t ss_w, int64_t stride,
                                       int64_t interval, const SPOS& spos);

  ADDR_DATUM_PTR Gen_comb_cr_base(ADDR_DATUM_PTR input_var,
                                  ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                  int64_t ih, int64_t iw, int64_t ss_w,
                                  int64_t stride, const SPOS& spos);

  ADDR_DATUM_PTR Gen_comb_cr_fast(ADDR_DATUM_PTR input_var,
                                  ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                  int64_t ih, int64_t iw, int64_t ss_w,
                                  int64_t stride, const SPOS& spos);

  NODE_PTR Gen_comb_cc_base(ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr,
                            int64_t channel, int64_t ih, int64_t iw, int64_t oh,
                            int64_t ow, const SPOS& spos);

  NODE_PTR Gen_comb_cc_fastest(ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr,
                               int64_t channel, int64_t ih, int64_t iw,
                               int64_t oh, int64_t ow, const SPOS& spos);

  NODE_PTR Gen_comb_cc_fast(ADDR_DATUM_PTR input_var, ARRAY_TYPE_PTR ty_arr,
                            int64_t channel, int64_t ih, int64_t iw, int64_t oh,
                            int64_t ow, const SPOS& spos);

  NODE_PTR Gen_dup_input_node(NODE_PTR input_node, int64_t dup_num,
                              int input_len, const SPOS& spos);
  void Gen_dup_input_stmt(NODE_PTR input_node, int64_t dup_num, int input_len,
                          ADDR_DATUM_PTR result_var, const SPOS& spos);

public:
  PREG_PTR Gen_collective_reduce_stmt(ADDR_DATUM_PTR input_var, TYPE_PTR etype,
                                      const SPOS& spos, int num_block,
                                      int width_block_data, int width_block_pad,
                                      int output_size, bool need_mask);

  //! @brief this is only used to process roll sum when the
  //! number of valid data in kernel shape is 4, no loop used.
  NODE_PTR Gen_special_roll_sum_body(NODE_PTR         input,
                                     std::vector<int> kernel_shape);

  //! @brief this is used to process roll sum, use loop.
  NODE_PTR Gen_roll_sum_body(NODE_PTR input, std::vector<int> kernel_shape);

  ADDR_DATUM_PTR Gen_store_zero_to_var_stmt(
      std::string var_name, std::string ty_name, ARRAY_TYPE_PTR ty_arr,
      const std::vector<int64_t>& var_shape, const SPOS& spos);

  ADDR_DATUM_PTR Gen_store_zero_to_var_stmt(std::string var_name,
                                            TYPE_PTR vtype, const SPOS& spos);
  //! roll input, multiply mask, add to result
  void Gen_loop_combine_stmt(const char* loop_name, int loop_ub,
                             int elem_interval, ADDR_DATUM_PTR input_var,
                             NODE_PTR mask_node, int64_t mask_slice_len,
                             ADDR_DATUM_PTR result_var, const SPOS& spos);

  //! @brief Two nested loops to combine elements in channel
  //! Inner loop body is roll input, multiply mask, add to
  //! result
  void Gen_loops_combine_stmt(std::string loop_name, int loop_ub1, int loop_ub2,
                              int iw, int stride, ADDR_DATUM_PTR input_var,
                              NODE_PTR mask_node, int64_t mask_slice_len,
                              ADDR_DATUM_PTR result_var, const SPOS& spos);

  //! roll, add to combine or sum
  void Gen_loop_roll_add_stmt(const char* loop_name, int loop_ub,
                              int elem_interval, NODE_PTR input_var,
                              const SPOS& spos);

  NODE_PTR Combine_cross_channel(ADDR_DATUM_PTR input_var,
                                 ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                 int64_t ih, int64_t iw, int64_t oh, int64_t ow,
                                 const SPOS& spos, bool enable_cc_fast = false);

protected:
  TENSOR2VECTOR_CTX& _ctx;
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_TENSOR2VECTOR_UTIL_H
