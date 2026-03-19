#!/usr/bin/env python3
"""
Test function inlining in PyACE.

This demonstrates how helper functions are automatically inlined
when called from a @kernel function.
"""

import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from ace_dsl import kernel, Tensor
from ace_dsl.frontend.decorator import register_helper
from ace_dsl.frontend.compile import compile_fhe


# ============================================================================
# Example 1: Simple helper function (auto-detected from closure)
# ============================================================================

def add_one(x):
    """Helper: adds 1 to input."""
    return x + 1


@kernel
def kernel_with_helper(a: Tensor[64]) -> Tensor[64]:
    """Kernel that calls a helper function."""
    return add_one(a)


# ============================================================================
# Example 2: Registered helper function
# ============================================================================

@register_helper
def square(x):
    """Helper: squares the input."""
    return x * x


@kernel
def kernel_with_registered_helper(a: Tensor[64]) -> Tensor[64]:
    """Kernel using a registered helper."""
    return square(a)


# ============================================================================
# Example 3: Multiple helper calls
# ============================================================================

def increment(x):
    return x + 1


@kernel
def kernel_with_multiple_calls(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
    """Kernel with multiple helper calls."""
    a1 = increment(a)
    b1 = increment(b)
    return a1 + b1


# ============================================================================
# Example 4: Chained helper calls
# ============================================================================

def double(x):
    return x + x


def quadruple(x):
    y = double(x)
    return double(y)


@kernel
def kernel_with_chained_calls(a: Tensor[64]) -> Tensor[64]:
    """Kernel with chained helper calls (helper calls helper)."""
    return quadruple(a)


# ============================================================================
# Main
# ============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("Testing Function Inlining in PyACE")
    print("=" * 60)
    
    # Test 1: Simple helper
    print("\n--- Test 1: Simple helper function ---")
    kernel_with_helper.compile(enable_ir_printing=True)
    print("\nAIR dump:")
    print(kernel_with_helper.air_module.dump())
    
    # Test 2: Registered helper
    print("\n--- Test 2: Registered helper function ---")
    kernel_with_registered_helper.compile(enable_ir_printing=True)
    print("\nAIR dump:")
    print(kernel_with_registered_helper.air_module.dump())
    
    # Test 3: Multiple calls
    print("\n--- Test 3: Multiple helper calls ---")
    kernel_with_multiple_calls.compile(enable_ir_printing=True)
    print("\nAIR dump:")
    print(kernel_with_multiple_calls.air_module.dump())
    
    # Test 4: Chained calls
    print("\n--- Test 4: Chained helper calls ---")
    kernel_with_chained_calls.compile(enable_ir_printing=True)
    print("\nAIR dump:")
    print(kernel_with_chained_calls.air_module.dump())
    
    # Compile to C code
    print("\n--- Compiling to C code ---")
    c_code = compile_fhe(kernel_with_helper)
    print(c_code)
    
    print("\n✓ All inlining tests passed!")

