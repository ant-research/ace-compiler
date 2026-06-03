#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR & Export: Export to AIR format (.B file)

Two ways to export:
  1. Decorator:  @fhe.export(format="air", output_path=...)
  2. API call:   fhe.export(format="air", output_path=...)(model)
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

@fhe.export(frontend="torch", format="air", output_path="/tmp/ace_export_decorator.B")
class ExportDecoratorModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


print("=== @fhe.export decorator → AIR ===")
result = ExportDecoratorModel.export([x])
print(f"  Exported to: {result}")
print()

# ── API style ────────────────────────────────────────────────────────

model = LinearModel()

print("=== fhe.export(...)(model) → AIR ===")
exported = fhe.export(frontend="torch", format="air", output_path="/tmp/ace_export_api.B")(model)
result = exported.export([x])
print(f"  Exported to: {result}")