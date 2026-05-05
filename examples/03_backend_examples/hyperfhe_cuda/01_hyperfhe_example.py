#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
HyperFHE CUDA Backend Example 1: GPU FHE Compilation

Example of using HyperFHE GPU library for FHE compilation.
Requires CUDA-capable GPU and HyperFHE library.
"""

import torch
from ace import fhe
from ace.fhe.util import gpu_available


@fhe.compile(frontend="torch", library="hyperfhe", device="cuda")
def add(x, y):
    """Add two tensors using HyperFHE GPU library."""
    return x + y


if __name__ == "__main__":
    # Check GPU availability
    if not gpu_available():
        print("WARNING: GPU not available. This example requires CUDA.")
        exit(1)

    # Prepare inputs
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)
    inputs = [x, y]

    # Compile with HyperFHE GPU library
    print("Compiling with HyperFHE GPU library...")
    prog = add.compile(inputs)

    # Run
    runner = fhe.FHERuntime(prog)
    result = runner.inference(x, y)

    # Validate
    expected = x + y
    print(f"Input X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result : {result.tolist()}")
    print(f"Expected: {expected.tolist()}")
    print(f"Validation: {'PASSED' if runner.validate() else 'FAILED'}")