"""Test kernel compilation ONLY - no model loading."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks


@ckks_kernel
def custom_bootstrap(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """Custom bootstrap"""
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    x2 = dft1 * dft1
    result = dft1 - x2
    idft_rot = ckks.rotate(result, 1)
    output = result + idft_rot
    return output


if __name__ == "__main__":
    print("Compiling custom_bootstrap...")
    custom_bootstrap.compile()
    print("Success!")
    ir = custom_bootstrap.air_module.dump()
    print(f"IR size: {len(ir)} chars")
    print(f"rotate: {ir.lower().count('ckks.rotate')}")
    print(f"add: {ir.lower().count('ckks.add')}")
    print(f"mul: {ir.lower().count('ckks.mul')}")
    print(f"sub: {ir.lower().count('ckks.sub')}")

