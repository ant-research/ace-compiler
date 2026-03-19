#!/usr/bin/env python3
"""
Example: Custom Lowering Functions
===================================

Demonstrates how to write custom lowering functions for new operations.
"""

import sys
sys.path.insert(0, '..')

from ace_dsl import nn_to_vector, vector_to_sihe
from ace_dsl.core.air_value import AIRValue
from ace_bindings import air_builder
from ace_dsl.core.registry import list_lowering_functions


# ═══════════════════════════════════════════════════════════════════════════════
# Custom Lowering Function: Polynomial Approximation of Sigmoid
# ═══════════════════════════════════════════════════════════════════════════════

@nn_to_vector("sigmoid", description="Sigmoid with polynomial approximation for FHE")
def sigmoid_to_vector(x: AIRValue) -> AIRValue:
    """
    Lower nn::core::SIGMOID to polynomial approximation.
    
    Sigmoid(x) ≈ 0.5 + 0.25*x - 0.02*x^3 + 0.002*x^5
    
    This polynomial approximation works well for x in [-5, 5].
    For FHE, we need polynomial approximations since comparisons
    and branches are extremely expensive or impossible.
    """
    container = x.container
    
    # Polynomial coefficients
    # σ(x) ≈ a0 + a1*x + a3*x^3 + a5*x^5
    a0 = 0.5
    a1 = 0.25
    a3 = -0.02
    a5 = 0.002
    
    # Compute powers of x
    x2 = x * x          # x^2
    x3 = x2 * x         # x^3
    x5 = x3 * x2        # x^5
    
    # Build polynomial: a0 + a1*x + a3*x^3 + a5*x^5
    # Note: Scalar multiplication would need encoding in FHE
    # Here we use placeholder syntax
    
    # term1 = a1 * x
    term1 = x  # Simplified - would need scalar multiply
    
    # term3 = a3 * x^3
    term3 = x3  # Simplified
    
    # term5 = a5 * x^5
    term5 = x5  # Simplified
    
    # Sum all terms
    result = term1 + term3
    result = result + term5
    
    return result


@nn_to_vector("gelu", description="GELU activation with polynomial approximation")
def gelu_to_vector(x: AIRValue) -> AIRValue:
    """
    Lower nn::core::GELU to polynomial approximation.
    
    GELU(x) = x * Φ(x) where Φ is the CDF of standard normal
    
    Approximation: GELU(x) ≈ 0.5*x * (1 + tanh(sqrt(2/π) * (x + 0.044715*x^3)))
    
    For FHE, we use a simpler polynomial approximation.
    """
    container = x.container
    
    # Simplified polynomial for GELU
    # GELU(x) ≈ 0.5*x + 0.398*x - 0.031*x^3
    
    x2 = x * x
    x3 = x2 * x
    
    # Combine terms
    result = x + x  # Would be 0.898*x
    result = result - x3  # Would subtract 0.031*x^3
    
    return result


@nn_to_vector("softmax", description="Softmax with exp polynomial approximation")
def softmax_to_vector(x: AIRValue, axis: int = -1) -> AIRValue:
    """
    Lower nn::core::SOFTMAX.
    
    Softmax requires exp() which is approximated as polynomial in FHE.
    exp(x) ≈ 1 + x + x^2/2 + x^3/6 + ...
    
    Note: This is very expensive in FHE due to the division by sum.
    Consider alternatives like polynomial activations.
    """
    container = x.container
    
    # Polynomial approximation of exp
    # exp(x) ≈ 1 + x + x^2/2 + x^3/6
    x2 = x * x
    x3 = x2 * x
    
    # exp_approx = 1 + x + x^2/2 + x^3/6
    exp_approx = x + x2  # Simplified
    
    # Would need sum and division for proper softmax
    # For FHE, often just use the exp approximation
    
    return exp_approx


# ═══════════════════════════════════════════════════════════════════════════════
# Custom SIHE Lowering
# ═══════════════════════════════════════════════════════════════════════════════

@vector_to_sihe("custom_madd", description="Multiply-add in SIHE")
def madd_to_sihe(a: AIRValue, b: AIRValue, c: AIRValue) -> AIRValue:
    """
    Custom multiply-add: result = a * b + c
    
    In CKKS, this is often more efficient than separate mul + add
    because we can fuse some operations.
    """
    # Multiply
    product = a * b
    
    # Add
    result = product + c
    
    return result


def main():
    print("=== ACE DSL Custom Lowering Example ===\n")
    
    # List registered lowering functions
    print("1. Registered nn_to_vector lowering functions:")
    funcs = list_lowering_functions("nn_to_vector")
    for name, desc in funcs.items():
        print(f"   {name}: {desc[:60]}...")
    
    print("\n2. Registered vector_to_sihe lowering functions:")
    funcs = list_lowering_functions("vector_to_sihe")
    for name, desc in funcs.items():
        print(f"   {name}: {desc[:60]}...")
    
    # Test custom lowering
    print("\n3. Testing sigmoid lowering:")
    glob = air_builder.create_glob_scope()
    func = glob.new_func("test_sigmoid")
    container = func.container
    arr_type = air_builder.Type.make_array([64], air_builder.Type.make_float(32))
    param = func.new_param("input", arr_type)
    x = AIRValue(param, container)
    
    result = sigmoid_to_vector(x)
    print(f"   Input: {x}")
    print(f"   Output: {result}")
    print(f"   Generated AIR:")
    
    print("\n=== Example Complete ===")


if __name__ == "__main__":
    main()

