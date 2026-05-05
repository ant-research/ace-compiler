#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import logging
from pathlib import Path
from typing import Callable, List, Any, Dict, Optional
import torch
from .registry import get_frontend, get_library_impl
from .builder import FHELibraryBuilder

logger = logging.getLogger(__name__)



class Driver:
    """
    Unified compiler that delegates to registered backends.

    Supports multiple compilation strategies while maintaining a single interface.
    """

    def __init__(
            self,
            frontend: str,
            library: str,
            device: str = "cpu",
            options: Any = None,
            **kwargs
    ):
        """
        Initialize compiler with specified library.

        Args:
            frontend: Frontend name (torch/ast/onnx)
            library: Library name (antlib/phantom/hyperfhe/seal)
            device: Device name (cpu/cuda)
            options: CompileOptions or ComputeOptions instance
            **kwargs: Library configuration (ckks/vec/sihe/p2c etc.)
        """
        self.func = None
        self.input_names = None

        # Build library config from options
        self.library_config = {"device": device}
        if options is not None:
            # Use to_compiler_options() method from BaseOption
            compiler_options = options.to_compiler_options()
            self.library_config.update(compiler_options)

        # Frontends no longer accept constructor arguments
        self.frontend_impl = get_frontend(frontend)
        self.backend_impl = FHELibraryBuilder(library, **self.library_config)

    def compile(self, source, input_tensors, input_names=None):
        from ..cache import (
            compile_with_cache, generate_cache_key, get_cache_dir, _get_project_root,
            _set_project_root, get_input_tensors_hash, get_full_config_hash, get_cache_path
        )

        self.func = source
        self.input_names = input_names

        # Get parameters
        device = getattr(self.backend_impl, 'device', 'cpu')
        library = self.backend_impl.library
        frontend = self.frontend_impl.name() if hasattr(self.frontend_impl, 'name') else 'torch'

        # Generate Level 1 cache key (entity-frontend-library-device)
        cache_key = generate_cache_key(source, frontend, library, device)

        # Get build options from backend (for Level 3 hash)
        build_options = self._get_build_options()

        # Set build_dir to the full config path (Level 3) so all compilation outputs land there
        _set_project_root(Path.cwd().resolve())
        config_path = get_cache_path(cache_key, input_tensors, self.library_config, build_options)
        config_path.mkdir(parents=True, exist_ok=True)
        self.backend_impl.build_dir = config_path
        self.backend_impl.so_file = config_path / f"kernel.so"

        # Define actual compilation
        def do_compile():
            build_dir = getattr(self.backend_impl, 'build_dir', None)
            frontend_kwargs = {'library': library, 'device': device}
            if build_dir is not None:
                # Convert Path to str for frontend compatibility
                frontend_kwargs['build_dir'] = str(build_dir)
            self.ir = self.frontend_impl.to_ir(source, input_tensors, input_names, **frontend_kwargs)
            self.so_file = self.backend_impl.build(ir=self.ir)
            self._package = self._build_package(input_tensors)
            return self._package

        # Use cache wrapper with three-level cache
        self._package = compile_with_cache(
            cache_key=cache_key,
            compile_options=self.library_config,
            build_options=build_options,
            input_tensors=input_tensors,
            compile_func=do_compile
        )
        return self._package

    def _get_build_options(self) -> dict:
        """Get build options from backend for cache key generation."""
        # Get optimization level and other build options from backend
        # Default to empty dict - can be extended to capture actual build flags
        return {
            "optimization_level": "O2",  # Default, can be extended
        }

    def export(self, input_tensors, input_names=None, format="air", output_path="exported.ir", source=None):
        """
        Export frontend IR to file without full compilation.

        Delegates to the frontend's export method for consistency.

        Args:
            input_tensors: List of input tensors
            input_names: List of input names
            format: Output format - "air" or "onnx"
            output_path: Output file path
            source: Source model/function to export (optional, uses stored func if not provided)

        Returns:
            Path to exported file
        """
        # Use provided source or fall back to stored func
        source_to_export = source if source is not None else self.func
        self.input_names = input_names

        # Get device from backend_impl
        device = getattr(self.backend_impl, 'device', 'cpu')
        library = self.backend_impl.library

        # Delegate to frontend's export method for consistency
        return self.frontend_impl.export(
            source_to_export,
            input_tensors,
            input_names,
            format=format,
            output_path=output_path
        )


    def _build_package(self, input_tensors: List[torch.Tensor]) -> Dict[str, Any]:
        """Generate package metadata from input tensors and function signature."""
        if len(input_tensors) != len(self.input_names):
            raise ValueError(
                f"Expected {len(self.input_names)} input tensors, got {len(input_tensors)}"
            )

        input_info = []
        for name, tensor in zip(self.input_names, input_tensors):
            input_info.append({
                "name": name,
                "shape": list(tensor.shape),
                "type": f"tensor({self._get_tensor_dtype(tensor)})",
            })

        # Run the original function to infer output (if callable)
        if callable(self.func):
            output_tensors = self.func(*input_tensors)
            if not isinstance(output_tensors, (list, tuple)):
                output_tensors = [output_tensors]
        else:
            # For file-based frontends (ONNX), we don't have a callable
            # Output info will be inferred at runtime
            output_tensors = None

        output_info = []
        if output_tensors is not None:
            for i, out in enumerate(output_tensors):
                output_info.append({
                    "name": "output",
                    "shape": list(out.shape),
                    "type": f"tensor({self._get_tensor_dtype(out)})",
                })

        package = {
            "model": self.ir.entry_name,
            "kernel": str(self.so_file),
            "input_info": input_info,
            "output_info": output_info,
        }

        # Include config path if available (for ONNX-based compilation)
        if hasattr(self.ir, '_config_path'):
            package["config_path"] = self.ir._config_path

        logger.debug("Package built: model=%s, kernel=%s, input_info=%s, output_info=%s, config_path=%s",
                     package.get("model"), package.get("kernel"), package.get("input_info"),
                     package.get("output_info"), package.get("config_path"))

        return package

    def _get_tensor_dtype(self, tensor: torch.Tensor) -> str:
        """Map PyTorch dtype to string representation."""
        dtype_map = {
            torch.float32: "float",
            torch.float64: "double",
            torch.int32: "int32",
            torch.int64: "int64",
        }
        return dtype_map.get(tensor.dtype, "float")