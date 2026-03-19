import ctypes
import numpy as np
import operator
from typing_extensions import deprecated
from functools import reduce
from typing import (
    Generic,
    Protocol,
    Union,
    Any,
    Type,
    TypeVar,
    overload,
    runtime_checkable,
)

from .common import *
from .ast_helpers import const_expr

# NOTE: This file is for AIR, not MLIR
# We don't import MLIR helpers - we use AIR instead
# AIR types and operations will be handled through air_builder bindings

# Try to import AIR bindings (may not be available)
try:
    from ace_bindings import air_builder
except ImportError:
    air_builder = None

# =============================================================================
# Dynamic Expression Protocol
# =============================================================================


@runtime_checkable
class DslDynamicExpressionProtocol(Protocol):
    """
    This is a protocol class that provides a common interface
    to generate user-defined dynamic expressions.

    The DSL checks this protocol to determine if a class is a dynamic expression (AIR value) or not.
    It is also used in _is_dynamic_expression.

    NOTE: Adapted for AIR instead of MLIR.
    """

    def __extract_air_values__(self):
        """
        Generate a dynamic expression for the current object.
        Returns list of AIR nodes.
        """
        raise NotImplementedError

    def __new_from_air_values__(self, values):
        """
        Create a new object from AIR values.
        """
        raise NotImplementedError
    
    # Keep MLIR methods for compatibility but mark as deprecated
    def __extract_mlir_values__(self):
        """Deprecated: Use __extract_air_values__ instead"""
        return self.__extract_air_values__()
    
    def __new_from_mlir_values__(self, values):
        """Deprecated: Use __new_from_air_values__ instead"""
        return self.__new_from_air_values__(values)


class DslType(type):
    """Metaclass for all DSL types in the system.

    This metaclass provides type system infrastructure for DSL types, handling AIR
    type mappings and NumPy type conversions.

    NOTE: Adapted for AIR instead of MLIR.

    All data types in DSL must provide the following methods:

    - ``__str__`` (classmethod): Return string representation of the type
    - ``__get_air_types__``: Return list of AIR types of the AIR values contained in the instance
      (required for use as argument type of JIT function)
    - ``__extract_air_values__``: Return list of AIR values contained in the instance
      (required for calling from JIT function or as kernel arguments)
    - ``__new_from_air_values__``: Return a new instance from list of AIR values
      (required for calling from JIT function or as kernel arguments)
    - ``__c_pointers__`` (optional): Return list of ctypes pointers of data used to invoke JIT function
      (required for calling from Python runtime)

    Attributes
    ----------
    _air_builder : Any
        AIR builder provider

    Properties
    ----------
    air_type
        Returns the corresponding AIR type for this DSL type

    Examples
    --------
    Define a custom data type::

        .. code-block:: python

            class CustomData(metaclass=DslType, ...):
                def __init__(self, int_value, ...):
                    self.int_value = int_value
                    ...

                def __str__(cls):
                    return "CustomData[int, ...]"

                def __c_pointers__(self):
                    return [ctypes.pointer(ctypes.c_int32(self.int_value)), ...]

                def __get_mlir_types__(self):
                    return [_T.i32(), ...]

                def __extract_mlir_values__(self):
                    return [self.int_value, ...]

                def __new_from_mlir_values__(self, values):
                    return CustomData(values[0], ...)

    Use in JIT function::

        .. code-block:: python

            @jit
            def foo(x: CustomData):
                return x.int_value + 1

            # Emit: `%c0 = arith.constant(1, i32)`
            c1 = const(1, Int32)
            # `c1` tracks `%c0` defined outside of function body of `foo`
            # `%c0` can't be used directly in function body of `foo`
            x = CustomData(c1, ...)

    When called like ``y = foo(x)``, the following steps occur:

    1. JIT compiler generates MLIR function definition using ``__get_mlir_types__``::

        .. code-block:: mlir

            func @foo(%arg0: i32, ...) -> i32 {
                ...
            }

    2. Function is traced in Python, wrapping MLIR values with ``__new_from_mlir_values__``::

        .. code-block:: python

            # Implementation of IR tracing
            new_x = CustomData(ir.Value(%arg0), ...)
            y = foo(new_x)
            # `x.int_value` is %arg0 rather than `c1` defined outside

    3. For Python runtime execution, JIT engine invokes compiled function using ``__c_pointers__``::

        jit_engine.invoke(foo, concat([x.__c_pointers__(), ...]))

    For JIT function calls, MLIR values are extracted with ``__extract_mlir_values__``::

        .. code-block:: python

            @jit
            def caller():
                x = CustomData(1, ...)
                return foo(x)

        .. code-block:: mlir

            func @caller() -> i32 {
                %0 = func.call @foo(%arg0, ...) : (i32, ...) -> i32
                return %0 : i32
            }
    """

    # Placeholder type (AIR instead of MLIR)
    _air_type = Any

    def __new__(cls, name, bases, attrs, air_type=None, mlir_type=None, is_abstract=False, **kwargs):
        """
        Create a new DSL type class.
        
        NOTE: mlir_type parameter is kept for compatibility but ignored.
        Use air_type instead for AIR generation.
        """
        new_cls = super().__new__(cls, name, bases, attrs)
        # Prefer air_type, fall back to mlir_type for compatibility
        if air_type is not None:
            new_cls._air_type = staticmethod(air_type)
        elif mlir_type is not None:
            # Convert MLIR type to AIR type (placeholder - needs actual conversion)
            new_cls._air_type = staticmethod(lambda: None)  # TODO: Convert MLIR to AIR

        # TODO: add verifier to check if all following methods are implemented
        # - __str__
        # - __c_pointers__
        # - __get_air_types__
        # - __extract_air_values__
        # - __new_from_air_values__
        new_cls._is_abstract = is_abstract

        return new_cls

    @property
    def air_type(cls):
        """Returns the corresponding AIR type for this DSL type"""
        return cls._air_type() if callable(cls._air_type) else cls._air_type
    
    @property
    def mlir_type(cls):
        """Deprecated: Use air_type instead. Returns None for AIR-based types."""
        return None

    @property
    def is_abstract(cls):
        return cls._is_abstract


class NumericMeta(DslType):
    """Metaclass for numeric types providing width and numpy dtype information.

    Parameters
    ----------
    width : int, default=8
        Bit width of the numeric type
    np_dtype : numpy.dtype, optional
        Corresponding NumPy dtype
    mlir_type : Any, optional
        Corresponding MLIR type

    Attributes
    ----------
    width : int
        Bit width of the numeric type
    _np_dtype : Union[numpy.dtype, None]
        Corresponding NumPy dtype

    Methods
    -------
    numpy_dtype : property
        Returns the corresponding NumPy dtype
    """

    width: int

    _np_dtype: Union[np.dtype, None]

    def __new__(
        cls,
        name,
        bases,
        attrs,
        width=8,
        np_dtype=None,
        mlir_type=None,
        is_abstract=False,
        **kwargs,
    ):
        def _extract_mlir_values(self):
            return [self.ir_value()]

        def _new_from_mlir_values(self, values: list) -> "Numeric":
            res_ty = type(self)
            return res_ty(values[0])

        new_attrs = {
            "__extract_mlir_values__": _extract_mlir_values,
            "__new_from_mlir_values__": _new_from_mlir_values,
        }
        new_cls = super().__new__(
            cls,
            name,
            bases,
            new_attrs | attrs,
            mlir_type=mlir_type,
            is_abstract=is_abstract,
            **kwargs,
        )
        new_cls.width = width
        new_cls._np_dtype = np_dtype
        return new_cls

    @property
    def numpy_dtype(cls):
        return cls._np_dtype

    @property
    def is_integer(cls) -> bool: ...

    @property
    def is_float(cls) -> bool: ...

    def is_same_kind(cls, other: Type) -> bool:
        return cls.is_integer == other.is_integer or cls.is_float == other.is_float

    @staticmethod
    def from_python(value: Any) -> Type["Numeric"]:
        """
        Deduce the DSL type from a Python value.
        """
        if isinstance(value, int):
            return Int32
        elif isinstance(value, float):
            return Float32
        elif isinstance(value, bool):
            return Boolean
        raise DSLRuntimeError(
            f"Could not deduce Type[Numeric] from python value: {value} :{type(value)}"
        )


Value = TypeVar("Value")


def as_value(obj: Union[bool, int, float, "Numeric"]) -> Any:  # MLIR ir.Value removed
    res = None
    if isinstance(obj, Numeric):
        res = obj.ir_value()
    else:
        res = obj
    return res


def cast(obj: Union[bool, int, float, Value], type_: Type["Numeric"]) -> "Numeric":
    """Cast an object to the specified numeric type.

    :param obj: Object to be cast
    :type obj: Union[bool, int, float, Value]
    :param type_: Target numeric type
    :type type_: Type[Numeric]
    :raises TypeError: If casting to an abstract type or unsupported type conversion
    :return: Object cast to the target numeric type
    :rtype: Numeric

    Example::
        >>> x = cast(5, Int32)  # Cast integer to Int32
        >>> y = cast(3.14, Float32)  # Cast float to Float32
    """
    if type_.is_abstract:
        if not isinstance(obj, type_):
            raise TypeError(
                f"can't cast {obj} to {type_}. Pass in concrete type instead, "
                "e.g. Int32, Float32, etc."
            )
        # If target_type is abstract, and value is instance of target_type,
        # then we can return value as is
    else:
        # Implicit cast based on using annotation type
        obj = type_(obj)
    return obj


# Option 1: use ir.Value as base
# class IntegerMeta(DslType, type(ir.Value)):
class IntegerMeta(NumericMeta):
    """Metaclass for integer types providing signedness information.

    :param width: Bit width of the integer type, defaults to 32
    :type width: int
    :param signed: Whether the integer type is signed, defaults to True
    :type signed: bool
    :param mlir_type: Corresponding MLIR type, defaults to None
    :type mlir_type: Any, optional

    :ivar signed: Whether the integer type is signed
    :vartype signed: bool
    :ivar arith: Arithmetic operations interface
    :vartype arith: Any
    """

    signed: bool

    def __new__(
        cls,
        name,
        bases,
        attrs,
        width=32,
        signed=True,
        mlir_type=None,
        is_abstract=False,
    ):
        if width == 1:
            np_dtype = np.bool_
        elif width == 128:
            np_dtype = None
        elif signed:
            np_dtype = getattr(np, f"int{width}")
        else:
            np_dtype = getattr(np, f"uint{width}")

        def _c_pointers(self):
            if width == 1:
                c_value = ctypes.c_bool(self.value)
            elif signed:
                c_value = getattr(ctypes, f"c_int{width}")(self.value)
            else:
                c_value = getattr(ctypes, f"c_uint{width}")(self.value)

            return [ctypes.cast(ctypes.pointer(c_value), ctypes.c_void_p)]

        new_attrs = {
            "__c_pointers__": _c_pointers,
        }
        new_cls = super().__new__(
            cls, name, bases, attrs | new_attrs, width, np_dtype, mlir_type, is_abstract
        )
        new_cls.signed = signed
        return new_cls

    def __str__(cls):
        return f"{cls.__name__}"

    @property
    def is_integer(cls) -> bool:
        return True

    @property
    def is_float(cls) -> bool:
        return False

    @property
    def zero(cls) -> int:
        return 0

    @property
    def min(cls) -> int:
        if cls.signed:
            return -(2 ** (cls.width - 1))
        else:
            return 0

    @property
    def max(cls) -> int:
        if cls.signed:
            return 2 ** (cls.width - 1) - 1
        else:
            return 2**cls.width - 1

    def recast_width(cls, width):
        return eval(f"Int{width}")


class FloatMeta(NumericMeta):
    """Metaclass for floating-point types.

    This metaclass provides type system infrastructure for floating-point types in the DSL,
    handling MLIR type mappings and NumPy type conversions.

    :param width: Bit width of the float type, defaults to 32
    :type width: int
    :param mlir_type: Corresponding MLIR type, defaults to None
    :type mlir_type: Any, optional
    :param is_abstract: Whether this is an abstract base class, defaults to False
    :type is_abstract: bool, optional

    :ivar _arith: Arithmetic operations interface
    :vartype _arith: Any

    Note: Does not have 1-to-1 mapping for special types like bfloat16, tfloat32
    """

    _exponent_width: int
    _mantissa_width: int

    def __new__(cls, name, bases, attrs, width=32, mlir_type=None, is_abstract=False):
        np_dtype = getattr(np, name.lower(), None)
        new_cls = super().__new__(
            cls, name, bases, attrs, width, np_dtype, mlir_type, is_abstract
        )
        # Extract exponent and mantissa bits from class name if it follows Float<E><M> pattern
        # For example: Float8E4M3 -> exponent_width=4, mantissa_width=3
        import re

        if not is_abstract:
            match = re.match(r"Float(\d+)E(\d+)M(\d+)(?:.*)", name)
            if match:
                exp_bits = int(match.group(2))
                mant_bits = int(match.group(3))

                # Store extracted values as class attributes
                new_cls._exponent_width = exp_bits
                new_cls._mantissa_width = mant_bits
        # Don't have 1-to-1 mapping of narrow precision types like bfloat16, tfloat32, etc.
        return new_cls

    def __str__(cls):
        return f"{cls.__name__}"

    @property
    def is_integer(cls) -> bool:
        return False

    @property
    def is_float(cls) -> bool:
        return True

    @property
    def zero(cls) -> float:
        return 0.0

    @property
    def inf(cls) -> float:
        return float("inf")

    @property
    def nan(cls) -> float:
        return float("nan")

    @property
    def exponent_width(cls) -> int:
        return cls._exponent_width

    @property
    def mantissa_width(cls) -> int:
        return cls._mantissa_width

    def recast_width(cls, width):
        return eval(f"Float{width}")


def _arith_signless_to_int(a, target_type):
    # is_signed: sign of result type
    if target_type.width > a.type.width:
        # arith dialect consider `1` in `i1` as `-1`, treat it as unsigned for DSL
        # MLIR arith operations removed - ace_edsl uses AIR instead
        if target_type.signed and a.type.width > 1:
            raise NotImplementedError("extsi not available - ace_edsl uses AIR, not MLIR")
        else:
            raise NotImplementedError("extui not available - ace_edsl uses AIR, not MLIR")
    elif target_type.width < a.type.width:
        raise NotImplementedError("trunci not available - ace_edsl uses AIR, not MLIR")
    else:
        return a


def _binary_op_type_promote(a, b, promote_bool: bool = False):
    """Promote two numeric operands following type promotion rules.

    :param a: First numeric operand
    :type a: Numeric
    :param b: Second numeric operand
    :type b: Numeric
    :param promote_bool: Whether to promote boolean types to Int32 for arithmetic operations, defaults to False
    :type promote_bool: bool, optional
    :raises ValueError: If implicit float promotion is not supported between the given types
    :return: Tuple containing promoted operands and their resulting type
    :rtype: tuple[Numeric, Numeric, Type[Numeric]]

    Type promotion rules:
    1. If operands are same type and not bools needing promotion:
       - No promotion needed, return original types
    2. If either operand is float:
       a. If one is float and one is int:
          - Convert int to the float type
       b. If both are float:
          - Promote to higher precision float if width >= 16
          - For same width, promote to more general type (Float32 over TFloat32)
          - Otherwise raise ValueError for unsupported promotion
    3. Otherwise, both operands are integers. Integer promotion rules:
       a. If promote_bool is True and either operand is bool:
          - Promote bool to Int32 for arithmetic operations

    Exceptions for numpy dtype casting:
    - array(dtype=np.bool_) + array(dtype=np.bool_) -> array(dtype=np.bool_)

    What is not supported:
    - promotion with narrow precision float types which requires explicit cast by user
    """
    a_type = a.dtype
    b_type = b.dtype

    # Early return for same types (except when they're bools that need promotion)
    if a_type == b_type and not (promote_bool and a_type.width == 1):
        return a, b, a_type

    # Handle floating point promotions
    if a_type.is_float or b_type.is_float:
        # Get highest precision float type based on bitwidth
        a_width = getattr(a_type, "width", 0)
        b_width = getattr(b_type, "width", 0)

        # If one type is integer, convert it to the float type
        if a_type.is_float and not b_type.is_float:
            b_type = a_type.recast_width(max(a_width, b_width))
        elif b_type.is_float and not a_type.is_float:
            a_type = b_type.recast_width(max(a_width, b_width))

        # Both are float types - handle precision promotion
        if a_width > b_width and a_width >= 16:
            res_type = a_type
        elif b_width > a_width and b_width >= 16:
            res_type = b_type
        elif a_width == b_width:
            # Same bitwidth - handle special cases like TFloat32 -> Float32 and BFloat16 -> Float16
            if isinstance(a, Float64) or isinstance(b, Float64):
                res_type = Float64
            elif isinstance(a, Float32) or isinstance(b, Float32):
                res_type = Float32
            elif isinstance(a, Float16) or isinstance(b, Float16):
                res_type = Float16
            else:
                raise ValueError(
                    f"implicit float promotion of {a_type} or {b_type} is not supported, cast explicitly"
                )
        else:
            raise ValueError(
                f"implicit float promotion of {a_type} or {b_type} is not supported, cast explicitly"
            )

        # Only convert if type is different
        new_a = a.to(res_type) if a.dtype != res_type else a
        new_b = b.to(res_type) if b.dtype != res_type else b
        print(f"a: {type(new_a)}, b: {type(new_b)}, res_type: {res_type}")
        return new_a, new_b, res_type

    # Handle bool promotion for arithmetic operations
    if promote_bool:
        if a_type is Boolean and b_type is Boolean:
            # Only promote to Int32 when both are bool
            a = a.to(Int32)
            b = b.to(Int32)
            a_type = b_type = a.dtype

        # If both were bools, they're now same type (Int32)
        if a_type == b_type:
            return a, b, a_type

    # Same type, no promotion needed
    if a_type == b_type:
        return a, b, a_type

    a_signed = a_type.signed
    b_signed = b_type.signed
    a_width = a_type.width
    b_width = b_type.width

    # Mixed signedness case
    if a_signed != b_signed:
        unsigned_type = a_type if not a_signed else b_type
        signed_type = a_type if a_signed else b_type
        unsigned_width = a_width if not a_signed else b_width

        if unsigned_width >= signed_type.width:
            # Promote both to unsigned of larger width
            res_type = unsigned_type
        else:
            # Promote both to signed of larger width
            res_type = signed_type

        new_a = a.to(res_type) if a.dtype != res_type else a
        new_b = b.to(res_type) if b.dtype != res_type else b
        return new_a, new_b, res_type

    # Same signedness, different width - promote to larger width
    if a_width >= b_width:
        return a, b.to(a.dtype), a.dtype
    else:
        return a.to(b.dtype), b, b.dtype


def _binary_op(op, promote_operand=True, promote_bool=False, flip=False):
    """Wrapper for binary operations on Numeric types.

    This wrapper handles type promotion, operation execution, and result type determination
    for binary operations between Numeric types.

    :param op: The binary operation to perform (e.g., operator.add, operator.sub)
    :type op: callable
    :param emitter: Function that emits the MLIR operation for dynamic values
    :type emitter: callable
    :param promote_operand: Whether to promote operands to the same type, defaults to True
    :type promote_operand: bool, optional
    :param promote_bool: Whether to promote boolean results to Boolean type, defaults to False
    :type promote_bool: bool, optional
    :param flip: Whether to flip the operands when calling the operation, defaults to False
    :type flip: bool, optional

    :raises TypeError: When an unsupported operation is attempted on specific numeric types

    .. note::
        Not all operations are supported for all numeric types. In particular:

        - Subtraction is not fully supported for Integer types
        - Multiplication, floor division, and modulo operations may have limited support
        - Division (truediv) with integer types is not fully supported and converts to Float32
    """

    def wrapper(lhs, rhs, *, loc=None, ip=None):
        orig_lhs_type = type(lhs)
        orig_rhs_type = type(rhs)

        # When called directly with self and other
        ty = type(lhs)
        # Canonicalize to Numeric type for promotion
        if not isinstance(rhs, Numeric):
            if not isinstance(rhs, (int, float, bool)):  # MLIR types removed
                # This allows rhs class to implement __rmul__
                return NotImplemented
            rhs = as_numeric(rhs)

        # default result type to left-hand-side
        res_type = ty

        if promote_operand:
            lhs, rhs, res_type = _binary_op_type_promote(lhs, rhs, promote_bool)
        else:
            rhs = ty(rhs)

        if op in (
            operator.lt,
            operator.le,
            operator.gt,
            operator.ge,
            operator.eq,
            operator.ne,
        ):
            res_type = Boolean
        elif op == operator.truediv and isinstance(lhs, Integer):
            res_type = Float32
        elif promote_bool and orig_lhs_type == Boolean and orig_rhs_type == Boolean:
            res_type = Boolean

        if False:  # MLIR ArithValue check removed
            lhs_val = lhs.value.with_signedness(lhs.signed)
        else:
            lhs_val = lhs.value

        if False:  # MLIR ArithValue check removed
            rhs_val = rhs.value.with_signedness(rhs.signed)
        else:
            rhs_val = rhs.value

        if flip:
            lhs_val, rhs_val = rhs_val, lhs_val

        # MLIR operations removed - ace_edsl uses AIR instead
        # Skip MLIR-specific code path - use Python operations instead
        res_val = op(lhs_val, rhs_val)
        return res_type(res_val, loc=loc, ip=ip)

    return wrapper


class Numeric(metaclass=NumericMeta, is_abstract=True):
    """Base class for all numeric types in the DSL.

    This class provides the foundation for both Integer and Float types,
    implementing basic arithmetic operations.

    :param value: The value to store in the numeric type
    :type value: Union[bool, int, float, Value]

    :ivar value: The stored numeric value
    :vartype value: Union[bool, int, float, Value]
    """

    def __init__(self, value: Union[bool, int, float, Value], *, loc=None, ip=None):
        self.value = value

    def __str__(self):
        return f"{self.value} : {type(self)}"

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self.value})"

    def __hash__(self):
        return hash(type(self).__class__) ^ hash(self.value)

    @property
    def dtype(self) -> Type["Numeric"]:
        return type(self)

    @overload
    def to(self, dtype: Type["Numeric"], *, loc=None, ip=None) -> "Numeric": ...

    @overload
    def to(self, dtype: Type[int], *, loc=None, ip=None) -> int: ...

    @overload
    def to(self, dtype: Type[float], *, loc=None, ip=None) -> float: ...

    @overload
    def to(self, dtype: Type[bool], *, loc=None, ip=None) -> bool: ...

    # MLIR ir.Value overload removed - ace_edsl uses AIR instead
    # @overload
    # def to(self, dtype: Type[ir.Value], *, loc=None, ip=None) -> ir.Value: ...

    def to(self, dtype: Type, *, loc=None, ip=None):
        """Convert this numeric value to another numeric type.

        If the target type is the same as the current type, returns self.
        Otherwise, creates a new instance of the target type with the same value.

        :param dtype: The target numeric type to convert to
        :type dtype: Union[Type["Numeric"], Type[int], Type[float], Type[bool]]
        :return: A new instance of the target type, or self if types match
        :rtype: Numeric
        :raises TypeError: If trying to convert an MLIR value to a static Python type

        Example::

            .. code-block:: python

                # Convert between DSL numeric types
                x = Int32(5)
                y = x.to(Float32)  # Converts to Float32(5.0)

                # Convert to Python primitive types
                # They are considered as static values at JIT time
                z = x.to(int)      # Returns Python int 5
                w = y.to(float)    # Returns Python float 5.0

                # This will raise a ValueError
                mlir_val = arith.constant(T.i32(), 42)
                num = Int32(mlir_val)
                num.to(int)        # ValueError: unable to convert MLIR value to static type: <class 'int'>
        """
        if isinstance(dtype, type(self)):
            return self
        elif isinstance(dtype, NumericMeta):
            return dtype(self)
        # MLIR ir.Value conversion removed - ace_edsl uses AIR instead
        elif dtype in (int, float, bool):
            # ace_edsl doesn't use MLIR values
            return dtype(self.value)
        else:
            raise ValueError(f"unable to convert {type(self)} to {dtype}")

    def ir_value(self, *, loc=None, ip=None):
        # MLIR ir_value removed - ace_edsl uses AIR instead
        raise NotImplementedError("ir_value() not available - ace_edsl uses AIR, not MLIR")

    def is_static(self):
        return False

    @deprecated("MLIR removed - ace_edsl uses AIR instead")
    def as_value(self):
        """MLIR as_value removed - ace_edsl uses AIR instead."""
        raise NotImplementedError("as_value() not available - ace_edsl uses AIR, not MLIR")

    @property
    def zero(self) -> "Numeric": ...

    def __dsl_not__(self, *, loc=None, ip=None):
        """DSL implementation of Python's `not` operator.

        Returns True if the value is equal to zero, False otherwise.
        This matches Python's behavior where any non-zero number is considered True.

        :param loc: The source location information, defaults to None
        :type loc: Optional[Location]
        :param ip: The insertion point for the operation, defaults to None
        :type ip: Optional[InsertionPoint]
        :return: The result of the logical not operation
        :rtype: Boolean
        """
        # MLIR arith.constant removed - ace_edsl uses AIR instead
        ty = type(self)
        zero_val = ty.zero  # Use Python value directly
        return self.__eq__(ty(zero_val), loc=loc, ip=ip)

    def __dsl_and__(self, other, *, loc=None, ip=None):
        """DSL implementation of Python's `and` operator.

        Returns the second operand if the first is truthy, otherwise returns the first operand.
        A numeric value is considered truthy if it is non-zero.

        :param other: The right-hand operand
        :type other: Numeric
        :param loc: The source location information, defaults to None
        :type loc: Optional[Location]
        :param ip: The insertion point for the operation, defaults to None
        :type ip: Optional[InsertionPoint]
        :return: The result of the logical and operation
        :rtype: Boolean

        Example::

            5 and 3 -> 3
            0 and 3 -> 0
            3 and 0 and ... -> 0
        """
        is_true = self.__dsl_bool__(loc=loc, ip=ip)

        def and_op(lhs, rhs):
            if isinstance(lhs, (int, float, bool)):
                if isinstance(rhs, (int, float, bool)):
                    return lhs and rhs
                else:
                    # MLIR arith operations removed - ace_edsl uses AIR instead
                    raise NotImplementedError("arith.select not available - ace_edsl uses AIR, not MLIR")
            else:
                if isinstance(rhs, (int, float, bool)):
                    # MLIR arith operations removed - ace_edsl uses AIR instead
                    raise NotImplementedError("arith.select not available - ace_edsl uses AIR, not MLIR")
                else:
                    # MLIR arith operations removed - ace_edsl uses AIR instead
                    raise NotImplementedError("arith.select not available - ace_edsl uses AIR, not MLIR")

        return _binary_op(and_op, promote_bool=True)(self, other, loc=loc, ip=ip)

    def __dsl_or__(self, other, *, loc=None, ip=None):
        """DSL implementation of Python's `or` operator.

        Returns the first operand if it is truthy, otherwise returns the second operand.
        A numeric value is considered truthy if it is non-zero.

        :param other: The right-hand operand
        :type other: Numeric
        :param loc: The source location information, defaults to None
        :type loc: Optional[Location]
        :param ip: The insertion point for the operation, defaults to None
        :type ip: Optional[InsertionPoint]
        :return: The result of the logical or operation
        :rtype: Boolean

        Example::

            5 or 3 -> 5
            0 or 3 -> 3
            3 or 0 -> 3
        """
        is_true = self.__dsl_bool__(loc=loc, ip=ip)

        def or_op(lhs, rhs):
            if isinstance(lhs, (int, float, bool)):
                if isinstance(rhs, (int, float, bool)):
                    return lhs or rhs
                else:
                    # MLIR arith operations removed - ace_edsl uses AIR instead
                    raise NotImplementedError("arith.select not available - ace_edsl uses AIR, not MLIR")
            else:
                if isinstance(rhs, (int, float, bool)):
                    # MLIR arith operations removed - ace_edsl uses AIR instead
                    raise NotImplementedError("arith.select not available - ace_edsl uses AIR, not MLIR")
                else:
                    # MLIR arith operations removed - ace_edsl uses AIR instead
                    raise NotImplementedError("arith.select not available - ace_edsl uses AIR, not MLIR")

        return _binary_op(or_op, promote_bool=True)(self, other, loc=loc, ip=ip)

    def __dsl_bool__(self, *, loc=None, ip=None) -> "Boolean":
        """DSL implementation of Python's __bool__ method.

        Returns a Boolean indicating whether this value is considered truthy.
        For numeric types, returns True if the value is non-zero.

        :param loc: The source location information, defaults to None
        :type loc: Optional[Location]
        :param ip: The insertion point for the operation, defaults to None
        :type ip: Optional[InsertionPoint]
        :return: True if this value is truthy (non-zero), False otherwise
        :rtype: Boolean
        """
        zero = type(self).zero
        return self.__ne__(zero, loc=loc, ip=ip)

    def __bool__(self):
        if not self.is_static():
            return True
        else:
            return bool(self.value)

    def __neg__(self, *, loc=None, ip=None): ...

    @staticmethod
    def _from_python_value(value):
        if isinstance(value, Numeric):
            return value

        if isinstance(value, bool):
            res_type = Boolean
        elif isinstance(value, int):
            # Should be Int64, keep Int32 as CuTe IR only support int32_t {$nv-internal-release}
            res_type = Int32
        elif isinstance(value, float):
            # Should be Float64 {$nv-internal-release}
            res_type = Float32
        # MLIR ArithValue removed - ace_edsl uses AIR instead
        elif False:
            pass
        elif False:  # MLIR BlockArgument removed
            # MLIR from_mlir_type removed - ace_edsl uses AIR instead
            raise NotImplementedError("from_mlir_type() not available - ace_edsl uses AIR, not MLIR")
        else:
            raise ValueError(f"unable to convert {value} to Numeric")
        return res_type(value)

    def __add__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.add, promote_bool=True)(self, other, loc=loc, ip=ip)

    def __sub__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.sub, promote_bool=True)(self, other, loc=loc, ip=ip)

    def __mul__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.mul, promote_bool=True)(self, other, loc=loc, ip=ip)

    def __floordiv__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.floordiv, promote_bool=True)(
            self, other, loc=loc, ip=ip
        )

    def __truediv__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.truediv, promote_bool=True)(
            self, other, loc=loc, ip=ip
        )

    def __mod__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.mod, promote_bool=True)(self, other, loc=loc, ip=ip)

    def __radd__(self, other, *, loc=None, ip=None) -> "Numeric":
        return self.__add__(other, loc=loc, ip=ip)

    def __rsub__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.sub, promote_bool=True, flip=True)(
            self, other, loc=loc, ip=ip
        )

    def __rmul__(self, other, *, loc=None, ip=None) -> "Numeric":
        return self.__mul__(other, loc=loc, ip=ip)

    def __rfloordiv__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.floordiv, promote_bool=True, flip=True)(
            self, other, loc=loc, ip=ip
        )

    def __rtruediv__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.truediv, promote_bool=True, flip=True)(
            self, other, loc=loc, ip=ip
        )

    def __rmod__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.mod, promote_bool=True, flip=True)(
            self, other, loc=loc, ip=ip
        )

    def __eq__(self, other, *, loc=None, ip=None) -> "Boolean":
        return _binary_op(operator.eq)(self, other, loc=loc, ip=ip)  # type: ignore

    def __ne__(self, other, *, loc=None, ip=None) -> "Boolean":
        return _binary_op(operator.ne)(self, other, loc=loc, ip=ip)  # type: ignore

    def __lt__(self, other, *, loc=None, ip=None) -> "Boolean":
        return _binary_op(operator.lt)(self, other, loc=loc, ip=ip)  # type: ignore

    def __le__(self, other, *, loc=None, ip=None) -> "Boolean":
        return _binary_op(operator.le)(self, other, loc=loc, ip=ip)  # type: ignore

    def __gt__(self, other, *, loc=None, ip=None) -> "Boolean":
        return _binary_op(operator.gt)(self, other, loc=loc, ip=ip)  # type: ignore

    def __ge__(self, other, *, loc=None, ip=None) -> "Boolean":
        return _binary_op(operator.ge)(self, other, loc=loc, ip=ip)  # type: ignore

    def __pow__(self, other, *, loc=None, ip=None) -> "Numeric":
        return _binary_op(operator.pow)(self, other, loc=loc, ip=ip)  # type: ignore

    def __c_pointers__(self):
        raise ValueError(
            f"only support built-in types: bool, (u)int{8, 16, 32, 64}, float{32, 64}, but got {type(self)}"
        )

    @staticmethod
    def from_mlir_type(mlir_type):
        # {$nv-internal-release begin}
        # This is a temporary WAR before moving everything to DslType with metaclass
        # After moving to metaclass, DSL type should be treated as first class citizen
        # that composed types like Vector, Pointer, Tensor should track Numeric type
        # {$nv-internal-release end}
        # MLIR from_mlir_type removed - ace_edsl uses AIR instead
        raise NotImplementedError("from_mlir_type() not available - ace_edsl uses AIR, not MLIR")


def as_numeric(obj: Union[bool, int, float, Numeric]) -> Numeric:  # MLIR types removed
    """Convert a Python primitive value to a Numeric type.

    :param obj: Python primitive value to convert
    :type obj: Union[bool, int, float]
    :return: The converted Numeric object
    :rtype: Numeric

    Example::

        .. code-block:: python

            x = as_numeric(5)  # Converts to Int32
            y = as_numeric(3.14)  # Converts to Float32
            z = as_numeric(True)  # Converts to Boolean
    """
    if isinstance(obj, Numeric):
        return obj
    return Numeric._from_python_value(obj)


class Scalar(Numeric, is_abstract=True):
    """
    Abstract scalar numeric type.

    This mirrors BaseDSL's use of abstract numeric types for signatures while
    allowing concrete scalar types (Int32, Float32, Boolean, etc.) to be chosen
    at call sites.
    """
    pass


class Integer(Numeric, metaclass=IntegerMeta, mlir_type=None, is_abstract=True):  # MLIR T removed
    """A class representing integer values with specific width and signedness.

    This class provides functionality to create and manipulate integer values with
    configurable width and signedness. It supports conversion from various input types
    including Python scalars, MLIR Values, and other numeric types.

    :param x: The input value to convert to this integer type
    :type x: Union[bool, int, float, Integer, Float]  # MLIR ir.Value removed

    :return: A new Integer instance with the converted value
    :rtype: Integer

    :raises AssertionError: If the type's numpy_dtype is None
    :raises NotImplementedError: If converting between different Integer types
    :raises ValueError: If the input type is not supported for conversion
    :raises OverflowError: If converting float infinity to integer

    Type conversion behavior:

    * Python scalars (bool, int, float):
        * Converted through numpy dtype casting
        * NaN and infinity values are rejected
        * Example: Int8(256) -> -256 (overflow behavior)

    * MLIR Value with IntegerType:
        * Width differences handled by signless to signed/unsigned conversion
        * Example: i8 -> i8/ui8 depending on target type

    * MLIR Value with FloatType:
        * Uses MLIR float-to-int conversion
        * NaN and infinity values is undefined behavior
        * Example: f32 -> i32/ui32 depending on target type

    * Integer:
        * Uses MLIR float-to-int conversion or numpy dtype casting
        * Example: Int32(Int32(5)) => 5

    * Float:
        * Uses MLIR float-to-int conversion
        * Example: Int32(Float(5.7)) -> 5

    Example usage:

    .. code-block:: python

        x = Int32(5)  # From integer
        y = Int32(True)  # From boolean
        z = Int32(3.7)  # From float (truncates)
        w = Int32(x)  # From same Integer type
        c5 = arith.constant(5, T.i32())
        a = Int32(c5)  # Treat c5 as int32 bitwise
    """

    def __init__(self, x, *, loc=None, ip=None):
        ty = type(self)

        if isinstance(x, (bool, int, float)):
            # Add check for NaN before numpy conversion
            if isinstance(x, float):
                if np.isnan(x):
                    raise ValueError("Cannot convert float NaN to integer")
                elif np.isinf(x):
                    raise OverflowError("Cannot convert float infinity to integer")

            np_dtype = ty.numpy_dtype
            assert np_dtype is not None, f"expects numpy.dtype, but got {np_dtype}"
            x_val = int(np.array(x).astype(np_dtype))
        elif type(x) == ty:
            x_val = x.value
        # MLIR ir.Value handling removed - ace_edsl uses AIR instead
        elif False:  # isinstance(x, ir.Value):  # type: ignore
            pass
            # x_val = x
            # if isinstance(x.type, ir.IntegerType):  # type: ignore
            #     if x.type.width != ty.width:
            #         # signless -> (u)int
            #         x_val = _arith_signless_to_int(x, ty)
            # elif isinstance(x.type, ir.FloatType):  # type: ignore
            #     # float -> (u)int
            #     x_val = arith_helper.fptoi(x, ty.signed, ty.mlir_type, loc=loc, ip=ip)
        elif isinstance(x, Integer):
            # MLIR ir.Value handling removed
            if False:  # isinstance(x.value, ir.Value):
                pass
                # x_val = arith_helper.int_to_int(x.ir_value(), ty)
            else:
                # For non-MLIR values, use numpy casting
                src_val = np.array(x.value, dtype=type(x).numpy_dtype)
                x_val = int(src_val.astype(ty.numpy_dtype))
        elif isinstance(x, Float):
            # float -> int is handled by Integer.__init__ recursively
            Integer.__init__(self, x.value)
            return
        else:
            raise DSLRuntimeError(f"{x} to integer conversion is not supported")

        super().__init__(x_val)

    def __str__(self) -> str:
        if hasattr(self.value, "pretty_str"):
            return self.value.pretty_str()
        # MLIR ir.Value removed - ace_edsl uses AIR instead
        elif False:  # isinstance(self.value, ir.Value):
            return "?"
        return self.value.__str__()

    def __invert__(self, *, loc=None, ip=None):
        res_type = type(self)
        # MLIR operations removed - ace_edsl uses AIR instead
        # For ace_edsl, use Python bitwise NOT
        return res_type(~self.value)

    def __lshift__(self, other, *, loc=None, ip=None):
        return _binary_op(operator.lshift)(self, other, loc=loc, ip=ip)

    def __rlshift__(self, other, *, loc=None, ip=None):
        other_ = as_numeric(other)
        if not isinstance(other_, Integer):
            raise ValueError(f"Cannot left shift {other_} with {self}")
        return other_.__lshift__(self, loc=loc, ip=ip)

    def __rshift__(self, other, *, loc=None, ip=None):
        return _binary_op(operator.rshift)(self, other, loc=loc, ip=ip)

    def __rrshift__(self, other, *, loc=None, ip=None):
        other_ = as_numeric(other)
        if not isinstance(other_, Integer):
            raise ValueError(f"Cannot right shift {other_} with {self}")
        return other_.__rshift__(self, loc=loc, ip=ip)

    def __and__(self, other, *, loc=None, ip=None):
        return _binary_op(operator.and_)(self, other, loc=loc, ip=ip)

    def __rand__(self, other, *, loc=None, ip=None):
        return self.__and__(other, loc=loc, ip=ip)

    def __or__(self, other, *, loc=None, ip=None):
        return _binary_op(operator.or_)(self, other, loc=loc, ip=ip)

    def __ror__(self, other, *, loc=None, ip=None):
        return self.__or__(other, loc=loc, ip=ip)

    def __xor__(self, other, *, loc=None, ip=None):
        return _binary_op(operator.xor)(self, other, loc=loc, ip=ip)

    def __rxor__(self, other, *, loc=None, ip=None):
        return self.__xor__(other, loc=loc, ip=ip)


class Float(Numeric, metaclass=FloatMeta, mlir_type=None, is_abstract=True):  # MLIR T removed
    """A class representing floating-point values.

    Parameters
    ----------
    x : Union[bool, int, float, ir.Value, Integer, Float]
        The input value to convert to this float type.

    Notes
    -----
    Type conversion behavior:

    1. Python scalars (bool, int, float):
       - Converted through numpy dtype casting
       - Example: Float32(1.7) -> 1.7

    2. MLIR Value with FloatType:
       - If width differs: converts between float types
       - Example: f16 -> f32

    3. MLIR Value with IntegerType:
       - Not supported, raises ValueError

    4. Integer:
       - Converts using MLIR int-to-float operation
       - Example: Float32(Int32(5)) -> 5.0

    5. Float:
       - Direct conversion between float types
       - Example: Float32(Float32(1.5)) -> 1.5

    Raises
    ------
    AssertionError
        If the type's numpy_dtype is None
    ValueError
        If conversion from the input type is not supported
    """

    def __init__(self, x, *, loc=None, ip=None):
        ty = type(self)

        if isinstance(x, (bool, int, float)):  # type: ignore
            # Why we need to convert x to with numpy?
            # np_dtype = ty.numpy_dtype
            # assert np_dtype is not None, f"expects numpy.dtype, but got {np_dtype}"
            # x = float(np.array(x).astype(np_dtype))
            super().__init__(float(x))
        # MLIR ir.Value handling removed - ace_edsl uses AIR instead
        elif False:  # isinstance(x, ir.Value):  # type: ignore
            pass
            # if isinstance(x.type, ir.IntegerType):  # type: ignore
            #     raise DSLRuntimeError("signless to float conversion is not implemented")
            # elif isinstance(x.type, ir.FloatType):  # type: ignore
            #     if x.type != ty.mlir_type:
            #         x = arith_helper.cvtf(x, ty.mlir_type, loc=loc, ip=ip)
            # super().__init__(x)
        elif isinstance(x, Integer):
            # MLIR ir.Value handling removed
            if False:  # isinstance(x.value, ir.Value):  # type: ignore
                pass
                # x = arith_helper.itofp(
                #     x.value, type(x).signed, ty.mlir_type, loc=loc, ip=ip
                # )
            else:
                x = float(x.value)
            super().__init__(x)
        elif isinstance(x, Float):
            Float.__init__(self, x.value)
        else:
            raise DSLRuntimeError(f"{x} to Float conversion is not supported")

    def __str__(self) -> str:
        # MLIR ir.Value removed - ace_edsl uses AIR instead
        if False:  # isinstance(self.value, ir.Value):
            return "?"
        return self.value.__str__()


class Boolean(Integer, metaclass=IntegerMeta, width=1, signed=True, mlir_type=None):  # MLIR T removed
    def __init__(
        self, a: Union[bool, int, float, "Value", Numeric], *, loc=None, ip=None
    ):
        if isinstance(a, (bool, int, float)):
            value = bool(a)
        elif isinstance(a, Boolean):
            value = a.value
        elif isinstance(a, Numeric):
            value = a.__dsl_bool__(loc=loc, ip=ip)
        # MLIR ir.Value handling removed - ace_edsl uses AIR instead
        elif False:  # isinstance(a, ir.Value):
            pass
            # if isinstance(a.type, ir.IntegerType):
            #     if a.type.width == 1:
            #         value = a
            #     else:
            #         value = arith.cmpi(
            #             arith.CmpIPredicate.ne,
            #             a,
            #             arith.constant(a.type, 0),
            #             loc=loc,
            #             ip=ip,
            #         )
            # elif isinstance(a.type, ir.FloatType):
            #     # In Python, bool(float("nan")) is True, so use unordered comparison here
            #     value = arith.cmpf(
            #         arith.CmpFPredicate.UNE,
            #         a,
            #         arith.constant(a.type, 0.0),
            #         loc=loc,
            #         ip=ip,
            #     )
            # else:
            #     raise DSLRuntimeError(f"Cannot convert {a} to Boolean")
        super().__init__(value, loc=loc, ip=ip)


class Int8(Integer, metaclass=IntegerMeta, width=8, signed=True, mlir_type=None): ...  # MLIR T removed


class Int16(Integer, metaclass=IntegerMeta, width=16, signed=True, mlir_type=None): ...  # MLIR T removed


class Int32(Integer, metaclass=IntegerMeta, width=32, signed=True, mlir_type=None): ...  # MLIR T removed


class Int64(Integer, metaclass=IntegerMeta, width=64, signed=True, mlir_type=None): ...  # MLIR T removed


class Int128(
    Integer, metaclass=IntegerMeta, width=128, signed=True, mlir_type=None  # MLIR T removed
): ...


class Uint8(Integer, metaclass=IntegerMeta, width=8, signed=False, mlir_type=None): ...  # MLIR T removed


class Uint16(
    Integer, metaclass=IntegerMeta, width=16, signed=False, mlir_type=None  # MLIR T removed
): ...


class Uint32(
    Integer, metaclass=IntegerMeta, width=32, signed=False, mlir_type=None  # MLIR T removed
): ...


class Uint64(
    Integer, metaclass=IntegerMeta, width=64, signed=False, mlir_type=None  # MLIR T removed
): ...


class Uint128(
    Integer, metaclass=IntegerMeta, width=128, signed=False, mlir_type=None  # MLIR T removed
): ...


class Float64(Float, metaclass=FloatMeta, width=64, mlir_type=None):  # MLIR T removed
    def __c_pointers__(self):
        res = []
        if const_expr(isinstance(self.value, float)):
            res = [
                ctypes.cast(
                    ctypes.pointer(ctypes.c_double(self.value)), ctypes.c_void_p
                )
            ]
        else:
            raise ValueError("only float is supported")
        return res


class Float32(Float, metaclass=FloatMeta, width=32, mlir_type=None):  # MLIR T removed
    def __c_pointers__(self):
        res = []
        if const_expr(isinstance(self.value, float)):
            res = [
                ctypes.cast(ctypes.pointer(ctypes.c_float(self.value)), ctypes.c_void_p)
            ]
        else:
            raise ValueError("only float is supported")
        return res


class TFloat32(Float, metaclass=FloatMeta, width=32, mlir_type=None):  # MLIR T removed
    def __c_pointers__(self):
        res = []
        if const_expr(isinstance(self.value, float)):
            res = Float.__c_pointers__(self)
        else:
            raise ValueError("only float is supported")
        return res


class Float16(Float, metaclass=FloatMeta, width=16, mlir_type=None):  # MLIR T removed
    def __c_pointers__(self):
        res = []
        if const_expr(isinstance(self.value, float)):
            # Convert float to float16 binary representation
            # First convert to numpy float16 to handle the conversion
            f16_val = np.float16(self.value)
            # Get the raw bits as a 16-bit integer
            bits = f16_val.view(np.uint16)
            # Create a short (16-bit int) with those bits
            c_val = ctypes.c_short(bits)
            res = [ctypes.cast(ctypes.pointer(c_val), ctypes.c_void_p)]
        else:
            raise ValueError("only float is supported")
        return res


class BFloat16(Float, metaclass=FloatMeta, width=16, mlir_type=None): ...  # MLIR T removed


class Float8E5M2(Float, metaclass=FloatMeta, width=8, mlir_type=None): ...  # MLIR T removed


class Float8E4M3FN(Float, metaclass=FloatMeta, width=8, mlir_type=None): ...  # MLIR T removed


class Float8E4M3B11FNUZ(
    Float, metaclass=FloatMeta, width=8, mlir_type=None  # MLIR T removed
): ...


# {$nv-internal-release begin}
class Float8E3M4(Float, metaclass=FloatMeta, width=8, mlir_type=None): ...  # MLIR T removed


# {$nv-internal-release end}


# Added missing float types
class Float8E4M3(Float, metaclass=FloatMeta, width=8, mlir_type=None): ...  # MLIR T removed


class Float8E8M0FNU(Float, metaclass=FloatMeta, width=8, mlir_type=None): ...  # MLIR T removed


class Float4E2M1FN(Float, metaclass=FloatMeta, width=4, mlir_type=None): ...  # MLIR T removed


class Float6E3M2FN(Float, metaclass=FloatMeta, width=6, mlir_type=None): ...  # MLIR T removed


class Float6E2M3FN(Float, metaclass=FloatMeta, width=6, mlir_type=None): ...  # MLIR T removed


ALL_DTYPES = {
    Int8,
    Int16,
    Int32,
    Int64,
    Int128,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Uint128,
    BFloat16,
    Float16,
    Float32,
    TFloat32,
    Float64,
    Float8E5M2,
    Float8E4M3,
    Float8E4M3FN,
    # {$nv-internal-release begin}
    Float8E3M4,
    # {$nv-internal-release end}
    Float8E8M0FNU,
    Float8E4M3B11FNUZ,
    Float4E2M1FN,
    Float6E2M3FN,
    Float6E3M2FN,
}
__STR_TO_DTYPE__ = {dt.__name__: dt for dt in ALL_DTYPES}


def dtype(dtype_) -> Type[Numeric]:
    t = None
    if const_expr(isinstance(dtype_, str) and dtype_ in __STR_TO_DTYPE__):
        t = __STR_TO_DTYPE__[dtype_]
    else:
        raise TypeError(f"can't interpret {dtype_} as data type")

    return t


##############################################################
# Tensor
##############################################################


class TensorMeta(DslType):
    _element_type = Any
    _shape = Any

    """
    Examples:
        >>> Tensor[Int32, (3,)]
        >>> Tensor[Float32, (3, 4)]
        >>> T = TypeVar("T")
        >>> Tensor[T, (3, 4, 5)]
    """

    def __new__(cls, name, bases, attrs, element_type=Any, shape=Any):
        new_cls = super().__new__(cls, name, bases, attrs)
        new_cls._element_type = element_type
        new_cls._shape = shape
        return new_cls


# Generic type
TY = TypeVar("TY")


class Constexpr(Generic[TY]):
    """Value is passed and computed by python interpreter"""

    pass


class align:
    def __init__(self, value: int):
        if value <= 0 or (value & (value - 1)) != 0:
            raise DSLRuntimeError("expects align be power of 2 as positive value")
        self._value = value

    def __str__(self):
        return f"align({self._value})"


class PointerMeta(DslType):
    def __new__(cls, name, bases, attrs, value_type=Int32, align_=align(1)):
        new_cls = super().__new__(
            cls,
            name,
            bases,
            attrs,
            mlir_type=None,  # MLIR ir.UnrankedMemRefType removed - ace_edsl uses AIR instead
        )
        new_cls._value_type = value_type
        new_cls._align = align_
        return new_cls

    def __eq__(cls, other):
        if not isinstance(other, PointerMeta):
            return False
        return (
            cls._value_type == other._value_type
            and cls._align._value == other._align._value
        )  # Compare alignment values

    def __hash__(cls):
        return hash((cls._value_type, cls._align._value))  # Hash alignment value

    def __getitem__(cls, params) -> Type["Pointer"]:
        value_type, align_ = params

        if not isinstance(align_, align):
            raise DSLRuntimeError(f"expects align but got {align_}")

        # Create new class with proper name and parameters
        new_cls = type(
            f"Pointer[{value_type.__name__}, {align_}]",
            (Pointer,),
            {},
            value_type=value_type,
            align_=align_,  # Pass alignment to __new__
        )
        return new_cls

    def __str__(cls):
        return f"ptr<{cls._value_type}, {cls._align}>"


class Pointer(metaclass=PointerMeta):
    """
    A pointer to a memory location.

    Examples:

        def foo(a : Pointer[Int32, align=8]):
            ...

    """

    def __init__(self, value):
        self.value = value

    def __str__(self):
        return f"{self.value} : {type(self)}"


class IRConst(Generic[TY]):
    """Value is passed as MLIR constant value for (arith.constant)."""

    def __init__(self, ty: TY):
        self.ty = ty


class IRValue(Generic[TY]):
    """Value is passed as MLIR dynamic value."""

    def __init__(self, ty: TY):
        self.ty = ty


class IRVariadic:
    """
    A helper class to pass a variadic number of arguments to a function.
    """

    def __init__(self, operands):
        """
        Create a list of variadic operands. `operands` must be SSA values.
        """
        self.operands = operands

    def block_arg_types(self):
        """
        Return the list of block args types.
        """
        return [operand.type for operand in self.operands]

    def set_func_args(self, block_args):
        """
        This function is called after entering a function. `block_args` are the
        block arguments that correspond to the passed operands. Derived classes
        may implement this function to provide convenience getters for block
        arguments.
        """
        pass

    def __len__(self):
        """
        Return the length of variadic operands.
        """
        return len(self.operands)


class FuncArgWithAttr(IRValue):
    """
    This derived class is specifically for func op arg with attr
    """

    def __init__(self, ty, attr_name, attr_ty, attr_value=None):
        super().__init__(ty)
        assert attr_name is not None and (
            attr_ty is not None or attr_value is not None
        ), "Invalid attr_name and/or attr_ty and/or attr_value for FuncArgWithAttr"
        self.attr_name = attr_name
        self.attr_ty = attr_ty
        self.attr_value = attr_value


class IRGridConst(FuncArgWithAttr):
    """
    This derived class is specifically for cute_nvgpu grid constant arg
    """

    def __init__(self, ty):
        super().__init__(ty, "cute_nvgpu.grid_constant", "UnitAttr")


# {$nv-internal-release begin}
##############################################################
# TODO: Belong to cuda tile
##############################################################


class VectorMeta(DslType):
    _element_type = Int8
    _shape = (1,)
    _size = 1

    def __new__(cls, name, bases, attrs, element_type=Int32, shape=(1,)):
        size = reduce(operator.mul, shape, 1)
        if size <= 0:
            raise DSLRuntimeError("Vector size must be a positive integer")

        def _mlir_type():
            # MLIR ir.VectorType removed - ace_edsl uses AIR instead
            return None

        new_cls = super().__new__(cls, name, bases, attrs, _mlir_type)
        new_cls._shape = shape
        new_cls._element_type = element_type
        new_cls._size = size
        return new_cls

    def __eq__(cls, other):
        if not isinstance(other, VectorMeta):
            return False
        return cls._element_type == other._element_type and cls._shape == other._shape

    def __hash__(cls):
        return hash((cls._element_type, cls._shape))

    def __getitem__(cls, params) -> Type["Vector"]:
        # Handle both type and shape parameters
        if not isinstance(params, tuple) or len(params) != 2:
            raise DSLRuntimeError(
                "Vector requires both type and shape parameters, e.g., Vector[int, (3,)]"
            )

        element_type, shape = params

        # Validate shape parameter
        if not isinstance(shape, tuple):
            shape = (shape,)

        # Create a new class with both type and shape parameters
        return type(
            f"Vector[{element_type.__name__}, {shape}]",
            (Vector,),
            {
                "__init__": Vector.__init__,
                "__str__": Vector.__str__,
                "__metaclass__": VectorMeta,
            },
            element_type=element_type,
            shape=shape,
        )

    def __str__(self):
        return f"vector<{self._shape} x {self._element_type}>"


class Vector(metaclass=VectorMeta):
    def __init__(self, value):
        if isinstance(value, list):
            # verify if size is smaller than the value
            if len(value) > type(self)._size:
                raise DSLRuntimeError(
                    f"size of {self.__class__} is smaller than the {value}"
                )

            # pad with zeros to match the size
            self._value = value + [0] * (self.__class__._size - len(value))
        else:
            self._value = value

    def __str__(self):
        return f"{self._value} : {self.__class__}"

    def element_type(self) -> DslType:
        return type(self)._element_type

    def shape(self):
        return type(self)._shape


class TensorType:
    """Tensor Type."""

    pass


# {$nv-internal-release end}


__all__ = [
    "DslType",
    "Numeric",
    "Scalar",
    "NumericMeta",
    "IntegerMeta",
    "FloatMeta",
    "Boolean",
    "Integer",
    "Index",
    "Int16",
    "Int32",
    "Int64",
    "Int128",
    "Int8",
    "Uint8",
    "Uint16",
    "Uint32",
    "Uint64",
    "Uint128",
    "Float",
    "Float16",
    "BFloat16",
    "TFloat32",
    "Float32",
    "Float64",
    "Float8E5M2",
    "Float8E4M3",
    "Float8E4M3FN",
    "Float8E4M3B11FNUZ",
    "Float8E3M4",  # {$nv-internal-release}
    "Float8E4M3",
    "Float8E8M0FNU",
    "Float4E2M1FN",
    "Float6E2M3FN",
    "Float6E3M2FN",
    "as_numeric",
    "align",
    "Pointer",
    "dtype",
    "as_value",
    "Constexpr",
    "IRConst",
    "IRValue",
    "IRVariadic",
    "FuncArgWithAttr",
    "IRGridConst",
    # {$nv-internal-release begin}
    "TensorType",
    "Vector",
    # {$nv-internal-release end}
]

class Index(Integer, width=64, mlir_type=None):  # MLIR T removed
    """Index type for MLIR index values (typically 64-bit on most platforms)."""
    
    @property
    def zero(self) -> "Index":
        return Index(0)
    
    def ir_value(self, *, loc=None, ip=None):
        """Override ir_value - MLIR removed, ace_edsl uses AIR instead."""
        # MLIR arith.constant removed - ace_edsl uses AIR instead
        if isinstance(self.value, (int, float, bool)):
            return self.value  # Return Python value directly
        else:
            # Return value directly (should be AIR value in ace_edsl)
            return self.value
    
    def __neg__(self, *, loc=None, ip=None):
        # MLIR arith operations removed - ace_edsl uses AIR instead
        zero = 0
        return Index(zero - self.value, loc=loc, ip=ip)
