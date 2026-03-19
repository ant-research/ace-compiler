#!/usr/bin/env python3
"""
SIHE Lowering Test with If-Then-Else (ACE EDSL)

Starts at fhe::sihe level (skips tensor2vector and vector2sihe) and runs:
  ckks_driver -> poly_driver -> poly2c

Includes an if-then-else with plaintext condition.
Uses AcePipeline for the lowering stages.
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


try:
    from ace_edsl.edsl import sihe_kernel, AceEDSL, AcePipeline
    from ace_edsl.edsl.core.types import SiheCiphertext, Int
    from ace_edsl.base_dsl.ast_helpers import dynamic_expr
    IMPORTS_AVAILABLE = True
    IMPORT_ERROR = None
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)


def _save_air_dumps(result, prefix="sihe_if"):
    """Save AIR dumps from pipeline result to files."""
    out_dir = os.path.join(os.path.dirname(__file__), "output")
    os.makedirs(out_dir, exist_ok=True)
    
    stage_names = {
        "tensor2vector": "after_tensor2vector_(nn_vector)",
        "vector2sihe": "after_vector2sihe_(fhe_sihe)",
        "ckks_driver": "after_ckks_driver_(fhe_ckks)",
        "poly_driver": "after_poly_driver_(fhe_poly)",
        "poly2c": "after_poly2c_(final)",
    }
    
    for stage, dump in result.air_dumps.items():
        name = stage_names.get(stage, stage)
        out_path = os.path.join(out_dir, f"{prefix}_air_ir_{name}.air")
        with open(out_path, "w") as f:
            f.write(dump)
        print(f"  ✓ AIR dump: {out_path}")


def test_sihe_if_lowering(use_dynamic_pred=False):
    """
    Test if-then-else at SIHE level with plaintext condition.
    
    Uses @sihe_kernel decorator which generates SIHE.add/SIHE.sub operations.
    Then runs: ckks_driver -> poly_driver -> poly2c (skips tensor2vector and vector2sihe)
    """
    if not IMPORTS_AVAILABLE:
        print("⚠ Imports not available:", IMPORT_ERROR)
        return False

    dsl = AceEDSL._get_dsl()
    
    # Use SiheCiphertext for SIHE domain inputs
    a = SiheCiphertext[float, 64]
    b = SiheCiphertext[float, 64]

    if use_dynamic_pred:
        # Dynamic predicate using scalar parameter comparison
        # Int parameter becomes AIRValue with comparison operators (__gt__, etc.)
        print("\n*** Using SCALAR PARAMETER predicate - generates real if in AIR ***")
        
        @sihe_kernel
        def sihe_if_kernel(a: SiheCiphertext, b: SiheCiphertext, flag: Int) -> SiheCiphertext:
            out = a + b
            # flag is an AIRValue, so flag > 0 uses AIRValue.__gt__
            # which generates an AIR comparison node
            if dynamic_expr(flag > 0):
                out = a + b
            else:
                out = a - b
            return out
    else:
        # Compile-time constant - evaluated at compile time, only 'then' branch emitted
        print("\n*** Using CONSTANT predicate (True) - evaluated at compile time ***")
        @sihe_kernel
        def sihe_if_kernel(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
            out = a + b
            if dynamic_expr(True):  # Python bool - evaluated at compile time
                out = a + b
            else:
                out = a - b
            return out

    # Call the kernel - for dynamic predicate, we pass a flag value
    if use_dynamic_pred:
        sihe_if_kernel(a, b, 1)  # flag=1 (positive, so condition is true)
    else:
        sihe_if_kernel(a, b)
    glob = dsl.current_air_module

    # Dump initial AIR
    if glob is not None:
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, "sihe_if_air_ir_after_initial_generation_(fhe_sihe).air")
        with open(out_path, "w") as f:
            f.write(glob.dump())
        print(f"✓ Initial AIR dump: {out_path}")

    if glob is None:
        print("⚠ AIR module not available")
        return False

    # Dynamic predicates generate real if-then-else in AIR
    if use_dynamic_pred:
        print("\n✓ If-then-else structure generated in AIR (see dump above)")
        print("  Continuing with CKKS driver (skipping tensor2vector and vector2sihe)...")

    # Run pipeline using AcePipeline (start at fhe::sihe, skip tensor2vector and vector2sihe)
    print("\nRunning AcePipeline (fhe::sihe → C code)...")
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file="test_sihe_if_data.msg",
    )

    result = pipeline.run(start_domain="fhe::sihe", dump_stages=True, verbose=True)

    if result.success:
        # Save AIR dumps
        _save_air_dumps(result, "sihe_if")
        
        # Write C code
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        out_path = os.path.join(out_dir, "sihe_if_output.c")
        with open(out_path, "w") as f:
            f.write(result.c_code)
        print(f"✓ C code written to: {out_path}")
        return True
    else:
        print(f"✗ Pipeline failed: {result.error}")
        print(f"  Stages completed: {result.stages_completed}")
        return False


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--dynamic", action="store_true",
                        help="Use dynamic predicate - generates real if in AIR")
    args = parser.parse_args()
    
    ok = test_sihe_if_lowering(use_dynamic_pred=args.dynamic)
    sys.exit(0 if ok else 1)
