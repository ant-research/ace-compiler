#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: Custom Encryption Parameters

Configure CKKS encryption parameters for FHE compilation.
Higher N = more security but slower; q0 and sf control precision.
"""

import torch
from ace import fhe


@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
    ckks={"N": 16, "q0": 60, "sf": 56},
)
def add(x, y):
    """Add two tensors with custom CKKS parameters."""
    return x + y


if __name__ == "__main__":
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)

    print("=== Custom CKKS parameters ===")
    print("  N=16, q0=60, sf=56")
    print()

    program = add.compile([x, y])
    result = program(x, y)
    expected = x + y

    print(f"  Result: {result.tolist()}")
    print(f"  Expected: {expected.tolist()}")
    print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")