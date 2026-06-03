#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Graph Transform Pass for Torch Frontend.

This pass transforms the FX graph to use custom tensor operations
(torch.ops.tensor.xxx) that generate AIR IR when executed.

Key Transformations:
    1. Remove Identity nodes (folded BatchNorm)
    2. Replace torch functions with custom ops
    3. Replace nn.Module calls with custom ops
    4. Handle argument normalization (remove kwargs like inplace)
    5. Generate ONNX-style node names for debugging

Design Document: Graph Rewrite Strategy
=========================================

Problem:
--------
Standard PyTorch FX graphs use generic operations that don't map
directly to FHE-friendly AIR IR. We need to:
1. Replace ops with FHE-compatible variants
2. Normalize arguments (remove unsupported kwargs)
3. Add metadata for IR generation

Solution:
---------
Two-pass graph rewrite:
1. Remove Identity nodes (from folded BN)
2. Replace call_function/call_module with custom ops

Custom Op Mapping:
    - torch.add -> torch.ops.tensor.add
    - F.relu -> torch.ops.tensor.relu
    - nn.Conv2d -> torch.ops.tensor.conv
    - nn.Linear -> torch.ops.tensor.gemm
    - etc.

Node Naming Convention:
    Format: /parent/child/module/OpType
    Example: /layer1/layer1.0/conv1/Conv

Usage:
    from .graph_transform import GraphTransformPass

    pass_instance = GraphTransformPass()
    transformed_model = pass_instance.apply(traced_model)
"""

import torch
import torch.nn as nn
import torch.nn.functional as F
import re
from typing import Optional

from ..torch_ops import get_custom_op, get_custom_op_for_module, get_module_op_name, get_op_name


class GraphTransformPass:
    """
    Graph transformation pass for FHE compilation.

    Transforms FX graph to use custom tensor operations that generate
    AIR IR when executed.
    """

    def __init__(self):
        """Initialize the graph transform pass."""
        self.rewritten_count = 0

    def apply(self, traced_model: torch.fx.GraphModule) -> torch.fx.GraphModule:
        """
        Apply graph transformations.

        Args:
            traced_model: FX traced model

        Returns:
            Transformed model with custom ops
        """
        self.rewritten_count = 0

        # Pass 1: Remove Identity nodes (folded BatchNorm)
        self._remove_identity_nodes(traced_model)

        # Pass 2: Rewrite to custom ops
        self._rewrite_to_custom_ops(traced_model)

        if self.rewritten_count > 0:
            traced_model.recompile()

        return traced_model

    def _remove_identity_nodes(self, traced_model: torch.fx.GraphModule) -> None:
        """
        Remove Identity module calls from FX graph.

        After BN folding, BatchNorm modules are replaced with nn.Identity.
        FX still traces these, so we need to remove them from the graph.

        Args:
            traced_model: FX traced model
        """
        graph = traced_model.graph
        nodes_to_remove = []

        for node in graph.nodes:
            if node.op == 'call_module':
                try:
                    module = traced_model.get_submodule(node.target)
                    if isinstance(module, nn.Identity):
                        nodes_to_remove.append(node)
                except (AttributeError, KeyError):
                    pass

        # Remove identity nodes and reconnect graph
        for node in nodes_to_remove:
            if node.args:
                replacement = node.args[0]
            else:
                continue

            node.replace_all_uses_with(replacement)
            graph.erase_node(node)

    def _rewrite_to_custom_ops(self, traced_model: torch.fx.GraphModule) -> None:
        """
        Rewrite FX graph to use custom tensor ops.

        Args:
            traced_model: FX traced model
        """
        graph = traced_model.graph

        for node in graph.nodes:
            if node.op == 'call_function':
                self._handle_call_function(node, graph)
            elif node.op == 'call_module':
                self._handle_call_module(node, graph, traced_model)
            elif node.op == 'call_method':
                self._handle_call_method(node, graph, traced_model)

    def _handle_call_function(self, node: torch.fx.Node, graph: torch.fx.Graph) -> None:
        """
        Handle call_function nodes.

        Args:
            node: FX node
            graph: FX graph
        """
        custom_op = get_custom_op(node.target)
        if custom_op is not None:
            op_name = get_op_name(node.target)
            if op_name:
                onnx_node_name = self._generate_onnx_name(node, op_name)
                node.meta['onnx_name'] = onnx_node_name

            node.target = custom_op
            node.args = self._filter_tensor_args(node.args)
            node.kwargs = {}
            self.rewritten_count += 1

        elif node.target == torch.add or (hasattr(node.target, '__name__') and node.target.__name__ == 'add'):
            # Handle built-in add function (e.g., residual connections)
            onnx_node_name = f"{node.name}_Add"
            node.meta['onnx_name'] = onnx_node_name

            node.target = torch.ops.tensor.add
            node.op = 'call_function'
            node.args = self._filter_tensor_args(node.args)
            node.kwargs = {}
            self.rewritten_count += 1

    def _handle_call_module(self, node: torch.fx.Node, graph: torch.fx.Graph, traced_model: torch.fx.GraphModule) -> None:
        """
        Handle call_module nodes (nn.Module instances).

        Args:
            node: FX node
            graph: FX graph
            traced_model: Traced model
        """
        module = traced_model.get_submodule(node.target)
        module_type = type(module)

        if isinstance(module, nn.Identity):
            return

        op_name = get_module_op_name(module)

        if op_name is not None:
            original_module_target = str(node.target)
            onnx_node_name = self._generate_module_onnx_name(node, original_module_target, op_name)
            node.meta['onnx_name'] = onnx_node_name

            custom_op = get_custom_op_for_module(module)
            if custom_op is not None:
                node.target = custom_op
                node.op = 'call_function'

                if op_name == 'average_pool':
                    self._handle_pool_node(node, module, 'kernel_size', 'stride', 'padding')
                elif op_name == 'max_pool':
                    self._handle_pool_node(node, module, 'kernel_size', 'stride', 'padding')
                elif op_name == 'gemm':
                    self._handle_gemm_node(node, module, traced_model, original_module_target, graph)
                elif op_name == 'conv':
                    self._handle_conv_node(node, module, traced_model, original_module_target, graph)
                else:
                    node.args = self._filter_tensor_args(node.args)
                    node.kwargs = {}

                self.rewritten_count += 1

    def _handle_call_method(self, node: torch.fx.Node, graph: torch.fx.Graph, traced_model: torch.fx.GraphModule) -> None:
        """
        Handle call_method nodes (tensor methods).

        Args:
            node: FX node
            graph: FX graph
            traced_model: Traced model
        """
        method_name = node.target
        if method_name in ('reshape', 'view'):
            self._handle_reshape_node(node, traced_model, graph, method_name)
        elif method_name == 'size':
            node.meta['skip_execution'] = True

    def _filter_tensor_args(self, args) -> tuple:
        """
        Filter arguments to keep only tensor-producing nodes and scalar constants.

        Args:
            args: Original arguments tuple

        Returns:
            Filtered tuple of arguments
        """
        new_args = []
        for arg in args:
            if isinstance(arg, torch.fx.Node):
                new_args.append(arg)
            elif isinstance(arg, torch.Tensor):
                new_args.append(arg)
            elif isinstance(arg, (int, float)):
                new_args.append(arg)
        return tuple(new_args)

    def _generate_onnx_name(self, node: torch.fx.Node, op_name: str) -> str:
        """Generate AIR node name from FX node name.

        Uses FX node name directly (underscore format) without conversion.
        Format: {node_name}_{OpType}

        Example: "layer1_0_relu" + "tensor.relu" -> "layer1_0_relu_Relu"

        Args:
            node: FX node
            op_name: Operation type name (e.g. "tensor.relu", "conv")

        Returns:
            AIR node name
        """
        op_type = op_name.title().replace("_", "")
        return f"{node.name}_{op_type}"

    def _generate_module_onnx_name(self, node: torch.fx.Node, module_target: str, op_name: str) -> str:
        """Generate AIR node name for nn.Module calls.

        Uses FX node name directly (underscore format) without conversion.
        Format: {node_name}_{OpType}

        Example: node.name="layer1_0_conv1" + op_name="conv" -> "layer1_0_conv1_Conv"

        Args:
            node: FX node
            module_target: Module target string (unused, kept for API compat)
            op_name: Operation type name

        Returns:
            AIR node name
        """
        op_type = op_name.title().replace("_", "")
        return f"{node.name}_{op_type}"

    def _normalize_padding(self, padding, ndim: int = 2) -> list:
        """
        Normalize padding to ONNX format.

        ONNX requires pads=[pad_top, pad_left, pad_bottom, pad_right]

        Args:
            padding: Original padding
            ndim: Number of dimensions

        Returns:
            Normalized padding list
        """
        if isinstance(padding, int):
            padding = [padding] * ndim

        if len(padding) == 1:
            padding = [padding[0]] * (ndim * 2)
        elif len(padding) == ndim:
            padding = [padding[0], padding[1], padding[0], padding[1]]

        return padding

    def _handle_pool_node(self, node: torch.fx.Node, module, kernel_attr: str, stride_attr: str, padding_attr: str) -> None:
        """
        Handle pooling operation nodes.

        Args:
            node: FX node
            module: Pool module
            kernel_attr: Attribute name for kernel_size
            stride_attr: Attribute name for stride
            padding_attr: Attribute name for padding
        """
        kernel_size = getattr(module, kernel_attr)
        stride = getattr(module, stride_attr)
        padding = getattr(module, padding_attr)

        if isinstance(kernel_size, int):
            kernel_size = [kernel_size, kernel_size]
        elif isinstance(kernel_size, (tuple, list)):
            kernel_size = list(kernel_size)

        if stride is None:
            stride = kernel_size
        elif isinstance(stride, int):
            stride = [stride, stride]
        elif isinstance(stride, (tuple, list)):
            stride = list(stride)

        padding = self._normalize_padding(padding)

        node.args = self._filter_tensor_args(node.args)
        node.kwargs = {
            'kernel_size': kernel_size,
            'stride': stride,
            'padding': padding
        }

    def _handle_gemm_node(self, node: torch.fx.Node, module, traced_model, attr_prefix: str, graph: torch.fx.Graph) -> None:
        """
        Handle GEMM (Linear) operation nodes.

        Args:
            node: FX node
            module: Linear module
            traced_model: Traced model
            attr_prefix: Attribute prefix for weight/bias
            graph: FX graph
        """
        safe_prefix = attr_prefix.replace('.', '_')

        setattr(traced_model, f'{safe_prefix}_weight', module.weight)
        if module.bias is not None:
            setattr(traced_model, f'{safe_prefix}_bias', module.bias)

        with graph.inserting_before(node):
            weight_node = graph.create_node('get_attr', f'{safe_prefix}_weight', kwargs={})
            bias_node = graph.create_node('get_attr', f'{safe_prefix}_bias', kwargs={}) if module.bias is not None else None

        new_args = list(node.args) + [weight_node]
        if bias_node is not None:
            new_args.append(bias_node)

        node.args = tuple(new_args)
        node.kwargs = {
            'alpha': 1.0,
            'beta': 1.0,
            'transA': 0,
            'transB': 1
        }

    def _handle_conv_node(self, node: torch.fx.Node, module, traced_model, attr_prefix: str, graph: torch.fx.Graph) -> None:
        """
        Handle Conv2d operation nodes.

        Args:
            node: FX node
            module: Conv2d module
            traced_model: Traced model
            attr_prefix: Attribute prefix for weight/bias
            graph: FX graph
        """
        safe_prefix = attr_prefix.replace('.', '_')

        setattr(traced_model, f'{safe_prefix}_weight', module.weight)
        if module.bias is not None:
            setattr(traced_model, f'{safe_prefix}_bias', module.bias)

        with graph.inserting_before(node):
            weight_node = graph.create_node('get_attr', f'{safe_prefix}_weight', kwargs={})
            bias_node = graph.create_node('get_attr', f'{safe_prefix}_bias', kwargs={}) if module.bias is not None else None

        new_args = list(node.args) + [weight_node]
        if bias_node is not None:
            new_args.append(bias_node)

        padding = self._normalize_padding(list(module.padding))

        node.args = tuple(new_args)
        node.kwargs = {
            'kernel_size': list(module.kernel_size),
            'stride': list(module.stride),
            'padding': padding,
            'dilation': list(module.dilation),
            'groups': module.groups
        }

    def _handle_reshape_node(self, node: torch.fx.Node, traced_model, graph: torch.fx.Graph, method_name: str) -> None:
        """
        Handle reshape/view operation nodes.

        Args:
            node: FX node
            traced_model: Traced model
            graph: FX graph
            method_name: Method name ('view' or 'reshape')
        """
        onnx_node_name = f"{node.name}_Reshape"
        node.meta['onnx_name'] = onnx_node_name

        node.target = torch.ops.tensor.reshape
        node.op = 'call_function'

        input_tensor_node = None
        for arg in node.args:
            if isinstance(arg, (torch.fx.Node, torch.Tensor)):
                input_tensor_node = arg
                break

        shape_const_name = f"_{method_name}_shape_{node.name}"
        shape_tensor = torch.tensor([0, -1], dtype=torch.int64)
        setattr(traced_model, shape_const_name, shape_tensor)

        with graph.inserting_before(node):
            shape_node = graph.create_node('get_attr', shape_const_name, kwargs={})

        node.args = (input_tensor_node, shape_node) if input_tensor_node else (shape_node,)
        node.kwargs = {}


# Convenience function for direct usage
def rewrite_graph_to_custom_ops(traced_model: torch.fx.GraphModule) -> torch.fx.GraphModule:
    """
    Rewrite FX graph to use custom tensor ops.

    Convenience function that applies GraphTransformPass.

    Args:
        traced_model: FX traced model

    Returns:
        Transformed model
    """
    pass_instance = GraphTransformPass()
    return pass_instance.apply(traced_model)