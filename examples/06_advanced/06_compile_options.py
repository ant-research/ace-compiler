#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: Compile Options

Pass compiler options to control FHE compilation behavior.
Options are specified in the @fhe.compile / @fhe.compute decorator.

Available option groups:
  ckks:  CKKS scheme — N, q0, sf, hw, sbm, icl, mcl
  vec:   Vectorization — ms (max_slots), conv_parl
  sihe:  SIHE scheme — relu_vr_def, relu_vr
  p2c:   Poly-to-C codegen — fp
  o2a:   O2A optimization — ts
  poly:  Polynomial options — ts, rtt
"""

import torch
from ace import fhe


# ── CKKS options ─────────────────────────────────────────────────────
# Control encryption parameters: poly degree, mod sizes, hamming weight

@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
    ckks={"N": 16, "q0": 60, "sf": 56},
)
def add_ckks(x, y):
    """Add with custom CKKS parameters."""
    return x + y


x = torch.randn(1, 4)
y = torch.randn(1, 4)

print("=== CKKS options: N=16, q0=60, sf=56 ===")
program = add_ckks.compile([x, y])
result = program(x, y)
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()


# ── Environment variable override ────────────────────────────────────
# ACE_COMPILE_OPTIONS env var has highest priority, overriding decorator options.
# Example: ACE_COMPILE_OPTIONS='{"ckks": {"N": 8192}}' python script.py

from ace.fhe.config.default_options import set_env_options, clear_env_options

print("=== Env var override: ACE_COMPILE_OPTIONS ===")
print("  Set env: ACE_COMPILE_OPTIONS='{\"ckks\": {\"N\": 16}}'")
print("  This overrides decorator-level ckks options.")
print()
print("  Programmatic equivalent:")
print("    from ace.fhe.config.default_options import set_env_options")
print("    set_env_options({'ckks': {'N': 16}})")
print("    # ... run compilation ...")
print("    clear_env_options()  # restore default")