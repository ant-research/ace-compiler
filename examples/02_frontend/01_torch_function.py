#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Frontend: Torch — Function

Compile a plain Python function with torch frontend.
Supports both decorator and API styles.
"""

import torch
from ace import fhe


# ── Decorator style ──────────────────────────────────────────────────

@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors."""
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

print("=== Decorator: @fhe.compile ===")
program = add.compile([x, y])
result = program(x, y)
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()


# ── API style ────────────────────────────────────────────────────────

def multiply(x, y):
    return x * y


print("=== API: fhe.compile(...)(func) ===")
compiled_mul = fhe.compile(frontend="torch", library="antlib", device="cpu")(multiply)
program = compiled_mul.compile([x, y])
result = program(x, y)
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")