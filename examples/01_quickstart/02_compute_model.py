#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Quickstart Example 2: FHE Compute Model

One-line FHE inference using @compute decorator.

This demonstrates the compute pattern (integration):
- @compute compiles and runs in one step
- Simplest usage for quick experimentation
- validate=True automatically validates the result
"""

import torch
import torch.nn as nn
from ace import fhe


# Step 1: Define model with @compute decorator
@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
class LinearModel(nn.Module):
    """Simple linear model: y = Wx + b."""

    def __init__(self, input_size=4, output_size=3):
        super().__init__()
        self.linear = nn.Linear(input_size, output_size)

    def forward(self, x):
        return self.linear(x)


# Step 2: Prepare inputs
x = torch.randn(1, 4)

# Step 3: Call model - runs in FHE transparently!
print("Running FHE inference with @compute decorator...")
fhe_result = LinearModel(x)

print(f"Input shape: {x.shape}")
print(f"FHE Result shape: {fhe_result.shape}")

