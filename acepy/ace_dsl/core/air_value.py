"""
AIRValue - Wrapper class for operator overloading.

This class wraps AIR nodes to enable Python operators (+, *, [], etc.)
to emit AIR nodes during lowering.

Example:
    @kernel
    def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
        return a + b  # AIRValue.__add__ emits nn::core::ADD
"""

from typing import Any, Optional, Tuple, List, Union
from base_dsl.loc import get_caller_loc, Loc, source_location

# Import C++ bindings
from ace_bindings import air_builder, nn_addon


class AIRValue:
    """
    Python wrapper around an AIR node that overloads operators.
    
    When you write `a + b` in a @kernel function, the + operator
    calls __add__, which emits an nn::core::ADD or nn::vector::ADD node.
    
    Attributes:
        node: The underlying AIR node from C++ bindings
        container: The container holding this node
        loc: Source location for debugging
        shape: Optional shape information for tensor operations
    """
    
    def __init__(self, node: Any, container: Any, loc: Optional[Loc] = None, 
                 shape: Optional[Tuple[int, ...]] = None):
        self._node = node
        self._container = container
        self._loc = loc or get_caller_loc()
        self._shape = shape
    
    @property
    def node(self) -> Any:
        """Return the underlying AIR node."""
        return self._node
    
    @property
    def container(self) -> Any:
        """Return the container holding this node."""
        return self._container
    
    @property
    def shape(self) -> Optional[Tuple[int, ...]]:
        """Return the shape of this value (if a tensor)."""
        return self._shape
    
    @property
    def loc(self) -> Loc:
        """Return the source location."""
        return self._loc
    
    # =========================================================================
    # Arithmetic Operators
    # =========================================================================
    
    @source_location
    def __add__(self, other: 'AIRValue') -> 'AIRValue':
        """Emit nn::vector::ADD (or nn::core::ADD at high level)."""
        other_node = other.node if isinstance(other, AIRValue) else other
        
        # Use nn_addon container if available
        if hasattr(self._container, 'new_vec_add'):
            result_node = self._container.new_vec_add(self._node, other_node)
        elif hasattr(self._container, 'new_add'):
            result_node = self._container.new_add(self._node, other_node)
        else:
            raise RuntimeError("Container does not support add operation")
        
        return AIRValue(result_node, self._container, get_caller_loc(), self._shape)
    
    def __radd__(self, other: Any) -> 'AIRValue':
        """Handle reverse add (e.g., scalar + AIRValue)."""
        return self.__add__(other)
    
    @source_location
    def __sub__(self, other: 'AIRValue') -> 'AIRValue':
        """Emit subtraction."""
        other_node = other.node if isinstance(other, AIRValue) else other
        
        if hasattr(self._container, 'new_sub'):
            result_node = self._container.new_sub(self._node, other_node)
        else:
            raise RuntimeError("Container does not support sub operation")
        
        return AIRValue(result_node, self._container, get_caller_loc(), self._shape)
    
    @source_location
    def __mul__(self, other: 'AIRValue') -> 'AIRValue':
        """Emit nn::vector::MUL (or nn::core::MUL at high level)."""
        other_node = other.node if isinstance(other, AIRValue) else other
        
        if hasattr(self._container, 'new_vec_mul'):
            result_node = self._container.new_vec_mul(self._node, other_node)
        elif hasattr(self._container, 'new_mul'):
            result_node = self._container.new_mul(self._node, other_node)
        else:
            raise RuntimeError("Container does not support mul operation")
        
        return AIRValue(result_node, self._container, get_caller_loc(), self._shape)
    
    def __rmul__(self, other: Any) -> 'AIRValue':
        """Handle reverse multiply."""
        return self.__mul__(other)
    
    @source_location
    def __matmul__(self, other: 'AIRValue') -> 'AIRValue':
        """Emit matrix multiplication (nn::core::MATMUL)."""
        other_node = other.node if isinstance(other, AIRValue) else other
        
        if hasattr(self._container, 'new_matmul'):
            result_node = self._container.new_matmul(self._node, other_node)
        else:
            raise NotImplementedError("Matrix multiplication not supported by container")
        
        return AIRValue(result_node, self._container, get_caller_loc())
    
    # =========================================================================
    # Indexing
    # =========================================================================
    
    @source_location
    def __getitem__(self, idx: Union[int, Tuple[int, ...], slice]) -> 'AIRValue':
        """
        Emit air::core::ILD (indexed load).
        
        Supports:
            x[0]      - Single index
            x[0, 1]   - Multiple indices
            x[0:10]   - Slicing (emits nn::vector::SLICE)
        """
        if isinstance(idx, slice):
            # Handle slicing
            start = idx.start or 0
            stop = idx.stop
            length = stop - start if stop else -1
            
            if hasattr(self._container, 'new_slice'):
                result_node = self._container.new_slice(self._node, start, length)
            else:
                raise NotImplementedError("Slicing not supported")
            
            return AIRValue(result_node, self._container, get_caller_loc())
        
        # Handle tuple of indices
        if isinstance(idx, tuple):
            idx_nodes = []
            for i in idx:
                if isinstance(i, int):
                    idx_nodes.append(self._container.new_intconst(i))
                elif isinstance(i, AIRValue):
                    idx_nodes.append(i.node)
                else:
                    idx_nodes.append(i)
            idx_node = idx_nodes[0] if len(idx_nodes) == 1 else idx_nodes
        elif isinstance(idx, int):
            idx_node = self._container.new_intconst(idx)
        elif isinstance(idx, AIRValue):
            idx_node = idx.node
        else:
            idx_node = idx
        
        if hasattr(self._container, 'new_ild'):
            result_node = self._container.new_ild(self._node, idx_node)
        else:
            raise NotImplementedError("Indexed load not supported")
        
        return AIRValue(result_node, self._container, get_caller_loc())
    
    # =========================================================================
    # Comparison Operators
    # =========================================================================
    
    def __eq__(self, other: Any) -> bool:
        """Check equality (for Python, not IR generation)."""
        if isinstance(other, AIRValue):
            return self._node == other._node
        return False
    
    def __hash__(self) -> int:
        """Make AIRValue hashable."""
        return id(self._node)
    
    # =========================================================================
    # String Representation
    # =========================================================================
    
    def __repr__(self) -> str:
        if hasattr(self._node, 'to_string'):
            return f"AIRValue({self._node.to_string()})"
        elif hasattr(self._node, 'name'):
            return f"AIRValue({self._node.name()})"
        else:
            return f"AIRValue({self._node})"


# ═══════════════════════════════════════════════════════════════════════════════
# Helper functions for creating AIRValues
# ═══════════════════════════════════════════════════════════════════════════════

def wrap_node(node: Any, container: Any, shape: Optional[Tuple[int, ...]] = None) -> AIRValue:
    """Wrap an AIR node in an AIRValue."""
    return AIRValue(node, container, get_caller_loc(), shape)


def zeros(shape: Tuple[int, ...], container: Any) -> AIRValue:
    """Create a zero tensor."""
    zero_node = container.new_zero()
    return AIRValue(zero_node, container, get_caller_loc(), shape)


def ones(shape: Tuple[int, ...], container: Any) -> AIRValue:
    """Create a ones tensor."""
    one_node = container.new_one()
    return AIRValue(one_node, container, get_caller_loc(), shape)


def const(value: Union[int, float], container: Any) -> AIRValue:
    """Create a constant value."""
    if isinstance(value, int):
        node = container.new_intconst(value)
    else:
        # Float constants may need different handling
        node = container.new_intconst(int(value))
    return AIRValue(node, container, get_caller_loc())


__all__ = [
    'AIRValue',
    'wrap_node',
    'zeros',
    'ones',
    'const',
]
