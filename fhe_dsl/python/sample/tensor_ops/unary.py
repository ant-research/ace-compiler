#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unary Custom C++ Operators.

Models using unary custom C++ operators (torch.ops.tensor.*).
These operators take one input tensor and produce one output tensor.
"""

import torch
import torch.nn as nn


class ReLUTensorOp(nn.Module):
    """Model using custom C++ relu operator."""

    def forward(self, x):
        return torch.ops.tensor.relu(x)


class SoftmaxTensorOp(nn.Module):
    """Model using custom C++ softmax operator."""

    def forward(self, x):
        return torch.ops.tensor.softmax(x)


class MaxPoolTensorOp(nn.Module):
    """Model using custom C++ max_pool operator."""

    def forward(self, x):
        return torch.ops.tensor.max_pool(x)


class AvgPoolTensorOp(nn.Module):
    """Model using custom C++ average_pool operator."""

    def forward(self, x):
        return torch.ops.tensor.average_pool(x)


class GlobalAvgPoolTensorOp(nn.Module):
    """Model using custom C++ global_average_pool operator."""

    def forward(self, x):
        return torch.ops.tensor.global_average_pool(x)


class FlattenTensorOp(nn.Module):
    """Model using custom C++ flatten operator."""

    def forward(self, x):
        return torch.ops.tensor.flatten(x)


class SqrtTensorOp(nn.Module):
    """Model using custom C++ sqrt operator."""

    def forward(self, x):
        return torch.ops.tensor.sqrt(x)


class SiluTensorOp(nn.Module):
    """Model using custom C++ silu operator."""

    def forward(self, x):
        return torch.ops.tensor.silu(x)