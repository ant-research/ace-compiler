#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: FHERuntime Low-Level API

Use FHERuntime directly for fine-grained control over FHE inference.
This is the same API that @fhe.compile uses internally.
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    return x + y


if __name__ == "__main__":
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)

    # Compile
    program = add.compile([x, y])

    # ── High-level API: CompiledProgram ───────────────────────────────
    print("=== High-level: program(x, y) + program.validate() ===")
    result = program(x, y)
    print(f"  Result: {result.tolist()}")
    print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
    print()

    # ── Low-level API: FHERuntime ────────────────────────────────────
    print("=== Low-level: program.runtime() ===")
    runner = program.runtime()
    result = runner.inference(x, y)
    expected = x + y
    print(f"  Result: {result.tolist()}")
    print(f"  Validate: {'PASSED' if runner.validate(result, expected) else 'FAILED'}")