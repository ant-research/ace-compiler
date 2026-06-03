#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Convenience methods for IRBuilder — typed wrappers around add_op().

These methods provide a fluent API for adding common NN operations.
They are separated from the core IRBuilder to keep it focused.

Usage:
    from ace.fhe.ir.core.ir_builder import IRBuilder
    # Convenience methods are available via mixin:
    builder = IRBuilder()
    builder.begin_function("Main_graph")
    v0 = builder.conv("x", "weight", stride=[1, 1], padding=[1, 1, 1, 1])
    v1 = builder.relu(v0)

Note: The interpreter mode (Path 2) uses add_op() directly, not these
convenience methods. These are retained for potential future use.
"""

from typing import List, Optional, Dict, Any


class OpHelpersMixin:
    """Mixin providing convenience methods for common NN operations.

    Each method delegates to add_op() with the correct op name, attrs,
    and metadata. Subclasses must implement add_op().
    """

    # =========================================================================
    # Convolution / Linear
    # =========================================================================

    def conv(self, x: str, weight: str, bias: Optional[str] = None,
             stride: Optional[List[int]] = None,
             padding: Optional[List[int]] = None,
             dilation: Optional[List[int]] = None,
             groups: int = 1,
             onnx_name: Optional[str] = None,
             output_shape: Optional[List[int]] = None) -> str:
        """Add a convolution operation."""
        if stride is None:
            stride = [1, 1]
        if padding is None:
            padding = [0, 0, 0, 0]
        if dilation is None:
            dilation = [1, 1]

        inputs = [x, weight]
        if bias is not None:
            inputs.append(bias)

        attrs = {
            "strides": stride,
            "pads": padding,
            "dilations": dilation,
            "group": groups,
        }

        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name

        return self.add_op("conv", inputs, attrs, metadata, output_shape or [])

    def gemm(self, a: str, b: str, c: Optional[str] = None,
             alpha: float = 1.0, beta: float = 1.0,
             trans_a: int = 0, trans_b: int = 1,
             onnx_name: Optional[str] = None,
             output_shape: Optional[List[int]] = None) -> str:
        """Add a GEMM (general matrix multiplication) operation."""
        inputs = [a, b]
        if c is not None:
            inputs.append(c)

        attrs = {
            "alpha": alpha,
            "beta": beta,
            "transA": trans_a,
            "transB": trans_b,
        }

        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name

        return self.add_op("gemm", inputs, attrs, metadata, output_shape or [])

    def matmul(self, a: str, b: str,
               onnx_name: Optional[str] = None,
               output_shape: Optional[List[int]] = None) -> str:
        """Add a matrix multiplication operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("matmul", [a, b], {}, metadata, output_shape or [])

    # =========================================================================
    # Binary element-wise
    # =========================================================================

    def add(self, a: str, b: str,
            onnx_name: Optional[str] = None,
            output_shape: Optional[List[int]] = None) -> str:
        """Add an element-wise addition operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("add", [a, b], {}, metadata, output_shape or [])

    def sub(self, a: str, b: str,
            onnx_name: Optional[str] = None,
            output_shape: Optional[List[int]] = None) -> str:
        """Add an element-wise subtraction operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("sub", [a, b], {}, metadata, output_shape or [])

    def mul(self, a: str, b: str,
            onnx_name: Optional[str] = None,
            output_shape: Optional[List[int]] = None) -> str:
        """Add an element-wise multiplication operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("mul", [a, b], {}, metadata, output_shape or [])

    def div(self, a: str, b: str,
            onnx_name: Optional[str] = None,
            output_shape: Optional[List[int]] = None) -> str:
        """Add an element-wise division operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("div", [a, b], {}, metadata, output_shape or [])

    # =========================================================================
    # Unary element-wise
    # =========================================================================

    def relu(self, x: str,
             onnx_name: Optional[str] = None,
             output_shape: Optional[List[int]] = None) -> str:
        """Add a ReLU activation operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("relu", [x], {}, metadata, output_shape or [])

    def silu(self, x: str,
             onnx_name: Optional[str] = None,
             output_shape: Optional[List[int]] = None) -> str:
        """Add a SiLU (Swish) activation operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("silu", [x], {}, metadata, output_shape or [])

    def softmax(self, x: str, axis: int = -1,
                onnx_name: Optional[str] = None,
                output_shape: Optional[List[int]] = None) -> str:
        """Add a softmax operation."""
        attrs = {"axis": axis}
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("softmax", [x], attrs, metadata, output_shape or [])

    def sqrt(self, x: str,
             onnx_name: Optional[str] = None,
             output_shape: Optional[List[int]] = None) -> str:
        """Add a square root operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("sqrt", [x], {}, metadata, output_shape or [])

    # =========================================================================
    # Pooling
    # =========================================================================

    def max_pool(self, x: str,
                 kernel_size: List[int],
                 stride: Optional[List[int]] = None,
                 padding: Optional[List[int]] = None,
                 onnx_name: Optional[str] = None,
                 output_shape: Optional[List[int]] = None) -> str:
        """Add a max pooling operation."""
        if stride is None:
            stride = kernel_size
        if padding is None:
            padding = [0, 0, 0, 0]

        attrs = {
            "kernel_shape": kernel_size,
            "strides": stride,
            "pads": padding,
        }

        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name

        return self.add_op("max_pool", [x], attrs, metadata, output_shape or [])

    def average_pool(self, x: str,
                     kernel_size: List[int],
                     stride: Optional[List[int]] = None,
                     padding: Optional[List[int]] = None,
                     onnx_name: Optional[str] = None,
                     output_shape: Optional[List[int]] = None) -> str:
        """Add an average pooling operation."""
        if stride is None:
            stride = kernel_size
        if padding is None:
            padding = [0, 0, 0, 0]

        attrs = {
            "kernel_shape": kernel_size,
            "strides": stride,
            "pads": padding,
        }

        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name

        return self.add_op("average_pool", [x], attrs, metadata, output_shape or [])

    def global_average_pool(self, x: str,
                            onnx_name: Optional[str] = None,
                            output_shape: Optional[List[int]] = None) -> str:
        """Add a global average pooling operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("global_average_pool", [x], {}, metadata, output_shape or [])

    # =========================================================================
    # Shape manipulation
    # =========================================================================

    def flatten(self, x: str, start_dim: int = 0, end_dim: int = -1,
                onnx_name: Optional[str] = None,
                output_shape: Optional[List[int]] = None) -> str:
        """Add a flatten operation."""
        attrs = {"start_dim": start_dim, "end_dim": end_dim}
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("flatten", [x], attrs, metadata, output_shape or [])

    def reshape(self, x: str, shape: str,
                onnx_name: Optional[str] = None,
                output_shape: Optional[List[int]] = None) -> str:
        """Add a reshape operation."""
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("reshape", [x, shape], {}, metadata, output_shape or [])

    def concat(self, inputs: List[str], axis: int,
               onnx_name: Optional[str] = None,
               output_shape: Optional[List[int]] = None) -> str:
        """Add a concatenation operation."""
        attrs = {"axis": axis}
        metadata = {}
        if onnx_name:
            metadata["onnx_name"] = onnx_name
        return self.add_op("concat", inputs, attrs, metadata, output_shape or [])