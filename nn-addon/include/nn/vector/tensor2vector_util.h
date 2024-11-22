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

class TENSOR2VECTOR_UTIL : public VECTOR_GEN {
public:
  TENSOR2VECTOR_UTIL(TENSOR2VECTOR_CTX& ctx)
      : VECTOR_GEN(ctx.Container()), _ctx(ctx) {}

  NODE_PTR New_gemm_metakernel(NODE_PTR op0, NODE_PTR op1, NODE_PTR op2,
                               const SPOS& spos);
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
                                    int kernel_hw, const SPOS& spos);

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

  void Gen_clear_data_stmt(ADDR_DATUM_PTR input_var, int64_t valid_len,
                           TYPE_PTR etype, const SPOS& spos);

  //! Gen combine cross channel vector IR, this only suitable for sparse
  //! data(channel*oh*ow <= ih*iw) scenarios this method does not cost level
  void Gen_combine_cross_channel(ADDR_DATUM_PTR input_var, int64_t channel,
                                 int64_t ih, int64_t iw, int64_t oh, int64_t ow,
                                 const SPOS& spos);

private:
  ADDR_DATUM_PTR Roll_valid_to_start(ADDR_DATUM_PTR input_var,
                                     ARRAY_TYPE_PTR ty_arr, int64_t ih,
                                     int64_t iw, int64_t padsize,
                                     const SPOS& spos);

  ADDR_DATUM_PTR Combine_cross_row(ADDR_DATUM_PTR input_var,
                                   ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                   int64_t ih, int64_t iw, int64_t ss_h,
                                   int64_t ss_w, int64_t ks, int64_t stride,
                                   int64_t padsize, const SPOS& spos);

  ADDR_DATUM_PTR Combine_cross_rc(ADDR_DATUM_PTR input_var,
                                  ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                  int64_t ih, int64_t iw, int64_t ss_h,
                                  int64_t ss_w, int64_t ks, int64_t stride,
                                  int64_t interval, const SPOS& spos);

  NODE_PTR Combine_cross_channel(ADDR_DATUM_PTR input_var,
                                 ARRAY_TYPE_PTR ty_arr, int64_t channel,
                                 int64_t ih, int64_t iw, int64_t ss_h,
                                 int64_t ss_w, int64_t ks, int64_t stride,
                                 const SPOS& spos);

  NODE_PTR Gen_dup_input_node(NODE_PTR input_node, int64_t dup_num,
                              int input_len, const SPOS& spos);
  void Gen_dup_input_stmt(NODE_PTR input_node, int64_t dup_num, int input_len,
                          ADDR_DATUM_PTR result_var, const SPOS& spos);

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

protected:
  TENSOR2VECTOR_CTX& _ctx;
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_TENSOR2VECTOR_UTIL_H
