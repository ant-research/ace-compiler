#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Backend Base Module

Provides base classes and utilities for FHE backend implementations.
"""

import os
import subprocess
import logging
from pathlib import Path
from typing import List, Optional

from ..driver import Backend
from ..config.compile_options import _dict_to_cmd_args

logger = logging.getLogger(__name__)


class FheCmplrBackend(Backend):
    """Base class for backends using fhe_cmplr compiler.

    This class provides common functionality for backends that:
    1. Use fhe_cmplr to compile IR to source code (.cpp or .cu)
    2. Use a native compiler (g++/nvcc) to build shared libraries

    Subclasses should override:
    - backend_name(), device_name(): Backend identification
    - supported_format_types(): Supported IR formats
    - _get_compiler_command(): fhe_cmplr command construction
    - build_command(): Native compiler command construction
    """

    def __init__(self, device: str = "cpu", **kwargs):
        self.device = device
        self._options = kwargs

    @staticmethod
    def _get_ace_root() -> Path:
        """Get the installed ace root directory."""
        import sysconfig
        platlib = sysconfig.get_path("platlib")
        return Path(platlib) / "ace"

    @property
    def fhe_cmplr_path(self) -> Path:
        """Get the installed fhe_cmplr path."""
        return self._get_ace_root() / "bin" / "fhe_cmplr"

    def _get_input_path(self, ir) -> Optional[str]:
        """Extract input file path from various IR object types."""
        if hasattr(ir, 'file_path'):
            return ir.file_path
        elif hasattr(ir, '_file_path'):
            return ir._file_path
        elif hasattr(ir, 'onnx_path'):
            return ir.onnx_path
        elif hasattr(ir, '_onnx_path'):
            return ir._onnx_path
        elif isinstance(ir, str):
            return ir
        elif isinstance(ir, Path):
            return str(ir)
        return None

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile IR to source code using fhe_cmplr.

        Args:
            ir: IR object or file path
            output_dir: Output directory for generated files

        Returns:
            Path to generated source file (.cpp or .cu)

        Raises:
            ValueError: If input file path not available
            RuntimeError: If fhe_cmplr compilation fails
        """
        os.makedirs(output_dir, exist_ok=True)

        # Get input file path
        input_path = self._get_input_path(ir)
        if input_path is None:
            raise ValueError("Input file path not available for compilation")

        input_path = str(input_path)
        input_path_obj = Path(input_path)

        # Determine output file extension and path
        source_ext = self._get_source_extension()
        import time
        unique_id = f"{int(time.time() * 1000000) % 1000000:06d}"
        source_filename = f"{self._get_source_prefix()}_{unique_id}.{source_ext}"
        source_path = Path(output_dir) / source_filename

        # Remove existing file to ensure fhe_cmplr creates a new one
        if source_path.exists():
            source_path.unlink()

        # Config file path
        cfg_path = Path(output_dir) / f"{input_path_obj.stem}.conf"

        # Build fhe_cmplr command
        full_cmd = self._get_compiler_command(input_path, str(source_path), cfg_path)

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

        return str(source_path)

    def _get_source_extension(self) -> str:
        """Get source file extension (cpp or cu)."""
        return "cpp"

    def _get_source_prefix(self) -> str:
        """Get source file prefix."""
        return f"fhe_{self.backend_name()}"

    def _get_compiler_command(
        self,
        input_path: str,
        source_path: str,
        cfg_path: Path
    ) -> List[str]:
        """Build fhe_cmplr command.

        Args:
            input_path: Input file path (.onnx or .B)
            source_path: Output source file path
            cfg_path: Config file path

        Returns:
            Command list for subprocess.run()
        """
        raise NotImplementedError("Subclasses must implement _get_compiler_command()")

    def check_available(self) -> bool:
        """Check if fhe_cmplr is available."""
        try:
            result = subprocess.run(
                [str(self.fhe_cmplr_path), "-h"],
                capture_output=True, text=True
            )
            return result.returncode == 0
        except FileNotFoundError:
            return False


class CpuBackend(FheCmplrBackend):
    """Base class for CPU backends using g++.

    Provides common g++ command construction for CPU backends.
    """

    def __init__(self, device: str = "cpu", **kwargs):
        super().__init__(device, **kwargs)

    def _get_source_extension(self) -> str:
        return "cpp"

    def _get_source_prefix(self) -> str:
        return f"fhe_{self.backend_name()}"

    def _build_gpp_command(
        self,
        source: str,
        output: str,
        include_dirs: List[str],
        lib_dirs: List[str],
        libs: List[str],
        extra_flags: Optional[List[str]] = None,
        default_flags: Optional[List[str]] = None
    ) -> List[str]:
        """Construct g++ command.

        Args:
            source: Source file path
            output: Output shared library path
            include_dirs: Include directory flags (-I...)
            lib_dirs: Library directory flags (-L...)
            libs: Library flags (-l...)
            extra_flags: Additional flags (used if provided)
            default_flags: Default flags (used if extra_flags not provided)

        Returns:
            Complete g++ command list
        """
        cmd = [
            "g++", "-std=c++17", "-O2", "-shared", "-fPIC",
            "-o", output, source,
            *include_dirs,
            *lib_dirs,
            *libs
        ]
        if extra_flags:
            cmd.extend(extra_flags)
        elif default_flags:
            cmd.extend(default_flags)
        else:
            cmd.append("-DUSE_CPU_BACKEND")

        return cmd


class GpuBackend(FheCmplrBackend):
    """Base class for GPU backends using nvcc.

    Provides common nvcc command construction for GPU backends.
    """

    def __init__(self, device: str = "cuda", **kwargs):
        super().__init__(device, **kwargs)

    def _get_source_extension(self) -> str:
        return "cu"

    def _get_source_prefix(self) -> str:
        return f"kernel_{self.backend_name()}"

    def check_available(self) -> bool:
        """Check if nvcc and fhe_cmplr are available."""
        try:
            nvcc = subprocess.run(["nvcc", "--version"], capture_output=True, text=True)
            cmplr = subprocess.run(
                [str(self.fhe_cmplr_path), "-h"],
                capture_output=True, text=True
            )
            return nvcc.returncode == 0 and cmplr.returncode == 0
        except FileNotFoundError:
            return False

    def _build_nvcc_command(
        self,
        source: str,
        output: str,
        include_dirs: List[str],
        lib_dirs: List[str],
        libs: List[str],
        extra_flags: Optional[List[str]] = None,
        default_flags: Optional[List[str]] = None
    ) -> List[str]:
        """Construct nvcc command.

        Args:
            source: Source file path
            output: Output shared library path
            include_dirs: Include directory flags (-I...)
            lib_dirs: Library directory flags (-L...)
            libs: Library flags (-l...)
            extra_flags: Additional flags (used if provided)
            default_flags: Default flags (used if extra_flags not provided)

        Returns:
            Complete nvcc command list
        """
        cmd = [
            "nvcc", "-shared", "-Xcompiler", "-fPIC,-O3",
            "--std=c++17", "--expt-relaxed-constexpr",
            "-o", output, source,
            *include_dirs,
            *lib_dirs,
            *libs
        ]
        if extra_flags:
            cmd.extend(extra_flags)
        elif default_flags:
            cmd.extend(default_flags)
        else:
            cmd.append("-DUSE_GPU_BACKEND")

        return cmd