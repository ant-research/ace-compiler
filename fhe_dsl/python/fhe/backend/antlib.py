#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AntLIB Backend - CPU backend using RTLIB and fhe_cmplr.
"""

import logging
import os
import subprocess
from pathlib import Path
from typing import List, Optional

from .base import CpuBackend
from ..config.compile_options import _dict_to_cmd_args

logger = logging.getLogger(__name__)


class AntLIB(CpuBackend):
    """RTLIB CPU backend using fhe_cmplr + g++ compilation."""

    def __init__(self, device: str = "cpu", **kwargs):
        super().__init__(device, **kwargs)
        logger.info("BACKEND: Using RTLIB as backend for build...")

    @classmethod
    def backend_name(cls) -> str:
        return "antlib"

    @classmethod
    def device_name(cls) -> str:
        return "cpu"

    @classmethod
    def supported_format_types(cls) -> List[str]:
        """Support file input (.onnx or .B) and memory IR."""
        return ["memory", "file"]

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile IR to C++ code using fhe_cmplr.

        Handles both file and memory IR formats.
        """
        # Handle string or Path directly as file input
        if isinstance(ir, (str, Path)):
            return self._compile_file(ir, output_dir)

        # Get format type from IR object
        if hasattr(ir, 'format_type'):
            ft = ir.format_type
            format_type = ft() if callable(ft) else ft
        else:
            format_type = None

        if format_type == "file":
            return self._compile_file(ir, output_dir)
        elif format_type == "memory":
            raise NotImplementedError("Memory IR compilation not yet implemented")
        else:
            raise ValueError(f"Unsupported IR type: {format_type}")

    def _compile_file(self, ir, output_dir: str) -> str:
        """Compile file input (.onnx or .B) to C++ code."""
        os.makedirs(output_dir, exist_ok=True)

        # Get input file path
        input_path = self._get_input_path(ir)
        if input_path is None:
            raise ValueError("Input file path not available for compilation")

        input_path = str(input_path)
        input_path_obj = Path(input_path)

        # Config file path
        cfg_path = Path(output_dir) / f"{input_path_obj.stem}.conf"
        weight_path = Path(output_dir) / "data.weight"

        # Build fhe_cmplr command
        base_cmd = [
            str(self.fhe_cmplr_path), input_path,
            f"-P2C:df={weight_path}",
            "-o", str(Path(output_dir) / f"fhe_{self.backend_name()}.cpp")
        ]
        option_args = _dict_to_cmd_args(self._options)
        full_cmd = base_cmd + option_args

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

        # Store config path in ir for driver to access
        if ir is not None and not isinstance(ir, (str, Path)):
            ir._config_path = str(cfg_path)

        # Return generated cpp path
        return str(Path(output_dir) / f"fhe_{self.backend_name()}.cpp")

    def _get_compiler_command(
        self,
        input_path: str,
        source_path: str,
        cfg_path: Path
    ) -> List[str]:
        """Build fhe_cmplr command for AntLIB."""
        raise NotImplementedError("Should not be called directly")

    def build_command(
        self,
        source: str,
        output: str,
        ace_root: Optional[str] = None,
        extra_flags: Optional[List[str]] = None
    ) -> List[str]:
        """Construct g++ command for AntLIB CPU backend."""
        logging.debug(f"Build Kernel Library: {output}")

        if ace_root is None:
            ace_root = str(self._get_ace_root())

        include_dirs = [
            f"-I{ace_root}/include",
            f"-I{ace_root}/include/rt_ant",
            f"-I{ace_root}/include/ant",
        ]

        lib_dirs = [f"-L{ace_root}/lib"]
        libs = ["-lFHErt_ant"]

        default_flags = [
            "-DUSE_CPU_BACKEND",
            f"-Wl,-rpath,{ace_root}/lib"
        ]

        return self._build_gpp_command(
            source, output, include_dirs, lib_dirs, libs,
            extra_flags=extra_flags, default_flags=default_flags
        )