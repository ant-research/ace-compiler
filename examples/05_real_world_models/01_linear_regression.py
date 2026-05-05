#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Real-World Model Example 1: Linear Regression

FHE-compiled linear regression for house price prediction.
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LinearRegression(nn.Module):
    """Linear regression model for prediction."""

    def __init__(self, input_size=5):
        super().__init__()
        self.linear = nn.Linear(input_size, 1)

    def forward(self, x):
        return self.linear(x)


if __name__ == "__main__":
    # Create model (5 features -> 1 price prediction)
    model = LinearRegression(input_size=5)
    model.eval()

    # Simulate trained weights (for demo purposes)
    # Using small weights for better FHE precision
    with torch.no_grad():
        model.linear.weight.fill_(0.1)
        model.linear.bias.fill_(0.5)

    # Prepare sample input (batch=1, features=5)
    # Features: [size/100, bedrooms/10, age/100, distance/10, rating/10]
    # Normalized inputs for better FHE precision
    x = torch.tensor([[0.8, 0.3, 0.2, 0.5, 0.8]])

    # Compile with FHE
    print("Compiling linear regression model...")
    program = model.compile([x])

    # Run FHE inference
    print("Running FHE inference...")
    result = program(x)

    # Compare with plaintext
    expected = model(x)
    print(f"\nInput features: {x.tolist()}")
    print(f"FHE prediction:   {result.flatten()[0].item():.4f}")
    print(f"Plaintext:        {expected.item():.4f}")

    # FHE uses CKKS encoding with multiple slots, compare first element
    # Note: FHE validation may fail due to CKKS approximation error
    is_close = abs(result.flatten()[0].item() - expected.item()) < 0.2
    print(f"Match (tol=0.2):  {'YES' if is_close else 'NO'}")