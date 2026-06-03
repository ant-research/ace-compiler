#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: Compile Cache Control

The FHE compiler uses a 3-level cache to avoid recompilation:

  Level 1: <entity>-<frontend>-<library>-<device>
           e.g. Add-torch-antlib-cpu

  Level 2: <input_shape_and_dtype>
           e.g. shape_1x4_dtype_torch_float32

  Level 3: <compile_options_hash>
           SHA-256 hash of ckks/vec/sihe/p2c options

Cache directory: /tmp/ace-compile-cache/ (default)

Control cache behavior with fhe.configure_cache():
  - force_rebuild=True  — recompile even if cache exists
  - cache_dir=path      — use a custom cache directory
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

# ── Default: cache hit ───────────────────────────────────────────────
print("=== First compile (cache miss) ===")
program = add.compile([x, y])
result = program(x, y)
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()

print("=== Second compile (cache hit) ===")
program = add.compile([x, y])
print("  Cache hit — no recompilation needed")
print()


# ── Force rebuild ────────────────────────────────────────────────────
print("=== Force rebuild ===")
print("  fhe.configure_cache(force_rebuild=True)")
print("  # Next compile() will recompile from scratch")
print()
print("  fhe.configure_cache(force_rebuild=False)")
print("  # Restore cache behavior")
print()


# ── Custom cache directory ──────────────────────────────────────────
print("=== Custom cache directory ===")
print("  fhe.configure_cache(cache_dir='/path/to/cache')")
print("  # Use a different directory for compiled artifacts")