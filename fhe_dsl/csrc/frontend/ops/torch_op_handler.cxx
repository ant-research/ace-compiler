//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// TORCH_OP_HANDLER Implementation
//
// Utility class for PyTorch custom op AIR IR generation.
// All methods are static — this class holds no instance state.
// See torch_op_handler.h for class overview.

#include "ace/frontend/ops/torch_op_handler.h"
#include "ace/frontend/core/ir_builder.h"
#include "ace/frontend/ops/op_schema.h"

namespace ace {
namespace frontend {

// ============================================================================
// Helper: Get tensor shape as vector<int64_t>
// ============================================================================

// ============================================================================
// Tensor Name Registry (data_ptr → name, for Path 1 / direct mode)
// ============================================================================

std::unordered_map<uintptr_t, std::string>&
TORCH_OP_HANDLER::Get_tensor_name_map() {
    static std::unordered_map<uintptr_t, std::string> map;
    return map;
}

void TORCH_OP_HANDLER::Register_tensor_name(uintptr_t data_ptr,
                                             const std::string& name) {
    Get_tensor_name_map()[data_ptr] = name;
}

std::string TORCH_OP_HANDLER::Lookup_tensor_name(uintptr_t data_ptr) {
    auto& map = Get_tensor_name_map();
    auto it = map.find(data_ptr);
    return (it != map.end()) ? it->second : "";
}

void TORCH_OP_HANDLER::Clear_tensor_names() {
    Get_tensor_name_map().clear();
}

std::vector<int64_t> TORCH_OP_HANDLER::Get_tensor_shape(const at::Tensor& t) {
    return std::vector<int64_t>(t.sizes().begin(), t.sizes().end());
}

// ============================================================================
// Shape computation primitives
// ============================================================================

int64_t TORCH_OP_HANDLER::Compute_conv_output_dim(
    int64_t input_dim, int64_t pad, int64_t dilation,
    int64_t kernel, int64_t stride) {
    return (input_dim + 2 * pad - dilation * (kernel - 1) - 1) / stride + 1;
}

int64_t TORCH_OP_HANDLER::Compute_pool_output_dim(
    int64_t input_dim, int64_t pad, int64_t kernel, int64_t stride) {
    return (input_dim + 2 * pad - kernel - 1) / stride + 1;
}

// ============================================================================
// Shape Functions (one per category)
// ============================================================================

std::vector<int64_t> TORCH_OP_HANDLER::Passthrough_shape(const OP_CONTEXT& ctx) {
    return Get_tensor_shape(ctx.Input_at(0));
}

std::vector<int64_t> TORCH_OP_HANDLER::Matmul_shape(const OP_CONTEXT& ctx) {
    auto output_shape = Get_tensor_shape(ctx.Input_at(0));
    const auto& y = ctx.Input_at(1);
    if (ctx.Input_at(0).dim() >= 2 && y.dim() >= 2) {
        output_shape.back() = y.size(-1);
    }
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Concat_shape(const OP_CONTEXT& ctx) {
    const auto& x = ctx.Input_at(0);
    const auto& y = ctx.Input_at(1);
    auto output_shape = Get_tensor_shape(x);

    int64_t axis = ctx.Get_attr<int>("axis", -1);
    int normalized_axis = static_cast<int>(axis);
    if (normalized_axis < 0) {
        normalized_axis += static_cast<int>(output_shape.size());
    }
    if (normalized_axis >= 0 &&
        normalized_axis < static_cast<int>(output_shape.size())) {
        output_shape[normalized_axis] += y.size(normalized_axis);
    }
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Pool_shape(const OP_CONTEXT& ctx) {
    const auto& x = ctx.Input_at(0);
    auto output_shape = Get_tensor_shape(x);

    if (output_shape.size() < 4) return output_shape;

    auto kernel_shape = ctx.Get_attr_vec<int>("kernel_shape");
    auto strides = ctx.Get_attr_vec<int>("strides");
    auto pads = ctx.Get_attr_vec<int>("pads");

    if (kernel_shape.empty()) return output_shape;

    int64_t h = output_shape[2], w = output_shape[3];
    int64_t kh = kernel_shape[0];
    int64_t kw = (kernel_shape.size() > 1) ? kernel_shape[1] : kh;
    int64_t sh = (!strides.empty()) ? strides[0] : kh;
    int64_t sw = (strides.size() > 1) ? strides[1] : sh;
    int64_t ph = (!pads.empty()) ? pads[0] : 0;
    int64_t pw = (pads.size() > 1) ? pads[1] : ph;

    output_shape[2] = Compute_pool_output_dim(h, ph, kh, sh);
    output_shape[3] = Compute_pool_output_dim(w, pw, kw, sw);
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Global_pool_shape(const OP_CONTEXT& ctx) {
    auto output_shape = Get_tensor_shape(ctx.Input_at(0));
    for (size_t i = 2; i < output_shape.size(); ++i) {
        output_shape[i] = 1;
    }
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Flatten_shape(const OP_CONTEXT& ctx) {
    auto input_shape = Get_tensor_shape(ctx.Input_at(0));
    int ndim = static_cast<int>(input_shape.size());
    int start_dim = ctx.Get_attr<int>("start_dim", 1);
    int end_dim = ctx.Get_attr<int>("end_dim", -1);

    if (start_dim < 0) start_dim += ndim;
    if (end_dim < 0) end_dim += ndim;

    std::vector<int64_t> output_shape;
    for (int i = 0; i < start_dim; ++i) {
        output_shape.push_back(input_shape[i]);
    }
    int64_t flat_size = 1;
    for (int i = start_dim; i <= end_dim; ++i) {
        flat_size *= input_shape[i];
    }
    output_shape.push_back(flat_size);
    for (int i = end_dim + 1; i < ndim; ++i) {
        output_shape.push_back(input_shape[i]);
    }
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Gemm_shape(const OP_CONTEXT& ctx) {
    const auto& a = ctx.Input_at(0);
    const auto& b = ctx.Input_at(1);
    auto output_shape = Get_tensor_shape(a);

    if (b.dim() >= 2) {
        int64_t batch = (output_shape.size() > 0) ? output_shape[0] : 1;
        int transB = ctx.Get_attr<int>("transB", 0);
        if (transB) {
            output_shape = {batch, b.size(0)};
        } else {
            output_shape = {batch, b.size(1)};
        }
    }
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Conv_shape(const OP_CONTEXT& ctx) {
    const auto& x = ctx.Input_at(0);
    const auto& w = ctx.Input_at(1);
    auto output_shape = Get_tensor_shape(x);

    if (output_shape.size() < 4) return output_shape;

    auto kernel_shape = ctx.Get_attr_vec<int>("kernel_shape");
    auto strides = ctx.Get_attr_vec<int>("strides");
    auto pads = ctx.Get_attr_vec<int>("pads");
    auto dilations = ctx.Get_attr_vec<int>("dilations");

    int64_t batch = output_shape[0];
    int64_t out_channels = w.size(0);
    int64_t h = output_shape[2], w_dim = output_shape[3];

    int64_t kh = (!kernel_shape.empty()) ? kernel_shape[0] :
                 ((w.dim() >= 3) ? w.size(2) : 1);
    int64_t kw = (kernel_shape.size() > 1) ? kernel_shape[1] :
                 ((w.dim() >= 4) ? w.size(3) : kh);
    int64_t sh = (!strides.empty()) ? strides[0] : 1;
    int64_t sw = (strides.size() > 1) ? strides[1] : sh;
    int64_t ph = (!pads.empty()) ? pads[0] : 0;
    int64_t pw = (pads.size() > 1) ? pads[1] : ph;
    int64_t dh = (!dilations.empty()) ? dilations[0] : 1;
    int64_t dw = (dilations.size() > 1) ? dilations[1] : dh;

    output_shape[0] = batch;
    output_shape[1] = out_channels;
    output_shape[2] = Compute_conv_output_dim(h, ph, dh, kh, sh);
    output_shape[3] = Compute_conv_output_dim(w_dim, pw, dw, kw, sw);
    return output_shape;
}

std::vector<int64_t> TORCH_OP_HANDLER::Reshape_shape(const OP_CONTEXT& ctx) {
    const auto& shape_tensor = ctx.Input_at(1);
    if (shape_tensor.dim() == 1) {
        auto accessor = shape_tensor.accessor<int64_t, 1>();
        std::vector<int64_t> output_shape;
        for (int i = 0; i < accessor.size(0); ++i) {
            output_shape.push_back(accessor[i]);
        }
        return output_shape;
    }
    return Get_tensor_shape(ctx.Input_at(0));
}

// ============================================================================
// Common Steps (all static)
// ============================================================================

std::vector<std::string> TORCH_OP_HANDLER::Resolve_input_names(
    const OP_CONTEXT& ctx) {
    // Priority 1: use OP_CONTEXT._input_name (forward name passing)
    if (!ctx._input_name.empty()) {
        return ctx._input_name;
    }

    // Priority 2: look up from TORCH_OP_HANDLER's internal data_ptr→name map
    std::vector<std::string> names;
    names.reserve(ctx._input.size());

    // Default input names based on position
    static const char* default_names[] = {"x", "y", "w", "b", "c"};

    for (size_t i = 0; i < ctx._input.size(); ++i) {
        const auto& tensor = ctx.Input_at(i);
        if (tensor.defined()) {
            uintptr_t ptr = reinterpret_cast<uintptr_t>(tensor.data_ptr());
            std::string name = Lookup_tensor_name(ptr);
            if (!name.empty()) {
                names.push_back(name);
                continue;
            }
        }
        // Fallback to default name
        names.push_back((i < 5) ? default_names[i] : "input_" + std::to_string(i));
    }
    return names;
}

std::map<std::string, std::string> TORCH_OP_HANDLER::Build_metadata() {
    // In direct mode, metadata comes purely from the op context.
    // onnx_name and is_output are not available (no per-node Python iteration).
    return {};
}

at::Tensor TORCH_OP_HANDLER::Register_result_tensor(
    const at::Tensor& input, const std::string& result_name) {
    at::Tensor result = input.clone();
    if (!result_name.empty()) {
        Register_tensor_name(
            reinterpret_cast<uintptr_t>(result.data_ptr()), result_name);
    }
    return result;
}

// ============================================================================
// Execute: Main entry point
// ============================================================================

at::Tensor TORCH_OP_HANDLER::Execute(const std::string& op_name,
                                      OP_CONTEXT& ctx) {
    IR_BUILDER& builder = IR_BUILDER::Instance();
    if (!builder.Is_building()) return ctx.Input_at(0);

    // Step 2: Resolve input names from symbol table
    auto input_names = Resolve_input_names(ctx);

    // Step 3: Apply schema defaults + compute shape from schema
    std::vector<int64_t> output_shape;
    const OP_SCHEMA* schema = OP_SCHEMA_REGISTRY::Instance().Get(op_name);
    if (schema) {
        schema->Apply_defaults(ctx._attr);
        output_shape = schema->Compute_shape(ctx);
    } else {
        // Fallback: passthrough (use first input shape)
        output_shape = Get_tensor_shape(ctx.Input_at(0));
    }

    // Step 4: Build metadata
    auto metadata = Build_metadata();

    // Step 5: Add operation
    std::string result_name = builder.Add_operation_cpp(
        op_name, input_names, ctx._attr, metadata, output_shape);

    // Step 6: Clone + register result tensor
    return Register_result_tensor(ctx.Input_at(0), result_name);
}

}  // namespace frontend
}  // namespace ace