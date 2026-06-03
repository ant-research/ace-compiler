#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend Module

PyTorch FX tracing path for AIR IR generation.

Usage:
    from ace.fhe.ir.frontends.torch import TorchTracedModel

    traced = torch.fx.symbolic_trace(model)
    ir_model = TorchTracedModel(traced, input_names, input_shapes, output_shape)
    ir_model.execute(*example_inputs)  # Generates AIR IR
    ir_model.export_ir("output.B")
"""

from .torch_trace import TorchTracedModel, FXTracedModel
from .custom_ops import CustomTracer, trace_with_metadata, STANDARD_OP_MAPPING, CUSTOM_OPERATORS

__all__ = [
    "TorchTracedModel",
    "FXTracedModel",
    "CustomTracer",
    "trace_with_metadata",
    "STANDARD_OP_MAPPING",
    "CUSTOM_OPERATORS",
]