//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_FUNC_BUILDER_H
#define AIR_FUNC_BUILDER_H

#include <string>
#include <vector>
#include <memory>

#include "air/base/st.h"

namespace ace {
namespace frontend {

// Forward declarations
class SYMBOL_TABLE;
class AIR_CONTEXT;
class TYPE_FACTORY;

//! @brief Variable entry (used for inputs and outputs)
struct VAR_ENTRY {
    std::string                 _name;
    std::vector<int64_t>        _shape;
    air::base::ADDR_DATUM_PTR   _var = air::base::Null_ptr;  // Variable reference
};

//! @brief Constant tensor entry
struct CONST_ENTRY {
    std::string                 _name;
    std::vector<int64_t>        _shape;
    const void*                 _data = nullptr;
    air::base::PRIMITIVE_TYPE   _elem_type = air::base::PRIMITIVE_TYPE::FLOAT_32;
    air::base::CONSTANT_PTR     _cst = air::base::Null_ptr;  // Constant reference
};

//! @brief FUNC_BUILDER - Manages AIR function lifecycle
//!
//! Responsible for:
//! - Function creation and finalization
//! - Input parameter management
//! - Constant parameter management
//! - Output variable management
//!
//! This class owns _func_scope and _output_var, and provides
//! access to them for statement generation.
class FUNC_BUILDER {
public:
    FUNC_BUILDER(AIR_CONTEXT* ctx);
    ~FUNC_BUILDER() = default;

    // ========================================================================
    // Function Lifecycle
    // ========================================================================

    //! @brief Begin building a new function
    //! @param func_name Function name
    void Begin_func(const std::string& func_name);

    //! @brief Add input parameter to current function
    //! @param entry Variable entry (name and shape)
    void New_param(const VAR_ENTRY& entry) { _input.push_back(entry); }

    //! @brief Add constant parameter
    //! @param entry Constant entry (name, shape, data, elem_type)
    //! @return CONSTANT_PTR for caller to store
    air::base::CONSTANT_PTR New_const(const CONST_ENTRY& entry);

    //! @brief End function definition
    //! @param output Output variable entry (shape used for type creation)
    //! @param sym_tab Symbol table for storing formal params
    void End_func(const VAR_ENTRY& output, SYMBOL_TABLE* sym_tab);

    //! @brief Finalize function with return statement
    void Complete_func();

    // ========================================================================
    // State Queries
    // ========================================================================

    //! @brief Check if a function is currently being built
    bool Is_building() const { return _func_scope != nullptr; }

    //! @brief Get inputs (variable entry list)
    const std::vector<VAR_ENTRY>& Get_input() const { return _input; }

    //! @brief Get output (variable entry)
    const VAR_ENTRY& Get_output() const { return _output; }

    //! @brief Get function scope
    air::base::FUNC_SCOPE* Get_func_scope() const { return _func_scope; }

    //! @brief Get container from function scope
    air::base::CONTAINER* Get_container() const {
        return _func_scope ? &_func_scope->Container() : nullptr;
    }

    //! @brief Get type factory
    TYPE_FACTORY* Get_type_factory() const { return _type_factory.get(); }

private:
    // Non-copyable
    FUNC_BUILDER(const FUNC_BUILDER&) = delete;
    FUNC_BUILDER& operator=(const FUNC_BUILDER&) = delete;

    // Member variables
    AIR_CONTEXT*                     _ctx;
    std::unique_ptr<TYPE_FACTORY>    _type_factory;
    air::base::STR_PTR               _func_name = air::base::Null_ptr;
    air::base::FUNC_PTR              _func;
    air::base::SIGNATURE_TYPE_PTR    _sig;
    air::base::FUNC_SCOPE*           _func_scope = nullptr;
    std::vector<VAR_ENTRY>           _input;
    VAR_ENTRY                        _output;
};

}  // namespace frontend
}  // namespace ace

#endif  // AIR_FUNC_BUILDER_H