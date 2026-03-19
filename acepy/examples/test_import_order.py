"""Test import order effects."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# Import order test
print("0. Importing air_builder...")
from ace_bindings import air_builder
print("   OK")

print("1. Importing python_lowering_pass...")
from ace_dsl.passes.python_lowering_pass import register_lowering, get_ops_to_skip, clear_lowerings
print("   OK")

print("2. Importing domain_kernels...")
from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks
print("   OK")

print("3. Defining simple add kernel...")
@ckks_kernel
def test_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    return a + b
print("   OK")

print("4. Compiling simple add kernel...")
try:
    test_add.compile()
    print("   OK")
    print("   IR:", len(test_add.air_module.dump()), "chars")
except Exception as e:
    print(f"   FAILED: {e}")

print("5. Defining bootstrap kernel...")
@ckks_kernel
def custom_bootstrap(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    dft2_rot = ckks.rotate(dft1, 1)
    slot_repr = dft1 + dft2_rot
    
    x = slot_repr
    x2 = x * x
    x3 = x * x2
    sin_approx = x - x3
    
    idft0_rot = ckks.rotate(sin_approx, 1)
    idft0 = sin_approx - idft0_rot
    idft1_rot = ckks.rotate(idft0, 2)
    idft1 = idft0 - idft1_rot
    idft2_rot = ckks.rotate(idft1, 4)
    result = idft1 - idft2_rot
    
    return result
print("   OK")

print("6. Compiling bootstrap kernel...")
try:
    custom_bootstrap.compile()
    print("   OK")
    ir = custom_bootstrap.air_module.dump()
    print(f"   IR: {len(ir)} chars")
    print(f"   rotate: {ir.lower().count('ckks.rotate')}")
except Exception as e:
    print(f"   FAILED: {e}")
    import traceback
    traceback.print_exc()

