"""
Vector Domain Operations

Domain-specific operations for the vector domain.
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def vec_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Vector addition for vector domain.
    
    Args:
        a: First vector
        b: Second vector
        
    Returns:
        AIRValue representing vector addition result
    """
    container = a.container
    
    # Use vector-specific add operation
    if hasattr(container, 'new_vec_add'):
        result_node = container.new_vec_add(a.value, b.value)
    else:
        # Fallback to regular add
        return a + b
    
    return AIRValue(result_node, container, a.shape)


def vec_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Vector multiplication for vector domain.
    
    Args:
        a: First vector
        b: Second vector
        
    Returns:
        AIRValue representing vector multiplication result
    """
    container = a.container
    
    # Use vector-specific mul operation
    if hasattr(container, 'new_vec_mul'):
        result_node = container.new_vec_mul(a.value, b.value)
    else:
        # Fallback to regular mul
        return a * b
    
    return AIRValue(result_node, container, a.shape)


def vec_sub(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Vector subtraction for vector domain.
    
    Args:
        a: First vector
        b: Second vector
        
    Returns:
        AIRValue representing vector subtraction result
    """
    container = a.container
    
    # Use vector-specific sub operation
    if hasattr(container, 'new_vec_sub'):
        result_node = container.new_vec_sub(a.value, b.value)
    else:
        # Fallback to regular sub
        return a - b
    
    return AIRValue(result_node, container, a.shape)


def vec_dot(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Vector dot product for vector domain.
    
    Args:
        a: First vector
        b: Second vector
        
    Returns:
        AIRValue representing dot product result (scalar)
    """
    container = a.container
    
    if hasattr(container, 'new_vec_dot'):
        result_node = container.new_vec_dot(a.value, b.value)
    else:
        # Fallback: element-wise multiply then sum
        result = a * b
        # TODO: Add reduction sum
        return result
    
    return AIRValue(result_node, container)


def vec_conv(x: AIRValue, weight: AIRValue, bias: AIRValue, **kwargs) -> AIRValue:
    """
    Vectorized convolution for vector domain.
    
    Args:
        x: Input tensor
        weight: Weight tensor
        bias: Bias tensor
        **kwargs: Additional arguments
        
    Returns:
        AIRValue representing convolution result
    """
    container = x.container
    
    if hasattr(container, 'new_vec_conv'):
        result_node = container.new_vec_conv(x.value, weight.value, bias.value)
    else:
        # Fallback: use element-wise operations
        temp = vec_mul(x, weight)
        result = vec_add(temp, bias)
        return result
    
    return AIRValue(result_node, container, x.shape)

