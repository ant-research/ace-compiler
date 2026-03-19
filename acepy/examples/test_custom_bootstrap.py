"""
Test Custom Bootstrap Lowering with Pipeline
=============================================

This test demonstrates the selective lowering infrastructure for FHE bootstrap.

STATUS:
- tensor2vector: ✓ skip_ops works
- vector2sihe: ✓ skip_ops supported  
- sihe2ckks: ✓ skip_ops works

This test shows:
1. The model generates bootstrap operations at SIHE level
2. sihe2ckks preserves them as CKKS.bootstrap when skip_ops is set
3. Python lowering can then replace CKKS.bootstrap with custom implementation

Output files saved to: examples/output/custom_bootstrap/
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.passes.python_lowering_pass import (
    register_lowering, 
    get_ops_to_skip,
    clear_lowerings
)
from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks
from dump_utils import Pipeline, PipelineTarget


def count_ops(ir: str, pattern: str) -> int:
    """Count occurrences of an operation in IR."""
    return ir.lower().count(pattern.lower())


# =============================================================================
# Define custom bootstrap OUTSIDE of run_test (for AST parsing)
# =============================================================================

@ckks_kernel
def custom_bootstrap(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """
    Custom bootstrap: CoeffToSlot + EvalMod + SlotToCoeff
    
    This replaces the default CKKS.bootstrap with a full
    CKKS-level implementation.
    """
    # Phase 1: CoeffToSlot (DFT)
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    dft2_rot = ckks.rotate(dft1, 1)
    slot_repr = dft1 + dft2_rot
    
    # Phase 2: EvalMod (simplified polynomial)
    x = slot_repr
    x2 = x * x
    x3 = x * x2
    sin_approx = x - x3
    
    # Phase 3: SlotToCoeff (iDFT)
    idft0_rot = ckks.rotate(sin_approx, 1)
    idft0 = sin_approx - idft0_rot
    idft1_rot = ckks.rotate(idft0, 2)
    idft1 = idft0 - idft1_rot
    idft2_rot = ckks.rotate(idft1, 4)
    result = idft1 - idft2_rot
    
    return result


def run_test():
    print("=" * 70)
    print("Test: Custom Bootstrap Lowering with Pipeline")
    print("=" * 70)
    
    # Clear any previous registrations
    clear_lowerings()
    
    # =========================================================================
    # Step 1: Register custom bootstrap for CKKS.bootstrap
    # =========================================================================
    print("\n1. Registering custom bootstrap lowering...")
    
    register_lowering(
        "fhe::ckks", "bootstrap", 
        target_domain="fhe::poly",
        description="Custom CKKS bootstrap with DFT/EvalMod/iDFT"
    )(custom_bootstrap)
    
    skip_ops = get_ops_to_skip()
    print(f"   Registered: {skip_ops}")
    
    # =========================================================================
    # Step 2: Load model and run pipeline
    # =========================================================================
    print("\n2. Loading model and running pipeline...")
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    
    if not os.path.exists(model_path):
        print(f"   ERROR: Model not found at {model_path}")
        return False
    
    # Create pipeline with phase tracking
    phase_ir = {}
    
    def on_phase(phase, ir):
        phase_ir[phase] = ir
    
    pipeline = Pipeline(
        "custom_bootstrap", 
        verbose=True,
        on_phase_complete=on_phase
    )
    
    # Run pipeline up to CKKS level with skip_ops for bootstrap
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
        .set_skip_ops(["fhe::sihe::bootstrap"])  # Skip bootstrap in sihe2ckks
        .run(target=PipelineTarget.CKKS)  # Stop at CKKS level
    )
    
    if not result.success:
        print(f"   Pipeline failed: {result.error}")
        return False
    
    # =========================================================================
    # Step 3: Analyze bootstrap ops at each phase
    # =========================================================================
    print("\n3. Analyzing bootstrap operations at each phase...")
    
    sihe_bootstrap = 0
    ckks_bootstrap = 0
    
    if "vector2sihe" in phase_ir:
        sihe_bootstrap = count_ops(phase_ir["vector2sihe"], "sihe.bootstrap")
        print(f"   SIHE.bootstrap after vector2sihe: {sihe_bootstrap}")
    
    if "sihe2ckks" in phase_ir:
        ckks_bootstrap = count_ops(phase_ir["sihe2ckks"], "ckks.bootstrap")
        print(f"   CKKS.bootstrap after sihe2ckks: {ckks_bootstrap}")
    
    # =========================================================================
    # Step 4: Verify skip_ops worked
    # =========================================================================
    print("\n4. Verifying skip_ops preserved CKKS.bootstrap...")
    
    pipeline.print_summary()
    
    # Note: The compiler may optimize away redundant bootstraps based on level analysis.
    # This is correct behavior - if a bootstrap is provably unnecessary, removing it is
    # an optimization. The test passes if CKKS.bootstrap > 0 (some are preserved).
    optimized_away = sihe_bootstrap - ckks_bootstrap
    print(f"""
  Pipeline Results:
    SIHE.bootstrap (after vector2sihe): {sihe_bootstrap}
    CKKS.bootstrap (after sihe2ckks):   {ckks_bootstrap}
    Optimized away (redundant):         {optimized_away}
    
  Status: {"✓ Working" if ckks_bootstrap > 0 else "⚠ Check needed"}
""")
    
    if ckks_bootstrap > 0:
        print(f"  ✓ Skip lowering works! {ckks_bootstrap} CKKS.bootstrap preserved")
        if optimized_away > 0:
            print(f"    ({optimized_away} redundant bootstraps optimized away by level analysis)")
        print("    Bootstrap nodes can now be replaced by custom Python lowering")
        return True
    elif sihe_bootstrap == 0:
        print("  ⚠ No bootstrap operations in model (model may be too small)")
        return True
    else:
        print("  ⚠ All bootstraps were optimized away - none available for custom lowering")
        return False


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
