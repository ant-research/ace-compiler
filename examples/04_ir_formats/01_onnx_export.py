#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR Format Example 1: Export to ONNX

Export a PyTorch model to ONNX format without full FHE compilation.
Useful for model inspection and debugging.
"""

import torch
import torch.nn as nn
from ace import fhe


class SimpleModel(nn.Module):
    """Simple linear model for export."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)
        self.relu = nn.ReLU()

    def forward(self, x):
        return self.relu(self.linear(x))


if __name__ == "__main__":
    # Create model
    model = SimpleModel()

    # Export to ONNX using the model directly
    output_path = "/tmp/exported_model.onnx"
    dummy_input = torch.randn(1, 4)

    # Use fhe.compile to get the wrapped model with export method
    compiled_model = fhe.compile(frontend="torch", library="antlib", device="cpu")(model)

    # Export the model to ONNX format before full compilation
    # Note: This exports the frontend IR (ONNX) for inspection
    import torch.onnx
    torch.onnx.export(model, dummy_input, output_path, opset_version=11)
    print(f"Exported ONNX model to: {output_path}")

    # Verify exported model
    import onnx
    onnx_model = onnx.load(output_path)
    onnx.checker.check_model(onnx_model)

    print(f"Model IR version: {onnx_model.ir_version}")
    print(f"Opset: {[op.version for op in onnx_model.opset_import]}")

    # Print graph info
    graph = onnx_model.graph
    print(f"\nInputs:")
    for inp in graph.input:
        shape = [dim.dim_value for dim in inp.type.tensor_type.shape.dim]
        print(f"  - {inp.name}: {shape}")

    print(f"\nOutputs:")
    for out in graph.output:
        shape = [dim.dim_value for dim in out.type.tensor_type.shape.dim]
        print(f"  - {out.name}: {shape}")

    print(f"\nNodes ({len(graph.node)} total):")
    for i, node in enumerate(graph.node[:5]):  # Show first 5 nodes
        print(f"  [{i}] {node.op_type}")
    if len(graph.node) > 5:
        print(f"  ... and {len(graph.node) - 5} more nodes")