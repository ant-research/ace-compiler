"""
Polynomial Domain Operations

Domain-specific operations for the polynomial domain (fhe::poly).
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def poly_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Polynomial addition for fhe::poly domain.
    
    Args:
        a: First polynomial
        b: Second polynomial
        
    Returns:
        AIRValue representing polynomial addition result
    """
    container = a.container
    
    if hasattr(container, 'new_poly_add'):
        result_node = container.new_poly_add(a.value, b.value)
    else:
        # Fallback to regular add
        return a + b
    
    return AIRValue(result_node, container)


def poly_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Polynomial multiplication for fhe::poly domain.
    
    Args:
        a: First polynomial
        b: Second polynomial
        
    Returns:
        AIRValue representing polynomial multiplication result
    """
    container = a.container
    
    if hasattr(container, 'new_poly_mul'):
        result_node = container.new_poly_mul(a.value, b.value)
    else:
        # Fallback to regular mul
        return a * b
    
    return AIRValue(result_node, container)


def poly_ntt(p: AIRValue) -> AIRValue:
    """
    Number Theoretic Transform (NTT) for fhe::poly domain.
    
    Converts polynomial from coefficient representation to NTT representation.
    
    Args:
        p: Input polynomial
        
    Returns:
        AIRValue representing NTT of polynomial
    """
    container = p.container
    
    if hasattr(container, 'new_poly_ntt'):
        result_node = container.new_poly_ntt(p.value)
    else:
        # Fallback: return input
        return p
    
    return AIRValue(result_node, container)


def poly_intt(p: AIRValue) -> AIRValue:
    """
    Inverse Number Theoretic Transform (INTT) for fhe::poly domain.
    
    Converts polynomial from NTT representation back to coefficient representation.
    
    Args:
        p: Input polynomial in NTT form
        
    Returns:
        AIRValue representing INTT of polynomial
    """
    container = p.container
    
    if hasattr(container, 'new_poly_intt'):
        result_node = container.new_poly_intt(p.value)
    else:
        # Fallback: return input
        return p
    
    return AIRValue(result_node, container)


def poly_neg(p: AIRValue) -> AIRValue:
    """
    Polynomial negation for fhe::poly domain.
    
    Args:
        p: Input polynomial
        
    Returns:
        AIRValue representing negated polynomial
    """
    container = p.container
    
    if hasattr(container, 'new_poly_neg'):
        result_node = container.new_poly_neg(p.value)
    else:
        # Fallback to regular negation
        return -p
    
    return AIRValue(result_node, container)

