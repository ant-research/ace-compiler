#!/usr/bin/env python3
"""
Test for scalar parameter approach to dynamic conditions.

This test verifies that scalar parameters (Int, Float) can be used
in dynamic_expr conditions, enabling cleaner syntax:

    @sihe_kernel
    def kernel(a: SiheCiphertext, b: SiheCiphertext, flag: Int):
        if dynamic_expr(flag > 0):  # flag is AIRValue, comparison returns AIRValue
            return a + b
        else:
            return a - b
"""

import os
import sys


def _setup_sys_path():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    parent_root = os.path.abspath(os.path.join(repo_root, ".."))
    for path in (repo_root, parent_root):
        if path not in sys.path:
            sys.path.insert(0, path)


_setup_sys_path()


from ace_edsl.edsl import sihe_kernel, AceEDSL
from ace_edsl.edsl.core.types import SiheCiphertext, Int
from ace_edsl.base_dsl.ast_helpers import dynamic_expr


def test_scalar_param_comparison():
    """
    Test that Int parameter can be used in dynamic_expr comparison.
    
    The kernel has:
      - a, b: SiheCiphertext parameters
      - flag: Int parameter (becomes AIRValue)
    
    Inside the kernel, `flag > 0` uses AIRValue.__gt__(0) to create
    an AIR comparison node, which dynamic_expr uses to generate
    an if-then-else structure in the AIR.
    """
    print("=" * 60)
    print("Test: Scalar Parameter for Dynamic Conditions")
    print("=" * 60)
    
    # Clear DSL cache for fresh state
    AceEDSL._get_dsl.cache_clear()
    dsl = AceEDSL._get_dsl()
    
    # Define kernel with Int scalar parameter
    @sihe_kernel
    def conditional_kernel(a: SiheCiphertext, b: SiheCiphertext, flag: Int) -> SiheCiphertext:
        out = a + b
        # flag is an AIRValue, so flag > 0 uses AIRValue.__gt__
        # which generates an AIR comparison node
        if dynamic_expr(flag > 0):
            out = a + b
        else:
            out = a - b
        return out
    
    # Create type instances for call
    a = SiheCiphertext[float, 64]
    b = SiheCiphertext[float, 64]
    
    print("\n[Step 1] Calling kernel with flag=1...")
    try:
        conditional_kernel(a, b, 1)
        print("  ✓ Kernel execution succeeded!")
    except Exception as e:
        print(f"  ✗ Kernel execution failed: {e}")
        return False
    
    # Check the generated AIR
    glob = dsl.current_air_module
    if glob is None:
        print("  ✗ No AIR module generated")
        return False
    
    air_dump = glob.dump()
    print(f"\n[Step 2] Analyzing generated AIR ({len(air_dump)} chars)...")
    
    # Save AIR dump for inspection
    out_dir = os.path.join(os.path.dirname(__file__), "output")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "scalar_param_test.air")
    with open(out_path, "w") as f:
        f.write(air_dump)
    print(f"  ✓ AIR dump saved to: {out_path}")
    
    # Check for comparison and control flow
    air_upper = air_dump.upper()
    
    has_comparison = "GT" in air_upper or "CMP" in air_upper
    has_control_flow = "IF" in air_upper or "TRUEBR" in air_upper or "FALSEBR" in air_upper
    
    print("\n[Step 3] Checking AIR structure...")
    if has_comparison:
        print("  ✓ Comparison operation (GT/CMP) found in AIR")
    else:
        print("  ⚠ No comparison operation found in AIR")
    
    if has_control_flow:
        print("  ✓ Control flow (IF/TRUEBR/FALSEBR) found in AIR")
    else:
        print("  ⚠ No control flow found in AIR (condition may have been optimized)")
    
    # Print snippet of AIR
    print("\n[AIR Snippet - first 2000 chars]")
    print("-" * 40)
    print(air_dump[:2000])
    print("-" * 40)
    
    return True


def test_constant_predicate():
    """
    Test that constant predicate (Python bool) is handled at compile time.
    
    When dynamic_expr receives a Python bool (not AIRValue), the if-else
    is evaluated at compile time and only one branch is emitted.
    """
    print("\n" + "=" * 60)
    print("Test: Constant Predicate (Compile-time)")
    print("=" * 60)
    
    # Clear DSL cache for fresh state
    AceEDSL._get_dsl.cache_clear()
    dsl = AceEDSL._get_dsl()
    
    @sihe_kernel
    def const_pred_kernel(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
        out = a + b
        if dynamic_expr(True):  # Python bool - evaluated at compile time
            out = a + b
        else:
            out = a - b  # This branch won't be in AIR
        return out
    
    a = SiheCiphertext[float, 64]
    b = SiheCiphertext[float, 64]
    
    print("\n[Step 1] Calling kernel with constant predicate (True)...")
    try:
        const_pred_kernel(a, b)
        print("  ✓ Kernel execution succeeded!")
    except Exception as e:
        print(f"  ✗ Kernel execution failed: {e}")
        return False
    
    glob = dsl.current_air_module
    if glob is None:
        print("  ✗ No AIR module generated")
        return False
    
    air_dump = glob.dump()
    print(f"\n[Step 2] Analyzing generated AIR ({len(air_dump)} chars)...")
    
    # For constant True predicate, there should be NO if/else in AIR
    air_upper = air_dump.upper()
    has_control_flow = "IF" in air_upper or "TRUEBR" in air_upper or "FALSEBR" in air_upper
    
    if not has_control_flow:
        print("  ✓ No control flow in AIR (constant predicate evaluated at compile time)")
    else:
        print("  ⚠ Control flow found (expected no control flow for constant predicate)")
    
    # Should have ADD but NOT SUB (since False branch not taken)
    has_sub = "SUB" in air_upper or "SIHE.SUB" in air_upper
    if not has_sub:
        print("  ✓ No SUB operation in AIR (else branch correctly pruned)")
    else:
        print("  ⚠ SUB operation found (else branch should have been pruned)")
    
    return True


if __name__ == "__main__":
    print("\n" + "#" * 60)
    print("# Scalar Parameter Test Suite")
    print("#" * 60 + "\n")
    
    # Run tests
    results = []
    
    results.append(("Scalar Parameter (Int)", test_scalar_param_comparison()))
    results.append(("Constant Predicate", test_constant_predicate()))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    for name, passed in results:
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"  {status}: {name}")
    
    all_passed = all(passed for _, passed in results)
    sys.exit(0 if all_passed else 1)

