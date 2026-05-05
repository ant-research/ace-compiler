#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Quickstart Example 3: FHE Compile Function

Compile a function separately, then run inference.

This demonstrates the compile + compute pattern (separation):
1. Compile the function with example inputs
2. Run inference with the compiled program
3. Validate result using program.validate()
"""

import torch
from ace import fhe


# Step 1: Define function with @fhe.compile decorator
@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors."""
    return x + y


# Step 2: Prepare example inputs
input_x = torch.ones(1, 4)
input_y = torch.ones(1, 4) * 2

# Step 3: Compile the function (separation - compile step)
print("Compiling add function...")
program = add.compile([input_x, input_y])

# Step 4: Run FHE inference (separation - compute step)
print("Running FHE inference...")
fhe_result = program(input_x, input_y)

print(f"Input X: {input_x.tolist()}")
print(f"Input Y: {input_y.tolist()}")
print(f"FHE Result: {fhe_result.tolist()}")

# Step 5: Validate using program.validate()
is_valid = program.validate()
print(f"Validation: {'PASSED' if is_valid else 'FAILED'}")

