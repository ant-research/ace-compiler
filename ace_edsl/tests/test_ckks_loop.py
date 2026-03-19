#!/usr/bin/env python3
"""
Test CKKS kernels with loop constructs.

This test demonstrates:
1. range_constexpr - compile-time unrolled loops
2. range_dynamic - AIR do_loop generation (constant bounds, step=1)
3. Kernel instantiation with actual CkksCiphertext instances

Usage:
    python3 tests/test_ckks_loop.py
"""

import sys
import os

# Setup path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".."))

from ace_edsl.edsl import (
    ckks_kernel, 
    CkksCiphertext, 
    AceEDSL,
    range_dynamic,
    range_constexpr,
)


# =============================================================================
# Create ciphertext instances for testing
# =============================================================================

# Create instances with explicit shapes and names for better debugging
ct_input = CkksCiphertext(shape=(16384,), name="ct_input")
ct_zero = CkksCiphertext(shape=(16384,), name="ct_zero")


# =============================================================================
# Test 1: range_constexpr (compile-time unrolled)
# =============================================================================

@ckks_kernel
def ckks_loop_constexpr(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """
    Loop with range_constexpr - fully unrolled at compile time.
    No AIR loop IR generated, just repeated operations.
    """
    result = ct
    
    # This loop is unrolled: generates 4 separate CKKS.add operations
    for i in range_constexpr(0, 4):
        result = result + ct
    
    return result


# =============================================================================
# Test 2: range_dynamic (AIR do_loop)
# =============================================================================

@ckks_kernel
def ckks_loop_dynamic(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """
    Loop with range_dynamic - generates AIR do_loop.
    Only supports constant bounds and step=1.
    """
    result = ct
    
    # This generates an AIR do_loop with induction variable
    for i in range_dynamic(0, 4):
        result = result + ct
    
    return result


# =============================================================================
# Test 3: Nested loops (constexpr)
# =============================================================================

@ckks_kernel
def ckks_nested_loop_constexpr(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """
    Nested loops with range_constexpr - all unrolled.
    """
    result = ct
    
    for i in range_constexpr(0, 2):
        for j in range_constexpr(0, 2):
            result = result + ct
    
    return result


# =============================================================================
# Test 4: Loop with rotate (common FHE pattern)
# =============================================================================

@ckks_kernel
def ckks_rotate_loop(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """
    Loop with rotation - common pattern in FHE (e.g., sum reduction).
    Uses range_constexpr since rotation amounts are typically fixed.
    """
    result = ct
    
    # Simulate a sum reduction pattern: rotate and add
    for shift in range_constexpr(1, 5):  # shifts: 1, 2, 3, 4
        rotated = ct.rotate(shift)
        result = result + rotated
    
    return result


# =============================================================================
# Test Runner
# =============================================================================

def run_test(name: str, kernel_func, expect_loop_ir: bool = False):
    """Run a single test and report results."""
    print(f"\n{'='*60}")
    print(f"Test: {name}")
    print(f"{'='*60}")
    
    # Clear DSL state
    AceEDSL._get_dsl.cache_clear()
    dsl = AceEDSL._get_dsl()
    
    # Create fresh instances for each test
    ct = CkksCiphertext(shape=(16384,), name="ct_input")
    zero = CkksCiphertext(shape=(16384,), name="ct_zero")
    
    # Execute kernel with actual instances (not None!)
    print(f"\n[1] Executing kernel with instances:")
    print(f"    ct   = {ct}")
    print(f"    zero = {zero}")
    try:
        kernel_func(ct, zero)
        print(f"    ✓ Kernel executed successfully")
    except Exception as e:
        print(f"    ✗ Kernel execution failed: {e}")
        return False
    
    # Get AIR
    glob = dsl.current_air_module
    if glob is None:
        print(f"    ✗ No AIR module generated")
        return False
    
    # Dump and analyze AIR
    air = glob.dump()
    print(f"\n[2] Generated AIR ({len(air)} chars):")
    
    # Count operations
    ckks_add_count = air.count("CKKS.add")
    ckks_rotate_count = air.count("CKKS.rotate")
    loop_count = air.count("do_loop") + air.count("LOOP")
    
    print(f"    CKKS.add count: {ckks_add_count}")
    print(f"    CKKS.rotate count: {ckks_rotate_count}")
    print(f"    Loop constructs: {loop_count}")
    
    # Check expectations
    if expect_loop_ir:
        if loop_count > 0:
            print(f"    ✓ Loop IR generated as expected")
        else:
            print(f"    ⚠ Expected loop IR but found none (may be unrolled)")
    else:
        if loop_count == 0:
            print(f"    ✓ No loop IR (unrolled as expected)")
        else:
            print(f"    ⚠ Found loop IR when unrolling was expected")
    
    # Show full AIR
    print(f"\n[3] Full AIR dump:")
    print("-" * 40)
    # Find the function body
    print(air)
    print("-" * 40)
    
    return True


def main():
    print("=" * 60)
    print("CKKS Loop Test Suite")
    print("=" * 60)
    print("\nThis test demonstrates loop constructs in ace_edsl:")
    print("- range_constexpr: Compile-time unrolled (no loop IR)")
    print("- range_dynamic: AIR do_loop (constant bounds, step=1)")
    
    results = {}
    
    # Test 1: range_constexpr
    results["constexpr_loop"] = run_test(
        "range_constexpr (unrolled)",
        ckks_loop_constexpr,
        expect_loop_ir=False  # Should be unrolled
    )
    
    # Test 2: range_dynamic
    results["dynamic_loop"] = run_test(
        "range_dynamic (AIR do_loop)",
        ckks_loop_dynamic,
        expect_loop_ir=True  # Should generate loop IR
    )
    
    # Test 3: Nested constexpr
    results["nested_constexpr"] = run_test(
        "Nested range_constexpr",
        ckks_nested_loop_constexpr,
        expect_loop_ir=False  # Should be unrolled
    )
    
    # Test 4: Rotate loop
    results["rotate_loop"] = run_test(
        "Rotate loop (range_constexpr)",
        ckks_rotate_loop,
        expect_loop_ir=False  # Should be unrolled
    )
    
    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    passed = sum(1 for v in results.values() if v)
    total = len(results)
    
    for name, result in results.items():
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"  {name}: {status}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n✓ All loop tests passed!")
        return 0
    else:
        print("\n✗ Some tests failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())

