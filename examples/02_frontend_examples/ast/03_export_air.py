#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AST Frontend: Export to AIR (Decorator)

Use @fhe.export decorator to export frontend IR to AIR format (.B file).
"""

import torch
from ace import fhe


@fhe.export(frontend="ast", format="air", output_path="/tmp/ast_export_decorator.B")
def add(x, y):
    """Add two tensors."""
    return x + y


if __name__ == "__main__":
    x = torch.ones(1, 4)
    y = torch.ones(1, 4) * 2

    # Export the function using decorator
    result = add.export([x, y])
    print(f"Exported to: {result}")