#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#


from typing import Callable, Dict, List, Any, Optional
import logging
import torch

logger = logging.getLogger(__name__)

from .config_loader import FHEConfigLoader
from .fhe_program import FHEProgram

VERIFY_MODES = {"array", "tensor"}


class FHERuntime:
    """
    FHE JIT scheduler - responsible for high-level scheduling, configuration management, and multi-backend support
    """
    
    def __init__(
        self,
        package: Dict[str, Any],
        verify: str = "array"
        ):
        """
        Args:
            package: Model package configuration dictionary
            verify: Validation mode ('array' or' tensor')
        """
        if verify not in VERIFY_MODES:
            raise ValueError(f"Invalid verify mode: {verify}. Expected one of {VERIFY_MODES}")

        self.package = package
        self.verify_mode = verify
        self.programs: Dict[str, FHEProgram] = {}

        # Try to get config path from package, or derive from model name
        config_path = package.get('config_path', f"{package['model']}.conf")
        self.config_loader = FHEConfigLoader(config_path)

        self._register_default_backends()


    def _register_default_backends(self):
        """
        Register the default backend. Currently, only one backend JIT operation is supported.
        """
        self.register_program("kernel", f"{self.package['kernel']}") # ./build_ops/libantlib_cpu.so")

    def register_program(self, name: str, lib_path: str):
        """
        Register the FHE Kernel program
        
        Args:
            name: Kernel Name
            lib_path: Path of the Kernel dynamic library
        """
        program = FHEProgram(name, lib_path, self.package)
        self.programs[name] = program
    
    def to_4d(self, tensor: torch.Tensor) -> torch.Tensor:
        return tensor if tensor.dim() == 4 else tensor.unsqueeze(1).unsqueeze(2)


    def inference(self, *input_tensors: torch.Tensor, backend: str = "kernel") -> torch.Tensor:
        """
        Perform Inference
        
        Args:
            *input_tensors: Input tensors
            backend: Backend Kernel name
            
        Returns:
            Inference result
        """
        if backend not in self.programs:
            available = list(self.programs.keys())
            raise ValueError(f"Backend '{backend}' not registered. Available: {available}")
        
        input_tensors_4d = [self.to_4d(t) for t in input_tensors]
        return self.programs[backend].execute(*input_tensors_4d)
    
    def validate(self) -> bool:
        """
        Verification result

        Returns:
            Verify whether it has passed.
        """
        # Get expected output from package if available
        expected = self.package.get('expected_output')

        if expected is None:
            # No expected output stored, skip validation
            logger.warning("No expected output found in package, skipping validation")
            return True

        if self.verify_mode == "array":
            # Use default backend validation
            default_backend = next(iter(self.programs.keys()))
            return self.programs[default_backend].validate(expected)
        else:
            # tensor schema validation (decrypt logic required)
            raise NotImplementedError("Tensor validation mode not implemented yet")
        

    def get_data_entry(self) -> List[tuple]:
        """Gets the data entries for all programs"""
        all_entries = []
        for program in self.programs.values():
            all_entries.extend(program.get_data_entry())
        return all_entries
    
    @property
    def ctx(self):
        """Exposes the default backend C++ context (backward compatible)"""
        if self.programs:
            return self.programs[next(iter(self.programs.keys()))].manager
        return None