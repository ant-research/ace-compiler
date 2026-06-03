#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: Partial Input Encryption

Use encrypt_inputs to selectively encrypt only some inputs,
leaving others in plaintext for better performance.

Note: Partial encryption (encrypting a subset of inputs) is not yet
implemented. Currently all inputs must be encrypted. This example shows
the intended API for when the feature is available.
"""

import torch
import torch.nn as nn
from ace import fhe


# ── Example 1: Encrypt all inputs (default) ──────────────────────────

@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add_default(x, y):
    """Both x and y are encrypted (default behavior)."""
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

print("=== Default: all inputs encrypted ===")
program = add_default.compile([x, y])
result = program(x, y)
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()


# ── Example 2: Explicit encrypt_inputs (all inputs) ──────────────────

@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
    encrypt_inputs=["x", "y"],  # Explicitly specify all encrypted inputs
)
def add_explicit(x, y):
    """Both inputs explicitly marked for encryption."""
    return x + y


print("=== Explicit: encrypt_inputs=['x', 'y'] ===")
program = add_explicit.compile([x, y])
result = program(x, y)
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()


# ── Example 3: Partial encryption (not yet implemented) ──────────────
# When partial encryption is supported, you will be able to do:
#
# @fhe.compile(
#     frontend="torch",
#     library="antlib",
#     device="cpu",
#     encrypt_inputs=["x"],  # Only x is encrypted; y stays plaintext
# )
# def add_partial(x, y):
#     return x + y