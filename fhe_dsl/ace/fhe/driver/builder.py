#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import os
import re
import subprocess
import json
import logging
from pathlib import Path
from typing import Dict, Callable, List, Optional, Tuple
from ..util import compute_build_meta_hash

from ..driver.registry import get_library_impl, list_supported_combos
from ..ir import CompilationUnit


logger = logging.getLogger(__name__)

class FHELibraryBuilder:
    """
    Builds FHE kernels into shared libraries for multiple libraries and backends.

    Supports combinations like: (antlib, cpu), (antlib, gpu), (sealib, cpu), etc.
    """

    # Default values
    DEFAULT_LIBRARY = "antlib"
    DEFAULT_DEVICE = "cpu"

    def __init__(self, library: str = DEFAULT_LIBRARY, test_name: str = None,
                 build_dir: Path = None, **kwargs):
        self.library = library
        self.device = kwargs.get('device', FHELibraryBuilder.DEFAULT_DEVICE)
        self.test_name = test_name  # Optional test name for organizing output

        # Store provided build_dir, will be used in build() method
        # If not provided, will be set lazily to avoid creating /tmp/ace-temp
        self._provided_build_dir = build_dir

        # Get library implementation (validates combo)
        try:
            self.strategy = get_library_impl(library, **kwargs)
        except ValueError as e:
            raise e

    def _sanitize_name(self, name: str) -> str:
        """Convert name to safe filename (alphanumeric + underscore)."""
        return re.sub(r'[^a-zA-Z0-9_]', '_', name).lower()

    def _ensure_build_dir(self) -> Path:
        """Ensure build_dir is set. Driver should always provide it."""
        if not hasattr(self, 'build_dir') or self.build_dir is None:
            if self._provided_build_dir:
                self.build_dir = self._provided_build_dir
            else:
                raise RuntimeError("build_dir must be provided by driver")
        return self.build_dir

    def build(
        self,
        ir: CompilationUnit,
        # output_file: str = "libfhe_kernel.so",
        ace_root: Optional[str] = None,
        extra_flags: Optional[List[str]] = None
    ) -> str:
        """Build for specified library and backend."""
        # if not os.path.isfile(source_file):
        #     raise FileNotFoundError(f"Source file not found: {source_file}")

        # Ensure build_dir is set (lazy initialization)
        build_dir = self._ensure_build_dir()

        # Use unique .so filename for each compilation to avoid conflicts
        import time
        lib_name = self._sanitize_name(self.library)
        device_name = self._sanitize_name(self.device)
        unique_id = f"{int(time.time() * 1000000) % 1000000:06d}"
        self.so_file = build_dir / f"lib{lib_name}_{device_name}_{unique_id}.so"

        # to C - use build_dir for all intermediate files (.cpp, .conf, .B, .so)
        output_dir = str(build_dir)
        source_file = self.strategy.compile_to_lib(ir, output_dir)

        # Build context metadata
        build_ctx = {
            "source_file": os.path.abspath(source_file),
            "backend": self.library,
            "device": self.device,
            "ace_root": os.path.abspath(ace_root) if ace_root else None,
            "extra_flags": extra_flags or [],
            "output_file": os.path.abspath(self.so_file)
        }

        meta_hash = compute_build_meta_hash(build_ctx)
        meta_file = build_dir / f"build_meta_{meta_hash}.json"
        output_path = Path(self.so_file)

        # Build command
        try:
            cmd = self.strategy.build_command(source_file, str(output_path), ace_root, extra_flags)
        except Exception as e:
            logger.error(f"[ERROR] Command construction failed: {e}")
            #return False

        logger.debug(f"Build command: {' '.join(cmd)}")
        success = self._run_build(cmd, f"{self.library.upper()}-{self.library.upper()}")
        if not success:
            raise RuntimeError(f"Build failed for {output_path}. Check logs for details.")

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(meta_file, 'w') as f:
            json.dump(build_ctx, f, indent=2, sort_keys=True)

        return output_path

    # ======================
    # Execution
    # ======================
    def _run_build(self, cmd: List[str], label: str) -> bool:
        """Execute build command with unified error handling."""
        try:
            subprocess.run(cmd, check=True, capture_output=True, text=True)
            logger.info(f"[SUCCESS] {label} build succeeded.")
            return True
        except subprocess.CalledProcessError as e:
            logger.error(f"[FAILED] {label} build failed:")
            logger.error(f"Command: {' '.join(cmd)}")
            logger.error(f"STDERR: {e.stderr}")
            return False
        except FileNotFoundError:
            compiler = cmd[0]
            logger.error(f"[ERROR] Compiler '{compiler}' not found. Install build tools for {label}.")
            return False