"""
SIHE Kernel Demo - FHE Pipeline

Demonstrates the full pass pipeline starting from SIHE level:
fhe::sihe -> fhe::ckks -> fhe::poly -> C code

Uses CIPHERTEXT RECORD_TYPE for FHE lowering.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import sihe_kernel, SiheCiphertext
from ace_bindings import air_builder


@sihe_kernel
def simple_mul(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Simple SIHE multiply with intermediate storage for poly compatibility."""
    # Use intermediate storage so poly pass can handle it
    # (retv with direct CKKS operand is not supported)
    result = a * b
    return result


def run_demo():
    print("=" * 70)
    print("SIHE Kernel Demo - FHE Pipeline")
    print("=" * 70)
    
    simple_mul.compile()
    glob = simple_mul.air_module
    
    print("\n1. Initial IR (fhe::sihe level):")
    print(glob.dump())
    
    print("\n2. Running ckks_driver...")
    result = air_builder.run_ckks_driver(glob)
    print(f"Success: {result['success']}, Message: {result['message']}")
    if result['success'] and 'ir_dump' in result:
        print(result["ir_dump"])

    if result['success']:
        print("\n3. Running poly_driver...")
        poly_result = air_builder.run_poly_driver(glob)
        print(f"Success: {poly_result['success']}, Message: {poly_result['message']}")
        if poly_result['success'] and 'ir_dump' in poly_result:
            print(poly_result['ir_dump'])

        if poly_result['success']:
            print("\n4. Running poly2c...")
            glob.run_cpp_pass("poly2c", [])
            c_code = glob.get_c_code()
            # Print first 100 lines of C code
            lines = c_code.split('\n')[:100]
            print('\n'.join(lines))
            if len(c_code.split('\n')) > 100:
                print(f"... ({len(c_code.split(chr(10)))} total lines)")

    print("\n" + "=" * 70)
    print("✓ Demo complete")
    print("=" * 70)


if __name__ == "__main__":
    run_demo()
