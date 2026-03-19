"""
CKKS Operations Demo
====================

Demonstrates the new CKKS-specific operations:
- rotate: Rotate ciphertext slots
- rescale: Reduce scale after multiplication
- relin: Relinearization after multiplication
- mod_switch: Reduce modulus level
- bootstrap: Refresh noise budget
- neg: Negate ciphertext
"""

from ace_dsl.frontend.domain_kernels import (
    ckks_kernel, CkksCiphertext, ckks
)
from ace_bindings import air_builder

print("=" * 70)
print("CKKS Operations Demo")
print("=" * 70)

# =============================================================================
# Test: CKKS kernel with various operations
# =============================================================================

@ckks_kernel
def ckks_full_ops(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    """Test kernel with multiple CKKS operations."""
    # Basic multiply
    product = a * b
    
    # Add
    sum_val = product + a
    
    # Rotate by 5 positions
    rotated = ckks.rotate(sum_val, 5)
    
    # Bootstrap to refresh noise
    refreshed = ckks.bootstrap(rotated)
    
    # Rescale
    scaled = ckks.rescale(refreshed)
    
    return scaled

print("\nCompiling CKKS kernel with multiple operations...")
print("-" * 40)

ckks_full_ops.compile()

# Dump the full IR
ir_dump = ckks_full_ops.air_module.dump()
print("\nFull IR dump:")
print(ir_dump)

# Check for each operation
print("\n" + "=" * 70)
print("Operation Check")
print("=" * 70)

ops_to_check = [
    ("CKKS.mul", "Multiplication"),
    ("CKKS.add", "Addition"),
    ("CKKS.rotate", "Rotation"),
    ("CKKS.bootstrap", "Bootstrap"),
    ("CKKS.rescale", "Rescale"),
]

for op_pattern, op_name in ops_to_check:
    if op_pattern.lower() in ir_dump.lower():
        print(f"  ✓ {op_name} ({op_pattern}) - Found")
    else:
        print(f"  ✗ {op_name} ({op_pattern}) - NOT FOUND")

# =============================================================================
# Summary
# =============================================================================

print("\n" + "=" * 70)
print("Summary of Available CKKS Operations")
print("=" * 70)
print("""
Binary operations (use + - * syntax):
  - a + b  →  CKKS.add
  - a - b  →  CKKS.sub  
  - a * b  →  CKKS.mul

Unary operations (use ckks.* functions):
  - ckks.neg(ct)           →  CKKS.neg        (negate)
  - ckks.rotate(ct, n)     →  CKKS.rotate     (rotate slots)
  - ckks.rescale(ct)       →  CKKS.rescale    (reduce scale)
  - ckks.relin(ct)         →  CKKS.relin      (relinearize)
  - ckks.mod_switch(ct)    →  CKKS.mod_switch (reduce level)
  - ckks.bootstrap(ct)     →  CKKS.bootstrap  (refresh noise)

These operations are essential for implementing:
  - DFT/IDFT (using rotate)
  - Bootstrapping (using bootstrap)
  - Depth management (using rescale, mod_switch)
""")

print("=" * 70)
print("✓ Demo complete")
print("=" * 70)
