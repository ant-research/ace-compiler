"""
Type Mapping Utilities for ace_edsl

Maps Python type annotations to AIR types based on domain.
"""

from typing import Any, Optional, Tuple

# Import AIR bindings
from ace_bindings import air_builder

# Import domain types
from .types import (
    Tensor,
    VectorTensor,
    MemRef,
    ComputeTensor,
    Ciphertext,
    SiheCiphertext,
    CkksCiphertext,
    CkksPlaintext,
    Polynomial,
    Scalar,
    Int,
    Float,
    get_tensor_shape,
    get_tensor_dtype,
    is_tensor_type,
    is_scalar_type,
)

# Optional: numeric scalar types from base_dsl
try:
    from ..base_dsl.typing import Numeric
except Exception:
    Numeric = None


def python_type_to_air_type(
    python_type: Any,
    domain: str = "tensor",
    default_shape: Tuple[int, ...] = (64,),
    default_dtype: str = "f32"
) -> Optional[Any]:
    """
    Map Python type annotation to AIR type.
    
    Args:
        python_type: Python type annotation (e.g., Tensor[64], VectorTensor[1024], CkksCiphertext)
        domain: Domain name ("tensor", "vector", "compute", "memory", "fhe::ckks", etc.)
        default_shape: Default shape if type doesn't specify one
        default_dtype: Default dtype string ("f32", "f64", "i32", "i64")
        
    Returns:
        AIR Type object, or None if air_builder not available
        
    Examples:
        >>> python_type_to_air_type(Tensor[64], "tensor")
        # Returns: air_builder.Type.make_array([64], air_builder.Type.make_float(32))
        
        >>> python_type_to_air_type(CkksCiphertext, "fhe::ckks")
        # Returns: air_builder.Type.make_ciphertext()
    """
    if air_builder is None:
        return None
    
    # Check for FHE/CKKS cipher types
    # These need to be handled specially for the FHE pipeline
    if (isinstance(python_type, type) and issubclass(python_type, (Ciphertext, SiheCiphertext, CkksCiphertext))):
        return air_builder.Type.make_ciphertext()
    if isinstance(python_type, (Ciphertext, SiheCiphertext, CkksCiphertext)):
        return air_builder.Type.make_ciphertext()
    
    # Also check by type name for forward references or subscripted types
    type_name = getattr(python_type, "__name__", str(type(python_type).__name__))
    if "Ciphertext" in type_name or "ciphertext" in str(python_type).lower():
        return air_builder.Type.make_ciphertext()
    
    # Check for CKKS plaintext type
    # Plaintext is an encoded polynomial for cipher+plain operations
    if isinstance(python_type, type) and issubclass(python_type, CkksPlaintext):
        return air_builder.Type.make_plaintext()
    if isinstance(python_type, CkksPlaintext):
        return air_builder.Type.make_plaintext()
    
    # Check by type name for plaintext
    if "Plaintext" in type_name or "plaintext" in str(python_type).lower():
        return air_builder.Type.make_plaintext()
    
    # Handle domain-based type inference for FHE domains
    if domain in ("fhe::ckks", "fhe::sihe", "fhe::poly"):
        # For FHE domains, default to ciphertext type if not specified otherwise
        if python_type is None:
            return air_builder.Type.make_ciphertext()
    
    # Scalar types (Scalar, Int, Float)
    if is_scalar_type(python_type):
        width = getattr(python_type, '_width', 64)
        if isinstance(python_type, Float) or (isinstance(python_type, type) and issubclass(python_type, Float)):
            return air_builder.Type.make_float(width)
        else:
            return air_builder.Type.make_int(width)
    
    # Scalars (Python primitives or values)
    if isinstance(python_type, (int, float, bool)) or python_type in (int, float, bool):
        if isinstance(python_type, bool) or python_type is bool:
            return air_builder.Type.make_int(1)
        if isinstance(python_type, float) or python_type is float:
            return air_builder.Type.make_float(32)
        return air_builder.Type.make_int(64)

    # Scalars (DSL numeric types)
    if Numeric is not None:
        if isinstance(python_type, Numeric):
            python_type = type(python_type)
        if isinstance(python_type, type) and issubclass(python_type, Numeric):
            width = getattr(python_type, "width", None)
            is_int = getattr(python_type, "is_integer", False)
            is_float = getattr(python_type, "is_float", False)
            if is_int:
                bits = int(width) if width else 64
                return air_builder.Type.make_int(bits)
            if is_float:
                bits = int(width) if width else 32
                return air_builder.Type.make_float(bits)
            # Abstract numeric type (e.g., Scalar) defaults to int64
            return air_builder.Type.make_int(64)

    # Extract shape from type annotation
    shape = get_tensor_shape(python_type)
    if shape is None:
        shape = default_shape
    
    # Extract dtype from type annotation
    dtype = get_tensor_dtype(python_type)
    
    # Map Python dtype to AIR dtype
    air_dtype = python_dtype_to_air_dtype(dtype, default_dtype)
    
    # Create AIR array type
    return air_builder.Type.make_array(list(shape), air_dtype)


def python_dtype_to_air_dtype(python_dtype: type, default: str = "f32") -> Any:
    """
    Map Python dtype to AIR dtype.
    
    Args:
        python_dtype: Python type (float, int, etc.)
        default: Default AIR dtype string if mapping not found
        
    Returns:
        AIR Type object
    """
    if air_builder is None:
        return None
    
    # Map Python types to AIR types
    dtype_map = {
        float: air_builder.Type.make_float(32),
        int: air_builder.Type.make_int(64),
    }
    
    # Try direct mapping
    if python_dtype in dtype_map:
        return dtype_map[python_dtype]
    
    # Try by name
    dtype_name_map = {
        "float32": air_builder.Type.make_float(32),
        "float64": air_builder.Type.make_float(64),
        "int32": air_builder.Type.make_int(32),
        "int64": air_builder.Type.make_int(64),
    }
    
    dtype_name = getattr(python_dtype, "__name__", str(python_dtype))
    if dtype_name in dtype_name_map:
        return dtype_name_map[dtype_name]
    
    # Default based on default string
    default_map = {
        "f32": air_builder.Type.make_float(32),
        "f64": air_builder.Type.make_float(64),
        "i32": air_builder.Type.make_int(32),
        "i64": air_builder.Type.make_int(64),
    }
    
    return default_map.get(default, air_builder.Type.make_float(32))


def get_domain_type_name(domain: str) -> str:
    """
    Get the AIR type name for a domain.
    
    Args:
        domain: Domain name ("tensor", "vector", "compute", "memory")
        
    Returns:
        Type name string for AIR
    """
    domain_type_map = {
        "tensor": "tensor",
        "vector": "vector",
        "compute": "compute",
        "memory": "memref",
    }
    
    return domain_type_map.get(domain, "tensor")


def infer_type_from_value(value: Any, domain: str = "tensor") -> Optional[Any]:
    """
    Infer AIR type from a Python value.
    
    Args:
        value: Python value (int, float, list, etc.)
        domain: Domain name
        
    Returns:
        AIR Type object, or None
    """
    if air_builder is None:
        return None
    
    if isinstance(value, (int, float)):
        # Scalar value
        if isinstance(value, int):
            return air_builder.Type.make_int(64)
        else:
            return air_builder.Type.make_float(32)
    elif isinstance(value, (list, tuple)):
        # Array value - infer shape
        shape = [len(value)]
        # Try to infer nested shape
        if value and isinstance(value[0], (list, tuple)):
            shape.append(len(value[0]))
        return air_builder.Type.make_array(shape, air_builder.Type.make_float(32))
    else:
        # Unknown - return default
        return air_builder.Type.make_array([64], air_builder.Type.make_float(32))


def is_plaintext_annotation(annotation) -> bool:
    """
    Check if a type annotation indicates a plaintext type (CkksPlaintext).
    
    Used to promote scalar runtime arguments (int/float) to PLAINTEXT AIR type
    when the parameter annotation says CkksPlaintext.
    """
    if annotation is None:
        return False
    if isinstance(annotation, type) and issubclass(annotation, CkksPlaintext):
        return True
    if isinstance(annotation, CkksPlaintext):
        return True
    type_name = getattr(annotation, "__name__", "")
    return "Plaintext" in type_name

