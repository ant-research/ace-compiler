#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Frontend: Torch-via-ONNX

Compile a PyTorch model by first exporting to ONNX, then compiling.
Useful when torch FX tracing has limitations.
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch-via-onnx", library="antlib", device="cpu")
class LinearViaONNX(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


model = LinearViaONNX()
model.eval()
x = torch.randn(1, 4)

print("=== @fhe.compile with torch-via-onnx ===")
program = LinearViaONNX.compile([x])
result = program(x)
expected = model(x)
print(f"  Result:   {result.tolist()}")
print(f"  Expected: {expected.tolist()}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")