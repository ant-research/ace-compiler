# ast_converter.py
import ast
import inspect
from typing import Dict, List, Any, Optional

from ...representations.graph import FHEGraph, BasicBlock, IRNode
from ...representations.fhe_program import FHEProgram

class ASTToIRConverter:
    """Convert Python AST to IR"""
    
    def __init__(self):
        self.current_block: Optional[BasicBlock] = None
        self.symbol_table: Dict[str, str] = {}
        self.graph: Optional[FHEGraph] = None
    
    def convert_function(self, func, graph_name: str = None) -> FHEGraph:
        """Converting a single function"""
        if not inspect.isfunction(func):
            raise TypeError("Input must be a function")
        
        if graph_name is None:
            graph_name = f"{func.__name__}_graph"
        
        try:
            source_code = inspect.getsource(func)
            filename = inspect.getfile(func)
        except (OSError, TypeError):
            source_code = f"def {func.__name__}(): pass"
            filename = "<dynamic>"
        
        tree = ast.parse(source_code, filename=filename)
        
        func_node = None
        for node in tree.body:
            if isinstance(node, ast.FunctionDef) and node.name == func.__name__:
                func_node = node
                break
        
        if func_node is None:
            raise ValueError(f"Function {func.__name__} not found in source")
        
        return self._convert_function(func_node, graph_name)

    def convert_module_from_source(self, source_code: str, filename: str = "<string>") -> FHEProgram:
        """Convert entire modules from source strings"""
        tree = ast.parse(source_code, filename=filename)
        return self._convert_module(tree)

    def convert_module_from_object(self, module_obj) -> FHEProgram:
        """Converting from a Python module object"""
        source = inspect.getsource(module_obj)
        tree = ast.parse(source)
        return self._convert_module(tree)

    def _convert_module(self, module_ast: ast.Module) -> FHEProgram:
        """Converting AST modules"""
        ir_module = FHEProgram("python_module")
        
        for node in module_ast.body:
            if isinstance(node, ast.FunctionDef):
                graph = self._convert_function(node, f"{node.name}_graph")
                ir_module.add_function(node.name, graph)
            # Ignore other nodes (import, class, etc.)
        
        return ir_module

    def _convert_function(self, func_node: ast.FunctionDef, graph_name: str) -> FHEGraph:
        """Converting a single function"""
        self.graph = FHEGraph(graph_name)
        
        # Creating the entry block
        entry_block = BasicBlock("entry")
        self.current_block = entry_block
        self.graph.add_block(entry_block)
        
        # Processing input parameters
        input_names = []
        for arg in func_node.args.args:
            input_node = IRNode(arg.arg)
            input_node.op_type = "input"
            input_node.inputs = []
            input_node.outputs = [arg.arg]
            input_node.attributes = {}
            entry_block.nodes.append(input_node)
            input_names.append(arg.arg)
        
        self.graph.input_nodes = input_names
        self.symbol_table = {name: name for name in input_names}
        
        # Conversion function body
        final_result = self._convert_stmts(func_node.body)
        
        # Setting the output
        output_node_name = "output"
        if final_result is not None:
            output_node = IRNode(output_node_name)
            output_node.op_type = "output"
            output_node.inputs = [final_result.name]
            output_node.outputs = [output_node_name]
            output_node.attributes = {}
            self.current_block.nodes.append(output_node)
            self.graph.output_nodes = [final_result.name]
        else:
            output_node = IRNode(output_node_name)
            output_node.op_type = "output"
            output_node.inputs = []
            output_node.outputs = [output_node_name]
            output_node.attributes = {}
            self.current_block.nodes.append(output_node)
            self.graph.output_nodes = []
        
        self.graph.entry_block = entry_block
        self.graph.metadata = {
            "source": "python_ast",
            "block_count": len(self.graph.blocks),
            "input_count": len(input_names),
            "output_count": len(self.graph.output_nodes)
        }
        
        return self.graph

    def _convert_stmts(self, stmts: List[ast.stmt]) -> Optional[IRNode]:
        """Converting a list of statements"""
        result = None
        for stmt in stmts:
            if isinstance(stmt, ast.Return):
                if stmt.value is not None:
                    result = self._convert_expr(stmt.value)
                break
            elif isinstance(stmt, ast.Assign):
                self._convert_assign(stmt)
            elif isinstance(stmt, ast.If):
                result = self._convert_if(stmt)
            elif isinstance(stmt, ast.For):
                self._convert_for(stmt)
            elif isinstance(stmt, ast.Expr):
                # Skip expression statements (e.g., docstrings)
                # These are standalone expressions that don't affect control flow
                pass
            elif isinstance(stmt, ast.Pass):
                # Skip pass statements (no-op)
                pass
            else:
                raise NotImplementedError(f"Unsupported statement: {type(stmt).__name__}")
        return result

    def _convert_assign(self, stmt: ast.Assign):
        if len(stmt.targets) != 1:
            raise NotImplementedError("Multiple assignment not supported")
        target = stmt.targets[0]
        if not isinstance(target, ast.Name):
            raise NotImplementedError(f"Unsupported assignment target: {type(target)}")
        
        value_node = self._convert_expr(stmt.value)
        var_name = target.id
        self.symbol_table[var_name] = value_node.name

    def _convert_if(self, stmt: ast.If) -> Optional[IRNode]:
        """Converting an if statement"""
        cond_node = self._convert_expr(stmt.test)
        self.current_block.nodes.append(cond_node)
        
        then_block = BasicBlock(f"if_then_{len(self.graph.blocks)}")
        else_block = BasicBlock(f"if_else_{len(self.graph.blocks)}")
        merge_block = BasicBlock(f"if_merge_{len(self.graph.blocks)}")
        
        self.graph.add_block(then_block)
        self.graph.add_block(else_block)
        self.graph.add_block(merge_block)
        
        branch_node = IRNode("cond_branch")
        branch_node.op_type = "cond_branch"
        branch_node.inputs = [cond_node.name]
        branch_node.attributes = {
            "true_target": then_block.name,
            "false_target": else_block.name
        }
        self.current_block.nodes.append(branch_node)
        self.current_block.successors = [then_block, else_block]
        then_block.predecessors = [self.current_block]
        else_block.predecessors = [self.current_block]
        
        old_symbol_table = self.symbol_table.copy()
        old_block = self.current_block
        
        # Then branch
        self.current_block = then_block
        then_result = self._convert_stmts(stmt.body)
        jump_node = IRNode("branch")
        jump_node.op_type = "branch"
        jump_node.attributes = {"target": merge_block.name}
        self.current_block.nodes.append(jump_node)
        self.current_block.successors = [merge_block]
        merge_block.predecessors.append(self.current_block)
        
        # Else branch
        self.symbol_table = old_symbol_table.copy()
        self.current_block = else_block
        else_result = None
        if stmt.orelse:
            else_result = self._convert_stmts(stmt.orelse)
        jump_node2 = IRNode("branch")
        jump_node2.op_type = "branch"
        jump_node2.attributes = {"target": merge_block.name}
        self.current_block.nodes.append(jump_node2)
        self.current_block.successors = [merge_block]
        merge_block.predecessors.append(self.current_block)
        
        # Merging symbol tables
        merged_symbol_table = old_symbol_table.copy()
        new_vars = set(self.symbol_table.keys()) - set(old_symbol_table.keys())
        for var in new_vars:
            phi_node = IRNode(f"phi_{var}")
            phi_node.op_type = "phi"
            phi_node.inputs = [
                self.symbol_table.get(var, var),
                old_symbol_table.get(var, var)
            ]
            phi_node.outputs = [var]
            phi_node.attributes = {
                "incoming_blocks": [then_block.name, else_block.name]
            }
            merge_block.nodes.append(phi_node)
            merged_symbol_table[var] = var
        
        self.symbol_table = merged_symbol_table
        self.current_block = merge_block
        
        if then_result is not None or else_result is not None:
            if then_result and else_result:
                phi_result = IRNode("result_phi")
                phi_result.op_type = "phi"
                phi_result.inputs = [then_result.name, else_result.name]
                phi_result.outputs = ["result"]
                phi_result.attributes = {
                    "incoming_blocks": [then_block.name, else_block.name]
                }
                merge_block.nodes.append(phi_result)
                return phi_result
            return then_result or else_result
        return None

    def _convert_for(self, stmt: ast.For):
        """Conversion for i in range(n)"""
        if not isinstance(stmt.target, ast.Name):
            raise NotImplementedError("For loop target must be a name")
        
        if not isinstance(stmt.iter, ast.Call) or \
           not isinstance(stmt.iter.func, ast.Name) or \
           stmt.iter.func.id != "range":
            raise NotImplementedError("Only 'for i in range(n)' supported")
        
        if len(stmt.iter.args) != 1:
            raise NotImplementedError("Only range(n) supported")
        
        target_var = stmt.target.id
        n_node = self._convert_expr(stmt.iter.args[0])
        self.current_block.nodes.append(n_node)
        
        zero_node = self._create_constant(0)
        self.current_block.nodes.append(zero_node)
        self.symbol_table[target_var] = zero_node.name
        
        loop_cond = BasicBlock(f"loop_cond_{len(self.graph.blocks)}")
        loop_body = BasicBlock(f"loop_body_{len(self.graph.blocks)}")
        loop_exit = BasicBlock(f"loop_exit_{len(self.graph.blocks)}")
        
        self.graph.add_block(loop_cond)
        self.graph.add_block(loop_body)
        self.graph.add_block(loop_exit)
        
        jump_init = IRNode("branch")
        jump_init.op_type = "branch"
        jump_init.attributes = {"target": loop_cond.name}
        self.current_block.nodes.append(jump_init)
        self.current_block.successors = [loop_cond]
        loop_cond.predecessors = [self.current_block]
        
        self.current_block = loop_cond
        cmp_node = IRNode("less")
        cmp_node.op_type = "less"
        cmp_node.inputs = [self.symbol_table[target_var], n_node.name]
        cmp_node.outputs = [self.graph.generate_unique_name("cmp")]
        loop_cond.nodes.append(cmp_node)
        
        branch_cond = IRNode("cond_branch")
        branch_cond.op_type = "cond_branch"
        branch_cond.inputs = [cmp_node.outputs[0]]
        branch_cond.attributes = {
            "true_target": loop_body.name,
            "false_target": loop_exit.name
        }
        loop_cond.nodes.append(branch_cond)
        loop_cond.successors = [loop_body, loop_exit]
        loop_body.predecessors = [loop_cond]
        loop_exit.predecessors = [loop_cond]
        
        self.current_block = loop_body
        self._convert_stmts(stmt.body)
        
        one_node = self._create_constant(1)
        add_node = IRNode("add")
        add_node.op_type = "add"
        add_node.inputs = [self.symbol_table[target_var], one_node.name]
        add_node.outputs = [self.graph.generate_unique_name("inc")]
        loop_body.nodes.extend([one_node, add_node])
        self.symbol_table[target_var] = add_node.outputs[0]
        
        jump_back = IRNode("branch")
        jump_back.op_type = "branch"
        jump_back.attributes = {"target": loop_cond.name}
        loop_body.nodes.append(jump_back)
        loop_body.successors = [loop_cond]
        loop_cond.predecessors.append(loop_body)
        
        self.current_block = loop_exit

    def _create_constant(self, value) -> IRNode:
        const_name = self.graph.generate_unique_name("const")
        const_node = IRNode(const_name)
        const_node.op_type = "constant"
        const_node.inputs = []
        const_node.outputs = [const_name]
        const_node.attributes = {"value": value}
        return const_node

    def _convert_expr(self, expr: ast.expr) -> IRNode:
        if isinstance(expr, ast.BinOp):
            return self._convert_binop(expr)
        elif isinstance(expr, ast.UnaryOp):
            return self._convert_unaryop(expr)
        elif isinstance(expr, ast.Compare):
            return self._convert_compare(expr)
        elif isinstance(expr, ast.Name):
            return self._convert_name(expr)
        elif isinstance(expr, ast.Constant):
            return self._convert_constant(expr)
        elif hasattr(ast, 'Num') and isinstance(expr, ast.Num):
            return self._convert_constant_old(expr)
        elif isinstance(expr, ast.Call):
            return self._convert_call(expr)
        else:
            return self._convert_unsupported(expr)

    def _convert_binop(self, expr: ast.BinOp) -> IRNode:
        left_node = self._convert_expr(expr.left)
        right_node = self._convert_expr(expr.right)
        
        op_map = {
            ast.Add: "add",
            ast.Sub: "sub",
            ast.Mult: "mul",
            ast.Div: "div",
        }
        op_name = op_map.get(type(expr.op), "custom_op")
        output_name = self.graph.generate_unique_name(op_name)
        
        op_node = IRNode(output_name)
        op_node.op_type = op_name
        op_node.inputs = [left_node.name, right_node.name]
        op_node.outputs = [output_name]
        op_node.attributes = {}
        
        # Key: add to current block
        self.current_block.nodes.append(op_node)

        return op_node

    def _convert_unaryop(self, expr: ast.UnaryOp) -> IRNode:
        operand_node = self._convert_expr(expr.operand)

        op_map = {
            ast.USub: "neg",
            ast.UAdd: "identity",
        }
        op_name = op_map.get(type(expr.op), "unsupported_unary")

        if op_name == "identity":
            return operand_node

        output_name = self.graph.generate_unique_name(op_name)

        op_node = IRNode(output_name)
        op_node.op_type = op_name
        op_node.inputs = [operand_node.name]
        op_node.outputs = [output_name]
        op_node.attributes = {}

        self.current_block.nodes.append(op_node)

        return op_node

    def _convert_compare(self, expr: ast.Compare) -> IRNode:
        """
        Conversion comparison expressions: x > 0, a <= b, etc.
        Supports single comparisons (e.g., x > 0), not chained comparisons (e.g., 1 < x < 10).
        """
        if len(expr.ops) != 1 or len(expr.comparators) != 1:
            raise NotImplementedError("Only single comparisons supported (e.g., x > 0)")
        
        left_node = self._convert_expr(expr.left)
        right_node = self._convert_expr(expr.comparators[0])
        
        # Map comparison operator
        cmp_map = {
            ast.Gt: "greater",
            ast.Lt: "less",
            ast.GtE: "greater_equal",
            ast.LtE: "less_equal",
            ast.Eq: "equal",
            ast.NotEq: "not_equal",
        }
        
        op_type = cmp_map.get(type(expr.ops[0]))
        if op_type is None:
            raise NotImplementedError(f"Unsupported comparison operator: {type(expr.ops[0])}")
        
        output_name = self.graph.generate_unique_name(op_type)
        cmp_node = IRNode(output_name)
        cmp_node.op_type = op_type
        cmp_node.inputs = [left_node.name, right_node.name]
        cmp_node.outputs = [output_name]
        cmp_node.attributes = {}
        
        self.current_block.nodes.append(cmp_node)
        return cmp_node

    def _convert_name(self, expr: ast.Name) -> IRNode:
        var_name = expr.id
        if var_name not in self.symbol_table:
            raise NameError(f"Undefined variable: '{var_name}'")
        ref_node = IRNode(var_name)
        ref_node.op_type = "ref"
        ref_node.outputs = [var_name]
        return ref_node

    def _convert_call(self, call: ast.Call) -> IRNode:
        """Converting function calls - Support for torch.relu(x) and direct calls"""
        
        if isinstance(call.func, ast.Attribute):
            # Handle attribute access calls: torch.relu(x) or x.reu ()
            if isinstance(call.func.value, ast.Name):
                # torch.relu(x) - Module function calls
                module_name = call.func.value.id
                func_name = call.func.attr
                ir_op_name = self._normalize_torch_function(module_name, func_name)
            elif isinstance(call.func.value, ast.Call):
                # Nested calls, temporarily not supported
                raise NotImplementedError("Nested function calls not supported")
            else:
                # x.relu() - Method calls
                method_name = call.func.attr
                ir_op_name = f"method.{method_name}"
                
        elif isinstance(call.func, ast.Name):
            # Direct function call: relu(x)
            func_name = call.func.id
            ir_op_name = func_name
            
        else:
            raise NotImplementedError(f"Unsupported function call type: {type(call.func)}")
        
        # Conversion parameters
        arg_nodes = []
        for arg in call.args:
            arg_node = self._convert_expr(arg)
            arg_nodes.append(arg_node)
        
        # Creating a calling node
        call_name = self.graph.generate_unique_name(f"call_{ir_op_name}")
        call_node = IRNode(call_name)
        call_node.op_type = ir_op_name
        call_node.inputs = [arg.name for arg in arg_nodes]
        call_node.outputs = [call_name]
        call_node.attributes = {"original_call": self._get_original_call_str(call)}
        
        self.current_block.nodes.append(call_node)
        return call_node
    
    def _normalize_torch_function(self, module_name: str, func_name: str) -> str:
        """Map PyTorch function names to standard IR operation names"""
        if module_name == "torch":
            torch_mapping = {
                "relu": "relu",
                "sigmoid": "sigmoid", 
                "tanh": "tanh",
                "add": "add",
                "sub": "sub",
                "mul": "mul",
                "div": "div",
                "matmul": "matmul",
                "transpose": "transpose",
                "sum": "reduce_sum",
                "mean": "reduce_mean",
                "max": "reduce_max",
                "min": "reduce_min",
                "abs": "abs",
                "sqrt": "sqrt",
                "exp": "exp",
                "log": "log",
                # Go ahead and add the functions...
            }
            return torch_mapping.get(func_name, f"torch.{func_name}")
        else:
            return f"{module_name}.{func_name}"
        
    def _get_original_call_str(self, call: ast.Call) -> str:
        """Get the raw call string for debugging"""
        try:
            if isinstance(call.func, ast.Attribute):
                if isinstance(call.func.value, ast.Name):
                    return f"{call.func.value.id}.{call.func.attr}"
                else:
                    return f"<expr>.{call.func.attr}"
            elif isinstance(call.func, ast.Name):
                return call.func.id
            else:
                return "<unknown>"
        except:
            return "<unknown>"

    def _convert_constant(self, expr: ast.Constant) -> IRNode:
        const_node = self._create_constant(expr.value)
        self.current_block.nodes.append(const_node)
        return const_node
    
    def _convert_constant_old(self, expr: ast.Num) -> IRNode:
        const_node = self._create_constant(expr.n)
        self.current_block.nodes.append(const_node)
        return const_node
    
    def _convert_unsupported(self, expr: ast.expr) -> IRNode:
        unknown_name = self.graph.generate_unique_name("unknown")
        unknown_node = IRNode(unknown_name)
        unknown_node.op_type = "unsupported"
        unknown_node.inputs = []
        unknown_node.outputs = [unknown_name]
        unknown_node.attributes = {"ast_type": type(expr).__name__}
        self.current_block.nodes.append(unknown_node)
        return unknown_node