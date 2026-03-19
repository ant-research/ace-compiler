"""
Domain-Specific Kernel Decorators
=================================

Allows users to write kernels at different levels of the compilation pipeline:

1. @kernel          - Tensor level (nn::core) - default high-level DSL
2. @nn_kernel       - Neural network level (nn::core) - explicit NN ops
3. @vector_kernel   - Vector level (nn::vector) - after vectorization
4. @sihe_kernel     - SIHE level (fhe::sihe) - scheme-independent FHE
5. @ckks_kernel     - CKKS level (fhe::ckks) - CKKS-specific operations
6. @poly_kernel     - Polynomial level (fhe::poly) - low-level polynomial ops

Example:
    @kernel
    def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
        return a + b
    
    @sihe_kernel
    def custom_fhe_add(a: Ciphertext, b: Ciphertext) -> Ciphertext:
        return sihe.add(a, b)
    
    @poly_kernel  
    def custom_ntt(p: Polynomial) -> Polynomial:
        return poly.ntt(p)

Custom Lowering Registration:
    Use @register_lowering to define how high-level ops are expanded:
    
    @register_lowering("nn::core", "conv")
    @vector_kernel
    def lower_conv(input: VectorTensor, weight: VectorTensor) -> VectorTensor:
        # This gets inlined when conv is encountered
        return input * weight
"""

import ast
import inspect
import time
import warnings
from typing import Callable, Any, Dict, List, Optional, Tuple
from functools import wraps
from dataclasses import dataclass

from base_dsl.python_ir import Scope
from base_dsl.ast_to_ir import get_function_ir
from ace_bindings import air_builder, nn_addon, fhe_cmplr, passmanager
from ace_dsl.core.types import Tensor, Ciphertext, SiheCiphertext, CkksCiphertext, CkksPlaintext, Polynomial


# ============================================================================
# Phase 2: Exceptions (shared with decorator.py)
# ============================================================================

class CompilationError(Exception):
    """Error during kernel compilation."""
    pass


class CacheWarning(UserWarning):
    """Warning about cache usage (too many variants)."""
    pass


# ============================================================================
# Phase 2: Shape Specialization Data Structures
# ============================================================================

# Shape signature type: tuple of parameter shapes
ShapeSignature = Tuple[Tuple[int, ...], ...]


@dataclass
class VariantInfo:
    """
    Information about a compiled kernel variant for specific shape signature.

    Attributes:
        shape_signature: Immutable shape signature
        air_module: AIR global scope for this variant
        air_func: AIR function scope for this variant
        python_ir: Python IR (shared across all variants)
        compile_time: Compilation duration in seconds
        call_count: Number of times this variant was used
    """
    shape_signature: ShapeSignature
    air_module: Any
    air_func: Any
    python_ir: Any
    compile_time: float
    call_count: int = 0


# Domain-specific container factories
def get_domain_container(domain: str, base_container):
    """Get domain-specific container wrapper for emitting domain ops."""
    if domain == "nn::vector":
        return nn_addon.create_container()
    elif domain == "fhe::sihe":
        return fhe_cmplr.create_sihe_container()
    elif domain == "fhe::ckks":
        return fhe_cmplr.create_ckks_container()
    elif domain == "fhe::poly":
        return fhe_cmplr.create_poly_container()
    else:
        return base_container


# =============================================================================
# Domain Types for Type Annotations
# =============================================================================

class NNTensor:
    """Tensor type at nn::core level (high-level neural network ops)."""
    def __init__(self, shape: Tuple[int, ...] = ()):
        self._shape = shape
    
    def __class_getitem__(cls, shape):
        if isinstance(shape, int):
            shape = (shape,)
        inst = cls(shape)
        return inst
    
    @property
    def shape(self):
        return self._shape


class VectorTensor:
    """Tensor type at nn::vector level."""
    def __init__(self, shape: Tuple[int, ...] = ()):
        self._shape = shape
    
    def __class_getitem__(cls, shape):
        if isinstance(shape, int):
            shape = (shape,)
        inst = cls(shape)
        return inst
    
    @property
    def shape(self):
        return self._shape


class SiheCiphertext(Ciphertext):
    """Ciphertext at fhe::sihe level (scheme-independent)."""
    pass


class CkksCiphertext(SiheCiphertext):
    """Ciphertext at fhe::ckks level.

    Supports CKKS-specific operations:
    - Basic: add, sub, mul, neg
    - Rotation: rotate(amount)
    - Scale management: rescale(), mod_switch()
    - Relinearization: relin()
    - Bootstrap: bootstrap()
    """
    def __init__(self):
        super().__init__()
        self._scale: float = 1.0
        self._level: int = 0

    @property
    def scale(self) -> float:
        return self._scale

    @property
    def level(self) -> int:
        return self._level


class CkksPlaintext:
    """Encoded plaintext at fhe::ckks level.

    Represents a plaintext value encoded into polynomial form.
    Maps to TYP[0x13] PLAINTEXT in the IR.

    Use this for:
    - Pre-encoded plaintext parameters
    - Vector plaintexts
    - Polynomial coefficients

    For simple scalar constants (5.0, 10.0), use Python primitives instead
    - they will be automatically encoded.
    """
    def __init__(self):
        self._slots: int = 0
        self._scale: float = 1.0

    @property
    def slots(self) -> int:
        return self._slots

    @property
    def scale(self) -> float:
        return self._scale


# =============================================================================
# CKKS Helper Functions - for use inside @ckks_kernel
# =============================================================================

class CkksOps:
    """CKKS operation helpers for use inside @ckks_kernel.
    
    Usage in @ckks_kernel:
        result = ckks.rotate(ct, 5)
        result = ckks.rescale(ct)
        result = ckks.bootstrap(ct)
    """
    
    @staticmethod
    def rotate(ct: CkksCiphertext, rotation: int) -> CkksCiphertext:
        """Rotate ciphertext slots by given amount.
        
        Args:
            ct: Input ciphertext
            rotation: Number of slots to rotate (positive = left, negative = right)
        
        Returns:
            Rotated ciphertext
        """
        # This is a marker - actual operation emitted during lowering
        return CkksRotate(ct, rotation)
    
    @staticmethod
    def rescale(ct: CkksCiphertext) -> CkksCiphertext:
        """Reduce ciphertext scale after multiplication.
        
        Args:
            ct: Input ciphertext (typically after multiplication)
        
        Returns:
            Rescaled ciphertext with reduced scale
        """
        return CkksRescale(ct)
    
    @staticmethod
    def relin(ct: CkksCiphertext) -> CkksCiphertext:
        """Relinearize ciphertext after multiplication.
        
        After multiplication, ciphertext has 3 polynomials (CIPHERTEXT3).
        Relinearization reduces back to 2 polynomials (CIPHERTEXT).
        
        Args:
            ct: Input ciphertext (CIPHERTEXT3 from multiplication)
        
        Returns:
            Relinearized ciphertext (CIPHERTEXT)
        """
        return CkksRelin(ct)
    
    @staticmethod
    def mod_switch(ct: CkksCiphertext) -> CkksCiphertext:
        """Reduce ciphertext modulus (level) by one.
        
        Args:
            ct: Input ciphertext
        
        Returns:
            Ciphertext with reduced level
        """
        return CkksModSwitch(ct)
    
    @staticmethod
    def bootstrap(ct: CkksCiphertext) -> CkksCiphertext:
        """Refresh ciphertext noise budget through bootstrapping.
        
        Bootstrapping is a complex operation that:
        1. Raises the modulus
        2. Performs homomorphic DFT (CoeffToSlot)
        3. Approximates modular reduction (EvalMod)
        4. Performs homomorphic inverse DFT (SlotToCoeff)
        
        Args:
            ct: Input ciphertext with low noise budget
        
        Returns:
            Bootstrapped ciphertext with refreshed noise budget
        """
        return CkksBootstrap(ct)
    
    @staticmethod
    def neg(ct: CkksCiphertext) -> CkksCiphertext:
        """Negate ciphertext.
        
        Args:
            ct: Input ciphertext
        
        Returns:
            Negated ciphertext (-ct)
        """
        return CkksNeg(ct)


# CKKS operation marker classes for DSL lowering
class CkksRotate:
    """Marker for CKKS rotation."""
    def __init__(self, ct, rotation: int):
        self.ct = ct
        self.rotation = rotation


class CkksRescale:
    """Marker for CKKS rescale."""
    def __init__(self, ct):
        self.ct = ct


class CkksRelin:
    """Marker for CKKS relinearization."""
    def __init__(self, ct):
        self.ct = ct


class CkksModSwitch:
    """Marker for CKKS mod switch."""
    def __init__(self, ct):
        self.ct = ct


class CkksBootstrap:
    """Marker for CKKS bootstrap."""
    def __init__(self, ct):
        self.ct = ct


class CkksNeg:
    """Marker for CKKS negation."""
    def __init__(self, ct):
        self.ct = ct


# Global instance for convenient access
ckks = CkksOps()


class Polynomial:
    """Polynomial at fhe::poly level."""
    def __init__(self, degree: int = 4096, level: int = 0):
        self.degree = degree
        self.level = level
    
    def __class_getitem__(cls, degree):
        return cls(degree)


# =============================================================================
# Base Compiled Kernel
# =============================================================================

class DomainKernel:
    """
    Base class for domain-specific compiled kernels.

    Phase 2: Supports shape specialization - can compile for multiple shapes.
    """

    # Domain configuration
    DOMAIN = "air::core"
    DOMAIN_PREFIX = ""
    START_PASS = None  # Which pass to start from (None = full pipeline)

    def __init__(self, name: str, python_func: Callable):
        self.name = name
        self.python_func = python_func
        self.python_ir = None

        # Phase 2: Variant storage
        self._compiled_variants: Dict[ShapeSignature, VariantInfo] = {}
        self._default_signature: Optional[ShapeSignature] = None

        # Legacy properties for backward compatibility (point to default variant)
        self.air_module = None
        self._compiled = False
        
        # Track if IR has been transformed by pipeline (invalidates cache)
        self._ir_transformed = False
    
    @property
    def parameters(self) -> Tuple[str, ...]:
        sig = inspect.signature(self.python_func)
        return tuple(sig.parameters.keys())

    # ========================================================================
    # Phase 2: Helper Methods for Shape Specialization
    # ========================================================================

    def _make_signature(self, shapes: List[Tuple[int, ...]]) -> ShapeSignature:
        """Create immutable shape signature from list of shapes."""
        return tuple(tuple(s) for s in shapes)

    def _extract_annotation_shapes(self) -> List[Tuple[int, ...]]:
        """Extract shapes from function type annotations."""
        sig = inspect.signature(self.python_func)
        shapes = []

        for param_name, param in sig.parameters.items():
            annotation = param.annotation

            # Handle domain-specific tensor types
            if hasattr(annotation, '_shape'):
                # VectorTensor instance has _shape attribute
                shapes.append(annotation._shape)
            elif hasattr(annotation, 'shape') and callable(getattr(type(annotation), 'shape', None)):
                # Has shape property - get the value
                shape_val = annotation.shape
                if isinstance(shape_val, tuple):
                    shapes.append(shape_val)
                else:
                    shapes.append((64,))
            elif annotation == inspect.Parameter.empty:
                # No annotation - use default
                shapes.append((64,))
            else:
                # Non-tensor type (Ciphertext, etc.) - no shape
                shapes.append(())

        return shapes

    def _compile_variant(
        self,
        signature: ShapeSignature,
        enable_ir_printing: bool
    ) -> VariantInfo:
        """Compile a specific shape variant (Phase 2)."""
        start = time.time()

        try:
            # Parse Python IR (once, shared across variants)
            if self.python_ir is None:
                scope = Scope()
                self.python_ir = get_function_ir(self.python_func, scope)

            # Lower to AIR with domain-specific opcode prefix
            from ace_dsl.frontend.decorator import PythonIRToAIRLowering
            lowering = DomainAwareLowering(self.DOMAIN, enable_ir_printing)

            # Use original lower_function() for backward compatibility
            # (lower_function_with_shapes has issues with AIR builder state)
            glob_scope = lowering.lower_function(self.python_ir)

            if enable_ir_printing:
                print(f"\n=== {self.DOMAIN} IR for {self.name} (shapes={signature}) ===")
                print(glob_scope.dump())

            return VariantInfo(
                shape_signature=signature,
                air_module=glob_scope,
                air_func=lowering._func_scope,
                python_ir=self.python_ir,
                compile_time=time.time() - start,
                call_count=0
            )
        except Exception as e:
            raise CompilationError(
                f"Failed to compile domain kernel '{self.name}' for shapes {signature}.\n"
                f"Reason: {type(e).__name__}: {e}"
            ) from e

    def compile(
        self,
        shapes: Optional[List[Tuple[int, ...]]] = None,
        enable_ir_printing: bool = False
    ) -> 'DomainKernel':
        """
        Compile kernel for specific shapes or annotation shapes (Phase 2).

        Args:
            shapes: List of parameter shapes. If None, uses annotation shapes.
            enable_ir_printing: Print IR during compilation.

        Returns:
            Self for chaining.
        """
        # Extract shapes (from args or annotations)
        if shapes is None:
            shapes = self._extract_annotation_shapes()

        # Create signature
        signature = self._make_signature(shapes)

        # Invalidate cache if IR was transformed by pipeline
        # (Pipeline modifies IR in-place: SIHE→CKKS, so cached IR is no longer valid)
        if self._ir_transformed:
            self._compiled_variants.pop(signature, None)
            self._ir_transformed = False

        # Check cache
        if signature in self._compiled_variants:
            return self  # Already compiled

        # Compile new variant
        variant = self._compile_variant(signature, enable_ir_printing)
        self._compiled_variants[signature] = variant

        # Warn if too many variants cached
        if len(self._compiled_variants) > 10:
            warnings.warn(
                f"Domain kernel '{self.name}' has {len(self._compiled_variants)} "
                f"compiled variants (~{len(self._compiled_variants) * 4}MB memory). "
                f"Consider calling .clear_cache().",
                CacheWarning
            )

        # Set default on first compile (backward compatibility)
        if self._default_signature is None:
            self._default_signature = signature
            self.air_module = variant.air_module
            self._compiled = True

        return self

    def get_variant(self, shapes: List[Tuple[int, ...]]) -> Optional[VariantInfo]:
        """Get compiled variant for specific shapes (Phase 2)."""
        signature = self._make_signature(shapes)
        return self._compiled_variants.get(signature)

    def list_variants(self) -> List[Tuple[ShapeSignature, VariantInfo]]:
        """List all compiled variants with their signatures (Phase 2)."""
        return list(self._compiled_variants.items())

    def clear_cache(self, keep_default: bool = True):
        """Clear compiled variants from cache (Phase 2)."""
        if keep_default and self._default_signature:
            default = self._compiled_variants.get(self._default_signature)
            self._compiled_variants.clear()
            if default:
                self._compiled_variants[self._default_signature] = default
        else:
            self._compiled_variants.clear()
            self._default_signature = None
            self._compiled = False
        self._ir_transformed = False
    
    def mark_ir_transformed(self):
        """Mark that the IR has been transformed by pipeline operations.
        
        Call this after running pipeline passes (run_cpp_pass, sihe2ckks, etc.)
        on the air_module. This ensures the next compile() creates fresh IR
        instead of returning the (now-modified) cached version.
        
        Example:
            kernel.compile()
            glob = kernel.air_module
            glob.run_cpp_pass("sihe2ckks", [])
            kernel.mark_ir_transformed()  # Cache now invalidated
            
            # Later, this will recompile instead of using transformed IR:
            kernel.compile()  # Creates fresh SIHE IR
        """
        self._ir_transformed = True

    def dump_ir(self, flat: bool = True) -> str:
        """Dump the IR in tree or flat format.
        
        Args:
            flat: If True (default), use flattened SSA-like format (children printed before parent).
                  If False, use tree format (parent printed before children).
        
        The flat format is SSA-like and easier to parse for inlining operations.
        """
        if not self._compiled:
            self.compile()
        if not self.air_module:
            return ""
        if flat and hasattr(self.air_module, 'dump_flat'):
            return self.air_module.dump_flat()
        return self.air_module.dump()
    
    def run_pipeline(self, enable_ir_printing: bool = False) -> str:
        """Run compilation pipeline starting from this domain."""
        if not self._compiled:
            self.compile()
        
        # Build pipeline based on starting point
        pipeline = self._get_pipeline()
        if not pipeline:
            return self.dump_ir()
        
        pm = passmanager.PassManager.parse(pipeline)
        if enable_ir_printing:
            pm.enable_ir_printing()
        
        module = passmanager.Module(self.name)
        module.set_ir(self.dump_ir())
        
        pm.run(module)
        return module.dump()
    
    def _get_pipeline(self) -> str:
        """Get the pipeline string for this domain."""
        full_pipeline = ["vector-pass", "sihe-pass", "ckks-pass", "poly-pass", "poly2c-pass"]
        
        if self.START_PASS is None:
            return ",".join(full_pipeline)
        
        try:
            start_idx = full_pipeline.index(self.START_PASS)
            return ",".join(full_pipeline[start_idx:])
        except ValueError:
            return ",".join(full_pipeline)


class DomainAwareLowering:
    """Lowering that uses domain-specific opcodes.
    
    Uses the same lowering approach as PythonIRToAIRLowering but with
    domain-specific type handling.
    
    Each domain uses its own operations:
    - nn::core: air::core ops (add, mul, sub, div)
    - nn::vector: nn::vector ops (vec_add, vec_mul, vec_sub)
    - fhe::sihe: fhe::sihe ops (sihe_add, sihe_mul, sihe_sub)
    - fhe::ckks: fhe::ckks ops (ckks_add, ckks_mul, ckks_sub)
    - fhe::poly: fhe::poly ops (poly_add, poly_mul, poly_sub)
    """
    
    def __init__(self, domain: str, enable_ir_printing: bool = False):
        self.domain = domain
        self.enable_ir_printing = enable_ir_printing
        self._glob_scope = None
        self._func_scope = None
        self._container = None  # Base container for structure
        self._domain_container = None  # Domain-specific container for ops
        self._var_map: Dict[str, Any] = {}

    def _extract_parameter_types(self, pyfunc) -> List[Dict[str, Any]]:
        """
        Extract type information for each parameter (domain-aware version).

        Handles domain-specific types: Ciphertext, Polynomial, VectorTensor, etc.

        Args:
            pyfunc: Python IR function with parameters and annotations

        Returns:
            List of dicts with keys:
                - 'name': parameter name (str)
                - 'shape': shape as list
                - 'air_type': Domain-specific AIR type
                - 'annotation': Original annotation (for special handling)
        """
        param_types = []

        for param in pyfunc.parameters:
            # Default shape
            shape = [64]
            annotation = None

            # Extract from annotation
            if param.name in pyfunc.annotations:
                annotation = pyfunc.annotations[param.name]
                print(f"[DEBUG-EXTRACT] param={param.name}, annotation={annotation}, type={type(annotation).__name__}")

                # Handle Tensor-like types with shape attribute
                if hasattr(annotation, 'shape') and annotation.shape:
                    shape = list(annotation.shape)

                # Handle Polynomial with degree attribute
                elif hasattr(annotation, 'degree'):
                    shape = [annotation.degree]

                # Handle Ciphertext types (no explicit shape)
                # These use domain-specific type creation below

            # Build domain-specific AIR type based on current domain and annotation
            air_type = self._get_domain_type_for_param(shape, annotation)
            print(f"[DEBUG-EXTRACT] air_type for {param.name}: {air_type}")

            param_types.append({
                'name': param.name,
                'shape': shape,
                'air_type': air_type,
                'annotation': annotation
            })

        return param_types

    def _build_param_types_from_signature(
        self,
        pyfunc,
        signature: ShapeSignature
    ) -> List[Dict[str, Any]]:
        """
        Build parameter types from shape signature (Phase 2).

        Similar to _extract_parameter_types() but uses provided signature
        instead of extracting from annotations.

        Args:
            pyfunc: Python IR function
            signature: Shape signature (tuple of parameter shapes)

        Returns:
            List of parameter type dicts
        """
        param_types = []

        for i, param in enumerate(pyfunc.parameters):
            # Get shape from signature
            if i < len(signature):
                shape = list(signature[i])
            else:
                shape = [64]  # Fallback

            # Build domain-specific AIR type
            air_type = self._get_domain_type(shape)

            param_types.append({
                'name': param.name,
                'shape': shape,
                'air_type': air_type
            })

        return param_types

    def lower_function_with_shapes(
        self,
        pyfunc,
        signature: ShapeSignature
    ) -> Any:
        """
        Lower Python IR to AIR with specific shape signature (Phase 2).

        This is the shape-specialized version of lower_function().

        Args:
            pyfunc: Python IR function
            signature: Shape signature (e.g., ((64,), (128,)))

        Returns:
            AIR global scope
        """
        self._glob_scope = air_builder.create_glob_scope()

        # Build param types from signature (KEY CHANGE!)
        param_types = self._build_param_types_from_signature(pyfunc, signature)

        # Create function with specialized types
        func_shape = param_types[0]['shape'] if param_types else [64]
        num_params = len(pyfunc.parameters)
        type_name = self._get_type_name()
        self._func_scope = self._glob_scope.new_func_with_type(
            pyfunc.name, num_params, func_shape, type_name
        )
        self._container = self._func_scope.container()

        # Create domain-specific container for emitting domain ops
        self._domain_container = self._create_domain_container()

        # Map parameters with specialized types
        self._var_map.clear()
        for i, param in enumerate(pyfunc.parameters):
            param_info = param_types[i]
            air_param = self._func_scope.new_param(param.name, param_info['air_type'])
            self._var_map[param.name] = air_param

        # Lower body (unchanged)
        self._lower_block(pyfunc.root_block)

        return self._glob_scope

    def lower_function(self, pyfunc) -> Any:
        """Lower Python IR function to domain-specific AIR."""
        self._glob_scope = air_builder.create_glob_scope()

        # Extract per-parameter type information
        param_types = self._extract_parameter_types(pyfunc)

        # Determine function-level shape (use first parameter's shape or default)
        func_shape = param_types[0]['shape'] if param_types else [64]

        # Create function with individual parameter types
        num_params = len(pyfunc.parameters)

        # Extract individual parameter types for the function signature
        param_air_types = [param_info['air_type'] for param_info in param_types]

        # Determine return type (use first parameter's type or CIPHERTEXT for CKKS)
        ret_type = param_types[0]['air_type'] if param_types else self._get_domain_type(func_shape)

        print(f"[DEBUG-FUNC] Creating function '{pyfunc.name}' with {num_params} params")
        print(f"[DEBUG-FUNC] Return type: {ret_type}")
        print(f"[DEBUG-FUNC] Parameter types: {param_air_types}")

        # Check if all parameters have the same type name (homogeneous)
        type_names = [t.name if hasattr(t, 'name') else str(t) for t in param_air_types]
        is_homogeneous = len(set(type_names)) <= 1

        # For homogeneous parameters, use new_func_with_type (properly initializes cipher types for passes)
        # For heterogeneous parameters, use new_func_with_param_types
        if is_homogeneous and param_types:
            print(f"[DEBUG-FUNC] Using new_func_with_type (homogeneous parameters: {type_names[0]})")
            type_name = self._get_type_name()
            self._func_scope = self._glob_scope.new_func_with_type(
                pyfunc.name, num_params, func_shape, type_name
            )
        else:
            print(f"[DEBUG-FUNC] Using new_func_with_param_types (heterogeneous parameters)")
            self._func_scope = self._glob_scope.new_func_with_param_types(
                pyfunc.name, ret_type, param_air_types
            )
        self._container = self._func_scope.container()

        # Create domain-specific container for emitting domain ops
        self._domain_container = self._create_domain_container()

        # Map parameters to their AIR nodes with individual types
        self._var_map.clear()
        for i, param in enumerate(pyfunc.parameters):
            param_info = param_types[i]
            # Use the per-parameter AIR type
            print(f"[DEBUG-NEWPARAM] Creating parameter '{param.name}' with type: {param_info['air_type']}")
            air_param = self._func_scope.new_param(param.name, param_info['air_type'])
            print(f"[DEBUG-NEWPARAM] Created parameter, result type: {air_param}")
            self._var_map[param.name] = air_param
        
        # Lower body
        self._lower_block(pyfunc.root_block)
        
        return self._glob_scope
    
    def _get_type_name(self):
        """Get the type name for this domain."""
        if self.domain == "fhe::sihe":
            return "CIPHERTEXT"
        elif self.domain == "fhe::ckks":
            return "CIPHERTEXT"
        elif self.domain == "fhe::poly":
            return "polynomial"
        elif self.domain == "nn::vector":
            return "vector_tensor"
        elif self.domain == "nn::core":
            return "tensor"
        else:
            return "param_type"
    
    def _get_domain_type(self, shape):
        """Get the appropriate type for this domain."""
        if self.domain == "fhe::sihe":
            return air_builder.Type.make_ciphertext("sihe")
        elif self.domain == "fhe::ckks":
            return air_builder.Type.make_ciphertext("ckks")
        elif self.domain == "fhe::poly":
            degree = shape[0] if shape else 4096
            return air_builder.Type.make_polynomial(degree)
        elif self.domain == "nn::vector":
            return air_builder.Type.make_array(shape, air_builder.Type.make_float(32))
        elif self.domain == "nn::core":
            return air_builder.Type.make_array(shape, air_builder.Type.make_float(32))
        else:
            # Default: array of float32
            return air_builder.Type.make_array(shape, air_builder.Type.make_float(32))

    def _get_domain_type_for_param(self, shape, annotation=None):
        """
        Get the appropriate AIR type for a parameter based on domain and annotation.

        This method handles special cases like CkksPlaintext → PLAINTEXT type.

        Args:
            shape: Parameter shape
            annotation: Type annotation (e.g., CkksCiphertext, CkksPlaintext)

        Returns:
            AIR type object
        """
        # Import here to avoid circular dependency
        from ace_dsl.core.types import CkksPlaintext as CoreCkksPlaintext

        # Debug: print what we're processing
        print(f"[DEBUG] _get_domain_type_for_param: domain={self.domain}, annotation={annotation}, type={type(annotation).__name__ if annotation else None}")

        # Check for CKKS plaintext annotation (works for both fhe::sihe and fhe::ckks domains)
        if self.domain in ["fhe::ckks", "fhe::sihe"] and annotation is not None:
            # Check if annotation IS the CkksPlaintext class (or a subclass)
            # annotation is a class object, not an instance, so use issubclass or direct comparison
            is_plaintext_class = False
            try:
                # Check if it's a class and is/subclasses CkksPlaintext
                if isinstance(annotation, type):
                    is_plaintext_class = (annotation is CkksPlaintext or
                                         annotation is CoreCkksPlaintext or
                                         (issubclass(annotation, (CkksPlaintext, CoreCkksPlaintext))))
            except TypeError:
                # issubclass raises TypeError if annotation is not a class
                pass

            # Also check by name as fallback
            if is_plaintext_class or type(annotation).__name__ == 'CkksPlaintext':
                print(f"[DEBUG] Detected CkksPlaintext annotation! Using PLAINTEXT record type...")
                # Use the actual PLAINTEXT record type (matches native compiler)
                # This is an encoded polynomial that can be used in cipher+plain operations
                plaintext_type = air_builder.Type.make_plaintext()
                print(f"[DEBUG] Created PLAINTEXT record type: {plaintext_type}")
                return plaintext_type

        # Default: use standard domain type
        print(f"[DEBUG] Using standard domain type")
        return self._get_domain_type(shape)

    def _create_domain_container(self):
        """Domain container not needed - we use base container with domain-specific ops."""
        return None
    
    def _lower_block(self, block):
        for op in block.operations:
            self._lower_operation(op)
    
    def _set_source_loc(self, op):
        """Set source location on container from operation's loc."""
        if hasattr(op, 'loc') and op.loc and hasattr(self._container, 'set_loc'):
            loc = op.loc
            # Use file_id 0 for now, line and col from loc
            line = loc.line if hasattr(loc, 'line') else 0
            col = loc.col if hasattr(loc, 'col') else 0
            self._container.set_loc(0, line, col)
    
    def _lower_operation(self, op):
        op_type = type(op).__name__
        
        # Set source location before creating any IR nodes
        self._set_source_loc(op)
        
        if op_type == 'Store':
            value = op.operands.get('value')
            if value:
                value_node = self._get_value(value)
                # When inside control flow, emit actual store statement
                if hasattr(self._container, 'in_control_flow_body') and self._container.in_control_flow_body():
                    stored_node = self._container.new_stid(op.var_name, value_node)
                    self._var_map[op.var_name] = stored_node
                else:
                    self._var_map[op.var_name] = value_node
        
        elif op_type == 'Return':
            value = op.operands.get('value')
            if value:
                result = self._get_value(value)
                if result is not None:
                    self._container.new_retv(result)
                else:
                    self._container.new_ret()
            else:
                self._container.new_ret()
        
        elif op_type == 'BinOp':
            left = self._get_value(op.operands.get('lhs'))
            right = self._get_value(op.operands.get('rhs'))
            op_name = op.op_name

            # For SIHE/CKKS domain, wrap constants in CKKS.encode_at_level
            # This allows constants to be properly handled by the C++ backend
            if self.domain in ["fhe::sihe", "fhe::ckks"]:
                if self._is_constant_node(left):
                    print(f"[DEBUG-BINOP] Left operand is constant, encoding...")
                    left = self._wrap_constant_in_encode(left)
                if self._is_constant_node(right):
                    print(f"[DEBUG-BINOP] Right operand is constant, encoding...")
                    right = self._wrap_constant_in_encode(right)

            result = self._emit_domain_binop(op_name, left, right)

            if result is not None and op.result_vars:
                self._var_map[op.result_vars[0].name] = result
        
        elif op_type == 'Const':
            if op.result_vars:
                result_name = op.result_vars[0].name
                # Store Python value for range extraction
                if not hasattr(self, '_const_values'):
                    self._const_values = {}
                self._const_values[result_name] = op.value
                # Create AIR node
                if isinstance(op.value, (int, float)):
                    node = self._container.new_intconst(int(op.value))
                else:
                    node = self._container.new_intconst(0)
                self._var_map[result_name] = node
        
        elif op_type == 'Load':
            if op.result_vars:
                result_name = op.result_vars[0].name
                if op.var_name in self._var_map:
                    self._var_map[result_name] = self._var_map[op.var_name]
                # Track built-in functions for control flow and attribute access
                if op.var_name in ('range', 'len', 'enumerate', 'zip', 'getattr'):
                    if not hasattr(self, '_func_name_map'):
                        self._func_name_map = {}
                    self._func_name_map[result_name] = op.var_name
                # Track NN function names for lowering
                nn_functions = ('conv', 'relu', 'add', 'matmul', 'gemm', 'softmax', 
                               'flatten', 'average_pool', 'max_pool', 'reshape')
                if op.var_name in nn_functions:
                    if not hasattr(self, '_func_name_map'):
                        self._func_name_map = {}
                    self._func_name_map[result_name] = op.var_name
        
        elif op_type == 'Call':
            # Function call - handle range() and NN ops
            callee_var = op.operands.get('callee')
            callee_name = callee_var.name if hasattr(callee_var, 'name') else str(callee_var)
            
            # Check func_name_map for built-ins (resolves temp vars like $tmp to function names like 'conv')
            if hasattr(self, '_func_name_map') and callee_name in self._func_name_map:
                callee_name = self._func_name_map[callee_name]
            
            # Check if this is a getattr call (e.g., from ckks.rotate)
            # Pattern: Call(getattr_func, (object, "method_name"), result)
            if callee_name == 'getattr' or (hasattr(self, '_func_name_map') and 
                    self._func_name_map.get(callee_name) == 'getattr'):
                raw_args = op.operands.get('args', ())
                if len(raw_args) >= 2:
                    # Second arg is the attribute name
                    attr_name_var = raw_args[1]
                    if hasattr(attr_name_var, 'name') and attr_name_var.name in getattr(self, '_const_values', {}):
                        attr_name = self._const_values[attr_name_var.name]
                        # Store the method name for when this result is called
                        if op.result_vars:
                            if not hasattr(self, '_method_map'):
                                self._method_map = {}
                            self._method_map[op.result_vars[0].name] = attr_name
                return
            
            # Check method_map for method calls (e.g., result of ckks.rotate attribute access)
            if hasattr(self, '_method_map') and callee_name in self._method_map:
                callee_name = self._method_map[callee_name]
            
            # Handle NN domain function calls
            raw_args = op.operands.get('args', ())
            args = [self._get_value(arg) for arg in raw_args]
            
            result = None
            if callee_name == 'conv' and self.domain == "nn::core":
                # Emit nn::core::conv
                if len(args) >= 3 and hasattr(self._container, 'new_nn_conv'):
                    result = self._container.new_nn_conv(args[0], args[1], args[2])
                elif len(args) >= 3:
                    # Fallback: just mul and add
                    temp = self._container.new_nn_mul(args[0], args[1])
                    result = self._container.new_nn_add(temp, args[2])
            elif callee_name == 'relu' and self.domain == "nn::core":
                # Emit nn::core::relu
                if len(args) >= 1 and hasattr(self._container, 'new_nn_relu'):
                    result = self._container.new_nn_relu(args[0])
                elif len(args) >= 1:
                    # Fallback: just return input
                    result = args[0]
            elif callee_name == 'add' and self.domain == "nn::core":
                if len(args) >= 2:
                    result = self._container.new_nn_add(args[0], args[1])
            elif callee_name == 'range':
                start = 0
                end = 10
                
                for arg in raw_args:
                    if hasattr(arg, 'name') and arg.name in getattr(self, '_const_values', {}):
                        val = self._const_values[arg.name]
                        if len(raw_args) == 1:
                            end = val
                        elif arg == raw_args[0]:
                            start = val
                        elif arg == raw_args[1]:
                            end = val
                
                if not hasattr(self, '_range_info'):
                    self._range_info = {}
                if op.result_vars:
                    self._range_info[op.result_vars[0].name] = (start, end)
                return  # Don't store result for range
            
            # CKKS-specific operations (callable as ckks.rotate, etc.)
            elif callee_name == 'rotate' and self.domain in ["fhe::sihe", "fhe::ckks"]:
                if len(args) >= 1 and hasattr(self._container, 'new_ckks_rotate'):
                    rotation = 0
                    if len(raw_args) >= 2:
                        rot_arg = raw_args[1]
                        if hasattr(rot_arg, 'name') and rot_arg.name in getattr(self, '_const_values', {}):
                            rotation = self._const_values[rot_arg.name]
                    result = self._container.new_ckks_rotate(args[0], rotation)
            elif callee_name == 'rescale' and self.domain in ["fhe::sihe", "fhe::ckks"]:
                if len(args) >= 1 and hasattr(self._container, 'new_ckks_rescale'):
                    result = self._container.new_ckks_rescale(args[0])
            elif callee_name == 'relin' and self.domain in ["fhe::sihe", "fhe::ckks"]:
                if len(args) >= 1 and hasattr(self._container, 'new_ckks_relin'):
                    result = self._container.new_ckks_relin(args[0])
            elif callee_name == 'mod_switch' and self.domain in ["fhe::sihe", "fhe::ckks"]:
                if len(args) >= 1 and hasattr(self._container, 'new_ckks_mod_switch'):
                    result = self._container.new_ckks_mod_switch(args[0])
            elif callee_name == 'bootstrap' and self.domain in ["fhe::sihe", "fhe::ckks"]:
                if len(args) >= 1 and hasattr(self._container, 'new_ckks_bootstrap'):
                    result = self._container.new_ckks_bootstrap(args[0])
            elif callee_name == 'neg' and self.domain in ["fhe::sihe", "fhe::ckks"]:
                if len(args) >= 1 and hasattr(self._container, 'new_ckks_neg'):
                    result = self._container.new_ckks_neg(args[0])
            
            if result is not None and op.result_vars:
                self._var_map[op.result_vars[0].name] = result
        
        elif op_type == 'ForLoop':
            self._lower_for_loop(op)
        
        elif op_type == 'If':
            self._lower_if(op)
    
    def _lower_for_loop(self, op):
        """Lower a for loop to AIR."""
        loop_var_name = op.loop_var
        iterable = op.operands.get('iterable')
        body_block = op.nested_blocks[0] if op.nested_blocks else None
        
        if body_block is None:
            return
        
        # Get range info
        range_start = 0
        range_end = 10
        is_range = False
        
        if hasattr(iterable, 'name') and hasattr(self, '_range_info'):
            if iterable.name in self._range_info:
                range_start, range_end = self._range_info[iterable.name]
                is_range = True
        
        # Emit loop begin
        loop_node = None
        if hasattr(self._container, 'new_loop_begin_range') and is_range:
            loop_node = self._container.new_loop_begin_range(range_start, range_end)
        elif hasattr(self._container, 'new_loop_begin'):
            iterable_val = self._get_value(iterable)
            loop_node = self._container.new_loop_begin(iterable_val)
        
        # Get loop index
        if loop_node and hasattr(self._container, 'new_loop_index'):
            idx_node = self._container.new_loop_index(loop_node)
            self._var_map[loop_var_name] = idx_node
        else:
            self._var_map[loop_var_name] = self._container.new_intconst(0)
        
        # Lower body
        self._lower_block(body_block)
        
        # Emit loop end
        if hasattr(self._container, 'new_loop_end'):
            self._container.new_loop_end()
    
    def _lower_if(self, op):
        """Lower an if statement to AIR."""
        cond = self._get_value(op.operands.get('condition'))
        then_block = op.nested_blocks[0] if op.nested_blocks else None
        else_block = op.nested_blocks[1] if len(op.nested_blocks) > 1 else None
        
        # Emit if begin
        if hasattr(self._container, 'new_if_begin'):
            self._container.new_if_begin(cond)
        
        # Lower then block
        if then_block:
            self._lower_block(then_block)
        
        # Lower else block
        if else_block:
            if hasattr(self._container, 'new_else'):
                self._container.new_else()
            self._lower_block(else_block)
        
        # Emit if end
        if hasattr(self._container, 'new_if_end'):
            self._container.new_if_end()
    
    def _emit_domain_binop(self, op_name: str, left, right):
        """Emit binary operation using domain-specific ops on the base container.
        
        For FHE domains (sihe, ckks, poly), results are stored to intermediate
        variables to avoid nested expression trees that the poly pass can't handle.
        """
        c = self._container
        result = None
        is_fhe_domain = self.domain in ("fhe::sihe", "fhe::ckks", "fhe::poly")
        
        if self.domain == "nn::core":
            # nn::core operations
            if op_name == 'add':
                result = c.new_nn_add(left, right)
            elif op_name == 'sub':
                result = c.new_nn_sub(left, right)
            elif op_name == 'mul':
                result = c.new_nn_mul(left, right)
                
        elif self.domain == "nn::vector":
            # nn::vector operations
            if op_name == 'add':
                result = c.new_vec_add(left, right)
            elif op_name == 'sub':
                result = c.new_vec_sub(left, right)
            elif op_name == 'mul':
                result = c.new_vec_mul(left, right)
                
        elif self.domain == "fhe::sihe":
            # fhe::sihe operations
            if op_name == 'add':
                result = c.new_sihe_add(left, right)
            elif op_name == 'sub':
                result = c.new_sihe_sub(left, right)
            elif op_name == 'mul':
                result = c.new_sihe_mul(left, right)
                
        elif self.domain == "fhe::ckks":
            # fhe::ckks operations
            if op_name == 'add':
                result = c.new_ckks_add(left, right)
            elif op_name == 'sub':
                result = c.new_ckks_sub(left, right)
            elif op_name == 'mul':
                result = c.new_ckks_mul(left, right)
                
        elif self.domain == "fhe::poly":
            # fhe::poly operations
            if op_name == 'add':
                result = c.new_poly_add(left, right)
            elif op_name == 'sub':
                result = c.new_poly_sub(left, right)
            elif op_name == 'mul':
                result = c.new_poly_mul(left, right)
        
        # Fallback to air::core operations (for @kernel decorator)
        if result is None:
            if op_name == 'add':
                result = c.new_add(left, right)
            elif op_name == 'sub':
                result = c.new_sub(left, right)
            elif op_name == 'mul':
                result = c.new_mul(left, right)
            elif op_name == 'matmul':
                result = c.new_matmul(left, right)
            elif op_name == 'truediv':
                result = c.new_div(left, right)
            else:
                raise NotImplementedError(f"Op {op_name} not implemented for domain {self.domain}")
        
        # For FHE domains, store result to intermediate variable to flatten tree
        if is_fhe_domain and result is not None and hasattr(c, 'new_stid'):
            # Generate unique temp name
            if not hasattr(self, '_temp_counter'):
                self._temp_counter = 0
            temp_name = f"_fhe_tmp_{self._temp_counter}"
            self._temp_counter += 1

            # Store and return load
            stored = c.new_stid(temp_name, result)
            self._var_map[temp_name] = stored
            return stored

        return result

    def _get_value(self, val):
        if val is None:
            return None
        if hasattr(val, 'name') and val.name in self._var_map:
            return self._var_map[val.name]
        return None

    def _is_constant_node(self, node):
        """Check if a node is a constant (intconst or floatconst)."""
        if node is None:
            return False
        # Check if node has a method to determine if it's a constant
        # AIR nodes have specific methods/attributes we can check
        node_str = str(node)
        return 'intconst' in node_str.lower() or 'floatconst' in node_str.lower()

    def _wrap_constant_in_encode(self, node):
        """
        Wrap a constant node in SIHE.encode for CKKS operations.

        This is necessary because CKKS binary operations expect operands to be either:
        1. Ciphertext (CIPHER type)
        2. Encoded plaintext (PLAINTEXT type from SIHE.encode)
        3. NOT raw primitive constants

        The SIHE→CKKS transformation pass will convert SIHE.encode to CKKS.encode
        with proper scale and level extraction.
        """
        # Debug: print available container methods
        print(f"[DEBUG-ENCODE] Container type: {type(self._container).__name__}")
        print(f"[DEBUG-ENCODE] Available encode methods: {[m for m in dir(self._container) if 'encode' in m.lower()]}")

        if not hasattr(self._container, 'new_sihe_encode'):
            # Fallback if encoding is not available
            print(f"[WARN] new_sihe_encode not available, constant will remain unwrapped")
            print(f"[DEBUG] Available methods: {[m for m in dir(self._container) if not m.startswith('_')][:20]}")
            return node

        print(f"[DEBUG-ENCODE] Wrapping constant in SIHE.encode: {node}")
        encoded = self._container.new_sihe_encode(node)
        print(f"[DEBUG-ENCODE] Encoded result: {encoded}")
        return encoded


# =============================================================================
# Domain-Specific Kernel Classes
# =============================================================================

class TensorKernel(DomainKernel):
    """Kernel at Tensor/air::core level (default)."""
    DOMAIN = "air::core"
    START_PASS = None  # Full pipeline


class NNKernel(DomainKernel):
    """Kernel at nn::core level - neural network operations."""
    DOMAIN = "nn::core"
    START_PASS = "vector-pass"  # Start from vector pass


class VectorKernel(DomainKernel):
    """Kernel at nn::vector level - after vectorization."""
    DOMAIN = "nn::vector"
    START_PASS = "sihe-pass"  # Skip vector-pass


class SiheKernel(DomainKernel):
    """Kernel at fhe::sihe level - scheme-independent FHE."""
    DOMAIN = "fhe::sihe"
    START_PASS = "ckks-pass"  # Skip vector + sihe passes


class CkksKernel(DomainKernel):
    """Kernel at fhe::ckks level - CKKS-specific.

    Compiles directly to CKKS domain ops (CKKS.add, CKKS.mul, CKKS.rotate, etc.)
    so that the resulting IR can be inlined into CKKS-level code without needing
    additional transformation passes.
    """
    DOMAIN = "fhe::ckks"
    START_PASS = "ckks-driver"  # Skip sihe2ckks since we're already CKKS


class PolyKernel(DomainKernel):
    """Kernel at fhe::poly level - polynomial operations."""
    DOMAIN = "fhe::poly"
    START_PASS = "poly2c-pass"  # Only C generation


# =============================================================================
# Decorators
# =============================================================================

def kernel(func: Callable) -> TensorKernel:
    """
    Decorator for tensor-level kernels (air::core).
    
    Example:
        @kernel
        def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
            return a + b
    """
    return TensorKernel(func.__name__, func)


def nn_kernel(func: Callable) -> NNKernel:
    """
    Decorator for nn::core level kernels.
    
    Example:
        @nn_kernel
        def nn_add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
            return a + b
    """
    return NNKernel(func.__name__, func)


def vector_kernel(func: Callable) -> VectorKernel:
    """
    Decorator for vector-level kernels (nn::vector).
    
    Example:
        @vector_kernel
        def vec_add(a: VectorTensor[64], b: VectorTensor[64]) -> VectorTensor[64]:
            return a + b
    """
    return VectorKernel(func.__name__, func)


def sihe_kernel(func: Callable) -> SiheKernel:
    """
    Decorator for SIHE-level kernels (fhe::sihe).
    
    Example:
        @sihe_kernel
        def fhe_add(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
            return a + b
    """
    return SiheKernel(func.__name__, func)


def ckks_kernel(func: Callable) -> CkksKernel:
    """
    Decorator for CKKS-level kernels (fhe::ckks).
    
    Example:
        @ckks_kernel
        def ckks_mul(a: CkksCiphertext, b: CkksCiphertext) -> CkksCiphertext:
            result = a * b
            # Would need rescale in real CKKS
            return result
    """
    return CkksKernel(func.__name__, func)


def poly_kernel(func: Callable) -> PolyKernel:
    """
    Decorator for polynomial-level kernels (fhe::poly).
    
    Example:
        @poly_kernel
        def ntt_mul(a: Polynomial[4096], b: Polynomial[4096]) -> Polynomial[4096]:
            # Low-level polynomial multiplication
            return a * b
    """
    return PolyKernel(func.__name__, func)


__all__ = [
    # Types
    'Tensor', 'NNTensor', 'VectorTensor', 'SiheCiphertext', 'CkksCiphertext', 'CkksPlaintext', 'Polynomial',
    # Kernel classes
    'TensorKernel', 'NNKernel', 'VectorKernel', 'SiheKernel', 'CkksKernel', 'PolyKernel',
    # Decorators
    'kernel', 'nn_kernel', 'vector_kernel', 'sihe_kernel', 'ckks_kernel', 'poly_kernel',
    # CKKS operations
    'ckks',
]

