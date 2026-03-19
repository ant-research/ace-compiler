"""
Test backward compatibility with existing code.

Ensures that existing kernels with single parameters or uniform shapes
still work correctly after the per-parameter changes.
"""

from ace_dsl.frontend.decorator import kernel
from ace_dsl.core.types import Tensor


@kernel
def old_style_single_param(x: Tensor[64]) -> Tensor[64]:
    """Existing code with single parameter should still work."""
    return x


@kernel
def old_style_uniform_shapes(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
    """Existing code with uniform shapes should still work."""
    return a


@kernel
def no_annotation(x):
    """Unannotated parameter should get default [64] shape."""
    return x


@kernel
def partially_annotated(a: Tensor[128], b):
    """Mix of annotated and unannotated should work."""
    return a


def test_backward_compatibility():
    """Test that all backward compatibility cases work."""
    print("Testing backward compatibility...")

    tests = [
        ("old_style_single_param", old_style_single_param),
        ("old_style_uniform_shapes", old_style_uniform_shapes),
        ("no_annotation", no_annotation),
        ("partially_annotated", partially_annotated),
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

    print("\n✓ Test passed: All backward compatibility tests succeeded")


if __name__ == "__main__":
    try:
        success = test_backward_compatibility()
        if success:
            print("\n" + "=" * 70)
            print("SUCCESS: test_backward_compat.py passed")
            print("=" * 70)
            exit(0)
        else:
            print("\n" + "=" * 70)
            print("FAILURE: test_backward_compat.py failed")
            print("=" * 70)
            exit(1)
    except Exception as e:
        print(f"\n✗ Test failed with exception: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
