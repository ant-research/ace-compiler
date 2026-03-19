"""
Base DSL Infrastructure
=======================

Core infrastructure shared by all DSL implementations.
"""

from .loc import Loc, get_caller_loc, get_current_loc, set_current_loc, source_location
from .python_ir import Scope, Var, Block, Function, Operation
from .python_ir import Load, Store, Const, BinOp, Call, Return, ForLoop
from .ast_to_ir import get_function_ir, ast2ir, Context
from .compiler import Compiler, CompilationError, get_tmpdir, is_dryrun, is_trace_ir

__all__ = [
    # Location tracking
    "Loc",
    "get_caller_loc",
    "get_current_loc",
    "set_current_loc",
    "source_location",
    # Python IR
    "Scope",
    "Var",
    "Block",
    "Function",
    "Operation",
    "Load",
    "Store",
    "Const",
    "BinOp",
    "Call",
    "Return",
    "ForLoop",
    # AST to IR
    "get_function_ir",
    "ast2ir",
    "Context",
    # Compiler
    "Compiler",
    "CompilationError",
    "get_tmpdir",
    "is_dryrun",
    "is_trace_ir",
]

