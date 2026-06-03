//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CUSTOM_OPS_TORCH_OPS_H
#define AIR_CUSTOM_OPS_TORCH_OPS_H

// This file contains declarations for PyTorch custom operators that generate
// AIR IR instead of computing results. These operators are registered with
// PyTorch via TORCH_LIBRARY and can be called as torch.ops.tensor.xxx.
//
// Each tensor_xxx_impl() builds an OP_CONTEXT and delegates to
// TORCH_OP_HANDLER::Execute() for the common AIR IR generation pattern.
//
// Thread-local op context state (Set_Current_Op_Name, Set_Is_Output_Op, etc.)
// is managed by TORCH_OP_HANDLER (see torch_op_handler.h).

#include <string>
#include <vector>

// Forward declarations to avoid including torch headers in this header
namespace at {
class Tensor;
}

namespace ace {
namespace frontend {

// ============================================================================
// PyTorch Custom Tensor Operators
// These operators generate AIR IR instead of computing results.
// Each builds an OP_CONTEXT and delegates to TORCH_OP_HANDLER::Execute().
// ============================================================================

// Binary operators (2 inputs)
at::Tensor tensor_add_impl(const at::Tensor& x, const at::Tensor& y);
at::Tensor tensor_sub_impl(const at::Tensor& x, const at::Tensor& y);
at::Tensor tensor_mul_impl(const at::Tensor& x, const at::Tensor& y);
at::Tensor tensor_div_impl(const at::Tensor& x, const at::Tensor& y);
at::Tensor tensor_matmul_impl(const at::Tensor& x, const at::Tensor& y);
at::Tensor tensor_concat_impl(const at::Tensor& x, const at::Tensor& y, int64_t axis);

// Unary operators (1 input)
at::Tensor tensor_relu_impl(const at::Tensor& x);
at::Tensor tensor_softmax_impl(const at::Tensor& x, int64_t axis);
at::Tensor tensor_max_pool_impl(const at::Tensor& x,
                                 const std::vector<int64_t>& kernel_size,
                                 const std::vector<int64_t>& stride,
                                 const std::vector<int64_t>& padding);
at::Tensor tensor_average_pool_impl(const at::Tensor& x,
                                     const std::vector<int64_t>& kernel_size,
                                     const std::vector<int64_t>& stride,
                                     const std::vector<int64_t>& padding);
at::Tensor tensor_global_average_pool_impl(const at::Tensor& x);
at::Tensor tensor_flatten_impl(const at::Tensor& x, int64_t start_dim, int64_t end_dim);
at::Tensor tensor_sqrt_impl(const at::Tensor& x);
at::Tensor tensor_silu_impl(const at::Tensor& x);

// Ternary operators (3 inputs)
at::Tensor tensor_conv_impl(const at::Tensor& x, const at::Tensor& w,
                             const at::Tensor* b,
                             const std::vector<int64_t>& kernel_size,
                             const std::vector<int64_t>& stride,
                             const std::vector<int64_t>& padding,
                             const std::vector<int64_t>& dilation,
                             int64_t groups);
at::Tensor tensor_gemm_impl(const at::Tensor& a, const at::Tensor& b,
                             const at::Tensor* c,
                             double alpha, double beta,
                             int64_t transA, int64_t transB);

}  // namespace frontend
}  // namespace ace

#endif