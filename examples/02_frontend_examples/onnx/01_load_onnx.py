#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ONNX Frontend: Load and Compile ONNX Model

Use Driver with onnx frontend to compile a pre-exported ONNX model.
"""

import torch
import torch.nn as nn
from ace.fhe.driver import Driver


class LinearModel(nn.Module):
    """Simple linear model."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 4)

    def forward(self, x):
        return self.linear(x)


if __name__ == "__main__":
    # Step 1: Export model to ONNX using torch
    model = LinearModel()
    model.eval()
    dummy_input = torch.randn(1, 4)
    onnx_path = "/tmp/linear_model.onnx"

    torch.onnx.export(
        model,
        dummy_input,
        onnx_path,
        input_names=["input"],
        output_names=["output"],
        opset_version=11,
    )
    print(f"Exported model to ONNX: {onnx_path}")

    # Step 2: Use Driver with onnx frontend
    driver = Driver(frontend="onnx", library="antlib", device="cpu")

    x = torch.randn(1, 4)

    # Compile using ONNX file path
    package = driver.compile(onnx_path, [x], input_names=["input"])

    # Run inference
    from ace.fhe.runtime import FHERuntime
    runtime = FHERuntime(package)
    result = runtime.inference(x)

    print(f"Input shape: {x.shape}")
    print(f"Output shape: {result.shape}")