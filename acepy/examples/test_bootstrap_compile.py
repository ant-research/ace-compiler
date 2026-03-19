"""Test custom bootstrap kernel compilation."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks


# Build up step by step to find where it fails

@ckks_kernel
def step1_rotate(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """Just rotate"""
    rotated = ckks.rotate(ct, 4)
    return rotated


@ckks_kernel
def step2_rotate_add(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """Rotate and add"""
    rotated = ckks.rotate(ct, 4)
    result = ct + rotated
    return result


@ckks_kernel
def step3_dft(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """DFT part"""
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    dft2_rot = ckks.rotate(dft1, 1)
    slot_repr = dft1 + dft2_rot
    return slot_repr


@ckks_kernel
def step4_evalmod(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """EvalMod (polynomial)"""
    x = ct
    x2 = x * x
    x3 = x * x2
    sin_approx = x - x3
    return sin_approx


@ckks_kernel
def step5_full(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """Full bootstrap"""
    # Phase 1: CoeffToSlot (DFT)
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    dft2_rot = ckks.rotate(dft1, 1)
    slot_repr = dft1 + dft2_rot
    
    # Phase 2: EvalMod (simplified polynomial)
    x = slot_repr
    x2 = x * x
    x3 = x * x2
    sin_approx = x - x3
    
    # Phase 3: SlotToCoeff (iDFT)
    idft0_rot = ckks.rotate(sin_approx, 1)
    idft0 = sin_approx - idft0_rot
    idft1_rot = ckks.rotate(idft0, 2)
    idft1 = idft0 - idft1_rot
    idft2_rot = ckks.rotate(idft1, 4)
    result = idft1 - idft2_rot
    
    return result


if __name__ == "__main__":
    tests = [
        ("step1_rotate", step1_rotate),
        ("step2_rotate_add", step2_rotate_add),
        ("step3_dft", step3_dft),
        ("step4_evalmod", step4_evalmod),
        ("step5_full", step5_full),
    ]
    
    for name, kernel in tests:
        print(f"\n{name}:")
        try:
            kernel.compile()
            ir = kernel.air_module.dump()
            
            # Count operations
            ops = {
                'rotate': ir.lower().count('ckks.rotate'),
                'add': ir.lower().count('ckks.add'),
                'sub': ir.lower().count('ckks.sub'),
                'mul': ir.lower().count('ckks.mul'),
            }
            print(f"  ✓ OK - {ops}")
        except Exception as e:
            print(f"  ✗ FAILED: {e}")
            import traceback
            traceback.print_exc()
            break  # Stop at first failure

