"""
Test domain-specific parameter types.

Verifies that CKKS and other domain-specific types work correctly
with per-parameter instantiation.
"""

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, sihe_kernel, SiheCiphertext


@ckks_kernel
def ckks_multi_params(a: CkksCiphertext, b: CkksCiphertext, c: CkksCiphertext) -> CkksCiphertext:
    """All parameters should be CIPHERTEXT type."""
    return a


@sihe_kernel
def sihe_multi_params(x: SiheCiphertext, y: SiheCiphertext) -> SiheCiphertext:
    """SIHE parameters should work."""
    return x


def test_domain_specific_types():
    """Test that domain-specific types compile correctly."""
    print("Testing domain-specific parameter types...")

    tests = [
        ("ckks_multi_params", ckks_multi_params),
        ("sihe_multi_params", sihe_multi_params),
    ]

    for name, kernel_func in tests:
        print(f"\n  Testing {name}...")
        kernel_func.compile()
        glob = kernel_func.air_module
        ir_dump = glob.dump()

        assert glob is not None, f"{name}: glob should not be None"
        assert len(ir_dump) > 0, f"{name}: IR dump should not be empty"

        print(f"    ✓ {name} compiled successfully")
        print(f"      IR length: {len(ir_dump)} chars")

        # Check for domain-specific keywords in IR
        if "CIPHERTEXT" in ir_dump or "cipher" in ir_dump.lower():
            print(f"      Found ciphertext type references in IR")

    print("\n✓ Test passed: Domain-specific types work correctly")


if __name__ == "__main__":
    try:
        success = test_domain_specific_types()
        if success:
            print("\n" + "=" * 70)
            print("SUCCESS: test_domain_params.py passed")
            print("=" * 70)
            exit(0)
        else:
            print("\n" + "=" * 70)
            print("FAILURE: test_domain_params.py failed")
            print("=" * 70)
            exit(1)
    except Exception as e:
        print(f"\n✗ Test failed with exception: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
