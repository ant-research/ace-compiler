#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR & Export: Export to ONNX format

Two ways to export:
  1. Decorator:  @fhe.export(format="onnx", output_path=...)
  2. API call:   fhe.export(format="onnx", output_path=...)(model)
"""

import torch
import torch.nn as nn
from ace import fhe


class LinearModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


x = torch.randn(1, 4)

# ── Decorator style ──────────────────────────────────────────────────

@fhe.export(frontend="torch", format="onnx", output_path="/tmp/ace_export_decorator.onnx")
class ExportDecoratorModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


print("=== @fhe.export decorator → ONNX ===")
result = ExportDecoratorModel.export([x])
print(f"  Exported to: {result}")
print()

# ── API style ────────────────────────────────────────────────────────

model = LinearModel()

print("=== fhe.export(...)(model) → ONNX ===")
exported = fhe.export(frontend="torch", format="onnx", output_path="/tmp/ace_export_api.onnx")(model)
result = exported.export([x])
print(f"  Exported to: {result}")