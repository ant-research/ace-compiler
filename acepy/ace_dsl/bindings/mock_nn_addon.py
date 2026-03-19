"""
Mock NN Addon - Pure Python mock for development/testing.

This module provides a Python-only implementation of the NN Addon API
for development and testing when C++ bindings are not available.
"""

from typing import List, Dict, Any, Optional
from dataclasses import dataclass, field


class Node:
    """NN Node representation."""
    
    _counter = 0
    
    def __init__(self, domain: str, opcode: str):
        Node._counter += 1
        self.id = Node._counter
        self.domain = domain
        self.opcode = opcode
        self.children: List['Node'] = []
        self.attrs: Dict[str, Any] = {}
    
    def name(self) -> str:
        return f"%{self.id}"
    
    def full_opcode(self) -> str:
        return f"{self.domain}::{self.opcode}"
    
    def add_child(self, child: 'Node'):
        self.children.append(child)
    
    def set_attr(self, key: str, value: Any):
        self.attrs[key] = value
    
    def to_string(self) -> str:
        children_str = ", ".join(c.name() for c in self.children)
        attrs_str = ", ".join(f"{k}={v}" for k, v in self.attrs.items())
        parts = [p for p in [children_str, attrs_str] if p]
        return f"{self.name()} = {self.full_opcode()}({', '.join(parts)})"
    
    def __repr__(self) -> str:
        return self.to_string()


class NNContainer:
    """Container for NN operations."""
    
    def __init__(self):
        self.nodes: List[Node] = []
    
    def _make_node(self, domain: str, opcode: str) -> Node:
        node = Node(domain, opcode)
        self.nodes.append(node)
        return node
    
    # =========================================================================
    # nn::core operations
    # =========================================================================
    
    def new_core_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::core", "ADD")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_core_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::core", "MUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_core_sub(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::core", "SUB")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_conv(self, x: Node, w: Node, b: Node,
                 kernel_size: List[int], padding: int = 0, stride: int = 1) -> Node:
        node = self._make_node("nn::core", "CONV")
        node.add_child(x)
        node.add_child(w)
        node.add_child(b)
        node.set_attr("kernel_size", kernel_size)
        node.set_attr("padding", padding)
        node.set_attr("stride", stride)
        return node
    
    def new_gemm(self, a: Node, b: Node, c: Node) -> Node:
        node = self._make_node("nn::core", "GEMM")
        node.add_child(a)
        node.add_child(b)
        node.add_child(c)
        return node
    
    def new_relu(self, x: Node) -> Node:
        node = self._make_node("nn::core", "RELU")
        node.add_child(x)
        return node
    
    def new_average_pool(self, x: Node, kernel_size: List[int]) -> Node:
        node = self._make_node("nn::core", "AVERAGE_POOL")
        node.add_child(x)
        node.set_attr("kernel_size", kernel_size)
        return node
    
    def new_flatten(self, x: Node, start_dim: int = 1) -> Node:
        node = self._make_node("nn::core", "FLATTEN")
        node.add_child(x)
        node.set_attr("start_dim", start_dim)
        return node
    
    def new_softmax(self, x: Node, axis: int = -1) -> Node:
        node = self._make_node("nn::core", "SOFTMAX")
        node.add_child(x)
        node.set_attr("axis", axis)
        return node
    
    def new_matmul(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::core", "MATMUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    # =========================================================================
    # nn::vector operations
    # =========================================================================
    
    def new_vec_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::vector", "ADD")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_vec_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::vector", "MUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_roll(self, x: Node, shift: int) -> Node:
        node = self._make_node("nn::vector", "ROLL")
        node.add_child(x)
        node.set_attr("shift", shift)
        return node
    
    def new_slice(self, x: Node, start: int, length: int) -> Node:
        node = self._make_node("nn::vector", "SLICE")
        node.add_child(x)
        node.set_attr("start", start)
        node.set_attr("length", length)
        return node
    
    def new_pad(self, x: Node, pad_amount: int) -> Node:
        node = self._make_node("nn::vector", "PAD")
        node.add_child(x)
        node.set_attr("pad", pad_amount)
        return node
    
    def new_reshape(self, x: Node, shape: List[int]) -> Node:
        node = self._make_node("nn::vector", "RESHAPE")
        node.add_child(x)
        node.set_attr("shape", shape)
        return node
    
    def dump(self) -> str:
        lines = []
        for node in self.nodes:
            lines.append(f"  {node.to_string()}")
        return "\n".join(lines)


# Opcode namespaces as module-like objects
class _CoreOpcodes:
    ADD = "nn::core::ADD"
    SUB = "nn::core::SUB"
    MUL = "nn::core::MUL"
    CONV = "nn::core::CONV"
    GEMM = "nn::core::GEMM"
    RELU = "nn::core::RELU"
    AVERAGE_POOL = "nn::core::AVERAGE_POOL"
    MAX_POOL = "nn::core::MAX_POOL"
    FLATTEN = "nn::core::FLATTEN"
    SOFTMAX = "nn::core::SOFTMAX"
    MATMUL = "nn::core::MATMUL"


class _VectorOpcodes:
    ADD = "nn::vector::ADD"
    MUL = "nn::vector::MUL"
    ROLL = "nn::vector::ROLL"
    SLICE = "nn::vector::SLICE"
    PAD = "nn::vector::PAD"
    RESHAPE = "nn::vector::RESHAPE"


core = _CoreOpcodes()
vector = _VectorOpcodes()


__all__ = [
    'Node',
    'NNContainer',
    'core',
    'vector',
]

