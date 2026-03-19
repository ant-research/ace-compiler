"""
ACE DSL Types - Type annotations for kernel functions.

Provides type hints that are used by the @kernel decorator
to infer tensor shapes and element types.
"""

from typing import Any, Tuple, TypeVar, Generic, Optional, Union, get_args, get_origin

T = TypeVar('T')


class TensorMeta(type):
    """
    Metaclass for Tensor that enables subscripting for shape annotations.
    
    Allows: Tensor[64], Tensor[3, 224, 224], Tensor[float, 64]
    """
    
    def __getitem__(cls, args: Union[int, Tuple[int, ...], Tuple[type, ...]]) -> 'Tensor':
        """Create a tensor type with shape annotation."""
        if isinstance(args, int):
            shape = (args,)
            dtype = float
        elif isinstance(args, tuple):
            # Check if first element is a type
            if args and isinstance(args[0], type):
                dtype = args[0]
                shape = args[1:] if len(args) > 1 else ()
            else:
                dtype = float
                shape = args
        else:
            shape = (args,)
            dtype = float
        
        # Return a special annotated type
        tensor = Tensor()
        tensor._shape = shape
        tensor._dtype = dtype
        return tensor


class Tensor(metaclass=TensorMeta):
    """
    Tensor type for kernel function annotations.
    
    Usage:
        @kernel
        def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
            return a + b
        
        @kernel
        def conv(x: Tensor[1, 3, 224, 224]) -> Tensor[1, 64, 112, 112]:
            ...
    """
    
    def __init__(self):
        self._shape: Tuple[int, ...] = ()
        self._dtype: type = float
    
    @property
    def shape(self) -> Tuple[int, ...]:
        return self._shape
    
    @property
    def dtype(self) -> type:
        return self._dtype
    
    def __repr__(self) -> str:
        if self._shape:
            shape_str = ", ".join(str(d) for d in self._shape)
            return f"Tensor[{shape_str}]"
        return "Tensor"


def get_tensor_shape(annotation: Any) -> Optional[Tuple[int, ...]]:
    """
    Extract shape from a tensor type annotation.
    
    Args:
        annotation: Type annotation (e.g., Tensor[64] or Tensor[3, 224, 224])
        
    Returns:
        Tuple of dimensions, or None if not a tensor type
    """
    if isinstance(annotation, Tensor):
        return annotation.shape
    
    # Handle typing module generics
    origin = get_origin(annotation)
    if origin is not None:
        args = get_args(annotation)
        if args:
            return tuple(a for a in args if isinstance(a, int))
    
    return None


class Ciphertext:
    """
    Base class for encrypted values in FHE.
    """
    
    def __init__(self):
        self._level: int = 0
        self._scale: float = 1.0
    
    @property
    def level(self) -> int:
        return self._level
    
    @property
    def scale(self) -> float:
        return self._scale


class SiheCiphertext(Ciphertext):
    """
    Scheme-Independent Homomorphic Encryption ciphertext.
    """
    pass


class CkksCiphertext(SiheCiphertext):
    """
    CKKS-specific ciphertext with scale management.
    """
    pass


class CkksPlaintext:
    """
    CKKS encoded plaintext type.

    Represents a plaintext value that has been encoded into polynomial form.
    Use this for pre-encoded plaintext parameters in CKKS kernels.

    Example:
        @ckks_kernel
        def polynomial_eval(x: CkksCiphertext,
                            c1: CkksPlaintext,  # Pre-encoded coefficient
                            c3: CkksPlaintext) -> CkksCiphertext:
            return x + c1 + c3

    IR Mapping:
        - Python: CkksPlaintext
        - AIR IR: TYP[0x13](record,"PLAINTEXT")
        - C Runtime: PLAINTEXT struct

    Note:
        Use scalar constants (5.0, 10.0) for simple values - they are
        auto-encoded. Use CkksPlaintext for vector plaintexts or when
        you want to avoid encoding overhead.
    """
    pass


class BfvCiphertext(SiheCiphertext):
    """
    BFV-specific ciphertext.
    """
    pass


class Polynomial:
    """
    Polynomial type for low-level FHE operations.
    
    Represents a polynomial in the ring R_q = Z_q[X]/(X^N + 1).
    """
    
    def __init__(self, degree: int = 4096, modulus_level: int = 0):
        self.degree = degree
        self.modulus_level = modulus_level
    
    def __repr__(self) -> str:
        return f"Polynomial(degree={self.degree}, level={self.modulus_level})"


# Type aliases for common shapes
Scalar = Tensor
Vector = Tensor
Matrix = Tensor
Image = Tensor  # For image tensors [N, C, H, W]


__all__ = [
    'Tensor',
    'get_tensor_shape',
    'Ciphertext',
    'SiheCiphertext',
    'CkksCiphertext',
    'CkksPlaintext',
    'BfvCiphertext',
    'Polynomial',
    'Scalar',
    'Vector',
    'Matrix',
    'Image',
]
