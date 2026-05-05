#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Quickstart Example 4: FHE Compile Model

Compile a PyTorch nn.Module using @compile decorator, then run inference.

This demonstrates the compile + compute pattern (separation):
1. @compile decorates the model class
2. Compile with example inputs
3. Run inference with the compiled program
4. Validate result using program.validate()
"""

import torch
import torch.nn as nn
from ace import fhe


# Step 1: Define model with @fhe.compile decorator
@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LinearModel(nn.Module):
    """Simple linear model: y = Wx + b."""

    def __init__(self, input_size=4, output_size=4):
        super().__init__()
        self.linear = nn.Linear(input_size, output_size)

    def forward(self, x):
        return self.linear(x)


# Step 2: Prepare example inputs
x = torch.randn(1, 4)
example_inputs = [x]

# Step 3: Compile (separation - compile step)
print("Compiling linear model...")
program = LinearModel.compile(example_inputs)

# Step 4: Run FHE inference (separation - compute step)
print("Running FHE inference...")
fhe_result = program(x)

print(f"Input: {x.tolist()}")
print(f"FHE Result: {fhe_result.tolist()}")

# Step 5: Validate result
is_valid = program.validate()
print(f"Validation: {'PASSED' if is_valid else 'FAILED'}")

