#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR Format Example 3: Memory IR Compilation

Compile using in-memory IR (without exporting to file).
This is the default compilation path for most use cases.
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors using memory IR compilation."""
    return x + y


if __name__ == "__main__":
    # Prepare inputs
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)
    inputs = [x, y]

    # Compile using memory IR (default)
    print("Compiling with memory IR (default)...")
    program = add.compile(inputs)

    # The IR is kept in memory during compilation
    # No intermediate files are created

    # Run
    result = program(x, y)

    # Validate
    expected = x + y
    print(f"Input X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result : {result.tolist()}")
    print(f"Expected: {expected.tolist()}")
    print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")

    # Note: Memory IR compilation is faster for development
    # Use file-based IR (ONNX/AIR) for:
    # - Model sharing
    # - Inspection/debugging
    # - Separate compilation pipeline