//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_SYMBOL_TABLE_H
#define AIR_SYMBOL_TABLE_H

#include <string>
#include <map>
#include <cstdint>

#include "air/base/st.h"
#include "air/base/container.h"

namespace ace {
namespace frontend {

//! @brief Symbol Table - Manages variables, pregisters, and constants
//! This class provides unified management for AIR IR symbolic entities
//! (variables, pseudo registers, constants).
class SYMBOL_TABLE {
public:
    SYMBOL_TABLE() = default;
    ~SYMBOL_TABLE() = default;

    // ========================================================================
    // Variable Management (ADDR_DATUM)
    // ========================================================================

    //! @brief Add a variable (input/output parameter)
    void Add_var(const std::string& name, air::base::ADDR_DATUM_PTR var) {
        _var[name] = var;
    }

    //! @brief Retrieve a variable by name
    air::base::ADDR_DATUM_PTR Get_var(const std::string& name) const {
        auto it = _var.find(name);
        return (it != _var.end()) ? it->second : air::base::ADDR_DATUM_PTR();
    }

    //! @brief Check if variable exists
    bool Has_var(const std::string& name) const {
        return _var.find(name) != _var.end();
    }

    // ========================================================================
    // Pregister Management (PREG - pseudo register for intermediate results)
    // ========================================================================

    //! @brief Add a preister (intermediate result)
    void Add_preg(const std::string& name, air::base::PREG_PTR preg) {
        _preg[name] = preg;
    }

    //! @brief Retrieve a preister by name
    air::base::PREG_PTR Get_preg(const std::string& name) const {
        auto it = _preg.find(name);
        return (it != _preg.end()) ? it->second : air::base::PREG_PTR();
    }

    //! @brief Check if preister exists
    bool Has_preg(const std::string& name) const {
        return _preg.find(name) != _preg.end();
    }

    // ========================================================================
    // Constant Management
    // ========================================================================

    //! @brief Add a constant
    void Add_const(const std::string& name, air::base::CONSTANT_PTR cst) {
        _const[name] = cst;
    }

    //! @brief Retrieve a constant by name
    air::base::CONSTANT_PTR Get_const(const std::string& name) const {
        auto it = _const.find(name);
        return (it != _const.end()) ? it->second : air::base::CONSTANT_PTR();
    }

    //! @brief Check if constant exists
    bool Has_const(const std::string& name) const {
        return _const.find(name) != _const.end();
    }

    // ========================================================================
    // Input Resolution
    // ========================================================================

    //! @brief Resolve input name to AIR node (ld/ldp/ldc)
    //! @param name Input variable name
    //! @return Node pointer for loading the input, or Null_ptr if not found
    air::base::NODE_PTR Resolve_input(const std::string& name,
                                      air::base::CONTAINER* cntr,
                                      const air::base::SPOS& spos) const;

    // ========================================================================
    // State Management
    // ========================================================================

    //! @brief Clear all symbol mappings
    void Clear() {
        _var.clear();
        _preg.clear();
        _const.clear();
    }

    //! @brief Get statistics
    size_t Get_var_cnt() const { return _var.size(); }
    size_t Get_preg_cnt() const { return _preg.size(); }
    size_t Get_const_cnt() const { return _const.size(); }

private:
    // Non-copyable
    SYMBOL_TABLE(const SYMBOL_TABLE&) = delete;
    SYMBOL_TABLE& operator=(const SYMBOL_TABLE&) = delete;

    // Member variables
    std::map<std::string, air::base::ADDR_DATUM_PTR>    _var;
    std::map<std::string, air::base::PREG_PTR>          _preg;
    std::map<std::string, air::base::CONSTANT_PTR>      _const;
};

}  // namespace frontend
}  // namespace ace

#endif  // AIR_SYMBOL_TABLE_H