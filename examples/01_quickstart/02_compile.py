#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Quickstart: FHE Compile — compile first, then run inference

Two ways to use fhe.compile:
  1. Decorator:  @fhe.compile(...)  — recommended
  2. API call:   fhe.compile(...)(func)  — dynamic selection at runtime

CompiledProgram can be used in two ways:
  - High-level:  program(x, y)           + program.validate()
  - Low-level:   program.runtime()       + runner.inference() + runner.validate()
"""

import torch
from ace import fhe


# ── Method 1: Decorator ──────────────────────────────────────────────

@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors."""
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

# Compile
print("=== @fhe.compile decorator ===")
program = add.compile([x, y])

# High-level API: call program directly
result = program(x, y)
expected = x + y
print(f"  Result:  {result.tolist()}")
print(f"  Expected: {expected.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()

# Low-level API: use the runtime from the program
runner = program.runtime()
result = runner.inference(x, y)
print(f"  FHERuntime result: {result.tolist()}")
print(f"  FHERuntime validate: {'PASSED' if runner.validate(result, expected) else 'FAILED'}")
print()


# ── Method 2: API call ───────────────────────────────────────────────

def add_three(x, y, z):
    return x + y + z


print("=== fhe.compile(...)(func) API ===")
compiled = fhe.compile(frontend="torch", library="antlib", device="cpu")(add_three)
z = torch.randn(1, 4)
program2 = compiled.compile([x, y, z])
result = program2(x, y, z)
print(f"  Result:  {result.tolist()}")
print(f"  Validate: {'PASSED' if program2.validate() else 'FAILED'}")