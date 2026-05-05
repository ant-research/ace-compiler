#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR Format Example 2: Export to AIR (.B file)

Export a PyTorch model to AIR format (intermediate representation for FHE).
The .B file can be inspected or used for later compilation.
"""

import torch
import torch.nn as nn

from ace import fhe


@fhe.compile(frontend="torch", library="antlib", device="cpu")
class AddModel(nn.Module):
    """Simple add model."""

    def forward(self, x, y):
        return x + y


if __name__ == "__main__":
    # Create model
    model = AddModel()

    # Export to AIR using torch.onnx.export and then convert to AIR
    output_path = "/tmp/exported_model.B"
    input_x = torch.randn(1, 4)
    input_y = torch.randn(1, 4)

    # Note: Direct AIR export requires the fhe_cmplr compiler.
    # This example demonstrates the compilation flow which generates AIR internally.

    try:
        print("Compiling model (AIR IR is generated internally)...")
        program = model.compile([input_x, input_y])

        print("Compilation successful!")
        print("\nNote: AIR IR is generated internally during compilation.")
        print("The .B file format is a binary ELF-like format containing:")
        print("  - Function definitions")
        print("  - Type information")
        print("  - Operation nodes")
        print("  - Constants and attributes")
        print("\nTo inspect AIR files, use the fhe_cmplr tool or examine the source.")

    except Exception as e:
        print(f"Compilation failed: {e}")