//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using namespace air::base;

void Get_block_size(int64_t height, int& h1, int& h2) {
  for (int i = (int)sqrt(height); i >= 1; i--)
    if (height % i == 0) {
      h1 = i;
      break;
    }
  h2 = height / h1;
}

std::string New_array_name(const std::string&          base_name,
                           const std::vector<int64_t>& shape) {
  std::string aname = base_name + "_";
  for (int i = 0; i < shape.size() - 1; i++)
    aname += (std::to_string(shape[i]) + "x");
  aname += std::to_string(shape[shape.size() - 1]);
  return aname;
}

TYPE_PTR New_array_type(GLOB_SCOPE* gscope, const std::string& ty_name,
                        CONST_TYPE_PTR              elem_type,
                        const std::vector<int64_t>& shape, const SPOS& spos) {
  std::string arr_type_name = New_array_name(ty_name, shape);
  TYPE_PTR    arr_type =
      gscope->New_arr_type(arr_type_name.c_str(), elem_type, shape, spos);
  return arr_type;
}

TYPE_PTR New_array_type(GLOB_SCOPE* gscope, const std::string& ty_name,
                        uint32_t ty_name_suffix, CONST_TYPE_PTR elem_type,
                        const std::vector<int64_t>& shape, const SPOS& spos) {
  std::string new_type_name = ty_name + std::to_string(ty_name_suffix);
  return New_array_type(gscope, new_type_name, elem_type, shape, spos);
}

//! New an array const based on shape and buf.
CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             int asize, CONST_TYPE_PTR elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos) {
  TYPE_PTR const_type = New_array_type(gscope, name, elem_type, shape, spos);

  // TODO: type Byte_sz()
  int          bsz = elem_type->Cast_to_prim()->Byte_size();
  CONSTANT_PTR result_const =
      gscope->New_const(CONSTANT_KIND::ARRAY, const_type, buf, bsz * asize);
  return result_const;
}

//! New an array const based on shape and buf.
CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             uint32_t name_suffix, int asize,
                             CONST_TYPE_PTR       elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos) {
  std::string new_name = name + std::to_string(name_suffix);
  return New_array_const(gscope, new_name, asize, elem_type, shape, buf, spos);
}

//! Get NCHW from array type. TODO: format
void Get_array_nchw(CONST_TYPE_PTR type, int64_t& n, int64_t& c, int64_t& h,
                    int64_t& w) {
  AIR_ASSERT_MSG(type->Is_array(), "not an array type");
  std::vector<int64_t> shape = type->Cast_to_arr()->Shape();
  int                  dim   = shape.size();
  AIR_ASSERT_MSG((dim >= 2) || (dim <= 4),
                 "dim=%d: only 2/3/4 is valid for NCHW", dim);

  h = shape[dim - 2];
  w = shape[dim - 1];
  c = 1;
  n = 1;
  if (dim == 3) c = shape[0];
  if (dim == 4) {
    n = shape[0];
    c = shape[1];
  }
}

//! Get stride value from array type. TODO: format
void Get_const_array_value(CONSTANT_PTR const_ptr, int64_t& val_row,
                           int64_t& val_col) {
  // AIR_ASSERT_MSG(type->Is_array(), "not an array type");
  // std::vector<int64_t> shape = type->Array_elem->Shape();
  size_t dim = const_ptr->Array_byte_len() / sizeof(int64_t);
  AIR_ASSERT_MSG(dim == 2, "dim=%d: is valid for strided slice child node",
                 dim);

  val_row = const_ptr->Array_elem<int64_t>(0);
  val_col = const_ptr->Array_elem<int64_t>(0);
}

//! Get int attr from node
std::vector<int> Get_attr_int(NODE_PTR node, const char* key) {
  uint32_t   count = 0;
  const int* attr  = node->Attr<int>(key, &count);
  AIR_ASSERT(attr != nullptr && count > 0);
  return std::vector<int>(attr, attr + count);
}

void Set_attr_int(NODE_PTR node, std::string key, std::vector<int> attr) {
  node->Set_attr(key.c_str(), attr.data(), attr.size());
}

/**
 * @brief Vector add utils. TODO: a new util file.
 */
FPVEC operator+(FPVEC const& x, FPVEC const& y) {
  FPVEC vec;
  vec.reserve(x.size() + y.size());
  vec.insert(vec.end(), x.begin(), x.end());
  vec.insert(vec.end(), y.begin(), y.end());
  return vec;
}

/**
 * @brief 2D Transpose_diagonal utils. TODO: a new util file.
 *
 * @param A: input 2D
 * @param position: where to start the diag
 * @param padw: pad A's width
 */
FPVEC Transpose_diagonal(FPMAT A, size_t position, size_t padw) {
  // = rows of A
  size_t h = A.size();
  size_t w = A[0].size();
  AIR_ASSERT_MSG((padw >= w) && (padw % h == 0),
                 "padw constraint: h=%d, w=%d, padw=%d", h, w, padw);

  FPVEC diag(padw, 0);

  size_t i = 0, j = position;

  for (size_t k = 0; k < padw; k++) {
    if (j >= w)
      diag[k] = 0;
    else
      diag[k] = A[i][j];

    i = (i + 1) % h;
    j = (j + 1) % padw;
  }

  return diag;
}

// Record the location of im2col
void Get_im2col_kernel(FPVEC& weight, int c_in, int h, int w, int c_out, int kh,
                       int kw, int padding, int stride, std::vector<int>& ra,
                       FPMAT& conv1_im2col_kernel) {
  FPMAT conv1_im2col_index(c_in * kh * kw, FPVEC(h * w, 0.0));
  int   oh, ow;
  if (padding) {
    oh = h;
    ow = w;
  } else {
    oh = h - kh + 1;
    ow = w - kw + 1;
  }
  int padsize = (kh - 1) / 2;

  // 1-compute im2col_index[c_in*kh*kw][oh*ow]
  int k = kh;  // assume kh = kw
  // row: c_in*kh*kw, column: oh*ow
  for (int hi = 0; hi < oh; hi++) {
    for (int wi = 0; wi < ow; wi++) {
      for (int ci = 0; ci < c_in; ci++) {
        for (int khi = 0; khi < k; khi++) {
          for (int kwi = 0; kwi < k; kwi++) {
            if (((hi + khi) < padsize) || ((wi + kwi) < padsize) ||
                ((hi + khi) >= (oh + padsize)) ||
                ((wi + kwi) >= (ow + padsize)))
              conv1_im2col_index[ci * k * k + khi * k + kwi][hi * ow + wi] = 0;
            else
              conv1_im2col_index[ci * k * k + khi * k + kwi][hi * ow + wi] = 1;
          }
        }
      }
    }
  }

  // 2-ra
  // first align [0,0]
  ra[0]           = -1 * (h * padsize + padsize);
  ra[kh * kw - 1] = h * padsize + padsize;
  ra[kh * kw / 2] = 0;
  if (kh > 1) {
    ra[kh * kw / 2 + 1] = 1;
    ra[kh * kw / 2 - 1] = -1;
  }

  for (int i = 1; i < (kh * kw / 2 - 1); i++) {
    ra[i] = ra[i - 1] + 1;
    if ((kh > 3) && (i % kh == 0)) {
      ra[i] += (h - kh);
    }
  }
  for (int i = kh * kw - 1; i > (kh * kw / 2 + 2); i--) {
    ra[i - 1] = ra[i] - 1;
    if ((kh > 3) && (i % kh == 0)) ra[i - 1] -= (h - kh);
  }

  for (int c1 = 0; c1 < c_out; c1++) {
    // compute im2col_kernel according to the c_out: [c_in*kh*kw, c_out*h*w]
    for (int i = 0; i < c_in * kh * kw; i++) {
      for (int j = 0; j < h * w; j++) {
        conv1_im2col_kernel[i][j + c1 * (h * w)] =
            conv1_im2col_index[(i + c1 * (kh * kw)) % (c_in * kh * kw)][j] *
            weight[c1 * (c_in * kh * kw) +
                   (i + c1 * (kh * kw)) % (c_in * kh * kw)];
        if (stride > 1) {
          if ((ra[i % (kh * kw)] > 0) &&
              (j >= h * w - stride * ra[i % (kh * kw)]))  // left
            conv1_im2col_kernel[i][j + c1 * (h * w)] = 0;
          if ((ra[i % (kh * kw)] < 0) &&
              (j < -1 * stride * ra[i % (kh * kw)]))  // right
            conv1_im2col_kernel[i][j + c1 * (h * w)] = 0;
        }
      }
    }
  }
}

void Masking_padding_stride_data_in_vec(int h, int w, int channel, int padding,
                                        int stride, FPVEC& input) {
  AIR_ASSERT_MSG((stride > 1) && (padding != 0),
                 "Masking_padding_stride_data_in_vec is executed when stride > "
                 "1 and padding != 0(padding same)");
  for (int i = 0; i < channel; i++) {
    for (int j = 0; j < h; j++) {
      if (j % stride == 0) {
        for (int k = 1; k < w; k += stride) {
          input[i * h * w + j * w + k] = 0;
        }
      } else {
        for (int k = 0; k < w; k++) {
          input[i * h * w + j * w + k] = 0;
        }
      }
    }
  }
}

void Masking_padding_stride_data_in_mat(int first_dim, int h, int w,
                                        int channel, int padding, int stride,
                                        FPMAT& input) {
  AIR_ASSERT_MSG((stride > 1) && (padding != 0),
                 "Masking_padding_stride_data_in_mat is executed when stride > "
                 "1 and padding != 0(padding same)");
  for (int i = 0; i < first_dim; i++) {
    Masking_padding_stride_data_in_vec(h, w, channel, padding, stride,
                                       input[i]);
  }
}

void Masking_no_padding_stride_data_in_vec(int h, int w, int channel,
                                           int padding, int stride, int kh,
                                           int kw, FPVEC& input) {
  AIR_ASSERT_MSG(padding == 0,
                 "Masking_no_padding_stride_data_in_vec is only needed when "
                 "real padding == 0");
  AIR_ASSERT_MSG(kh == kw, "Masking_no_padding_stride_data_in_vec: kh != kw");
  int padsize = (kh - 1) / 2;

  // process padding
  for (int i = 0; i < channel; i++) {
    for (int j = 0; j < h; j++) {
      if (j < padsize || j >= (h - padsize)) {
        for (int k = 0; k < w; k++) {
          input[i * h * w + j * w + k] = 0;
        }
      } else {
        for (int k = 0; k < w; k++) {
          if (k < padsize || k >= (w - padsize)) {
            input[i * h * w + j * w + k] = 0;
          }
        }
      }
    }
  }

  // process stride
  if (stride > 1) {
    for (int i = 0; i < channel; i++) {
      // start from padsize since above loop complete mask work for padding
      for (int j = padsize; j < h; j++) {
        // -padsize then jobs be similar to work at 1010,0000,1010,0000
        if ((j - padsize) % stride == 0) {
          for (int k = padsize + 1; k < w; k += stride) {
            input[i * h * w + j * w + k] = 0;
          }
        } else {
          for (int k = 0; k < w; k++) {
            input[i * h * w + j * w + k] = 0;
          }
        }
      }
    }
  }
}

void Masking_no_padding_stride_data_in_mat(int first_dim, int h, int w, int kh,
                                           int kw, int channel, int padding,
                                           int stride, FPMAT& input) {
  AIR_ASSERT_MSG(
      padding == 0,
      "Masking_no_padding_stride_data_in_mat is only needed when padding = 0");

  for (int i = 0; i < first_dim; i++) {
    Masking_no_padding_stride_data_in_vec(h, w, channel, padding, stride, kh,
                                          kw, input[i]);
  }
}

FPVEC Get_avg_value_mask(int c_in, int h, int w, int ks) {
  // TODO: size should be next power of 2
  FPVEC avg_value_mask(c_in * h * w, 0.0);
  for (int64_t ci = 0; ci < c_in; ci++)
    for (int64_t i = 0; i < h; i++)
      for (int64_t j = 0; j < w; j++) {
        if ((i % ks != 0) || (j % ks != 0))
          avg_value_mask[ci * (h * w) + i * h + j] = 0;
        else
          avg_value_mask[ci * (h * w) + i * h + j] = 0.25;
      }
  return avg_value_mask;
}

FPVEC Get_global_avg_value_mask(int c_in, int h, int w) {
  bool  channel_begin = true;
  FPVEC avg_value_mask(c_in * h * w, 0.0);
  for (int i = 0; i < c_in; i++) {
    for (int j = 0; j < h; j++) {
      for (int k = 0; k < w; k++) {
        if (channel_begin) {
          avg_value_mask[i * h * w + j * w + k] = 1.0 / (h * w);
          channel_begin                         = false;
        }
      }
    }
    channel_begin = true;
  }
  return avg_value_mask;
}

// used to eliminate the invalid data in rows, put valid data together in rows
FPVEC Get_mask_for_row_combine(int c_in, int h, int w, int ss_h, int ss_w,
                               int stride) {
  int   matrix_row_size = ss_w / stride;
  FPMAT mask_matrix(matrix_row_size, FPVEC(c_in * h * w, 0.0));

  for (int q = 0; q < matrix_row_size; q++) {
    for (int i = 0; i < c_in; i++) {
      for (int j = 0; j < h; j++) {
        for (int k = 0; k < w; k++) {
          if ((j % 2 != 1) && (k == q)) {
            mask_matrix[q][i * h * w + j * w + k] = 1.0;
          }
        }
      }
    }
  }

  FPVEC mask_vec;
  for (int i = 0; i < matrix_row_size; i++) {
    mask_vec.insert(end(mask_vec), begin(mask_matrix[i]), end(mask_matrix[i]));
  }

  return mask_vec;
}

// used to eliminate the invalid data in columns and rows, put valid data
// together by columns and rows
FPVEC Get_mask_for_rc_combine(int c_in, int h, int w, int valid_data_count) {
  int   number_of_valid_data = valid_data_count;
  int   matrix_row_size      = valid_data_count;
  FPMAT mask_matrix(matrix_row_size, FPVEC(c_in * h * w, 0.0));

  int start = 0;
  for (int q = 0; q < matrix_row_size; q++) {
    for (int i = 0; i < c_in; i++) {
      for (int j = 0; j < h; j++) {
        for (int k = 0; k < w; k++) {
          if (((i * h * w + j * h + k) >= (i * h * w + start)) &&
              ((i * h * w + j * h + k) <
               (i * h * w + start + number_of_valid_data))) {
            mask_matrix[q][i * h * w + j * h + k] = 1.0;
          }
        }
      }
    }
    start += number_of_valid_data;
  }

  FPVEC mask_vec;
  for (int i = 0; i < matrix_row_size; i++) {
    mask_vec.insert(end(mask_vec), begin(mask_matrix[i]), end(mask_matrix[i]));
  }

  return mask_vec;
}

// a consecutive piece of 1 mask, others are all 0, no matter what channels
FPVEC Get_mask_for_cross_channel_combine(int c_in, int h, int w,
                                         int valid_data_count) {
  // number_of_valid_data stands for the number of consecutive 1
  int number_of_valid_data = valid_data_count;

  int   matrix_row_size = c_in;
  FPMAT mask_matrix(matrix_row_size, FPVEC(c_in * h * w, 0.0));

  // start stands for the begining postion of consecutive 1
  int start = 0;

  for (int q = 0; q < matrix_row_size; q++) {
    for (int i = 0; i < c_in; i++) {
      for (int j = 0; j < h; j++) {
        for (int k = 0; k < w; k++) {
          if (((i * h * w + j * h + k) >= start) &&
              ((i * h * w + j * h + k) < (start + number_of_valid_data))) {
            mask_matrix[q][i * h * w + j * h + k] = 1.0;
          }
        }
      }
    }
    start += number_of_valid_data;
  }

  FPVEC mask_vec;
  for (int i = 0; i < matrix_row_size; i++) {
    mask_vec.insert(end(mask_vec), begin(mask_matrix[i]), end(mask_matrix[i]));
  }

  return mask_vec;
}

}  // namespace vector
}  // namespace nn
