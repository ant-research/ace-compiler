#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Basic arithmetic and activation operations.
"""
import torch
import torch.nn as nn
import torch.nn.functional as F


# ============================================================================
# Binary Operators
# ============================================================================

class AddOp(nn.Module):
    """Element-wise addition."""
    def forward(self, x, y):
        return x + y


class SubOp(nn.Module):
    """Element-wise subtraction."""
    def forward(self, x, y):
        return x - y


class MulOp(nn.Module):
    """Element-wise multiplication."""
    def forward(self, x, y):
        return x * y


class DivOp(nn.Module):
    """Element-wise division."""
    def forward(self, x, y):
        return x / (y + 1e-8)


class MatMulOp(nn.Module):
    """Matrix multiplication."""
    def forward(self, x, y):
        return torch.matmul(x, y)


class ConcatOp(nn.Module):
    """Concatenation along dimension."""
    def __init__(self, dim=0):
        super().__init__()
        self.dim = dim

    def forward(self, *inputs):
        return torch.cat(inputs, dim=self.dim)


# ============================================================================
# Unary Operators
# ============================================================================

class ReluOp(nn.Module):
    """ReLU activation."""
    def forward(self, x):
        return F.relu(x)


class SigmoidOp(nn.Module):
    """Sigmoid activation."""
    def forward(self, x):
        return torch.sigmoid(x)


class TanhOp(nn.Module):
    """Tanh activation."""
    def forward(self, x):
        return torch.tanh(x)


class SoftmaxOp(nn.Module):
    """Softmax activation."""
    def __init__(self, dim=-1):
        super().__init__()
        self.dim = dim

    def forward(self, x):
        return F.softmax(x, dim=self.dim)


class SqrtOp(nn.Module):
    """Square root."""
    def forward(self, x):
        return torch.sqrt(torch.abs(x))


class SiluOp(nn.Module):
    """SiLU activation (Swish)."""
    def forward(self, x):
        return x * torch.sigmoid(x)


class FlattenOp(nn.Module):
    """Flatten input."""
    def forward(self, x):
        return torch.flatten(x, 1)


# ============================================================================
# Pooling
# ============================================================================

class MaxPool2dOp(nn.Module):
    """Max pooling."""
    def __init__(self, kernel_size=2, stride=2):
        super().__init__()
        self.pool = nn.MaxPool2d(kernel_size, stride)

    def forward(self, x):
        return self.pool(x)


class AvgPool2dOp(nn.Module):
    """Average pooling."""
    def __init__(self, kernel_size=2, stride=2, count_include_pad=False):
        super().__init__()
        self.pool = nn.AvgPool2d(kernel_size, stride, count_include_pad=count_include_pad)

    def forward(self, x):
        return self.pool(x)


class GlobalAvgPool2dOp(nn.Module):
    """Global average pooling."""
    def forward(self, x):
        return F.adaptive_avg_pool2d(x, 1)


class GlobalMaxPool2dOp(nn.Module):
    """Global max pooling."""
    def forward(self, x):
        return F.adaptive_max_pool2d(x, 1)