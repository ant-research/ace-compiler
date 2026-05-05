//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "frontend/ops/op_registry.h"
#include "frontend/ops/op_schema.h"
#include "frontend/ops/torch_op_handler.h"

#include "nn/core/opcode.h"

namespace ace {
namespace frontend {

//=============================================================================
// NN Core (Tensor Level) Operator Registration
//=============================================================================

static void Register_nn_add_op() {
    OP_SCHEMA schema("add", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD));
    schema.Input("x", true)
          .Input("y", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("add", schema);
}

static void Register_nn_sub_op() {
    OP_SCHEMA schema("sub", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SUB));
    schema.Input("x", true)
          .Input("y", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("sub", schema);
}

static void Register_nn_mul_op() {
    OP_SCHEMA schema("mul", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MUL));
    schema.Input("x", true)
          .Input("y", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("mul", schema);
}

static void Register_nn_div_op() {
    OP_SCHEMA schema("div", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::DIVIDE));
    schema.Input("x", true)
          .Input("y", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("div", schema);
}

static void Register_nn_matmul_op() {
    OP_SCHEMA schema("matmul", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MATMUL));
    schema.Input("x", true)
          .Input("y", true)
          .Shape(&TORCH_OP_HANDLER::Matmul_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("matmul", schema);
}

static void Register_nn_concat_op() {
    OP_SCHEMA schema("concat", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONCAT));
    schema.Input("x", true)
          .Input("y", true)
          .Attr("axis", ATTR_TYPE::INT, false, -1)
          .Shape(&TORCH_OP_HANDLER::Concat_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("concat", schema);
}

static void Register_nn_relu_op() {
    OP_SCHEMA schema("relu", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RELU));
    schema.Input("x", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("relu", schema);
}

static void Register_nn_softmax_op() {
    OP_SCHEMA schema("softmax", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SOFTMAX));
    schema.Input("x", true)
          .Attr("axis", ATTR_TYPE::INT, false, -1)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("softmax", schema);
}

static void Register_nn_max_pool_op() {
    OP_SCHEMA schema("max_pool", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MAX_POOL));
    schema.Input("x", true)
          .Attr("kernel_shape", ATTR_TYPE::INTS, false)
          .Attr("strides", ATTR_TYPE::INTS, false)
          .Attr("pads", ATTR_TYPE::INTS, false)
          .Shape(&TORCH_OP_HANDLER::Pool_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("max_pool", schema);
}

static void Register_nn_average_pool_op() {
    OP_SCHEMA schema("average_pool", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::AVERAGE_POOL));
    schema.Input("x", true)
          .Attr("kernel_shape", ATTR_TYPE::INTS, false)
          .Attr("strides", ATTR_TYPE::INTS, false)
          .Attr("pads", ATTR_TYPE::INTS, false)
          .Shape(&TORCH_OP_HANDLER::Pool_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("average_pool", schema);
}

static void Register_nn_global_average_pool_op() {
    OP_SCHEMA schema("global_average_pool", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL));
    schema.Input("x", true)
          .Shape(&TORCH_OP_HANDLER::Global_pool_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("global_average_pool", schema);
}

static void Register_nn_flatten_op() {
    OP_SCHEMA schema("flatten", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN));
    schema.Input("x", true)
          .Attr("start_dim", ATTR_TYPE::INT, false, 1)
          .Attr("end_dim", ATTR_TYPE::INT, false, -1)
          .Shape(&TORCH_OP_HANDLER::Flatten_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("flatten", schema);
}

static void Register_nn_reshape_op() {
    OP_SCHEMA schema("reshape", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE));
    schema.Input("x", true)
          .Attr("shape", ATTR_TYPE::INTS, false)
          .Shape(&TORCH_OP_HANDLER::Reshape_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("reshape", schema);
}

static void Register_nn_sqrt_op() {
    OP_SCHEMA schema("sqrt", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SQRT));
    schema.Input("x", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("sqrt", schema);
}

static void Register_nn_silu_op() {
    OP_SCHEMA schema("silu", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SILU));
    schema.Input("x", true)
          .Shape(&TORCH_OP_HANDLER::Passthrough_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("silu", schema);
}

// NOTE: BatchNorm is folded into Conv/Linear in frontend, so no need to register
// void Register_nn_batch_norm_op() {
//     OP_SCHEMA schema("batch_norm", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::BATCH_NORM));
//     schema.Input("x", true)
//           .Input("weight", false)
//           .Input("bias", false)
//           .Input("running_mean", false)
//           .Input("running_var", false)
//           .Attr("epsilon", ATTR_TYPE::FLOAT, false, 1e-5f)
//           .Attr("momentum", ATTR_TYPE::FLOAT, false, 0.1f)
//           .Attr("training", ATTR_TYPE::INT, false, 0);
//     OP_SCHEMA_REGISTRY::Instance().Register("batch_norm", schema);
// }

static void Register_nn_conv_op() {
    OP_SCHEMA schema("conv", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV));
    schema.Input("x", true)
          .Input("w", true)
          .Input("b", false)
          .Attr("kernel_shape", ATTR_TYPE::INTS, false)
          .Attr("strides", ATTR_TYPE::INTS, false, std::vector<int>{1, 1})
          .Attr("pads", ATTR_TYPE::INTS, false)
          .Attr("dilations", ATTR_TYPE::INTS, false, std::vector<int>{1, 1})
          .Attr("groups", ATTR_TYPE::INT, false, 1)
          .Shape(&TORCH_OP_HANDLER::Conv_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("conv", schema);
}

static void Register_nn_gemm_op() {
    OP_SCHEMA schema("gemm", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GEMM));
    schema.Input("a", true)
          .Input("b", true)
          .Input("c", false)
          .Attr("alpha", ATTR_TYPE::FLOAT, false, 1.0f)
          .Attr("beta", ATTR_TYPE::FLOAT, false, 1.0f)
          .Attr("transA", ATTR_TYPE::INT, false, 0)
          .Attr("transB", ATTR_TYPE::INT, false, 0)
          .Shape(&TORCH_OP_HANDLER::Gemm_shape);
    OP_SCHEMA_REGISTRY::Instance().Register("gemm", schema);
}

//=============================================================================
// Vector Level Operator Registration (Future Extension)
//=============================================================================
static void Register_vector_op() {
    // TODO: Add Vector level operator registration
}

//=============================================================================
// CKKS Level Operator Registration (Future Extension)
//=============================================================================
static void Register_ckks_op() {
    // TODO: Add CKKS level operator registration
}

//=============================================================================
// SIHE Level Operator Registration (Future Extension)
//=============================================================================
static void Register_sihe_op() {
    // TODO: Add SIHE level operator registration
}

//=============================================================================
// Poly Level Operator Registration (Future Extension)
//=============================================================================
static void Register_poly_op() {
    // TODO: Add Poly level operator registration
}

//=============================================================================
// Register_all_ops - Register All Level Operators
//=============================================================================
void Register_all_ops() {
    // NN Core (Tensor Level)
    Register_nn_add_op();
    Register_nn_sub_op();
    Register_nn_mul_op();
    Register_nn_div_op();
    Register_nn_matmul_op();
    Register_nn_concat_op();

    Register_nn_relu_op();
    Register_nn_softmax_op();
    Register_nn_max_pool_op();
    Register_nn_average_pool_op();
    Register_nn_global_average_pool_op();
    Register_nn_flatten_op();
    Register_nn_reshape_op();
    Register_nn_sqrt_op();
    Register_nn_silu_op();
    // Register_nn_batch_norm_op();  // BN folded into Conv/Linear

    Register_nn_conv_op();
    Register_nn_gemm_op();

    // Vector Level
    Register_vector_op();

    // CKKS Level
    Register_ckks_op();

    // SIHE Level
    Register_sihe_op();

    // Poly Level
    Register_poly_op();
}

}  // namespace frontend
}  // namespace ace