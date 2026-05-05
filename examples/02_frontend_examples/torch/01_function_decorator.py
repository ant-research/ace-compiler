#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend: Function with Decorator

Use @fhe.compile decorator with torch frontend for a Python function.
"""

import torch
from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    """Add two tensors."""
    return x + y


if __name__ == "__main__":
    x = torch.ones(1, 4)
    y = torch.ones(1, 4) * 2

    program = add.compile([x, y])
    result = program(x, y)

    print(f"Input X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result: {result.tolist()}")
    print(f"Validation: {'PASSED' if program.validate() else 'FAILED'}")

