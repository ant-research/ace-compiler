#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IRBuilder - Python wrapper for AIR IR generation.

This class wraps the C++ Frontend singleton and provides:
1. A Pythonic interface for building AIR IR
2. Type-safe convenience methods for common operations
3. Method chaining for fluent API usage
"""

from typing import List, Optional, Dict, Any, Union
from dataclasses import dataclass

from .op_helpers import OpHelpersMixin

# Check if C++ frontend extension is available
try:
    import ace.frontend as _frontend
    HAS_FRONTEND = _frontend is not None
except ImportError:
    HAS_FRONTEND = False
    _frontend = None


@dataclass
class TensorInfo:
    """Information about a tensor in the IR."""
    name: str
    shape: List[int]
    dtype: str = "float32"


class IRBuilder(OpHelpersMixin):
    """
    Python IRBuilder for AIR IR generation.

    This class wraps the C++ Frontend singleton and provides
    a more Pythonic interface for building AIR IR.

    Example usage:
        builder = IRBuilder()
        builder.begin_function("Main_graph") \\
              .add_input("x", [1, 3, 32, 32]) \\
              .add_constant("weight", [16, 3, 3, 3], weight_data) \\
              .end_function([1, 16, 30, 30])

        v0 = builder.conv("x", "weight", stride=[1, 1], padding=[1, 1, 1, 1])
        v1 = builder.relu(v0)

        builder.finalize().write_ir("model.B")

    Attributes:
        _frontend: The C++ Frontend singleton instance
        _inputs: List of (name, shape) tuples for input parameters
        _constants: Dict of constant name -> TensorInfo
        _output_shape: Output tensor shape
        _result_counter: Counter for generating result variable names
    """

    def __init__(self):
        """Initialize the IRBuilder."""
        if not HAS_FRONTEND:
            raise RuntimeError("C++ extension not available")

        self._frontend = _frontend.Frontend.get_instance()
        self._inputs: List[tuple] = []
        self._constants: Dict[str, TensorInfo] = {}
        self._output_shape: Optional[List[int]] = None
        self._result_counter: int = 0
        self._is_building: bool = False
        self._function_name: Optional[str] = None

    # =========================================================================
    # Function-level operations
    # =========================================================================

    def begin_function(self, name: str) -> 'IRBuilder':
        """
        Begin building a new AIR function.

        Args:
            name: Function name (entry point name)

        Returns:
            self for method chaining
        """
        self._frontend.begin_function(name)
        self._function_name = name
        self._is_building = True
        self._inputs = []
        self._constants = {}
        self._result_counter = 0
        return self

    def add_input(self, name: str, shape: List[int]) -> 'IRBuilder':
        """
        Add an input parameter to the current function.

        Args:
            name: Input parameter name
            shape: Input tensor shape

        Returns:
            self for method chaining
        """
        self._frontend.add_input(name, shape)
        self._inputs.append((name, shape))
        return self

    def add_constant(self, name: str, shape: List[int],
                    data: Union[List[float], List[int]],
                    dtype: str = "float32",
                    tensor: Optional[Any] = None) -> 'IRBuilder':
        """
        Add a constant tensor to the current function.

        Args:
            name: Constant name
            shape: Constant tensor shape
             Flattened tensor data
            dtype: Data type ("float32" or "int64")
            tensor: Optional torch.Tensor for data_ptr→name registration (direct mode)

        Returns:
            self for method chaining
        """
        if dtype == "int64":
            self._frontend.add_constant_int64(name, shape, data)
        else:
            self._frontend.add_constant(name, shape, data)

        self._constants[name] = TensorInfo(name, shape, dtype)
        return self

    def end_function(self, output_shape: List[int]) -> 'IRBuilder':
        """
        End the current function definition.

        Args:
            output_shape: Output tensor shape

        Returns:
            self for method chaining
        """
        self._frontend.end_function(output_shape)
        self._output_shape = output_shape
        return self

    def finalize(self) -> 'IRBuilder':
        """
        Finalize and complete the AIR function.

        Returns:
            self for method chaining
        """
        self._frontend.finalize()
        self._is_building = False
        return self

    # =========================================================================
    # Operation-level operations
    # =========================================================================

    def add_op(self, op_name: str,
               inputs: List[str],
               attrs: Optional[Dict[str, Any]] = None,
               meta: Optional[Dict[str, str]] = None,
               output_shape: Optional[List[int]] = None) -> str:
        """
        Add an operation to the current function.

        Args:
            op_name: Operation name (e.g., "conv", "relu", "add")
            inputs: List of input tensor names
            attrs: Operation attributes
            meta Operation metadata (e.g., {"onnx_name": "/conv1/Conv"})
            output_shape: Output tensor shape (optional)

        Returns:
            Result variable name (e.g., "_v0", "_v1")
        """
        if attrs is None:
            attrs = {}
        if meta is None:
            meta = {}
        if output_shape is None:
            output_shape = []

        result = self._frontend.add_operation(op_name, inputs, attrs, meta, output_shape)
        self._result_counter += 1
        return result

    # =========================================================================
    # Output operations
    # =========================================================================

    def write_ir(self, filename: str, phase: str = "ONNX2AIR") -> 'IRBuilder':
        """
        Write the generated AIR IR to a file.

        Args:
            filename: Output file path (e.g., "model.B")
            phase: Phase name (default "ONNX2AIR")

        Returns:
            self for method chaining
        """
        self._frontend.write_ir(filename, phase)
        return self

    def print_ir(self) -> 'IRBuilder':
        """
        Print the generated AIR IR to stdout.

        Returns:
            self for method chaining
        """
        self._frontend.print_ir()
        return self

    # =========================================================================
    # State queries
    # =========================================================================

    def is_building(self) -> bool:
        """Check if AIR function is currently being built."""
        return self._frontend.is_building()

    def get_output_shape(self) -> List[int]:
        """Get the output shape of the current function."""
        return self._frontend.get_output_shape()

    def get_func_scope(self) -> int:
        """Get the current function scope as opaque handle."""
        return self._frontend.get_func_scope()

    def get_glob_scope(self) -> int:
        """Get the global scope as opaque handle."""
        return self._frontend.get_glob_scope()

    def get_level(self) -> str:
        """Get the current level name."""
        return self._frontend.get_level()

    def set_level(self, level_name: str) -> 'IRBuilder':
        """
        Set the current level.

        Args:
            level_name: Level name ("tensor", "vector", "ckks", "sihe", "poly")

        Returns:
            self for method chaining
        """
        self._frontend.set_level(level_name)
        return self

    # =========================================================================
    # Utility methods
    # =========================================================================

    @property
    def input_names(self) -> List[str]:
        """Get the list of input tensor names."""
        return [name for name, _ in self._inputs]

    @property
    def input_shapes(self) -> List[List[int]]:
        """Get the list of input tensor shapes."""
        return [list(shape) for _, shape in self._inputs]

    @property
    def constants(self) -> Dict[str, TensorInfo]:
        """Get the dictionary of constants."""
        return self._constants.copy()

    @property
    def function_name(self) -> Optional[str]:
        """Get the current function name."""
        return self._function_name

    @staticmethod
    def print_bfile_ir(filename: str) -> str:
        """
        Read a .B file and return AIR IR text.

        Args:
            filename: .B file path

        Returns:
            AIR IR text string
        """
        return _frontend.print_bfile_ir(filename)

    @staticmethod
    def is_available() -> bool:
        """Check if the C++ extension is available."""
        return HAS_FRONTEND

    # =========================================================================
    # Tensor name registry (for Path 1 / direct mode)
    # =========================================================================

    def register_tensor_name(self, data_ptr: int, name: str) -> None:
        """
        Register a data_ptr to name mapping for direct mode.

        Args:
            data_ptr: Tensor data pointer (from tensor.data_ptr())
            name: AIR IR name for the tensor
        """
        self._frontend.register_tensor_name(data_ptr, name)

    def clear_tensor_names(self) -> None:
        """Clear all data_ptr to name mappings."""
        self._frontend.clear_tensor_names()