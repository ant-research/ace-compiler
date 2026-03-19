#!/usr/bin/env python3
"""
Minimal NN lowering test: only NN.add and NN.mul ops.

This test ensures that @nn_kernel produces nn::core ops before lowering,
and runs the full pipeline through poly2c using AcePipeline.
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
    from ace_edsl.edsl import nn_kernel, AceEDSL, AcePipeline
    IMPORTS_AVAILABLE = True
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)


def _save_air_dumps(result, prefix="nn_simple"):
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


def test_nn_simple_lowering():
    if not IMPORTS_AVAILABLE:
        print("⚠ Imports not available:", IMPORT_ERROR)
        return False

    @nn_kernel
    def nn_simple_kernel(a, b):
        # Only nn::core ops: add and mul
        return a + (b * b)

    dsl = AceEDSL._get_dsl()

    # Trigger AIR generation via @jit execution
    nn_simple_kernel(None, None)
    glob = dsl.current_air_module

    # Dump initial AIR
    if glob is not None:
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, "nn_simple_air_ir_after_initial_generation_(nn_core).air")
        with open(out_path, "w") as f:
            f.write(glob.dump())
        print(f"✓ Initial AIR dump: {out_path}")

        # Basic checks for NN ops
        ir = glob.dump()
        has_nn_add = "NN.add" in ir
        has_nn_mul = "NN.mul" in ir
        print(f"\nNN.add present: {has_nn_add}")
        print(f"NN.mul present: {has_nn_mul}")
        if not (has_nn_add and has_nn_mul):
            print("⚠ Missing NN ops in AIR dump")
            return False

    # Run pipeline using AcePipeline
    print("\nRunning AcePipeline (nn::core → C code)...")
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file="test_nn_simple_data.msg",
    )

    result = pipeline.run(start_domain="nn::core", dump_stages=True, verbose=True)

    if result.success:
        # Save AIR dumps
        _save_air_dumps(result, "nn_simple")
        
        # Write C code
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        out_path = os.path.join(out_dir, "nn_simple_output.c")
        with open(out_path, "w") as f:
            f.write(result.c_code)
        print(f"✓ C code written to: {out_path}")
        return True
    else:
        print(f"✗ Pipeline failed: {result.error}")
        print(f"  Stages completed: {result.stages_completed}")
        return False


if __name__ == "__main__":
    ok = test_nn_simple_lowering()
    sys.exit(0 if ok else 1)
