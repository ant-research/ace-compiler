#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Provider: AntLib — CPU Backend

Basic usage and custom CKKS parameters for the antlib CPU provider.
"""

import torch
from ace import fhe


# ── Basic usage ──────────────────────────────────────────────────────

@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors using AntLib CPU provider."""
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

print("=== AntLib CPU: basic usage ===")
program = add.compile([x, y])
result = program(x, y)
expected = x + y
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()

# ── Custom CKKS parameters ──────────────────────────────────────────
# AntLib CKKS options: N (poly degree), q0 (first mod size), sf (scaling factor size)

@fhe.compile(frontend="torch", library="antlib", device="cpu",
             ckks={"N": 16, "q0": 60, "sf": 56})
def add_custom(x, y):
    return x + y


print("=== AntLib CPU: custom CKKS parameters ===")
program = add_custom.compile([x, y])
result = program(x, y)
expected = x + y
print(f"  CKKS: N=16, q0=60, sf=56")
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")