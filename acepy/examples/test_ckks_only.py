#!/usr/bin/env python3
"""Test CKKS-only kernel (no SIHE transformation) through poly_driver."""

import sys
import os

# Setup path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".."))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext
from ace_bindings import air_builder

print("=" * 70)
print("Test: CKKS-only kernel (no SIHE transformation)")
print("=" * 70)

@ckks_kernel
def ckks_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    return a + b

# Compile kernel
ckks_add.compile()
glob = ckks_add.air_module

print(f"\n[AIR] Initial size: {len(glob.dump())} chars")

# Configure FHE params
glob.configure_fhe_params(
    poly_degree=16384,
    mul_level=10,
    scaling_factor_bits=56,
    first_prime_bits=60,
    hamming_weight=192,
)

# Run CKKS driver (skip sihe2ckks since it's already CKKS)
print("\n[Pipeline] Running ckks_driver...")
result = air_builder.run_ckks_driver(glob)
if not result.get("success"):
    print(f"  CKKS driver failed: {result.get('message')}")
    sys.exit(1)
print("  ✓ CKKS driver succeeded")

# Run Poly driver
print("\n[Pipeline] Running poly_driver...")
result = air_builder.run_poly_driver(glob)
if not result.get("success"):
    print(f"  Poly driver failed: {result.get('message')}")
    sys.exit(1)
print("  ✓ Poly driver succeeded")

# Dump IR after poly_driver
poly_ir = glob.dump()
print(f"\n[IR after poly_driver] Size: {len(poly_ir)} chars")

# Test poly2c
print("\n[Pipeline] Running poly2c...")
ok = glob.run_poly2c()
if ok:
    c_code = glob.get_c_code()
    print(f"  ✓ C code generated: {len(c_code)} bytes")
else:
    print("  ✗ poly2c failed")
    sys.exit(1)

print("\n" + "=" * 70)
print("SUCCESS: CKKS-only kernel works through full pipeline!")
print("=" * 70)

