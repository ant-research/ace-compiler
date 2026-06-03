#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Function module: Runtime Computation.
"""

from .runtime import FHERuntime
from .program import CompiledProgram
from .results import BatchResult, BatchTiming, DatasetResult

__all__ = ["FHERuntime", "CompiledProgram", "BatchResult", "BatchTiming", "DatasetResult"]