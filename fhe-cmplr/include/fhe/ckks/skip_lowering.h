//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_SKIP_LOWERING_H
#define FHE_CKKS_SKIP_LOWERING_H

/**
 * @file skip_lowering.h
 * @brief Skip Lowering Registry for Selective Lowering in CKKS domain
 * 
 * This module provides a mechanism to skip C++ lowering for specific operations,
 * allowing Python-defined lowerings to be applied instead.
 * 
 * IMPORTANT: The singleton is defined in skip_lowering.cxx (part of libFHEckks).
 * All code linking against libFHEckks shares the SAME singleton instance.
 * 
 * Usage:
 *   1. Python registers ops to skip via pybind11 bindings
 *   2. C++ handlers check Should_skip_lowering() before lowering
 *   3. Skipped ops are preserved for Python post-lowering pass
 * 
 * Example:
 *   // From Python (via pybind11):
 *   fhe_ckks.set_skip_ops(["fhe::ckks::bootstrap"])
 *   
 *   // In C++ handler:
 *   if (fhe::ckks::Should_skip_lowering("fhe::ckks", "bootstrap")) {
 *       return Clone_with_visited_children<RETV>(visitor, node);
 *   }
 *   // ... normal C++ lowering
 */

#include <set>
#include <string>
#include <vector>

namespace fhe {
namespace ckks {

/**
 * @class SKIP_LOWERING_REGISTRY
 * @brief Singleton registry of ops that should skip C++ lowering
 * 
 * Operations registered here have Python-defined lowerings that will
 * be applied after C++ passes complete. The C++ passes check this
 * registry and preserve (clone) the original node instead of lowering.
 * 
 * NOTE: The singleton Instance() is defined in skip_lowering.cxx
 *       to ensure all users share the SAME instance.
 */
class SKIP_LOWERING_REGISTRY {
public:
    /**
     * @brief Get the singleton instance
     * @note Defined in skip_lowering.cxx - shared across all linked code
     */
    static SKIP_LOWERING_REGISTRY& Instance();
    
    /**
     * @brief Set the list of ops to skip
     * @param ops Vector of op names in format "domain::op_name"
     */
    void Set_skip_ops(const std::vector<std::string>& ops) {
        _skip_ops.clear();
        _skip_ops.insert(ops.begin(), ops.end());
    }
    
    /**
     * @brief Add a single op to the skip list
     */
    void Add_skip_op(const std::string& op) {
        _skip_ops.insert(op);
    }
    
    /**
     * @brief Clear all skip ops
     */
    void Clear_skip_ops() {
        _skip_ops.clear();
    }
    
    /**
     * @brief Check if an op should be skipped
     */
    bool Should_skip(const std::string& domain, const std::string& op_name) const {
        std::string full_name = domain + "::" + op_name;
        return _skip_ops.count(full_name) > 0;
    }
    
    /**
     * @brief Check if an op should be skipped (by full name)
     */
    bool Should_skip(const std::string& full_op_name) const {
        return _skip_ops.count(full_op_name) > 0;
    }
    
    /**
     * @brief Get all ops that should be skipped
     */
    const std::set<std::string>& Get_skip_ops() const {
        return _skip_ops;
    }
    
    /**
     * @brief Get the number of ops to skip
     */
    size_t Skip_count() const {
        return _skip_ops.size();
    }
    
    /**
     * @brief Check if any ops are registered to skip
     */
    bool Has_skip_ops() const {
        return !_skip_ops.empty();
    }

private:
    SKIP_LOWERING_REGISTRY() = default;
    SKIP_LOWERING_REGISTRY(const SKIP_LOWERING_REGISTRY&) = delete;
    SKIP_LOWERING_REGISTRY& operator=(const SKIP_LOWERING_REGISTRY&) = delete;
    
    std::set<std::string> _skip_ops;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Convenience Functions - Defined in skip_lowering.cxx
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief Check if an op should skip C++ lowering
 * @note Defined in skip_lowering.cxx - links to shared singleton
 */
bool Should_skip_lowering(const std::string& domain, const std::string& op_name);

/**
 * @brief Check if an op should skip C++ lowering (by full name)
 */
bool Should_skip_lowering(const std::string& full_op_name);

/**
 * @brief Set ops to skip
 */
void Set_skip_lowering_ops(const std::vector<std::string>& ops);

/**
 * @brief Add an op to skip
 */
void Add_skip_lowering_op(const std::string& op);

/**
 * @brief Clear skip ops
 */
void Clear_skip_lowering_ops();

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_SKIP_LOWERING_H

