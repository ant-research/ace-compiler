"""
Domain-Specific Types for ace_edsl

Provides type annotations for kernel functions in different domains:
- Tensor: Tensor domain (high-level tensor operations) - air::core
- VectorTensor: Vector domain (vectorized operations) - nn::vector
- MemRef: Memory domain (memory operations)
- ComputeTensor: Compute domain (compute operations)
- SiheCiphertext: SIHE domain (scheme-independent FHE) - fhe::sihe
- CkksCiphertext: CKKS domain (CKKS-specific FHE) - fhe::ckks
- Polynomial: Polynomial domain (low-level polynomial ops) - fhe::poly

Usage:
    @tensor_kernel
    def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
        return a + b
    
    @nn_kernel
    def nn_add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
        return a + b  # Uses nn::core ops
    
    @vector_kernel
    def vec_add(a: VectorTensor[1024], b: VectorTensor[1024]) -> VectorTensor[1024]:
        return a + b
    
    @sihe_kernel
    def sihe_add(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
        return a + b
    
    @ckks_kernel
    def ckks_add(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
        return a + b
    
    @poly_kernel
    def ntt(p: Polynomial[4096]) -> Polynomial[4096]:
        return p  # NTT operations
    
    @memory_kernel
    def mem_copy(src: MemRef[64], dst: MemRef[64]):
        dst[:] = src[:]
"""

from typing import Any, Tuple, Optional, Union, get_args, get_origin


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
    Tensor type for tensor-domain kernel function annotations.
    
    Usage:
        @tensor_kernel
        def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
            return a + b
        
        @tensor_kernel
        def conv(x: Tensor[1, 3, 224, 224]) -> Tensor[1, 64, 112, 112]:
            ...
    """
    
    def __init__(self):
        self._shape: Tuple[int, ...] = ()
        self._dtype: type = float
    
    @property
    def shape(self) -> Tuple[int, ...]:
        """Return the shape tuple."""
        return self._shape
    
    @property
    def dtype(self) -> type:
        """Return the dtype."""
        return self._dtype
    
    def __repr__(self) -> str:
        if self._shape:
            shape_str = ", ".join(str(d) for d in self._shape)
            return f"Tensor[{shape_str}]"
        return "Tensor"


class VectorTensorMeta(type):
    """Metaclass for VectorTensor that enables subscripting."""
    
    def __getitem__(cls, args: Union[int, Tuple[int, ...], Tuple[type, ...]]) -> 'VectorTensor':
        """Create a vector tensor type with dtype + shape annotation."""
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
        
        tensor = VectorTensor()
        tensor._shape = shape
        tensor._dtype = dtype
        return tensor


class VectorTensor(metaclass=VectorTensorMeta):
    """
    VectorTensor type for vector-domain kernel function annotations.
    
    Usage:
        @vector_kernel
        def vec_add(a: VectorTensor[1024], b: VectorTensor[1024]) -> VectorTensor[1024]:
            return a + b
    """
    
    def __init__(self):
        self._shape: Tuple[int, ...] = ()
        self._dtype: type = float
    
    @property
    def shape(self) -> Tuple[int, ...]:
        """Return the shape tuple."""
        return self._shape
    
    @property
    def dtype(self) -> type:
        """Return the dtype."""
        return self._dtype
    
    def __repr__(self) -> str:
        if self._shape:
            shape_str = ", ".join(str(d) for d in self._shape)
            return f"VectorTensor[{shape_str}]"
        return "VectorTensor"


class MemRefMeta(type):
    """Metaclass for MemRef that enables subscripting."""
    
    def __getitem__(cls, args: Union[int, Tuple[int, ...]]) -> 'MemRef':
        """Create a memref type with shape annotation."""
        if isinstance(args, int):
            shape = (args,)
        elif isinstance(args, tuple):
            shape = args
        else:
            shape = (args,)
        
        memref = MemRef()
        memref._shape = shape
        memref._dtype = float
        return memref


class MemRef(metaclass=MemRefMeta):
    """
    MemRef type for memory-domain kernel function annotations.
    
    Usage:
        @memory_kernel
        def mem_copy(src: MemRef[64], dst: MemRef[64]):
            dst[:] = src[:]
    """
    
    def __init__(self):
        self._shape: Tuple[int, ...] = ()
        self._dtype: type = float
    
    @property
    def shape(self) -> Tuple[int, ...]:
        """Return the shape tuple."""
        return self._shape
    
    @property
    def dtype(self) -> type:
        """Return the dtype."""
        return self._dtype
    
    def __repr__(self) -> str:
        if self._shape:
            shape_str = ", ".join(str(d) for d in self._shape)
            return f"MemRef[{shape_str}]"
        return "MemRef"


class ComputeTensorMeta(type):
    """Metaclass for ComputeTensor that enables subscripting."""
    
    def __getitem__(cls, args: Union[int, Tuple[int, ...]]) -> 'ComputeTensor':
        """Create a compute tensor type with shape annotation."""
        if isinstance(args, int):
            shape = (args,)
        elif isinstance(args, tuple):
            shape = args
        else:
            shape = (args,)
        
        tensor = ComputeTensor()
        tensor._shape = shape
        tensor._dtype = float
        return tensor


class ComputeTensor(metaclass=ComputeTensorMeta):
    """
    ComputeTensor type for compute-domain kernel function annotations.
    
    Usage:
        @compute_kernel
        def compute_op(a: ComputeTensor[64], b: ComputeTensor[64]) -> ComputeTensor[64]:
            return a + b
    """
    
    def __init__(self):
        self._shape: Tuple[int, ...] = ()
        self._dtype: type = float
    
    @property
    def shape(self) -> Tuple[int, ...]:
        """Return the shape tuple."""
        return self._shape
    
    @property
    def dtype(self) -> type:
        """Return the dtype."""
        return self._dtype
    
    def __repr__(self) -> str:
        if self._shape:
            shape_str = ", ".join(str(d) for d in self._shape)
            return f"ComputeTensor[{shape_str}]"
        return "ComputeTensor"


# =============================================================================
# Helper Functions
# =============================================================================

def get_tensor_shape(annotation: Any) -> Optional[Tuple[int, ...]]:
    """
    Extract shape from a tensor type annotation.
    
    Args:
        annotation: Type annotation (e.g., Tensor[64] or Tensor[3, 224, 224])
        
    Returns:
        Tuple of dimensions, or None if not a tensor type
    """
    # Handle our domain-specific types
    if isinstance(annotation, (Tensor, VectorTensor, MemRef, ComputeTensor)):
        return annotation.shape
    
    # Handle typing module generics (for forward compatibility)
    origin = get_origin(annotation)
    if origin is not None:
        args = get_args(annotation)
        if args:
            return tuple(a for a in args if isinstance(a, int))
    
    return None


def get_tensor_dtype(annotation: Any) -> type:
    """
    Extract dtype from a tensor type annotation.
    
    Args:
        annotation: Type annotation
        
    Returns:
        dtype (default: float)
    """
    if isinstance(annotation, (Tensor, VectorTensor, MemRef, ComputeTensor)):
        return annotation.dtype
    
    return float


def is_tensor_type(annotation: Any) -> bool:
    """
    Check if an annotation is a tensor type.
    
    Args:
        annotation: Type annotation
        
    Returns:
        True if it's a tensor type, False otherwise
    """
    return isinstance(annotation, (Tensor, VectorTensor, MemRef, ComputeTensor))


# =============================================================================
# FHE (Fully Homomorphic Encryption) Types
# =============================================================================

class Ciphertext:
    """
    Base class for encrypted values in FHE.
    """
    
    def __init__(self):
        self._level: int = 0
        self._scale: float = 1.0
    
    @property
    def level(self) -> int:
        """Return the modulus level."""
        return self._level
    
    @property
    def scale(self) -> float:
        """Return the scale."""
        return self._scale


class SiheCiphertextMeta(type):
    """Metaclass for SiheCiphertext that enables subscripting for shape annotations."""
    
    def __getitem__(cls, args) -> 'SiheCiphertext':
        """Create a SiheCiphertext type with shape annotation.
        
        Usage: SiheCiphertext[64], SiheCiphertext[float, 64]
        """
        if isinstance(args, int):
            shape = (args,)
            dtype = float
        elif isinstance(args, tuple):
            if args and isinstance(args[0], type):
                dtype = args[0]
                shape = args[1:] if len(args) > 1 else (64,)
                if len(shape) == 1 and isinstance(shape[0], int):
                    shape = (shape[0],)
            else:
                shape = args
                dtype = float
        else:
            shape = (64,)
            dtype = float
        
        ct = SiheCiphertext()
        ct._shape = shape
        ct._dtype = dtype
        return ct


class SiheCiphertext(Ciphertext, metaclass=SiheCiphertextMeta):
    """
    Scheme-Independent Homomorphic Encryption ciphertext.
    
    Works with any FHE scheme (CKKS, BFV, BGV).
    
    Instantiation patterns:
        # Type annotation (no instance)
        def kernel(ct: SiheCiphertext): ...
        
        # Subscript type annotation
        def kernel(ct: SiheCiphertext[64]): ...
        
        # Create instance with explicit shape
        ct = SiheCiphertext(shape=(64,))
        kernel(ct)  # Called with actual instance!
    """
    
    def __init__(self, shape: Tuple[int, ...] = (64,), dtype: type = float, name: str = None):
        super().__init__()
        self._shape: Tuple[int, ...] = shape if isinstance(shape, tuple) else (shape,)
        self._dtype: type = dtype
        self._name: Optional[str] = name  # For debugging/tracing
    
    @property
    def shape(self) -> Tuple[int, ...]:
        return self._shape
    
    @property
    def dtype(self) -> type:
        return self._dtype
    
    @property
    def name(self) -> Optional[str]:
        return self._name
    
    def __repr__(self) -> str:
        name_str = f", name='{self._name}'" if self._name else ""
        return f"{self.__class__.__name__}(shape={self._shape}, dtype={self._dtype.__name__}{name_str})"


class CkksCiphertextMeta(SiheCiphertextMeta):
    """Metaclass for CkksCiphertext."""
    
    def __getitem__(cls, args) -> 'CkksCiphertext':
        """Create a CkksCiphertext type with shape annotation."""
        if isinstance(args, int):
            shape = (args,)
            dtype = float
        elif isinstance(args, tuple):
            if args and isinstance(args[0], type):
                dtype = args[0]
                shape = args[1:] if len(args) > 1 else (64,)
            else:
                shape = args
                dtype = float
        else:
            shape = (64,)
            dtype = float
        
        ct = CkksCiphertext(shape=shape, dtype=dtype)
        return ct


class CkksCiphertext(SiheCiphertext, metaclass=CkksCiphertextMeta):
    """
    CKKS-specific ciphertext with scale management.
    
    Supports CKKS-specific operations:
    - Basic: add, sub, mul, neg
    - Rotation: rotate(amount)
    - Scale management: rescale(), mod_switch()
    - Relinearization: relin()
    - Bootstrap: bootstrap()
    
    Instantiation patterns:
        # Type annotation
        @ckks_kernel
        def kernel(ct: CkksCiphertext): ...
        
        # Create instance and call
        ct = CkksCiphertext(shape=(16384,), name="input_ct")
        zero = CkksCiphertext(shape=(16384,), name="zero_ct")
        kernel(ct, zero)  # Called with actual instances!
    """
    
    def __init__(self, shape: Tuple[int, ...] = (64,), dtype: type = float, 
                 name: str = None, scale: float = None, level: int = None):
        super().__init__(shape=shape, dtype=dtype, name=name)
        self._scale: Optional[float] = scale
        self._level: Optional[int] = level
    
    @property
    def scale(self) -> Optional[float]:
        """CKKS scale factor."""
        return self._scale
    
    @property
    def level(self) -> Optional[int]:
        """CKKS modulus level."""
        return self._level
    
    def __repr__(self) -> str:
        base = super().__repr__()
        extras = []
        if self._scale is not None:
            extras.append(f"scale={self._scale}")
        if self._level is not None:
            extras.append(f"level={self._level}")
        if extras:
            return base[:-1] + ", " + ", ".join(extras) + ")"
        return base


# =============================================================================
# CKKS Plaintext Type
# =============================================================================

class CkksPlaintextMeta(type):
    """Metaclass for CkksPlaintext that enables subscripting."""
    
    def __getitem__(cls, args) -> 'CkksPlaintext':
        """Create a CkksPlaintext type with shape annotation."""
        if isinstance(args, int):
            shape = (args,)
        elif isinstance(args, tuple):
            shape = args
        else:
            shape = (64,)
        return CkksPlaintext(shape=shape)


class CkksPlaintext(metaclass=CkksPlaintextMeta):
    """
    CKKS encoded plaintext type.
    
    Represents a plaintext value encoded into polynomial form.
    Maps to TYP[0x13] PLAINTEXT in the IR.
    
    Use this for:
    - Pre-encoded plaintext parameters
    - Vector plaintexts (encoded arrays)
    - Polynomial coefficients
    
    Usage:
        @ckks_kernel
        def kernel(ct: CkksCiphertext, pt: CkksPlaintext) -> CkksCiphertext:
            return ct * pt  # cipher * plaintext (no relin needed)
        
        @nn_kernel
        def kernel_with_plain(ct, pt: CkksPlaintext):
            return ct * pt  # Also works with nn_kernel
    
    IR Mapping:
        - Python: CkksPlaintext
        - AIR IR: TYP[0x13](record,"PLAINTEXT")
        - C Runtime: PLAINTEXT struct
    
    Note:
        Use scalar constants (5.0, 10.0) for simple values - they are
        auto-encoded. Use CkksPlaintext for vector plaintexts or when
        you want to avoid encoding overhead.
    """
    
    def __init__(self, shape: tuple = (64,), dtype: type = float, name: str = None):
        self._shape = shape
        self._dtype = dtype
        self._name = name
    
    @property
    def shape(self) -> tuple:
        """Return the shape tuple."""
        return self._shape
    
    @property
    def dtype(self) -> type:
        """Return the dtype."""
        return self._dtype
    
    @property
    def name(self) -> str:
        """Return the name."""
        return self._name
    
    def __repr__(self) -> str:
        name_str = f", name='{self._name}'" if self._name else ""
        return f"CkksPlaintext(shape={self._shape}, dtype={self._dtype.__name__}{name_str})"


class PolynomialMeta(type):
    """Metaclass for Polynomial that enables subscripting."""
    
    def __getitem__(cls, degree: int) -> 'Polynomial':
        """Create a polynomial type with degree annotation."""
        poly = Polynomial()
        poly._degree = degree
        return poly


class Polynomial(metaclass=PolynomialMeta):
    """
    Polynomial type for low-level FHE operations.
    
    Represents a polynomial in the ring R_q = Z_q[X]/(X^N + 1).
    
    Usage:
        @poly_kernel
        def ntt(p: Polynomial[4096]) -> Polynomial[4096]:
            ...
    """
    
    def __init__(self, degree: int = 4096, modulus_level: int = 0):
        self._degree = degree
        self._modulus_level = modulus_level
    
    @property
    def degree(self) -> int:
        """Return the polynomial degree."""
        return self._degree
    
    @property
    def modulus_level(self) -> int:
        """Return the modulus level."""
        return self._modulus_level
    
    def __repr__(self) -> str:
        return f"Polynomial(degree={self._degree}, level={self._modulus_level})"


# =============================================================================
# Scalar Types (for dynamic conditions and plaintext values)
# =============================================================================

class ScalarMeta(type):
    """Metaclass for Scalar that enables subscripting for bit width."""
    
    def __getitem__(cls, width: int) -> 'Scalar':
        """Create a scalar type with bit width annotation.
        
        Usage: Scalar[32], Scalar[64], Int[32], Float[64]
        """
        scalar = cls()
        scalar._width = width
        return scalar


class Scalar(metaclass=ScalarMeta):
    """
    Scalar type for kernel parameter annotations.
    
    When used as a parameter type, the value is converted to an AIRValue
    that supports operator overloading (including comparisons).
    
    Usage:
        @sihe_kernel
        def conditional_op(a: SiheCiphertext, b: SiheCiphertext, flag: Scalar) -> SiheCiphertext:
            if dynamic_expr(flag > 0):  # flag is AIRValue, > returns AIRValue
                return a + b
            else:
                return a - b
    """
    
    _is_scalar = True
    _is_integer = False
    _is_float = False
    
    def __init__(self, width: int = 64):
        self._width = width
    
    @property
    def width(self) -> int:
        """Return the bit width."""
        return self._width
    
    def __repr__(self) -> str:
        return f"Scalar[{self._width}]"


class Int(Scalar):
    """
    Integer scalar type for kernel parameters.
    
    Usage:
        @sihe_kernel
        def conditional_op(a: SiheCiphertext, b: SiheCiphertext, flag: Int) -> SiheCiphertext:
            if dynamic_expr(flag > 0):
                return a + b
            else:
                return a - b
    """
    
    _is_integer = True
    
    def __init__(self, width: int = 64):
        super().__init__(width)
    
    def __repr__(self) -> str:
        return f"Int[{self._width}]"


class Float(Scalar):
    """
    Floating-point scalar type for kernel parameters.
    
    Usage:
        @sihe_kernel
        def threshold_op(a: SiheCiphertext, threshold: Float) -> SiheCiphertext:
            if dynamic_expr(threshold > 0.5):
                return a + a
            else:
                return a
    """
    
    _is_float = True
    
    def __init__(self, width: int = 32):
        super().__init__(width)
    
    def __repr__(self) -> str:
        return f"Float[{self._width}]"


def is_scalar_type(annotation: Any) -> bool:
    """
    Check if an annotation is a scalar type (Scalar, Int, or Float).
    
    Args:
        annotation: Type annotation
        
    Returns:
        True if it's a scalar type, False otherwise
    """
    if isinstance(annotation, (Scalar, Int, Float)):
        return True
    if isinstance(annotation, type) and issubclass(annotation, Scalar):
        return True
    return getattr(annotation, '_is_scalar', False)
