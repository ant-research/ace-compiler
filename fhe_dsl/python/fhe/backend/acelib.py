#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Acelib Backend - GPU backend using ace-library.
"""

import logging
import os
import subprocess
from pathlib import Path
from typing import List, Optional

from .base import FheCmplrBackend

logger = logging.getLogger(__name__)


class AcelibLIB(FheCmplrBackend):
    """Acelib GPU backend using fhe_cmplr + nvcc compilation."""

    implemented = True

    def __init__(self, device: str = "cuda", **kwargs):
        super().__init__(device, **kwargs)
        logger.info("BACKEND: Use ACELIB as backend for build...")

    @classmethod
    def backend_name(cls) -> str:
        return "acelib"

    @classmethod
    def device_name(cls) -> str:
        return "cuda"

    @classmethod
    def supported_format_types(cls) -> List[str]:
        """Return supported IR format types."""
        return ["air", "onnx"]

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile ONNX model to Acelib CUDA source."""
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
        cu_path = Path(output_dir) / "kernel_acelib.cu"

        # Build fhe_cmplr command
        base_cmd = [
            str(self.fhe_cmplr_path), input_path,
            "-P2C:lib=ace_library",
            "-o", str(cu_path)
        ]

        # Get option args from user-provided options
        from ..config.compile_options import _dict_to_cmd_args
        option_args = _dict_to_cmd_args(self._options)

        # Ensure CKKS:N is set
        has_ckks_n = any("-CKKS:" in arg and "N=" in arg for arg in option_args)
        if not has_ckks_n:
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
        """Construct nvcc command for Acelib GPU backend."""
        logging.debug(f"Build Kernel Library: {output}")

        if ace_root is None:
            ace_root = str(self._get_ace_root())

        # Include directories:
        #   lib_ace       - runtime headers (ace_library_api.h, rt_ace_library/)
        #   lib_ace/public - SDK headers (Ciphertext.h, Context.h, etc.)
        #                   also RMM headers (rmm/cuda_stream_pool.hpp, etc.)
        #   include       - common headers (rt_api.h, tensor.h, etc.)
        #   lib_ace/cccl  - CCCL/libcudacxx headers bundled with RMM, needed
        #                   for namespace ABI compatibility with libFHErt_ace.so
        include_dirs = [
            f"-I{ace_root}/include/lib_ace",
            f"-I{ace_root}/include/lib_ace/public",
            f"-I{ace_root}/include/lib_ace/cccl",
            f"-I{ace_root}/include",
        ]

        lib_dirs = [f"-L{ace_root}/lib"]
        libs = ["-lFHErt_ace", "-lcudart"]

        default_flags = [
            "-DGPU_BACKEND",
            "-DHYPER_BTS_MACRO",
            "-DACE_LIBRARY_ENABLE_BTS=1",
            "-DLIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE",
            "-D__DATA_WORD_SIZE_64__",
            "-D__NTT_4STEP_MONT__",
            "-DCCCL_DISABLE_PDL",
            "-DCUB_DISABLE_NAMESPACE_MAGIC",
            "-DCUB_IGNORE_NAMESPACE_MAGIC_ERROR",
            "-DTHRUST_DISABLE_ABI_NAMESPACE",
            "-DTHRUST_IGNORE_ABI_NAMESPACE_ERROR",
            "-Xlinker",
            f"-rpath={ace_root}/lib"
        ]

        # Build nvcc command inline
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

    def check_available(self) -> bool:
        """Check if nvcc is available."""
        try:
            result = subprocess.run(["nvcc", "--version"], capture_output=True, text=True)
            return result.returncode == 0
        except FileNotFoundError:
            return False