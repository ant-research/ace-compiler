"""
Mock FHE Compiler - Pure Python mock for development/testing.

This module provides a Python-only implementation of the FHE Compiler API
for development and testing when C++ bindings are not available.
"""

from typing import List, Dict, Any, Optional
from dataclasses import dataclass, field


class Node:
    """FHE Node representation."""
    
    _counter = 0
    
    def __init__(self, domain: str, opcode: str):
        Node._counter += 1
        self.id = Node._counter
        self.domain = domain
        self.opcode = opcode
        self.children: List['Node'] = []
        self.attrs: Dict[str, Any] = {}
        # FHE-specific properties
        self.scale: float = 1.0
        self.level: int = 0
        self.degree: int = 4096
    
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
        attrs_parts = []
        if self.scale != 1.0:
            attrs_parts.append(f"scale={self.scale}")
        if self.level > 0:
            attrs_parts.append(f"level={self.level}")
        for k, v in self.attrs.items():
            attrs_parts.append(f"{k}={v}")
        
        parts = [children_str] + attrs_parts
        parts = [p for p in parts if p]
        return f"{self.name()} = {self.full_opcode()}({', '.join(parts)})"
    
    def __repr__(self) -> str:
        return self.to_string()


class FHEContainer:
    """Container for FHE operations."""
    
    def __init__(self):
        self.nodes: List[Node] = []
    
    def _make_node(self, domain: str, opcode: str) -> Node:
        node = Node(domain, opcode)
        self.nodes.append(node)
        return node
    
    # =========================================================================
    # fhe::sihe operations (scheme-independent)
    # =========================================================================
    
    def new_sihe_encode(self, plaintext: Node) -> Node:
        node = self._make_node("fhe::sihe", "ENCODE")
        node.add_child(plaintext)
        return node
    
    def new_sihe_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("fhe::sihe", "ADD")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_sihe_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("fhe::sihe", "MUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_sihe_rotate(self, ct: Node, shift: int) -> Node:
        node = self._make_node("fhe::sihe", "ROTATE")
        node.add_child(ct)
        node.set_attr("shift", shift)
        return node
    
    def new_sihe_bootstrap(self, ct: Node) -> Node:
        node = self._make_node("fhe::sihe", "BOOTSTRAP")
        node.add_child(ct)
        return node
    
    # =========================================================================
    # fhe::ckks operations
    # =========================================================================
    
    def new_ckks_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("fhe::ckks", "ADD")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_ckks_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("fhe::ckks", "MUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_ckks_rescale(self, ct: Node) -> Node:
        node = self._make_node("fhe::ckks", "RESCALE")
        node.add_child(ct)
        return node
    
    def new_ckks_relin(self, ct: Node) -> Node:
        node = self._make_node("fhe::ckks", "RELIN")
        node.add_child(ct)
        return node
    
    def new_ckks_mod_switch(self, ct: Node) -> Node:
        node = self._make_node("fhe::ckks", "MOD_SWITCH")
        node.add_child(ct)
        return node
    
    def new_ckks_upscale(self, ct: Node) -> Node:
        node = self._make_node("fhe::ckks", "UPSCALE")
        node.add_child(ct)
        return node
    
    # =========================================================================
    # fhe::poly operations
    # =========================================================================
    
    def new_poly_alloc(self, degree: int, level: int) -> Node:
        node = self._make_node("fhe::poly", "ALLOC")
        node.degree = degree
        node.level = level
        node.set_attr("degree", degree)
        node.set_attr("level", level)
        return node
    
    def new_poly_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("fhe::poly", "ADD")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_poly_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("fhe::poly", "MUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_poly_ntt(self, poly: Node) -> Node:
        node = self._make_node("fhe::poly", "NTT")
        node.add_child(poly)
        return node
    
    def new_poly_intt(self, poly: Node) -> Node:
        node = self._make_node("fhe::poly", "INTT")
        node.add_child(poly)
        return node
    
    def dump(self) -> str:
        lines = []
        for node in self.nodes:
            lines.append(f"  {node.to_string()}")
        return "\n".join(lines)


# Opcode namespaces
class _SiheOpcodes:
    ENCODE = "fhe::sihe::ENCODE"
    ADD = "fhe::sihe::ADD"
    MUL = "fhe::sihe::MUL"
    ROTATE = "fhe::sihe::ROTATE"
    BOOTSTRAP = "fhe::sihe::BOOTSTRAP"


class _CkksOpcodes:
    ADD = "fhe::ckks::ADD"
    MUL = "fhe::ckks::MUL"
    RESCALE = "fhe::ckks::RESCALE"
    RELIN = "fhe::ckks::RELIN"
    MOD_SWITCH = "fhe::ckks::MOD_SWITCH"
    UPSCALE = "fhe::ckks::UPSCALE"


class _PolyOpcodes:
    ALLOC = "fhe::poly::ALLOC"
    ADD = "fhe::poly::ADD"
    MUL = "fhe::poly::MUL"
    NTT = "fhe::poly::NTT"
    INTT = "fhe::poly::INTT"


sihe = _SiheOpcodes()
ckks = _CkksOpcodes()
poly = _PolyOpcodes()


__all__ = [
    'Node',
    'FHEContainer',
    'sihe',
    'ckks',
    'poly',
]

