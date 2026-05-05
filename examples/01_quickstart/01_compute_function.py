#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Quickstart Example 1: FHE Compute Function

One-line FHE inference using @compute decorator.

This demonstrates the compute pattern (integration):
- @compute compiles and runs in one step
- Simplest usage for quick experimentation
- validate=True automatically validates the result
"""

import torch
from ace import fhe


# Step 1: Define function with @compute decorator
# This compiles AND runs FHE inference in one call
# validate=True will automatically validate the result
@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
def add(x, y):
    """Add two tensors - runs in FHE automatically."""
    return x + y


# Step 2: Prepare inputs
input_x = torch.ones(1, 4)
input_y = torch.ones(1, 4) * 2

# Step 3: Call function - runs in FHE transparently!
print("Running FHE inference with @compute decorator...")
fhe_result = add(input_x, input_y)

print(f"Input X: {input_x.tolist()}")
print(f"Input Y: {input_y.tolist()}")
print(f"FHE Result: {fhe_result.tolist()}")
print(f"Expected: {(input_x + input_y).tolist()}")

