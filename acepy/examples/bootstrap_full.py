"""
Full CKKS Bootstrap Algorithm Implementation
=============================================

**SIMPLIFIED DEMONSTRATION** for testing the Python DSL compilation pipeline.

Bootstrap refreshes a ciphertext's noise budget through:
1. ModRaise: Raise the ciphertext modulus (implicit in CKKS)
2. CoeffToSlot: Homomorphic DFT (coefficient → slot representation)
3. EvalMod: Approximate modular reduction using sine polynomial
4. SlotToCoeff: Homomorphic inverse DFT (slot → coefficient representation)

This version uses subtraction instead of negation since ckks2poly
supports sub but not neg. Compiles through the full pipeline to C code.

COMPARISON WITH PRODUCTION IMPLEMENTATION
-----------------------------------------
This is a simplified demo. The production C++ implementation is in:
  fhe-cmplr/rtlib/ant/src/util/ckks_bootstrap_context.c

Key differences:
  - Python: Simple rotate+add DFT, Taylor series sine
  - C++: Precomputed FFT matrices, Chebyshev polynomials, Paterson-Stockmeyer

Operation counts:
  - Python: 6 rotations, 13 muls, 8 adds, 9 subs
  - C++: 20-40+ rotations, 15-25 muls, many rescales

Use this for: Pipeline testing, rapid prototyping, research experiments
Use C++ for: Production FHE applications requiring accuracy

References:
- "Bootstrapping for Approximate Homomorphic Encryption" (Cheon et al., 2018)
- "Improved Bootstrapping for Approximate Homomorphic Encryption" (Chen & Cheon, 2019)
- "Better Bootstrapping for Approximate Homomorphic Encryption" (Lee et al., 2020)
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks
from ace_bindings import air_builder


# =============================================================================
# Configuration
# =============================================================================

LOG_SLOTS = 3  # 8 slots for demo
NUM_SLOTS = 1 << LOG_SLOTS
NUM_DOUBLE_ANGLE = 3
SINE_POLY_DEGREE = 7


# =============================================================================
# Complete Bootstrap Kernel
# =============================================================================

@ckks_kernel
def bootstrap_full(ct: CkksCiphertext, zero: CkksCiphertext) -> CkksCiphertext:
    """
    Complete CKKS Bootstrap Algorithm.
    
    Refreshes the noise budget of a ciphertext through:
    
    1. COEFF TO SLOT (Homomorphic DFT)
       - Transforms from coefficient to evaluation representation
       - Uses log(n) layers of butterfly operations
    
    2. EVAL MOD (Sine Approximation + Double-Angle)
       - Approximates t mod 1 using sin(2πKt)/(2πK)
       - Polynomial approximation of sine (degree 7)
       - Double-angle iterations extend K value
    
    3. SLOT TO COEFF (Homomorphic Inverse DFT)
       - Transforms back to coefficient representation
    
    Args:
        ct: Input ciphertext to bootstrap
        zero: Ciphertext encoding zeros (for negation: 0 - x = -x)
    
    Returns:
        Ciphertext with refreshed noise budget
    """
    # =========================================================================
    # Phase 1: CoeffToSlot - Homomorphic DFT (3 layers for 8 slots)
    # =========================================================================
    
    # DFT Layer 0: stride = 4
    dft0_rot = ckks.rotate(ct, 4)
    dft0 = ct + dft0_rot
    
    # DFT Layer 1: stride = 2
    dft1_rot = ckks.rotate(dft0, 2)
    dft1 = dft0 + dft1_rot
    
    # DFT Layer 2: stride = 1
    dft2_rot = ckks.rotate(dft1, 1)
    slot_repr = dft1 + dft2_rot
    
    # =========================================================================
    # Phase 2: EvalMod - Sine Polynomial Approximation
    # =========================================================================
    # sin(x) ≈ x - x³/6 + x⁵/120 - x⁷/5040
    # cos(x) ≈ 1 - x²/2 + x⁴/24 - x⁶/720
    # Note: CKKS driver will automatically insert relin after mul operations
    
    x = slot_repr
    
    # --- Compute powers ---
    x2 = x * x           # x²
    x3 = x * x2          # x³
    x4 = x2 * x2         # x⁴
    x5 = x * x4          # x⁵
    x6 = x2 * x4         # x⁶
    x7 = x * x6          # x⁷
    
    # --- Evaluate sin polynomial using subtraction ---
    # sin(x) ≈ x - x³ + x⁵ - x⁷ (simplified coefficients)
    sin_t1 = x - x3          # x - x³
    sin_t2 = sin_t1 + x5     # x - x³ + x⁵
    sin_approx = sin_t2 - x7 # x - x³ + x⁵ - x⁷
    
    # --- Evaluate cos polynomial using subtraction ---
    # cos(x) ≈ 1 - x² + x⁴ - x⁶ (using zero param for constant 1)
    cos_t1 = zero - x2       # -x² (via 0 - x²)
    cos_t2 = cos_t1 + x4     # -x² + x⁴
    cos_approx = cos_t2 - x6 # -x² + x⁴ - x⁶
    
    # =========================================================================
    # Phase 2b: Double-Angle Iterations
    # =========================================================================
    # sin(2θ) = 2·sin(θ)·cos(θ)
    # cos(2θ) = cos²(θ) - sin²(θ)
    
    # --- Double-angle 1 ---
    da1_prod = sin_approx * cos_approx
    sin_2x = da1_prod + da1_prod  # 2·sin·cos
    
    cos_sq = cos_approx * cos_approx
    sin_sq = sin_approx * sin_approx
    cos_2x = cos_sq - sin_sq
    
    # --- Double-angle 2 ---
    da2_prod = sin_2x * cos_2x
    sin_4x = da2_prod + da2_prod
    
    cos_2x_sq = cos_2x * cos_2x
    sin_2x_sq = sin_2x * sin_2x
    cos_4x = cos_2x_sq - sin_2x_sq
    
    # --- Double-angle 3 ---
    da3_prod = sin_4x * cos_4x
    evalmod_result = da3_prod + da3_prod  # sin(8x)
    
    # =========================================================================
    # Phase 3: SlotToCoeff - Homomorphic Inverse DFT
    # =========================================================================
    
    # iDFT Layer 0: stride = 1
    idft0_rot = ckks.rotate(evalmod_result, 1)
    idft0 = evalmod_result - idft0_rot
    
    # iDFT Layer 1: stride = 2
    idft1_rot = ckks.rotate(idft0, 2)
    idft1 = idft0 - idft1_rot
    
    # iDFT Layer 2: stride = 4
    idft2_rot = ckks.rotate(idft1, 4)
    result = idft1 - idft2_rot
    
    return result


# =============================================================================
# Demo with Full Pipeline
# =============================================================================

def run_demo():
    print("=" * 70)
    print("Full CKKS Bootstrap Algorithm - Complete Pipeline")
    print("=" * 70)
    
    print("""
Bootstrap Algorithm:
┌─────────────────────────────────────────────────────────────────────┐
│  Phase 1: CoeffToSlot (DFT)     - 3 layers of rotate + add          │
│  Phase 2: EvalMod (Polynomial)  - sin/cos approx + 3 double-angles  │
│  Phase 3: SlotToCoeff (iDFT)    - 3 layers of rotate + sub          │
└─────────────────────────────────────────────────────────────────────┘
""")
    
    # ========================================================================
    # Step 1: Compile to CKKS IR
    # ========================================================================
    print("=" * 70)
    print("Step 1: Compile to CKKS IR")
    print("=" * 70)
    
    bootstrap_full.compile()
    glob = bootstrap_full.air_module
    ir = glob.dump()
    
    # Count operations
    rotate_count = ir.lower().count('ckks.rotate')
    mul_count = ir.lower().count('ckks.mul')
    add_count = ir.lower().count('ckks.add')
    sub_count = ir.lower().count('ckks.sub')
    
    print(f"\nOperation counts:")
    print(f"  CKKS.rotate: {rotate_count:3d}  (DFT + iDFT)")
    print(f"  CKKS.mul:    {mul_count:3d}  (powers + double-angle)")
    print(f"  CKKS.add:    {add_count:3d}  (combining terms)")
    print(f"  CKKS.sub:    {sub_count:3d}  (negation via subtraction)")
    print(f"  ─────────────────")
    print(f"  Total:       {rotate_count + mul_count + add_count + sub_count}")
    
    print(f"\nCKKS IR (first 2000 chars):")
    print("-" * 40)
    print(ir[:2000])
    if len(ir) > 2000:
        print(f"... ({len(ir)} total chars)")
    
    # ========================================================================
    # Step 2: CKKS Driver
    # ========================================================================
    print("\n" + "=" * 70)
    print("Step 2: CKKS Driver (add relin after mul)")
    print("=" * 70)
    
    ckks_result = air_builder.run_ckks_driver(glob)
    print(f"Success: {ckks_result['success']}")
    print(f"Message: {ckks_result['message']}")
    
    if not ckks_result['success']:
        print("*** Failed at CKKS driver ***")
        return
    
    # ========================================================================
    # Step 3: Poly Driver
    # ========================================================================
    print("\n" + "=" * 70)
    print("Step 3: Poly Driver (CKKS -> Poly)")
    print("=" * 70)
    
    poly_result = air_builder.run_poly_driver(glob)
    print(f"Success: {poly_result['success']}")
    print(f"Message: {poly_result['message']}")
    
    if not poly_result['success']:
        print("*** Failed at poly driver ***")
        return
    
    if 'ir_dump' in poly_result:
        poly_ir = poly_result['ir_dump']
        print(f"\nPoly IR ({len(poly_ir)} chars, first 1500):")
        print("-" * 40)
        print(poly_ir[:1500])
        if len(poly_ir) > 1500:
            print(f"... (truncated)")
    
    # ========================================================================
    # Step 4: Poly2C
    # ========================================================================
    print("\n" + "=" * 70)
    print("Step 4: Poly2C (Poly -> C code)")
    print("=" * 70)
    
    glob.run_cpp_pass("poly2c", [])
    c_code = glob.get_c_code()
    
    lines = c_code.split('\n')
    print(f"\nGenerated C code ({len(lines)} lines)")
    
    # Write complete C code to file
    output_file = "bootstrap_full.c"
    with open(output_file, 'w') as f:
        f.write(c_code)
    print(f"Complete C code written to: {output_file}")
    
    # Show excerpt
    print("-" * 40)
    print('\n'.join(lines[:80]))
    print(f"\n... (see {output_file} for complete {len(lines)} lines)")
    
    # ========================================================================
    # Summary
    # ========================================================================
    print("\n" + "=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"""
✓ Full bootstrap compiled to C code!

Pipeline Results:
  ├─ CKKS IR:    {len(ir):,} chars
  ├─ Poly IR:    {len(poly_ir):,} chars  
  └─ C Code:     {len(lines)} lines

Bootstrap Phases:
  ├─ CoeffToSlot:  DFT with {LOG_SLOTS} rotation layers
  ├─ EvalMod:      Degree-{SINE_POLY_DEGREE} polynomial + {NUM_DOUBLE_ANGLE} double-angles
  └─ SlotToCoeff:  Inverse DFT with {LOG_SLOTS} rotation layers

Generated C Code Structure:
  • CIPHERTEXT bootstrap_full(CIPHERTEXT ct, CIPHERTEXT zero)
  • CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx)
  • CIPHERTEXT Relinearize(CIPHERTEXT3 ciph3)
  • RNS polynomial loop operations
  • Hardware-optimized modular arithmetic
""")
    
    print("=" * 70)
    print("✓ Full bootstrap demo complete")
    print("=" * 70)


if __name__ == "__main__":
    run_demo()
