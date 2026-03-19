"""
Python IR Data Structures
=========================

Intermediate representation between Python AST and AIR.
This provides a cleaner abstraction for the lowering process.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Optional, Tuple, Any
from .loc import Loc


class Scope:
    """Manages variable naming and uniqueness."""
    
    def __init__(self):
        self._all_vars: Dict[str, str] = {}
        self._counter = 0
    
    def make_var(self, name: str, loc: Loc) -> 'Var':
        """Create a unique variable name."""
        var_name = name
        while var_name in self._all_vars:
            var_name = f"{name}.{self._counter}"
            self._counter += 1
        self._all_vars[var_name] = name
        return Var(var_name, loc)
    
    def lookup(self, name: str) -> Optional['Var']:
        """Look up a variable by original name."""
        if name in self._all_vars:
            return Var(name, Loc.unknown())
        return None


@dataclass
class Var:
    """Represents a variable in Python IR."""
    name: str
    loc: Loc
    
    def __hash__(self):
        return hash(self.name)
    
    def __eq__(self, other):
        if isinstance(other, Var):
            return self.name == other.name
        return False


class Operation:
    """Base class for Python IR operations."""
    
    def __init__(self, op: str, operands: Dict[str, Any], 
                 result_vars: List[Var], loc: Loc):
        self.op = op
        self.operands = operands
        self.result_vars = result_vars
        self.loc = loc
        self.nested_blocks: List['Block'] = []
    
    def __repr__(self):
        results = ", ".join(v.name for v in self.result_vars)
        return f"{results} = {self.op}({self.operands})"


class Load(Operation):
    """Load a variable by name."""
    
    def __init__(self, var_name: str, result: Var, loc: Loc):
        super().__init__("load", {}, [result], loc)
        self.var_name = var_name
    
    def __repr__(self):
        return f"{self.result_vars[0].name} = load({self.var_name})"


class Store(Operation):
    """Store to a variable name."""
    
    def __init__(self, var_name: str, value: Var, loc: Loc):
        super().__init__("store", {"value": value}, [], loc)
        self.var_name = var_name
    
    def __repr__(self):
        return f"store({self.var_name}, {self.operands['value'].name})"


class Const(Operation):
    """A constant value."""
    
    def __init__(self, value: Any, result: Var, loc: Loc):
        super().__init__("const", {}, [result], loc)
        self.value = value
    
    def __repr__(self):
        return f"{self.result_vars[0].name} = const({self.value})"


class BinOp(Operation):
    """Binary operation: add, sub, mul, etc."""
    
    def __init__(self, op_name: str, lhs: Var, rhs: Var, result: Var, loc: Loc):
        super().__init__("binop", {"lhs": lhs, "rhs": rhs}, [result], loc)
        self.op_name = op_name
    
    def __repr__(self):
        return f"{self.result_vars[0].name} = {self.op_name}({self.operands['lhs'].name}, {self.operands['rhs'].name})"


class UnaryOp(Operation):
    """Unary operation: neg, not, etc."""
    
    def __init__(self, op_name: str, operand: Var, result: Var, loc: Loc):
        super().__init__("unaryop", {"operand": operand}, [result], loc)
        self.op_name = op_name


class Call(Operation):
    """Function/operation call."""
    
    def __init__(self, callee: Var, args: Tuple[Var, ...], 
                 kwargs: Dict[str, Var], result: Var, loc: Loc):
        super().__init__("call", {"callee": callee, "args": args}, [result], loc)
        self.kwargs = kwargs
    
    def __repr__(self):
        args_str = ", ".join(a.name for a in self.operands['args'])
        return f"{self.result_vars[0].name} = call({self.operands['callee'].name}, [{args_str}])"


class Return(Operation):
    """Return statement."""
    
    def __init__(self, value: Optional[Var], loc: Loc):
        operands = {"value": value} if value else {}
        super().__init__("return", operands, [], loc)
    
    def __repr__(self):
        if "value" in self.operands:
            return f"return({self.operands['value'].name})"
        return "return()"


class ForLoop(Operation):
    """For loop construct."""
    
    def __init__(self, loop_var: str, iterable: Var, body: 'Block', loc: Loc):
        super().__init__("for", {"iterable": iterable}, [], loc)
        self.loop_var = loop_var
        self.nested_blocks = [body]
    
    def __repr__(self):
        return f"for {self.loop_var} in {self.operands['iterable'].name}:"


class If(Operation):
    """If statement."""
    
    def __init__(self, condition: Var, then_block: 'Block', 
                 else_block: Optional['Block'], loc: Loc):
        super().__init__("if", {"condition": condition}, [], loc)
        self.nested_blocks = [then_block]
        if else_block:
            self.nested_blocks.append(else_block)


class Block:
    """A sequence of operations."""
    
    def __init__(self, scope: Scope):
        self.scope = scope
        self.operations: List[Operation] = []
    
    def append(self, op: Operation):
        self.operations.append(op)
    
    def make_temp_var(self, loc: Loc) -> Var:
        return self.scope.make_var("$tmp", loc)
    
    def __iter__(self):
        return iter(self.operations)
    
    def __len__(self):
        return len(self.operations)


@dataclass
class Function:
    """A Python IR function."""
    name: str
    root_block: Block
    parameters: Tuple[Var, ...]
    return_value: Optional[Var]
    loc: Loc
    annotations: Dict[str, Any] = field(default_factory=dict)
    
    def __repr__(self):
        params = ", ".join(p.name for p in self.parameters)
        return f"func {self.name}({params})"

