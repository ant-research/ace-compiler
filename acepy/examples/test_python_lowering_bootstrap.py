"""
Test Python Lowering for Bootstrap (OBSOLETE)
==============================================

NOTE: This test is OBSOLETE. Please use test_deferred_lowering.py instead.

The deferred lowering test properly handles the compilation order:
1. Register lowering WITHOUT compiling
2. Load model and run C++ passes  
3. Run Python lowering pass (compiles kernel lazily)

This test was written before deferred compilation was implemented.

Run the working test with:
    cd acepy
    PYTHONPATH=.:examples python examples/test_deferred_lowering.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

print("=" * 70)
print("NOTE: This test is OBSOLETE")
print("=" * 70)
print()
print("Please use test_deferred_lowering.py instead, which properly handles")
print("the deferred compilation pattern required for bootstrap lowering.")
print()
print("The issue with this test:")
print("  - It compiles the kernel BEFORE loading the model")
print("  - This causes state pollution in the shared GLOB_SCOPE")
print("  - Results in 'Valid_operator' assertion failures")
print()
print("The solution (implemented in test_deferred_lowering.py):")
print("  1. Register lowering WITHOUT compiling")
print("  2. Load model and run C++ passes")
print("  3. Python lowering pass compiles kernel LAZILY")
print()
print("Run the working test:")
print("  cd acepy")
print("  PYTHONPATH=.:examples python examples/test_deferred_lowering.py")
print()
print("=" * 70)

# Redirect to the working test
if __name__ == "__main__":
    print("\nRunning test_deferred_lowering.py instead...\n")
    exec(open(os.path.join(os.path.dirname(__file__), "test_deferred_lowering.py")).read())
