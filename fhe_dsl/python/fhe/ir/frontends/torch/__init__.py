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

from .custom_ops import CUSTOM_OPERATORS, STANDARD_OP_MAPPING, CustomTracer, trace_with_metadata
from .torch_trace import FXTracedModel, TorchTracedModel


__all__ = [
    "CUSTOM_OPERATORS",
    "STANDARD_OP_MAPPING",
    "CustomTracer",
    "FXTracedModel",
    "TorchTracedModel",
    "trace_with_metadata",
]
