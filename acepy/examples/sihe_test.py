"""Test SIHE kernel"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.stdout.flush()

print("Before imports", flush=True)
from ace_dsl.frontend.domain_kernels import sihe_kernel, SiheCiphertext
from ace_bindings import air_builder
print("After imports", flush=True)


@sihe_kernel
def simple_mul(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Simple SIHE multiply."""
    return a * b

print("Kernel defined", flush=True)

print("Compiling...", flush=True)
simple_mul.compile()
print("Compiled!", flush=True)

glob = simple_mul.air_module

print("\nIR:", flush=True)
print(glob.dump(), flush=True)

print("\nRunning ckks_driver...", flush=True)
result = air_builder.run_ckks_driver(glob)
print(f"Success: {result['success']}", flush=True)
print(f"Message: {result['message']}", flush=True)
if 'debug' in result:
    print(f"Debug:\n{result['debug']}", flush=True)
