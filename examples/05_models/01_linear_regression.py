#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Model: Linear Regression

FHE-compiled linear regression for house price prediction.
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LinearRegression(nn.Module):
    """Linear regression: 5 features -> 1 price prediction."""

    def __init__(self, input_size=5):
        super().__init__()
        self.linear = nn.Linear(input_size, 1)

    def forward(self, x):
        return self.linear(x)


if __name__ == "__main__":
    model = LinearRegression(input_size=5)
    model.eval()

    # Simulate trained weights (small values for better FHE precision)
    with torch.no_grad():
        model.linear.weight.fill_(0.1)
        model.linear.bias.fill_(0.5)

    # Features: [size/100, bedrooms/10, age/100, distance/10, rating/10]
    x = torch.tensor([[0.8, 0.3, 0.2, 0.5, 0.8]])

    print("Compiling linear regression model...")
    program = model.compile([x])

    print("Running FHE inference...")
    result = program(x)
    expected = model(x)

    print(f"\nInput features: {x.tolist()}")
    print(f"FHE prediction:   {result.flatten()[0].item():.4f}")
    print(f"Plaintext:        {expected.item():.4f}")

    # CKKS has approximation error; use tolerance-based comparison
    is_close = abs(result.flatten()[0].item() - expected.item()) < 0.2
    print(f"Match (tol=0.2):  {'YES' if is_close else 'NO'}")