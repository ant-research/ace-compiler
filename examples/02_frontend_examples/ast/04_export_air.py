#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AST Frontend: Export to AIR (API)

Use fhe.export function to export frontend IR to AIR format (.B file).
"""

import torch
from ace import fhe


def add(x, y):
    """Add two tensors."""
    return x + y


if __name__ == "__main__":
    x = torch.ones(1, 4)
    y = torch.ones(1, 4) * 2

    # Use fhe.export as function (API)
    exported_func = fhe.export(frontend="ast", format="air", output_path="/tmp/ast_export_api.B")(add)

    # Export the function
    result = exported_func.export([x, y])
    print(f"Exported to: {result}")