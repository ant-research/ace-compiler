#!/usr/bin/env python3
"""
Test CIPHERTEXT + Scalar Operations
===================================

Tests cipher+scalar operations which generate:
- Encode (scalar → plaintext polynomial)
- Hw_modmul with encoded plaintext

Scalar operations are typically the cheapest because the
scalar is encoded once and multiplied component-wise.

Run with:
    cd acepy
    PYTHONPATH=.:examples python examples/test_cipher_scalar.py
"""

import sys
import os

# Setup path
script_dir = os.path.dirname(os.path.abspath(__file__))
acepy_dir = os.path.dirname(script_dir)
sys.path.insert(0, acepy_dir)
sys.path.insert(0, script_dir)

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext
from ace_dsl.bindings import air_builder

# Output directory for IR dumps
OUTPUT_DIR = os.path.join(script_dir, "output", "cipher_scalar")
os.makedirs(OUTPUT_DIR, exist_ok=True)


@ckks_kernel
def kernel_cipher_scalar(ct: CkksCiphertext) -> CkksCiphertext:
    """CIPHERTEXT + Scalar operations."""
    scaled = ct * 2.0
    return scaled


def save_ir(glob, phase_name, description=""):
    """Save IR dump to file."""
    ir_dump = glob.dump()
    filename = os.path.join(OUTPUT_DIR, f"{phase_name}.ir")
    with open(filename, 'w') as f:
        f.write(f"# Phase: {phase_name}\n")
        f.write(f"# Description: {description}\n")
        f.write(f"# {'='*60}\n\n")
        f.write(ir_dump)
    print(f"  → Saved IR to {filename}")
    return filename


def run_test():
    print("="*60)
    print("Test: CIPHERTEXT + Scalar")
    print("="*60)
    print(f"Output directory: {OUTPUT_DIR}")
    
    # Phase 1: Compile kernel
    print("\n[Phase 1] Compiling @ckks_kernel...")
    kernel_cipher_scalar.compile()
    glob = kernel_cipher_scalar.air_module
    print("  ✓ Compiled")
    save_ir(glob, "01_after_compile", "After @ckks_kernel compilation - scalar multiply")
    
    # Phase 2: CKKS driver
    print("\n[Phase 2] Running ckks_driver...")
    result = air_builder.run_ckks_driver(glob)
    if not result.get('success'):
        print(f"  ✗ ckks_driver failed: {result.get('message')}")
        return False
    print("  ✓ ckks_driver succeeded")
    save_ir(glob, "02_after_ckks_driver", "After ckks_driver - scalar encoded to plaintext")
    
    # Phase 3: Poly driver
    print("\n[Phase 3] Running poly_driver...")
    result = air_builder.run_poly_driver(glob)
    if not result.get('success'):
        print(f"  ✗ poly_driver failed: {result.get('message')}")
        return False
    print("  ✓ poly_driver succeeded")
    save_ir(glob, "03_after_poly_driver", "After poly_driver - polynomial operations")
    
    # Phase 4: poly2c
    print("\n[Phase 4] Running poly2c...")
    glob.run_poly2c(output_file="", data_file="", ct_encode=False, free_poly=True)
    c_code = glob.get_c_code()
    
    if not c_code:
        print("  ✗ No C code generated")
        return False
    
    print(f"  ✓ Generated {len(c_code)} bytes of C code")
    
    # Save C code
    c_filename = os.path.join(OUTPUT_DIR, "04_output.c")
    with open(c_filename, 'w') as f:
        f.write(c_code)
    print(f"  → Saved C code to {c_filename}")
    
    # Analysis
    print("\n[Analysis] C code for scalar operations:")
    ops_found = []
    for op in ['Encode', 'Hw_modmul', 'Mul_plain', 'Mul_const', '2.0']:
        count = c_code.count(op)
        if count > 0:
            ops_found.append(f"{op}:{count}")
            print(f"  ✓ {op}: {count} occurrences")
    
    print(f"\n  Summary: {ops_found}")
    print(f"\nOutput files saved to: {OUTPUT_DIR}/")
    print("  - 01_after_compile.ir")
    print("  - 02_after_ckks_driver.ir")
    print("  - 03_after_poly_driver.ir")
    print("  - 04_output.c")
    
    print("\n✓ cipher+scalar test PASSED")
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
