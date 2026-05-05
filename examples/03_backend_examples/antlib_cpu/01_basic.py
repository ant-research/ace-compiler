#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Backend Example: AntLib CPU

Use antlib backend with CPU device for FHE compilation.
"""

import torch
from ace import fhe


@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
def add(x, y):
    """Add two tensors."""
    return x + y


if __name__ == "__main__":
    x = torch.ones(1, 4)
    y = torch.ones(1, 4) * 2

    result = add(x, y)

    print(f"Backend: antlib")
    print(f"Device: cpu")
    print(f"Input X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result: {result.tolist()}")