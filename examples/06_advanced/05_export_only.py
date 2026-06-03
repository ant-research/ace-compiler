#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced: Export Only (No Compilation)

Export models to AIR or ONNX format without full FHE compilation.
Useful for inspecting IR or integrating with external tools.
"""

import torch
import torch.nn as nn
from ace import fhe
from ace.fhe.driver import Driver


class LinearModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


x = torch.randn(1, 4)
model = LinearModel()
model.eval()

# ── Export to AIR via @fhe.export ────────────────────────────────────

@fhe.export(frontend="torch", format="air", output_path="/tmp/ace_export_air.B")
class ExportAIRModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


print("=== Export to AIR (.B file) ===")
result = ExportAIRModel.export([x])
print(f"  AIR file: {result}")
print()

# ── Export to ONNX via @fhe.export ───────────────────────────────────

@fhe.export(frontend="torch", format="onnx", output_path="/tmp/ace_export_onnx.onnx")
class ExportONNXModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


print("=== Export to ONNX (.onnx file) ===")
result = ExportONNXModel.export([x])
print(f"  ONNX file: {result}")
print()

# ── Export via Driver API ────────────────────────────────────────────

print("=== Export via Driver API ===")
driver = Driver(frontend="torch", library="antlib", device="cpu")
air_path = driver.export([x], format="air", output_path="/tmp/ace_driver_export.B", source=model)
print(f"  AIR file: {air_path}")