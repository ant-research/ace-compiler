#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import torch
import logging
from typing import List, Tuple, Any, Optional

from ._ext import get_module
from .key_manager import KeyManager

logger = logging.getLogger(__name__)

# ======================
# Constants
# ======================

SHAPE_DIMENSION = 4
VERIFY_MODES = {"array", "tensor"}

# ======================
# FHERuntime Class
# ======================

class FHEProgram:
    """
    FHE Program Executor - responsible for the complete execution process of a single backend.
    
    The JitRunner feature corresponding to C++.
    """

    def __init__(self, backend_name: str, lib_path: str, package: dict):
        """
        Args:
            backend_name: Backend name (e.g., "antlib_cpu")
            lib_path: Dynamic library path
            Package: Model package configuration
        """
        rt_mod = get_module()

        self.backend_name = backend_name
        self.lib_path = lib_path
        self.package = package

        # log_level = logging.getLevelName(logging.getLogger().getEffectiveLevel())
        rt_mod = get_module()

        self._manager = rt_mod.BackendManager() # (log_level)
        self._manager.register_backend(backend_name, lib_path) # "./build_ops/libantlib_cpu.so")
    
        # # Initialize key manager and data entries
        self._data_entry: List[Tuple[str, str, Any, int]] = []

    def _build_tensor_info(self, tensor_list: List[dict]) -> List[Any]:
        """Convert Python tensor dicts to C++ TensorInfo objects."""
        rt_mod = get_module()
        result = []
        for item in tensor_list:
            info = rt_mod.TensorInfo()
            info.name = item["name"]
            info.shape = self._pad_shape_to_4d(item["shape"])
            info.type = item["type"]
            info.tensor = item["data"]
            result.append(info)
        return result

    def _pad_shape_to_4d(self, shape: List[int]) -> List[int]:
        """Pad shape to 4D (e.g., [2,3] → [1,1,2,3])."""
        if len(shape) > SHAPE_DIMENSION:
            raise ValueError(f"Shape {shape} exceeds max dimension {SHAPE_DIMENSION}")
        padded = [1] * (SHAPE_DIMENSION - len(shape)) + shape
        return padded
    
    def execute(self, *input_tensors: torch.Tensor)-> torch.Tensor:
        """
        Perform FHE computations
        
        Args:
            *input_tensors: input tensors
            
        Returns:
            output tensors
        """
        return self._manager.run(self.backend_name, list(input_tensors))

    def validate(self, expected: torch.Tensor) -> bool:
        """Verification result"""
        return self._manager.validate(expected.to(torch.float64))

    def get_data_entry(self) -> List[Tuple[str, str, Any, int]]:
        """Return collected runtime data entries."""
        return self._data_entry

    @property
    def ctx(self):
        """Expose C++ Manager (Use with Caution)"""
        return self._manager
