#!/usr/bin/env python3
"""Debug SIHE domain detection in acepy."""

import sys
import os

# Setup path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".."))

from ace_dsl.frontend.domain_kernels import sihe_kernel, ckks_kernel, SiheCiphertext, CkksCiphertext
from ace_bindings import air_builder

print("=" * 70)
print("Debug SIHE Domain Detection")
print("=" * 70)

# Test 1: SIHE kernel
print("\n=== Test 1: @sihe_kernel ===")

@sihe_kernel
def sihe_add(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    return a + b

sihe_add.compile()
glob = sihe_add.air_module

print(f"\n[AIR from @sihe_kernel]")
air_dump = glob.dump()
print("First 3000 chars of dump:")
print(air_dump[:3000])

# Save for analysis
with open("/tmp/acepy_sihe_initial.air", "w") as f:
    f.write(air_dump)
print("\nSaved to /tmp/acepy_sihe_initial.air")

# Check for SIHE vs CKKS ops in the dump
if "fhe::sihe" in air_dump or "SIHE" in air_dump:
    print("\n✓ Found SIHE ops in dump")
else:
    print("\n✗ NO SIHE ops found in dump!")
    
if "fhe::ckks" in air_dump or "CKKS" in air_dump:
    print("✗ Found CKKS ops in dump (unexpected!)")
else:
    print("✓ No CKKS ops in dump")

# Test 2: CKKS kernel for comparison
print("\n\n=== Test 2: @ckks_kernel ===")

@ckks_kernel
def ckks_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    return a + b

ckks_add.compile()
glob2 = ckks_add.air_module

print(f"\n[AIR from @ckks_kernel]")
air_dump2 = glob2.dump()
print("First 3000 chars of dump:")
print(air_dump2[:3000])

with open("/tmp/acepy_ckks_initial.air", "w") as f:
    f.write(air_dump2)
print("\nSaved to /tmp/acepy_ckks_initial.air")

if "fhe::sihe" in air_dump2 or "SIHE" in air_dump2:
    print("\n✗ Found SIHE ops in CKKS kernel dump (unexpected)")
else:
    print("\n✓ No SIHE ops in dump")
    
if "fhe::ckks" in air_dump2 or "CKKS" in air_dump2:
    print("✓ Found CKKS ops in dump")
else:
    print("✗ NO CKKS ops found!")

