#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Real-World Model Example 2: Logistic Regression

FHE-compiled logistic regression for binary classification.
Note: Sigmoid is approximated in FHE using polynomial approximation.
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LogisticRegression(nn.Module):
    """Logistic regression model for binary classification.

    Note: Sigmoid requires polynomial approximation in FHE.
    This example uses linear output only for demonstration.
    """

    def __init__(self, input_size=4):
        super().__init__()
        self.linear = nn.Linear(input_size, 1)
        # Note: Sigmoid is omitted for FHE compatibility
        # In practice, use polynomial approximation: sigmoid(x) ≈ 0.5 + 0.25x - 0.02x^3

    def forward(self, x):
        return self.linear(x)  # Output logits (pre-sigmoid)


if __name__ == "__main__":
    # Create model (4 features -> binary classification)
    model = LogisticRegression(input_size=4)
    model.eval()

    # Simulate trained weights (for demo purposes)
    with torch.no_grad():
        model.linear.weight.fill_(0.3)
        model.linear.bias.fill_(0.0)

    # Prepare sample input (batch=1, features=4)
    # Features for iris classification example
    x = torch.tensor([[5.1, 3.5, 1.4, 0.2]])

    # Compile with FHE
    print("Compiling logistic regression model...")
    program = model.compile([x])

    # Run FHE inference
    print("Running FHE inference...")
    result = program(x)

    # Compare with plaintext
    expected = model(x)
    print(f"\nInput features: {x.tolist()}")
    print(f"FHE prediction:   {result.flatten()[0].item():.4f}")
    print(f"Plaintext:        {expected.item():.4f}")
    print(f"\nNote: Output is logit (pre-sigmoid).")
    print(f"Class:            {'Positive' if result.flatten()[0].item() > 0 else 'Negative'}")
    print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")