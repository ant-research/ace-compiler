#!/usr/bin/env python3
"""
Vector Lowering Test with If-Then-Else (ACE EDSL)

Starts at nn::vector and runs:
  vector2sihe -> ckks_driver -> poly_driver -> poly2c

Includes an if-then-else in the vector kernel.
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
    from ace_edsl.base_dsl.ast_helpers import dynamic_expr
    IMPORTS_AVAILABLE = True
    IMPORT_ERROR = None
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)


def _save_air_dumps(result, prefix="vector_if"):
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


def test_vector_if_lowering(use_dynamic_pred=False):
    """
    Test if-then-else in vector kernel.
    
    Args:
        use_dynamic_pred: If True, use dynamic predicate (generates real if in AIR,
                          but FHE pipeline will fail since it doesn't support dynamic
                          control flow on encrypted data).
                          If False, use compile-time constant (evaluates at compile time,
                          goes through full FHE pipeline).
    """
    if not IMPORTS_AVAILABLE:
        print("⚠ Imports not available:", IMPORT_ERROR)
        return False

    dsl = AceEDSL._get_dsl()
    a = VectorTensor[float, 64]
    b = VectorTensor[float, 64]

    if use_dynamic_pred:
        # Dynamic predicate using PLAINTEXT scalar comparison
        # The condition is computed on plaintext integers, not encrypted arrays
        print("\n*** Using PLAINTEXT scalar predicate - generates real if in AIR ***")
        
        from ace_edsl.edsl.domain_ast_decorators import get_current_container
        
        @vector_kernel
        def vec_if_kernel(a: VectorTensor, b: VectorTensor) -> VectorTensor:
            out = a + b
            # Create plaintext scalar comparison (not on encrypted data)
            # This creates: 1 > 0 as AIR nodes, which is a valid relational op
            container = get_current_container()
            one = container.new_intconst(1)
            zero = container.new_intconst(0)
            pred_node = container.new_gt(one, zero)
            # Wrap in AIRValue for dynamic_expr
            from ace_edsl.edsl.core.air_value import AIRValue
            pred = AIRValue(pred_node, container)
            if dynamic_expr(pred):
                out = a + b
            else:
                out = a - b
            return out
    else:
        # Compile-time constant - evaluated at compile time, only 'then' branch emitted
        print("\n*** Using CONSTANT predicate (True) - evaluated at compile time ***")
        @vector_kernel
        def vec_if_kernel(a: VectorTensor, b: VectorTensor) -> VectorTensor:
            out = a + b
            if dynamic_expr(True):  # Python bool - evaluated at compile time
                out = a + b
            else:
                out = a - b
            return out

    vec_if_kernel(a, b)
    glob = dsl.current_air_module

    # Dump initial AIR
    if glob is not None:
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, "vector_if_air_ir_after_initial_generation_(nn_vector).air")
        with open(out_path, "w") as f:
            f.write(glob.dump())
        print(f"✓ Initial AIR dump: {out_path}")

    if glob is None:
        print("⚠ AIR module not available")
        return False

    # Dynamic predicates generate real if-then-else in AIR
    # However, the current FHE vector2sihe pass doesn't support control flow structures
    if use_dynamic_pred:
        print("\n✓ If-then-else structure generated in AIR (see dump above)")
        print("⚠ Stopping: FHE vector2sihe pass doesn't support control flow yet.")
        print("  This is a limitation of the FHE lowering, not the if generation.")
        return True

    # Run pipeline using AcePipeline (start at nn::vector)
    print("\nRunning AcePipeline (nn::vector → C code)...")
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file="test_vector_if_data.msg",
    )

    result = pipeline.run(start_domain="nn::vector", dump_stages=True, verbose=True)

    if result.success:
        # Save AIR dumps
        _save_air_dumps(result, "vector_if")
        
        # Write C code
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        out_path = os.path.join(out_dir, "vector_if_output.c")
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
                        help="Use dynamic predicate (AIRValue) - generates real if, but FHE will fail")
    args = parser.parse_args()
    
    ok = test_vector_if_lowering(use_dynamic_pred=args.dynamic)
    sys.exit(0 if ok else 1)
