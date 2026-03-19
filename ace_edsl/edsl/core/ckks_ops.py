"""
CKKS Domain Operations

Domain-specific operations for the CKKS (Cheon-Kim-Kim-Song) domain.
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def ckks_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    CKKS addition for fhe::ckks domain.
    
    Args:
        a: First ciphertext
        b: Second ciphertext
        
    Returns:
        AIRValue representing CKKS addition result
    """
    container = a.container
    
    if hasattr(container, 'new_ckks_add'):
        result_node = container.new_ckks_add(a.value, b.value)
    else:
        # Fallback to regular add
        return a + b
    
    return AIRValue(result_node, container)


def ckks_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    CKKS multiplication for fhe::ckks domain.
    
    Note: After multiplication, you typically need to call rescale() and relin().
    
    Args:
        a: First ciphertext
        b: Second ciphertext
        
    Returns:
        AIRValue representing CKKS multiplication result (CIPHERTEXT3)
    """
    container = a.container
    
    if hasattr(container, 'new_ckks_mul'):
        result_node = container.new_ckks_mul(a.value, b.value)
    else:
        # Fallback to regular mul
        return a * b
    
    return AIRValue(result_node, container)


def ckks_rescale(ct: AIRValue) -> AIRValue:
    """
    CKKS rescale operation.
    
    Reduces ciphertext scale after multiplication.
    
    Args:
        ct: Input ciphertext (typically after multiplication)
        
    Returns:
        AIRValue representing rescaled ciphertext
    """
    container = ct.container
    
    if hasattr(container, 'new_ckks_rescale'):
        result_node = container.new_ckks_rescale(ct.value)
    else:
        # Fallback: return input
        return ct
    
    return AIRValue(result_node, container)


def ckks_relin(ct: AIRValue) -> AIRValue:
    """
    CKKS relinearization operation.
    
    After multiplication, ciphertext has 3 polynomials (CIPHERTEXT3).
    Relinearization reduces back to 2 polynomials (CIPHERTEXT).
    
    Args:
        ct: Input ciphertext (CIPHERTEXT3 from multiplication)
        
    Returns:
        AIRValue representing relinearized ciphertext (CIPHERTEXT)
    """
    container = ct.container
    
    if hasattr(container, 'new_ckks_relin'):
        result_node = container.new_ckks_relin(ct.value)
    else:
        # Fallback: return input
        return ct
    
    return AIRValue(result_node, container)


def ckks_rotate(ct: AIRValue, rotation: int) -> AIRValue:
    """
    CKKS rotation operation.
    
    Rotates ciphertext slots by given amount.
    
    Args:
        ct: Input ciphertext
        rotation: Number of slots to rotate (positive = left, negative = right)
        
    Returns:
        AIRValue representing rotated ciphertext
    """
    container = ct.container
    
    if hasattr(container, 'new_ckks_rotate'):
        rotation_node = container.new_intconst(rotation)
        result_node = container.new_ckks_rotate(ct.value, rotation_node)
    else:
        # Fallback: return input
        return ct
    
    return AIRValue(result_node, container)


def ckks_mod_switch(ct: AIRValue) -> AIRValue:
    """
    CKKS modulus switch operation.
    
    Reduces ciphertext modulus (level) by one.
    
    Args:
        ct: Input ciphertext
        
    Returns:
        AIRValue representing ciphertext with reduced level
    """
    container = ct.container
    
    if hasattr(container, 'new_ckks_mod_switch'):
        result_node = container.new_ckks_mod_switch(ct.value)
    else:
        # Fallback: return input
        return ct
    
    return AIRValue(result_node, container)


def ckks_bootstrap(ct: AIRValue) -> AIRValue:
    """
    CKKS bootstrapping operation.
    
    Refreshes ciphertext noise budget through bootstrapping.
    
    Args:
        ct: Input ciphertext with low noise budget
        
    Returns:
        AIRValue representing bootstrapped ciphertext with refreshed noise budget
    """
    container = ct.container
    
    if hasattr(container, 'new_ckks_bootstrap'):
        result_node = container.new_ckks_bootstrap(ct.value)
    else:
        # Fallback: return input
        return ct
    
    return AIRValue(result_node, container)


def ckks_bootstrap_coeffs_to_slots(
    ct: AIRValue, num_slots: int = 0
) -> AIRValue:
    """
    CKKS bootstrap stage op: coeffs-to-slots.

    Args:
        ct: Input ciphertext
        num_slots: Target slots for precom lookup (0 = use ciphertext slots)

    Returns:
        AIRValue representing transformed ciphertext
    """
    container = ct.container

    if hasattr(container, "new_ckks_bootstrap_coeffs_to_slots"):
        result_node = container.new_ckks_bootstrap_coeffs_to_slots(
            ct.value, int(num_slots)
        )
    else:
        return ct

    return AIRValue(result_node, container)


def ckks_bootstrap_eval_mod(ct: AIRValue) -> AIRValue:
    """
    CKKS bootstrap stage op: EvalMod approximation.

    Args:
        ct: Input ciphertext

    Returns:
        AIRValue representing transformed ciphertext
    """
    container = ct.container

    if hasattr(container, "new_ckks_bootstrap_eval_mod"):
        result_node = container.new_ckks_bootstrap_eval_mod(ct.value)
    else:
        return ct

    return AIRValue(result_node, container)


def ckks_bootstrap_slots_to_coeffs(
    ct: AIRValue, num_slots: int = 0
) -> AIRValue:
    """
    CKKS bootstrap stage op: slots-to-coeffs.

    Args:
        ct: Input ciphertext
        num_slots: Target slots for precom lookup (0 = use ciphertext slots)

    Returns:
        AIRValue representing transformed ciphertext
    """
    container = ct.container

    if hasattr(container, "new_ckks_bootstrap_slots_to_coeffs"):
        result_node = container.new_ckks_bootstrap_slots_to_coeffs(
            ct.value, int(num_slots)
        )
    else:
        return ct

    return AIRValue(result_node, container)


def ckks_conjugate(ct: AIRValue) -> AIRValue:
    """
    CKKS conjugation operation.

    Args:
        ct: Input ciphertext

    Returns:
        AIRValue representing conjugated ciphertext
    """
    container = ct.container

    if hasattr(container, 'new_ckks_conjugate'):
        result_node = container.new_ckks_conjugate(ct.value)
    else:
        return ct

    return AIRValue(result_node, container)


def ckks_mul_mono(ct: AIRValue, power: int) -> AIRValue:
    """
    CKKS multiply-by-monomial operation.

    Args:
        ct: Input ciphertext
        power: Monomial power (X^power)

    Returns:
        AIRValue representing transformed ciphertext
    """
    container = ct.container

    if hasattr(container, 'new_ckks_mul_mono'):
        result_node = container.new_ckks_mul_mono(ct.value, int(power))
    else:
        return ct

    return AIRValue(result_node, container)


def ckks_raise_mod(ct: AIRValue, mod_size: int) -> AIRValue:
    """
    CKKS modulus raising operation.

    Args:
        ct: Input ciphertext
        mod_size: Target modulus size/level

    Returns:
        AIRValue representing raised ciphertext
    """
    container = ct.container

    if hasattr(container, 'new_ckks_raise_mod'):
        result_node = container.new_ckks_raise_mod(ct.value, int(mod_size))
    else:
        return ct

    return AIRValue(result_node, container)
