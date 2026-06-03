#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: Batch and Dataset Inference

Run FHE inference on multiple inputs using FHERuntime.
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    return x + y


if __name__ == "__main__":
    # Compile once
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)
    program = add.compile([x, y])
    runner = fhe.FHERuntime(program)

    # ── Single inference ──────────────────────────────────────────────
    print("=== Single inference ===")
    result = runner.inference(x, y)
    expected = x + y
    print(f"  Validate: {'PASSED' if runner.validate(result, expected) else 'FAILED'}")
    print()

    # ── Batch inference ───────────────────────────────────────────────
    print("=== Batch inference (5 samples) ===")
    for i in range(5):
        xi = torch.randn(1, 4)
        yi = torch.randn(1, 4)
        result = runner.inference(xi, yi)
        expected = xi + yi
        valid = runner.validate(result, expected)
        print(f"  Sample {i+1}: {'PASSED' if valid else 'FAILED'}")