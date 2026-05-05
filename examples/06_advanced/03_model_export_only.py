#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Advanced Example 3: Model Export Only

Export model to ONNX or AIR format without full FHE compilation.
Useful for model sharing, inspection, or separate compilation pipeline.
"""

import torch
import torch.nn as nn
from ace import fhe


class SimpleModel(nn.Module):
    """Simple model for export."""

    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)
        self.relu = nn.ReLU()

    def forward(self, x):
        return self.relu(self.linear(x))


if __name__ == "__main__":
    # Create model
    model = SimpleModel()
    model.eval()

    # Prepare dummy input
    x = torch.randn(1, 4)

    # =========================================================================
    # Export to ONNX
    # =========================================================================
    print("Exporting model to ONNX...")
    onnx_path = "/tmp/exported_model.onnx"

    # Use standard PyTorch ONNX export
    torch.onnx.export(model, x, onnx_path, opset_version=11)
    print(f"  Exported to: {onnx_path}")

    # Verify ONNX model
    try:
        import onnx
        onnx_model = onnx.load(onnx_path)
        onnx.checker.check_model(onnx_model)
        print(f"  ONNX validation: PASSED")
    except ImportError:
        print("  ONNX not installed, skipping validation")

    # =========================================================================
    # Export to AIR (.B file)
    # =========================================================================
    print("\nExporting model to AIR (.B file)...")
    air_path = "/tmp/exported_model.B"

    try:
        # Use Driver's export method to export AIR
        from ace.fhe.driver import Driver
        driver = Driver(frontend="torch", library="antlib", device="cpu")
        exported_air = driver.export(
            [x],
            input_names=None,
            format="air",
            output_path=air_path,
            source=model
        )
        print(f"  Exported to: {exported_air}")

        import os
        if os.path.exists(exported_air):
            print(f"  File size: {os.path.getsize(exported_air)} bytes")
    except Exception as e:
        print(f"  AIR export failed: {e}")

    # =========================================================================
    # Use cases for export-only:
    # =========================================================================
    print("\n" + "=" * 50)
    print("Use cases for model export:")
    print("  - Model sharing between teams")
    print("  - Model inspection and debugging")
    print("  - Separate compilation pipeline")
    print("  - Version control of model architecture")
    print("=" * 50)