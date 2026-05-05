#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Control flow examples.

These examples demonstrate conditional execution and loops in PyTorch.
Note: Some control flow patterns may not be supported by FHE compilation.
"""
import torch


# ============================================================================
# Function Examples
# ============================================================================

def conditional_add_func(x, y):
    """Conditional addition based on tensor sum."""
    if x.sum() > 0:
        return x + y
    else:
        return x - y


def conditional_relu_func(x):
    """Conditional ReLU based on mean value."""
    if x.mean() > 0:
        return torch.relu(x)
    else:
        return -torch.relu(-x)


def loop_multiply_func(x):
    """Loop that multiplies by 2 three times."""
    for i in range(3):
        x = x * 2.0
    return x


def loop_add_func(x):
    """Loop that adds 1 five times."""
    for i in range(5):
        x = x + 1.0
    return x


def nested_loop_func(x):
    """Nested loop operation."""
    for i in range(2):
        for j in range(2):
            x = x + 1.0
    return x


def while_loop_func(x):
    """Simple while loop."""
    count = 0
    while count < 3:
        x = x * 2.0
        count += 1
    return x


def conditional_chain_func(x, y):
    """Multiple conditionals in sequence."""
    if x.sum() > 0:
        x = x + 1
    if y.sum() > 0:
        y = y + 1
    return x + y


def branch_execution_func(x, y):
    """Branch with different operations."""
    if x.sum() > y.sum():
        return x * y
    else:
        return x + y