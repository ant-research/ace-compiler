#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR & Export: Compile from an ONNX file

Use the Driver API to load an existing .onnx file and compile it for FHE.

Note: ONNX frontend runtime is still in progress — compiled .so may
fail at inference with "fail to find <op>" errors. Compilation and
export work correctly.
"""

import torch
import torch.nn as nn
from ace import fhe
from ace.fhe.driver import Driver


# Step 1: Create an ONNX file (in practice you'd use an existing file)
class LinearModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


model = LinearModel()
model.eval()
x = torch.randn(1, 4)
onnx_path = "/tmp/ace_example_linear.onnx"
torch.onnx.export(model, x, onnx_path, input_names=["input"])

# Step 2: Compile the ONNX file
print("=== Compile from ONNX file ===")
driver = Driver(frontend="onnx", library="antlib", device="cpu")
package = driver.compile(onnx_path, [x], input_names=["input"])

print(f"  Kernel:   {package['kernel']}")
print(f"  Inputs:   {package['input_info']}")
print(f"  Outputs:  {package['output_info']}")
print()
print("  Note: ONNX frontend runtime is in progress.")
print("  Compilation succeeds but inference may fail with weight-loading errors.")