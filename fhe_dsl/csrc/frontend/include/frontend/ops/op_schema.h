//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OP_SCHEMA_H
#define AIR_OP_SCHEMA_H

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <memory>
#include <pybind11/pybind11.h>

#include "air/base/opcode.h"

// Forward declaration for shape function type
namespace at {
class Tensor;
}

namespace ace {
namespace frontend {

// Forward declaration
struct OP_CONTEXT;

// Shape function type: computes output shape from op context
using SHAPE_FN = std::function<std::vector<int64_t>(const OP_CONTEXT&)>;

//=============================================================================
// Attribute Type Enumeration
//=============================================================================
enum class ATTR_TYPE {
    INT,      // Single integer
    INTS,     // Integer array
    FLOAT,    // Single float
    FLOATS,   // Float array
    STRING,   // String
    TENSOR    // Tensor
};

inline std::string Attr_Type_To_String(ATTR_TYPE type) {
    switch (type) {
        case ATTR_TYPE::INT:    return "int";
        case ATTR_TYPE::INTS:   return "ints";
        case ATTR_TYPE::FLOAT:  return "float";
        case ATTR_TYPE::FLOATS: return "floats";
        case ATTR_TYPE::STRING: return "string";
        case ATTR_TYPE::TENSOR: return "tensor";
        default: return "unknown";
    }
}

//=============================================================================
// Operator Attribute Definition
//=============================================================================
struct OP_ATTRIBUTE {
    std::string  _name;           // Attribute name
    ATTR_TYPE    _type;           // Attribute type
    bool         _required;       // Is required
    std::any     _default_value;  // Default value (optional)

    OP_ATTRIBUTE() : _type(ATTR_TYPE::INT), _required(false) {}

    OP_ATTRIBUTE(const std::string& name, ATTR_TYPE type,
                 bool required = false, std::any default_val = {})
        : _name(name), _type(type), _required(required), _default_value(default_val) {}
};

//=============================================================================
// Operator Schema Definition
//=============================================================================
class OP_SCHEMA {
public:
    OP_SCHEMA();
    OP_SCHEMA(const std::string& name, air::base::OPCODE opcode);

    // Chain API for defining schema
    OP_SCHEMA& Attr(const std::string& name, ATTR_TYPE type,
                    bool required = false, std::any default_val = {});
    OP_SCHEMA& Input(const std::string& name, bool required = true);
    OP_SCHEMA& Output(const std::string& name);
    OP_SCHEMA& Shape(SHAPE_FN fn);

    // Validate and parse attributes
    bool Validate_and_parse(const pybind11::kwargs& kwargs,
                            std::map<std::string, std::any>& out_attr) const;

    // Accessors
    const std::string& Name() const { return _name; }
    air::base::OPCODE  Opcode() const { return _opcode; }

    // Input/Output access
    const std::vector<std::string>& Input() const { return _input; }
    const std::string& Output() const { return _output; }
    const std::vector<OP_ATTRIBUTE>& Attribute() const { return _attr; }

    // Check if attribute is supported
    bool Has_attr(const std::string& name) const;

    // Fill in missing attrs with default values from schema
    void Apply_defaults(std::map<std::string, std::any>& attrs) const;

    // Shape function access
    bool Has_shape() const { return _shape_fn != nullptr; }
    std::vector<int64_t> Compute_shape(const OP_CONTEXT& ctx) const;

private:
    // Member variables
    std::string               _name;
    air::base::OPCODE         _opcode;
    std::vector<OP_ATTRIBUTE> _attr;
    std::vector<std::string>  _input;
    std::string               _output;
    SHAPE_FN                   _shape_fn;
};

//=============================================================================
// Operator Schema Registry (Singleton)
//=============================================================================
class OP_SCHEMA_REGISTRY {
public:
    static OP_SCHEMA_REGISTRY& Instance();

    void Register(const std::string& name, OP_SCHEMA schema);
    const OP_SCHEMA* Get(const std::string& name) const;
    bool Has(const std::string& name) const;
    std::vector<std::string> Get_all_op_name() const;

private:
    // Non-copyable (singleton)
    OP_SCHEMA_REGISTRY(const OP_SCHEMA_REGISTRY&) = delete;
    OP_SCHEMA_REGISTRY& operator=(const OP_SCHEMA_REGISTRY&) = delete;

    // Member variables
    std::map<std::string, OP_SCHEMA> _schema_map;

    // Member functions
    OP_SCHEMA_REGISTRY() = default;
};

//=============================================================================
// Global Registration Function
//=============================================================================
void Register_all_ops();

}  // namespace frontend
}  // namespace ace

#endif  // AIR_OP_SCHEMA_H