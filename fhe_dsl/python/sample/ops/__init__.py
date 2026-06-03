#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Module examples - basic neural network modules.
"""
from .arithmetic import (
    # Binary operators
    AddOp,
    SubOp,
    MulOp,
    DivOp,
    MatMulOp,
    ConcatOp,
    # Unary operators
    ReluOp,
    SigmoidOp,
    TanhOp,
    SoftmaxOp,
    SqrtOp,
    SiluOp,
    # Constant operators
    AddConstOp,
    MulConstOp,
)

from .linear import (
    LinearOp,
    LinearReluOp,
    ReluLinearOp,
    LinearBiasOp,
    MLP,
    FlattenOp,
    init_linear_fixed,
    init_linear_small,
)

from .conv import (
    Conv2dOp,
    Conv2dReluOp,
    Conv2dBnReluOp,
    DepthwiseConv2dOp,
    SeparableConv2dOp,
    ConvTranspose2dOp,
)

from .pool import (
    AvgPool2dOp,
    MaxPool2dOp,
    GlobalAvgPool2dOp,
    GlobalMaxPool2dOp,
    AvgPoolConv2dOp,
    Conv2dAvgPool2dOp,
    ReluAvgPoolOp,
    AvgPoolFlattenOp,
)

from . import specs

__all__ = [
    # Arithmetic operators
    "AddOp",
    "SubOp",
    "MulOp",
    "DivOp",
    "MatMulOp",
    "ConcatOp",
    "ReluOp",
    "SigmoidOp",
    "TanhOp",
    "SoftmaxOp",
    "SqrtOp",
    "SiluOp",
    "AddConstOp",
    "MulConstOp",
    # Linear modules
    "LinearOp",
    "LinearReluOp",
    "ReluLinearOp",
    "LinearBiasOp",
    "MLP",
    "FlattenOp",
    "init_linear_fixed",
    "init_linear_small",
    # Conv modules
    "Conv2dOp",
    "Conv2dReluOp",
    "Conv2dBnReluOp",
    "DepthwiseConv2dOp",
    "SeparableConv2dOp",
    "ConvTranspose2dOp",
    # Pool modules
    "AvgPool2dOp",
    "MaxPool2dOp",
    "GlobalAvgPool2dOp",
    "GlobalMaxPool2dOp",
    "AvgPoolConv2dOp",
    "Conv2dAvgPool2dOp",
    "ReluAvgPoolOp",
    "AvgPoolFlattenOp",
]