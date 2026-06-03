#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Reusable Python functions for FHE examples.
"""

import torch


def add_func(x: torch.Tensor, y: torch.Tensor) -> torch.Tensor:
    """Add two tensors: output = x + y."""
    return x + y


def mul_func(x: torch.Tensor, y: torch.Tensor) -> torch.Tensor:
    """Multiply two tensors: output = x * y."""
    return x * y


def relu_func(x: torch.Tensor) -> torch.Tensor:
    """Apply ReLU activation: output = max(0, x)."""
    return torch.relu(x)


def linear_func(x: torch.Tensor, weight: torch.Tensor, bias: torch.Tensor) -> torch.Tensor:
    """Linear transformation: output = x @ weight.T + bias."""
    return torch.nn.functional.linear(x, weight, bias)