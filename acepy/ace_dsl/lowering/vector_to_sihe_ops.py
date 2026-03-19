"""
Vector to SIHE Lowering Functions
==================================

Lowering functions from nn::vector to fhe::sihe domain.
SIHE (Scheme-Independent Homomorphic Encryption) provides
a uniform interface for CKKS, BFV, and BGV schemes.

See: fhe-cmplr/include/fhe/sihe/opcode_def.inc for SIHE opcodes
"""

from ..core.registry import vector_to_sihe
from ..core.air_value import AIRValue


# ═══════════════════════════════════════════════════════════════════════════════
# Helper Functions
# ═══════════════════════════════════════════════════════════════════════════════

def sihe_encode(x: AIRValue) -> AIRValue:
    """
    Emit fhe::sihe::ENCODE.
    
    Encodes plaintext vector into ciphertext.
    """
    result = x.container.new_sihe_encode(x.node)
    return AIRValue(result, x.container)


def sihe_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """Emit fhe::sihe::ADD (homomorphic addition)."""
    result = a.container.new_sihe_add(a.node, b.node)
    return AIRValue(result, a.container)


def sihe_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """Emit fhe::sihe::MUL (homomorphic multiplication)."""
    result = a.container.new_sihe_mul(a.node, b.node)
    return AIRValue(result, a.container)


def sihe_rotate(x: AIRValue, shift: int) -> AIRValue:
    """Emit fhe::sihe::ROTATE (ciphertext rotation)."""
    result = x.container.new_sihe_rotate(x.node, shift)
    return AIRValue(result, x.container)


# ═══════════════════════════════════════════════════════════════════════════════
# Lowering Functions
# ═══════════════════════════════════════════════════════════════════════════════

@vector_to_sihe("add", description="Lower nn::vector::ADD to fhe::sihe::ADD")
def add_to_sihe(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Lower nn::vector::ADD to fhe::sihe::ADD.
    
    Input:  VectorValue (nn::vector domain)
    Output: SiheCiphertext (scheme-independent encrypted)
    
    The @vector_to_sihe decorator handles:
    1. Looking up already-lowered values from replacement_map
    2. Encoding VectorValue → SiheCiphertext via fhe::sihe::ENCODE
    
    SIHE is scheme-independent - works for CKKS, BFV, or BGV.
    """
    return sihe_add(a, b)


@vector_to_sihe("sub", description="Lower nn::vector::SUB to fhe::sihe::SUB")
def sub_to_sihe(a: AIRValue, b: AIRValue) -> AIRValue:
    """Lower nn::vector::SUB to fhe::sihe::SUB."""
    result = a.container.new_sihe_sub(a.node, b.node)
    return AIRValue(result, a.container)


@vector_to_sihe("mul", description="Lower nn::vector::MUL to fhe::sihe::MUL")
def mul_to_sihe(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Lower nn::vector::MUL to fhe::sihe::MUL.
    
    Homomorphic multiplication is more expensive than addition.
    In CKKS, this also increases noise and may require rescaling.
    """
    return sihe_mul(a, b)


@vector_to_sihe("roll", description="Lower nn::vector::ROLL to fhe::sihe::ROTATE")
def roll_to_sihe(x: AIRValue, shift: int = 0) -> AIRValue:
    """
    Lower nn::vector::ROLL to fhe::sihe::ROTATE.
    
    Rotation in ciphertext space corresponds to:
    - CKKS: Galois automorphism
    - BFV/BGV: Similar rotation operation
    
    Requires pre-generated rotation keys.
    """
    return sihe_rotate(x, shift)


@vector_to_sihe("slice", description="Lower nn::vector::SLICE to rotate + mask")
def slice_to_sihe(x: AIRValue, start: int = 0, length: int = 0) -> AIRValue:
    """
    Lower nn::vector::SLICE to rotate + mask.
    
    Slicing is implemented as:
    1. Rotate to bring slice to beginning
    2. Multiply by mask to zero out unwanted elements
    
    This is expensive in FHE - try to avoid slicing when possible.
    """
    # Rotate to bring slice to beginning
    rotated = sihe_rotate(x, -start)
    
    # Would need to multiply by mask - simplified for now
    return rotated


@vector_to_sihe("gemm", description="Lower nn::vector::GEMM to SIHE operations")
def gemm_to_sihe(a: AIRValue, b: AIRValue, bias: AIRValue = None) -> AIRValue:
    """
    Lower nn::vector::GEMM to SIHE operations.
    
    Matrix multiplication in SIHE uses:
    1. Rotations to align elements
    2. Multiplications for products
    3. Additions to accumulate
    
    This is the most expensive operation in FHE neural networks.
    """
    # The actual GEMM lowering was done in nn_to_vector
    # Here we just wrap the operations in SIHE
    result = sihe_mul(a, b)
    if bias is not None:
        result = sihe_add(result, bias)
    return result

