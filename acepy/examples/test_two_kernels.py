"""Test with two kernels to reproduce the issue."""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

print("1. Importing...")
from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext
print("2. OK")

print("3. Defining first kernel...")
@ckks_kernel
def test_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    return a + b
print("4. First kernel defined")

print("5. Compiling first kernel...")
try:
    test_add.compile()
    print("6. First kernel compiled successfully!")
except Exception as e:
    print(f"6. FAILED: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

print("7. Defining second kernel...")
@ckks_kernel
def test_sub(x: CkksCiphertext, y: CkksCiphertext) -> CkksCiphertext:
    return x - y
print("8. Second kernel defined")

print("9. Compiling second kernel...")
try:
    test_sub.compile()
    print("10. Second kernel compiled successfully!")
except Exception as e:
    print(f"10. FAILED: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

print("11. ALL TESTS PASSED!")
