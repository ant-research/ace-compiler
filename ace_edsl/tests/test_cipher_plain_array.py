#!/usr/bin/env python3
"""
Test CIPHERTEXT + PLAINTEXT (constant array) Operations (ace_edsl)
==================================================================

Tests cipher+plaintext operations where the plaintext is a constant
float array (not a scalar). The @ckks_kernel decorator detects a
Python list for a CkksPlaintext-annotated parameter, creates a
CONSTANT_KIND::ARRAY in AIR, and encodes it via CKKS.encode
**without** the MASK attribute.

Expected C code generation:
- Encode_float (array pointer form), NOT Encode_float_mask (scalar form)

Uses @ckks_kernel which compiles directly to fhe::ckks domain:
  ckks → poly → C

Output files saved to: tests/output/cipher_plain_array/

Run with:
    cd ace_edsl
    python tests/test_cipher_plain_array.py
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
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), "output", "cipher_plain_array")
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
    print("Test: CIPHERTEXT + PLAINTEXT constant array (ace_edsl)")
    print("="*60)
    print(f"Output directory: {OUTPUT_DIR}")
    
    try:
        from ace_edsl.edsl import ckks_kernel, AceEDSL, AcePipeline, CkksPlaintext, CkksCiphertext
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    
    # Define kernel with cipher+plaintext operations
    # Both parameters are explicitly typed: CIPHERTEXT and PLAINTEXT
    @ckks_kernel
    def kernel_cipher_plain_array(ct: CkksCiphertext, pt: CkksPlaintext):
        """CIPHERTEXT * PLAINTEXT (constant array) operation.
        
        The plaintext is a constant float array encoded via CKKS.encode
        without the MASK attribute (array form, not scalar replicate).
        cipher*plain stays degree 1 — no Relinearize needed.
        """
        result = ct * pt
        return result
    
    print("\n[Phase 1] Compiling @ckks_kernel with constant array plaintext...")
    
    # Trigger AIR generation with actual ciphertext and constant array
    # The @ckks_kernel decorator detects the list and:
    #   1. Creates a CONSTANT_KIND::ARRAY (float32) in AIR via new_array_const
    #   2. Wraps it in CKKS.encode (no MASK attribute → Encode_float in C)
    ct = CkksCiphertext(shape=(64,), name="input_ct")
    data = [float(i) for i in range(64)]  # 64-element float array
    kernel_cipher_plain_array(ct, data)
    
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module
    print("  ✓ Compiled")
    save_ir(glob, "01_after_compile",
            "After @ckks_kernel compilation - constant array plaintext")
    
    # Create pipeline
    # Start from fhe::ckks since @ckks_kernel compiles directly to CKKS domain
    print("\n[Phase 2-4] Running CKKS pipeline (ckks_driver → poly → C)...")
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file=os.path.join(OUTPUT_DIR, "cipher_plain_array_data.msg"),
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
    print("\n[Analysis] C code for cipher+plaintext (constant array) operations:")
    ops_found = []
    for op in ['Hw_modmul', 'Encode_float', 'Encode_float_mask',
               'Pt_from_msg', 'PLAINTEXT', 'Relinearize']:
        count = c_code.count(op)
        if count > 0:
            ops_found.append(f"{op}:{count}")
            print(f"  ✓ {op}: {count} occurrences")
    
    # Key checks
    if 'Relinearize' not in c_code:
        print("  ✓ No Relinearize (expected: cipher*plain stays degree-1)")
    else:
        print("  ⚠ Relinearize found (unexpected for cipher*plain)")
    
    # Encode_float (array form) should be present OR Pt_from_msg (offline form)
    has_encode_float = 'Encode_float' in c_code
    has_pt_from_msg = 'Pt_from_msg' in c_code
    if has_encode_float or has_pt_from_msg:
        print("  ✓ Array encoding present (Encode_float or Pt_from_msg)")
    else:
        print("  ⚠ No array encoding found")
    
    # Encode_float_mask should NOT be present (that is for scalars)
    if 'Encode_float_mask' not in c_code:
        print("  ✓ No Encode_float_mask (expected: array, not scalar)")
    else:
        print("  ⚠ Encode_float_mask found (unexpected for array constant)")
    
    print(f"\n  Summary: {ops_found}")

    # Validation: array plaintext path should avoid relin and avoid mask-encode
    ckks_ir = result.air_dumps.get("ckks_driver", "")
    failures = []

    if "CKKS.mul" not in ckks_ir:
        failures.append("ckks_driver IR missing CKKS.mul")
    if "CKKS.encode" not in ckks_ir:
        failures.append("ckks_driver IR missing CKKS.encode for array plaintext")
    if "CKKS.relin" in ckks_ir:
        failures.append("ckks_driver IR unexpectedly contains CKKS.relin for cipher*plain array")
    if c_code.count("Relinearize(") != 0:
        failures.append("generated C unexpectedly contains Relinearize for cipher*plain array")
    if ("Pt_from_msg" not in c_code) and ("Encode_float" not in c_code):
        failures.append("generated C missing array plaintext encode path (Pt_from_msg/Encode_float)")
    if "Encode_float_mask" in c_code:
        failures.append("generated C unexpectedly contains Encode_float_mask for array plaintext")

    if failures:
        print("\n✗ Validation failed:")
        for msg in failures:
            print(f"  - {msg}")
        return False

    print("\n[Phase 5] Running end-to-end rtlib execution...")
    input_p0 = [((i % 10) - 4) / 25.0 for i in range(64)]
    plain_const = [float(i) for i in range(64)]
    expected = [a * b for a, b in zip(input_p0, plain_const)]
    try:
        actual = run_kernel_e2e(
            output_dir=OUTPUT_DIR,
            generated_c_path=c_filename,
            kernel_name="kernel_cipher_plain_array",
            input_tensors=[("p0", input_p0)],
            output_len=len(expected),
        )
    except RuntimeError as e:
        print(f"  ✗ End-to-end execution failed:\n{e}")
        return False

    ok, msg = compare_with_tolerance(actual, expected, tol=6e-2)
    if not ok:
        print(f"  ✗ End-to-end numeric mismatch: {msg}")
        return False
    print(f"  ✓ End-to-end numeric check passed: {msg}")

    print("\n✓ cipher+plain (constant array) test PASSED")
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
