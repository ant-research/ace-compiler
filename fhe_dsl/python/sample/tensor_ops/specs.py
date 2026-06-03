#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ModelSpec instances for custom C++ operator models.

These specs pair tensor_ops models with their example_inputs and encrypt_inputs,
following the same pattern as ace.sample.ops.specs.
"""
import torch

from ace.fhe.config.spec import ModelSpec
from ace.sample.tensor_ops import (
    AddTensorOp,
    SubTensorOp,
    MulTensorOp,
    DivTensorOp,
    MatmulTensorOp,
    ConcatTensorOp,
    ReLUTensorOp,
    SoftmaxTensorOp,
    MaxPoolTensorOp,
    AvgPoolTensorOp,
    GlobalAvgPoolTensorOp,
    FlattenTensorOp,
    SqrtTensorOp,
    SiluTensorOp,
    ConvTensorOp,
    GemmTensorOp,
    CompositeTensorOp,
)


# =============================================================================
# Binary Ops
# =============================================================================

ADD_TENSOR_OP = ModelSpec(
    name="add_tensor_op",
    model_class=AddTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["add"],
)

SUB_TENSOR_OP = ModelSpec(
    name="sub_tensor_op",
    model_class=SubTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["sub"],
)

MUL_TENSOR_OP = ModelSpec(
    name="mul_tensor_op",
    model_class=MulTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["mul"],
)

DIV_TENSOR_OP = ModelSpec(
    name="div_tensor_op",
    model_class=DivTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["div"],
)

MATMUL_TENSOR_OP = ModelSpec(
    name="matmul_tensor_op",
    model_class=MatmulTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(4, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["matmul"],
)

CONCAT_TENSOR_OP = ModelSpec(
    name="concat_tensor_op",
    model_class=ConcatTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["concat"],
)


# =============================================================================
# Unary Ops
# =============================================================================

RELU_TENSOR_OP = ModelSpec(
    name="relu_tensor_op",
    model_class=ReLUTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["relu"],
)

SOFTMAX_TENSOR_OP = ModelSpec(
    name="softmax_tensor_op",
    model_class=SoftmaxTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["softmax"],
)

MAX_POOL_TENSOR_OP = ModelSpec(
    name="max_pool_tensor_op",
    model_class=MaxPoolTensorOp,
    example_inputs=(torch.randn(1, 1, 4, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["max_pool"],
)

AVG_POOL_TENSOR_OP = ModelSpec(
    name="avg_pool_tensor_op",
    model_class=AvgPoolTensorOp,
    example_inputs=(torch.randn(1, 1, 4, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["average_pool"],
)

GLOBAL_AVG_POOL_TENSOR_OP = ModelSpec(
    name="global_avg_pool_tensor_op",
    model_class=GlobalAvgPoolTensorOp,
    example_inputs=(torch.randn(1, 3, 8, 8, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["global_average_pool"],
)

FLATTEN_TENSOR_OP = ModelSpec(
    name="flatten_tensor_op",
    model_class=FlattenTensorOp,
    example_inputs=(torch.randn(1, 3, 8, 8, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["flatten"],
)

SQRT_TENSOR_OP = ModelSpec(
    name="sqrt_tensor_op",
    model_class=SqrtTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["sqrt"],
)

SILU_TENSOR_OP = ModelSpec(
    name="silu_tensor_op",
    model_class=SiluTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32),),
    encrypt_inputs=["x"],
    expected_ops=["silu"],
)


# =============================================================================
# Ternary Ops
# =============================================================================

CONV_TENSOR_OP = ModelSpec(
    name="conv_tensor_op",
    model_class=ConvTensorOp,
    example_inputs=(
        torch.randn(1, 1, 8, 8, dtype=torch.float32),
        torch.randn(16, 1, 3, 3, dtype=torch.float32),
        torch.randn(16, dtype=torch.float32),
    ),
    encrypt_inputs=["x", "w", "b"],
    expected_ops=["conv"],
)

GEMM_TENSOR_OP = ModelSpec(
    name="gemm_tensor_op",
    model_class=GemmTensorOp,
    example_inputs=(
        torch.randn(1, 4, dtype=torch.float32),
        torch.randn(4, 4, dtype=torch.float32),
        torch.randn(4, dtype=torch.float32),
    ),
    encrypt_inputs=["a", "b", "c"],
    expected_ops=["gemm"],
)


# =============================================================================
# Composite
# =============================================================================

COMPOSITE_TENSOR_OP = ModelSpec(
    name="composite_tensor_op",
    model_class=CompositeTensorOp,
    example_inputs=(torch.randn(1, 4, dtype=torch.float32), torch.randn(1, 4, dtype=torch.float32)),
    encrypt_inputs=["x", "y"],
    expected_ops=["add", "mul", "relu"],
)


# =============================================================================
# All Specs
# =============================================================================

ALL_TENSOR_OPS_SPECS = [
    ADD_TENSOR_OP,
    SUB_TENSOR_OP,
    MUL_TENSOR_OP,
    DIV_TENSOR_OP,
    MATMUL_TENSOR_OP,
    CONCAT_TENSOR_OP,
    RELU_TENSOR_OP,
    SOFTMAX_TENSOR_OP,
    MAX_POOL_TENSOR_OP,
    AVG_POOL_TENSOR_OP,
    GLOBAL_AVG_POOL_TENSOR_OP,
    FLATTEN_TENSOR_OP,
    SQRT_TENSOR_OP,
    SILU_TENSOR_OP,
    CONV_TENSOR_OP,
    GEMM_TENSOR_OP,
    COMPOSITE_TENSOR_OP,
]