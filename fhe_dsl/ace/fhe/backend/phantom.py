#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Phantom Backend - GPU backend using Phantom library and fhe_cmplr.
"""

import logging
import subprocess
from pathlib import Path
from typing import List, Optional

from .base import GpuBackend
from ..config.compile_options import _dict_to_cmd_args

logger = logging.getLogger(__name__)


class PhantomLIB(GpuBackend):
    """Phantom GPU backend using fhe_cmplr + nvcc compilation."""

    def __init__(self, device: str = "cuda", **kwargs):
        super().__init__(device, **kwargs)
        logger.info("BACKEND: Using Phantom as backend for build...")

    @classmethod
    def backend_name(cls) -> str:
        return "phantom"

    @classmethod
    def device_name(cls) -> str:
        return "cuda"

    @classmethod
    def supported_format_types(cls) -> List[str]:
        return ["file"]

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile ONNX model to Phantom CUDA source."""
        import os
        os.makedirs(output_dir, exist_ok=True)

        # Get input file path
        input_path = self._get_input_path(ir)
        if input_path is None:
            raise ValueError("Input file path not available for compilation")

        input_path = str(input_path)
        input_path_obj = Path(input_path)

        # Config file path
        cfg_path = Path(output_dir) / f"{input_path_obj.stem}.conf"
        cu_path = Path(output_dir) / "kernel_phantom.cu"

        # Build fhe_cmplr command
        base_cmd = [
            str(self.fhe_cmplr_path), input_path,
            "-P2C:lib=phantom",
            "-o", str(cu_path)
        ]

        # Get option args from user-provided options
        logger.info(f"[DEBUG phantom] self._options = {self._options}")
        option_args = _dict_to_cmd_args(self._options)
        logger.info(f"[DEBUG phantom] option_args = {option_args}")

        # Ensure CKKS:N is set (default to 65536 for Phantom if not specified)
        has_ckks_n = any("-CKKS:" in arg and "N=" in arg for arg in option_args)
        if not has_ckks_n:
            # Add default N=65536 for Phantom GPU execution
            option_args.append("-CKKS:N=65536")

        full_cmd = base_cmd + option_args

        # Store config path in ir for driver to access
        if ir is not None and not isinstance(ir, (str, Path)):
            ir._config_path = str(cfg_path)

        try:
            subprocess.run(
                full_cmd,
                check=True,
                capture_output=True,
                text=True,
                cwd=output_dir
            )
            logger.debug(f"[SUCCESS] Compilation succeeded: {' '.join(full_cmd)}")
        except subprocess.CalledProcessError as e:
            logger.error(f"[FAILED] Compilation failed: {' '.join(full_cmd)}")
            logger.error(f"STDERR: {e.stderr}")
            raise RuntimeError(f"fhe_cmplr compilation failed: {e.stderr}")
        except FileNotFoundError:
            logger.error(f"[ERROR] Compiler not found: {full_cmd[0]}")
            raise FileNotFoundError(f"fhe_cmplr not found at {full_cmd[0]}")

        return str(cu_path)

    def build_command(
        self,
        source: str,
        output: str,
        ace_root: Optional[str] = None,
        extra_flags: Optional[List[str]] = None
    ) -> List[str]:
        """Construct nvcc command for Phantom GPU backend."""
        logging.debug(f"Build Kernel Library: {output}")

        if ace_root is None:
            ace_root = str(self._get_ace_root())

        include_dirs = [
            f"-I{ace_root}/include",
            f"-I{ace_root}/include/lib_ant",
            f"-I{ace_root}/include/lib_phantom",
        ]

        lib_dirs = [f"-L{ace_root}/lib"]
        libs = ["-lFHErt_phantom", "-lcudart"]

        default_flags = [
            "-DUSE_GPU_BACKEND",
            "-Xlinker",
            f"-rpath={ace_root}/lib"
        ]

        return self._build_nvcc_command(
            source, output, include_dirs, lib_dirs, libs,
            extra_flags=extra_flags, default_flags=default_flags
        )