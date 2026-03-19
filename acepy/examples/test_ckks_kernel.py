"""Test basic CKKS kernel compilation."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks


@ckks_kernel
def simple_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    return a + b


@ckks_kernel  
def simple_mul(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    return a * b


@ckks_kernel
def with_rotate(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    rotated = ckks.rotate(a, 1)
    return rotated + b


if __name__ == "__main__":
    print("Testing CKKS kernel compilation...")
    
    print("\n1. simple_add:")
    try:
        simple_add.compile()
        ir = simple_add.air_module.dump()
        print(f"   OK - {len(ir)} chars")
        print("   IR preview:")
        for line in ir.split('\n')[:15]:
            print(f"   {line}")
    except Exception as e:
        print(f"   ERROR: {e}")
    
    print("\n2. simple_mul:")
    try:
        simple_mul.compile()
        ir = simple_mul.air_module.dump()
        print(f"   OK - {len(ir)} chars")
    except Exception as e:
        print(f"   ERROR: {e}")
    
    print("\n3. with_rotate:")
    try:
        with_rotate.compile()
        ir = with_rotate.air_module.dump()
        print(f"   OK - {len(ir)} chars")
        print("   IR preview:")
        for line in ir.split('\n')[:20]:
            print(f"   {line}")
    except Exception as e:
        print(f"   ERROR: {e}")

