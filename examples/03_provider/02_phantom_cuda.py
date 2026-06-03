#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Provider: Phantom — CUDA GPU Backend

GPU-accelerated FHE using the Phantom CUDA provider.
Requires CUDA-capable GPU.
Phantom CKKS options: N, q0, sf, hw
"""

import torch
from ace import fhe
from ace.fhe.util import gpu_available


@fhe.compile(frontend="torch", library="phantom", device="cuda")
def add(x, y):
    """Add two tensors using Phantom GPU provider."""
    return x + y


if __name__ == "__main__":
    if not gpu_available():
        print("WARNING: GPU not available. This example requires CUDA.")
        exit(1)

    x = torch.randn(1, 4)
    y = torch.randn(1, 4)

    print("=== Phantom CUDA: basic usage ===")
    program = add.compile([x, y])
    result = program(x, y)
    expected = x + y
    print(f"  Result: {result.tolist()}")
    print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")