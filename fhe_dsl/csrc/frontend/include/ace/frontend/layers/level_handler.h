//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_LEVEL_HANDLER_H
#define AIR_LEVEL_HANDLER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <any>

#include "air/base/opcode.h"
#include "air/base/st.h"
#include "ace/frontend/layers/level_types.h"

namespace ace {
namespace frontend {

// Forward declarations
class SYMBOL_TABLE;
class TYPE_FACTORY;

//! @brief Operation build information
struct OP_BUILD_INFO {
    std::string                        _op_name;      // Operation name
    air::base::OPCODE                  _opcode;       // AIR opcode
    std::vector<std::string>           _input_name;   // Input variable names
    std::map<std::string, std::any>    _attr;         // Attributes
    std::map<std::string, std::string> _metadata;     // Metadata (e.g., onnx_name)
};

//! @brief Abstract base class for level handlers
//! Each level (Tensor, Vector, CKKS, SIHE, Poly) implements this interface
class LEVEL_HANDLER {
public:
    virtual ~LEVEL_HANDLER() = default;

    // ========================================================================
    // Level Information
    // ========================================================================

    //! @brief Get the level name
    virtual std::string Get_level_name() const = 0;

    //! @brief Get the level type
    virtual LEVEL_TYPE Get_level_type() const = 0;

    // ========================================================================
    // Operation Support
    // ========================================================================

    //! @brief Check if an operation is supported by this level
    virtual bool Has_op(const std::string& op_name) const = 0;

    //! @brief Get list of supported operations
    virtual std::vector<std::string> Get_supported_op() const = 0;

    // ========================================================================
    // Operation Processing
    // ========================================================================

    //! @brief Process an operation and generate AIR IR node
    //! @param op_name Operation name (e.g., "add", "conv", "relu")
    //! @param input_names Names of input variables
    //! @param attrs Operation attributes
    //! @param metadata Operation metadata (e.g., onnx_name for pragma)
    //! @param cntr AIR container for node creation
    //! @param spos Source position for IR tracking
    //! @param sym_tab Symbol table for resolving input names
    //! @param type_factory Type factory for creating tensor types
    //! @param output_shape Optional output shape for type creation
    //! @return Created AIR IR node, or Null_ptr on failure
    virtual air::base::NODE_PTR Process_op(
        const std::string& op_name,
        const std::vector<std::string>& input_names,
        const std::map<std::string, std::any>& attrs,
        const std::map<std::string, std::string>& metadata,
        air::base::CONTAINER* cntr,
        const air::base::SPOS& spos,
        SYMBOL_TABLE* sym_tab,
        TYPE_FACTORY* type_factory,
        const std::vector<int64_t>& output_shape = {}) = 0;

    //! @brief Get opcode for operation name
    //! @param op_name Operation name
    //! @return Opcode, or empty opcode if not found
    virtual air::base::OPCODE Get_opcode(const std::string& op_name) const = 0;
};

//! @brief Type alias for level handler pointer
using LEVEL_HANDLER_PTR = std::unique_ptr<LEVEL_HANDLER>;

}  // namespace frontend
}  // namespace ace

#endif  // AIR_LEVEL_HANDLER_H