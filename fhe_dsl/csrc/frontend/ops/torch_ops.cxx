//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// PyTorch Custom Operators for AIR IR Generation
//
// This file implements custom PyTorch operators (torch.ops.tensor.xxx) that
// generate AIR IR instead of computing results.
//
// Each tensor_xxx_impl() builds an OP_CONTEXT with inputs and attrs, then
// delegates to TORCH_OP_HANDLER::Execute() which handles the common pattern:
//   1. Is_building() guard
//   2. Resolve input tensor names from symbol table
//   3. Apply schema defaults to attrs
//   4. Build metadata from thread-local state
//   5. Compute output shape via registered SHAPE_FN
//   6. Call Add_operation_cpp()
//   7. Clone + register result tensor

#include "ace/frontend/ops/torch_ops.h"
#include "ace/frontend/ops/torch_op_handler.h"

#include <torch/library.h>
#include <c10/core/ScalarType.h>

namespace ace {
namespace frontend {

// ============================================================================
// Binary Operators (2 inputs)
// ============================================================================

at::Tensor tensor_add_impl(const at::Tensor& x, const at::Tensor& y) {
    OP_CONTEXT ctx;
    ctx._input = {x, y};
    return TORCH_OP_HANDLER::Execute("add", ctx);
}

at::Tensor tensor_sub_impl(const at::Tensor& x, const at::Tensor& y) {
    OP_CONTEXT ctx;
    ctx._input = {x, y};
    return TORCH_OP_HANDLER::Execute("sub", ctx);
}

at::Tensor tensor_mul_impl(const at::Tensor& x, const at::Tensor& y) {
    OP_CONTEXT ctx;
    ctx._input = {x, y};
    return TORCH_OP_HANDLER::Execute("mul", ctx);
}

at::Tensor tensor_div_impl(const at::Tensor& x, const at::Tensor& y) {
    OP_CONTEXT ctx;
    ctx._input = {x, y};
    return TORCH_OP_HANDLER::Execute("div", ctx);
}

at::Tensor tensor_matmul_impl(const at::Tensor& x, const at::Tensor& y) {
    OP_CONTEXT ctx;
    ctx._input = {x, y};
    return TORCH_OP_HANDLER::Execute("matmul", ctx);
}

at::Tensor tensor_concat_impl(const at::Tensor& x, const at::Tensor& y, int64_t axis) {
    OP_CONTEXT ctx;
    ctx._input = {x, y};
    ctx._attr["axis"] = static_cast<int>(axis);
    return TORCH_OP_HANDLER::Execute("concat", ctx);
}

// ============================================================================
// Unary Operators (1 input)
// ============================================================================

at::Tensor tensor_relu_impl(const at::Tensor& x) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    return TORCH_OP_HANDLER::Execute("relu", ctx);
}

at::Tensor tensor_softmax_impl(const at::Tensor& x, int64_t axis) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    ctx._attr["axis"] = static_cast<int>(axis);
    return TORCH_OP_HANDLER::Execute("softmax", ctx);
}

at::Tensor tensor_max_pool_impl(const at::Tensor& x,
                                 const std::vector<int64_t>& kernel_size,
                                 const std::vector<int64_t>& stride,
                                 const std::vector<int64_t>& padding) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    if (!kernel_size.empty()) {
        ctx._attr["kernel_shape"] = std::vector<int>(kernel_size.begin(), kernel_size.end());
    }
    if (!stride.empty()) {
        ctx._attr["strides"] = std::vector<int>(stride.begin(), stride.end());
    }
    if (!padding.empty()) {
        ctx._attr["pads"] = std::vector<int>(padding.begin(), padding.end());
    }
    return TORCH_OP_HANDLER::Execute("max_pool", ctx);
}

at::Tensor tensor_average_pool_impl(const at::Tensor& x,
                                     const std::vector<int64_t>& kernel_size,
                                     const std::vector<int64_t>& stride,
                                     const std::vector<int64_t>& padding) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    if (!kernel_size.empty()) {
        ctx._attr["kernel_shape"] = std::vector<int>(kernel_size.begin(), kernel_size.end());
    }
    if (!stride.empty()) {
        ctx._attr["strides"] = std::vector<int>(stride.begin(), stride.end());
    }
    if (!padding.empty()) {
        ctx._attr["pads"] = std::vector<int>(padding.begin(), padding.end());
    }
    return TORCH_OP_HANDLER::Execute("average_pool", ctx);
}

at::Tensor tensor_global_average_pool_impl(const at::Tensor& x) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    return TORCH_OP_HANDLER::Execute("global_average_pool", ctx);
}

at::Tensor tensor_flatten_impl(const at::Tensor& x, int64_t start_dim, int64_t end_dim) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    ctx._attr["start_dim"] = static_cast<int>(start_dim);
    ctx._attr["end_dim"] = static_cast<int>(end_dim);
    return TORCH_OP_HANDLER::Execute("flatten", ctx);
}

at::Tensor tensor_sqrt_impl(const at::Tensor& x) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    return TORCH_OP_HANDLER::Execute("sqrt", ctx);
}

at::Tensor tensor_silu_impl(const at::Tensor& x) {
    OP_CONTEXT ctx;
    ctx._input = {x};
    return TORCH_OP_HANDLER::Execute("silu", ctx);
}

// ============================================================================
// Ternary Operators (3 inputs)
// ============================================================================

at::Tensor tensor_conv_impl(const at::Tensor& x, const at::Tensor& w,
                             const at::Tensor* b,
                             const std::vector<int64_t>& kernel_size,
                             const std::vector<int64_t>& stride,
                             const std::vector<int64_t>& padding,
                             const std::vector<int64_t>& dilation,
                             int64_t groups) {
    OP_CONTEXT ctx;
    ctx._input = {x, w};
    if (b && b->defined()) ctx._input.push_back(*b);

    if (!kernel_size.empty()) {
        ctx._attr["kernel_shape"] = std::vector<int>(kernel_size.begin(), kernel_size.end());
    }
    if (!stride.empty()) {
        ctx._attr["strides"] = std::vector<int>(stride.begin(), stride.end());
    }
    if (!padding.empty()) {
        ctx._attr["pads"] = std::vector<int>(padding.begin(), padding.end());
    }
    if (!dilation.empty()) {
        ctx._attr["dilations"] = std::vector<int>(dilation.begin(), dilation.end());
    }
    ctx._attr["group"] = static_cast<int>(groups);
    return TORCH_OP_HANDLER::Execute("conv", ctx);
}

at::Tensor tensor_gemm_impl(const at::Tensor& a, const at::Tensor& b,
                             const at::Tensor* c,
                             double alpha, double beta,
                             int64_t transA, int64_t transB) {
    OP_CONTEXT ctx;
    ctx._input = {a, b};
    if (c && c->defined()) ctx._input.push_back(*c);

    ctx._attr["alpha"] = static_cast<float>(alpha);
    ctx._attr["beta"] = static_cast<float>(beta);
    ctx._attr["transA"] = static_cast<int>(transA);
    ctx._attr["transB"] = static_cast<int>(transB);
    return TORCH_OP_HANDLER::Execute("gemm", ctx);
}

// ============================================================================
// PyTorch Operator Registration
// ============================================================================

TORCH_LIBRARY(tensor, m) {
    // Binary operators
    m.def("add(Tensor x, Tensor y) -> Tensor");
    m.def("sub(Tensor x, Tensor y) -> Tensor");
    m.def("mul(Tensor x, Tensor y) -> Tensor");
    m.def("div(Tensor x, Tensor y) -> Tensor");
    m.def("matmul(Tensor x, Tensor y) -> Tensor");
    m.def("concat(Tensor x, Tensor y, int axis=-1) -> Tensor");

    // Unary operators
    m.def("relu(Tensor x) -> Tensor");
    m.def("softmax(Tensor x, int axis=-1) -> Tensor");
    m.def("max_pool(Tensor x, int[]? kernel_size=None, int[]? stride=None, int[]? padding=None) -> Tensor");
    m.def("average_pool(Tensor x, int[]? kernel_size=None, int[]? stride=None, int[]? padding=None) -> Tensor");
    m.def("global_average_pool(Tensor x) -> Tensor");
    m.def("flatten(Tensor x, int start_dim=1, int end_dim=-1) -> Tensor");
    m.def("sqrt(Tensor x) -> Tensor");
    m.def("silu(Tensor x) -> Tensor");
    m.def("reshape(Tensor x, Tensor shape) -> Tensor");

    // Ternary operators
    m.def("conv(Tensor x, Tensor w, Tensor? b=None, int[]? kernel_size=None, int[]? stride=None, int[]? padding=None, int[]? dilation=None, int groups=1) -> Tensor");
    m.def("gemm(Tensor a, Tensor b, Tensor? c=None, float alpha=1.0, float beta=1.0, int transA=0, int transB=0) -> Tensor");
}

TORCH_LIBRARY_IMPL(tensor, CPU, m) {
    // Binary operators
    m.impl("add", tensor_add_impl);
    m.impl("sub", tensor_sub_impl);
    m.impl("mul", tensor_mul_impl);
    m.impl("div", tensor_div_impl);
    m.impl("matmul", tensor_matmul_impl);
    m.impl("concat", [](const at::Tensor& x, const at::Tensor& y, int64_t axis) {
        return tensor_concat_impl(x, y, axis);
    });

    // Unary operators
    m.impl("relu", tensor_relu_impl);
    m.impl("softmax", [](const at::Tensor& x, int64_t axis) {
        return tensor_softmax_impl(x, axis);
    });
    m.impl("max_pool", [](const at::Tensor& x,
                          c10::OptionalArrayRef<int64_t> kernel_size,
                          c10::OptionalArrayRef<int64_t> stride,
                          c10::OptionalArrayRef<int64_t> padding) {
        std::vector<int64_t> ks = kernel_size.has_value() ? kernel_size.value().vec() : std::vector<int64_t>();
        std::vector<int64_t> st = stride.has_value() ? stride.value().vec() : std::vector<int64_t>();
        std::vector<int64_t> pd = padding.has_value() ? padding.value().vec() : std::vector<int64_t>();
        return tensor_max_pool_impl(x, ks, st, pd);
    });
    m.impl("average_pool", [](const at::Tensor& x,
                              c10::OptionalArrayRef<int64_t> kernel_size,
                              c10::OptionalArrayRef<int64_t> stride,
                              c10::OptionalArrayRef<int64_t> padding) {
        std::vector<int64_t> ks = kernel_size.has_value() ? kernel_size.value().vec() : std::vector<int64_t>();
        std::vector<int64_t> st = stride.has_value() ? stride.value().vec() : std::vector<int64_t>();
        std::vector<int64_t> pd = padding.has_value() ? padding.value().vec() : std::vector<int64_t>();
        return tensor_average_pool_impl(x, ks, st, pd);
    });
    m.impl("global_average_pool", tensor_global_average_pool_impl);
    m.impl("flatten", [](const at::Tensor& x, int64_t start_dim, int64_t end_dim) {
        return tensor_flatten_impl(x, start_dim, end_dim);
    });
    m.impl("sqrt", tensor_sqrt_impl);
    m.impl("silu", tensor_silu_impl);
    m.impl("reshape", [](const at::Tensor& x, const at::Tensor& shape) {
        OP_CONTEXT ctx;
        ctx._input = {x, shape};
        // Extract shape values from the shape tensor for attrs
        if (shape.dim() == 1) {
            auto shape_accessor = shape.accessor<int64_t, 1>();
            std::vector<int> shape_vals;
            for (int i = 0; i < shape_accessor.size(0); ++i) {
                shape_vals.push_back(static_cast<int>(shape_accessor[i]));
            }
            ctx._attr["shape"] = shape_vals;
        }
        return TORCH_OP_HANDLER::Execute("reshape", ctx);
    });

    // Ternary operators
    m.impl("conv", [](const at::Tensor& x, const at::Tensor& w,
                      const std::optional<at::Tensor>& b,
                      c10::OptionalArrayRef<int64_t> kernel_size,
                      c10::OptionalArrayRef<int64_t> stride,
                      c10::OptionalArrayRef<int64_t> padding,
                      c10::OptionalArrayRef<int64_t> dilation,
                      int64_t groups) {
        std::vector<int64_t> ks = kernel_size.has_value() ? kernel_size.value().vec() : std::vector<int64_t>();
        std::vector<int64_t> st = stride.has_value() ? stride.value().vec() : std::vector<int64_t>();
        std::vector<int64_t> pd;
        if (padding.has_value()) {
            pd = padding.value().vec();
        }
        std::vector<int64_t> dl = dilation.has_value() ? dilation.value().vec() : std::vector<int64_t>();
        const at::Tensor* b_ptr = (b.has_value() && b.value().defined()) ? &b.value() : nullptr;
        return tensor_conv_impl(x, w, b_ptr, ks, st, pd, dl, groups);
    });
    m.impl("gemm", [](const at::Tensor& a, const at::Tensor& b,
                      const std::optional<at::Tensor>& c,
                      double alpha, double beta,
                      int64_t transA, int64_t transB) {
        const at::Tensor* c_ptr = (c.has_value() && c.value().defined()) ? &c.value() : nullptr;
        return tensor_gemm_impl(a, b, c_ptr, alpha, beta, transA, transB);
    });
}

}  // namespace frontend
}  // namespace ace