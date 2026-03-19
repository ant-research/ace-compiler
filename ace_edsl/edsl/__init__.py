"""
ACE EDSL - Embedded DSL with Multiple Domains

A single DSL class that handles multiple domains (tensor, vector, compute, memory)
using BaseDSL infrastructure with AST preprocessing and operator overloading.
"""

from .edsl import AceEDSL
from .domain_kernels import (
    tensor_kernel,
    kernel,  # Alias for tensor_kernel (acepy compatibility)
    nn_kernel,
    vector_kernel,
    sihe_kernel,
    ckks_kernel,
    poly_kernel,
    compute_kernel,
    memory_kernel,
)

# Re-export from base_dsl (AST preprocessing)
from ..base_dsl.ast_helpers import (
    range_dynamic,
    range_constexpr,
    dynamic_expr,
    const_expr,
    executor,
)

# Export domain types
from .core.types import (
    Tensor,
    VectorTensor,
    MemRef,
    ComputeTensor,
    Ciphertext,
    SiheCiphertext,
    CkksCiphertext,
    CkksPlaintext,
    Polynomial,
    # Scalar types for dynamic conditions
    Scalar,
    Int,
    Float,
    get_tensor_shape,
    get_tensor_dtype,
    is_tensor_type,
    is_scalar_type,
)

# Export AIRValue for operator overloading
from .core.air_value import AIRValue

# Export domain-specific operations
from .core import tensor_ops
from .core import vector_ops
from .core import compute_ops
from .core import memory_ops
from .core import sihe_ops
from .core import ckks_ops
from .core import poly_ops

# Import AST decorators to register them
# This ensures executor.set_functions() is called when AceEDSL is instantiated
from . import domain_ast_decorators  # noqa: F401

# Export pipeline (including acepy-compatible Pipeline class)
from .pipeline import (
    AcePipeline, 
    FHEConfig, 
    PipelineResult, 
    compile_to_c,
    Pipeline,
    PipelineTarget,
)

# Export selective lowering registry
from .lowering_registry import (
    register_lowering,
    get_lowering,
    has_lowering,
    list_lowerings,
    clear_lowerings,
    get_ops_to_skip,
    get_ops_to_skip_for_pass,
    call_lowering,
    configure_pipeline_skip_ops,
    print_registry_status,
    sync_skip_ops_to_cpp,
    LoweringInfo,
)

# Export optional CKKS rewrite pass helper
from .passes import rewrite_extended_ckks_ops_to_primitives

# Export high-level compiler API
from .compiler import (
    ace_compile,
    jit_compile,
    compile_to_c as compiler_compile_to_c,
    CompilerOptions,
    CompileResult,
    OptLevel,
    Target,
    # ONNX loading (matching acepy)
    load_onnx,
    compile_onnx,
)

# Backward compatibility alias
PyDSL = AceEDSL

__all__ = [
    'AceEDSL',
    'PyDSL',  # Backward compatibility
    # High-level Compiler API (like acepy)
    'ace_compile',
    'jit_compile',
    'CompilerOptions',
    'CompileResult',
    'OptLevel',
    'Target',
    # ONNX Loading (matching acepy)
    'load_onnx',
    'compile_onnx',
    # Pipeline (including acepy-compatible Pipeline class)
    'AcePipeline',
    'Pipeline',
    'PipelineTarget',
    'FHEConfig',
    'PipelineResult',
    'compile_to_c',
    # Selective lowering
    'register_lowering',
    'get_lowering',
    'has_lowering',
    'list_lowerings',
    'clear_lowerings',
    'get_ops_to_skip',
    'get_ops_to_skip_for_pass',
    'call_lowering',
    'configure_pipeline_skip_ops',
    'print_registry_status',
    'sync_skip_ops_to_cpp',
    'LoweringInfo',
    'rewrite_extended_ckks_ops_to_primitives',
    # Domain decorators (matching acepy)
    'kernel',           # Alias for tensor_kernel
    'tensor_kernel',    # Tensor domain (air::core)
    'nn_kernel',        # Neural network domain (nn::core)
    'vector_kernel',    # Vector domain (nn::vector)
    'sihe_kernel',      # SIHE domain (fhe::sihe)
    'ckks_kernel',      # CKKS domain (fhe::ckks)
    'poly_kernel',      # Polynomial domain (fhe::poly)
    # Additional domains
    'compute_kernel',   # Compute domain
    'memory_kernel',    # Memory domain
    # Domain types
    'Tensor',
    'VectorTensor',
    'MemRef',
    'ComputeTensor',
    'Ciphertext',
    'SiheCiphertext',
    'CkksCiphertext',
    'CkksPlaintext',
    'Polynomial',
    # Scalar types for dynamic conditions
    'Scalar',
    'Int',
    'Float',
    'get_tensor_shape',
    'get_tensor_dtype',
    'is_tensor_type',
    'is_scalar_type',
    'AIRValue',
    # Loop and control flow helpers (from base_dsl)
    'range_dynamic',
    'range_constexpr',
    'dynamic_expr',
    'const_expr',
    # Domain operations
    'tensor_ops',
    'vector_ops',
    'compute_ops',
    'memory_ops',
    'sihe_ops',
    'ckks_ops',
    'poly_ops',
]
