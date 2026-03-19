"""
Mock AIR Builder - Pure Python mock for development/testing.

This module provides a Python-only implementation of the AIR Builder API
for development and testing when C++ bindings are not available.
"""

from typing import List, Dict, Optional, Any
from dataclasses import dataclass, field


@dataclass
class Type:
    """AIR Type representation."""
    kind: str  # "void", "int", "float", "array", "pointer"
    bits: int = 0
    shape: List[int] = field(default_factory=list)
    elem_type: Optional['Type'] = None
    name: str = "void"
    
    @staticmethod
    def make_void() -> 'Type':
        return Type(kind="void", name="void")
    
    @staticmethod
    def make_int(bits: int) -> 'Type':
        return Type(kind="int", bits=bits, name=f"i{bits}")
    
    @staticmethod
    def make_float(bits: int) -> 'Type':
        return Type(kind="float", bits=bits, name=f"f{bits}")
    
    @staticmethod
    def make_array(shape: List[int], elem_type: 'Type' = None) -> 'Type':
        if elem_type is None:
            elem_type = Type.make_float(32)
        return Type(kind="array", shape=shape, elem_type=elem_type, name="array")
    
    def to_string(self) -> str:
        if self.kind == "array":
            shape_str = "x".join(str(d) for d in self.shape)
            return f"[{shape_str}x{self.elem_type.name if self.elem_type else 'f32'}]"
        return self.name
    
    def is_array(self) -> bool:
        return self.kind == "array"
    
    def __repr__(self) -> str:
        return self.to_string()


class Node:
    """AIR Node (operation) representation."""
    
    _counter = 0
    
    def __init__(self, opcode: str, rtype: Type = None):
        Node._counter += 1
        self.id = Node._counter
        self.opcode = opcode
        self.rtype = rtype or Type.make_void()
        self.children: List['Node'] = []
        self.attrs: Dict[str, Any] = {}
    
    def name(self) -> str:
        return f"%{self.id}"
    
    def opcode_name(self) -> str:
        return self.opcode
    
    def result_type(self) -> Type:
        return self.rtype
    
    def add_child(self, child: 'Node'):
        self.children.append(child)
    
    def set_attr(self, key: str, value: Any):
        self.attrs[key] = value
    
    def to_string(self) -> str:
        children_str = ", ".join(c.name() for c in self.children)
        attrs_str = ", ".join(f"{k}={v}" for k, v in self.attrs.items())
        parts = [p for p in [children_str, attrs_str] if p]
        return f"{self.name()} = {self.opcode}({', '.join(parts)})"
    
    def __repr__(self) -> str:
        return self.to_string()


class Container:
    """Container for AIR nodes within a function."""
    
    def __init__(self):
        self.nodes: List[Node] = []
        self.default_type = Type.make_float(32)
    
    def _make_node(self, opcode: str, rtype: Type = None) -> Node:
        node = Node(opcode, rtype or self.default_type)
        self.nodes.append(node)
        return node
    
    # Arithmetic operations
    def new_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("air::core::ADD")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_sub(self, a: Node, b: Node) -> Node:
        node = self._make_node("air::core::SUB")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("air::core::MUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_div(self, a: Node, b: Node) -> Node:
        node = self._make_node("air::core::DIV")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_matmul(self, a: Node, b: Node) -> Node:
        node = self._make_node("nn::core::MATMUL")
        node.add_child(a)
        node.add_child(b)
        return node
    
    # NN Core operations
    def new_nn_add(self, a: Node, b: Node) -> Node:
        node = self._make_node("NN.add")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_nn_sub(self, a: Node, b: Node) -> Node:
        node = self._make_node("NN.sub")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_nn_mul(self, a: Node, b: Node) -> Node:
        node = self._make_node("NN.mul")
        node.add_child(a)
        node.add_child(b)
        return node
    
    def new_nn_conv(self, x: Node, w: Node, b: Node) -> Node:
        node = self._make_node("NN.conv")
        node.add_child(x)
        node.add_child(w)
        node.add_child(b)
        return node
    
    def new_nn_relu(self, x: Node) -> Node:
        node = self._make_node("NN.relu")
        node.add_child(x)
        return node
    
    # Load/Store
    def new_ld(self, addr: Node) -> Node:
        node = self._make_node("air::core::LD")
        node.add_child(addr)
        return node
    
    def new_st(self, val: Node, addr: Node) -> Node:
        node = self._make_node("air::core::ST", Type.make_void())
        node.add_child(val)
        node.add_child(addr)
        return node
    
    def new_ild(self, base: Node, idx: Node) -> Node:
        node = self._make_node("air::core::ILD")
        node.add_child(base)
        node.add_child(idx)
        return node
    
    def new_ist(self, val: Node, base: Node, idx: Node) -> Node:
        node = self._make_node("air::core::IST", Type.make_void())
        node.add_child(val)
        node.add_child(base)
        node.add_child(idx)
        return node
    
    # Constants
    def new_intconst(self, val: int) -> Node:
        node = self._make_node("air::core::INTCONST", Type.make_int(64))
        node.set_attr("value", val)
        return node
    
    def new_zero(self) -> Node:
        return self._make_node("air::core::ZERO")
    
    def new_one(self) -> Node:
        return self._make_node("air::core::ONE")
    
    # Array
    def new_array(self, base: Node) -> Node:
        node = self._make_node("air::core::ARRAY")
        node.add_child(base)
        return node
    
    # Control flow
    def new_retv(self, val: Node) -> Node:
        node = self._make_node("air::core::RETV", Type.make_void())
        node.add_child(val)
        return node
    
    def new_ret(self) -> Node:
        return self._make_node("air::core::RET", Type.make_void())
    
    def dump(self) -> str:
        lines = []
        for node in self.nodes:
            lines.append(f"  {node.to_string()}")
        return "\n".join(lines)


class FuncScope:
    """Function scope for building a single function."""
    
    def __init__(self, name: str):
        self.name = name
        self.params: List[Node] = []
        self._container = Container()
    
    def new_param(self, param_name: str, param_type: Type) -> Node:
        node = Node("PARAM", param_type)
        node.set_attr("name", param_name)
        self.params.append(node)
        return node
    
    def container(self) -> Container:
        return self._container
    
    def dump(self) -> str:
        params_str = ", ".join(
            f"{p.attrs.get('name', p.name())}: {p.rtype}" for p in self.params
        )
        body = self._container.dump()
        return f"func {self.name}({params_str}):\n{body}"


class GlobScope:
    """Global scope for entire compilation unit."""
    
    def __init__(self):
        self.functions: List[FuncScope] = []
        self.types: Dict[str, Type] = {
            "void": Type.make_void(),
            "i32": Type.make_int(32),
            "i64": Type.make_int(64),
            "f32": Type.make_float(32),
            "f64": Type.make_float(64),
        }
        self._inlined_sections: List[str] = []  # Store inlined IR sections
    
    def new_func(self, name: str) -> FuncScope:
        func = FuncScope(name)
        self.functions.append(func)
        return func
    
    def get_type(self, name: str) -> Type:
        return self.types.get(name, Type.make_void())
    
    def new_array_type(self, shape: List[int], elem: str = "f32") -> Type:
        return Type.make_array(shape, self.get_type(elem))
    
    def inline_lowering(self, op_pattern: str, lowering_ir: str, operand_names: List[str] = None) -> bool:
        """
        Inline a lowering body into the glob scope.
        
        This modifies the Container nodes to replace matched ops with the lowering body.
        
        Args:
            op_pattern: Pattern to match (e.g., "NN.conv")
            lowering_ir: The IR of the lowering body to inline
            operand_names: Names of operands at the call site (for parameter mapping)
        
        Returns:
            True if any inlining was performed
        """
        inlined = False
        for func in self.functions:
            container = func._container
            new_nodes = []
            
            for node in container.nodes:
                # Check if this node matches the pattern
                if op_pattern.lower() in node.opcode.lower():
                    # Found a match - inline the lowering body
                    # Create a comment node showing inlining
                    inline_marker = Node(f"INLINED[{op_pattern}]", node.rtype)
                    inline_marker.set_attr("lowering", lowering_ir[:100] + "..." if len(lowering_ir) > 100 else lowering_ir)
                    new_nodes.append(inline_marker)
                    
                    # Store the full lowering IR for dump
                    self._inlined_sections.append(f"  ; === INLINED {op_pattern} ===\n{lowering_ir}\n  ; === END INLINED ===")
                    inlined = True
                else:
                    new_nodes.append(node)
            
            container.nodes = new_nodes
        
        return inlined
    
    def dump(self) -> str:
        base_dump = "\n\n".join(f.dump() for f in self.functions)
        if self._inlined_sections:
            # Insert inlined sections into the dump
            inlined_str = "\n".join(self._inlined_sections)
            return base_dump + "\n\n; Inlined lowering bodies:\n" + inlined_str
        return base_dump


def create_glob_scope() -> GlobScope:
    """Create a new global scope for building AIR."""
    return GlobScope()


# Module-level exports
__all__ = [
    'Type',
    'Node', 
    'Container',
    'FuncScope',
    'GlobScope',
    'create_glob_scope',
]

