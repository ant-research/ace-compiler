#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Reusable sample models, functions, and input generators for FHE examples.
"""

from .functions import (
    add_func,
    linear_func,
    mul_func,
    relu_func,
)
from .input_generators import (
    get_conv_inputs,
    get_linear_inputs,
    ones_input,
    rand_input,
    randn_input,
)
from .models import (
    AddModel,
    ConvModel,
    LinearModel,
    MLPClassifier,
    ReluModel,
)


__all__ = [
    "AddModel",
    "ConvModel",
    # Models
    "LinearModel",
    "MLPClassifier",
    "ReluModel",
    # Functions
    "add_func",
    "get_conv_inputs",
    "get_linear_inputs",
    "linear_func",
    "mul_func",
    "ones_input",
    # Input generators
    "rand_input",
    "randn_input",
    "relu_func",
]
