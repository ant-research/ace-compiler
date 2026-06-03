#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR Analysis utilities for FHE compilation.

Provides tools to extract and analyze IR structure for:
- Regression testing (detect unexpected changes in compilation pipeline)
- Performance analysis (understand FHE-specific bottlenecks)
- Debugging (inspect model architecture before/after transforms)
- Architecture exploration (compare variants like channel changes)
"""

from typing import Dict, List, Any, Optional
import torch
import torch.nn as nn


def extract_ir_structure(traced_model) -> Dict[str, Any]:
    """Extract detailed IR structure from TorchTracedModel for FHE analysis.

    Captures comprehensive information for detecting architecture changes
    and FHE-specific characteristics.

    Args:
        traced_model: TorchTracedModel instance after FX tracing

    Returns:
        Dictionary containing:
        - input_shape: Input tensor shape (e.g., [1, 3, 32, 32])
        - output_shape: Output tensor shape (e.g., [1, 10])
        - op_counts: Count of each operation type
        - total_ops: Total number of operations
        - conv_layers: List of Conv2d layer details (channels, kernel, stride, etc.)
        - conv_count: Number of Conv layers
        - linear_layers: List of Linear layer details (in/out features)
        - linear_count: Number of Linear layers
        - constant_shapes: Shapes of all constants (weights/biases)
        - total_constants: Total number of constants
        - add_ops: Residual connection details
        - add_count: Number of add operations (residual connections)

    Example:
        >>> from ace.fhe.frontend import get_frontend
        >>> model = resnet20_cifar10()
        >>> frontend = get_frontend("torch")
        >>> traced = frontend.prepare(model, [input])
        >>> structure = extract_ir_structure(traced)
        >>> print(f"First conv: {structure['conv_layers'][0]}")
        {'name': 'conv1', 'in_channels': 3, 'out_channels': 16, ...}

    Use cases:
        1. Regression testing: Detect unexpected architecture changes
        2. Channel exploration: Monitor (3,32,32) -> (2,32,32) changes
        3. FHE analysis: Understand polynomial degree from activation counts
    """
    # Basic shapes
    result = {
        "input_shape": traced_model._input_shapes[0] if traced_model._input_shapes else [],
        "output_shape": traced_model._output_shape,
        "total_constants": len(traced_model._constants),
    }

    # Op counts from FX graph
    fx_op_counts = {}
    for node in traced_model.traced_model.graph.nodes:
        if node.op == 'call_function':
            target = node.target
            # Get a stable name for the op, avoiding memory addresses
            # from builtin functions like <built-in method abs at 0x...>
            if callable(target):
                op_name = target.__name__
            else:
                op_name = str(target).split('.')[-1]
            fx_op_counts[op_name] = fx_op_counts.get(op_name, 0) + 1
        elif node.op == 'call_module':
            module = traced_model.traced_model.get_submodule(node.target)
            mod_name = type(module).__name__
            fx_op_counts[mod_name] = fx_op_counts.get(mod_name, 0) + 1

    result["op_counts"] = fx_op_counts
    result["total_ops"] = sum(fx_op_counts.values())

    # Detailed Conv layer analysis (critical for FHE)
    conv_layers = []
    for name, module in traced_model.traced_model.named_modules():
        if isinstance(module, nn.Conv2d):
            conv_info = {
                "name": name,
                "in_channels": module.in_channels,
                "out_channels": module.out_channels,
                "kernel_size": module.kernel_size[0] if isinstance(module.kernel_size, tuple) else module.kernel_size,
                "stride": module.stride[0] if isinstance(module.stride, tuple) else module.stride,
                "padding": module.padding[0] if isinstance(module.padding, tuple) else module.padding,
                "groups": module.groups,
                "bias": module.bias is not None,
            }
            conv_layers.append(conv_info)
    result["conv_layers"] = conv_layers
    result["conv_count"] = len(conv_layers)

    # Linear/Gemm layer analysis
    linear_layers = []
    for name, module in traced_model.traced_model.named_modules():
        if isinstance(module, nn.Linear):
            linear_info = {
                "name": name,
                "in_features": module.in_features,
                "out_features": module.out_features,
                "bias": module.bias is not None,
            }
            linear_layers.append(linear_info)
    result["linear_layers"] = linear_layers
    result["linear_count"] = len(linear_layers)

    # Constant shapes (weight shapes detect architecture changes)
    constant_shapes = {}
    for name, info in traced_model._constants.items():
        constant_shapes[name] = info.get("shape", [])
    result["constant_shapes"] = constant_shapes

    # Residual connections (add ops count and locations)
    add_ops = []
    for node in traced_model.traced_model.graph.nodes:
        if node.op == 'call_function' and 'add' in str(node.target).lower():
            add_info = {"name": node.name}
            # Try to find which layer this add belongs to
            if hasattr(node, 'args') and len(node.args) >= 2:
                # args are the inputs to the add operation
                input_names = []
                for arg in node.args:
                    if isinstance(arg, torch.fx.Node):
                        input_names.append(arg.name)
                add_info["inputs"] = input_names
            add_ops.append(add_info)
    result["add_ops"] = add_ops
    result["add_count"] = len(add_ops)

    return result


def compare_ir_structures(
    actual: Dict[str, Any],
    expected: Dict[str, Any],
    tolerance: Optional[Dict[str, Any]] = None
) -> tuple[bool, List[str]]:
    """Compare two IR structures and report differences.

    Args:
        actual: Structure from current model
        expected: Baseline structure to compare against
        tolerance: Optional tolerance for numeric comparisons
            e.g., {"in_channels": lambda a, e: abs(a - e) <= 1}

    Returns:
        (is_match, list_of_differences)

    Example:
        >>> match, diffs = compare_ir_structures(actual, baseline)
        >>> if not match:
        ...     print("Differences:", diffs)
    """
    differences = []
    tolerance = tolerance or {}

    # Compare shapes
    if actual.get("input_shape") != expected.get("input_shape"):
        differences.append(
            f"input_shape: got {actual.get('input_shape')}, "
            f"expected {expected.get('input_shape')}"
        )

    if actual.get("output_shape") != expected.get("output_shape"):
        differences.append(
            f"output_shape: got {actual.get('output_shape')}, "
            f"expected {expected.get('output_shape')}"
        )

    # Compare conv layer count
    if actual.get("conv_count") != expected.get("conv_count"):
        differences.append(
            f"conv_count: got {actual.get('conv_count')}, "
            f"expected {expected.get('conv_count')}"
        )

    # Compare conv layer details
    actual_convs = {c["name"]: c for c in actual.get("conv_layers", [])}
    expected_convs = {c["name"]: c for c in expected.get("conv_layers", [])}

    for name, expected_conv in expected_convs.items():
        if name not in actual_convs:
            differences.append(f"Missing conv layer: {name}")
            continue
        actual_conv = actual_convs[name]
        for key in ["in_channels", "out_channels", "kernel_size", "stride", "groups"]:
            if actual_conv.get(key) != expected_conv.get(key):
                differences.append(
                    f"Conv {name}.{key}: got {actual_conv.get(key)}, "
                    f"expected {expected_conv.get(key)}"
                )

    # Compare op counts
    actual_ops = actual.get("op_counts", {})
    expected_ops = expected.get("op_counts", {})
    all_ops = set(actual_ops.keys()) | set(expected_ops.keys())
    for op in all_ops:
        actual_count = actual_ops.get(op, 0)
        expected_count = expected_ops.get(op, 0)
        if actual_count != expected_count:
            differences.append(
                f"Op '{op}' count: got {actual_count}, expected {expected_count}"
            )

    return len(differences) == 0, differences


def summarize_ir_structure(structure: Dict[str, Any]) -> str:
    """Create a human-readable summary of IR structure.

    Useful for debugging and logging.

    Args:
        structure: Result from extract_ir_structure()

    Returns:
        Multi-line string summary
    """
    lines = [
        "IR Structure Summary:",
        f"  Input:  {structure.get('input_shape', 'N/A')}",
        f"  Output: {structure.get('output_shape', 'N/A')}",
        f"  Conv layers: {structure.get('conv_count', 0)}",
        f"  Linear layers: {structure.get('linear_count', 0)}",
        f"  Total ops: {structure.get('total_ops', 0)}",
        f"  Constants: {structure.get('total_constants', 0)}",
        f"  Residual adds: {structure.get('add_count', 0)}",
        "",
        "Conv Layers:",
    ]

    for conv in structure.get("conv_layers", [])[:5]:  # First 5
        lines.append(
            f"  {conv['name']}: {conv['in_channels']} -> {conv['out_channels']}, "
            f"k={conv['kernel_size']}, s={conv['stride']}"
        )
    if structure.get("conv_count", 0) > 5:
        lines.append(f"  ... and {structure['conv_count'] - 5} more")

    return "\n".join(lines)
