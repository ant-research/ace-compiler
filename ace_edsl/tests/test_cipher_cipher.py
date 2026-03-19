#!/usr/bin/env python3
"""
Test CIPHERTEXT + CIPHERTEXT Operations (ace_edsl)
==================================================

Tests cipher+cipher operations which generate:
- ckks.add / ckks.mul (CKKS domain ops)
- Hw_modadd (hardware modular add)
- Hw_modmul (hardware modular multiply)
- Relinearize (after cipher*cipher multiplication)
- Rescale (scale management)

This is the most expensive FHE operation type.

Uses @ckks_kernel which compiles directly to fhe::ckks domain:
  ckks → poly → C

Output files saved to: tests/output/cipher_cipher/

Run with:
    cd ace_edsl
    python tests/test_cipher_cipher.py
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

TESTS_DIR = os.path.dirname(os.path.abspath(__file__))
if TESTS_DIR not in sys.path:
    sys.path.insert(0, TESTS_DIR)

from ckks_e2e_utils import compare_with_tolerance, run_kernel_e2e

# Output directory for IR dumps
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), "output", "cipher_cipher")
os.makedirs(OUTPUT_DIR, exist_ok=True)


def save_ir(glob, phase_name, description=""):
    """Save IR dump to file."""
    ir_dump = glob.dump()
    filename = os.path.join(OUTPUT_DIR, f"{phase_name}.ir")
    with open(filename, 'w') as f:
        f.write(f"# Phase: {phase_name}\n")
        f.write(f"# Description: {description}\n")
        f.write(f"# {'='*60}\n\n")
        f.write(ir_dump)
    print(f"  → Saved IR to {filename}")
    return filename


def save_ir_text(ir_dump: str, phase_name: str, description: str = ""):
    """Save raw IR text dump to file."""
    filename = os.path.join(OUTPUT_DIR, f"{phase_name}.ir")
    with open(filename, "w") as f:
        f.write(f"# Phase: {phase_name}\n")
        f.write(f"# Description: {description}\n")
        f.write(f"# {'='*60}\n\n")
        f.write(ir_dump)
    print(f"  → Saved IR to {filename}")
    return filename


def run_test():
    print("="*60)
    print("Test: CIPHERTEXT + CIPHERTEXT (ace_edsl)")
    print("="*60)
    print(f"Output directory: {OUTPUT_DIR}")
    
    try:
        from ace_edsl.edsl import ckks_kernel, AceEDSL, AcePipeline, CkksCiphertext
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    
    # Define kernel with cipher+cipher operations
    # Using ckks_kernel which compiles directly to fhe::ckks domain
    @ckks_kernel
    def kernel_cipher_cipher(a: CkksCiphertext, b: CkksCiphertext):
        """CIPHERTEXT + CIPHERTEXT operations."""
        sum_result = a + b    # cipher + cipher -> ckks.add
        prod_result = a * b   # cipher * cipher -> ckks.mul (needs relin)
        return prod_result
    
    print("\n[Phase 1] Compiling @ckks_kernel...")
    # Trigger AIR generation
    kernel_cipher_cipher(None, None)
    
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module
    print("  ✓ Compiled")
    save_ir(glob, "01_after_compile", "After @ckks_kernel compilation - CKKS domain ops")
    
    # Create pipeline
    # Start from fhe::ckks since @ckks_kernel compiles directly to CKKS domain
    print("\n[Phase 2-4] Running CKKS pipeline (ckks_driver → poly → C)...")
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file=os.path.join(OUTPUT_DIR, "cipher_cipher_data.msg"),
    )
    
    result = pipeline.run(start_domain="fhe::ckks", verbose=False, dump_stages=True)
    
    if not result.success:
        print(f"  ✗ Pipeline failed: {result.error}")
        return False
    
    print(f"  ✓ Pipeline succeeded: {result.stages_completed}")

    # Save intermediate CKKS stage dump for relin/rescale inspection
    if result.air_dumps.get("ckks_driver"):
        save_ir_text(
            result.air_dumps["ckks_driver"],
            "02_after_ckks_driver",
            "After CKKS driver - scale/relin insertion on CKKS IR",
        )

    # Save final IR and C code
    save_ir(glob, "04_after_poly2c", "After full pipeline")
    
    c_code = result.c_code
    c_filename = os.path.join(OUTPUT_DIR, "output.c")
    with open(c_filename, 'w') as f:
        f.write(c_code)
    print(f"  → Saved C code to {c_filename} ({len(c_code)} bytes)")
    
    # Analysis
    print("\n[Analysis] C code runtime calls:")
    ops_found = []
    for op in ['Hw_modadd', 'Hw_modmul', 'Relinearize', 'Rescale']:
        count = c_code.count(op)
        if count > 0:
            ops_found.append(f"{op}:{count}")
            print(f"  ✓ {op}: {count} calls")
    
    print(f"\n  Summary: {ops_found}")

    # Validation: ensure expected CKKS behavior is present
    ckks_ir = result.air_dumps.get("ckks_driver", "")
    failures = []

    if "CKKS.mul" not in ckks_ir:
        failures.append("ckks_driver IR missing CKKS.mul")
    if "CKKS.relin" not in ckks_ir:
        failures.append("ckks_driver IR missing CKKS.relin for cipher*cipher")
    if "CKKS.rescale" not in ckks_ir:
        failures.append("ckks_driver IR missing CKKS.rescale")
    if c_code.count("Relinearize(") == 0:
        failures.append("generated C missing Relinearize runtime call")
    if c_code.count("Hw_modmul") == 0:
        failures.append("generated C missing Hw_modmul")

    if failures:
        print("\n✗ Validation failed:")
        for msg in failures:
            print(f"  - {msg}")
        return False

    print("\n[Phase 5] Running end-to-end rtlib execution...")
    input_p0 = [((i % 11) - 5) / 32.0 for i in range(64)]
    input_p1 = [((i % 13) - 6) / 27.0 for i in range(64)]
    expected = [a * b for a, b in zip(input_p0, input_p1)]
    try:
        actual = run_kernel_e2e(
            output_dir=OUTPUT_DIR,
            generated_c_path=c_filename,
            kernel_name="kernel_cipher_cipher",
            input_tensors=[("p0", input_p0), ("p1", input_p1)],
            output_len=len(expected),
        )
    except RuntimeError as e:
        print(f"  ✗ End-to-end execution failed:\n{e}")
        return False

    ok, msg = compare_with_tolerance(actual, expected, tol=5e-2)
    if not ok:
        print(f"  ✗ End-to-end numeric mismatch: {msg}")
        return False
    print(f"  ✓ End-to-end numeric check passed: {msg}")

    print("\n✓ cipher+cipher test PASSED")
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
