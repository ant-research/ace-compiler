#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Module examples - basic neural network modules.
"""

from . import specs
from .arithmetic import (
    # Constant operators
    AddConstOp,
    # Binary operators
    AddOp,
    ConcatOp,
    DivOp,
    MatMulOp,
    MulConstOp,
    MulOp,
    # Unary operators
    ReluOp,
    SigmoidOp,
    SiluOp,
    SoftmaxOp,
    SqrtOp,
    SubOp,
    TanhOp,
)
from .conv import (
    Conv2dBnReluOp,
    Conv2dOp,
    Conv2dReluOp,
    ConvTranspose2dOp,
    DepthwiseConv2dOp,
    SeparableConv2dOp,
)
from .linear import (
    MLP,
    FlattenOp,
    LinearBiasOp,
    LinearOp,
    LinearReluOp,
    ReluLinearOp,
    init_linear_fixed,
    init_linear_small,
)
from .pool import (
    AvgPool2dOp,
    AvgPoolConv2dOp,
    AvgPoolFlattenOp,
    Conv2dAvgPool2dOp,
    GlobalAvgPool2dOp,
    GlobalMaxPool2dOp,
    MaxPool2dOp,
    ReluAvgPoolOp,
)


__all__ = [
    "MLP",
    "AddConstOp",
    # Arithmetic operators
    "AddOp",
    # Pool modules
    "AvgPool2dOp",
    "AvgPoolConv2dOp",
    "AvgPoolFlattenOp",
    "ConcatOp",
    "Conv2dAvgPool2dOp",
    "Conv2dBnReluOp",
    # Conv modules
    "Conv2dOp",
    "Conv2dReluOp",
    "ConvTranspose2dOp",
    "DepthwiseConv2dOp",
    "DivOp",
    "FlattenOp",
    "GlobalAvgPool2dOp",
    "GlobalMaxPool2dOp",
    "LinearBiasOp",
    # Linear modules
    "LinearOp",
    "LinearReluOp",
    "MatMulOp",
    "MaxPool2dOp",
    "MulConstOp",
    "MulOp",
    "ReluAvgPoolOp",
    "ReluLinearOp",
    "ReluOp",
    "SeparableConv2dOp",
    "SigmoidOp",
    "SiluOp",
    "SoftmaxOp",
    "SqrtOp",
    "SubOp",
    "TanhOp",
    "init_linear_fixed",
    "init_linear_small",
]
