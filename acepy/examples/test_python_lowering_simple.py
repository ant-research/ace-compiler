"""
Simple Python Lowering Test with Pipeline
==========================================

Tests the Python lowering with a simple kernel that compiles correctly.
Uses the Pipeline class for clean, chainable API.

Output files saved to: examples/output/python_lowering_simple/
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks
from dump_utils import Pipeline, PipelineTarget


def count_ops(ir: str, pattern: str) -> int:
    """Count occurrences of an operation in IR."""
    return ir.lower().count(pattern.lower())


# =============================================================================
# Define custom bootstrap kernel (at module level for AST inspection)
# =============================================================================

@ckks_kernel
def custom_bootstrap(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """Custom bootstrap with rotation and arithmetic"""
    # DFT phase
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    
    # Polynomial approximation
    x2 = dft1 * dft1
    result = dft1 - x2
    
    # iDFT phase
    idft_rot = ckks.rotate(result, 1)
    output = result + idft_rot
    
    return output


def run_test():
    print("=" * 70)
    print("Simple Python Lowering Test with Pipeline")
    print("=" * 70)
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    
    if not os.path.exists(model_path):
        print(f"ERROR: Model not found at {model_path}")
        return False
    
    # =========================================================================
    # Step 1: Track ops at each phase
    # =========================================================================
    phase_ops = {}
    
    def track_ops(phase, ir):
        phase_ops[phase] = {
            'ckks.bootstrap': count_ops(ir, 'ckks.bootstrap'),
            'ckks.rotate': count_ops(ir, 'ckks.rotate'),
            'ckks.add': count_ops(ir, 'ckks.add'),
            'ckks.mul': count_ops(ir, 'ckks.mul'),
        }
    
    # =========================================================================
    # Step 2: Run pipeline up to CKKS level with skip_ops
    # =========================================================================
    print("\n[Step 1] Running pipeline to CKKS level with skip_ops...")
    
    pipeline = Pipeline(
        "python_lowering_simple",
        verbose=True,
        on_phase_complete=track_ops,
    )
    
    result = (pipeline
        .load_onnx(model_path)
        .configure_fhe(
            scaling_factor_bits=56,
            first_prime_bits=60,
            hamming_weight=192,
            conv_fast=True,
            gemm_fast=False,
            relu_vr_def=3.0,
        )
        .set_skip_ops(["fhe::sihe::bootstrap"])  # Preserve bootstrap for Python lowering
        .run(target=PipelineTarget.CKKS)
    )
    
    if not result.success:
        print(f"\n✗ Pipeline failed: {result.error}")
        return False
    
    # =========================================================================
    # Step 3: Compile the custom lowering kernel (AFTER passes)
    # =========================================================================
    print("\n[Step 2] Compiling custom bootstrap kernel...")
    
    try:
        custom_bootstrap.compile()
        lowering_glob = custom_bootstrap.air_module
        lowering_ir = lowering_glob.dump()
        
        print(f"  ✓ Compiled successfully!")
        print(f"  Full IR size: {len(lowering_ir):,} chars")
        
        # Count ops in custom_bootstrap function only
        func_ir = ""
        in_func = False
        for line in lowering_ir.split('\n'):
            if 'custom_bootstrap' in line:
                in_func = True
            if in_func:
                func_ir += line + '\n'
                if 'end_block' in line:
                    break
        
        print(f"  Operations in custom_bootstrap:")
        print(f"    CKKS.rotate: {count_ops(func_ir, 'ckks.rotate')}")
        print(f"    CKKS.mul:    {count_ops(func_ir, 'ckks.mul')}")
        print(f"    CKKS.add:    {count_ops(func_ir, 'ckks.add')}")
        print(f"    CKKS.sub:    {count_ops(func_ir, 'ckks.sub')}")
        
    except Exception as e:
        print(f"  ✗ Compilation failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    # =========================================================================
    # Summary
    # =========================================================================
    pipeline.print_summary()
    
    sihe2ckks_ops = phase_ops.get("sihe2ckks", {})
    
    print(f"""
  Model IR state after sihe2ckks:
    CKKS.bootstrap: {sihe2ckks_ops.get('ckks.bootstrap', 0)} (preserved by skip_ops)
    CKKS.rotate:    {sihe2ckks_ops.get('ckks.rotate', 0)}
    CKKS.add:       {sihe2ckks_ops.get('ckks.add', 0)}
    CKKS.mul:       {sihe2ckks_ops.get('ckks.mul', 0)}
    
  Custom lowering kernel compiled: ✓
    Contains CKKS operations for DFT/EvalMod/iDFT
    
  Pipeline phases completed: {result.phases_completed}
  Total time: {result.total_time:.2f}s
""")
    
    print("=" * 70)
    print("✓ Test completed successfully!")
    print("=" * 70)
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
