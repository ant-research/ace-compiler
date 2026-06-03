#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Convolutional neural network layers.
"""
import torch
import torch.nn as nn
import torch.nn.functional as F


# ============================================================================
# Convolutional Ops
# ============================================================================

class Conv2dOp(nn.Module):
    """Simple 2D convolution layer."""
    def __init__(self, in_channels=3, out_channels=3, kernel_size=3):
        super().__init__()
        self.conv = nn.Conv2d(in_channels, out_channels, kernel_size)

    def forward(self, x):
        return self.conv(x)


class Conv2dReluOp(nn.Module):
    """Conv2d followed by ReLU."""
    def __init__(self, in_channels=3, out_channels=3, kernel_size=3):
        super().__init__()
        self.conv = nn.Conv2d(in_channels, out_channels, kernel_size)

    def forward(self, x):
        return F.relu(self.conv(x))


class Conv2dBnReluOp(nn.Module):
    """Conv2d, BatchNorm, then ReLU."""
    def __init__(self, in_channels=3, out_channels=3, kernel_size=3):
        super().__init__()
        self.conv = nn.Conv2d(in_channels, out_channels, kernel_size)
        self.bn = nn.BatchNorm2d(out_channels)

    def forward(self, x):
        return F.relu(self.bn(self.conv(x)))


class DepthwiseConv2dOp(nn.Module):
    """Depthwise convolution layer."""
    def __init__(self, channels=3, kernel_size=3):
        super().__init__()
        self.conv = nn.Conv2d(channels, channels, kernel_size, groups=channels)

    def forward(self, x):
        return self.conv(x)


class SeparableConv2dOp(nn.Module):
    """Separable convolution layer (depthwise + pointwise)."""
    def __init__(self, in_channels=3, out_channels=3, kernel_size=3):
        super().__init__()
        self.depthwise = nn.Conv2d(in_channels, in_channels, kernel_size, groups=in_channels)
        self.pointwise = nn.Conv2d(in_channels, out_channels, 1)

    def forward(self, x):
        x = self.depthwise(x)
        x = self.pointwise(x)
        return x


class ConvTranspose2dOp(nn.Module):
    """Transposed convolution layer (deconvolution)."""
    def __init__(self, in_channels=3, out_channels=3, kernel_size=3):
        super().__init__()
        self.conv_transpose = nn.ConvTranspose2d(in_channels, out_channels, kernel_size)

    def forward(self, x):
        return self.conv_transpose(x)