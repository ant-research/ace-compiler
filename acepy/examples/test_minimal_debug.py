"""Minimal test to debug parameter issue."""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

print("1. Importing...")
from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext
print("2. Defining kernel...")

@ckks_kernel
def simple_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    print("[KERNEL-BODY] simple_add body executing")
    return a + b

print("3. Kernel defined successfully")
print("4. Calling compile()...")

try:
    simple_add.compile()
    print("5. Compilation succeeded!")
except Exception as e:
    print(f"5. Compilation FAILED: {e}")
    import traceback
    traceback.print_exc()
