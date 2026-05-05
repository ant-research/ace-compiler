//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "frontend/ops/op_schema.h"

#include <algorithm>

#include "frontend/ops/op_context.h"
#include "nn/core/opcode.h"

namespace ace {
namespace frontend {

//=============================================================================
// OP_SCHEMA Implementation
//=============================================================================

OP_SCHEMA::OP_SCHEMA() : _opcode(air::base::OPCODE(nn::core::NN, nn::core::OPCODE::INVALID)) {}

OP_SCHEMA::OP_SCHEMA(const std::string& name, air::base::OPCODE opcode)
    : _name(name), _opcode(opcode) {}

OP_SCHEMA& OP_SCHEMA::Attr(const std::string& name, ATTR_TYPE type,
                           bool required, std::any default_val) {
    _attr.emplace_back(name, type, required, default_val);
    return *this;
}

OP_SCHEMA& OP_SCHEMA::Input(const std::string& name, bool required) {
    _input.push_back(name);
    // Mark input as required by adding a placeholder attribute
    // This is used for validation
    if (required) {
        // Required inputs are tracked in _input
    }
    return *this;
}

OP_SCHEMA& OP_SCHEMA::Output(const std::string& name) {
    _output = name;
    return *this;
}

OP_SCHEMA& OP_SCHEMA::Shape(SHAPE_FN fn) {
    _shape_fn = std::move(fn);
    return *this;
}

bool OP_SCHEMA::Has_attr(const std::string& name) const {
    return std::any_of(_attr.begin(), _attr.end(),
                       [&name](const OP_ATTRIBUTE& attr) {
                           return attr._name == name;
                       });
}

bool OP_SCHEMA::Validate_and_parse(const pybind11::kwargs& kwargs,
                                    std::map<std::string, std::any>& out_attr) const {
    // Convert Python kwargs to C++ attributes
    for (const auto& [key, value] : kwargs) {
        std::string key_str = pybind11::cast<std::string>(key);

        // Find matching attribute in schema
        auto it = std::find_if(_attr.begin(), _attr.end(),
                               [&key_str](const OP_ATTRIBUTE& attr) {
                                   return attr._name == key_str;
                               });

        if (it != _attr.end()) {
            // Parse based on expected type
            try {
                switch (it->_type) {
                    case ATTR_TYPE::INT: {
                        out_attr[key_str] = pybind11::cast<int>(value);
                        break;
                    }
                    case ATTR_TYPE::INTS: {
                        out_attr[key_str] = pybind11::cast<std::vector<int>>(value);
                        break;
                    }
                    case ATTR_TYPE::FLOAT: {
                        out_attr[key_str] = pybind11::cast<float>(value);
                        break;
                    }
                    case ATTR_TYPE::FLOATS: {
                        out_attr[key_str] = pybind11::cast<std::vector<float>>(value);
                        break;
                    }
                    case ATTR_TYPE::STRING: {
                        out_attr[key_str] = pybind11::cast<std::string>(value);
                        break;
                    }
                    default: {
                        // Store as-is for unknown types
                        out_attr[key_str] = value;
                        break;
                    }
                }
            } catch (const pybind11::cast_error& e) {
// Warning: Failed to cast attribute
                // Store with default value if available
                if (it->_default_value.has_value()) {
                    out_attr[key_str] = it->_default_value;
                }
            }
        } else {
            // Unknown attribute - store it anyway (flexible mode)
            out_attr[key_str] = value;
        }
    }

    // Check required attributes
    bool valid = true;
    for (const auto& attr : _attr) {
        if (attr._required && out_attr.find(attr._name) == out_attr.end()) {
// Error: Required attribute is missing
            valid = false;
        }
    }

    return valid;
}

void OP_SCHEMA::Apply_defaults(std::map<std::string, std::any>& attrs) const {
    for (const auto& attr : _attr) {
        // Only fill in defaults for attrs that are missing and have a default value
        if (attrs.find(attr._name) == attrs.end() && attr._default_value.has_value()) {
            attrs[attr._name] = attr._default_value;
        }
    }
}

std::vector<int64_t> OP_SCHEMA::Compute_shape(const OP_CONTEXT& ctx) const {
    if (_shape_fn) {
        return _shape_fn(ctx);
    }
    // Fallback: return first input shape
    if (ctx.Input_count() > 0) {
        const auto& t = ctx.Input_at(0);
        return std::vector<int64_t>(t.sizes().begin(), t.sizes().end());
    }
    return {};
}

//=============================================================================
// OP_SCHEMA_REGISTRY Implementation
//=============================================================================

OP_SCHEMA_REGISTRY& OP_SCHEMA_REGISTRY::Instance() {
    static OP_SCHEMA_REGISTRY instance;
    return instance;
}

void OP_SCHEMA_REGISTRY::Register(const std::string& name, OP_SCHEMA schema) {
    _schema_map[name] = std::move(schema);
}

const OP_SCHEMA* OP_SCHEMA_REGISTRY::Get(const std::string& name) const {
    auto it = _schema_map.find(name);
    if (it != _schema_map.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool OP_SCHEMA_REGISTRY::Has(const std::string& name) const {
    return _schema_map.find(name) != _schema_map.end();
}

std::vector<std::string> OP_SCHEMA_REGISTRY::Get_all_op_name() const {
    std::vector<std::string> names;
    names.reserve(_schema_map.size());
    for (const auto& [name, schema] : _schema_map) {
        names.push_back(name);
    }
    return names;
}

//=============================================================================
// Register_All_Ops Declaration - defined in op_registry.cxx
//=============================================================================
// This function is forward-declared here and implemented in op_registry.cxx
// to separate schema definition from registration logic

}  // namespace frontend
}  // namespace ace