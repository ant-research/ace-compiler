"""
Compilation Functions - High-level compilation API.

Provides functions to compile kernels to FHE using C++ passes.
"""

from typing import Union, Dict, Any, Optional
import os

from base_dsl import Compiler, get_tmpdir, is_dryrun
from ace_dsl.frontend.decorator import CompiledKernel, PythonIRToAIRLowering
from ace_bindings import air_builder, nn_addon, fhe_cmplr, passmanager


# ═══════════════════════════════════════════════════════════════════════════════
# Default Pipelines
# ═══════════════════════════════════════════════════════════════════════════════

ACE_FHE_PIPELINE = (
    "vector-pass,"
    "sihe-pass,"
    "ckks-pass,"
    "poly-pass,"
    "poly2c-pass"
)

ACE_VECTOR_ONLY_PIPELINE = "vector-pass"

ACE_SIHE_PIPELINE = "vector-pass,sihe-pass"

ACE_CKKS_PIPELINE = "vector-pass,sihe-pass,ckks-pass"


# ═══════════════════════════════════════════════════════════════════════════════
# Compilation Functions
# ═══════════════════════════════════════════════════════════════════════════════

def compile_fhe(
    kernel_or_module: Union[CompiledKernel, Any],
    target: str = "ckks",
    opt_level: int = 3,
    enable_ir_printing: bool = False,
    pipeline: Optional[str] = None,
) -> str:
    """
    Compile a kernel to FHE and generate C code.
    
    Args:
        kernel_or_module: Either a @kernel decorated function or an AIR module
        target: Target FHE scheme: "ckks", "bfv", "bgv"
        opt_level: Optimization level (0-3)
        enable_ir_printing: Print IR after each pass
        pipeline: Custom pipeline string (overrides default)
        
    Returns:
        Generated C code as a string
    """
    # Handle CompiledKernel
    if isinstance(kernel_or_module, CompiledKernel):
        kernel_or_module.compile(enable_ir_printing)
        air_module = kernel_or_module.air_module
    else:
        air_module = kernel_or_module
    
    # Select pipeline based on target
    if pipeline is None:
        if target == "ckks":
            pipeline = ACE_FHE_PIPELINE
        elif target in ("bfv", "bgv"):
            # BFV/BGV use different passes
            pipeline = "vector-pass,sihe-pass,bfv-pass,poly-pass,poly2c-pass"
        else:
            pipeline = ACE_FHE_PIPELINE
    
    # Create pass manager
    pm = passmanager.PassManager.parse(pipeline)
    
    if enable_ir_printing:
        pm.enable_ir_printing()
    
    # Create module wrapper
    module = passmanager.Module("kernel")
    
    # Convert AIR to string for pass processing
    if hasattr(air_module, 'dump'):
        module.set_ir(air_module.dump())
    else:
        module.set_ir(str(air_module))
    
    # If native GLOB_SCOPE is available, pass it to module for real IR2C
    if hasattr(air_module, 'get_native_ptr') and hasattr(air_module, 'has_native_ir'):
        if air_module.has_native_ir():
            native_ptr = air_module.get_native_ptr()
            if native_ptr != 0:
                module.set_native_glob_scope(native_ptr)
    
    # Run passes
    pm.run(module)
    
    # Return final output (C code)
    return module.dump()


def compile_to_ir(
    kernel_func: CompiledKernel,
    level: str = "nn_core",
    enable_ir_printing: bool = False,
) -> Any:
    """
    Compile a kernel to a specific IR level without generating C code.
    
    Args:
        kernel_func: A @kernel decorated function
        level: Target IR level: "nn_core", "nn_vector", "sihe", "ckks", "poly"
        enable_ir_printing: Print IR after each pass
        
    Returns:
        AIR module at the specified level
    """
    kernel_func.compile(enable_ir_printing)
    
    if level == "nn_core":
        # Already at nn::core level
        return kernel_func.air_module
    
    # Select pipeline based on level
    level_to_pipeline = {
        "nn_vector": "vector-pass",
        "sihe": "vector-pass,sihe-pass",
        "ckks": "vector-pass,sihe-pass,ckks-pass",
        "poly": "vector-pass,sihe-pass,ckks-pass,poly-pass",
    }
    
    pipeline = level_to_pipeline.get(level, "vector-pass")
    pm = passmanager.PassManager.parse(pipeline)
    
    if enable_ir_printing:
        pm.enable_ir_printing()
    
    module = passmanager.Module("kernel")
    module.set_ir(kernel_func.air_module.dump())
    
    pm.run(module)
    
    return module


def load_onnx(onnx_path: str, enable_ir_printing: bool = False) -> Any:
    """
    Load an ONNX model and convert it to AIR.
    
    Args:
        onnx_path: Path to the ONNX file
        enable_ir_printing: Print IR during conversion
        
    Returns:
        AIR module representing the ONNX model
    """
    if not os.path.exists(onnx_path):
        raise FileNotFoundError(f"ONNX file not found: {onnx_path}")
    
    # In a full implementation, this would:
    # 1. Use the C++ ONNX2AIR_PASS to parse the ONNX file
    # 2. Convert ONNX ops to nn::core ops
    # 3. Return the AIR module
    
    # Placeholder implementation
    glob_scope = air_builder.create_glob_scope()
    func = glob_scope.new_func("onnx_model")
    
    # Add placeholder
    func.new_param("input", air_builder.Type.make_array([1, 3, 224, 224]))
    
    if enable_ir_printing:
        print(f"Loaded ONNX model from {onnx_path}")
        print(glob_scope.dump())
    
    return glob_scope


# ═══════════════════════════════════════════════════════════════════════════════
# Environment-based Configuration
# ═══════════════════════════════════════════════════════════════════════════════

def get_default_target() -> str:
    """Get default target from environment."""
    return os.environ.get("ACE_DSL_TARGET", "ckks")


def get_default_opt_level() -> int:
    """Get default optimization level from environment."""
    return int(os.environ.get("ACE_DSL_OPT_LEVEL", "2"))


def should_print_ir() -> bool:
    """Check if IR printing is enabled via environment."""
    return os.environ.get("ACE_DSL_ENABLE_IR_PRINT", "0") == "1"


__all__ = [
    'compile_fhe',
    'compile_to_ir',
    'load_onnx',
    'ACE_FHE_PIPELINE',
    'ACE_VECTOR_ONLY_PIPELINE',
    'ACE_SIHE_PIPELINE',
    'ACE_CKKS_PIPELINE',
    'get_default_target',
    'get_default_opt_level',
    'should_print_ir',
]
