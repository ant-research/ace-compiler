#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Reusable PyTorch models for FHE examples.
"""

import torch
import torch.nn as nn


class LinearModel(nn.Module):
    """Simple linear model: y = Wx + b."""

    def __init__(self, input_size: int = 4, output_size: int = 3):
        super().__init__()
        self.linear = nn.Linear(input_size, output_size)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return self.linear(x)


class AddModel(nn.Module):
    """Model that adds two inputs: output = x + y."""

    def __init__(self):
        super().__init__()

    def forward(self, x: torch.Tensor, y: torch.Tensor) -> torch.Tensor:
        return x + y


class ReluModel(nn.Module):
    """Model with ReLU activation: output = ReLU(x)."""

    def __init__(self):
        super().__init__()
        self.relu = nn.ReLU()

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return self.relu(x)


class ConvModel(nn.Module):
    """Simple Conv2D model for image-like inputs."""

    def __init__(self, in_channels: int = 3, out_channels: int = 16, kernel_size: int = 3):
        super().__init__()
        padding = kernel_size // 2
        self.conv = nn.Conv2d(in_channels, out_channels, kernel_size, padding=padding)
        self.relu = nn.ReLU()

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return self.relu(self.conv(x))


class MLPClassifier(nn.Module):
    """Multi-layer perceptron for classification.

    Architecture: Linear -> ReLU -> Linear -> ReLU -> Linear
    """

    def __init__(self, input_size: int = 784, hidden_size: int = 128, num_classes: int = 10):
        super().__init__()
        self.network = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            nn.ReLU(),
            nn.Linear(hidden_size, hidden_size // 2),
            nn.ReLU(),
            nn.Linear(hidden_size // 2, num_classes),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # Flatten input if needed
        if x.dim() > 2:
            x = x.view(x.size(0), -1)
        return self.network(x)