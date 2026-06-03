# IR module - Intermediate Representations for FHE compilation

# =============================================================================
# Base
# =============================================================================
from .base import CompilationUnit

# =============================================================================
# Core (IR Builder)
# =============================================================================
from .core import IRBuilder, TensorInfo

# =============================================================================
# Representations (In-memory IR)
# =============================================================================
from .representations import FHEProgram, FHEGraph, IRNode, BasicBlock

# =============================================================================
# Frontends (Converters from various formats to IR)
# =============================================================================

# Torch path
from .frontends.torch import (
    TorchTracedModel,
    FXTracedModel,
    CustomTracer,
    trace_with_metadata,
    STANDARD_OP_MAPPING,
    CUSTOM_OPERATORS,
)

# ONNX path
from .frontends.onnx import (
    convert_onnx_to_fhe_program,
    convert_onnx_to_air_binary,
)

# AST path
from .frontends.ast import ASTToIRConverter

# =============================================================================
# IO (File formats and tools)
# =============================================================================
from .io import (
    FileIR,
    ONNXFileIR,
    AIRFileIR,
    # Backward compatibility aliases
    ONNXModel,
    AIRModel,
    FileModel,
    # ONNX tools
    export_model_to_onnx,
    export_function_to_onnx,
    validate_onnx_model,
    inspect_onnx_model,
    convert_onnx_to_air,
)

# =============================================================================
# Analysis utilities
# =============================================================================
from .analysis import (
    extract_ir_structure,
    compare_ir_structures,
    summarize_ir_structure,
)

# =============================================================================
# Export utilities
# =============================================================================
from .export import IRSerializer, export_fhe_program_to_onnx, export_fhe_program_to_air

# =============================================================================
# Backward Compatibility
# Re-export from old locations for existing code
# =============================================================================

# Old: from .ir_builder import IRBuilder, TensorInfo
# Now: from .core import IRBuilder, TensorInfo
# (Already exported above)

# Old: from .tensor_registry import TensorRegistry
# Removed: TensorRegistry is no longer needed

# Old: from .fhe_program import FHEProgram
# Now: from .representations import FHEProgram
# (Already exported above)

# Old: from .graph import BasicBlock, FHEGraph, IRNode
# Now: from .representations import BasicBlock, FHEGraph, IRNode
# (Already exported above)

# Old: from .torch_trace import TorchTracedModel, FXTracedModel
# Now: from .frontends.torch import TorchTracedModel, FXTracedModel
# (Already exported above)

# Old: from .ir_formats import FileIR, ONNXFileIR, AIRFileIR
# Now: from .io import FileIR, ONNXFileIR, AIRFileIR
# (Already exported above)

# Old: from .onnx_tools import export_model_to_onnx, ...
# Now: from .io import export_model_to_onnx, ...
# (Already exported above)

# Old: from .conversion import ASTToIRConverter
# Now: from .frontends.ast import ASTToIRConverter
# (Already exported above)

__all__ = [
    # Base
    'CompilationUnit',
    # Core
    'IRBuilder',
    'TensorInfo',
    # Representations
    'FHEProgram',
    'FHEGraph',
    'IRNode',
    'BasicBlock',
    # Frontends - Torch
    'TorchTracedModel',
    'FXTracedModel',
    'CustomTracer',
    'trace_with_metadata',
    'STANDARD_OP_MAPPING',
    'CUSTOM_OPERATORS',
    # Frontends - ONNX
    'convert_onnx_to_fhe_program',
    'convert_onnx_to_air_binary',
    # Frontends - AST
    'ASTToIRConverter',
    # IO
    'FileIR',
    'ONNXFileIR',
    'AIRFileIR',
    'ONNXModel',
    'AIRModel',
    'FileModel',
    'export_model_to_onnx',
    'export_function_to_onnx',
    'validate_onnx_model',
    'inspect_onnx_model',
    'convert_onnx_to_air',
    # Analysis
    'extract_ir_structure',
    'compare_ir_structures',
    'summarize_ir_structure',
    # Export
    'IRSerializer',
    'export_fhe_program_to_onnx',
    'export_fhe_program_to_air',
]