#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Composite Custom C++ Operator Models.

Models with multiple custom C++ operators composed together.
These models test the FX tracing and IR generation for complex graphs.
"""

import torch
import torch.nn as nn


class CompositeTensorOp(nn.Module):
    """Model with multiple custom operators composed together."""

    def forward(self, x, y):
        a = torch.ops.tensor.add(x, y)
        b = torch.ops.tensor.mul(a, y)
        return torch.ops.tensor.relu(b)