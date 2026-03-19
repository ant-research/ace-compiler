"""
Domain-Level Kernel Examples
============================

Demonstrates writing kernels at different levels of the FHE compilation pipeline:

1. @kernel          - Tensor level (nn::core) - High-level neural network ops
2. @vector_kernel   - Vector level (nn::vector) - After vectorization  
3. @sihe_kernel     - SIHE level (fhe::sihe) - Scheme-independent FHE
4. @ckks_kernel     - CKKS level (fhe::ckks) - CKKS-specific with rescale
5. @poly_kernel     - Polynomial level (fhe::poly) - Low-level NTT/polynomial
"""

from ace_dsl.frontend.domain_kernels import (
    # Decorators
    kernel, vector_kernel, sihe_kernel, ckks_kernel, poly_kernel,
    # Types
    Tensor, VectorTensor, SiheCiphertext, CkksCiphertext, Polynomial,
)


# =============================================================================
# Level 1: Tensor/NN-Core Level (@kernel)
# =============================================================================
# This is the highest level - users write neural network operations.
# The compiler handles all lowering to FHE.

@kernel
def nn_add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
    """Simple tensor addition at nn::core level."""
    return a + b


@kernel
def nn_polynomial(x: Tensor[64], a: Tensor[64], b: Tensor[64], c: Tensor[64]) -> Tensor[64]:
    """Polynomial: a + b*x + c*x^2 at nn::core level."""
    x2 = x * x
    bx = b * x
    cx2 = c * x2
    return a + bx + cx2


# =============================================================================
# Level 2: Vector Level (@vector_kernel)
# =============================================================================
# After vectorization - operations work on packed vectors.
# Useful for custom SIMD-style operations.

@vector_kernel
def vec_fused_mul_add(a: VectorTensor[64], b: VectorTensor[64], c: VectorTensor[64]) -> VectorTensor[64]:
    """Fused multiply-add: a * b + c at nn::vector level."""
    ab = a * b
    return ab + c


@vector_kernel  
def vec_square_diff(a: VectorTensor[64], b: VectorTensor[64]) -> VectorTensor[64]:
    """Squared difference: (a - b)^2 at nn::vector level."""
    diff = a - b
    return diff * diff


# =============================================================================
# Level 3: SIHE Level (@sihe_kernel)
# =============================================================================
# Scheme-Independent Homomorphic Encryption level.
# Works with any FHE scheme (CKKS, BFV, BGV).

@sihe_kernel
def sihe_add(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Ciphertext addition at fhe::sihe level."""
    return a + b


@sihe_kernel
def sihe_mul(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Ciphertext multiplication at fhe::sihe level."""
    return a * b


@sihe_kernel
def sihe_dot_product(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """
    Dot product at SIHE level.
    In real FHE, this would include rotation and sum operations.
    """
    # Element-wise multiply
    product = a * b
    # Would add rotation-and-sum here for real dot product
    return product


# =============================================================================
# Level 4: CKKS Level (@ckks_kernel)
# =============================================================================
# CKKS-specific operations with scale management.
# For approximate arithmetic on encrypted floats.

@ckks_kernel
def ckks_mul_with_scale(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
    """
    CKKS multiplication.
    In real CKKS, would need rescale after multiplication.
    """
    result = a * b
    # Real CKKS: result = rescale(result)
    return result


@ckks_kernel
def ckks_polynomial_eval(x: CkksCiphertext, c0: CkksCiphertext, c1: CkksCiphertext) -> CkksCiphertext:
    """
    Polynomial evaluation in CKKS: c0 + c1*x
    Each multiplication needs rescaling in real CKKS.
    """
    term1 = c1 * x
    # Real: term1 = rescale(term1)
    return c0 + term1


# =============================================================================
# Level 5: Polynomial Level (@poly_kernel)
# =============================================================================
# Lowest level - direct polynomial ring operations.
# For NTT, polynomial multiplication, modular arithmetic.

@poly_kernel
def poly_add(p: Polynomial[4096], q: Polynomial[4096]) -> Polynomial[4096]:
    """Polynomial addition in R_q."""
    return p + q


@poly_kernel
def poly_mul(p: Polynomial[4096], q: Polynomial[4096]) -> Polynomial[4096]:
    """
    Polynomial multiplication in R_q.
    In real implementation: NTT(p) * NTT(q) then INTT.
    """
    return p * q


@poly_kernel
def poly_square(p: Polynomial[4096]) -> Polynomial[4096]:
    """Polynomial squaring: p^2."""
    return p * p


# =============================================================================
# Main - Demonstrate all levels
# =============================================================================

if __name__ == "__main__":
    print("=" * 70)
    print("Domain-Level Kernel Examples")
    print("=" * 70)
    
    levels = [
        ("Level 1: Tensor (nn::core)", [nn_add, nn_polynomial]),
        ("Level 2: Vector (nn::vector)", [vec_fused_mul_add, vec_square_diff]),
        ("Level 3: SIHE (fhe::sihe)", [sihe_add, sihe_mul, sihe_dot_product]),
        ("Level 4: CKKS (fhe::ckks)", [ckks_mul_with_scale, ckks_polynomial_eval]),
        ("Level 5: Polynomial (fhe::poly)", [poly_add, poly_mul, poly_square]),
    ]
    
    for level_name, kernels in levels:
        print(f"\n{'='*70}")
        print(f"{level_name}")
        print(f"{'='*70}")
        
        for k in kernels:
            print(f"\n--- {k.name} (domain: {k.DOMAIN}) ---")
            k.compile(enable_ir_printing=False)
            print(f"Initial IR:\n{k.dump_ir()}")
            
            print(f"After pipeline:")
            final = k.run_pipeline(enable_ir_printing=False)
            # Show just the first few lines
            lines = final.split('\n')[:5]
            print('\n'.join(lines))
            if len(final.split('\n')) > 5:
                print("...")
    
    print("\n" + "=" * 70)
    print("✓ All domain-level kernels compiled successfully!")
    print("=" * 70)

