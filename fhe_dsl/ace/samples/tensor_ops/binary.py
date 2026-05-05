#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Binary Custom C++ Operators.

Models using binary custom C++ operators (torch.ops.tensor.*).
These operators take two input tensors and produce one output tensor.
"""

import torch
import torch.nn as nn


class AddTensorOp(nn.Module):
    """Model using custom C++ add operator."""

    def forward(self, x, y):
        return torch.ops.tensor.add(x, y)


class SubTensorOp(nn.Module):
    """Model using custom C++ sub operator."""

    def forward(self, x, y):
        return torch.ops.tensor.sub(x, y)


class MulTensorOp(nn.Module):
    """Model using custom C++ mul operator."""

    def forward(self, x, y):
        return torch.ops.tensor.mul(x, y)


class DivTensorOp(nn.Module):
    """Model using custom C++ div operator."""

    def forward(self, x, y):
        return torch.ops.tensor.div(x, y)


class MatmulTensorOp(nn.Module):
    """Model using custom C++ matmul operator."""

    def forward(self, x, y):
        return torch.ops.tensor.matmul(x, y)


class ConcatTensorOp(nn.Module):
    """Model using custom C++ concat operator."""

    def forward(self, x, y):
        return torch.ops.tensor.concat(x, y)