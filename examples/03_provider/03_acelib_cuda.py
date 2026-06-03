#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Provider: Acelib — CUDA GPU Backend

GPU-accelerated FHE using the Acelib CUDA provider.
Requires CUDA-capable GPU.
Acelib supports provider-specific parameters:
  ckks: sbm, hw, q0, sf, N, icl, mcl
  p2c:  fp
"""

import torch
from ace import fhe
from ace.fhe.util import gpu_available


@fhe.compile(
    frontend="torch",
    library="acelib",
    device="cuda",
    ckks={"sbm": True, "hw": 192, "q0": 56, "sf": 51, "N": 65536, "icl": 17, "mcl": 34},
    p2c={"fp": True},
)
def add(x, y):
    """Add two tensors using Acelib GPU provider."""
    return x + y


if __name__ == "__main__":
    if not gpu_available():
        print("WARNING: GPU not available. This example requires CUDA.")
        exit(1)

    x = torch.randn(1, 4)
    y = torch.randn(1, 4)

    print("=== Acelib CUDA: basic usage ===")
    program = add.compile([x, y])
    result = program(x, y)
    expected = x + y
    print(f"  Result: {result.tolist()}")
    print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")