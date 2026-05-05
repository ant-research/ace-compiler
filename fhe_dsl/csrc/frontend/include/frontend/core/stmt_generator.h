//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef STMT_GENERATOR_H
#define STMT_GENERATOR_H

#include <string>
#include <cstdint>

#include "air/base/st.h"
#include "air/base/opcode.h"

// NN core headers for pragma
#include "nn/core/opcode.h"
#include "nn/core/pragma.h"

namespace ace {
namespace frontend {

// Forward declarations
class AIR_CONTEXT;
class SYMBOL_TABLE;

//! @brief STMT_GENERATOR - Generates AIR statements for operations
//!
//! Responsibilities:
//! - Generate pragma statements (OP_START, OP_END)
//! - Generate comment statements
//! - Generate store statements (st, stp)
//! - Manage pragma state (op name, enable flag, comment ID)
//!
//! Depends on AIR_CONTEXT for SPOS.
//! Receives FUNC_SCOPE and output_var from FUNC_BUILDER per operation.
class STMT_GENERATOR {
public:
    inline explicit STMT_GENERATOR(AIR_CONTEXT* ctx) : _ctx(ctx) {}
    ~STMT_GENERATOR() = default;

    // ========================================================================
    // Pragma State Management
    // ========================================================================

    //! @brief Set operator name for pragma generation
    void Set_op_name(const std::string& op_name) { _current_op_name = op_name; }

    //! @brief Get current operator name
    const std::string& Get_op_name() const { return _current_op_name; }

    //! @brief Enable pragma generation for next operation
    void Enable_pragma(bool enable = true) { _enable_pragma = enable; }

    //! @brief Check if pragma generation is enabled
    bool Is_pragma_enabled() const { return _enable_pragma; }

    //! @brief Get next comment ID and increment counter
    uint32_t Get_next_comment_id() { return _comment_id_counter++; }

    //! @brief Advance comment ID counter by specified amount
    void Advance_comment_id(uint32_t count) { _comment_id_counter += count; }

    // ========================================================================
    // Statement Generation
    // ========================================================================

    //! @brief Generate pragma and comment statements for an operation
    //! @param op_name Operation name for comment
    //! @param opcode AIR opcode for pragma
    //! @param func_scope Current function scope
    //! @return Comment ID for OP_START/OP_END matching
    uint32_t Generate_pragma_comment(const std::string& op_name,
                                     air::base::OPCODE opcode,
                                     air::base::FUNC_SCOPE* func_scope);

    //! @brief Generate store statement for operation result
    //! @param op_node Operation node to store
    //! @param result_name Name for result variable
    //! @param is_output Whether this is an output operation
    //! @param sym_tab Symbol table for PREG storage
    //! @param func_scope Current function scope
    //! @param output_var Output variable (for is_output=true)
    //! @param rtype Result type for PREG creation
    void Generate_store_statement(air::base::NODE_PTR op_node,
                                  const std::string& result_name,
                                  bool is_output,
                                  SYMBOL_TABLE* sym_tab,
                                  air::base::FUNC_SCOPE* func_scope,
                                  air::base::ADDR_DATUM_PTR output_var,
                                  air::base::TYPE_PTR rtype);

    //! @brief Complete operation generation (pragma_end after store)
    //! @param opcode AIR opcode for pragma
    //! @param comment_id Comment ID from Generate_pragma_comment
    //! @param func_scope Current function scope
    void Complete_operation(air::base::OPCODE opcode,
                            uint32_t comment_id,
                            air::base::FUNC_SCOPE* func_scope);

private:
    // Non-copyable
    STMT_GENERATOR(const STMT_GENERATOR&) = delete;
    STMT_GENERATOR& operator=(const STMT_GENERATOR&) = delete;

    // Member variables
    AIR_CONTEXT*    _ctx;
    std::string     _current_op_name;
    bool            _enable_pragma = false;
    uint32_t        _comment_id_counter = 0x21;
};

}  // namespace frontend
}  // namespace ace

#endif  // STMT_GENERATOR_H