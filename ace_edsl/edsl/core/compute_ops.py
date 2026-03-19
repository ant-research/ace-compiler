"""
Compute Domain Operations

Domain-specific operations for the compute domain.
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def compute_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Compute addition for compute domain.
    
    Args:
        a: First operand
        b: Second operand
        
    Returns:
        AIRValue representing addition result
    """
    container = a.container
    
    if hasattr(container, 'new_compute_add'):
        result_node = container.new_compute_add(a.value, b.value)
    else:
        # Fallback to regular add
        return a + b
    
    return AIRValue(result_node, container, a.shape)


def compute_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Compute multiplication for compute domain.
    
    Args:
        a: First operand
        b: Second operand
        
    Returns:
        AIRValue representing multiplication result
    """
    container = a.container
    
    if hasattr(container, 'new_compute_mul'):
        result_node = container.new_compute_mul(a.value, b.value)
    else:
        # Fallback to regular mul
        return a * b
    
    return AIRValue(result_node, container, a.shape)


def parallel_reduce(x: AIRValue, op: str = "sum", axis: Optional[int] = None) -> AIRValue:
    """
    Parallel reduction for compute domain.
    
    Args:
        x: Input tensor
        op: Reduction operation ("sum", "max", "min", "mean")
        axis: Axis along which to reduce (None = all)
        
    Returns:
        AIRValue representing reduction result
    """
    container = x.container
    
    if hasattr(container, 'new_compute_reduce'):
        result_node = container.new_compute_reduce(x.value)
    else:
        # Fallback: return input
        result_node = x.value
    
    return AIRValue(result_node, container)


def parallel_scan(x: AIRValue, op: str = "sum") -> AIRValue:
    """
    Parallel scan (prefix sum) for compute domain.
    
    Args:
        x: Input tensor
        op: Scan operation ("sum", "max", "min")
        
    Returns:
        AIRValue representing scan result
    """
    container = x.container
    
    if hasattr(container, 'new_compute_scan'):
        result_node = container.new_compute_scan(x.value)
    else:
        # Fallback: return input
        result_node = x.value
    
    return AIRValue(result_node, container, x.shape)

