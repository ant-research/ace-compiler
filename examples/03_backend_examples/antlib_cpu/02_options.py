#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Backend Example: AntLib CPU with Options

Configure CKKS encryption parameters for antlib backend.
"""

import torch
from ace import fhe


# Configure CKKS parameters
# N: Polynomial modulus degree (higher = more secure, slower)
# scale: Scaling factor for encoding
# level: Multiplicative depth
fhe_options = {
    "ckks": {
        "N": 4096,
        "scale": 2**40,
        "level": 2
    }
}


@fhe.compute(
    frontend="torch",
    library="antlib",
    device="cpu",
    validate=True,
    **fhe_options
)
def add(x, y):
    """Add two tensors."""
    return x + y


if __name__ == "__main__":
    x = torch.ones(1, 4)
    y = torch.ones(1, 4) * 2

    result = add(x, y)

    print(f"Backend: antlib")
    print(f"Device: cpu")
    print(f"CKKS N: 4096")
    print(f"Input X: {x.tolist()}")
    print(f"Input Y: {y.tolist()}")
    print(f"Result: {result.tolist()}")