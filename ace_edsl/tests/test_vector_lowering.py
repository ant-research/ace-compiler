#!/usr/bin/env python3
"""
Vector Lowering Test (ACE EDSL)

Starts at nn::vector and runs:
  vector2sihe -> ckks_driver -> poly_driver -> poly2c

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
    from ace_edsl.edsl import vector_kernel, AceEDSL, AcePipeline
    from ace_edsl.edsl.core.types import VectorTensor
    IMPORTS_AVAILABLE = True
    IMPORT_ERROR = None
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)


def _save_air_dumps(result, prefix="vector"):
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


def test_vector_lowering():
    if not IMPORTS_AVAILABLE:
        print("⚠ Imports not available:", IMPORT_ERROR)
        return False

    @vector_kernel
    def vec_simple_kernel(a: VectorTensor, b: VectorTensor) -> VectorTensor:
        # Only nn::vector ops: VECTOR.add / VECTOR.mul
        return a + (b * b)

    dsl = AceEDSL._get_dsl()

    # Instantiate VectorTensor arguments (matches DSL expectations)
    a = VectorTensor[float, 64]
    b = VectorTensor[float, 64]
    
    # Trigger AIR generation via @jit execution
    vec_simple_kernel(a, b)
    glob = dsl.current_air_module

    # Dump initial AIR
    if glob is not None:
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, "vector_air_ir_after_initial_generation_(nn_vector).air")
        with open(out_path, "w") as f:
            f.write(glob.dump())
        print(f"✓ Initial AIR dump: {out_path}")

    if glob is None:
        print("⚠ AIR module not available")
        return False

    # Run pipeline using AcePipeline (start at nn::vector, skip tensor2vector)
    print("\nRunning AcePipeline (nn::vector → C code)...")
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file="test_vector_data.msg",
    )

    result = pipeline.run(start_domain="nn::vector", dump_stages=True, verbose=True)

    if result.success:
        # Save AIR dumps
        _save_air_dumps(result, "vector")
        
        # Write C code
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        out_path = os.path.join(out_dir, "vector_output.c")
        with open(out_path, "w") as f:
            f.write(result.c_code)
        print(f"✓ C code written to: {out_path}")
        return True
    else:
        print(f"✗ Pipeline failed: {result.error}")
        print(f"  Stages completed: {result.stages_completed}")
        return False


if __name__ == "__main__":
    ok = test_vector_lowering()
    sys.exit(0 if ok else 1)
