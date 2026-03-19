#!/usr/bin/env python3
"""Test flat vs tree IR output formats.

All kernels now default to flat=True (SSA-like, children before parent).

Run with:
    cd acepy
    PYTHONPATH=. python examples/test_dump_flat.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks


@ckks_kernel
def simple_bootstrap(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """Simple bootstrap for testing flat dump."""
    return ct + zero


def main():
    print("Compiling simple_bootstrap...")
    simple_bootstrap.compile()
    
    print("\n" + "="*60)
    print("=== Default format for @ckks_kernel (flat=True) ===")
    print("="*60)
    default_ir = simple_bootstrap.dump_ir()  # Uses flat=True by default
    print(default_ir)
    
    print("\n" + "="*60)
    print("=== Tree format (flat=False) ===")
    print("="*60)
    tree_ir = simple_bootstrap.dump_ir(flat=False)
    print(tree_ir)
    
    print("\n" + "="*60)
    print("=== Flat format (flat=True, SSA-like) ===")
    print("="*60)
    flat_ir = simple_bootstrap.dump_ir(flat=True)
    print(flat_ir)
    
    print("\n" + "="*60)
    print("Summary:")
    print("  All kernels now default to flat=True")
    print(f"  Tree format lines: {len(tree_ir.splitlines())}")
    print(f"  Flat format lines: {len(flat_ir.splitlines())}")
    print("="*60)


if __name__ == "__main__":
    main()

