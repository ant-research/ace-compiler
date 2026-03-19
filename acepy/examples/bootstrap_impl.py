"""
CKKS Bootstrap Implementation - Polynomial Evaluation
======================================================

This demonstrates the EvalMod (polynomial evaluation) phase of bootstrap
using operations supported by the ckks2poly pass.

The full bootstrap algorithm has three phases:
1. CoeffToSlot: Homomorphic DFT (requires rotate - not yet in ckks2poly)
2. EvalMod: Polynomial approximation of sin(2πx) - SUPPORTED
3. SlotToCoeff: Homomorphic inverse DFT (requires rotate - not yet in ckks2poly)

This file implements Phase 2 (EvalMod) which compiles through the full pipeline.

Operations supported by ckks2poly:
  ✓ CKKS.add
  ✓ CKKS.sub  
  ✓ CKKS.mul
  ✓ CKKS.relin

Operations NOT YET supported (would need expansion):
  ✗ CKKS.rotate  (needs galois key application)
  ✗ CKKS.neg     (could be 0 - x)
  ✗ CKKS.rescale (explicit - normally auto-inserted)

References:
- "Bootstrapping for Approximate Homomorphic Encryption" (Cheon et al., 2018)
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext
from ace_bindings import air_builder


@ckks_kernel
def eval_mod_polynomial(x: CkksCiphertext, 
                        c1: CkksCiphertext,
                        c3: CkksCiphertext,
                        c5: CkksCiphertext) -> CkksCiphertext:
    """
    EvalMod: Polynomial approximation of sin(2πx)/(2π)
    
    The modular reduction in CKKS bootstrap is approximated by:
        sin(2πx)/(2π) ≈ x - (2π)²x³/6 + (2π)⁴x⁵/120 - ...
    
    For simplicity, we use:
        p(x) = c1·x + c3·x³ + c5·x⁵
    
    where c1, c3, c5 are encoded Chebyshev coefficients.
    
    This is the core computation of EvalMod that allows bootstrap
    to remove modular "error" from the encrypted value.
    
    Args:
        x: Input ciphertext (scaled to appropriate range)
        c1: Coefficient for linear term (encoded plaintext)
        c3: Coefficient for cubic term (encoded plaintext)
        c5: Coefficient for quintic term (encoded plaintext)
    
    Returns:
        Approximation of sin(2πx)/(2π)
    """
    # Compute powers of x
    # x² = x * x
    x2 = x * x
    
    # x³ = x * x²
    x3 = x * x2
    
    # x⁴ = x² * x²
    x4 = x2 * x2
    
    # x⁵ = x * x⁴
    x5 = x * x4
    
    # Polynomial evaluation: p(x) = c1·x + c3·x³ + c5·x⁵
    # Term 1: c1 * x
    term1 = c1 * x
    
    # Term 3: c3 * x³
    term3 = c3 * x3
    
    # Term 5: c5 * x⁵
    term5 = c5 * x5
    
    # Combine: term1 + term3 + term5
    partial = term1 + term3
    result = partial + term5
    
    return result


def run_demo():
    print("=" * 70)
    print("CKKS Bootstrap - EvalMod Polynomial Evaluation")
    print("Full Pipeline: CKKS -> Poly -> C code")
    print("=" * 70)
    
    # ========================================================================
    # Step 1: Compile to CKKS IR
    # ========================================================================
    print("\n" + "=" * 70)
    print("Step 1: Compile to CKKS IR")
    print("=" * 70)
    
    eval_mod_polynomial.compile()
    glob = eval_mod_polynomial.air_module
    
    ir = glob.dump()
    mul_count = ir.lower().count('ckks.mul')
    add_count = ir.lower().count('ckks.add')
    
    print(f"\nOperation counts:")
    print(f"  CKKS.mul: {mul_count} (for x², x³, x⁴, x⁵, c1·x, c3·x³, c5·x⁵)")
    print(f"  CKKS.add: {add_count} (for combining terms)")
    
    print(f"\nCKKS IR ({len(ir)} chars):")
    print("-" * 40)
    print(ir)
    
    # ========================================================================
    # Step 2: CKKS Driver
    # ========================================================================
    print("\n" + "=" * 70)
    print("Step 2: CKKS Driver (prepare for poly pass)")
    print("=" * 70)
    
    ckks_result = air_builder.run_ckks_driver(glob)
    print(f"Success: {ckks_result['success']}")
    print(f"Message: {ckks_result['message']}")
    
    if not ckks_result['success']:
        print("\n*** Failed at CKKS driver ***")
        return
    
    if 'ir_dump' in ckks_result:
        ckks_ir = ckks_result['ir_dump']
        print(f"\nCKKS-prepared IR (first 1500 chars):")
        print("-" * 40)
        print(ckks_ir[:1500])
        if len(ckks_ir) > 1500:
            print(f"... ({len(ckks_ir)} total chars)")
    
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
        print("\n*** Failed at poly driver ***")
        return
    
    if 'ir_dump' in poly_result:
        poly_ir = poly_result['ir_dump']
        print(f"\nPoly IR (first 2000 chars):")
        print("-" * 40)
        print(poly_ir[:2000])
        if len(poly_ir) > 2000:
            print(f"... ({len(poly_ir)} total chars)")
    
    # ========================================================================
    # Step 4: Poly2C
    # ========================================================================
    print("\n" + "=" * 70)
    print("Step 4: Poly2C (Poly -> C code)")
    print("=" * 70)
    
    glob.run_cpp_pass("poly2c", [])
    c_code = glob.get_c_code()
    
    lines = c_code.split('\n')
    print(f"\nGenerated C code ({len(lines)} lines):")
    print("-" * 40)
    # Print first 80 lines
    print('\n'.join(lines[:80]))
    if len(lines) > 80:
        print(f"\n... ({len(lines)} total lines)")
    
    # ========================================================================
    # Summary
    # ========================================================================
    print("\n" + "=" * 70)
    print("Summary: Bootstrap EvalMod Phase")
    print("=" * 70)
    print("""
Pipeline completed successfully!

╔═══════════════════════════════════════════════════════════════════════╗
║  eval_mod_polynomial: Polynomial approximation of sin(2πx)/(2π)       ║
╠═══════════════════════════════════════════════════════════════════════╣
║                                                                       ║
║  Mathematical form:                                                   ║
║    p(x) = c₁·x + c₃·x³ + c₅·x⁵                                        ║
║                                                                       ║
║  Full form (Chebyshev):                                               ║
║    sin(2πx)/(2π) ≈ x - (2π)²x³/6 + (2π)⁴x⁵/120 - (2π)⁶x⁷/5040 + ...  ║
║                                                                       ║
║  Operations (7 multiplications):                                      ║
║    x² = x * x                                                         ║
║    x³ = x * x²                                                        ║
║    x⁴ = x² * x²                                                       ║
║    x⁵ = x * x⁴                                                        ║
║    term1 = c₁ * x                                                     ║
║    term3 = c₃ * x³                                                    ║
║    term5 = c₅ * x⁵                                                    ║
║    result = term1 + term3 + term5                                     ║
║                                                                       ║
╚═══════════════════════════════════════════════════════════════════════╝

This demonstrates that the polynomial evaluation core of bootstrap
can be compiled through the full FHE pipeline to C code.

Full bootstrap would additionally need (in ckks2poly):
  - CKKS.rotate for CoeffToSlot/SlotToCoeff (DFT/iDFT)
  - Double-angle iterations for extending approximation range
""")
    
    print("=" * 70)
    print("✓ Bootstrap EvalMod demo complete")
    print("=" * 70)


if __name__ == "__main__":
    run_demo()
