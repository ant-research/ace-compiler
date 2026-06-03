#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Model: MLP Classifier

Multi-layer perceptron for classification.
ReLU activations omitted for FHE compatibility (use linear-only layers).
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class MLPClassifier(nn.Module):
    """MLP with linear-only layers (ReLU omitted for FHE compatibility)."""

    def __init__(self, input_size=10, hidden_size=16, num_classes=4):
        super().__init__()
        self.network = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            # nn.ReLU() — omitted for FHE; add with relu_vr profiling if needed
            nn.Linear(hidden_size, hidden_size // 2),
            nn.Linear(hidden_size // 2, num_classes),
        )

    def forward(self, x):
        return self.network(x)


if __name__ == "__main__":
    model = MLPClassifier(input_size=10, hidden_size=16, num_classes=4)
    model.eval()

    for m in model.modules():
        if isinstance(m, nn.Linear):
            torch.nn.init.xavier_uniform_(m.weight)
            torch.nn.init.zeros_(m.bias)

    x = torch.randn(1, 10)

    print("Compiling MLP classifier...")
    program = model.compile([x])

    print("Running FHE inference...")
    result = program(x)
    expected = model(x)

    print(f"\nInput shape:  {x.shape}")
    print(f"Output shape: {result.shape}")
    print(f"FHE output:   {result.flatten().tolist()}")
    print(f"Plaintext:    {expected.tolist()[0]}")
    print(f"Validate: {'PASSED' if program.validate() else 'FAILED'}")