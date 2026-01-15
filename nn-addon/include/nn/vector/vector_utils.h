//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_UTILS_H
#define NN_VECTOR_UTILS_H

#include <cmath>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st.h"
#include "air/util/debug.h"

namespace nn {
namespace vector {

using namespace air::base;

#define LARGE_CHANNEL_BORDER 256
#define TINY_HW_BORDER       14

typedef std::vector<std::vector<float> > FPMAT;
typedef std::vector<float>               FPVEC;

void Get_block_size(int64_t height, int& h1, int& h2);

TYPE_PTR New_array_type(GLOB_SCOPE* gscope, const std::string& ty_name,
                        CONST_TYPE_PTR              elem_type,
                        const std::vector<int64_t>& shape, const SPOS& spos);

TYPE_PTR New_array_type(GLOB_SCOPE* gscope, const std::string& ty_name,
                        uint32_t ty_name_suffix, CONST_TYPE_PTR elem_type,
                        const std::vector<int64_t>& shape, const SPOS& spos);

CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, uint64_t asize,
                             CONST_TYPE_PTR       elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos);

CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             uint64_t asize, CONST_TYPE_PTR elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos);
CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             uint32_t ty_name_suffix, uint64_t asize,
                             CONST_TYPE_PTR       elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos);

std::string New_array_name(const std::string&          base_name,
                           const std::vector<int64_t>& shape);

template <typename T>
std::vector<T> Get_attr_data(NODE_PTR node, const char* key) {
  uint32_t count = 0;
  const T* attr  = node->Attr<T>(key, &count);
  AIR_ASSERT(attr != nullptr && count > 0);
  return std::vector<T>(attr, attr + count);
}

template <typename T>
bool Has_attr(NODE_PTR node, const char* key) {
  uint32_t count = 0;
  const T* attr  = node->Attr<T>(key, &count);
  return attr != nullptr;
}

//! Move to infra once it is general.
template <typename T>
void Print_array_const(std::ostream& os, CONSTANT_PTR vconst, std::string msg) {
  os << "Array_const (last 2D): " << msg << std::endl;
  vconst->Print(os, 0);
  std::vector<int64_t> shape = vconst->Type()->Cast_to_arr()->Shape();
  const T*             cptr  = vconst->Array_ptr<T>();
  int64_t              h = 0, w = 0;
  int                  dim = shape.size();
  if (dim >= 2) {
    h = shape[dim - 2];
    w = shape[dim - 1];
  } else {
    h = 1;
    w = shape[0];
  }
  for (int64_t i = 0; i < h; i++) {
    for (int64_t j = 0; j < w; j++) {
      os << std::dec << cptr[i * w + j] << " ";
    }
    os << std::endl;
  }
}

bool Is_power_of_two(int n);

//! @brief only works when the type of n parameter is int64_t
int64_t Next_power_of_two(int64_t n);

bool Is_even(int n);

void Get_array_nchw(CONST_TYPE_PTR type, int64_t& n, int64_t& c, int64_t& h,
                    int64_t& w);
void Verify_conv(NODE_PTR node, int64_t& batch, int64_t& channel_in,
                 int64_t& input_height, int64_t& input_width,
                 int64_t& channel_out, int64_t& kernel_height,
                 int64_t& kernel_width, int group);

void Get_const_array_value(CONSTANT_PTR const_ptr, int64_t& val_row,
                           int64_t& val_col);

//! get upper bounds of array dimensions
std::vector<int64_t> Get_array_dim_ubs(ARRAY_TYPE_PTR array_type_ptr);

//!  @brief Vector add utils. TODO: a new util file.
FPVEC operator+(FPVEC const& x, FPVEC const& y);

// IRMA packing: Bdiv->M
FPMAT Data_transform_irma(const FPMAT& b);
void  Imra_cost_model(int64_t nd, int64_t kd, int64_t num_slots, int& bs,
                      int& pb, int& ps, int& cost, int& num_search);
void  Dump_fpmat(const FPMAT& b, const std::string& msg);

/**
 * @brief 2D Transpose_diagonal utils. TODO: a new util file.
 *
 * @param A: input 2D
 * @param position: where to start the diag
 * @param padw: pad A's width
 */
FPVEC Transpose_diagonal_fast(FPMAT A, size_t position, size_t padw);
FPVEC Transpose_diagonal(FPMAT A, size_t position, size_t padw);

// Record the location of im2col
void Get_im2col_kernel(FPVEC& weight, int c_in, int h, int w, int c_out, int kh,
                       int kw, int padding, int stride, std::vector<int>& ra,
                       FPMAT& conv1_im2col_kernel);

NODE_PTR Transpose_weight2row_sharding(
    CONTAINER* cntr, NODE_PTR weight_node, int64_t x, int64_t y, int64_t z,
    int64_t halosize, int64_t channel_in_kernel, int64_t input_height,
    int64_t input_width, int64_t channel_out, int64_t kernel_height,
    int64_t kernel_width, int stride);

//! @brief Clear invalid data in vector to zero due to stride.
//! Current impl of conv padding same no matter what real padding
//! attribute is. Valid data and invalid data are separated by stride. Used only
//! when (stride>1 and padding!=0) or (stride>1 and padding=0 and ks=1)
//! Currently used for conv, make values in gaps in conv result be 0,
//! then eliminate potential masking operations. For
//! example input: 1x2x3x4x,xxxxxxxx; output:10203040,00000000
//! @param h: original height of the tensor
//! @param w: original width of the tensor
//! @param channel: original channel of the tensor
//! @param padding: padding attribute of conv
//! @param stride: stride attribute of conv
//! @param kh: kernel height attribute of conv
//! @param input: input vector
void Masking_stride_data_in_vec(int h, int w, int channel, int padding,
                                int stride, int kh, FPVEC& input);

//! @brief Clear invalid data in two-dimensional matrix(2D vector) to zero with
//! real padding(real padding!=0). Current impl of conv padding same no matter
//! what real padding attribute is. Valid data and invalid data are separated by
//! stride. Used only when stride > 1 and padding!=0. Currently used for conv,
//! make values in gaps in conv result be 0, then eliminate potential masking
//! operations. For example input: 1x2x3x4x,xxxxxxxx; output:10203040,00000000
//! @param first_dim: first dimension of the two-dimensional matrix
//! @param h: original height of the tensor
//! @param w: original width of the tensor
//! @param channel: original channel of the tensor
//! @param padding: padding attribute of conv
//! @param stride: stride attribute of conv
//! @param kh: kernel height attribute of conv
//! @param input: input two-dimensional matrix(2D vector)
void Masking_padding_stride_data_in_mat(int first_dim, int h, int w,
                                        int channel, int padding, int stride,
                                        int kh, FPMAT& input);

//! @brief Clear invalid data in two-dimensional matrix(2D vector) to zero
//! without real padding(real padding==0). Current impl of conv padding same no
//! matter what real padding attribute is. Valid data and invalid data are
//! separated by stride and padding. Used only when stride > 1 and pdding==0.
//! Currently used for conv, make values in gaps in conv result be 0, then
//! eliminate potential masking operations. For example input1:
//! xxxxxxxx,x1x2x3xx,xxxxxxxx; output1:00000000,01020300,00000000
//! input2: xxxx,x12x, x34x; output2:0000,0120,0340
//! @param first_dim: first dimension of the two-dimensional matrix
//! @param h: original height of the tensor
//! @param w: original width of the tensor
//! @param kh: conv kernel height
//! @param kw: conv kernel width
//! @param channel: original channel of the tensor
//! @param padding: padding attribute of conv
//! @param stride: stride attribute of conv
//! @param input: input two-dimensional matrix(2D vector)
//! @param fhe_padsize: real padsize in ciphertext
void Masking_no_padding_stride_data_in_mat(int first_dim, int h, int w, int kh,
                                           int kw, int channel, int padding,
                                           int stride, FPMAT& input,
                                           int fhe_padsize);

//! @brief Clear invalid data in vector to zero without real padding(real
//! padding==0). Current impl of conv padding same no matter what real padding
//! attribute is. Valid data and invalid data are separated by stride and
//! padding. Used only when stride > 1 and pdding==0. Currently used for conv,
//! make values in gaps in conv result be 0, then eliminate potential masking
//! operations. For example input1: xxxxxxxx,x1x2x3xx,xxxxxxxx;
//! output1:00000000,01020300,00000000
//! input2: xxxx,x12x, x34x; output2:0000,0120,0340
//! @param h: original height of the tensor
//! @param w: original width of the tensor
//! @param channel: original channel of the tensor
//! @param padding: padding attribute of conv
//! @param stride: stride attribute of conv
//! @param kh: conv kernel height
//! @param kw: conv kernel width
//! @param input: input two-dimensional matrix(2D vector)
//! @param fhe_padsize: real padsize in ciphertext
void Masking_no_padding_stride_data_in_vec(int h, int w, int channel,
                                           int padding, int stride, int kh,
                                           int kw, FPVEC& input,
                                           int fhe_padsize);

//! calculate average value and clear zero mask
FPVEC Get_avg_value_mask(int c_in, int h, int w, int ks);

//! calculate global average pool's mask
//! @param fuse_avg_val: when this is enabled, mask value will use average value
//! replace 1
FPVEC Get_gap_mask(int c_in, int h, int w, bool fuse_avg_val = true);

//! data in channel combine mask
FPVEC Get_dic_comb_mask(int c_in, int h, int w, int stride);

FPVEC Get_large_row_mask(int c_in, int h, int w, int stride);

FPVEC Get_large_channel_mask(int c_in, int h, int w, int stride);

//! extract valid data row mask
FPVEC Get_row_combine_mask(int c_in, int h, int w, int valid_data_num,
                           int valid_data_group_num);

//! extract valid data row and column mask
FPVEC Get_rc_combine_mask(int c_in, int h, int w, int gvdn,
                          int valid_data_group_num, int last_gvdn);

//! extract valid data cross channel mask
FPVEC Get_cc_combine_mask(int c_in, int h, int w, int gvdn,
                          int valid_data_group_num, int last_gvdn);

//! @brief Expand input constants with zero according to data layout
//! @param input: input constants
//! @param orig_h: original height of input constants
//! @param orig_w: original width of input constants
//! @param group_size: how many valid data exists in one row
//! @param stride: indicate the gaps between valid data
//! @param trailing_zero_num: number of trailing 0 need to be inserted after
//! valid data and stride is placed
//! @param target_w: after expand, the target width
//! @param vds_in_channel: valid data size in one channel
//! @param channel_size: size of one channel
FPMAT Expand_with_zero(const float* input, const int orig_h, const int orig_w,
                       const int group_size, const int stride,
                       const int trailing_zero_num, const int target_w,
                       const int vds_in_channel, const int channel_size);

int Get_valid_data_in_row(CONSTANT_PTR slice_sizes, CONSTANT_PTR slice_strides);

//! get log cross channel loop upper bound
int Get_log_cc_lub(int channel, int ih, int iw, int stride);

//! @brief get log cross row and column loop upper bound
//! @param interval: the number of slots between valid data groups
//! @param oh: the size of output height
//! @param ow: the size of output width
int Get_log_rc_lub(int interval, int oh, int ow);

bool Enable_large_channel_comb(int channel, int stride);

bool Enable_log_cc_comb(int channel, int ih, int iw, int oh, int ow);

bool Enable_1level_channel_comb(int ih, int iw);

}  // namespace vector
}  // namespace nn

#endif
