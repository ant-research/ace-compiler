#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Logistic Regression model for FHE classification.

The model is a single linear layer (nn.Linear) with no hidden activation —
the canonical "smallest" FHE model. Inputs may be flat vectors; the model
exposes a forward that flattens leading dimensions like LinearOp.

Note: preprocessing (e.g. normalization) is the caller's responsibility.
"""

import torch
import torch.nn as nn


class LogisticRegression(nn.Module):
    """Single-layer linear classifier (logistic regression)."""

    def __init__(self, input_size: int, num_classes: int, bias: bool = True):
        super().__init__()
        self.input_size = input_size
        self.num_classes = num_classes
        self.linear = nn.Linear(input_size, num_classes, bias=bias)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # Flatten any leading dims (batches / spatial) to a vector.
        x = torch.flatten(x, start_dim=1)
        return self.linear(x)