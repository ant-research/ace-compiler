//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_TENSOR_LEVEL_HANDLER_H
#define AIR_TENSOR_LEVEL_HANDLER_H

#include "frontend/layers/level_handler.h"

#include <string>
#include <vector>
#include <map>

#include <torch/extension.h>

namespace ace {
namespace frontend {

//! @brief Tensor Level Handler - Handles plain tensor operations
//! This handler processes tensor operations (add, conv, relu, etc.)
//! and generates corresponding AIR IR nodes using nn::core opcodes
class TENSOR_LEVEL_HANDLER : public LEVEL_HANDLER {
public:
    TENSOR_LEVEL_HANDLER();
    ~TENSOR_LEVEL_HANDLER() override = default;

    // ========================================================================
    // Level Information
    // ========================================================================

    //! @brief Get the level name
    std::string Get_level_name() const override;

    //! @brief Get the level type
    LEVEL_TYPE Get_level_type() const override;

    // ========================================================================
    // Operation Support
    // ========================================================================

    //! @brief Check if operation is supported
    bool Has_op(const std::string& op_name) const override;

    //! @brief Get list of supported operations
    std::vector<std::string> Get_supported_op() const override;

    // ========================================================================
    // Operation Processing
    // ========================================================================

    //! @brief Process tensor operation and generate AIR IR node
    air::base::NODE_PTR Process_op(
        const std::string& op_name,
        const std::vector<std::string>& input_names,
        const std::map<std::string, std::any>& attrs,
        const std::map<std::string, std::string>& metadata,
        air::base::CONTAINER* cntr,
        const air::base::SPOS& spos,
        SYMBOL_TABLE* sym_tab,
        TYPE_FACTORY* type_factory,
        const std::vector<int64_t>& output_shape = {}) override;

    //! @brief Get opcode for operation name
    air::base::OPCODE Get_opcode(const std::string& op_name) const override;

private:
    // Non-copyable
    TENSOR_LEVEL_HANDLER(const TENSOR_LEVEL_HANDLER&) = delete;
    TENSOR_LEVEL_HANDLER& operator=(const TENSOR_LEVEL_HANDLER&) = delete;

    // Member variables
    std::map<std::string, air::base::OPCODE> _op_map;

    // Member functions
    void Init_op_map();
    void Set_node_attrs(air::base::NODE_PTR node,
                      const std::map<std::string, std::any>& attrs,
                      const std::map<std::string, std::string>& metadata);
    void Set_node_metadata(air::base::NODE_PTR node,
                         const std::map<std::string, std::string>& metadata);
};

}  // namespace frontend
}  // namespace ace

#endif  // AIR_TENSOR_LEVEL_HANDLER_H