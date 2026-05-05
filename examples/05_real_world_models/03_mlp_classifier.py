#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Real-World Model Example 3: MLP Classifier

Multi-layer perceptron for classification with ReLU activations.
"""

import torch
import torch.nn as nn
from ace import fhe


class MLPClassifier(nn.Module):
    """Multi-layer perceptron for classification.

    Architecture: Linear -> Linear -> Linear (ReLU omitted for FHE compatibility)
    Note: ReLU requires polynomial approximation in FHE.
    """

    def __init__(self, input_size=10, hidden_size=16, num_classes=4):
        super().__init__()
        # For FHE compatibility, use linear layers only
        # ReLU can be approximated with polynomials if needed
        self.network = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            # nn.ReLU(),  # Omitted for FHE compatibility
            nn.Linear(hidden_size, hidden_size // 2),
            # nn.ReLU(),  # Omitted for FHE compatibility
            nn.Linear(hidden_size // 2, num_classes),
        )

    def forward(self, x):
        # Flatten input - static reshape for FHE compatibility
        batch_size = x.size(0)
        x = x.view(batch_size, -1)
        return self.network(x)


if __name__ == "__main__":
    # Create model (smaller for demo)
    model = MLPClassifier(input_size=10, hidden_size=16, num_classes=4)
    model.eval()

    # Initialize weights for demo
    for m in model.modules():
        if isinstance(m, nn.Linear):
            torch.nn.init.xavier_uniform_(m.weight)
            torch.nn.init.zeros_(m.bias)

    # Prepare sample input (small for demo)
    x = torch.randn(1, 10)

    # Compile with FHE - wrap the forward call
    print("Compiling MLP classifier...")

    compiled_model = fhe.compile(frontend="torch", library="antlib", device="cpu")
    program = compiled_model(model)._fhe_compile([x])

    # Run FHE inference
    print("Running FHE inference...")
    result = program(x)

    # Compare with plaintext
    expected = model(x)
    print(f"\nInput shape:  {x.shape}")
    print(f"Output shape: {result.shape}")
    print(f"FHE output:   {result.flatten().tolist()}")
    print(f"Plaintext:    {expected.tolist()[0]}")