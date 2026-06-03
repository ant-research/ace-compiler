#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Reusable sample models, functions, and input generators for FHE examples.
"""

from .models import (
    LinearModel,
    AddModel,
    ReluModel,
    ConvModel,
    MLPClassifier,
)
from .functions import (
    add_func,
    mul_func,
    relu_func,
    linear_func,
)
from .input_generators import (
    rand_input,
    randn_input,
    ones_input,
    get_linear_inputs,
    get_conv_inputs,
)

__all__ = [
    # Models
    "LinearModel",
    "AddModel",
    "ReluModel",
    "ConvModel",
    "MLPClassifier",
    # Functions
    "add_func",
    "mul_func",
    "relu_func",
    "linear_func",
    # Input generators
    "rand_input",
    "randn_input",
    "ones_input",
    "get_linear_inputs",
    "get_conv_inputs",
]