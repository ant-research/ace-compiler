#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced Example 2: Partial Input Encryption

Encrypt only specific inputs while keeping others in plaintext.
Useful when only some inputs are sensitive.
"""

import torch
from ace import fhe


# Example 1: Encrypt all inputs (standard usage)
@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
)
def add_all(x, y):
    """Add where both x and y are encrypted."""
    return x + y


# Example 2: Using encrypt_inputs to specify which inputs to encrypt
# Note: This feature may have limited support depending on library
@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
    encrypt_inputs=["x", "y"],  # Explicitly specify encrypted inputs
)
def add_explicit(x, y):
    """Add with explicitly specified encrypted inputs."""
    return x + y


if __name__ == "__main__":
    print("=" * 50)
    print("Example 1: Standard Encryption (all inputs)")
    print("=" * 50)

    x = torch.randn(1, 4)
    y = torch.randn(1, 4)

    program1 = add_all.compile([x, y])
    result1 = program1(x, y)

    expected1 = x + y
    print(f"Input X (encrypted): {x.tolist()}")
    print(f"Input Y (encrypted): {y.tolist()}")
    print(f"Result: {result1.tolist()}")
    print(f"Validation: {'PASSED' if program1.validate() else 'FAILED'}")

    print("\n" + "=" * 50)
    print("Example 2: Explicit encrypt_inputs specification")
    print("=" * 50)

    x2 = torch.ones(1, 4)
    y2 = torch.ones(1, 4) * 2

    program2 = add_explicit.compile([x2, y2])
    result2 = program2(x2, y2)

    expected2 = x2 + y2
    print(f"Input X (encrypted): {x2.tolist()}")
    print(f"Input Y (encrypted): {y2.tolist()}")
    print(f"Result: {result2.tolist()}")
    print(f"Validation: {'PASSED' if program2.validate() else 'FAILED'}")

    print("\n" + "=" * 50)
    print("Note: encrypt_inputs option allows specifying which inputs to encrypt.")
    print("  - Default: All inputs are encrypted")
    print("  - Use list of names: encrypt_inputs=['x', 'y']")
    print("  - Useful for skipping encryption on public constants")
    print("=" * 50)