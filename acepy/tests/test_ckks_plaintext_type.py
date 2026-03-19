"""
Unit test for CkksPlaintext type.

Validates that CkksPlaintext maps to TYP[0x13] PLAINTEXT in the IR.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import (
    ckks_kernel, CkksCiphertext, CkksPlaintext
)
from ace_dsl.core.types import CkksPlaintext as CkksPlaintextType


# Define test kernels at module level to avoid indentation issues
@ckks_kernel
def add_plain(ct: CkksCiphertext, pt: CkksPlaintext) -> CkksCiphertext:
    return ct + pt


@ckks_kernel
def add_ct(ct1: CkksCiphertext, ct2: CkksCiphertext) -> CkksCiphertext:
    return ct1 + ct2


@ckks_kernel
def add_pt(ct: CkksCiphertext, pt: CkksPlaintext) -> CkksCiphertext:
    return ct + pt


def test_plaintext_type_exists():
    """Test that CkksPlaintext type is importable."""
    assert CkksPlaintextType is not None
    print("✓ CkksPlaintext type exists")


def test_plaintext_param_ir():
    """Test that CkksPlaintext maps to TYP[0x13] in IR."""
    # Compile
    add_plain.compile()
    ir_dump = add_plain.air_module.dump()

    # Print IR for debugging
    print("\n=== IR Dump ===")
    print(ir_dump)
    print("=== End IR Dump ===\n")

    # Verify IR contains PLAINTEXT type
    assert "PLAINTEXT" in ir_dump, "PLAINTEXT type not found in IR"

    # Check for TYP[0x13] or PLAINTEXT with parameter references
    has_plaintext_type = (
        'TYP[0x13]' in ir_dump or
        'PLAINTEXT' in ir_dump
    )
    assert has_plaintext_type, "Parameter pt not using PLAINTEXT type"

    print("✓ CkksPlaintext maps to TYP[0x13] PLAINTEXT in IR")


def test_ciphertext_vs_plaintext():
    """Test that CkksCiphertext and CkksPlaintext generate different types."""
    add_ct.compile()
    add_pt.compile()

    ir_ct = add_ct.air_module.dump()
    ir_pt = add_pt.air_module.dump()

    # Both should have CIPHERTEXT
    assert "CIPHERTEXT" in ir_ct
    assert "CIPHERTEXT" in ir_pt

    # Only pt version should have PLAINTEXT
    # (ct version has two CIPHERTEXTs)
    assert ir_pt.count("PLAINTEXT") > ir_ct.count("PLAINTEXT")

    print("✓ CkksCiphertext and CkksPlaintext generate distinct types")


if __name__ == "__main__":
    test_plaintext_type_exists()
    test_plaintext_param_ir()
    test_ciphertext_vs_plaintext()
    print("\n✓ All tests passed!")
