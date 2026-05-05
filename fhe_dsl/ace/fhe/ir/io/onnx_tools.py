# -*- coding: utf-8 -*-
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ONNX tools for exporting PyTorch models and converting to AIR IR.

This module provides utilities for:
- Exporting PyTorch models/functions to ONNX format
- Converting ONNX models to AIR IR
- Validating and inspecting ONNX models

Note: Conversion functions are in frontends/onnx/onnx_converter.py
Export functions for FHEProgram are in export/onnx_export.py
"""

import logging
import onnx
import torch
import torch.nn as nn
from pathlib import Path
from typing import Union, Optional, List, Tuple, Any, Dict, TYPE_CHECKING

# Get logger
logger = logging.getLogger(__name__)

# Import from new locations for backward compatibility
from ..frontends.onnx.onnx_converter import (
    convert_onnx_to_fhe_program,
    convert_onnx_to_air_binary,
)

if TYPE_CHECKING:
    from ..representations.fhe_program import FHEProgram


# =============================================================================
# Exporter: PyTorch -> ONNX
# =============================================================================

def export_model_to_onnx(
    model: nn.Module,
    example_inputs: Union[torch.Tensor, Tuple[torch.Tensor, ...]],
    output_path: Union[str, Path],
    input_names: Optional[List[str]] = None,
    output_names: Optional[List[str]] = None,
    dynamic_axes: Optional[dict] = None,
    opset_version: int = 13,
    export_params: bool = True,
    do_constant_folding: bool = True,
    verbose: bool = False,
) -> Path:
    """
    Export a PyTorch nn.Module to ONNX format.

    Args:
        model: PyTorch model to export
        example_inputs: Example inputs for tracing
        output_path: Output file path
        input_names: Names for inputs (default: input_0, input_1, ...)
        output_names: Names for outputs (default: output)
        dynamic_axes: Dynamic axes specification
        opset_version: ONNX opset version (default: 13)
        export_params: Whether to export parameters (default: True)
        do_constant_folding: Enable constant folding (default: True)
        verbose: Verbose output (default: False)

    Returns:
        Absolute path to the exported ONNX file.
    """
    if not isinstance(example_inputs, (tuple, list)):
        example_inputs = (example_inputs,)
    else:
        example_inputs = tuple(example_inputs)

    if input_names is None:
        input_names = [f"input_{i}" for i in range(len(example_inputs))]
    else:
        assert len(input_names) == len(example_inputs), \
            f"input_names length ({len(input_names)}) != number of inputs ({len(example_inputs)})"

    output_names = output_names or ["output"]
    model.eval()

    # Temporarily disable requires_grad for all parameters
    original_requires_grad = {}
    for name, param in model.named_parameters():
        original_requires_grad[name] = param.requires_grad
        param.requires_grad = False

    output_path = Path(output_path).resolve()
    try:
        with torch.no_grad():
            torch.onnx.export(
                model=model,
                args=example_inputs,
                f=str(output_path),
                export_params=export_params,
                opset_version=opset_version,
                do_constant_folding=do_constant_folding,
                input_names=input_names,
                output_names=output_names,
                dynamic_axes=dynamic_axes,
                verbose=verbose,
                dynamo=False,
            )
    finally:
        # Restore requires_grad
        for name, param in model.named_parameters():
            if name in original_requires_grad:
                param.requires_grad = original_requires_grad[name]

    return output_path


def export_function_to_onnx(
    func: callable,
    example_inputs: Union[torch.Tensor, Tuple[torch.Tensor, ...]],
    output_path: Union[str, Path],
    input_names: Optional[List[str]] = None,
    output_names: Optional[List[str]] = None,
    dynamic_axes: Optional[dict] = None,
    opset_version: int = 13,
    export_params: bool = True,
    do_constant_folding: bool = True,
    verbose: bool = False,
) -> Path:
    """
    Export a standalone PyTorch function to ONNX by wrapping it in a Module.

    Args:
        func: Python function to export
        example_inputs: Example inputs for tracing
        output_path: Output file path
        input_names: Names for inputs
        output_names: Names for outputs
        dynamic_axes: Dynamic axes specification
        opset_version: ONNX opset version
        export_params: Whether to export parameters
        do_constant_folding: Enable constant folding
        verbose: Verbose output

    Returns:
        Absolute path to the exported ONNX file.
    """
    class FunctionWrapper(nn.Module):
        def __init__(self, fn):
            super().__init__()
            self.fn = fn

        def forward(self, *args):
            return self.fn(*args)

    wrapped_model = FunctionWrapper(func)

    # If the function wraps a model, disable gradients on the original model's parameters
    original_model = getattr(func, '_original_model', None)
    if original_model is not None:
        original_requires_grad = {}
        for name, param in original_model.named_parameters():
            original_requires_grad[name] = param.requires_grad
            param.requires_grad = False

    try:
        return export_model_to_onnx(
            model=wrapped_model,
            example_inputs=example_inputs,
            output_path=output_path,
            input_names=input_names,
            output_names=output_names,
            dynamic_axes=dynamic_axes,
            opset_version=opset_version,
            export_params=export_params,
            do_constant_folding=do_constant_folding,
            verbose=verbose,
        )
    finally:
        # Restore requires_grad on original model
        if original_model is not None:
            for name, param in original_model.named_parameters():
                if name in original_requires_grad:
                    param.requires_grad = original_requires_grad[name]


# =============================================================================
# Converter: ONNX -> AIR (backward compatibility wrapper)
# =============================================================================

def convert_onnx_to_air(
    onnx_path: Union[str, Path],
    output_path: Optional[Union[str, Path]] = None
) -> Union["FHEProgram", str]:
    """
    Convert an ONNX model to AIR IR.

    Args:
        onnx_path: Path to ONNX model file
        output_path: Optional output path for AIR .B file.
                     If provided, uses fhe_cmplr to generate binary AIR file.
                     If None, returns FHEProgram with Python IR representation.

    Returns:
        If output_path is None: FHEProgram instance containing a main graph.
        If output_path is provided: Path to the generated .B file.
    """
    onnx_path = str(onnx_path)

    # If output_path is provided, use fhe_cmplr to generate AIR binary
    if output_path is not None:
        return convert_onnx_to_air_binary(onnx_path, str(output_path))

    # Otherwise, return Python FHEProgram representation
    return convert_onnx_to_fhe_program(onnx_path)


# =============================================================================
# Utils: ONNX validation and inspection
# =============================================================================

def validate_onnx_model(onnx_path: Union[str, Path]) -> None:
    """
    Validate an ONNX model file.

    Args:
        onnx_path: Path to ONNX model file

    Raises:
        onnx.onnx_cpp2py_export.checker.ValidationError: If model is invalid.
    """
    model = onnx.load(str(onnx_path))
    onnx.checker.check_model(model)


def inspect_onnx_model(
    onnx_path: Union[str, Path],
    verbose: bool = True
) -> Dict[str, Any]:
    """
    Load and inspect an ONNX model, returning structured info.

    Args:
        onnx_path: Path to .onnx file.
        verbose: If True, print human-readable summary.

    Returns:
        Dictionary containing IR version, opsets, inputs, outputs, and nodes.
    """
    validate_onnx_model(onnx_path)
    model = onnx.load(str(onnx_path))
    graph = model.graph

    def _get_shape_str(tensor_type):
        dims = []
        for dim in tensor_type.shape.dim:
            if dim.HasField("dim_value"):
                dims.append(dim.dim_value)
            elif dim.HasField("dim_param"):
                dims.append(dim.dim_param)
            else:
                dims.append(None)
        return dims

    def _dtype_to_str(elem_type):
        return str(onnx.mapping.TENSOR_TYPE_TO_NP_TYPE.get(elem_type, "unknown"))

    inputs = [
        {
            "name": inp.name,
            "shape": _get_shape_str(inp.type.tensor_type),
            "dtype": _dtype_to_str(inp.type.tensor_type.elem_type)
        }
        for inp in graph.input
    ]

    outputs = [
        {
            "name": out.name,
            "shape": _get_shape_str(out.type.tensor_type),
            "dtype": _dtype_to_str(out.type.tensor_type.elem_type)
        }
        for out in graph.output
    ]

    nodes = []
    for i, node in enumerate(graph.node):
        attrs = {}
        for attr in node.attribute:
            if attr.type == onnx.AttributeProto.FLOAT:
                val = attr.f
            elif attr.type == onnx.AttributeProto.INT:
                val = attr.i
            elif attr.type == onnx.AttributeProto.STRING:
                val = attr.s.decode('utf-8')
            elif attr.type == onnx.AttributeProto.TENSOR:
                val = onnx.numpy_helper.to_array(attr.t).tolist()
            else:
                val = str(attr)
            attrs[attr.name] = val

        nodes.append({
            "index": i,
            "op_type": node.op_type,
            "inputs": list(node.input),
            "outputs": list(node.output),
            "attributes": attrs
        })

    opsets = [
        f"{imp.domain if imp.domain else 'ai.onnx'}:{imp.version}"
        for imp in model.opset_import
    ]

    info = {
        "ir_version": model.ir_version,
        "opsets": opsets,
        "inputs": inputs,
        "outputs": outputs,
        "nodes": nodes
    }

    if verbose:
        logger.info("Model '%s' is valid!", onnx_path)
        logger.info("IR Version: %s", info['ir_version'])
        logger.info("Opsets: %s", info['opsets'])

        logger.info("Inputs:")
        for inp in info["inputs"]:
            logger.info("  - %s: shape=%s, dtype=%s", inp['name'], inp['shape'], inp['dtype'])

        logger.info("Outputs:")
        for out in info["outputs"]:
            logger.info("  - %s: shape=%s, dtype=%s", out['name'], out['shape'], out['dtype'])

        logger.info("Nodes (%d total):", len(info['nodes']))
        for node in info["nodes"]:
            logger.info("  [%2d] %s (inputs=%s, outputs=%s)",
                  node['index'], node['op_type'], node['inputs'], node['outputs'])

    return info