"""
AST to Python IR Conversion
===========================

Stage 1 of the two-stage transformation:
Python AST → Python IR → AIR

This module parses Python source code and converts it to Python IR.
"""

import ast
import inspect
from typing import Dict, Callable, Any, Optional, List

from .loc import Loc
from .python_ir import (
    Scope, Var, Block, Function, Operation,
    Load, Store, Const, BinOp, UnaryOp, Call, Return, ForLoop, If
)


class Context:
    """Parsing context with source info and globals."""
    
    def __init__(self, filename: str, first_line: int, globals_dict: dict,
                 source_lines: List[str] = None):
        self.filename = filename
        self.first_line = first_line
        self.globals = globals_dict
        self._source_lines = source_lines
    
    def get_loc(self, node: ast.AST) -> Loc:
        """Extract source location from AST node."""
        line = self.first_line + node.lineno - 1
        end_line = self.first_line + (getattr(node, 'end_lineno', node.lineno) or node.lineno) - 1
        
        return Loc(
            line=line,
            col=getattr(node, 'col_offset', 0),
            filename=self.filename,
            end_line=end_line,
            end_col=getattr(node, 'end_col_offset', None)
        )
    
    def syntax_error(self, node: ast.AST, message: str) -> SyntaxError:
        """Create a SyntaxError with source location."""
        loc = self.get_loc(node)
        return SyntaxError(loc.format_error(message, self._source_lines))


# ═══════════════════════════════════════════════════════════════════════════════
# Expression Handlers Registry
# ═══════════════════════════════════════════════════════════════════════════════

_expr_handlers: Dict[type, Callable] = {}


def register_expr(node_type):
    """Decorator to register an expression handler."""
    def decorator(func):
        _expr_handlers[node_type] = func
        return func
    return decorator


def expr(node: ast.AST, block: Block, ctx: Context) -> Var:
    """Dispatch expression to handler."""
    handler = _expr_handlers.get(type(node))
    if handler is None:
        raise ctx.syntax_error(node, f"Unsupported expression type: {type(node).__name__}")
    return handler(node, block, ctx)


@register_expr(ast.Name)
def handle_name(node: ast.Name, block: Block, ctx: Context) -> Var:
    """Handle variable reference: x, h, etc."""
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(Load(node.id, result, ctx.get_loc(node)))
    return result


@register_expr(ast.Constant)
def handle_constant(node: ast.Constant, block: Block, ctx: Context) -> Var:
    """Handle literal: 1, 3.14, "hello", etc."""
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(Const(node.value, result, ctx.get_loc(node)))
    return result


# Python 3.7 compatibility
@register_expr(ast.Num)
def handle_num(node: ast.Num, block: Block, ctx: Context) -> Var:
    """Handle numeric literal (Python 3.7)."""
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(Const(node.n, result, ctx.get_loc(node)))
    return result


@register_expr(ast.Str)
def handle_str(node: ast.Str, block: Block, ctx: Context) -> Var:
    """Handle string literal (Python 3.7)."""
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(Const(node.s, result, ctx.get_loc(node)))
    return result


# Binary operation mapping
_binop_map = {
    ast.Add: "add",
    ast.Sub: "sub",
    ast.Mult: "mul",
    ast.Div: "truediv",
    ast.FloorDiv: "floordiv",
    ast.Mod: "mod",
    ast.Pow: "pow",
    ast.MatMult: "matmul",
    ast.LShift: "lshift",
    ast.RShift: "rshift",
    ast.BitOr: "bitor",
    ast.BitXor: "bitxor",
    ast.BitAnd: "bitand",
}


@register_expr(ast.BinOp)
def handle_binop(node: ast.BinOp, block: Block, ctx: Context) -> Var:
    """Handle binary operations: a + b, x * y, etc."""
    lhs = expr(node.left, block, ctx)
    rhs = expr(node.right, block, ctx)
    
    op_name = _binop_map.get(type(node.op))
    if op_name is None:
        raise ctx.syntax_error(node, f"Unsupported binary operator: {type(node.op).__name__}")
    
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(BinOp(op_name, lhs, rhs, result, ctx.get_loc(node)))
    return result


# Unary operation mapping
_unaryop_map = {
    ast.UAdd: "pos",
    ast.USub: "neg",
    ast.Not: "not",
    ast.Invert: "invert",
}


@register_expr(ast.UnaryOp)
def handle_unaryop(node: ast.UnaryOp, block: Block, ctx: Context) -> Var:
    """Handle unary operations: -x, not y, etc."""
    operand = expr(node.operand, block, ctx)
    
    op_name = _unaryop_map.get(type(node.op))
    if op_name is None:
        raise ctx.syntax_error(node, f"Unsupported unary operator: {type(node.op).__name__}")
    
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(UnaryOp(op_name, operand, result, ctx.get_loc(node)))
    return result


@register_expr(ast.Call)
def handle_call(node: ast.Call, block: Block, ctx: Context) -> Var:
    """Handle function calls: conv(x, w), relu(h), etc."""
    callee = expr(node.func, block, ctx)
    args = tuple(expr(arg, block, ctx) for arg in node.args)
    kwargs = {kw.arg: expr(kw.value, block, ctx) for kw in node.keywords if kw.arg}
    
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(Call(callee, args, kwargs, result, ctx.get_loc(node)))
    return result


@register_expr(ast.Attribute)
def handle_attribute(node: ast.Attribute, block: Block, ctx: Context) -> Var:
    """Handle attribute access: x.shape, tensor.dtype, etc."""
    value = expr(node.value, block, ctx)
    
    # Create a getattr-like operation
    result = block.make_temp_var(ctx.get_loc(node))
    attr_name_var = block.make_temp_var(ctx.get_loc(node))
    block.append(Const(node.attr, attr_name_var, ctx.get_loc(node)))
    
    # Use a special "getattr" call
    getattr_var = block.make_temp_var(ctx.get_loc(node))
    block.append(Load("getattr", getattr_var, ctx.get_loc(node)))
    block.append(Call(getattr_var, (value, attr_name_var), {}, result, ctx.get_loc(node)))
    return result


@register_expr(ast.Subscript)
def handle_subscript(node: ast.Subscript, block: Block, ctx: Context) -> Var:
    """Handle subscript access: x[i], tensor[0:10], etc."""
    value = expr(node.value, block, ctx)
    
    # Handle slice
    if isinstance(node.slice, ast.Slice):
        # x[start:stop:step]
        lower = expr(node.slice.lower, block, ctx) if node.slice.lower else None
        upper = expr(node.slice.upper, block, ctx) if node.slice.upper else None
        step = expr(node.slice.step, block, ctx) if node.slice.step else None
        # Create slice object (simplified)
        result = block.make_temp_var(ctx.get_loc(node))
        # For now, just emit as getitem
        block.append(Load("getitem", result, ctx.get_loc(node)))
        return result
    else:
        # x[idx]
        idx = expr(node.slice, block, ctx)
        result = block.make_temp_var(ctx.get_loc(node))
        getitem_var = block.make_temp_var(ctx.get_loc(node))
        block.append(Load("getitem", getitem_var, ctx.get_loc(node)))
        block.append(Call(getitem_var, (value, idx), {}, result, ctx.get_loc(node)))
        return result


@register_expr(ast.Tuple)
def handle_tuple(node: ast.Tuple, block: Block, ctx: Context) -> Var:
    """Handle tuple: (a, b, c)"""
    elements = [expr(elt, block, ctx) for elt in node.elts]
    result = block.make_temp_var(ctx.get_loc(node))
    
    tuple_var = block.make_temp_var(ctx.get_loc(node))
    block.append(Load("tuple", tuple_var, ctx.get_loc(node)))
    block.append(Call(tuple_var, tuple(elements), {}, result, ctx.get_loc(node)))
    return result


@register_expr(ast.List)
def handle_list(node: ast.List, block: Block, ctx: Context) -> Var:
    """Handle list: [a, b, c]"""
    elements = [expr(elt, block, ctx) for elt in node.elts]
    result = block.make_temp_var(ctx.get_loc(node))
    
    list_var = block.make_temp_var(ctx.get_loc(node))
    block.append(Load("list", list_var, ctx.get_loc(node)))
    block.append(Call(list_var, tuple(elements), {}, result, ctx.get_loc(node)))
    return result


# Compare operations
_cmpop_map = {
    ast.Eq: "eq",
    ast.NotEq: "ne",
    ast.Lt: "lt",
    ast.LtE: "le",
    ast.Gt: "gt",
    ast.GtE: "ge",
    ast.Is: "is",
    ast.IsNot: "is_not",
    ast.In: "in",
    ast.NotIn: "not_in",
}


@register_expr(ast.Compare)
def handle_compare(node: ast.Compare, block: Block, ctx: Context) -> Var:
    """Handle comparison: a < b, x == y, etc."""
    left = expr(node.left, block, ctx)
    
    # For simplicity, handle single comparison only
    if len(node.ops) == 1 and len(node.comparators) == 1:
        op_name = _cmpop_map.get(type(node.ops[0]))
        right = expr(node.comparators[0], block, ctx)
        result = block.make_temp_var(ctx.get_loc(node))
        block.append(BinOp(op_name, left, right, result, ctx.get_loc(node)))
        return result
    else:
        raise ctx.syntax_error(node, "Chained comparisons not supported")


@register_expr(ast.IfExp)
def handle_ifexp(node: ast.IfExp, block: Block, ctx: Context) -> Var:
    """Handle ternary expression: a if cond else b"""
    cond = expr(node.test, block, ctx)
    then_val = expr(node.body, block, ctx)
    else_val = expr(node.orelse, block, ctx)
    
    result = block.make_temp_var(ctx.get_loc(node))
    select_var = block.make_temp_var(ctx.get_loc(node))
    block.append(Load("select", select_var, ctx.get_loc(node)))
    block.append(Call(select_var, (cond, then_val, else_val), {}, result, ctx.get_loc(node)))
    return result


# ═══════════════════════════════════════════════════════════════════════════════
# Statement Handlers Registry
# ═══════════════════════════════════════════════════════════════════════════════

_stmt_handlers: Dict[type, Callable] = {}


def register_stmt(node_type):
    """Decorator to register a statement handler."""
    def decorator(func):
        _stmt_handlers[node_type] = func
        return func
    return decorator


def stmt(node: ast.AST, block: Block, ctx: Context):
    """Dispatch statement to handler."""
    handler = _stmt_handlers.get(type(node))
    if handler is None:
        raise ctx.syntax_error(node, f"Unsupported statement type: {type(node).__name__}")
    return handler(node, block, ctx)


@register_stmt(ast.Assign)
def handle_assign(node: ast.Assign, block: Block, ctx: Context):
    """Handle assignment: h = conv(x, w)"""
    value = expr(node.value, block, ctx)
    for target in node.targets:
        if isinstance(target, ast.Name):
            block.append(Store(target.id, value, ctx.get_loc(node)))
        elif isinstance(target, ast.Tuple):
            # Tuple unpacking (simplified)
            for i, elt in enumerate(target.elts):
                if isinstance(elt, ast.Name):
                    block.append(Store(elt.id, value, ctx.get_loc(node)))
        else:
            raise ctx.syntax_error(target, "Unsupported assignment target")


@register_stmt(ast.AugAssign)
def handle_augassign(node: ast.AugAssign, block: Block, ctx: Context):
    """Handle augmented assignment: x += 1"""
    if not isinstance(node.target, ast.Name):
        raise ctx.syntax_error(node, "Augmented assignment only supported for names")
    
    # Load current value
    current = block.make_temp_var(ctx.get_loc(node))
    block.append(Load(node.target.id, current, ctx.get_loc(node)))
    
    # Compute new value
    rhs = expr(node.value, block, ctx)
    op_name = _binop_map.get(type(node.op))
    result = block.make_temp_var(ctx.get_loc(node))
    block.append(BinOp(op_name, current, rhs, result, ctx.get_loc(node)))
    
    # Store back
    block.append(Store(node.target.id, result, ctx.get_loc(node)))


@register_stmt(ast.AnnAssign)
def handle_annassign(node: ast.AnnAssign, block: Block, ctx: Context):
    """Handle annotated assignment: x: int = 1"""
    if node.value:
        value = expr(node.value, block, ctx)
        if isinstance(node.target, ast.Name):
            block.append(Store(node.target.id, value, ctx.get_loc(node)))


@register_stmt(ast.Return)
def handle_return(node: ast.Return, block: Block, ctx: Context):
    """Handle return statement."""
    value = expr(node.value, block, ctx) if node.value else None
    block.append(Return(value, ctx.get_loc(node)))


@register_stmt(ast.Expr)
def handle_expr_stmt(node: ast.Expr, block: Block, ctx: Context):
    """Handle expression statement (e.g., function call with side effects)."""
    expr(node.value, block, ctx)


@register_stmt(ast.For)
def handle_for(node: ast.For, block: Block, ctx: Context):
    """Handle for loops."""
    if not isinstance(node.target, ast.Name):
        raise ctx.syntax_error(node, "For loop variable must be a simple name")
    
    iterable = expr(node.iter, block, ctx)
    body_block = Block(block.scope)
    
    for stmt_node in node.body:
        stmt(stmt_node, body_block, ctx)
    
    loop_op = ForLoop(node.target.id, iterable, body_block, ctx.get_loc(node))
    block.append(loop_op)


@register_stmt(ast.If)
def handle_if(node: ast.If, block: Block, ctx: Context):
    """Handle if statements."""
    condition = expr(node.test, block, ctx)
    
    then_block = Block(block.scope)
    for stmt_node in node.body:
        stmt(stmt_node, then_block, ctx)
    
    else_block = None
    if node.orelse:
        else_block = Block(block.scope)
        for stmt_node in node.orelse:
            stmt(stmt_node, else_block, ctx)
    
    if_op = If(condition, then_block, else_block, ctx.get_loc(node))
    block.append(if_op)


@register_stmt(ast.Pass)
def handle_pass(node: ast.Pass, block: Block, ctx: Context):
    """Handle pass statement (no-op)."""
    pass


@register_stmt(ast.FunctionDef)
def handle_functiondef(node: ast.FunctionDef, block: Block, ctx: Context):
    """Handle nested function definitions."""
    # For now, skip nested functions
    pass


# ═══════════════════════════════════════════════════════════════════════════════
# Main Entry Points
# ═══════════════════════════════════════════════════════════════════════════════

def ast2ir(func_def: ast.FunctionDef, scope: Scope, ctx: Context) -> Function:
    """Convert AST FunctionDef to Python IR Function."""
    # Create root block
    root_block = Block(scope)
    
    # Extract parameters
    param_vars = tuple(
        scope.make_var(arg.arg, ctx.get_loc(arg))
        for arg in func_def.args.args
    )
    
    # Extract type annotations
    annotations = {}
    for arg in func_def.args.args:
        if arg.annotation:
            # Evaluate annotation AST to get actual type object
            try:
                # Use compile() and eval() to evaluate the annotation
                # in the function's global namespace
                annotation_code = compile(
                    ast.Expression(arg.annotation),
                    filename='<annotation>',
                    mode='eval'
                )
                annotation_value = eval(annotation_code, ctx.globals)
                annotations[arg.arg] = annotation_value
                print(f"[DEBUG-AST2IR] Successfully evaluated annotation for {arg.arg}: {annotation_value}")
            except Exception as e:
                # Fallback: store AST node if evaluation fails
                print(f"[DEBUG-AST2IR] Failed to evaluate annotation for {arg.arg}: {e}")
                print(f"[DEBUG-AST2IR]   annotation AST: {ast.dump(arg.annotation)}")
                print(f"[DEBUG-AST2IR]   globals keys: {list(ctx.globals.keys())[:20]}")
                annotations[arg.arg] = arg.annotation

    if func_def.returns:
        try:
            return_code = compile(
                ast.Expression(func_def.returns),
                filename='<annotation>',
                mode='eval'
            )
            return_value = eval(return_code, ctx.globals)
            annotations['return'] = return_value
            print(f"[DEBUG-AST2IR] Successfully evaluated return annotation: {return_value}")
        except Exception as e:
            print(f"[DEBUG-AST2IR] Failed to evaluate return annotation: {e}")
            annotations['return'] = func_def.returns
    
    # Process function body
    for stmt_node in func_def.body:
        # Skip docstrings
        if isinstance(stmt_node, ast.Expr) and isinstance(stmt_node.value, (ast.Str, ast.Constant)):
            if isinstance(stmt_node.value, ast.Constant) and isinstance(stmt_node.value.value, str):
                continue
            if isinstance(stmt_node.value, ast.Str):
                continue
        stmt(stmt_node, root_block, ctx)
    
    return Function(
        name=func_def.name,
        root_block=root_block,
        parameters=param_vars,
        return_value=None,
        loc=ctx.get_loc(func_def),
        annotations=annotations
    )


def get_function_ir(pyfunc, scope: Scope = None) -> Function:
    """Convert Python function to Python IR."""
    if scope is None:
        scope = Scope()
    
    # Get source
    source_lines, first_line = inspect.getsourcelines(pyfunc)
    source = "".join(source_lines)
    
    # Parse AST
    tree = ast.parse(source)
    func_def = tree.body[0]
    
    if not isinstance(func_def, ast.FunctionDef):
        raise ValueError("Expected a function definition")
    
    ctx = Context(
        filename=inspect.getfile(pyfunc),
        first_line=first_line,
        globals_dict=pyfunc.__globals__,
        source_lines=source_lines
    )
    
    return ast2ir(func_def, scope, ctx)

