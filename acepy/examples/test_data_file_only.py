#!/usr/bin/env python3
"""
Test poly2c with ONLY data_file parameter.
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".."))

from ace_dsl.frontend.domain_kernels import sihe_kernel, SiheCiphertext


@sihe_kernel
def simple_sihe_add(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Simple SIHE addition."""
    out = a + b
    out = a + b
    return out


def main():
    print("=" * 70)
    print("Test: poly2c with ONLY data_file parameter")
    print("=" * 70)
    
    simple_sihe_add.compile()
    glob = simple_sihe_add.air_module
    
    print(f"\n[AIR] Initial size: {len(glob.dump())} chars")
    
    print("\n[Pipeline] Running sihe2ckks...")
    glob.configure_fhe_params(
        poly_degree=16384,
        mul_level=10,
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
    )
    glob.run_cpp_pass("sihe2ckks", [])
    print("[PYTHON] sihe2ckks done")
    
    print(f"\n[IR after sihe2ckks] Size: {len(glob.dump())} chars")
    
    print("\n[poly2c] Testing with data_file='test.msg'...")
    
    try:
        ok = glob.run_poly2c(data_file="test.msg")
        print(f"  Result: {ok}")
        
        if ok:
            c_code = glob.get_c_code()
            print(f"  ✓ C code generated: {len(c_code)} bytes")
            return True
        else:
            print("  ✗ poly2c returned False")
            return False
            
    except Exception as e:
        print(f"  ✗ Exception: {e}")
        return False


if __name__ == "__main__":
    success = main()
    print("\n" + "=" * 70)
    print("RESULT:", "✓ PASS" if success else "✗ FAIL")
    print("=" * 70)
    sys.exit(0 if success else 1)
