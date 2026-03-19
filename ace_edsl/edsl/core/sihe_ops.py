"""
SIHE Domain Operations

Domain-specific operations for the SIHE (Scheme-Independent Homomorphic Encryption) domain.
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def sihe_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    SIHE addition for fhe::sihe domain.
    
    Args:
        a: First ciphertext
        b: Second ciphertext
        
    Returns:
        AIRValue representing SIHE addition result
    """
    container = a.container
    
    if hasattr(container, 'new_sihe_add'):
        result_node = container.new_sihe_add(a.value, b.value)
    else:
        # Fallback to regular add
        return a + b
    
    return AIRValue(result_node, container)


def sihe_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    SIHE multiplication for fhe::sihe domain.
    
    Args:
        a: First ciphertext
        b: Second ciphertext
        
    Returns:
        AIRValue representing SIHE multiplication result
    """
    container = a.container
    
    if hasattr(container, 'new_sihe_mul'):
        result_node = container.new_sihe_mul(a.value, b.value)
    else:
        # Fallback to regular mul
        return a * b
    
    return AIRValue(result_node, container)


def sihe_sub(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    SIHE subtraction for fhe::sihe domain.
    
    Args:
        a: First ciphertext
        b: Second ciphertext
        
    Returns:
        AIRValue representing SIHE subtraction result
    """
    container = a.container
    
    if hasattr(container, 'new_sihe_sub'):
        result_node = container.new_sihe_sub(a.value, b.value)
    else:
        # Fallback to regular sub
        return a - b
    
    return AIRValue(result_node, container)


def sihe_neg(a: AIRValue) -> AIRValue:
    """
    SIHE negation for fhe::sihe domain.
    
    Args:
        a: Input ciphertext
        
    Returns:
        AIRValue representing negated ciphertext
    """
    container = a.container
    
    if hasattr(container, 'new_sihe_neg'):
        result_node = container.new_sihe_neg(a.value)
    else:
        # Fallback to regular negation
        return -a
    
    return AIRValue(result_node, container)

