#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Model: Logistic Regression

FHE-compiled logistic regression for binary classification.
Outputs logits (sigmoid omitted for FHE compatibility).
"""

import torch
import torch.nn as nn
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LogisticRegression(nn.Module):
    """Logistic regression: outputs logits (no sigmoid for FHE compatibility)."""

    def __init__(self, input_size=5):
        super().__init__()
        self.linear = nn.Linear(input_size, 1)

    def forward(self, x):
        return self.linear(x)  # logits only; apply sigmoid client-side


if __name__ == "__main__":
    model = LogisticRegression(input_size=5)
    model.eval()

    with torch.no_grad():
        model.linear.weight.fill_(0.1)
        model.linear.bias.fill_(0.0)

    x = torch.tensor([[0.8, 0.3, 0.2, 0.5, 0.8]])

    print("Compiling logistic regression model...")
    program = model.compile([x])

    print("Running FHE inference...")
    result = program(x)
    expected = model(x)

    # Apply sigmoid client-side to get probability
    prob = torch.sigmoid(result.flatten()[0])
    expected_prob = torch.sigmoid(expected.flatten()[0])

    print(f"\nFHE logit:        {result.flatten()[0].item():.4f}")
    print(f"FHE probability:  {prob.item():.4f}")
    print(f"Plaintext prob:   {expected_prob.item():.4f}")
    print(f"Validate: {'PASSED' if program.validate() else 'FAILED'}")