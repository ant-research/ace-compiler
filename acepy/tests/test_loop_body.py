"""Test loop body IR generation."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import vector_kernel, VectorTensor


@vector_kernel
def simple_loop(a: VectorTensor, b: VectorTensor) -> VectorTensor:
    """Simple loop with body statements."""
    result = a + b  # Initial value outside loop
    
    # Loop with body
    for i in range(3):
        # Each iteration should accumulate
        tmp = a * b
        result = result + tmp
    
    return result


@vector_kernel
def nested_loops(a: VectorTensor, b: VectorTensor) -> VectorTensor:
    """Nested loops."""
    result = a
    
    for i in range(2):
        for j in range(3):
            result = result + a * b
    
    return result


if __name__ == "__main__":
    print("=" * 70)
    print("Testing Loop Body IR Generation")
    print("=" * 70)
    
    # Test 1: Simple loop
    print("\n1. Simple Loop:")
    print("-" * 50)
    simple_loop.compile()
    ir = simple_loop.dump_ir()
    print(ir)
    
    # Check if loop body has statements
    if "do_loop" in ir.lower():
        print("\n✓ do_loop found")
        # Look for statements inside the loop
        lines = ir.split('\n')
        in_loop = False
        body_stmts = 0
        for line in lines:
            if "do_loop" in line.lower():
                in_loop = True
            if in_loop and ("st " in line or "stid" in line.lower() or "VECTOR.mul" in line or "VECTOR.add" in line):
                body_stmts += 1
                print(f"  Body statement: {line.strip()}")
        print(f"  Total body statements: {body_stmts}")
    else:
        print("\n✗ No do_loop found!")
    
    # Test 2: Nested loops
    print("\n2. Nested Loops:")
    print("-" * 50)
    nested_loops.compile()
    ir = nested_loops.dump_ir()
    print(ir[:1500])  # Print first 1500 chars
    if len(ir) > 1500:
        print(f"\n... ({len(ir) - 1500} more chars)")
    
    print("\n" + "=" * 70)

