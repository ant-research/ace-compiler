//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_LEVELS_LEVEL_BASE_H
#define AIR_LEVELS_LEVEL_BASE_H

#include <string>
#include <vector>
#include <memory>

// Include nn/core/opcode.h BEFORE torch headers to avoid OPCODE name conflict
#include "nn/core/opcode.h"

#include <torch/extension.h>

#include "frontend/layers/level_types.h"

namespace ace {
namespace frontend {

//! @brief Abstract base class for all level implementations
//! Each level (Tensor, Vec, Ckks, Poly) implements this interface
class LEVEL_BASE {
public:
    virtual ~LEVEL_BASE() = default;

    //! @brief Get the level name
    virtual std::string Get_level_name() const = 0;

    //! @brief Get the level type
    virtual LEVEL_TYPE Get_level_type() const = 0;

    //! @brief Register this level's operations to PyTorch
    //! @param m PyTorch module to register ops into
    virtual void Register_py_ops(pybind11::module& m) = 0;

    //! @brief Build an operation and generate AIR IR
    //! @param op_name Operation name (e.g., "add", "relu")
    //! @param inputs Input tensors
    //! @return Output tensor (for IR generation, actual computation is skipped)
    virtual at::Tensor Build_op(const std::string& op_name,
                               const std::vector<at::Tensor>& inputs) = 0;

    //! @brief Check if an operation is supported by this level
    virtual bool Has_op(const std::string& op_name) const = 0;

    //! @brief Get list of supported operations
    virtual std::vector<std::string> Get_supported_op() const = 0;
};

//! @brief Type alias for level pointer
using LEVEL_PTR = std::unique_ptr<LEVEL_BASE>;

}  // namespace frontend
}  // namespace ace

#endif