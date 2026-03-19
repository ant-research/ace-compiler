"""
Compiler stub for ACE EDSL.

ACE EDSL uses AIR for IR generation and C code generation (via poly2c),
not traditional compilation backends. This module provides minimal stubs
for BaseDSL interface compatibility.
"""

from typing import Sequence, Optional, Tuple, Any, Dict
import inspect
from .common import DSLRuntimeError


class CompilationError(RuntimeError):
    """Custom error class for compilation failures."""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(message)
        self.details = kwargs


class Compiler:
    """
    Stub compiler class for BaseDSL interface compatibility.
    
    ACE EDSL doesn't use this - it generates AIR and uses poly2c for C code generation.
    """

    def __init__(self, passmanager=None, execution_engine=None):
        self.passmanager = passmanager
        self.execution_engine = execution_engine

    def __call__(self, module):
        """Convenience application method."""
        raise NotImplementedError("ACE EDSL uses AIR and poly2c, not traditional compilation")

    def compile(self, module, pipeline: str, **kwargs):
        """Stub compile method."""
        raise NotImplementedError("ACE EDSL uses AIR and poly2c, not traditional compilation")

    def jit(self, module, **kwargs):
        """Stub JIT method."""
        raise NotImplementedError("ACE EDSL uses AIR and poly2c, not traditional compilation")

    def compile_and_jit(self, module, pipeline: str, **kwargs):
        """Stub compile and JIT method."""
        raise NotImplementedError("ACE EDSL uses AIR and poly2c, not traditional compilation")


def compile(func, *args, **kwargs):
    """
    Compile a decorated function.
    
    For ACE EDSL, this is not used - AIR is generated via operator overloading
    and C code via poly2c.
    """
    if func is None:
        raise DSLRuntimeError("Function is not set or invalid.")

    if not callable(func):
        raise DSLRuntimeError("Object is not callable.")

    kwargs["compile_only"] = True
    kwargs["no_cache"] = True

    if inspect.isfunction(func):
        pass
    elif inspect.ismethod(func):
        args = [func.__self__] + list(args)
        func = func.__func__
    elif inspect.isclass(type(func)) and hasattr(func, "__call__"):
        args = [func] + list(args)
        func = func.__call__.__func__
    else:
        raise DSLRuntimeError(
            "Invalid function type, only function, method and module are supported"
        )

    if hasattr(func, "__wrapped__"):
        func = func.__wrapped__

    if not hasattr(func, "dsl_object"):
        raise DSLRuntimeError("Function is not decorated with jit decorator.")

    return func.dsl_object._func(func, *args, **kwargs)
