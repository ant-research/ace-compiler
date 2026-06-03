#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-layer perceptron examples.

Note: This module re-exports MLP from linear.py for convenience.
"""
from .linear import MLP, LinearOp, LinearReluOp, ReluLinearOp

__all__ = [
    "MLP",
    "LinearOp",
    "LinearReluOp",
    "ReluLinearOp",
]