//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_IR_BUILDER_H
#define AIR_IR_BUILDER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <any>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "frontend/layers/level_base.h"
#include "frontend/core/air_context.h"
#include "frontend/core/type_factory.h"
#include "frontend/core/stmt_generator.h"
#include "frontend/core/func_builder.h"
#include "frontend/core/symbol_table.h"

namespace ace {
namespace frontend {

//! @brief IR_BUILDER - Main entry point for AIR IR generation
//! Manages different level implementations and provides
//! Python API for building AIR IR.
//!
//! This class is a singleton that coordinates:
//! - AIR_CONTEXT: Global AIR infrastructure
//! - FUNC_BUILDER: Function lifecycle management
//! - STMT_GENERATOR: Statement generation
//! - TYPE_FACTORY: Type creation
//! - SYMBOL_TABLE: Symbol and tensor name management
class IR_BUILDER {
public:
    IR_BUILDER();
    ~IR_BUILDER();

    //! @brief Get singleton instance
    static IR_BUILDER& Instance();

    // ========================================================================
    // Level Management
    // ========================================================================

    //! @brief Register a new level handler
    void Register_level(LEVEL_TYPE level, LEVEL_PTR handler);

    //! @brief Set the current level
    void Set_level(LEVEL_TYPE level);

    //! @brief Set the current level by string
    void Set_level(const std::string& level_name);

    //! @brief Get the current level type
    LEVEL_TYPE Get_current_level() const;

    //! @brief Get the current level name
    std::string Get_current_level_name() const;

    //! @brief Check if a level is registered
    bool Has_level(LEVEL_TYPE level) const;

    // ========================================================================
    // Function Level Operations
    // ========================================================================

    //! @brief Begin building a new function
    //! This also clears the tensor name registry
    void Begin_func(const std::string& func_name);

    //! @brief Add input parameter
    void New_param(const std::string& name, const std::vector<int64_t>& shape);

    //! @brief Add constant parameter
    void New_const(const std::string& name, const std::vector<int64_t>& shape,
                     const void* data, size_t byte_len);

    //! @brief Add int64 constant parameter
    void New_const_int64(const std::string& name, const std::vector<int64_t>& shape,
                          const void* data, size_t byte_len);

    //! @brief End function definition
    void End_func(const std::vector<int64_t>& output_shape);

    //! @brief Finalize function
    void Complete_func();

    // ========================================================================
    // Operation Level Operations
    // ========================================================================

    //! @brief Add an operation using current level (Python binding version)
    std::string Add_operation(const std::string& op_name,
                             const std::vector<std::string>& input_names,
                             const std::map<std::string, py::object>& attrs = {},
                             const std::map<std::string, std::string>& metadata = {},
                             const std::vector<int64_t>& output_shape = {});

    //! @brief Add an operation using current level (C++ version with std::any attrs)
    std::string Add_operation_cpp(const std::string& op_name,
                                const std::vector<std::string>& input_names,
                                const std::map<std::string, std::any>& attrs = {},
                                const std::map<std::string, std::string>& metadata = {},
                                const std::vector<int64_t>& output_shape = {});

    //! @brief Add an operation with direct opcode (for torch_ops compatibility)
    //! Returns result name
    std::string Add_operation_with_opcode(const std::string& op_name,
                                       const std::vector<std::string>& input_names,
                                       air::base::OPCODE opcode);

    // ========================================================================
    // Output Operations
    // ========================================================================

    //! @brief Print generated IR
    void Print_ir();

    //! @brief Write IR to file
    void Write_ir(const std::string& filename, const std::string& phase = "ONNX2AIR");

    // ========================================================================
    // Pragma State Management (for torch_ops compatibility)
    // ========================================================================

    //! @brief Set operator name for pragma generation
    void Set_op_name(const std::string& op_name);

    //! @brief Enable pragma generation for next operation
    void Enable_pragma(bool enable = true);

    //! @brief Check if building is active
    bool Is_building() const;

    // ========================================================================
    // Accessors
    // ========================================================================

    //! @brief Get AIR context
    AIR_CONTEXT* Get_context() { return _ctx.get(); }

    //! @brief Get statement generator
    STMT_GENERATOR* Get_stmt_gen() { return _stmt_gen.get(); }

    //! @brief Get type factory
    TYPE_FACTORY* Get_type_factory() { return _type_factory.get(); }

    //! @brief Get function builder
    FUNC_BUILDER* Get_func_builder() { return _func_builder.get(); }

    //! @brief Get symbol table instance
    SYMBOL_TABLE& Get_symbol_table() { return _symbol_table; }

    //! @brief Get current function scope
    air::base::FUNC_SCOPE* Get_func_scope();

    //! @brief Get global scope
    air::base::GLOB_SCOPE* Get_glob();

    //! @brief Get current container
    air::base::CONTAINER* Get_container();

    //! @brief Get source position
    const air::base::SPOS& Get_spos() const;

    //! @brief Get output
    const VAR_ENTRY& Get_output() const;

    // ========================================================================
    // Python Binding Helpers
    // ========================================================================

    //! @brief Build operation (called from Python)
    at::Tensor Build_op(const std::string& op_name,
                       const std::vector<at::Tensor>& inputs);

    //! @brief Register all levels to PyTorch
    void Register_all_level(pybind11::module& m);

private:
    // Non-copyable and non-movable (singleton with unique_ptr members)
    IR_BUILDER(IR_BUILDER&&) = delete;
    IR_BUILDER(const IR_BUILDER&) = delete;
    IR_BUILDER& operator=(IR_BUILDER&&) = delete;
    IR_BUILDER& operator=(const IR_BUILDER&) = delete;

    // Member variables
    std::unique_ptr<AIR_CONTEXT>    _ctx;
    std::unique_ptr<TYPE_FACTORY>   _type_factory;
    std::unique_ptr<STMT_GENERATOR> _stmt_gen;
    std::unique_ptr<FUNC_BUILDER>   _func_builder;
    std::map<LEVEL_TYPE, LEVEL_PTR>   _level;
    LEVEL_TYPE                       _current_level = LEVEL_TYPE::TENSOR;
    SYMBOL_TABLE                    _symbol_table;
    int                             _var_id_counter = 0;

    // Member functions
    void Init_default_level();
    LEVEL_BASE* Get_current_level_handler();
};

// Backward compatibility alias
using Frontend = IR_BUILDER;

}  // namespace frontend
}  // namespace ace

#endif