"""
Tensor to Vector Pass (Python Implementation)
==============================================

Python implementation of the Tensor → Vector lowering pass.
This applies all registered @nn_to_vector lowering functions.

In production, prefer the C++ VECTOR_PASS from nn-addon.
This Python version is for experimentation and debugging.
"""

from typing import Dict, Any, List
from ..core.registry import get_nn_to_vector_registry
from ..core.air_value import AIRValue


class Tensor2VectorPyPass:
    """
    Python implementation of Tensor → Vector lowering.
    
    This pass:
    1. Iterates over all operations in topological order
    2. For each nn::core operation, looks up the lowering function
    3. Executes the lowering function with AIRValue wrappers
    4. Replaces the operation with the lowered result
    
    Usage:
        pass_instance = Tensor2VectorPyPass()
        pass_instance.run(air_module)
    """
    
    def __init__(self, verbose: bool = False):
        self.verbose = verbose
        self._replacement_map: Dict[Any, AIRValue] = {}
        self._container = None
    
    def run(self, air_module):
        """
        Run the pass on an AIR module.
        
        Args:
            air_module: The AIR module to transform
        """
        if self.verbose:
            print("=== Running Tensor2VectorPyPass ===")
        
        # Get registered lowering functions
        registry = get_nn_to_vector_registry()
        
        if self.verbose:
            print(f"Registered lowering functions: {list(registry.keys())}")
        
        # Process each function in the module
        if hasattr(air_module, 'functions'):
            for func in air_module.functions:
                self._lower_function(func, registry)
        else:
            # Module might be a single function
            self._lower_function(air_module, registry)
        
        if self.verbose:
            print("=== Tensor2VectorPyPass complete ===")
    
    def _lower_function(self, func, registry: Dict):
        """Lower all operations in a function."""
        if self.verbose:
            func_name = func.name if hasattr(func, 'name') else str(func)
            print(f"Processing function: {func_name}")
        
        # Get container for emitting new nodes
        if hasattr(func, 'container'):
            self._container = func.container
        else:
            raise RuntimeError("Function has no container - cannot lower operations")
        
        # Get statements/operations to process
        if hasattr(func, 'stmt_list'):
            stmts = list(func.stmt_list())
        elif hasattr(func, 'body'):
            stmts = list(func.body)
        elif hasattr(func, 'operations'):
            stmts = list(func.operations)
        else:
            if self.verbose:
                print("  No statements found")
            return
        
        # Process each statement
        for stmt in stmts:
            self._lower_statement(stmt, registry)
    
    def _lower_statement(self, stmt, registry: Dict):
        """Lower a single statement."""
        # Get the operation node
        if hasattr(stmt, 'node'):
            node = stmt.node()
        else:
            node = stmt
        
        # Get operation name
        if hasattr(node, 'opcode_name'):
            op_name = node.opcode_name()
        elif hasattr(node, 'op'):
            op_name = node.op
        else:
            return
        
        if self.verbose:
            print(f"  Processing: {op_name}")
        
        # Look up lowering function
        if op_name in registry:
            info = registry[op_name]
            lowering_func = info.func
            
            # Get input values (use replacement map for already-lowered)
            input_values = self._get_input_values(node)
            
            # Execute lowering function
            result = self._inline_lowering(lowering_func, input_values)
            
            # Store replacement
            if result is not None:
                self._replacement_map[node] = result
                
                if self.verbose:
                    print(f"    Lowered: {op_name} → {result}")
    
    def _get_input_values(self, node) -> List[AIRValue]:
        """Get input values, checking replacement map first."""
        inputs = []
        
        if hasattr(node, 'children'):
            for child in node.children():
                if child in self._replacement_map:
                    inputs.append(self._replacement_map[child])
                else:
                    inputs.append(AIRValue(child, self._container))
        elif hasattr(node, 'operands'):
            for key, val in node.operands.items():
                if key in ('lhs', 'rhs', 'operand'):
                    if val in self._replacement_map:
                        inputs.append(self._replacement_map[val])
                    else:
                        inputs.append(AIRValue(val, self._container))
        
        return inputs
    
    def _inline_lowering(self, func, inputs: List[AIRValue]) -> AIRValue:
        """Execute a lowering function with inputs."""
        try:
            if len(inputs) == 0:
                return func()
            elif len(inputs) == 1:
                return func(inputs[0])
            elif len(inputs) == 2:
                return func(inputs[0], inputs[1])
            elif len(inputs) == 3:
                return func(inputs[0], inputs[1], inputs[2])
            else:
                return func(*inputs)
        except Exception as e:
            if self.verbose:
                print(f"    Error in lowering: {e}")
            return None
    
    def get_lowered_nodes(self) -> List:
        """Get all lowered nodes (for testing)."""
        if self._container and hasattr(self._container, 'get_nodes'):
            return self._container.get_nodes()
        return []


def run_tensor2vector_pass(module, verbose: bool = False):
    """Convenience function to run the pass."""
    pass_instance = Tensor2VectorPyPass(verbose=verbose)
    pass_instance.run(module)
    return pass_instance.get_lowered_nodes()

