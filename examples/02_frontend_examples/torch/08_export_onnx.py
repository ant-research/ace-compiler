#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend: Export to ONNX (API)

Use fhe.export function to export frontend IR to ONNX format.
"""

import torch
import torch.nn as nn
from ace import fhe


class LinearModel(nn.Module):
    """Simple linear model."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 4)

    def forward(self, x):
        return self.linear(x)


if __name__ == "__main__":
    model = LinearModel()
    x = torch.randn(1, 4)

    # Use fhe.export as function (API)
    exported_model = fhe.export(frontend="torch", format="onnx", output_path="/tmp/exported_model_api.onnx")(model)

    # Export the model
    result = exported_model.export([x])
    print(f"Exported to: {result}")