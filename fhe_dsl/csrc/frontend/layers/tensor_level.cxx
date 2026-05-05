//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "frontend/layers/tensor_level.h"

#include "frontend/core/ir_builder.h"
#include "frontend/ops/torch_op_handler.h"

#include <algorithm>

#include <nn/core/opcode.h>

#include <torch/library.h>
#include <iostream>

namespace ace {
namespace frontend {

//! @brief Helper to get tensor name from TORCH_OP_HANDLER registry
static std::string GetTensorName(const at::Tensor& tensor, const std::string& default_name) {
    if (tensor.defined()) {
        uintptr_t ptr = reinterpret_cast<uintptr_t>(tensor.data_ptr());
        std::string name = TORCH_OP_HANDLER::Lookup_tensor_name(ptr);
        if (!name.empty()) {
            return name;
        }
    }
    return default_name;
}

//! @brief Helper macro to generate AIR IR for binary operations
#define TENSOR_GEN_AIR_OP2(op_name, opcode, t0, t1) \
    do { \
        if (IR_BUILDER::Instance().Is_building()) { \
            std::string name0 = GetTensorName(t0, "x"); \
            std::string name1 = GetTensorName(t1, "y"); \
            IR_BUILDER::Instance().Add_operation_with_opcode( \
                op_name, \
                {name0, name1}, \
                air::base::OPCODE(nn::core::NN, nn::core::OPCODE::opcode) \
            ); \
        } \
    } while(0)

//! @brief Helper macro to generate AIR IR for unary operations
#define TENSOR_GEN_AIR_OP1(op_name, opcode, t0) \
    do { \
        if (IR_BUILDER::Instance().Is_building()) { \
            std::string name0 = GetTensorName(t0, "x"); \
            IR_BUILDER::Instance().Add_operation_with_opcode( \
                op_name, \
                {name0}, \
                air::base::OPCODE(nn::core::NN, nn::core::OPCODE::opcode) \
            ); \
        } \
    } while(0)

//! @brief Helper macro to generate AIR IR for ternary operations
#define TENSOR_GEN_AIR_OP3(op_name, opcode, t0, t1, t2) \
    do { \
        if (IR_BUILDER::Instance().Is_building()) { \
            std::string name0 = GetTensorName(t0, "a"); \
            std::string name1 = GetTensorName(t1, "b"); \
            std::string name2 = GetTensorName(t2, "c"); \
            IR_BUILDER::Instance().Add_operation_with_opcode( \
                op_name, \
                {name0, name1, name2}, \
                air::base::OPCODE(nn::core::NN, nn::core::OPCODE::opcode) \
            ); \
        } \
    } while(0)

TENSOR_LEVEL::TENSOR_LEVEL() : _handler(std::make_unique<TENSOR_LEVEL_HANDLER>()) {
    // Opcode map now delegated to TENSOR_LEVEL_HANDLER
}

std::string TENSOR_LEVEL::Get_level_name() const {
    return "tensor";
}

LEVEL_TYPE TENSOR_LEVEL::Get_level_type() const {
    return LEVEL_TYPE::TENSOR;
}

void TENSOR_LEVEL::Register_py_ops(pybind11::module& m) {
    // Binary operators
    m.def("tensor_add", &TENSOR_LEVEL::Op_add, "Tensor addition");
    m.def("tensor_sub", &TENSOR_LEVEL::Op_sub, "Tensor subtraction");
    m.def("tensor_mul", &TENSOR_LEVEL::Op_mul, "Tensor multiplication");
    m.def("tensor_div", &TENSOR_LEVEL::Op_div, "Tensor division");
    m.def("tensor_matmul", &TENSOR_LEVEL::Op_matmul, "Tensor matrix multiplication");
    m.def("tensor_concat", &TENSOR_LEVEL::Op_concat, "Tensor concatenation");

    // Unary operators
    m.def("tensor_relu", &TENSOR_LEVEL::Op_relu, "ReLU activation");
    m.def("tensor_softmax", &TENSOR_LEVEL::Op_softmax, "Softmax");
    m.def("tensor_max_pool", &TENSOR_LEVEL::Op_max_pool, "Max pooling");
    m.def("tensor_average_pool", &TENSOR_LEVEL::Op_average_pool, "Average pooling");
    m.def("tensor_global_average_pool", &TENSOR_LEVEL::Op_global_average_pool, "Global average pooling");
    m.def("tensor_flatten", &TENSOR_LEVEL::Op_flatten, "Flatten");
    m.def("tensor_sqrt", &TENSOR_LEVEL::Op_sqrt, "Square root");
    m.def("tensor_silu", &TENSOR_LEVEL::Op_silu, "SiLU activation");

    // Ternary operators
    m.def("tensor_conv", &TENSOR_LEVEL::Op_conv, "Convolution");
    m.def("tensor_gemm", &TENSOR_LEVEL::Op_gemm, "Generalized matrix multiplication");
}

at::Tensor TENSOR_LEVEL::Build_op(const std::string& op_name,
                                 const std::vector<at::Tensor>& inputs) {
    if (inputs.empty()) {
        return at::Tensor();
    }

    // Dispatch to appropriate handler based on input count
    if (inputs.size() == 1) {
        // Unary operation
        if (op_name == "relu") return Op_relu(inputs[0]);
        if (op_name == "softmax") return Op_softmax(inputs[0]);
        if (op_name == "max_pool") return Op_max_pool(inputs[0]);
        if (op_name == "average_pool") return Op_average_pool(inputs[0]);
        if (op_name == "global_average_pool") return Op_global_average_pool(inputs[0]);
        if (op_name == "flatten") return Op_flatten(inputs[0]);
        if (op_name == "sqrt") return Op_sqrt(inputs[0]);
        if (op_name == "silu") return Op_silu(inputs[0]);
    } else if (inputs.size() == 2) {
        // Binary operation
        if (op_name == "add") return Op_add(inputs[0], inputs[1]);
        if (op_name == "sub") return Op_sub(inputs[0], inputs[1]);
        if (op_name == "mul") return Op_mul(inputs[0], inputs[1]);
        if (op_name == "div") return Op_div(inputs[0], inputs[1]);
        if (op_name == "matmul") return Op_matmul(inputs[0], inputs[1]);
        if (op_name == "concat") return Op_concat(inputs[0], inputs[1]);
    } else if (inputs.size() == 3) {
        // Ternary operation
        if (op_name == "conv") return Op_conv(inputs[0], inputs[1], inputs[2]);
        if (op_name == "gemm") return Op_gemm(inputs[0], inputs[1], inputs[2]);
    }

    std::cerr << "[TENSOR_LEVEL] Unknown operation: " << op_name << std::endl;
    return inputs[0];
}

bool TENSOR_LEVEL::Has_op(const std::string& op_name) const {
    return _handler->Has_op(op_name);
}

std::vector<std::string> TENSOR_LEVEL::Get_supported_op() const {
    return _handler->Get_supported_op();
}

air::base::OPCODE TENSOR_LEVEL::Get_opcode(const std::string& op_name) const {
    return _handler->Get_opcode(op_name);
}

// ========================================================================
// Tensor Operation Implementations
// ========================================================================

at::Tensor TENSOR_LEVEL::Op_add(const at::Tensor& x, const at::Tensor& y) {
    std::cout << "[TENSOR_LEVEL] Op_add" << std::endl;
    TENSOR_GEN_AIR_OP2("add", ADD, x, y);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_sub(const at::Tensor& x, const at::Tensor& y) {
    std::cout << "[TENSOR_LEVEL] Op_sub" << std::endl;
    TENSOR_GEN_AIR_OP2("sub", SUB, x, y);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_mul(const at::Tensor& x, const at::Tensor& y) {
    std::cout << "[TENSOR_LEVEL] Op_mul" << std::endl;
    TENSOR_GEN_AIR_OP2("mul", MUL, x, y);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_div(const at::Tensor& x, const at::Tensor& y) {
    std::cout << "[TENSOR_LEVEL] Op_div" << std::endl;
    TENSOR_GEN_AIR_OP2("div", DIVIDE, x, y);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_matmul(const at::Tensor& x, const at::Tensor& y) {
    std::cout << "[TENSOR_LEVEL] Op_matmul" << std::endl;
    TENSOR_GEN_AIR_OP2("matmul", MATMUL, x, y);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_concat(const at::Tensor& x, const at::Tensor& y) {
    std::cout << "[TENSOR_LEVEL] Op_concat" << std::endl;
    TENSOR_GEN_AIR_OP2("concat", CONCAT, x, y);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_relu(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_relu" << std::endl;
    TENSOR_GEN_AIR_OP1("relu", RELU, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_softmax(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_softmax" << std::endl;
    TENSOR_GEN_AIR_OP1("softmax", SOFTMAX, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_max_pool(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_max_pool" << std::endl;
    TENSOR_GEN_AIR_OP1("max_pool", MAX_POOL, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_average_pool(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_average_pool" << std::endl;
    TENSOR_GEN_AIR_OP1("average_pool", AVERAGE_POOL, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_global_average_pool(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_global_average_pool" << std::endl;
    TENSOR_GEN_AIR_OP1("global_average_pool", GLOBAL_AVERAGE_POOL, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_flatten(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_flatten" << std::endl;
    TENSOR_GEN_AIR_OP1("flatten", FLATTEN, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_sqrt(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_sqrt" << std::endl;
    TENSOR_GEN_AIR_OP1("sqrt", SQRT, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_silu(const at::Tensor& x) {
    std::cout << "[TENSOR_LEVEL] Op_silu" << std::endl;
    TENSOR_GEN_AIR_OP1("silu", SILU, x);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_conv(const at::Tensor& x, const at::Tensor& w, const at::Tensor& b) {
    std::cout << "[TENSOR_LEVEL] Op_conv" << std::endl;
    TENSOR_GEN_AIR_OP3("conv", CONV, x, w, b);
    return x;
}

at::Tensor TENSOR_LEVEL::Op_gemm(const at::Tensor& a, const at::Tensor& b, const at::Tensor& c) {
    std::cout << "[TENSOR_LEVEL] Op_gemm" << std::endl;
    TENSOR_GEN_AIR_OP3("gemm", GEMM, a, b, c);
    return a;
}

}  // namespace frontend
}  // namespace ace