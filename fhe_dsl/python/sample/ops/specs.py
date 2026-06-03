#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ModelSpec instances for sample operations.
"""
import torch

from ace.fhe.config.spec import ModelSpec
from ace.sample.ops import (
    LinearOp, LinearReluOp, ReluLinearOp, LinearBiasOp,
    MLP, FlattenOp, Conv2dOp, Conv2dReluOp, DepthwiseConv2dOp,
    AvgPool2dOp, MaxPool2dOp, GlobalAvgPool2dOp,
    AvgPoolConv2dOp, Conv2dAvgPool2dOp, ReluAvgPoolOp, AvgPoolFlattenOp,
    AddOp, SubOp, MulOp, DivOp,
    ReluOp, SigmoidOp, TanhOp, SoftmaxOp, SqrtOp,
    AddConstOp, MulConstOp,
)


def _make_op_spec(name, model_class, model_init_args=(), input_shape=(1, 4), ir_ops=None):
    """Helper to build an op ModelSpec."""
    return ModelSpec(
        name=name,
        model_class=model_class,
        example_inputs=(torch.randn(*input_shape, dtype=torch.float32),),
        encrypt_inputs=["x"],
        model_init_args=model_init_args,
        expected_ops=ir_ops,
    )


# =============================================================================
# Linear Operations
# =============================================================================

LINEAR_OP = _make_op_spec("linear_op", LinearOp, (4, 4), (1, 4), ir_ops=["Gemm"])
LINEAR_RELU_OP = _make_op_spec("linear_relu_op", LinearReluOp, (4, 4), (1, 4), ir_ops=["Gemm", "Relu"])
RELU_LINEAR_OP = _make_op_spec("relu_linear_op", ReluLinearOp, (4, 4), (1, 4), ir_ops=["Relu", "Gemm"])
LINEAR_BIAS_OP = _make_op_spec("linear_bias_op", LinearBiasOp, (4, 4), (1, 4), ir_ops=["Gemm"])
MLP_OP = _make_op_spec("mlp_op", MLP, (4, 8, 4), (1, 4), ir_ops=["Gemm", "Relu", "Gemm"])
FLATTEN_OP = _make_op_spec("flatten_op", FlattenOp, (), (1, 3, 4, 4), ir_ops=["Flatten"])

# =============================================================================
# Convolutional Operations
# =============================================================================

CONV2D_OP = _make_op_spec("conv2d_op", Conv2dOp, (3, 16, 3), (1, 3, 16, 16), ir_ops=["Conv"])
CONV2D_RELU_OP = _make_op_spec("conv2d_relu_op", Conv2dReluOp, (3, 16, 3), (1, 3, 16, 16), ir_ops=["Conv", "Relu"])
DEPTHWISE_CONV2D_OP = _make_op_spec("depthwise_conv2d_op", DepthwiseConv2dOp, (3, 3), (1, 3, 16, 16), ir_ops=["Conv"])

# =============================================================================
# Pooling Operations
# =============================================================================

AVG_POOL2D_OP = _make_op_spec("avg_pool2d_op", AvgPool2dOp, (2, 2), (1, 1, 8, 8), ir_ops=["AveragePool"])
MAX_POOL2D_OP = _make_op_spec("max_pool2d_op", MaxPool2dOp, (2, 2), (1, 1, 8, 8), ir_ops=["MaxPool"])
GLOBAL_AVG_POOL_OP = _make_op_spec("global_avg_pool_op", GlobalAvgPool2dOp, (), (1, 3, 8, 8), ir_ops=["GlobalAveragePool"])

# =============================================================================
# Arithmetic Operations (binary: require two inputs)
# =============================================================================

ADD_OP = ModelSpec(
    name="add_op",
    model_class=AddOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["Add"],
)
SUB_OP = ModelSpec(
    name="sub_op",
    model_class=SubOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["Sub"],
)
MUL_OP = ModelSpec(
    name="mul_op",
    model_class=MulOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["Mul"],
)
DIV_OP = ModelSpec(
    name="div_op",
    model_class=DivOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["Div"],
)

# =============================================================================
# Activation Operations
# =============================================================================

RELU_OP = _make_op_spec("relu_op", ReluOp, (), (1, 4), ir_ops=["Relu"])
SIGMOID_OP = _make_op_spec("sigmoid_op", SigmoidOp, (), (1, 4), ir_ops=["Sigmoid"])
TANH_OP = _make_op_spec("tanh_op", TanhOp, (), (1, 4), ir_ops=["Tanh"])
SOFTMAX_OP = _make_op_spec("softmax_op", SoftmaxOp, (1,), (1, 4), ir_ops=["Softmax"])
SQRT_OP = _make_op_spec("sqrt_op", SqrtOp, (), (1, 4), ir_ops=["Sqrt"])

# =============================================================================
# Constant Operations
# =============================================================================

ADD_CONST_OP = _make_op_spec("add_const", AddConstOp, (), (1, 1, 2, 2), ir_ops=["Add"])
MUL_CONST_OP = _make_op_spec("mult_const", MulConstOp, (), (1, 1, 2, 2), ir_ops=["Mul", "Constant"])

# =============================================================================
# GEMM Variants (for regression tests)
# =============================================================================

GEMM_49X3_OP = ModelSpec(
    name="gemm_49x3",
    model_class=LinearOp,
    example_inputs=(torch.flatten(torch.randn(1, 1, 7, 7), 1),),
    encrypt_inputs=["x"],
    model_init_args=(49, 3),
    expected_ops=["Gemm"],
)
RELU_GEMM_OP = ModelSpec(
    name="relu_gemm",
    model_class=ReluLinearOp,
    example_inputs=(torch.flatten(torch.randn(1, 1, 1, 3), 1),),
    encrypt_inputs=["x"],
    model_init_args=(3, 2),
    expected_ops=["Gemm", "Relu"],
)

# =============================================================================
# Conv Test Variants (for regression tests)
# =============================================================================

CONV2D_TEST_OP = ModelSpec(
    name="conv2d",
    model_class=Conv2dOp,
    example_inputs=(torch.randn(1, 3, 16, 16, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(3, 16, 3),
    expected_ops=["Conv"],
)

# =============================================================================
# Pool Test Variants (for regression tests)
# =============================================================================

AVG_POOL_2D_OP = ModelSpec(
    name="avg_pool_2d",
    model_class=AvgPool2dOp,
    example_inputs=(torch.randn(1, 1, 4, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(2, 2),
    expected_ops=["AveragePool"],
)
AVG_POOL_2D_STRIDE_OP = ModelSpec(
    name="avg_pool_2d_with_stride",
    model_class=AvgPool2dOp,
    example_inputs=(torch.randn(1, 1, 8, 8, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(2, 2, False),
    expected_ops=["AveragePool"],
)
GLOBAL_AVG_POOL_TEST_OP = ModelSpec(
    name="global_avg_pool",
    model_class=GlobalAvgPool2dOp,
    example_inputs=(torch.randn(1, 3, 8, 8, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["GlobalAveragePool"],
)
RELU_AVG_POOL_OP = ModelSpec(
    name="relu_avg_pool",
    model_class=ReluAvgPoolOp,
    example_inputs=(torch.randn(1, 3, 4, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["Relu", "AveragePool"],
)
AVG_POOL_FLATTEN_OP = ModelSpec(
    name="avg_pool_flatten",
    model_class=AvgPoolFlattenOp,
    example_inputs=(torch.randn(1, 3, 4, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["AveragePool", "Flatten"],
)

# =============================================================================
# All Specs
# =============================================================================

ALL_OPS_SPECS = [
    LINEAR_OP, LINEAR_RELU_OP, RELU_LINEAR_OP, LINEAR_BIAS_OP, MLP_OP, FLATTEN_OP,
    CONV2D_OP, CONV2D_RELU_OP, DEPTHWISE_CONV2D_OP,
    AVG_POOL2D_OP, MAX_POOL2D_OP, GLOBAL_AVG_POOL_OP,
    ADD_OP, SUB_OP, MUL_OP, DIV_OP,
    RELU_OP, SIGMOID_OP, TANH_OP, SOFTMAX_OP, SQRT_OP,
    ADD_CONST_OP, MUL_CONST_OP,
    GEMM_49X3_OP, RELU_GEMM_OP,
    CONV2D_TEST_OP,
    AVG_POOL_2D_OP, AVG_POOL_2D_STRIDE_OP, GLOBAL_AVG_POOL_TEST_OP,
    RELU_AVG_POOL_OP, AVG_POOL_FLATTEN_OP,
]

__all__ = [
    "LINEAR_OP", "LINEAR_RELU_OP", "RELU_LINEAR_OP", "LINEAR_BIAS_OP", "MLP_OP", "FLATTEN_OP",
    "CONV2D_OP", "CONV2D_RELU_OP", "DEPTHWISE_CONV2D_OP",
    "AVG_POOL2D_OP", "MAX_POOL2D_OP", "GLOBAL_AVG_POOL_OP",
    "ADD_OP", "SUB_OP", "MUL_OP", "DIV_OP",
    "RELU_OP", "SIGMOID_OP", "TANH_OP", "SOFTMAX_OP", "SQRT_OP",
    "ADD_CONST_OP", "MUL_CONST_OP",
    "GEMM_49X3_OP", "RELU_GEMM_OP",
    "CONV2D_TEST_OP",
    "AVG_POOL_2D_OP", "AVG_POOL_2D_STRIDE_OP", "GLOBAL_AVG_POOL_TEST_OP",
    "RELU_AVG_POOL_OP", "AVG_POOL_FLATTEN_OP",
    "ALL_OPS_SPECS",
]