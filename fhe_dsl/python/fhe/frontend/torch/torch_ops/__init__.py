#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
PyTorch op mappings organized by abstraction level.

Levels:
- tensor: Tensor-level operations (current)
- vec: Vector-level operations (future)
- ckks: CKKS scheme operations (future)
- poly: Polynomial-level operations (future)
"""

from .tensor import (
    TORCH_OP_TO_CUSTOM_OP,
    NN_MODULE_TO_CUSTOM_OP,
    get_custom_op,
    get_op_name,
    get_custom_op_for_module,
    get_module_op_name,
)

__all__ = [
    "TORCH_OP_TO_CUSTOM_OP",
    "NN_MODULE_TO_CUSTOM_OP",
    "get_custom_op",
    "get_op_name",
    "get_custom_op_for_module",
    "get_module_op_name",
]