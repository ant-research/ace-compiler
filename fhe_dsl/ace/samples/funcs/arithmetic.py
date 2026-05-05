#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Arithmetic operation examples.

These are basic element-wise operations that can be used as building blocks
for more complex models.
"""
import torch
import torch.nn.functional as F


# ============================================================================
# Function Examples
# ============================================================================

def add_func(x, y):
    """Element-wise addition."""
    return x + y


def sub_func(x, y):
    """Element-wise subtraction."""
    return x - y


def mul_func(x, y):
    """Element-wise multiplication."""
    return x * y


def div_func(x, y):
    """Element-wise division."""
    return x / (y + 1e-8)


def abs_func(x):
    """Absolute value."""
    return torch.abs(x)


def neg_func(x):
    """Negation."""
    return -x


def square_func(x):
    """Square operation."""
    return x * x


def sqrt_func(x):
    """Square root."""
    return torch.sqrt(torch.abs(x))


def clamp_func(x):
    """Clamp operation."""
    return torch.clamp(x, 0.0, 1.0)


def log_func(x):
    """Logarithm."""
    return torch.log(torch.abs(x) + 1e-8)


def exp_func(x):
    """Exponential."""
    return torch.exp(x)


# ============================================================================
# Activation Functions
# ============================================================================

def relu_func(x):
    """ReLU activation."""
    return torch.relu(x)


def sigmoid_func(x):
    """Sigmoid activation."""
    return torch.sigmoid(x)


def tanh_func(x):
    """Tanh activation."""
    return torch.tanh(x)


def softmax_func(x):
    """Softmax operation."""
    return F.softmax(x, dim=-1)