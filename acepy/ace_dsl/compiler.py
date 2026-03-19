"""
ACE Compiler - High-level compilation API

Users don't need to specify passes explicitly. The compiler automatically
selects and runs the appropriate pipeline based on the kernel domain.

Example:
    @kernel
    def add(a: Tensor, b: Tensor) -> Tensor:
        return a + b
    
    # Simple compile - runs default pipeline
    result = ace_compile(add)
    
    # Or use CompilerOptions for control
    result = ace_compile(add, CompilerOptions(opt_level=3))
"""

from dataclasses import dataclass, field
from typing import Optional, List, Callable, Any
from enum import Enum

from .passes import get_ops_to_skip
from .frontend.lowering_registry import has_lowering


class OptLevel(Enum):
    """Optimization level"""
    O0 = 0  # No optimization
    O1 = 1  # Basic optimization
    O2 = 2  # Standard optimization (default)
    O3 = 3  # Aggressive optimization


class Target(Enum):
    """Compilation target"""
    AIR = "air"          # Generate AIR only
    VECTOR = "vector"    # Lower to vector IR
    SIHE = "sihe"        # Lower to SIHE (FHE)
    CKKS = "ckks"        # Lower to CKKS
    POLY = "poly"        # Lower to polynomial IR  
    C = "c"              # Generate C code


@dataclass
class CompilerOptions:
    """
    Compiler configuration options.
    
    Users typically don't need to modify these - defaults work for most cases.
    
    Attributes:
        opt_level: Optimization level (0-3, default 2)
        target: Final compilation target (default: auto-detect based on domain)
        verbose: Print compilation progress
        dump_ir: Dump IR after each pass
        inline_lowerings: Whether to inline Python-defined lowerings
    """
    opt_level: int = 2
    target: Optional[Target] = None  # Auto-detect
    verbose: bool = False
    dump_ir: bool = False
    inline_lowerings: bool = True
    
    def __post_init__(self):
        if not 0 <= self.opt_level <= 3:
            raise ValueError(f"opt_level must be 0-3, got {self.opt_level}")


# Default pipelines for each starting domain
# Full FHE pipeline: nn::core -> nn::vector -> fhe::sihe -> fhe::ckks -> fhe::poly -> C
_DEFAULT_PIPELINES = {
    "air::core": [],  # Just AIR, no lowering
    "nn::core": ["tensor2vector", "vector2sihe", "sihe2ckks", "ckks2poly", "poly2c"],
    "nn::vector": ["vector2sihe", "sihe2ckks", "ckks2poly", "poly2c"],
    "fhe::sihe": ["sihe2ckks", "ckks2poly", "poly2c"],
    "fhe::ckks": ["ckks2poly", "poly2c"],
    "fhe::poly": ["poly2c"],
}


def _detect_domain(kernel) -> str:
    """Detect the domain of a kernel based on its decorator."""
    # Check for domain-specific attributes set by decorators
    if hasattr(kernel, '_domain'):
        return kernel._domain
    if hasattr(kernel, '_ace_domain'):
        return kernel._ace_domain
    
    # Default based on kernel type
    kernel_type = type(kernel).__name__
    domain_map = {
        'NNKernel': 'nn::core',
        'VectorKernel': 'nn::vector', 
        'SIHEKernel': 'fhe::sihe',
        'CKKSKernel': 'fhe::ckks',
        'PolyKernel': 'fhe::poly',
        'DomainKernel': 'nn::core',  # Generic domain kernel defaults to nn::core
    }
    return domain_map.get(kernel_type, 'nn::core')


def _get_pipeline_for_domain(domain: str, target: Optional[Target]) -> List[str]:
    """Get the pass pipeline for a domain and target."""
    if domain not in _DEFAULT_PIPELINES:
        domain = 'nn::core'  # Default
    
    pipeline = _DEFAULT_PIPELINES[domain].copy()
    
    # Truncate pipeline based on target
    if target:
        target_pass_map = {
            Target.AIR: [],
            Target.VECTOR: ["tensor2vector"],
            Target.SIHE: ["tensor2vector", "vector2sihe"],
            Target.CKKS: ["tensor2vector", "vector2sihe", "sihe2ckks"],
            Target.POLY: ["tensor2vector", "vector2sihe", "sihe2ckks", "ckks2poly"],
            Target.C: pipeline,  # Full pipeline
        }
        pipeline = target_pass_map.get(target, pipeline)
    
    return pipeline


def ace_compile(kernel, options: Optional[CompilerOptions] = None):
    """
    Compile a PyACE kernel.
    
    This is the main entry point for compilation. Users don't need to
    specify passes - the compiler automatically selects the appropriate
    pipeline based on the kernel's domain.
    
    Args:
        kernel: A decorated kernel function (@kernel, @nn_kernel, etc.)
        options: Optional compiler configuration
        
    Returns:
        Compiled result with AIR module and optional generated code
        
    Example:
        @kernel
        def add(a: Tensor, b: Tensor) -> Tensor:
            return a + b
        
        # Compile with defaults
        result = ace_compile(add)
        
        # Compile with options
        result = ace_compile(add, CompilerOptions(verbose=True))
    """
    if options is None:
        options = CompilerOptions()
    
    # Detect kernel domain
    domain = _detect_domain(kernel)
    
    if options.verbose:
        print(f"[ace_compile] Kernel domain: {domain}")
    
    # Get default pipeline for this domain
    pipeline = _get_pipeline_for_domain(domain, options.target)
    
    if options.verbose:
        print(f"[ace_compile] Default pipeline: {pipeline}")
    
    # Check for Python-defined lowerings that should skip C++ passes
    skip_ops = list(get_ops_to_skip()) if options.inline_lowerings else []
    
    if skip_ops and options.verbose:
        print(f"[ace_compile] Ops with Python lowerings (will skip C++): {skip_ops}")
    
    # Compile the kernel to get AIR
    if hasattr(kernel, 'compile'):
        kernel.compile()
        glob_scope = kernel.air_module
    elif hasattr(kernel, 'air_module'):
        glob_scope = kernel.air_module
    else:
        # Try to compile as a decorated function
        from .frontend.decorator import compile_kernel
        glob_scope = compile_kernel(kernel)
    
    if options.dump_ir:
        print(f"\n=== AIR after initial compilation ===")
        print(glob_scope.dump())
    
    # Run C++ passes with Python lowering after each pass (except final code gen)
    if pipeline and hasattr(glob_scope, 'run_cpp_pass'):
        from .passes import run_python_lowering_pass
        
        # Passes that generate final output (no Python lowering after these)
        final_passes = {'poly2c', 'ir2c', 'codegen'}
        
        for pass_name in pipeline:
            # Run C++ pass
            if options.verbose:
                print(f"[ace_compile] Running C++ pass: {pass_name}")
            
            success = glob_scope.run_cpp_pass(pass_name, skip_ops)
            
            if not success and options.verbose:
                print(f"[ace_compile] Pass {pass_name} returned false")
            
            if options.dump_ir:
                print(f"\n=== AIR after C++ {pass_name} ===")
                print(glob_scope.dump())
            
            # Run Python lowering pass after each C++ pass (except final code gen)
            # This ensures Python lowerings are applied at the right level
            if pass_name not in final_passes and options.inline_lowerings and skip_ops:
                if options.verbose:
                    print(f"[ace_compile] Running Python lowering pass (after {pass_name})")
                
                run_python_lowering_pass(glob_scope, verbose=options.verbose)
                
                if options.dump_ir:
                    print(f"\n=== AIR after Python lowering (post-{pass_name}) ===")
                    print(glob_scope.dump())
    
    # Also run final Python lowering in case there's no C++ pipeline
    elif options.inline_lowerings and skip_ops:
        from .passes import run_python_lowering_pass
        
        if options.verbose:
            print(f"[ace_compile] Running Python lowering pass (no C++ pipeline)")
        
        run_python_lowering_pass(glob_scope, verbose=options.verbose)
        
        if options.dump_ir:
            print(f"\n=== AIR after Python lowering ===")
            print(glob_scope.dump())
    
    return CompileResult(
        glob_scope=glob_scope,
        domain=domain,
        pipeline=pipeline,
        options=options
    )


@dataclass
class CompileResult:
    """Result of compilation."""
    glob_scope: Any  # GlobScope
    domain: str
    pipeline: List[str]
    options: CompilerOptions
    
    def dump(self) -> str:
        """Dump the AIR."""
        return self.glob_scope.dump()
    
    def get_c_code(self) -> Optional[str]:
        """Get generated C code (if poly2c was run)."""
        if hasattr(self.glob_scope, 'get_c_code'):
            return self.glob_scope.get_c_code()
        return None


def jit_compile(pyfunc, *args, **kwargs):
    """
    JIT compile a kernel function.
    
    Args:
        pyfunc: Python function to compile
        *args: Arguments to specialize the kernel
        **kwargs: Compiler options
        
    Example:
        @kernel
        def matmul(A: Tensor, B: Tensor) -> Tensor:
            return A @ B
        
        compiled = jit_compile(matmul)
    """
    options = CompilerOptions(**kwargs) if kwargs else None
    return ace_compile(pyfunc, options)


__all__ = [
    'ace_compile',
    'jit_compile', 
    'CompilerOptions',
    'CompileResult',
    'OptLevel',
    'Target',
]

