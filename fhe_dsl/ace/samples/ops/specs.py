#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
CompileSpec instances for sample operations using three-layer architecture.

Layer 1: ModelEntity - describes the operation model
Layer 2: CompileConfig - describes how to compile
Layer 3: RuntimeConfig - describes how to run/validate
"""
import torch

from ace.fhe.spec import (
    CompileSpec, ModelEntity, CompileConfig, InputSpec
)
from ace.samples.ops import (
    LinearOp, LinearReluOp, ReluLinearOp, LinearBiasOp,
    MLP, FlattenOp, Conv2dOp, Conv2dReluOp, DepthwiseConv2dOp,
    AvgPool2dOp, MaxPool2dOp, GlobalAvgPool2dOp,
    AddOp, SubOp, MulOp, DivOp,
    ReluOp, SigmoidOp, TanhOp, SoftmaxOp, SqrtOp,
)


def _make_op_spec(name, model_class, model_init_args=(), input_shape=(1, 4), ir_ops=None):
    """Helper to build an op CompileSpec."""
    entity = ModelEntity(
        name=name,
        model_class=model_class,
        model_init_args=model_init_args,
        ir_ops=ir_ops,
    )

    compile_config = CompileConfig(
        input_spec=[InputSpec(shape=input_shape, dtype=torch.float32)],
        encrypt_inputs=["x"],
    )

    spec = CompileSpec(entity=entity, compile=compile_config)

    # Override create
    def create():
        model = model_class(*model_init_args)
        model._fhe_name = entity.name
        return model
    spec.create = create

    return spec


# =============================================================================
# Linear Operations
# =============================================================================

LINEAR_OP = _make_op_spec("linear_op", LinearOp, (4, 4), (1, 4), ir_ops=["Gemm"])
LINEAR_RELU_OP = _make_op_spec("linear_relu_op", LinearReluOp, (4, 4), (1, 4), ir_ops=["Gemm", "Relu"])
RELU_LINEAR_OP = _make_op_spec("relu_linear_op", ReluLinearOp, (4, 4), (1, 4), ir_ops=["Relu", "Gemm"])
LINEAR_BIAS_OP = _make_op_spec("linear_bias_op", LinearBiasOp, (4, 4), (1, 4), ir_ops=["Gemm"])
MLP_OP = _make_op_spec("mlp_op", MLP, (4, [8, 4]), (1, 4), ir_ops=["Gemm", "Relu", "Gemm"])
FLATTEN_OP = _make_op_spec("flatten_op", FlattenOp, (), (1, 3, 4, 4), ir_ops=["Flatten"])

# =============================================================================
# Convolutional Operations
# =============================================================================

CONV2D_OP = _make_op_spec("conv2d_op", Conv2dOp, (3, 16, 3), (1, 3, 16, 16), ir_ops=["Conv"])
CONV2D_RELU_OP = _make_op_spec("conv2d_relu_op", Conv2dReluOp, (3, 16, 3), (1, 3, 16, 16), ir_ops=["Conv", "Relu"])
DEPTHWISE_CONV2D_OP = _make_op_spec("depthwise_conv2d_op", DepthwiseConv2dOp, (3, 3, 3), (1, 3, 16, 16), ir_ops=["Conv"])

# =============================================================================
# Pooling Operations
# =============================================================================

AVG_POOL2D_OP = _make_op_spec("avg_pool2d_op", AvgPool2dOp, (2, 2), (1, 1, 8, 8), ir_ops=["AveragePool"])
MAX_POOL2D_OP = _make_op_spec("max_pool2d_op", MaxPool2dOp, (2, 2), (1, 1, 8, 8), ir_ops=["MaxPool"])
GLOBAL_AVG_POOL_OP = _make_op_spec("global_avg_pool_op", GlobalAvgPool2dOp, (), (1, 3, 8, 8), ir_ops=["GlobalAveragePool"])

# =============================================================================
# Arithmetic Operations
# =============================================================================

ADD_OP = _make_op_spec("add_op", AddOp, (), (1, 4), ir_ops=["Add"])
SUB_OP = _make_op_spec("sub_op", SubOp, (), (1, 4), ir_ops=["Sub"])
MUL_OP = _make_op_spec("mul_op", MulOp, (), (1, 4), ir_ops=["Mul"])
DIV_OP = _make_op_spec("div_op", DivOp, (), (1, 4), ir_ops=["Div"])

# =============================================================================
# Activation Operations
# =============================================================================

RELU_OP = _make_op_spec("relu_op", ReluOp, (), (1, 4), ir_ops=["Relu"])
SIGMOID_OP = _make_op_spec("sigmoid_op", SigmoidOp, (), (1, 4), ir_ops=["Sigmoid"])
TANH_OP = _make_op_spec("tanh_op", TanhOp, (), (1, 4), ir_ops=["Tanh"])
SOFTMAX_OP = _make_op_spec("softmax_op", SoftmaxOp, (1,), (1, 4), ir_ops=["Softmax"])
SQRT_OP = _make_op_spec("sqrt_op", SqrtOp, (), (1, 4), ir_ops=["Sqrt"])

# =============================================================================
# All Specs
# =============================================================================

ALL_OPS_SPECS = [
    LINEAR_OP, LINEAR_RELU_OP, RELU_LINEAR_OP, LINEAR_BIAS_OP, MLP_OP, FLATTEN_OP,
    CONV2D_OP, CONV2D_RELU_OP, DEPTHWISE_CONV2D_OP,
    AVG_POOL2D_OP, MAX_POOL2D_OP, GLOBAL_AVG_POOL_OP,
    ADD_OP, SUB_OP, MUL_OP, DIV_OP,
    RELU_OP, SIGMOID_OP, TANH_OP, SOFTMAX_OP, SQRT_OP,
]

__all__ = [
    "LINEAR_OP", "LINEAR_RELU_OP", "RELU_LINEAR_OP", "LINEAR_BIAS_OP", "MLP_OP", "FLATTEN_OP",
    "CONV2D_OP", "CONV2D_RELU_OP", "DEPTHWISE_CONV2D_OP",
    "AVG_POOL2D_OP", "MAX_POOL2D_OP", "GLOBAL_AVG_POOL_OP",
    "ADD_OP", "SUB_OP", "MUL_OP", "DIV_OP",
    "RELU_OP", "SIGMOID_OP", "TANH_OP", "SOFTMAX_OP", "SQRT_OP",
    "ALL_OPS_SPECS",
]