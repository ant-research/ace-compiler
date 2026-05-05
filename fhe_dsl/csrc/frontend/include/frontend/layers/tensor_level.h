//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_LEVELS_TENSOR_LEVEL_H
#define AIR_LEVELS_TENSOR_LEVEL_H

#include "frontend/layers/level_base.h"
#include "frontend/layers/tensor_level_handler.h"

#include <string>
#include <vector>
#include <memory>

namespace ace {
namespace frontend {

//! @brief Tensor Level - Plain tensor operations
//! This level handles operations on plain tensors (float/int)
//! and generates corresponding AIR IR nodes.
//! Delegates opcode lookup to TENSOR_LEVEL_HANDLER.
class TENSOR_LEVEL : public LEVEL_BASE {
public:
    TENSOR_LEVEL();
    ~TENSOR_LEVEL() override = default;

    //! @brief Get the level name
    std::string Get_level_name() const override;

    //! @brief Get the level type
    LEVEL_TYPE Get_level_type() const override;

    //! @brief Register tensor operations to PyTorch
    void Register_py_ops(pybind11::module& m) override;

    //! @brief Build a tensor operation and generate AIR IR
    at::Tensor Build_op(const std::string& op_name,
                       const std::vector<at::Tensor>& inputs) override;

    //! @brief Check if operation is supported (delegates to TENSOR_LEVEL_HANDLER)
    bool Has_op(const std::string& op_name) const override;

    //! @brief Get list of supported operations (delegates to TENSOR_LEVEL_HANDLER)
    std::vector<std::string> Get_supported_op() const override;

    //! @brief Get opcode for operation name (delegates to TENSOR_LEVEL_HANDLER)
    air::base::OPCODE Get_opcode(const std::string& op_name) const;

private:
    // Non-copyable
    TENSOR_LEVEL(const TENSOR_LEVEL&) = delete;
    TENSOR_LEVEL& operator=(const TENSOR_LEVEL&) = delete;

    // Member variables
    std::unique_ptr<TENSOR_LEVEL_HANDLER> _handler;

    // Member functions
    at::Tensor Op_add(const at::Tensor& x, const at::Tensor& y);
    at::Tensor Op_sub(const at::Tensor& x, const at::Tensor& y);
    at::Tensor Op_mul(const at::Tensor& x, const at::Tensor& y);
    at::Tensor Op_div(const at::Tensor& x, const at::Tensor& y);
    at::Tensor Op_matmul(const at::Tensor& x, const at::Tensor& y);
    at::Tensor Op_concat(const at::Tensor& x, const at::Tensor& y);
    at::Tensor Op_relu(const at::Tensor& x);
    at::Tensor Op_softmax(const at::Tensor& x);
    at::Tensor Op_max_pool(const at::Tensor& x);
    at::Tensor Op_average_pool(const at::Tensor& x);
    at::Tensor Op_global_average_pool(const at::Tensor& x);
    at::Tensor Op_flatten(const at::Tensor& x);
    at::Tensor Op_sqrt(const at::Tensor& x);
    at::Tensor Op_silu(const at::Tensor& x);
    at::Tensor Op_conv(const at::Tensor& x, const at::Tensor& w, const at::Tensor& b);
    at::Tensor Op_gemm(const at::Tensor& a, const at::Tensor& b, const at::Tensor& c);
};

}  // namespace frontend
}  // namespace ace

#endif