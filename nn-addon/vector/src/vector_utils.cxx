//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vector_utils.h"

#include <algorithm>

namespace nn {
namespace vector {

using namespace air::base;

bool Is_power_of_two(int n) { return (n > 0) && ((n & (n - 1)) == 0); }

bool Is_even(int n) { return (n & 1) == 0; }

int64_t Next_power_of_two(int64_t n) {
  // Handling edge cases
  if (n < 1) {
    return 1;
  }

  // Set x -= 1 to calculate the nearest data with all digits equal to 1
  n--;

  // By calculating the previous number of bits, increase the remaining bits
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n |= n >> 32;

  // The final result needs to be the current value plus 1 to make it an
  // exponent of 2
  return n + 1;
}

static int Get_num_rot(int rep, int bs, int gs, int ps) {
  return ceil(log2(rep)) + (bs - 1) + (gs - 1) + (ps - 1);
}

void Imra_cost_model(int64_t nd, int64_t kd, int64_t num_slots, int& bs,
                     int& pb, int& ps, int& cost, int& num_search) {
  int64_t min_cost = nd * kd;

  // Generate all divisors of nd
  std::vector<int64_t> divisors;
  for (int64_t i = 1; i <= nd; ++i) {
    if (nd % i == 0) {
      divisors.push_back(i);
    }
  }

  for (const auto& curr_bs : divisors) {
    int64_t curr_pb = nd / curr_bs;

    int64_t max_ps = curr_pb;

    for (int64_t curr_ps = 1; curr_ps <= max_ps; ++curr_ps) {
      if ((curr_pb % curr_ps != 0) ||
          ((kd * curr_ps + nd > num_slots) && (kd != num_slots)) ||
          ((kd == num_slots) && (curr_ps > 1)))
        continue;
      float   curr_rep = ceil(1.0 * (kd * curr_ps + nd) / kd);
      int64_t curr_cost =
          Get_num_rot(curr_rep, curr_bs, curr_pb / curr_ps, curr_ps);
      num_search++;
      if ((curr_cost < min_cost) ||
          ((curr_cost == min_cost) && (curr_ps > ps))) {
        min_cost = curr_cost;
        bs       = curr_bs;
        pb       = curr_pb;
        ps       = curr_ps;
      }
    }
  }
  cost = min_cost;
}

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
CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, uint64_t asize,
                             CONST_TYPE_PTR       elem_type,
                             std::vector<int64_t> shape, void* buf,
                             const SPOS& spos) {
  TYPE_PTR arr_type = gscope->New_arr_type(elem_type, shape, spos);

  int          bsz = elem_type->Cast_to_prim()->Byte_size();
  CONSTANT_PTR result_const =
      gscope->New_const(CONSTANT_KIND::ARRAY, arr_type, buf, bsz * asize);
  return result_const;
}

//! New an array const based on shape and buf.
CONSTANT_PTR New_array_const(GLOB_SCOPE* gscope, const std::string& name,
                             uint64_t asize, CONST_TYPE_PTR elem_type,
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
                             uint32_t name_suffix, uint64_t asize,
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

//! Verify conv and get shape
void Verify_conv(NODE_PTR node, int64_t& batch, int64_t& channel_in,
                 int64_t& input_height, int64_t& input_width,
                 int64_t& channel_out, int64_t& kernel_height,
                 int64_t& kernel_width, int group) {
  NODE_PTR orig_input = node->Child(0);
  int64_t  n = 0, c = 0, h = 0, w = 0;
  Get_array_nchw(orig_input->Rtype(), n, c, h, w);
  AIR_ASSERT_MSG(n == 1, "Conv only supports batch=1");

  NODE_PTR weight_node = node->Child(1);
  int64_t  f = 0, c2 = 0, r = 0, s = 0;
  Get_array_nchw(weight_node->Rtype(), f, c2, r, s);

  if (group == 1) {
    AIR_ASSERT_MSG(c == c2, "channel_in == channel_in_kernel");
  } else {
    AIR_ASSERT_MSG((c == group) && (f == c),
                   "depthwise convolution: channel_in == group");
  }
  AIR_ASSERT_MSG(r == s, "Only support square kernel");

  batch         = n;
  channel_in    = c;
  input_height  = h;
  input_width   = w;
  channel_out   = f;
  kernel_height = r;
  kernel_width  = s;
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
  val_col = const_ptr->Array_elem<int64_t>(1);
}

std::vector<int64_t> Get_array_dim_ubs(ARRAY_TYPE_PTR array_type_ptr) {
  std::vector<int64_t> upper_bounds;
  for (DIM_ITER dim_iter = array_type_ptr->Begin_dim();
       dim_iter != array_type_ptr->End_dim(); ++dim_iter) {
    upper_bounds.push_back((*dim_iter)->Ub_val());
  }
  return upper_bounds;
}

//! Get int attr from node
std::vector<int> Get_attr_int(NODE_PTR node, const char* key) {
  uint32_t   count = 0;
  const int* attr  = node->Attr<int>(key, &count);
  AIR_ASSERT(attr != nullptr && count > 0);
  return std::vector<int>(attr, attr + count);
}

// bad performance, need to improve later.
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

void Dump_fpmat(const FPMAT& b, const std::string& msg) {
  std::cout << "FPMAT " << msg << ":" << std::endl;
  for (const auto& row : b) {
    for (double val : row) {
      std::cout << val << " ";
    }
    std::cout << "\n";
  }
}

/**
 * @brief IRMA packing for B[n,k] which is shape divisable.
 */
FPMAT Data_transform_irma(const FPMAT& b) {
  int n = b.size();
  int k = b[0].size();
  AIR_ASSERT_MSG(((n % k == 0) || (k % n == 0)), "shape-divisible: n=%d, k=%d",
                 n, k);

  int nd = std::min(n, k);
  int kd = std::max(n, k);

  FPMAT m(nd, FPVEC(kd, 0.0));

  for (int h = 0; h < nd; h++) {
    int i = (n - h) % n;
    int j = 0;
    for (int w = 0; w < kd; w++) {
      m[h][w] = b[i][j];
      i       = (i + 1) % n;
      j       = (j + 1) % k;
    }
  }

  return m;
}

/**
 * @brief 2D Transpose_diagonal utils. TODO: a new util file.
 *
 * @param A: input 2D
 * @param position: where to start the diag
 * @param padw: pad A's width
 */
FPVEC Transpose_diagonal_fast(FPMAT A, size_t position, size_t padw) {
  // = rows of A
  int64_t n = A.size();
  int64_t k = A[0].size();
  AIR_ASSERT_MSG(((n >= k) && (n % k == 0)) || ((k > n) && (k % n == 0)),
                 "Diag shape constraint DIVIDE: n=%d, k=%d", n, k);

  int     num_part = (n <= k) ? 1 : (n / k);
  int64_t len_diag = num_part * k;

  FPVEC  diag(len_diag, 0);
  size_t i = 0, j = position;

  for (size_t d = 0; d < len_diag; d++) {
    diag[d] = A[i][j];
    i       = (i + 1) % n;
    j       = (j + 1) % k;
  }

  return diag;
}

// slow version
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

FPMAT Expand_with_zero(const float* input, const int num_rows,
                       const int num_cols, const int group_size,
                       const int stride, const int trailing_zero_num,
                       const int target_cols, const int vds_in_channel,
                       const int channel_size) {
  FPMAT output(num_rows);

  for (int i = 0; i < num_rows; ++i) {
    FPVEC current_row;

    for (int j = 0; j < num_cols; j += group_size) {
      // Copy and process 14 elements
      for (int k = 0; k < group_size && j + k < num_cols; ++k) {
        current_row.push_back(input[i * num_cols + j + k]);
        // current_row.push_back(0);  // Insert a zero after each element
        current_row.insert(current_row.end(), stride - 1, 0);
      }
      // Insert 28 zeros after each group of 14 elements
      current_row.insert(current_row.end(), trailing_zero_num, 0);
      // when there are gaps due to padding, extra 0 need to be inserted
      if ((j + group_size) % vds_in_channel == 0) {
        current_row.insert(current_row.end(),
                           channel_size - group_size * (trailing_zero_num +
                                                        group_size * stride),
                           0);
      }
    }
    current_row.resize(target_cols, 0);
    output[i] = current_row;
  }

  return output;
}

int Get_valid_data_in_row(CONSTANT_PTR slice_sizes,
                          CONSTANT_PTR slice_strides) {
  int64_t ss_height = 0, ss_width = 0;
  Get_const_array_value(slice_sizes, ss_height, ss_width);

  int64_t stride_row = 0, stride_col = 0;
  Get_const_array_value(slice_strides, stride_row, stride_col);
  AIR_ASSERT_MSG(stride_row == stride_col, "only support same stride size");
  AIR_ASSERT_MSG(ss_height / stride_row == ss_width / stride_col,
                 "valid data in rows and columns should be equal");

  return ss_height / stride_row;
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

  // offset for h!=w. TODO: unify all cases.
  if (h != w) {
    for (int i = 0; i < kh; i++) {
      for (int j = 0; j < kw; j++) {
        ra[i * kh + j] = w * (i - (kh - 1) / 2) + (j - (kh - 1) / 2);
      }
    }
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

void Masking_stride_data_in_vec(int h, int w, int channel, int padding,
                                int stride, int kh, FPVEC& input) {
  AIR_ASSERT_MSG(((stride > 1) && (padding != 0)) ||
                     ((stride > 1) && (padding == 0) && (kh == 1)),
                 "Masking_stride_data_in_vec execute condition is not correct");
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
                                        int kh, FPMAT& input) {
  AIR_ASSERT_MSG(
      ((stride > 1) && (padding != 0)) ||
          ((stride > 1) && (padding == 0) && (kh == 1)),
      "Masking_padding_stride_data_in_mat execute condition is not correct");
  for (int i = 0; i < first_dim; i++) {
    Masking_stride_data_in_vec(h, w, channel, padding, stride, kh, input[i]);
  }
}

// split the 4D tensor along 0 and 1 dimensions by x and y.
std::vector<std::vector<float>> Split_tensor(const std::vector<float>& input,
                                             int dim0, int dim1, int dim2,
                                             int dim3, int x, int y) {
  AIR_ASSERT((dim0 * dim1 * dim2 * dim3 == input.size()) && (dim0 % x == 0) &&
             (dim1 % y == 0));

  int block_dim0       = dim0 / x;
  int block_dim1       = dim1 / y;
  int block_dim23_size = dim2 * dim3;

  std::vector<std::vector<float>> split_tensors;

  for (int idx0 = 0; idx0 < x; ++idx0) {
    for (int idx1 = 0; idx1 < y; ++idx1) {
      std::vector<float> split_tensor;
      for (int b = 0; b < block_dim0; b++) {
        int start_index = (idx0 * block_dim0 + b) * dim1 * block_dim23_size +
                          idx1 * block_dim1 * block_dim23_size;
        int end_index = start_index + (block_dim1 * block_dim23_size);

        split_tensor.insert(split_tensor.end(), input.begin() + start_index,
                            input.begin() + end_index);
      }
      AIR_ASSERT(split_tensor.size() == input.size() / (x * y));
      split_tensors.push_back(split_tensor);
    }
  }

  return split_tensors;
}

// Weight sharding design choices:
// 1) weight-block transpose one by one, then compose.
// 2) Original weight transpose and then paritition. need handle halosize.
// 2 is better: equivalence (??), and save transpose time.
// we implement 1 for simplicity.
// Output: x*y small_weight, each shape [kh*kw][ho*wo/z+2*halosize]
// Note: for conv sharding, each conv-block still use metakernel.
// So conv-block: default "SAME" padding, output.shape=input.shape
NODE_PTR Transpose_weight2row_sharding(CONTAINER* cntr, NODE_PTR weight_node,
                                       int64_t x, int64_t y, int64_t z,
                                       int64_t halosize, int64_t channel_in,
                                       int64_t input_height,
                                       int64_t input_width, int64_t channel_out,
                                       int64_t kernel_height,
                                       int64_t kernel_width, int stride) {
  GLOB_SCOPE* gscope = cntr->Glob_scope();
  // ild (-array (--ldca weight_const; --ld...;))
  CONSTANT_PTR weight_const = weight_node->Child(0)->Child(0)->Const();

  int64_t input_size        = input_height * input_width;
  int64_t kernel_size       = kernel_height * kernel_width;
  int64_t weight_block_size = channel_out * channel_in * kernel_size;

  //  block size: weight_const is [x*y][block_data]
  AIR_ASSERT_MSG(x * y == weight_const->Type()->Cast_to_arr()->Elem_count(),
                 "Transpose_weight2row_sharding verify weight_size");

  const float* cptr = weight_const->Array_ptr<float>();

  FPVEC largeweight(cptr, cptr + weight_block_size * x * y);
  FPVEC largeweight2row;

  // Original large weight is splitted into x*y blocks.
  std::vector<FPVEC> largeweight_blocks =
      Split_tensor(largeweight, channel_out * x, channel_in * y, kernel_height,
                   kernel_width, x, y);
  AIR_ASSERT_MSG(largeweight_blocks.size() == x * y,
                 "largeweight_blocks.size = x*y");

  int64_t weight2row_block_size =
      channel_in * kernel_size * channel_out * input_size;
  // Total x*y ke2col matrices are constructed.
  // They are concatanated to ke2col_shard (or new_largeweight).
  // for x: for y: conv_part(input_part, ild new_largeweight[x][y])"
  // TODO: define a common ke2col funciton for readability.
  for (int xi = 0; xi < x; xi++)
    for (int yi = 0; yi < y; yi++) {
      int              index = xi * y + yi;
      FPMAT            weight2row_block(channel_in * kernel_size,
                                        FPVEC(channel_out * input_size, 0.0));
      std::vector<int> ra(kernel_size, 0);
      // [channel_in*kernel_size, *]
      Get_im2col_kernel(largeweight_blocks[index], channel_in, input_height,
                        input_width, channel_out, kernel_height, kernel_width,
                        1, 1 /*stride*/, ra, weight2row_block);
      AIR_ASSERT(weight2row_block.size() * weight2row_block[0].size() ==
                 weight2row_block_size);

      for (int i = 0; i < channel_in; i++) {
        for (int j = 0; j < kernel_size; j++) {
          int index = i * kernel_size + j;
          // channel_out * input_size
          int overlap = halosize * input_width;
          // halo to 0
          for (int l = 0; l < overlap; l++) weight2row_block[index][l] = 0.0;
          for (int l = input_size - overlap; l < input_size; l++)
            weight2row_block[index][l] = 0.0;
          // stride position to 0
          for (int c = 0; c < channel_out; c++) {
            for (int h = 0; h < input_height - 2 * halosize; h++) {
              for (int w = 0; w < input_width; w++) {
                if ((h % stride != 0) || (w % stride != 0))
                  weight2row_block[index][c * input_size +
                                          (halosize + h) * input_width + w] =
                      0.0;
              }
            }
          }
        }
      }

      for (int i = 0; i < channel_in; i++) {
        for (int j = 0; j < kernel_size; j++) {
          int index = i * kernel_size + j;
          std::rotate(weight2row_block[index].begin(),
                      weight2row_block[index].begin() +
                          weight2row_block[index].size() -
                          (i % channel_out) * input_size,
                      weight2row_block[index].end());

          largeweight2row.insert(largeweight2row.end(),
                                 weight2row_block[index].begin(),
                                 weight2row_block[index].end());
        }
      }
    }

  AIR_ASSERT_MSG(largeweight2row.size() == x * y * weight2row_block_size,
                 "largeweight_t size mismatch");
  // Very important: weightT_sharding
  // 2D[channel_in(y)*kernel_size*meshx, input_height*input_width/meshx]
  //
  std::vector<int64_t> weight_shardingt_shape{
      channel_in * kernel_size * x * y,
      channel_out * input_height * input_width};
  std::string weight_shardingt_str =
      New_array_name("weight_shardingT", weight_shardingt_shape);
  CONSTANT_PTR weight_shardingt_const = New_array_const(
      gscope, weight_shardingt_str.c_str(), largeweight2row.size(),
      weight_node->Rtype()->Cast_to_arr()->Elem_type(), weight_shardingt_shape,
      (void*)largeweight2row.data(), weight_node->Spos());
  NODE_PTR new_largeweight =
      cntr->New_ldc(weight_shardingt_const, weight_node->Spos());
  return new_largeweight;
}

void Masking_no_padding_stride_data_in_vec(int h, int w, int channel,
                                           int padding, int stride, int kh,
                                           int kw, FPVEC& input,
                                           int fhe_padsize) {
  AIR_ASSERT_MSG(padding == 0,
                 "Masking_no_padding_stride_data_in_vec is only needed when "
                 "real padding == 0");
  AIR_ASSERT_MSG(kh == kw, "Masking_no_padding_stride_data_in_vec: kh != kw");
  int padsize = (kh - 1) / 2;
  if (fhe_padsize > 0) {
    padsize = fhe_padsize;
  }

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
    // need to support stride > 2
    AIR_ASSERT(stride == 2);
    for (int i = 0; i < channel; i++) {
      // start from padsize since above loop complete mask work for
      // padding
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
                                           int stride, FPMAT& input,
                                           int fhe_padsize) {
  AIR_ASSERT_MSG(padding == 0,
                 "Masking_no_padding_stride_data_in_mat is only needed "
                 "when padding = 0");

  for (int i = 0; i < first_dim; i++) {
    Masking_no_padding_stride_data_in_vec(h, w, channel, padding, stride, kh,
                                          kw, input[i], fhe_padsize);
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

FPVEC Get_gap_mask(int c_in, int h, int w, bool fuse_avg_val) {
  float mask_val = 1.0;
  if (fuse_avg_val) {
    mask_val = 1.0 / (h * w);
  }
  bool  channel_begin = true;
  FPVEC avg_mask(c_in * h * w, 0.0);
  for (int i = 0; i < c_in; i++) {
    for (int j = 0; j < h; j++) {
      for (int k = 0; k < w; k++) {
        if (channel_begin) {
          avg_mask[i * h * w + j * w + k] = mask_val;
          channel_begin                   = false;
        }
      }
    }
    channel_begin = true;
  }
  return avg_mask;
}

// used to eliminate the invalid data in channel, put valid data together
// in channel
FPVEC Get_dic_comb_mask(int c_in, int h, int w, int stride) {
  int   oh                   = h / stride;
  int   ow                   = w / stride;
  int   c_size               = h * w;
  int   vd_block_in_row      = ceil(double(ow) / stride);
  int   vd_blocks_in_channel = vd_block_in_row * oh;
  FPMAT mask_mat(vd_blocks_in_channel, FPVEC(c_in * h * w, 0.0));

  if (ow % stride == 0) {
    for (int q = 0; q < vd_blocks_in_channel; q++) {
      int pos1 = q * 2;
      int pos2 = q * 2 + 1;
      for (int i = 0; i < c_in; i++) {
        mask_mat[q][i * c_size + pos1] = 1;
        mask_mat[q][i * c_size + pos2] = 1;
      }
    }
  } else {
    AIR_ASSERT(ow % stride == 1);
    int pos = 0;
    for (int i = 0; i < oh; i++) {
      for (int j = 0; j < vd_block_in_row; j++) {
        bool not_equal_block_count = false;
        if ((j + 1) != vd_block_in_row) {
          not_equal_block_count = true;
        }
        int q = i * vd_block_in_row + j;
        for (int k = 0; k < c_in; k++) {
          mask_mat[q][k * c_size + pos] = 1;
          if (not_equal_block_count) {
            mask_mat[q][k * c_size + pos + 1] = 1;
          }
        }
        if (not_equal_block_count) {
          pos += 2;
        } else {
          pos++;
        }
      }
    }
  }

  FPVEC mask_vec;
  for (int i = 0; i < vd_blocks_in_channel; i++) {
    mask_vec.insert(end(mask_vec), begin(mask_mat[i]), end(mask_mat[i]));
  }

  return mask_vec;
}

// used to eliminate the invalid data in rows
FPVEC Get_large_row_mask(int c_in, int h, int w, int stride) {
  FPVEC mask_vec = FPVEC(c_in * h * w, 0.0);

  for (int i = 0; i < c_in; i++) {
    for (int j = 0; j < h; j++) {
      for (int k = 0; k < w; k += stride * 2) {
        if ((j % stride != 1) && (k + 1 != w)) {
          mask_vec[i * h * w + j * w + k]     = 1.0;
          mask_vec[i * h * w + j * w + k + 1] = 1.0;
        }
      }
    }
  }

  return mask_vec;
}

// used to eliminate the invalid data between channels
FPVEC Get_large_channel_mask(int c_in, int h, int w, int stride) {
  FPVEC mask_vec = FPVEC(c_in * h * w, 0.0);

  for (int i = 0; i < c_in; i += stride * 2) {
    for (int j = 0; j < h; j++) {
      for (int k = 0; k < w; k++) {
        mask_vec[i * h * w + j * w + k] = 1.0;
      }
    }
  }

  return mask_vec;
}

// used to eliminate the invalid data in rows, put valid data together in
// rows
FPVEC Get_row_combine_mask(int c_in, int h, int w, int valid_data_num,
                           int valid_data_group_num) {
  int   matrix_row_size = valid_data_group_num;
  FPMAT mask_matrix(matrix_row_size, FPVEC(c_in * h * w, 0.0));

  for (int q = 0; q < matrix_row_size; q++) {
    for (int i = 0; i < c_in; i++) {
      for (int j = 0; j < h; j++) {
        int num_in_block = 0;
        for (int k = 0; k < w; k++) {
          if ((j % 2 != 1) && (k >= q * valid_data_num) &&
              (num_in_block < valid_data_num)) {
            num_in_block++;
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
FPVEC Get_rc_combine_mask(int c_in, int h, int w, int gvdn,
                          int valid_data_group_num, int last_gvdn) {
  int   matrix_row_size = valid_data_group_num;
  FPMAT mask_matrix(matrix_row_size, FPVEC(c_in * h * w, 0.0));

  int start          = 0;
  int valid_data_num = gvdn;
  for (int q = 0; q < matrix_row_size; q++) {
    if ((q == (matrix_row_size - 1)) && (last_gvdn != 0)) {
      valid_data_num = last_gvdn;
    }
    for (int i = 0; i < c_in; i++) {
      for (int j = 0; j < h; j++) {
        for (int k = 0; k < w; k++) {
          if (((i * h * w + j * h + k) >= (i * h * w + start)) &&
              ((i * h * w + j * h + k) <
               (i * h * w + start + valid_data_num))) {
            mask_matrix[q][i * h * w + j * h + k] = 1.0;
          }
        }
      }
    }
    start += valid_data_num;
  }

  FPVEC mask_vec;
  for (int i = 0; i < matrix_row_size; i++) {
    mask_vec.insert(end(mask_vec), begin(mask_matrix[i]), end(mask_matrix[i]));
  }

  return mask_vec;
}

// a consecutive piece of 1 mask, others are all 0, no matter what
// channels
FPVEC Get_cc_combine_mask(int c_in, int h, int w, int gvdn,
                          int valid_data_group_num, int last_gvdn) {
  // valid_data_num stands for the number of consecutive 1
  int   matrix_row_size = valid_data_group_num;
  FPMAT mask_matrix(matrix_row_size, FPVEC(c_in * h * w, 0.0));

  // start stands for the beginning position of consecutive 1
  int start          = 0;
  int valid_data_num = gvdn;
  for (int q = 0; q < matrix_row_size; q++) {
    if ((q == (matrix_row_size - 1)) && (last_gvdn != 0)) {
      valid_data_num = last_gvdn;
    }
    for (int i = 0; i < c_in; i++) {
      for (int j = 0; j < h; j++) {
        for (int k = 0; k < w; k++) {
          if (((i * h * w + j * h + k) >= start) &&
              ((i * h * w + j * h + k) < (start + valid_data_num))) {
            mask_matrix[q][i * h * w + j * h + k] = 1.0;
          }
        }
      }
    }
    start += valid_data_num;
  }

  FPVEC mask_vec;
  for (int i = 0; i < matrix_row_size; i++) {
    mask_vec.insert(end(mask_vec), begin(mask_matrix[i]), end(mask_matrix[i]));
  }

  return mask_vec;
}

int Get_log_cc_lub(int channel, int ih, int iw, int stride) {
  int loop_ub = stride;
  if ((stride == ih) && (stride == iw)) {
    // similar to global average pool
    loop_ub = log2(ih * iw);
  }
  // when only two valid channel exist, 1 time combine is enough
  if (channel == 2) {
    loop_ub = 1;
  }
  return loop_ub;
}

int Get_log_rc_lub(int interval, int oh, int ow) {
  int loop_ub = ceil(log2(interval / ow));
  // when only two valid rows exist, 1 time combine is enough
  if (oh == 2) {
    loop_ub = 1;
  }
  return loop_ub;
}

bool Enable_large_channel_comb(int channel, int stride) {
  // need more precise cost model to determine the large channel border
  bool is_large_channel   = true ? (channel >= LARGE_CHANNEL_BORDER) : false;
  bool is_support_channel = Is_power_of_two(channel);
  // may support more stride value later
  bool is_support_stride = true ? (stride == 2) : false;

  return is_large_channel && is_support_channel && is_support_stride;
}

bool Enable_log_cc_comb(int channel, int ih, int iw, int oh, int ow) {
  // when all the valid output data can be put into 1 input channel
  bool is_slot_enough = true ? (ih * iw >= oh * ow * channel) : false;
  // may support not power of two channel later
  bool is_support_channel = Is_power_of_two(channel);
  return is_slot_enough && is_support_channel;
}

bool Enable_1level_channel_comb(int ih, int iw) {
  // need more precise cost model to determine the tiny height/width border
  bool is_tiny_hw =
      true ? ((ih <= TINY_HW_BORDER) && (ih <= TINY_HW_BORDER)) : false;
  return is_tiny_hw;
}

}  // namespace vector
}  // namespace nn
