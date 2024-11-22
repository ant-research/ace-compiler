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

typedef std::vector<std::vector<float> > FPMAT;
typedef std::vector<float>               FPVEC;

void Get_block_size(int64_t height, int& h1, int& h2);

TYPE_PTR New_array_type(GLOB_SCOPE* gscope, const std::string& ty_name,
                        CONST_TYPE_PTR              elem_type,
                        const std::vector<int64_t>& shape, const SPOS& spos);

TYPE_PTR New_array_type(GLOB_SCOPE* gscope, const std::string& ty_name,
                        uint32_t ty_name_suffix, CONST_TYPE_PTR elem_type,
                        const std::vector<int64_t>& shape, const SPOS& spos);

CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             int asize, CONST_TYPE_PTR elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos);

CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             uint32_t ty_name_suffix, int asize,
                             CONST_TYPE_PTR       elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos);

std::string New_array_name(const std::string&          base_name,
                           const std::vector<int64_t>& shape);

std::vector<int> Get_attr_int(NODE_PTR node, const char* key);
void Set_attr_int(NODE_PTR node, std::string key, std::vector<int> attr);

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

void Get_array_nchw(CONST_TYPE_PTR type, int64_t& n, int64_t& c, int64_t& h,
                    int64_t& w);

void Get_const_array_value(CONSTANT_PTR const_ptr, int64_t& val_row,
                           int64_t& val_col);

//!  @brief Vector add utils. TODO: a new util file.
FPVEC operator+(FPVEC const& x, FPVEC const& y);

/**
 * @brief 2D Transpose_diagonal utils. TODO: a new util file.
 *
 * @param A: input 2D
 * @param position: where to start the diag
 * @param padw: pad A's width
 */
FPVEC Transpose_diagonal(FPMAT A, size_t position, size_t padw);

// Record the location of im2col
void Get_im2col_kernel(FPVEC& weight, int c_in, int h, int w, int c_out, int kh,
                       int kw, int padding, int stride, std::vector<int>& ra,
                       FPMAT& conv1_im2col_kernel);

//! @brief Clear invalid data in vector to zero with real padding(real
//! padding!=0). Current impl of conv padding same no matter what real padding
//! attribute is. Valid data and invalid data are separated by stride. Used only
//! when stride > 1 and padding!=0. Currently used for conv, make values in gaps
//! in conv result be 0, then eliminate potential masking operations. For
//! example input: 1x2x3x4x,xxxxxxxx; output:10203040,00000000
//! @param h: original height of the tensor
//! @param w: original width of the tensor
//! @param channel: original channel of the tensor
//! @param padding: padding attribute of conv
//! @param stride: stride attribute of conv
//! @param input: input vector
void Masking_padding_stride_data_in_vec(int h, int w, int channel, int padding,
                                        int stride, FPVEC& input);

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
//! @param input: input two-dimensional matrix(2D vector)
void Masking_padding_stride_data_in_mat(int first_dim, int h, int w,
                                        int channel, int padding, int stride,
                                        FPMAT& input);

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
void Masking_no_padding_stride_data_in_mat(int first_dim, int h, int w, int kh,
                                           int kw, int channel, int padding,
                                           int stride, FPMAT& input);

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
void Masking_no_padding_stride_data_in_vec(int h, int w, int channel,
                                           int padding, int stride, int kh,
                                           int kw, FPVEC& input);

//! calculate average value and clear zero mask
FPVEC Get_avg_value_mask(int c_in, int h, int w, int ks);

//! calculate global average value and clear zero mask
FPVEC Get_global_avg_value_mask(int c_in, int h, int w);

//! extract valid data row mask
FPVEC Get_mask_for_row_combine(int c_in, int h, int w, int ss_h, int ss_w,
                               int stride);

//! extract valid data row and column mask
FPVEC Get_mask_for_rc_combine(int c_in, int h, int w, int valid_data_count);

//! extract valid data cross channel mask
FPVEC Get_mask_for_cross_channel_combine(int c_in, int h, int w,
                                         int valid_data_count);

}  // namespace vector
}  // namespace nn

#endif
