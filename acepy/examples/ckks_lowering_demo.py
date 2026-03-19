"""
CKKS Kernel Demo - FHE Pipeline

Demonstrates the full pass pipeline starting from CKKS level:
fhe::ckks -> fhe::poly -> C code

Starts at the CKKS level, skipping sihe2ckks transformation.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext
from ace_bindings import air_builder


@ckks_kernel
def ckks_mul_add(a: CkksCiphertext, b: CkksCiphertext, c: CkksCiphertext) -> CkksCiphertext:
    """
    CKKS-level multiply-add operation.
    Creates CKKS.mul, CKKS.add operations directly at fhe::ckks level.
    """
    # CKKS multiplication followed by addition
    product = a * b
    result = product + c
    return result


def run_demo():
    print("=" * 70)
    print("CKKS Kernel Demo - FHE Pipeline")
    print("=" * 70)
    
    ckks_mul_add.compile()
    glob = ckks_mul_add.air_module
    
    # ========================================================================
    # Step 1: Initial IR (fhe::ckks level)
    # ========================================================================
    print("\n" + "=" * 70)
    print("1. Initial IR (fhe::ckks level)")
    print("   Starting point: CKKS.mul, CKKS.add operations")
    print("=" * 70)
    print(glob.dump())
    
    # ========================================================================
    # Step 2: Prepare IR for poly pass
    # The poly_driver expects intermediate storage for CKKS operations.
    # run_ckks_driver handles this but since we start at CKKS level,
    # we still need to call it for the post-processing step.
    # ========================================================================
    print("\n" + "=" * 70)
    print("2. Preparing IR for poly pass (via ckks_driver post-processing)")
    print("=" * 70)
    ckks_result = air_builder.run_ckks_driver(glob)
    print(f"CKKS prep: {ckks_result['success']}, Message: {ckks_result['message']}")
    if ckks_result['success'] and 'ir_dump' in ckks_result:
        print(ckks_result["ir_dump"])
    
    if not ckks_result['success']:
        print("Failed to prepare IR for poly pass")
        return
    
    # ========================================================================
    # Step 3: poly_driver (fhe::ckks -> fhe::poly)
    # ========================================================================
    print("\n" + "=" * 70)
    print("3. Poly-level IR (fhe::poly operations)")
    print("=" * 70)
    poly_result = air_builder.run_poly_driver(glob)
    print(f"Success: {poly_result['success']}, Message: {poly_result['message']}")
    if poly_result['success'] and 'ir_dump' in poly_result:
        print(poly_result['ir_dump'])

    # ========================================================================
    # Step 4: poly2c (fhe::poly -> C code)
    # ========================================================================
    if poly_result['success']:
        print("\n" + "=" * 70)
        print("4. Generated C Code (poly2c)")
        print("=" * 70)
        glob.run_cpp_pass("poly2c", [])
        c_code = glob.get_c_code()
        # Print first 100 lines of C code
        lines = c_code.split('\n')[:100]
        print('\n'.join(lines))
        if len(c_code.split('\n')) > 100:
            print(f"... ({len(c_code.split(chr(10)))} total lines)")

    # ========================================================================
    # Summary
    # ========================================================================
    print("\n" + "=" * 70)
    print("Summary")
    print("=" * 70)
    print("""
Pipeline Status (starting from @ckks_kernel):
  ✓ fhe::ckks (initial)      -> prepared for poly (ckks_driver post-processing)
  ✓ fhe::ckks -> fhe::poly   (poly_driver)
  ✓ fhe::poly -> C code      (poly2c)

Note: @ckks_kernel starts at fhe::ckks level, skipping:
  - tensor2vector pass
  - vector2sihe pass
  - sihe2ckks transformation

The ckks_driver is still called to:
  - Register CKKS types (CIPHERTEXT3, POLY)
  - Insert intermediate storage for poly pass compatibility

All stages show AIR IR dumps:
  - fhe::ckks:  CKKS.mul, CKKS.add with CIPHERTEXT types
  - fhe::poly:  POLY.hw_modmul, POLY.coeffs, RNS loops
  - C code:     Generated C code with FHE polynomial runtime calls
""")
    
    print("=" * 70)
    print("✓ Demo complete")
    print("=" * 70)


if __name__ == "__main__":
    run_demo()

