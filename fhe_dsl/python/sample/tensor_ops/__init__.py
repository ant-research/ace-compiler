#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Custom C++ Operator Models for ACE FHE Testing.

This package provides model definitions using custom C++ operators
(torch.ops.tensor.*). These models are used for testing FX tracing
and AIR IR generation in the torch frontend.

Structure:
    - binary.py: Binary operators (add, sub, mul, div, matmul, concat)
    - unary.py: Unary operators (relu, softmax, pool, flatten, sqrt, silu)
    - ternary.py: Ternary operators (conv, gemm)
    - composite.py: Composite models with multiple operators

Usage:
    # Import custom operator models
    from ace.sample.tensor_ops import AddTensorOp, ReLUTensorOp, ConvTensorOp

    # Use in tests
    model = AddTensorOp()
    output = model(x, y)
"""

from .binary import (
    AddTensorOp,
    SubTensorOp,
    MulTensorOp,
    DivTensorOp,
    MatmulTensorOp,
    ConcatTensorOp,
)

from .unary import (
    ReLUTensorOp,
    SoftmaxTensorOp,
    MaxPoolTensorOp,
    AvgPoolTensorOp,
    GlobalAvgPoolTensorOp,
    FlattenTensorOp,
    SqrtTensorOp,
    SiluTensorOp,
)

from .ternary import (
    ConvTensorOp,
    GemmTensorOp,
)

from .composite import (
    CompositeTensorOp,
)

from .specs import (
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
    ALL_TENSOR_OPS_SPECS,
)

__all__ = [
    # Binary operators
    "AddTensorOp",
    "SubTensorOp",
    "MulTensorOp",
    "DivTensorOp",
    "MatmulTensorOp",
    "ConcatTensorOp",
    # Unary operators
    "ReLUTensorOp",
    "SoftmaxTensorOp",
    "MaxPoolTensorOp",
    "AvgPoolTensorOp",
    "GlobalAvgPoolTensorOp",
    "FlattenTensorOp",
    "SqrtTensorOp",
    "SiluTensorOp",
    # Ternary operators
    "ConvTensorOp",
    "GemmTensorOp",
    # Composite models
    "CompositeTensorOp",
    # Specs
    "ADD_TENSOR_OP",
    "SUB_TENSOR_OP",
    "MUL_TENSOR_OP",
    "DIV_TENSOR_OP",
    "MATMUL_TENSOR_OP",
    "CONCAT_TENSOR_OP",
    "RELU_TENSOR_OP",
    "SOFTMAX_TENSOR_OP",
    "MAX_POOL_TENSOR_OP",
    "AVG_POOL_TENSOR_OP",
    "GLOBAL_AVG_POOL_TENSOR_OP",
    "FLATTEN_TENSOR_OP",
    "SQRT_TENSOR_OP",
    "SILU_TENSOR_OP",
    "CONV_TENSOR_OP",
    "GEMM_TENSOR_OP",
    "COMPOSITE_TENSOR_OP",
    "ALL_TENSOR_OPS_SPECS",
]