#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend Module

PyTorch FX tracing path for AIR IR generation.

Pipeline:
1. prepare()  - PyTorch → FX Trace → TorchTracedModel
2. compile()  - Execute traced model → generates AIR IR
3. export()   - Export AIR IR to .B file
"""

from .torch_frontend import Torch

__all__ = [
    "Torch",
]