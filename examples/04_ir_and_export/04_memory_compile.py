#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR & Export: In-memory IR compilation

By default, @fhe.compile uses in-memory IR (FHEProgram) without
writing to disk. This is the simplest and most common workflow.
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors — compiled entirely in memory."""
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

print("=== In-memory IR compilation (default) ===")
program = add.compile([x, y])
result = program(x, y)
expected = x + y
print(f"  Result: {result.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()
print("No files written to disk — IR stays in memory.")