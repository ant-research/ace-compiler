#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Custom Tracer for FX Graph Tracing

This module provides a custom FX tracer that automatically identifies
custom operators and injects op_name into node.meta during tracing.
"""

from typing import Optional
import torch.fx as fx

from .registry import STANDARD_OP_MAPPING

# No other changes needed - registry.py is in the same directory


class CustomTracer(fx.Tracer):
    """
    Custom Tracer that identifies custom ops during tracing.

    This tracer injects op_name and hardware info into node.meta
    during the create_proxy phase, allowing the execution phase
    to directly read this information without thread-local storage.

    Workflow:
    1. During create_proxy, check if target is a custom op
    2. If yes, inject node.meta['op_name'] = operator name
    3. Also inject node.meta['hardware'] and node.meta['library']
    """

    def __init__(self, library: str = "antlib", device: str = "cpu", **kwargs):
        super().__init__(**kwargs)
        self._library = library
        self._device = device

    def create_proxy(self, kind, target, args, kwargs, name=None, type_expr=None):
        proxy = super().create_proxy(kind, target, args, kwargs, name, type_expr)

        # Only handle call_function type
        if kind == 'call_function':
            target_str = str(target)

            # Check if this is a custom op
            op_name = self._get_op_name(target, target_str)
            if op_name:
                # Inject op_name into node.meta
                proxy.node.meta['op_name'] = op_name
                # Inject hardware info
                proxy.node.meta['hardware'] = self._device
                proxy.node.meta['library'] = self._library

        return proxy

    def _get_op_name(self, target, target_str: str) -> Optional[str]:
        """Extract operator name from target."""
        # First check STANDARD_OP_MAPPING
        if target_str in STANDARD_OP_MAPPING:
            return STANDARD_OP_MAPPING[target_str]

        # Check if target.__name__ is in mapping
        if hasattr(target, '__name__'):
            func_name = target.__name__
            # Try various format matches
            for key, value in STANDARD_OP_MAPPING.items():
                if key.endswith(func_name):
                    return value

        # Check if target_str ends with a key
        for key, value in STANDARD_OP_MAPPING.items():
            if target_str == key or target_str.endswith('.' + key.split('.')[-1]):
                return value

        return None


def trace_with_metadata(model, example_inputs, library="antlib", device="cpu"):
    """
    Trace a model using CustomTracer with automatic metadata injection.

    Args:
        model: PyTorch model or function
        example_inputs: Example inputs for tracing
        library: Target library (antlib/phantom/acelib)
        device: Target device (cpu/cuda)

    Returns:
        Traced GraphModule with node.meta containing op_name
    """
    tracer = CustomTracer(library=library, device=device)
    traced = fx.GraphModule(model, tracer.trace(model))
    return traced