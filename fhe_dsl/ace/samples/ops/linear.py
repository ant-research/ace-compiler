#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Linear/GEMM (General Matrix Multiply) layers.
"""
import torch
import torch.nn as nn
import torch.nn.functional as F


# ============================================================================
# GEMM Ops
# ============================================================================

class LinearOp(nn.Module):
    """Basic linear (GEMM) layer."""
    def __init__(self, input_size, num_classes):
        super().__init__()
        self.linear = nn.Linear(input_size, num_classes)

    def forward(self, x):
        x = torch.flatten(x, 1)
        return self.linear(x)


class LinearReluOp(nn.Module):
    """Linear layer followed by ReLU."""
    def __init__(self, input_size, num_classes):
        super().__init__()
        self.linear = nn.Linear(input_size, num_classes)

    def forward(self, x):
        return F.relu(self.linear(x))


class ReluLinearOp(nn.Module):
    """ReLU followed by Linear layer."""
    def __init__(self, input_size, num_classes):
        super().__init__()
        self.linear = nn.Linear(input_size, num_classes)

    def forward(self, x):
        return self.linear(F.relu(x))


class LinearBiasOp(nn.Module):
    """Linear layer with bias (default)."""
    def __init__(self, input_size, num_classes):
        super().__init__()
        self.linear = nn.Linear(input_size, num_classes, bias=True)

    def forward(self, x):
        return self.linear(x)


class MLP(nn.Module):
    """Multi-layer perceptron with hidden layers."""
    def __init__(self, input_size, hidden_size, num_classes):
        super().__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, num_classes)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = self.fc2(x)
        return x


# ============================================================================
# Simple Ops
# ============================================================================

class FlattenOp(nn.Module):
    """Op that flattens input."""
    def forward(self, x):
        return torch.flatten(x, 1)


# ============================================================================
# Weight Initialization Helpers
# ============================================================================

def init_linear_fixed(model: nn.Module, weight_values=None, bias_value=1.0):
    """Initialize Linear layer with fixed weights."""
    if weight_values is None:
        weight_values = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0]
    weight_tensor = torch.Tensor(weight_values).reshape(2, 6)
    model.linear.weight.data = weight_tensor
    model.linear.bias.data.fill_(bias_value)


def init_linear_small(model: nn.Module):
    """Initialize small Linear layer with fixed weights."""
    weight_values = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0]
    weight_tensor = torch.Tensor(weight_values).reshape(2, 3)
    model.linear.weight.data = weight_tensor
    model.linear.bias.data.fill_(1.0)