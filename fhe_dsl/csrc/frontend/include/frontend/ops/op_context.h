//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OP_CONTEXT_H
#define AIR_OP_CONTEXT_H

#include <vector>
#include <map>
#include <string>
#include <any>
#include <ATen/ATen.h>

namespace ace {
namespace frontend {

//=============================================================================
// Operator Context - Unified passing of inputs and attributes
//=============================================================================
struct OP_CONTEXT {
    // Input tensors
    std::vector<at::Tensor> _input;

    // Input names (parallel to _input, for forward name passing)
    std::vector<std::string> _input_name;

    // Attributes (parsed from Python kwargs)
    std::map<std::string, std::any> _attr;

    // Accessors - Single value attributes
    template<typename T>
    T Get_attr(const std::string& key, T default_val) const {
        auto it = _attr.find(key);
        if (it != _attr.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return default_val;
            }
        }
        return default_val;
    }

    // Accessors - Array attributes
    template<typename T>
    std::vector<T> Get_attr_vec(const std::string& key,
                                std::vector<T> default_val = {}) const {
        auto it = _attr.find(key);
        if (it != _attr.end()) {
            try {
                return std::any_cast<std::vector<T>>(it->second);
            } catch (const std::bad_any_cast&) {
                return default_val;
            }
        }
        return default_val;
    }

    // Helper methods
    bool Has_attr(const std::string& key) const {
        return _attr.find(key) != _attr.end();
    }

    size_t Input_count() const { return _input.size(); }

    const at::Tensor& Input_at(size_t index) const {
        if (index >= _input.size()) {
            static at::Tensor empty;
            return empty;
        }
        return _input[index];
    }

    };

// Operator implementation function type
using OP_IMPL_FN = std::function<at::Tensor(const OP_CONTEXT& ctx)>;

}  // namespace frontend
}  // namespace ace

#endif  // AIR_OP_CONTEXT_H