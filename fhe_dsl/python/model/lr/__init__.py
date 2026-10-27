#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Logistic Regression (LR) models for FHE classification.

This package provides a small, minimal nn.Module for logistic regression
(linear layer, no hidden activation) — the simplest end-to-end FHE model.

ModelSpec instances live at ace.model.spec_lr (in the model package root,
mirroring spec_resnet.py).

Usage:
    from ace.model.spec_lr import LR_SPECS
    model = LR_SPECS[0].create_model()
"""

# Import model creator(s)
from .lr import LogisticRegression

__all__ = [
    "LogisticRegression",
]
