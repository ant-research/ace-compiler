#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import logging
import os
from pathlib import Path
from typing import Optional

import torch

from ..driver import Backend


logger = logging.getLogger(__name__)


class SealLIB(Backend):
    """Seallib CPU backend using g++ compilation."""

    implemented = False

    @classmethod
    def backend_name(cls) -> str:
        return "seal"

    @classmethod
    def device_name(cls) -> str:
        return "cpu"

    def __init__(self, device: str = "cpu", **kwargs):
        self.device = device
        self._options = kwargs
        logger.info("Library: using %s for build...", self.backend_name())

    def check_available(self) -> bool:
        return True

    @classmethod
    def supported_format_types(cls) -> list[str]:
        """Return supported IR format types."""
        return ["air", "onnx"]

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile IR to library."""
        # TODO: Implement SEAL-specific compilation
        raise NotImplementedError("SEAL compilation not yet implemented")

    def build_command(
        self, source: str, output: str, ace_root: str | None = None, extra_flags: list[str] | None = None
    ) -> list[str]:
        """Construct g++ command for CPU."""
        if ace_root is None:
            import sysconfig

            platlib = sysconfig.get_path("platlib")
            ace_root = os.path.join(platlib, "ace")

        torch_lib = Path(torch.__file__).parent / "lib"
        torch_include = torch_lib.parent / "include"

        cmd = [
            "g++",
            "-std=c++17",
            "-O2",
            "-shared",
            "-fPIC",
            "-o",
            output,
            source,
            f"-I{ace_root}/rtlib/include",
            f"-I{ace_root}/rtlib/include/ant",
            f"-I{torch_include}",
            f"-L{ace_root}/rtlib/lib",
            f"-L{torch_lib}",
            f"{ace_root}/ace_torch_ext.so",
            "-lFHErt_ant",
            "-lFHErt_common",
        ]
        if extra_flags:
            cmd.extend(extra_flags)
        else:
            cmd.append("-DUSE_CPU_BACKEND")
        return cmd
