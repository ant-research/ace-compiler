"""
Dead Code Elimination Pass
==========================

Removes unused operations from the IR.
Based on CuTile's DCE approach.
"""

from typing import Dict, List, Set, Tuple
from collections import defaultdict

from base_dsl.python_ir import (
    Function, Block, Operation, Var,
    Load, Store, Const, BinOp, UnaryOp, Call, Return, ForLoop, If
)


class DeadCodeEliminationPass:
    """
    Dead code elimination pass.
    
    Removes operations whose results are never used.
    Handles control flow correctly.
    """
    
    def __init__(self, func: Function):
        self.func = func
        self.graph: Dict[str, List[str]] = {}  # dependency graph
        self.used: Set[str] = set()  # used variables
    
    def run(self) -> None:
        """Run DCE on the function."""
        # Build dependency graph
        self._build_graph(self.func.root_block)
        
        # Find all used variables (transitively)
        self._find_used()
        
        # Prune unused operations
        self._prune_block(self.func.root_block)
    
    def _build_graph(self, block: Block):
        """Build the dependency graph for a block."""
        for op in block.operations:
            op_type = type(op).__name__
            
            # Get all input variables
            inputs = self._get_inputs(op)
            
            # Operations with side effects are always used
            if self._has_side_effect(op):
                self.used.update(inputs)
            
            # Add edges for result variables
            for result in op.result_vars:
                self.graph[result.name] = inputs
            
            # Handle nested blocks
            if op_type == 'ForLoop':
                if op.nested_blocks:
                    self._build_graph(op.nested_blocks[0])
            elif op_type == 'If':
                for nested in op.nested_blocks:
                    self._build_graph(nested)
    
    def _get_inputs(self, op: Operation) -> List[str]:
        """Get all input variable names for an operation."""
        inputs = []
        
        for key, val in op.operands.items():
            if isinstance(val, Var):
                inputs.append(val.name)
            elif isinstance(val, tuple):
                for v in val:
                    if isinstance(v, Var):
                        inputs.append(v.name)
        
        return inputs
    
    def _has_side_effect(self, op: Operation) -> bool:
        """Check if operation has side effects (must not be removed)."""
        op_type = type(op).__name__
        return op_type in ('Return', 'Store', 'ForLoop', 'If')
    
    def _find_used(self):
        """Find all transitively used variables."""
        pending = list(self.used)
        
        while pending:
            var = pending.pop()
            for dep in self.graph.get(var, []):
                if dep not in self.used:
                    self.used.add(dep)
                    pending.append(dep)
    
    def _prune_block(self, block: Block):
        """Remove unused operations from a block."""
        new_ops = []
        
        for op in block.operations:
            op_type = type(op).__name__
            
            # Check if any result is used
            keep = False
            if self._has_side_effect(op):
                keep = True
            elif op.result_vars:
                keep = any(r.name in self.used for r in op.result_vars)
            
            if keep:
                # Prune nested blocks
                if op_type == 'ForLoop' and op.nested_blocks:
                    self._prune_block(op.nested_blocks[0])
                elif op_type == 'If':
                    for nested in op.nested_blocks:
                        self._prune_block(nested)
                
                new_ops.append(op)
        
        block.operations = new_ops


def eliminate_dead_code(func: Function) -> None:
    """
    Run dead code elimination on a Python IR function.
    
    Modifies the function in place.
    
    Args:
        func: The Python IR Function to optimize
    """
    pass_ = DeadCodeEliminationPass(func)
    pass_.run()

