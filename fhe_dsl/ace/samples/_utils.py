#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Input generation utilities for examples and benchmarks.
"""
from enum import Enum
from typing import Tuple

from ace import TORCH_AVAILABLE, torch


class InputMode(Enum):
    """Input generation modes."""
    ONES = "ones"           # All ones
    NEG_ONES = "neg_ones"   # All negative ones
    ARANGE = "arange"       # Incremental values
    RANDOM = "random"       # Random values


def generate_inputs_by_mode(
    example_inputs: Tuple,
    mode: InputMode,
    seed: int = 42
) -> Tuple:
    """
    Generate inputs based on the specified mode, preserving shape and dtype.

    Args:
        example_inputs: Original input tensors to get shape and dtype from
        mode: Input generation mode
        seed: Random seed (used only for RANDOM mode)

    Returns:
        Tuple of newly generated input tensors
    """
    if not TORCH_AVAILABLE:
        raise RuntimeError("torch not available")

    result = []
    for inp in example_inputs:
        shape = inp.shape
        dtype = inp.dtype

        if mode == InputMode.ONES:
            result.append(torch.ones(shape, dtype=dtype))
        elif mode == InputMode.NEG_ONES:
            result.append(torch.full(shape, -1.0, dtype=dtype))
        elif mode == InputMode.ARANGE:
            total = 1
            for dim in shape:
                total *= dim
            tensor = torch.arange(total, dtype=dtype).reshape(shape)
            result.append(tensor / total)  # Normalize to [0, 1)
        elif mode == InputMode.RANDOM:
            torch.manual_seed(seed)
            result.append(torch.randn(shape, dtype=dtype))
        else:
            raise ValueError(f"Unknown mode: {mode}")

    return tuple(result)


# Fixed input modes for regression tests
REGRESSION_MODES = [InputMode.ONES, InputMode.NEG_ONES, InputMode.ARANGE]