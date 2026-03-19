"""
BaseDSL - Base class for DSL implementations.

This module provides the BaseDSL class that ACE EDSL inherits from.
ACE EDSL only uses the AST preprocessing infrastructure from BaseDSL.
"""


# Standard library imports
from dataclasses import dataclass, field
import atexit
import os
import sys
import inspect
from functools import lru_cache, wraps
from abc import ABC, abstractmethod
from typing import Any
try:
    from typing import Annotated
except ImportError:
    from typing_extensions import Annotated
import warnings

from . import typing as t
from .env_manager import EnvironmentVarManager

# =============================================================================
# Local module imports
# =============================================================================

from .cache_helpers import *
from .jit_executor import JitExecutor
from .utils.timer import timer
from .utils.numpy import *
from .utils.logger import setup_log, log
from .utils.stacktrace import filter_exception, walk_to_top_module, filter_stackframe
from .runtime import Argument
from .runtime.tensor_descriptor import TensorDescriptor
from .ast_preprocessor import DSLPreprocessor
from .common import *


# =============================================================================
# Utility Functions
# =============================================================================

def const(value, dtype=None):
    """Const function - returns value as-is (no IR generation)."""
    return value


# =============================================================================
# Main DSL Class
# =============================================================================


class BaseDSL:
    """
    Base DSL class providing AST preprocessing infrastructure.
    
    ACE EDSL inherits from this class but only uses:
    1. AST preprocessing (transforming Python AST before execution)
    2. Decorator infrastructure (@jit, @kernel patterns)
    3. Environment management
    4. Logging infrastructure
    
    Subclasses must implement:
    - _kernel_helper: Handle kernel generation
    - _func: Handle function execution
    - _get_globals: Get globals for AST preprocessing
    """
    
    gpu_module = None

    def __init__(
        self,
        name: str,
        compiler_provider: Any,
        pass_sm_arch_name: str,
        device_compilation_only=False,
        preprocess=False,
    ):
        """
        Constructor for initializing the DSL.

        Parameters:
        - name (str): Name of DSL, used for environment variables and logging.
        - compiler_provider: Provider for compilation (stub for ACE EDSL).
        - pass_sm_arch_name (str): Architecture name (placeholder for ACE EDSL).
        - device_compilation_only (bool): Only device code (not used by ACE EDSL).
        - preprocess (bool): Enable AST transformation.
        """
        if not all([name, compiler_provider, pass_sm_arch_name]):
            raise DSLRuntimeError(
                "All required parameters must be provided and non-empty"
            )

        self.name = name
        self.compiler_provider = compiler_provider
        self.pass_sm_arch_name = pass_sm_arch_name
        self.frame = None
        self.no_cache = False
        self.device_compilation_only = device_compilation_only
        self.num_kernels = 0
        
        # Read environment variables
        self.envar = EnvironmentVarManager(self.name)
        self.preprocessed = False
        self.enable_preprocessor = preprocess
        
        # Cache (disabled for ACE EDSL - uses AIR, not JIT compilation)
        self.jit_cache = dict()

        self._tensor_capsules = []

        # Set warnings
        if self.envar.warnings_as_errors:
            warnings.filterwarnings("error")
        if self.envar.warnings_ignore:
            warnings.filterwarnings("ignore")

        # Initialize logger
        if self.envar.log_to_console == False and self.envar.jitTimeProfiling:
            self.envar.log_to_console = True
            self.envar.log_level = 10  # info level
        setup_log(
            self.name,
            self.envar.log_to_console,
            self.envar.log_to_file,
            f"{self.name}.log",
            self.envar.log_level,
        )

        self.kernel_symbols = []
        self.launch_inner_count = 0

        if preprocess:
            self.preprocessor = DSLPreprocessor()
            
        log().info(f"Initializing {name} DSL")
        log().debug(f"Logger initialized for {self.name}")

        # Hook excepthook for better error messages
        if self.envar.filterStacktrace:
            origin_excepthook = sys.excepthook
            module_dir = walk_to_top_module(os.path.dirname(os.path.abspath(__file__)))

            def excepthook(excep_type, value, traceback):
                filter_exception(value, module_dir)
                if hasattr(value, "__traceback__"):
                    origin_excepthook(excep_type, value, value.__traceback__)
                else:
                    origin_excepthook(
                        excep_type, value, filter_stackframe(traceback, module_dir)
                    )

            sys.excepthook = excepthook

            def restore_excepthook(hook):
                sys.excepthook = hook

            atexit.register(restore_excepthook, origin_excepthook)

    def dump_cache(self):
        """Dump cache to file (no-op for ACE EDSL)."""
        pass

    @lru_cache(maxsize=1)
    def print_warning_once(self, message):
        log().warning(f"Warning: {message}")
        warnings.warn(message, UserWarning)

    def print_warning(self, message):
        log().warning(f"Warning: {message}")
        warnings.warn(message, UserWarning)

    @classmethod
    @lru_cache(maxsize=1)
    def _get_dsl(cls):
        """Get singleton DSL instance."""
        main_dsl = cls()
        return main_dsl

    @staticmethod
    def _can_preprocess(**dkwargs):
        """Check if AST transformation is enabled."""
        return dkwargs.pop("preprocess", True)

    def jit_runner(self, frame, executor, *dargs, **dkwargs):
        """
        Decorator to mark a function for JIT compilation.
        
        For ACE EDSL, this sets up AST preprocessing and wraps the function
        to call the executor (which generates AIR).
        """
        self.frame = frame
        log().info("jit_runner")

        def jit_runner_decorator(func):
            func.dsl_object = self
            
            # Run preprocessor that alters AST
            preprocess = BaseDSL._can_preprocess(**dkwargs)
            if self.enable_preprocessor and preprocess:
                new_func = self.run_preprocessor(func)
                if new_func is not None:
                    new_func.dsl_object = self
                    # Preserve domain metadata from original function
                    if hasattr(func, "_py_domain"):
                        new_func._py_domain = func._py_domain
                    
                    @wraps(new_func)
                    def jit_wrapper_preprocessed(*args, **kwargs):
                        return executor(new_func, *args, **kwargs)
                    return jit_wrapper_preprocessed

            @wraps(func)
            def jit_wrapper(*args, **kwargs):
                return executor(func, *args, **kwargs)

            return jit_wrapper

        if len(dargs) == 1 and callable(dargs[0]):
            return jit_runner_decorator(dargs[0])
        else:
            return jit_runner_decorator

    @classmethod
    def jit(cls, *dargs, **dkwargs):
        """Decorator to mark a function for JIT compilation."""
        frame = inspect.currentframe().f_back
        main_dsl = cls._get_dsl()
        return main_dsl.jit_runner(frame, main_dsl._func, *dargs, **dkwargs)

    @classmethod
    def kernel(cls, *dargs, **dkwargs):
        """Decorator to mark a function as a kernel."""
        frame = inspect.currentframe().f_back
        main_dsl = cls._get_dsl()
        return main_dsl.jit_runner(frame, main_dsl._kernel_helper, *dargs, **dkwargs)

    @abstractmethod
    def _kernel_helper(self, func, *args, **kwargs):
        """Helper function to handle kernel generation logic."""
        pass

    @abstractmethod
    def _get_globals(self):
        """
        Get global and local variables for AST preprocessing.
        
        This combines globals from the current module and the caller's frame.
        """
        pass

    def _is_tensor_descriptor(self, maybe_tensor_descriptor) -> bool:
        """Check if value is a TensorDescriptor."""
        return isinstance(maybe_tensor_descriptor, TensorDescriptor)

    def run_preprocessor(self, funcBody):
        """
        Run AST preprocessor on the function.
        
        This transforms the Python AST before execution, enabling
        features like loop transformations and custom syntax.
        
        Each function is preprocessed independently and cached via
        funcBody._preprocessed attribute.
        """
        # Unwrap function if it's wrapped by decorators
        original_funcBody = funcBody
        while hasattr(original_funcBody, '__wrapped__'):
            original_funcBody = original_funcBody.__wrapped__
        
        # Check if this specific function was already preprocessed
        if hasattr(original_funcBody, "_preprocessed"):
            # Already preprocessed - return cached result if available
            if hasattr(original_funcBody, "_preprocessed_func"):
                return original_funcBody._preprocessed_func
            return None
        
        function_name = original_funcBody.__name__
        self.funcBody = original_funcBody
        log().info("Started preprocessing [%s]", function_name)
        exec_globals = self._get_globals()
        
        try:
            transformed_ast = self.preprocessor.transform(original_funcBody, exec_globals)
        except Exception as e:
            log().warning("Preprocessing failed for [%s]: %s", function_name, e)
            original_funcBody._preprocessed = True  # Mark as attempted
            return None
        
        if self.envar.print_after_preprocessor:
            log().info(
                f"# Printing unparsed AST after preprocess of func=`{function_name}` id=`{id(funcBody)}`"
            )
            DSLPreprocessor.print_ast(transformed_ast)

        func_ptr = self.preprocessor.exec(function_name, original_funcBody, exec_globals)
        
        # Cache the result on the function
        original_funcBody._preprocessed = True
        original_funcBody._preprocessed_func = func_ptr
        
        return func_ptr

    @lru_cache(maxsize=None)
    def _get_function_signature(self, func):
        """Get function signature, handling preprocessed functions."""
        try:
            return inspect.signature(func)
        except (OSError, TypeError, ValueError):
            # Preprocessed function created by exec()
            if hasattr(self, 'funcBody') and self.funcBody is not None:
                try:
                    return inspect.signature(self.funcBody)
                except (OSError, TypeError, ValueError):
                    pass
            # Fallback: minimal signature
            return inspect.Signature(parameters=[])

    def _check_arg_count(self, *args, **kwargs):
        """Validate argument count against function signature."""
        if not self.funcBody:
            raise DSLRuntimeError("Function body is not set.")

        sig = self._get_function_signature(self.funcBody)
        function_name = self.funcBody.__name__

        try:
            bound_args = sig.bind_partial(*args, **kwargs)
            bound_args.apply_defaults()
        except Exception as e:
            raise DSLRuntimeError(
                f"Failed to bind arguments to function `{function_name}` with signature `{sig}`",
                cause=e,
            )

        # Check if all non-default arguments are provided
        for param in sig.parameters.values():
            if (
                param.default is inspect.Parameter.empty
                and param.name not in bound_args.arguments
            ):
                raise DSLRuntimeError(
                    f"Missing required argument in `{function_name}`: '{param.name}'"
                )

    # =========================================================================
    # Abstract methods that subclasses must implement
    # =========================================================================

    @abstractmethod
    def _func(self, funcBody, *args, **kwargs):
        """Execute a JIT-decorated function."""
        pass

    def _build_gpu_module(self, attrs=None):
        """Build GPU module (no-op for ACE EDSL)."""
        pass

    def _get_pipeline(self, pipeline):
        """Get compilation pipeline (not used by ACE EDSL)."""
        return pipeline

    @staticmethod
    def log_additions(operands=None, types=None, arg_attrs=None):
        """Log additions to kernel operands/types."""
        if operands is not None and operands != []:
            log().debug("Added kernel_operands: [%s]", ", ".join(map(str, operands)))
        if types is not None:
            log().debug("Added kernel_types: [%s]", ", ".join(map(str, types)))
        if arg_attrs is not None:
            log().debug("Added kernel_arg_attrs: [%s]", ", ".join(map(str, arg_attrs)))

    # =========================================================================
    # Stub methods (not used by ACE EDSL but kept for interface compatibility)
    # =========================================================================

    def _generate_mlir_type_for_tensor_descriptor(self, tensor):
        """Stub - ACE EDSL uses AIR types instead."""
        raise NotImplementedError("ACE EDSL uses AIR, not MLIR")

    def _generate_executable_arg_for_tensor_descriptor(self, mlir_value=None, ptr_tensor_ty=None, tensor=None):
        """Stub - ACE EDSL uses AIR instead."""
        raise NotImplementedError("ACE EDSL uses AIR, not MLIR")

    def generate_execution_arguments(self, args, fop, function_name, args_spec):
        """
        Generate execution arguments.
        
        Subclasses should override this to create appropriate argument types.
        ACE EDSL creates AIRValue objects.
        """
        raise NotImplementedError("Subclass must implement generate_execution_arguments")

    @dataclass
    class LaunchConfig:
        """Launch configuration (not used by ACE EDSL)."""
        cluster: list = None
        grid: list = field(default_factory=lambda: [1, 1, 1])
        block: list = field(default_factory=lambda: [1, 1, 1])
        smem: int = 0
        async_deps: list = field(default_factory=list)
        has_cluster: bool = False
        min_blocks_per_mp: int = 0

        def __post_init__(self):
            self.has_cluster = self.cluster is not None
            if self.cluster is None:
                self.cluster = [None, None, None]
