#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced Example 4: Direct Runtime API

Use FHERuntime directly for fine-grained control over execution.
"""

import torch
from ace import fhe
from ace.fhe.runtime import FHERuntime


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors."""
    return x + y


if __name__ == "__main__":
    # Prepare inputs
    x = torch.randn(1, 4)
    y = torch.randn(1, 4)
    inputs = [x, y]

    # Compile to get program package
    print("Compiling...")
    program = add.compile(inputs)

    # =========================================================================
    # Using CompiledProgram (high-level API)
    # =========================================================================
    print("\n" + "=" * 50)
    print("Using CompiledProgram (high-level API)")
    print("=" * 50)

    result_jit = program(x, y)
    print(f"CompiledProgram result: {result_jit.tolist()}")
    print(f"CompiledProgram validation: {'PASSED' if program.validate() else 'FAILED'}")

    # =========================================================================
    # Using FHERuntime directly (low-level API)
    # =========================================================================
    print("\n" + "=" * 50)
    print("Using FHERuntime (low-level API)")
    print("=" * 50)

    # Create runtime with explicit verify mode
    runtime = FHERuntime(program.package, verify="array")

    # Run inference
    result_rt = runtime.inference(x, y)
    print(f"FHERuntime result: {result_rt.tolist()}")

    # Validate
    is_valid = runtime.validate()
    print(f"FHERuntime validation: {'PASSED' if is_valid else 'FAILED'}")

    # =========================================================================
    # API Comparison
    # =========================================================================
    print("\n" + "=" * 50)
    print("API Comparison:")
    print("  CompiledProgram: High-level, simpler API")
    print("  FHERuntime: Low-level, more control")
    print("    - Explicit verify mode ('array' or 'tensor')")
    print("    - Direct access to encryption/decryption")
    print("    - Fine-grained execution control")
    print("=" * 50)