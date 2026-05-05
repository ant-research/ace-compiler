//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "frontend/layers/tensor_level_handler.h"

#include <iostream>
#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "frontend/core/type_factory.h"
#include "frontend/core/symbol_table.h"
#include "nn/core/opcode.h"

namespace ace {
namespace frontend {

// ========================================================================
// Constructor
// ========================================================================

TENSOR_LEVEL_HANDLER::TENSOR_LEVEL_HANDLER() {
    Init_op_map();
}

// ========================================================================
// Level Information
// ========================================================================

std::string TENSOR_LEVEL_HANDLER::Get_level_name() const {
    return "tensor";
}

LEVEL_TYPE TENSOR_LEVEL_HANDLER::Get_level_type() const {
    return LEVEL_TYPE::TENSOR;
}

// ========================================================================
// Operation Support
// ========================================================================

bool TENSOR_LEVEL_HANDLER::Has_op(const std::string& op_name) const {
    return _op_map.find(op_name) != _op_map.end();
}

std::vector<std::string> TENSOR_LEVEL_HANDLER::Get_supported_op() const {
    std::vector<std::string> ops;
    for (const auto& p : _op_map) {
        ops.push_back(p.first);
    }
    return ops;
}

air::base::OPCODE TENSOR_LEVEL_HANDLER::Get_opcode(const std::string& op_name) const {
    auto it = _op_map.find(op_name);
    if (it != _op_map.end()) {
        return it->second;
    }
    return air::base::OPCODE();
}

// ========================================================================
// Operation Processing
// ========================================================================

air::base::NODE_PTR TENSOR_LEVEL_HANDLER::Process_op(
    const std::string& op_name,
    const std::vector<std::string>& input_names,
    const std::map<std::string, std::any>& attrs,
    const std::map<std::string, std::string>& metadata,
    air::base::CONTAINER* cntr,
    const air::base::SPOS& spos,
    SYMBOL_TABLE* sym_tab,
    TYPE_FACTORY* type_factory,
    const std::vector<int64_t>& output_shape) {

    // Get opcode
    air::base::OPCODE opcode = Get_opcode(op_name);
    if (opcode == air::base::OPCODE::INVALID) {
        std::cerr << "[TENSOR_LEVEL_HANDLER] Invalid opcode for: " << op_name << std::endl;
        return air::base::NODE_PTR();
    }

    // Resolve inputs from symbol table first (needed for type inference)
    std::vector<air::base::NODE_PTR> inputs;
    for (const auto& name : input_names) {
        air::base::NODE_PTR input_node = sym_tab->Resolve_input(name, cntr, spos);
        if (input_node == air::base::Null_ptr) {
            std::cerr << "[TENSOR_LEVEL_HANDLER] Failed to resolve input: " << name << std::endl;
            return air::base::NODE_PTR();
        }
        inputs.push_back(input_node);
    }

    // Determine result type
    // If output_shape is provided and type_factory is available, use it to create type
    // Otherwise, use the first input's Access_type (preserve tensor shape)
    air::base::TYPE_PTR rtype;
    if (!output_shape.empty() && type_factory) {
        rtype = type_factory->New_tensor_type(output_shape);
    } else {
        rtype = inputs[0]->Access_type();
    }

    // Create operation node based on input count
    air::base::NODE_PTR op_node;
    size_t input_count = input_names.size();

    if (input_count == 3) {
        // Ternary operation (conv with bias, gemm)
        op_node = cntr->New_tern_arith(opcode, rtype, inputs[0], inputs[1], inputs[2], spos);
    } else if (input_count == 2) {
        // Binary operation OR conv without bias
        // Check if it's conv (needs special handling)
        if (op_name == "conv") {
            // For conv without bias, use ternary op with duplicated input
            op_node = cntr->New_tern_arith(opcode, rtype, inputs[0], inputs[1], inputs[0], spos);
        } else {
            // Binary operation (add, sub, mul, div, matmul)
            op_node = cntr->New_bin_arith(opcode, rtype, inputs[0], inputs[1], spos);
        }
    } else if (input_count == 1) {
        // Unary operation (relu, softmax, sqrt, silu, flatten)
        op_node = cntr->New_una_arith(opcode, rtype, inputs[0], spos);
    } else {
        std::cerr << "[TENSOR_LEVEL_HANDLER] Unsupported input count: " << input_count << std::endl;
        return air::base::NODE_PTR();
    }

    // Set attributes
    if (op_node != air::base::Null_ptr) {
        Set_node_attrs(op_node, attrs, metadata);
    }

    return op_node;
}

// ========================================================================
// Private Helpers
// ========================================================================

void TENSOR_LEVEL_HANDLER::Init_op_map() {
    // Binary operations
    _op_map["add"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD);
    _op_map["sub"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SUB);
    _op_map["mul"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MUL);
    _op_map["div"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::DIVIDE);
    _op_map["matmul"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MATMUL);
    _op_map["concat"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONCAT);

    // Unary operations
    _op_map["relu"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RELU);
    _op_map["softmax"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SOFTMAX);
    _op_map["sqrt"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SQRT);
    _op_map["silu"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SILU);
    _op_map["flatten"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN);

    // Pooling operations
    _op_map["max_pool"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MAX_POOL);
    _op_map["average_pool"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::AVERAGE_POOL);
    _op_map["global_average_pool"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL);

    // Linear operations
    _op_map["conv"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV);
    _op_map["gemm"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GEMM);
    _op_map["reshape"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE);
    _op_map["transpose"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::TRANSPOSE);
    _op_map["strided_slice"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE);

    // Additional operations
    _op_map["bmm"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::BMM);
    _op_map["sigmoid"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SOFTMAX);  // reuse softmax opcode
    _op_map["rmsnorm"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RMSNORM);
    _op_map["swiglu"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SWIGLU);
    _op_map["rope"] = air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ROPE);
}

void TENSOR_LEVEL_HANDLER::Set_node_attrs(
    air::base::NODE_PTR node,
    const std::map<std::string, std::any>& attrs,
    const std::map<std::string, std::string>& metadata) {

    // Set name attribute first (ATTR[0]) if provided in metadata
    // This aligns with onnx2air reference format
    auto it = metadata.find("onnx_name");
    if (it != metadata.end()) {
        node->Set_attr("name", it->second.c_str());
    }

    // Set remaining attributes
    for (const auto& [key, value] : attrs) {
        // Try int
        try {
            int int_val = std::any_cast<int>(value);
            node->Set_attr(key.c_str(), &int_val, 1);
            continue;
        } catch (const std::bad_any_cast&) {}

        // Try float
        try {
            float float_val = std::any_cast<float>(value);
            node->Set_attr(key.c_str(), &float_val, 1);
            continue;
        } catch (const std::bad_any_cast&) {}

        // Try vector<int>
        try {
            auto vec_val = std::any_cast<std::vector<int>>(value);
            node->Set_attr(key.c_str(), vec_val.data(), vec_val.size());
            continue;
        } catch (const std::bad_any_cast&) {}

        // Try vector<float>
        try {
            auto vec_val = std::any_cast<std::vector<float>>(value);
            node->Set_attr(key.c_str(), vec_val.data(), vec_val.size());
            continue;
        } catch (const std::bad_any_cast&) {}

        std::cerr << "[TENSOR_LEVEL_HANDLER] Unknown attribute type: " << key << std::endl;
    }
}

void TENSOR_LEVEL_HANDLER::Set_node_metadata(
    air::base::NODE_PTR node,
    const std::map<std::string, std::string>& metadata) {

    // Set name attribute if provided (for pragma generation)
    auto it = metadata.find("onnx_name");
    if (it != metadata.end()) {
        node->Set_attr("name", it->second.c_str());
    }
}

}  // namespace frontend
}  // namespace ace