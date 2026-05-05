#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import logging
import subprocess
from typing import List, Optional
from ..driver import Backend

logger = logging.getLogger(__name__)

class HyperfheLIB(Backend):
    """Phantom Library using cuda compilation."""

    @classmethod
    def backend_name(cls) -> str:
        return "hyperfhe"

    @classmethod
    def device_name(cls) -> str:
        return "cuda"

    def __init__(self, device: str = "cuda", **kwargs):
        self.device = device
        self._options = kwargs
        logger.info(f"BACKEND : Use HYPERFHE as backend for build...")

    @classmethod
    def supported_format_types(cls) -> List[str]:
        """Return supported IR format types."""
        return ["air", "onnx"]

    def compile_to_lib(self, ir, output_dir: str) -> str:
        """Compile IR to library."""
        raise NotImplementedError("Hyperfhe compilation not yet implemented")

    def check_available(self) -> bool:
        try:
            result = subprocess.run(["nvcc", "--version"], capture_output=True, text=True)
            return result.returncode == 0
        except FileNotFoundError:
            return False

    def build_command(
        self,
        source: str,
        output: str,
        ace_root: Optional[str],
        extra_flags: Optional[List[str]]
    ) -> List[str]:
        
        logging.debug(f"Build Kernel Library : {output}")

        include_dirs = []
        lib_dirs = []
        libs = ["-lFHErt_hyperfhe", "-arch=sm_90", "-lcudart"] # "-lFHErt_common",
        if ace_root is None:
            import os
            import sysconfig
            platlib = sysconfig.get_path("platlib")
            ace_root = os.path.join(platlib, "ace")

            #include_dirs.extend([
            #    #f"-I{ace_root}/include",
            #    f"-I{ace_root}/include",
            #    f"-I{ace_root}/include/lib_ant",
            #    f"-I{ace_root}/include/lib_hyperfhe",
            #    f"-I{ace_root}/include/lib_hyperfhe/public",
            #    f"-I{ace_root}/include/lib_hyperfhe/include",
            #    f"-I{ace_root}/../rapids_logger/include",
            #])

            include_dirs.extend([
              f"-I/usr/local/lib/python3.10/dist-packages/rapids_logger/include",
              f"-I/app/ace/fhe_lib/hyperfhe/include",
              f"-I/app/ace/build/_deps/rmm-src/cpp/include",
              f"-I/app/ace/build/_deps/rmm-build/include",
              f"-I/app/ace/build/_deps/ckks_infra-src/ckks-app/src",
              f"-I/app/ace/build/_deps/ckks_infra-src/ckks-app/src/common",
              f"-I/app/ace/build/_deps/ckks_infra-src/ckks-gpu/include/public",
              f"-I/app/ace/build/_deps/ckks_infra-build/include/public",
              f"-I/app/ace/compiler/air-infra/include",
              f"-I/app/ace/compiler/nn-addon/include",
              f"-I/app/ace/compiler/fhe-cmplr/include",
              f"-I/app/ace/compiler/fhe-cmplr/rtlib/include",
              f"-I/app/ace/compiler/fhe-cmplr/rtlib/ant/include",
              f"-I/app/ace/build/rtlib/build/_deps/uthash-src/src",
              f"-I/app/ace/build/include",
              f"-I/app/ace/build/_deps/ckks_infra-src/ckks-app/src/nn",
              f"-I/app/ace/build/fhe_lib/hyperfhe/ckks_gpu/include/public",
              f"-I/app/ace/build/_deps/cccl-src/lib/cmake/thrust/../../../thrust",
              f"-I/app/ace/build/_deps/cccl-src/lib/cmake/libcudacxx/../../../libcudacxx/include",
              f"-I/app/ace/build/_deps/cccl-src/lib/cmake/cub/../../../cub",
              f"-I/app/ace/build/_deps/cccl-src/cub/cmake/cub",
              f"-I/usr/local/lib/python3.10/dist-packages/ace/include",
              f"-I/usr/local/lib/python3.10/dist-packages/ace/include/lib_hyperfhe",
            ])

            lib_dirs.append(f"-L{ace_root}/lib")
            #lib_dirs.append(f"-L{ace_root}/rtlib/lib")

        # for test
        source = "build_ops/kernel_hyperfhe.cu"

        cmd = [
            "nvcc", "-shared", "-Xcompiler", "-fPIC,-O3",
            "--std=c++17", "--expt-relaxed-constexpr",
            "-o", output, source,
            *include_dirs,
            *(extra_flags or []),
            *lib_dirs,
            *libs
        ]

        if extra_flags:
            cmd.extend(extra_flags)
        else:
            cmd.append("-DGPU_BACKEND")
            cmd.append("-DHYPER_BTS_MACRO")
            cmd.append("-DLIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE")
            cmd.append("-Xlinker")
            cmd.append(f"-rpath={ace_root}/lib")
        return cmd
