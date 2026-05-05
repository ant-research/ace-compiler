#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AntLib CPU Backend Example 1: Basic Usage

Basic example of using AntLib CPU library for FHE compilation.
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors using AntLib CPU library."""
    return x + y


if __name__ == "__main__":
    # Prepare inputs
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)
    inputs = [x, y]

    # Compile with AntLib CPU library
    print("Compiling with AntLib CPU library...")
    program = add.compile(inputs)

    # Run
    result = program(x, y)

    # Validate
    expected = x + y
    print(f"Input X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result : {result.tolist()}")
    print(f"Expected: {expected.tolist()}")
    print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")