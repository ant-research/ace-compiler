#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend: Model with fhe.compile API

Use fhe.compile function directly (without decorator) to compile a PyTorch nn.Module.
"""

import torch
import torch.nn as nn
from ace import fhe


class LinearModel(nn.Module):
    """Simple linear model."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 4)

    def forward(self, x):
        return self.linear(x)


if __name__ == "__main__":
    # Use fhe.compile as a function (not decorator)
    compiled_model = fhe.compile(frontend="torch", library="antlib", device="cpu")(LinearModel)

    x = torch.randn(1, 4)

    program = compiled_model.fhe_compile([x])
    result = program(x)

    print(f"Input shape: {x.shape}")
    print(f"Output shape: {result.shape}")
    print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")

