#!/usr/bin/env python3
"""
Example: Simple Addition Kernel
================================

Demonstrates the basic usage of ACE DSL for a simple addition operation.
"""

import sys
sys.path.insert(0, '..')

from ace_dsl import kernel, compile_to_ir, Tensor


@kernel
def simple_add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
    """Simple element-wise addition."""
    return a + b


@kernel
def add_with_bias(x: Tensor[1, 64], w: Tensor[64, 64], b: Tensor[64]) -> Tensor[64]:
    """Matrix-vector multiply with bias."""
    h = x @ w  # Matrix multiplication
    return h + b


def main():
    print("=== ACE DSL Simple Add Example ===\n")
    
    # Example 1: Simple addition
    print("1. Simple addition kernel:")
    print(f"   Kernel: {simple_add}")
    print(f"   Parameters: {[p.name for p in simple_add.parameters]}")
    print("\n   Python IR:")
    print(simple_add.dump_ir())
    
    # Example 2: Add with bias
    print("\n2. Add with bias kernel:")
    print(f"   Kernel: {add_with_bias}")
    print("\n   Python IR:")
    print(add_with_bias.dump_ir())
    
    # Compile to IR (dry run)
    print("\n3. Compile to IR:")
    ir = compile_to_ir(simple_add, target="tensor")
    print(ir)
    
    print("\n=== Example Complete ===")


if __name__ == "__main__":
    main()

