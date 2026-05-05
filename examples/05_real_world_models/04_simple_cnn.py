#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Real-World Model Example 4: Simple CNN

Convolutional neural network for image classification.
Note: CNN models are computationally intensive in FHE.
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class SimpleCNN(nn.Module):
    """Simple CNN for image classification.

    Architecture: Conv -> ReLU -> Pool -> Conv -> ReLU -> Pool -> Linear
    Note: ReLU is not FHE-friendly, this is for structure demonstration.
    """

    def __init__(self, num_classes=10):
        super().__init__()
        # For 8x8 input:
        # Conv1: 8x8 -> 8x8 (padding=1, kernel=3)
        # Pool1: 8x8 -> 4x4
        # Conv2: 4x4 -> 4x4 (padding=1, kernel=3)
        # Pool2: 4x4 -> 2x2
        # Flatten: 16 * 2 * 2 = 64
        self.features = nn.Sequential(
            nn.Conv2d(1, 8, kernel_size=3, padding=1),
            nn.ReLU(),
            nn.AvgPool2d(2),
            nn.Conv2d(8, 16, kernel_size=3, padding=1),
            nn.ReLU(),
            nn.AvgPool2d(2),
        )
        self.classifier = nn.Linear(16 * 2 * 2, num_classes)  # 64 -> 10

    def forward(self, x):
        x = self.features(x)
        x = x.view(x.size(0), -1)  # Flatten
        x = self.classifier(x)
        return x


if __name__ == "__main__":
    # Create model
    model = SimpleCNN(num_classes=10)
    model.eval()

    # Initialize weights
    for m in model.modules():
        if isinstance(m, nn.Conv2d):
            torch.nn.init.kaiming_uniform_(m.weight)
            torch.nn.init.zeros_(m.bias)
        elif isinstance(m, nn.Linear):
            torch.nn.init.xavier_uniform_(m.weight)
            torch.nn.init.zeros_(m.bias)

    # Prepare sample input (small image for demo: 1x8x8)
    x = torch.randn(1, 1, 8, 8)

    # Get plaintext output first
    with torch.no_grad():
        expected = model(x)

    print("Simple CNN Model")
    print("=" * 40)
    print(f"Input shape:  {x.shape}")
    print(f"Output shape: {expected.shape}")
    print(f"Output (plaintext): {expected.tolist()[0][:5]}...")

    # Note about CNN compilation
    print("\n" + "=" * 40)
    print("Note: CNN models are computationally intensive in FHE.")
    print("Compilation may take several minutes.")
    print("For demo purposes, we show model structure only.")
    print("=" * 40)

    # Uncomment to actually compile (takes time):
    # program = model.compile([x])
    # result = program(x)
    # print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")