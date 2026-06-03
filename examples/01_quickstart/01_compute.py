#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Quickstart: FHE Compute — compile and run in one step

Two ways to use fhe.compute:
  1. Decorator:  @fhe.compute(...)  — simplest, recommended
  2. API call:   fhe.compute(...)(func)  — dynamic selection at runtime
"""

import torch
from ace import fhe


# ── Method 1: Decorator ──────────────────────────────────────────────

@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
def add(x, y):
    """Add two tensors — runs in FHE transparently."""
    return x + y


x = torch.ones(1, 4)
y = torch.ones(1, 4) * 2

print("=== @fhe.compute decorator ===")
result = add(x, y)
print(f"  Input X: {x.tolist()}")
print(f"  Input Y: {y.tolist()}")
print(f"  Result:  {result.tolist()}")
print()


# ── Method 2: API call ───────────────────────────────────────────────

def add_three(x, y, z):
    return x + y + z


print("=== fhe.compute(...)(func) API ===")
compiled = fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)(add_three)
z = torch.ones(1, 4) * 3
result = compiled(x, y, z)
print(f"  Input X: {x.tolist()}")
print(f"  Input Y: {y.tolist()}")
print(f"  Input Z: {z.tolist()}")
print(f"  Result:  {result.tolist()}")