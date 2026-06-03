#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Frontend: Torch — Model (nn.Module)

Compile a PyTorch nn.Module with torch frontend.
"""

import torch
import torch.nn as nn
from ace import fhe


# ── Decorator style ──────────────────────────────────────────────────

@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LinearModel(nn.Module):
    """Simple linear model: y = Wx + b."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)


x = torch.randn(1, 4)

print("=== Decorator: @fhe.compile on nn.Module ===")
program = LinearModel.compile([x])
result = program(x)
print(f"  Input shape:  {x.shape}")
print(f"  Output shape: {result.shape}")
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")
print()

# ── API style ────────────────────────────────────────────────────────

class TwoInputModel(nn.Module):
    """Model with two inputs."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 4)

    def forward(self, x, y):
        return self.linear(x) + y


print("=== API: fhe.compile(...)(Model) ===")
compiled = fhe.compile(frontend="torch", library="antlib", device="cpu")(TwoInputModel)

y = torch.randn(1, 4)
program = compiled.compile([x, y])
result = program(x, y)
print(f"  Validate: {'PASSED' if program.validate() else 'FAILED'}")