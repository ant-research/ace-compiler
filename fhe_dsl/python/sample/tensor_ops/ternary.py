#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Ternary Custom C++ Operators.

Models using ternary custom C++ operators (torch.ops.tensor.*).
These operators take three input tensors and produce one output tensor.
"""

import torch
import torch.nn as nn


class ConvTensorOp(nn.Module):
    """Model using custom C++ conv operator."""

    def forward(self, x, w, b):
        return torch.ops.tensor.conv(x, w, b)


class GemmTensorOp(nn.Module):
    """Model using custom C++ gemm operator."""

    def forward(self, a, b, c):
        return torch.ops.tensor.gemm(a, b, c)