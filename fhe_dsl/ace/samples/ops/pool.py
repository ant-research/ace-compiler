#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Pooling operation examples.
"""
import torch
import torch.nn as nn
import torch.nn.functional as F


# ============================================================================
# Pooling Ops
# ============================================================================

class AvgPool2dOp(nn.Module):
    """Average pooling."""
    def __init__(self, kernel_size=2, stride=2, count_include_pad=False):
        super().__init__()
        self.pool = nn.AvgPool2d(kernel_size, stride, count_include_pad=count_include_pad)

    def forward(self, x):
        return self.pool(x)


class MaxPool2dOp(nn.Module):
    """Max pooling."""
    def __init__(self, kernel_size=2, stride=2):
        super().__init__()
        self.pool = nn.MaxPool2d(kernel_size, stride)

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


class AvgPoolConv2dOp(nn.Module):
    """AvgPool followed by Conv2d."""
    def __init__(self):
        super().__init__()
        self.conv = nn.Conv2d(3, 3, 3)
        self.pool = nn.AvgPool2d(2, 2, count_include_pad=False)

    def forward(self, x):
        return self.conv(self.pool(x))


class Conv2dAvgPool2dOp(nn.Module):
    """Conv2d followed by AvgPool."""
    def __init__(self):
        super().__init__()
        self.conv = nn.Conv2d(3, 3, 3)
        self.pool = nn.AvgPool2d(2, 2, count_include_pad=False)

    def forward(self, x):
        return self.pool(self.conv(x))


class ReluAvgPoolOp(nn.Module):
    """ReLU followed by Average pooling."""
    def __init__(self):
        super().__init__()
        self.pool = nn.AvgPool2d(2, 2, count_include_pad=False)

    def forward(self, x):
        return self.pool(F.relu(x))


class AvgPoolFlattenOp(nn.Module):
    """Average pooling followed by flatten."""
    def __init__(self):
        super().__init__()
        self.pool = nn.AvgPool2d(2, 2, count_include_pad=False)

    def forward(self, x):
        pool_output = self.pool(x)
        return torch.flatten(pool_output, 1)