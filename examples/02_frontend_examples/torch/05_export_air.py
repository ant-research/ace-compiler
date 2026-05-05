#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend: Export to AIR (Decorator)

Use @fhe.export decorator to export frontend IR to AIR format (.B file).
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.export(frontend="torch", format="air", output_path="/tmp/exported_model_decorator.B")
class LinearModel(nn.Module):
    """Simple linear model."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 4)

    def forward(self, x):
        return self.linear(x)


if __name__ == "__main__":
    x = torch.randn(1, 4)

    # Export the model using decorator
    result = LinearModel.export([x])
    print(f"Exported to: {result}")

