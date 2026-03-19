"""
Memory Domain Operations

Domain-specific operations for the memory domain.
These functions generate AIR operations via operator overloading.
"""

from typing import Any, Optional
from .air_value import AIRValue


def memory_load(addr: AIRValue, size: Optional[int] = None) -> AIRValue:
    """
    Memory load operation for memory domain.
    
    Args:
        addr: Memory address
        size: Size to load (in bytes)
        
    Returns:
        AIRValue representing loaded value
    """
    container = addr.container
    
    if hasattr(container, 'new_memory_load'):
        result_node = container.new_memory_load(addr.value)
    elif hasattr(container, 'new_ld'):
        result_node = container.new_ld(addr.value)
    else:
        # Fallback: use indexing
        return addr[0]  # Load from address
    
    return AIRValue(result_node, container)


def memory_store(addr: AIRValue, value: AIRValue, size: Optional[int] = None) -> None:
    """
    Memory store operation for memory domain.
    
    Args:
        addr: Memory address
        value: Value to store
        size: Size to store (in bytes)
    """
    container = addr.container
    
    if hasattr(container, 'new_memory_store'):
        container.new_memory_store(addr.value, value.value)
    elif hasattr(container, 'new_st'):
        container.new_st(value.value, addr.value)
    else:
        # Fallback: use assignment
        addr[0] = value  # Store to address


def memory_copy(src: AIRValue, dst: AIRValue, size: Optional[int] = None) -> None:
    """
    Memory copy operation for memory domain.
    
    Args:
        src: Source memory address
        dst: Destination memory address
        size: Size to copy (in bytes)
    """
    container = src.container
    
    if hasattr(container, 'new_memory_copy'):
        container.new_memory_copy(src.value, dst.value)
    else:
        # Fallback: load then store
        value = memory_load(src, size)
        memory_store(dst, value, size)


def memory_move(src: AIRValue, dst: AIRValue, size: Optional[int] = None) -> None:
    """
    Memory move operation for memory domain.
    
    Args:
        src: Source memory address
        dst: Destination memory address
        size: Size to move (in bytes)
    """
    # Move is same as copy for now
    memory_copy(src, dst, size)


def memory_alloc(size: int, align: Optional[int] = None) -> AIRValue:
    """
    Memory allocation for memory domain.
    
    Args:
        size: Size to allocate (in bytes)
        align: Alignment requirement
        
    Returns:
        AIRValue representing allocated memory address
    """
    # This would need access to a memory allocator
    # For now, return a placeholder
    raise NotImplementedError("Memory allocation not yet implemented")


def memory_free(addr: AIRValue) -> None:
    """
    Memory deallocation for memory domain.
    
    Args:
        addr: Memory address to free
    """
    raise NotImplementedError("Memory deallocation not yet implemented")

