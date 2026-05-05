#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced Example 1: Custom Encryption Parameters

Configure custom FHE encryption parameters for specific use cases.
"""

import torch
from ace import fhe


@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
    # Custom encryption parameters
    # Note: Parameters must be valid for the library. Using default for demo.
)
def add(x, y):
    """Add two tensors with default encryption parameters."""
    return x + y


if __name__ == "__main__":
    # Prepare inputs
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)
    inputs = [x, y]

    # Compile with default parameters
    print("Compiling with default encryption parameters...")
    print("  - Using library default CKKS parameters")
    print("  - For custom parameters, see documentation")

    program = add.compile(inputs)

    # Run
    result = program(x, y)

    # Validate
    expected = x + y
    print(f"\nInput X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result : {result.tolist()}")
    print(f"Expected: {expected.tolist()}")
    print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")

    # Parameter tuning tips:
    # - Larger N: More slots, higher precision, slower
    # - Higher level: More multiplications supported, larger ciphertext
    # - Larger scale: Higher precision, more noise growth