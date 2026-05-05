#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
OpenFHE Backend - CPU backend using OpenFHE library.
"""

import os
from pathlib import Path
from typing import List, Optional
from ..driver import Backend

import logging

logger = logging.getLogger(__name__)


class OpenFHELIB(Backend):
    """OpenFHE CPU backend using g++ compilation."""

    @classmethod
    def backend_name(cls) -> str:
        return "openfhe"

    @classmethod
    def device_name(cls) -> str:
        return "cpu"

    def __init__(self, **kwargs):
        logger.info("BACKEND: Use OpenFHE as backend for build...")
        self._options = kwargs

    def check_available(self) -> bool:
        """Check if OpenFHE library is available."""
        # TODO: Implement actual check for OpenFHE installation
        return True

    @classmethod
    def supported_format_types(cls) -> List[str]:
        """Return supported IR format types."""
        return ["air", "onnx", "torch_traced"]

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile IR to shared library using OpenFHE."""
        os.makedirs(output_dir, exist_ok=True)

        # Get format type
        if hasattr(ir, 'format_type'):
            ft = ir.format_type
            format_type = ft() if callable(ft) else ft
        else:
            format_type = None

        if format_type == "onnx":
            return self._compile_onnx(ir, output_dir)
        elif format_type == "air":
            return self._compile_air(ir, output_dir)
        elif format_type == "torch_traced":
            return self._compile_torch_traced(ir, output_dir)
        else:
            raise ValueError(f"Unsupported IR type: {format_type}")

    def _compile_onnx(self, onnx_model, output_dir: str) -> str:
        """Compile ONNX model to library."""
        from ..config.compile_options import _dict_to_cmd_args

        cpp_path = Path(output_dir) / "kernel_openfhe_onnx.cpp"

        base_cmd = [
            "/root/build/driver/fhe_cmplr", onnx_model.onnx_path,
            "-o", cpp_path,
            "-BACKEND=openfhe"
        ]
        option_args = _dict_to_cmd_args(self._options)
        full_cmd = base_cmd + option_args

        try:
            import subprocess
            subprocess.run(full_cmd, check=True, capture_output=True, text=True)
            logger.info(f"OpenFHE compilation succeeded: {' '.join(full_cmd)}")
        except subprocess.CalledProcessError as e:
            logger.error(f"OpenFHE compilation failed: {e.stderr}")
        except FileNotFoundError:
            logger.error(f"Compiler not found: {full_cmd[0]}")

        return str(cpp_path)

    def _compile_air(self, air_program, output_dir: str) -> str:
        """Compile AIR IR to library."""
        # TODO: Implement AIR compilation for OpenFHE
        raise NotImplementedError("AIR compilation for OpenFHE not yet implemented")

    def _compile_torch_traced(self, traced_model, output_dir: str) -> str:
        """Compile Torch traced model to library."""
        import torch

        logger.info("Executing Torch traced model for OpenFHE backend...")

        # Create dummy inputs for traced model execution
        dummy_inputs = []
        for shape in traced_model._input_shapes:
            dummy_inputs.append(torch.zeros(shape))

        # Execute the traced model to generate AIR IR
        traced_model.execute(*dummy_inputs)

        logger.info("Torch traced model executed successfully.")

        # Write AIR IR to file
        traced_model.write_ir(str(Path(output_dir) / "torch_model.air"))

        # TODO: Implement OpenFHE-specific compilation from AIR
        raise NotImplementedError("OpenFHE compilation from torch_traced not yet implemented")

    def build_command(
        self,
        source: str,
        output: str,
        ace_root: Optional[str] = None,
        extra_flags: Optional[List[str]] = None
    ) -> list[str]:
        """Construct g++ command for OpenFHE backend."""
        logging.debug(f"Build Kernel Library (OpenFHE): {output}")

        include_dirs = []
        lib_dirs = []
        libs = ["-lOPENFHEcore", "-lOPENFHEpke", "-lOPENFHEbinfhe"]

        if ace_root is None:
            import sysconfig
            platlib = sysconfig.get_path("platlib")
            ace_root = os.path.join(platlib, "ace")

            include_dirs.extend([
                f"-I{ace_root}/include/openfhe",
                f"-I{ace_root}/include/openfhe/core",
                f"-I{ace_root}/include/openfhe/pke",
            ])

            lib_dirs.append(f"-L{ace_root}/../rtlib/lib")

        cmd = [
            "g++", "-std=c++17", "-O2", "-shared", "-fPIC",
            "-o", output, source,
            *include_dirs,
            *lib_dirs,
            *libs
        ]

        if extra_flags:
            cmd.extend(extra_flags)
        else:
            cmd.append("-DUSE_OPENFHE_BACKEND")
            cmd.append(f"-Wl,-rpath,{ace_root}/../rtlib/lib")

        return cmd