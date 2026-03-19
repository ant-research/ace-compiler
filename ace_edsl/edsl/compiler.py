"""
ACE EDSL Compiler - High-level compilation API
===============================================

Provides a clean, high-level API for compiling ACE EDSL kernels.
Users don't need to specify passes explicitly - the compiler automatically
selects and runs the appropriate pipeline based on the kernel domain.

Example:
    @ckks_kernel
    def fhe_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
        return a + b
    
    # Simple compile - runs default pipeline
    result = ace_compile(fhe_add)
    
    # Or use CompilerOptions for control
    result = ace_compile(fhe_add, CompilerOptions(verbose=True, target=Target.POLY))

ONNX Loading (matching acepy):
    # Load an ONNX model and compile through FHE pipeline
    result = load_onnx("model.onnx")
    glob = result['glob_scope']
    
    # Or use Pipeline class
    from ace_edsl.edsl.pipeline import Pipeline
    result = Pipeline("my_test").load_onnx("model.onnx").run()

Differences from acepy:
    - ace_edsl uses operator overloading, so inlining is automatic
    - No intermediate Python IR stage
    - Simpler pass management (fewer passes needed due to automatic inlining)
"""

from dataclasses import dataclass, field
from typing import Optional, List, Callable, Any, Union, Dict
from enum import Enum
import os

from .lowering_registry import get_ops_to_skip, configure_pipeline_skip_ops


class OptLevel(Enum):
    """Optimization level for compilation."""
    O0 = 0  # No optimization - fastest compile
    O1 = 1  # Basic optimization
    O2 = 2  # Standard optimization (default)
    O3 = 3  # Aggressive optimization


class Target(Enum):
    """Compilation target - determines where to stop the pipeline."""
    AIR = "air"          # Generate AIR only (no lowering)
    VECTOR = "vector"    # Lower to nn::vector level
    SIHE = "sihe"        # Lower to fhe::sihe (scheme-independent FHE)
    CKKS = "ckks"        # Lower to fhe::ckks (CKKS-specific)
    POLY = "poly"        # Lower to fhe::poly (polynomial level)
    C = "c"              # Generate C code (full pipeline)


@dataclass
class CompilerOptions:
    """
    Compiler configuration options.
    
    Users typically don't need to modify these - defaults work for most cases.
    
    Attributes:
        opt_level: Optimization level (0-3, default 2)
        target: Final compilation target (default: auto-detect based on domain)
        verbose: Print compilation progress and debug info
        dump_ir: Dump IR after each pass
        dump_ir_to_file: If set, dump IR to this file path
        inline_lowerings: Whether to automatically inline Python-defined lowerings
        skip_cpp_for_registered_ops: Tell C++ passes to skip ops with Python lowerings
        
    Example:
        # Default options
        opts = CompilerOptions()
        
        # Verbose with IR dumping
        opts = CompilerOptions(verbose=True, dump_ir=True)
        
        # Compile to specific target
        opts = CompilerOptions(target=Target.CKKS)
    """
    opt_level: int = 2
    target: Optional[Target] = None  # Auto-detect from kernel domain
    verbose: bool = False
    dump_ir: bool = False
    dump_ir_to_file: Optional[str] = None
    inline_lowerings: bool = True
    skip_cpp_for_registered_ops: bool = True
    
    def __post_init__(self):
        if not 0 <= self.opt_level <= 3:
            raise ValueError(f"opt_level must be 0-3, got {self.opt_level}")


# Default pass pipelines for each starting domain
# Full FHE pipeline: nn::core → nn::vector → fhe::sihe → fhe::ckks → fhe::poly → C
_DEFAULT_PIPELINES = {
    "air::core": [],  # Just AIR, no lowering needed
    "nn::core": ["tensor2vector", "vector2sihe", "sihe2ckks", "ckks2poly", "poly2c"],
    "nn::vector": ["vector2sihe", "sihe2ckks", "ckks2poly", "poly2c"],
    "fhe::sihe": ["sihe2ckks", "ckks2poly", "poly2c"],
    "fhe::ckks": ["ckks2poly", "poly2c"],
    "fhe::poly": ["poly2c"],
}

# Map target to the passes needed to reach it
_TARGET_PIPELINES = {
    Target.AIR: [],
    Target.VECTOR: ["tensor2vector"],
    Target.SIHE: ["tensor2vector", "vector2sihe"],
    Target.CKKS: ["tensor2vector", "vector2sihe", "sihe2ckks"],
    Target.POLY: ["tensor2vector", "vector2sihe", "sihe2ckks", "ckks2poly"],
    Target.C: None,  # Use full default pipeline
}


def _detect_domain(kernel) -> str:
    """Detect the domain of a kernel based on its decorator or type."""
    # Check for domain attributes set by decorators
    if hasattr(kernel, '_py_domain'):
        return kernel._py_domain
    if hasattr(kernel, '_domain'):
        return kernel._domain
    if hasattr(kernel, '_ace_domain'):
        return kernel._ace_domain
    if hasattr(kernel, 'DOMAIN'):
        return kernel.DOMAIN
    
    # Check the function's name for hints
    func = kernel._func if hasattr(kernel, '_func') else kernel
    func_name = getattr(func, '__name__', '')
    
    # Default to nn::core for most cases
    return 'nn::core'


def _get_pipeline_for_domain(domain: str, target: Optional[Target]) -> List[str]:
    """Get the pass pipeline for a domain and target."""
    # Get default pipeline for domain
    if domain not in _DEFAULT_PIPELINES:
        domain = 'nn::core'  # Default
    
    default_pipeline = _DEFAULT_PIPELINES[domain].copy()
    
    # If target specified, use target-specific pipeline
    if target is not None:
        target_pipeline = _TARGET_PIPELINES.get(target)
        if target_pipeline is not None:
            # Filter to only include passes in default pipeline
            return [p for p in target_pipeline if p in default_pipeline or domain == 'nn::core']
        # target_pipeline is None means use full pipeline
        return default_pipeline
    
    return default_pipeline


def _notify_cpp_skip_ops(skip_ops: List[str], verbose: bool = False):
    """Notify C++ bindings to skip specific ops."""
    if not skip_ops:
        return
    
    if verbose:
        print(f"[ace_compile] Notifying C++ to skip: {skip_ops}")
    
    # Try passmanager
    try:
        from ace_bindings import passmanager
        if hasattr(passmanager, 'set_skip_ops'):
            passmanager.set_skip_ops(skip_ops)
            if verbose:
                print(f"[ace_compile] Called passmanager.set_skip_ops()")
        elif hasattr(passmanager, 'add_skip_op'):
            for op in skip_ops:
                passmanager.add_skip_op(op)
            if verbose:
                print(f"[ace_compile] Called passmanager.add_skip_op() for {len(skip_ops)} ops")
    except ImportError:
        if verbose:
            print(f"[ace_compile] passmanager not available")
    
    # Try nn_addon
    try:
        from ace_bindings import nn_addon
        if hasattr(nn_addon, 'set_skip_ops'):
            nn_addon.set_skip_ops(skip_ops)
            if verbose:
                print(f"[ace_compile] Called nn_addon.set_skip_ops()")
    except ImportError:
        pass


@dataclass
class CompileResult:
    """Result of compilation.
    
    Attributes:
        glob_scope: The AIR GlobScope containing compiled IR
        domain: The detected kernel domain
        pipeline: The passes that were run
        options: The compiler options used
        c_code: Generated C code (if target was C)
    """
    glob_scope: Any  # GlobScope
    domain: str
    pipeline: List[str]
    options: CompilerOptions
    c_code: Optional[str] = None
    
    def dump(self) -> str:
        """Dump the AIR as a string."""
        if self.glob_scope and hasattr(self.glob_scope, 'dump'):
            return self.glob_scope.dump()
        return ""
    
    def dump_flat(self) -> str:
        """Dump the AIR in flat SSA format."""
        if self.glob_scope and hasattr(self.glob_scope, 'dump_flat'):
            return self.glob_scope.dump_flat()
        return self.dump()
    
    def get_c_code(self) -> Optional[str]:
        """Get generated C code (if poly2c was run)."""
        if self.c_code:
            return self.c_code
        if self.glob_scope and hasattr(self.glob_scope, 'get_c_code'):
            return self.glob_scope.get_c_code()
        return None
    
    def save_ir(self, path: str):
        """Save IR to a file."""
        with open(path, 'w') as f:
            f.write(self.dump())
    
    def save_c(self, path: str) -> bool:
        """Save C code to a file. Returns True if successful."""
        c_code = self.get_c_code()
        if c_code:
            with open(path, 'w') as f:
                f.write(c_code)
            return True
        return False


def ace_compile(
    kernel,
    options: Optional[CompilerOptions] = None
) -> CompileResult:
    """
    Compile an ACE EDSL kernel.
    
    This is the main entry point for compilation. Users don't need to
    specify passes - the compiler automatically selects the appropriate
    pipeline based on the kernel's domain.
    
    Args:
        kernel: A decorated kernel function (@kernel, @nn_kernel, @ckks_kernel, etc.)
                Can also be an already-traced kernel with air_module attribute.
        options: Optional compiler configuration
        
    Returns:
        CompileResult with AIR module and optional generated code
        
    Example:
        @ckks_kernel
        def fhe_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
            return a + b
        
        # Compile with defaults
        result = ace_compile(fhe_add)
        print(result.dump())
        
        # Compile with options
        result = ace_compile(fhe_add, CompilerOptions(verbose=True))
        
        # Compile to specific target
        result = ace_compile(fhe_add, CompilerOptions(target=Target.POLY))
    """
    if options is None:
        options = CompilerOptions()
    
    # Detect kernel domain
    domain = _detect_domain(kernel)
    
    if options.verbose:
        print(f"[ace_compile] Kernel domain: {domain}")
    
    # Get pipeline for this domain and target
    pipeline = _get_pipeline_for_domain(domain, options.target)
    
    if options.verbose:
        print(f"[ace_compile] Pipeline: {pipeline if pipeline else '(none)'}")
    
    # Get ops to skip (those with Python lowerings)
    skip_ops = []
    if options.skip_cpp_for_registered_ops:
        skip_ops = list(get_ops_to_skip())
        if skip_ops and options.verbose:
            print(f"[ace_compile] Ops with Python lowerings (skip C++): {skip_ops}")
        
        # Notify C++ to skip these ops
        _notify_cpp_skip_ops(skip_ops, options.verbose)
    
    # Get or create AIR module
    glob_scope = None
    
    # Check if kernel is already compiled (has air_module)
    if hasattr(kernel, 'current_air_module'):
        glob_scope = kernel.current_air_module
    elif hasattr(kernel, 'air_module'):
        glob_scope = kernel.air_module
    elif hasattr(kernel, '_get_dsl'):
        # It's a DSL singleton
        dsl = kernel._get_dsl()
        glob_scope = dsl.current_air_module
    elif callable(kernel):
        # Try to trace/compile the kernel
        # Determine number of arguments from function signature
        import inspect
        try:
            func = kernel._func if hasattr(kernel, '_func') else kernel
            sig = inspect.signature(func)
            num_params = len([p for p in sig.parameters.values() 
                            if p.default == inspect.Parameter.empty])
            # Call with None args to trigger tracing
            none_args = [None] * num_params
            kernel(*none_args)
            # Get the DSL and its module
            from .edsl import AceEDSL
            dsl = AceEDSL._get_dsl()
            glob_scope = dsl.current_air_module
        except Exception as e:
            if options.verbose:
                print(f"[ace_compile] Could not trace kernel: {e}")
            raise ValueError(f"Could not compile kernel: {e}")
    
    if glob_scope is None:
        raise ValueError("Could not get AIR module from kernel")
    
    if options.dump_ir:
        print(f"\n=== AIR after initial compilation ===")
        print(glob_scope.dump())
    
    # Run C++ passes if available
    c_code = None
    if pipeline and hasattr(glob_scope, 'run_cpp_pass'):
        for pass_name in pipeline:
            if options.verbose:
                print(f"[ace_compile] Running C++ pass: {pass_name}")
            
            try:
                success = glob_scope.run_cpp_pass(pass_name, skip_ops)
                if not success and options.verbose:
                    print(f"[ace_compile] Pass {pass_name} returned false")
            except Exception as e:
                if options.verbose:
                    print(f"[ace_compile] Pass {pass_name} failed: {e}")
            
            if options.dump_ir:
                print(f"\n=== AIR after {pass_name} ===")
                print(glob_scope.dump())
        
        # Get C code if poly2c was run
        if 'poly2c' in pipeline and hasattr(glob_scope, 'get_c_code'):
            c_code = glob_scope.get_c_code()
    
    # Save IR to file if requested
    if options.dump_ir_to_file:
        with open(options.dump_ir_to_file, 'w') as f:
            f.write(glob_scope.dump())
        if options.verbose:
            print(f"[ace_compile] Saved IR to {options.dump_ir_to_file}")
    
    return CompileResult(
        glob_scope=glob_scope,
        domain=domain,
        pipeline=pipeline,
        options=options,
        c_code=c_code
    )


def jit_compile(pyfunc, *args, **kwargs) -> CompileResult:
    """
    JIT compile a kernel function.
    
    Convenience wrapper around ace_compile for a more dynamic API.
    
    Args:
        pyfunc: Python function to compile (should be decorated with @kernel, etc.)
        *args: Ignored (for API compatibility)
        **kwargs: Passed to CompilerOptions
        
    Returns:
        CompileResult
        
    Example:
        @ckks_kernel
        def matmul(A, B):
            return A @ B
        
        # JIT compile
        result = jit_compile(matmul, verbose=True)
    """
    options = CompilerOptions(**kwargs) if kwargs else None
    return ace_compile(pyfunc, options)


def compile_to_c(kernel, output_path: Optional[str] = None, **kwargs) -> str:
    """
    Compile a kernel to C code.
    
    Convenience function that compiles to the C target.
    
    Args:
        kernel: Decorated kernel function
        output_path: Optional path to save C code
        **kwargs: Passed to CompilerOptions
        
    Returns:
        Generated C code as string
        
    Example:
        @nn_kernel
        def my_model(x, w):
            return x * w
        
        c_code = compile_to_c(my_model, "output.c", verbose=True)
    """
    kwargs['target'] = Target.C
    result = ace_compile(kernel, CompilerOptions(**kwargs))
    
    c_code = result.get_c_code() or ""
    
    if output_path and c_code:
        with open(output_path, 'w') as f:
            f.write(c_code)
    
    return c_code


def load_onnx(
    onnx_path: str,
    verbose: bool = False
) -> Dict[str, Any]:
    """
    Load an ONNX model and convert it to AIR.
    
    This function matches acepy's `air_builder.load_onnx_model()` API.
    
    Args:
        onnx_path: Path to the ONNX file
        verbose: Print loading progress
        
    Returns:
        Dict with:
            - 'success': bool - whether loading succeeded
            - 'glob_scope': GlobScope - the AIR module (if successful)
            - 'message': str - error message (if failed)
            
    Example:
        result = load_onnx("model.onnx")
        if result['success']:
            glob = result['glob_scope']
            print(glob.dump())
    """
    if not os.path.exists(onnx_path):
        return {
            'success': False,
            'glob_scope': None,
            'message': f"ONNX file not found: {onnx_path}"
        }
    
    if verbose:
        print(f"[load_onnx] Loading ONNX model: {onnx_path}")
    
    try:
        # Try to use the C++ binding directly
        from ace_bindings import air_builder
        
        result = air_builder.load_onnx_model(onnx_path)
        
        if verbose:
            if result.get('success'):
                print(f"[load_onnx] Successfully loaded model")
            else:
                print(f"[load_onnx] Failed: {result.get('message')}")
        
        return result
        
    except ImportError as e:
        return {
            'success': False,
            'glob_scope': None,
            'message': f"ace_bindings not available: {e}"
        }
    except Exception as e:
        return {
            'success': False,
            'glob_scope': None,
            'message': str(e)
        }


def compile_onnx(
    onnx_path: str,
    options: Optional[CompilerOptions] = None
) -> CompileResult:
    """
    Load an ONNX model and compile it through the FHE pipeline.
    
    This is a convenience function that combines load_onnx and ace_compile.
    
    Args:
        onnx_path: Path to the ONNX file
        options: Compiler options
        
    Returns:
        CompileResult with compiled AIR and optional C code
        
    Example:
        result = compile_onnx("model.onnx", CompilerOptions(verbose=True))
        if result.c_code:
            print(f"Generated {len(result.c_code)} bytes of C code")
    """
    if options is None:
        options = CompilerOptions()
    
    # Load ONNX
    load_result = load_onnx(onnx_path, verbose=options.verbose)
    
    if not load_result.get('success'):
        raise RuntimeError(f"Failed to load ONNX: {load_result.get('message')}")
    
    glob_scope = load_result['glob_scope']
    
    # Create a wrapper that looks like a kernel
    class OnnxModelWrapper:
        def __init__(self, glob):
            self.air_module = glob
            self._domain = 'nn::core'  # ONNX models start at nn::core
    
    wrapper = OnnxModelWrapper(glob_scope)
    
    return ace_compile(wrapper, options)


__all__ = [
    # Main API
    'ace_compile',
    'jit_compile',
    'compile_to_c',
    # ONNX Loading (matching acepy)
    'load_onnx',
    'compile_onnx',
    # Configuration
    'CompilerOptions',
    'CompileResult',
    'OptLevel',
    'Target',
]

