//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_TORCH_OP_HANDLER_H
#define AIR_TORCH_OP_HANDLER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <any>
#include <functional>
#include <cstdint>
#include <ATen/ATen.h>

#include "ace/frontend/ops/op_context.h"

namespace ace {
namespace frontend {

//=============================================================================
// TORCH_OP_HANDLER - Utility class for PyTorch custom op AIR IR generation
//
// All methods are static. This class holds no instance state — it is a
// collection of helper functions that delegate to IR_BUILDER (the real
// singleton) for AIR IR generation.
//
// Each tensor_xxx_impl() in torch_ops.cxx builds an OP_CONTEXT and calls
// Execute(), which handles the common 7-step pattern:
//   1. Is_building() guard
//   2. Resolve input tensor names from symbol table
//   3. Apply schema defaults to attrs
//   4. Build metadata
//   5. Compute output shape via OP_SCHEMA::Compute_Shape()
//   6. Call IR_BUILDER::Add_operation_cpp()
//   7. Clone + register result tensor
//=============================================================================

class TORCH_OP_HANDLER {
public:
    // Not instantiable — all methods are static
    TORCH_OP_HANDLER() = delete;

    // =====================================================================
    // Tensor name registry (data_ptr → name, for Path 1 / direct mode)
    // =====================================================================

    //! @brief Register a data_ptr to name mapping
    static void Register_tensor_name(uintptr_t data_ptr, const std::string& name);

    //! @brief Look up name by data pointer
    static std::string Lookup_tensor_name(uintptr_t data_ptr);

    //! @brief Clear all data_ptr→name mappings
    static void Clear_tensor_names();

    // Main entry: handles the full 7-step pattern for any op
    static at::Tensor Execute(const std::string& op_name, OP_CONTEXT& ctx);

    // =====================================================================
    // Shape functions (public for OP_SCHEMA registration in op_registry.cxx)
    // =====================================================================
    static std::vector<int64_t> Passthrough_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Matmul_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Concat_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Pool_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Global_pool_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Flatten_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Gemm_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Conv_shape(const OP_CONTEXT& ctx);
    static std::vector<int64_t> Reshape_shape(const OP_CONTEXT& ctx);

private:
    // Internal data_ptr→name map (static, lives in .cxx)
    static std::unordered_map<uintptr_t, std::string>& Get_tensor_name_map();

    // Resolve input tensor names from symbol table
    static std::vector<std::string> Resolve_input_names(const OP_CONTEXT& ctx);

    // Build metadata map
    static std::map<std::string, std::string> Build_metadata();

    // Clone input tensor and register result AIR name
    static at::Tensor Register_result_tensor(const at::Tensor& input,
                                            const std::string& result_name);

    // Get tensor shape as vector<int64_t>
    static std::vector<int64_t> Get_tensor_shape(const at::Tensor& t);

    // Shape computation helpers
    static int64_t Compute_conv_output_dim(int64_t input_dim, int64_t pad,
                                         int64_t dilation, int64_t kernel,
                                         int64_t stride);
    static int64_t Compute_pool_output_dim(int64_t input_dim, int64_t pad,
                                         int64_t kernel, int64_t stride);
};

}  // namespace frontend
}  // namespace ace

#endif  // AIR_TORCH_OP_HANDLER_H