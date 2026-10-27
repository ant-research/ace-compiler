# IR module - Intermediate Representations for FHE compilation

# =============================================================================
# Base
# =============================================================================
# =============================================================================
# Analysis utilities
# =============================================================================
from .analysis import (
    compare_ir_structures,
    extract_ir_structure,
    summarize_ir_structure,
)
from .base import CompilationUnit

# =============================================================================
# Core (IR Builder)
# =============================================================================
from .core import IRBuilder, TensorInfo

# =============================================================================
# Export utilities
# =============================================================================
from .export import IRSerializer, export_fhe_program_to_air, export_fhe_program_to_onnx

# AST path
from .frontends.ast import ASTToIRConverter

# ONNX path
from .frontends.onnx import (
    convert_onnx_to_air_binary,
    convert_onnx_to_fhe_program,
)

# =============================================================================
# Frontends (Converters from various formats to IR)
# =============================================================================
# Torch path
from .frontends.torch import (
    CUSTOM_OPERATORS,
    STANDARD_OP_MAPPING,
    CustomTracer,
    FXTracedModel,
    TorchTracedModel,
    trace_with_metadata,
)

# =============================================================================
# IO (File formats and tools)
# =============================================================================
from .io import (
    AIRFileIR,
    AIRModel,
    FileIR,
    FileModel,
    ONNXFileIR,
    # Backward compatibility aliases
    ONNXModel,
    convert_onnx_to_air,
    export_function_to_onnx,
    # ONNX tools
    export_model_to_onnx,
    inspect_onnx_model,
    validate_onnx_model,
)

# =============================================================================
# Representations (In-memory IR)
# =============================================================================
from .representations import BasicBlock, FHEGraph, FHEProgram, IRNode


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
    "CUSTOM_OPERATORS",
    "STANDARD_OP_MAPPING",
    "AIRFileIR",
    "AIRModel",
    # Frontends - AST
    "ASTToIRConverter",
    "BasicBlock",
    # Base
    "CompilationUnit",
    "CustomTracer",
    "FHEGraph",
    # Representations
    "FHEProgram",
    "FXTracedModel",
    # IO
    "FileIR",
    "FileModel",
    # Core
    "IRBuilder",
    "IRNode",
    # Export
    "IRSerializer",
    "ONNXFileIR",
    "ONNXModel",
    "TensorInfo",
    # Frontends - Torch
    "TorchTracedModel",
    "compare_ir_structures",
    "convert_onnx_to_air",
    "convert_onnx_to_air_binary",
    # Frontends - ONNX
    "convert_onnx_to_fhe_program",
    "export_fhe_program_to_air",
    "export_fhe_program_to_onnx",
    "export_function_to_onnx",
    "export_model_to_onnx",
    # Analysis
    "extract_ir_structure",
    "inspect_onnx_model",
    "summarize_ir_structure",
    "trace_with_metadata",
    "validate_onnx_model",
]
