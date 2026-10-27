#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Utility modules for ACE FHE.

This package provides:
- Logging utilities (logger.py)
- Temp workspace management (temp_workspace.py)
- GPU availability check
"""

import hashlib
import json
import os
from pathlib import Path

import torch

from .logger import (
    disable_debug_logging,
    # Convenience functions
    enable_debug_logging,
    get_driver_logger,
    # Pre-configured loggers
    get_frontend_logger,
    get_ir_logger,
    get_logger,
    get_onnx_frontend_logger,
    get_runtime_logger,
    get_torch_frontend_logger,
    set_log_level,
    # Backward compatibility
    setup_fhe_logger,
    # Core functions
    setup_logging,
)


__all__ = [
    # Hash utilities
    "compute_build_meta_hash",
    "disable_debug_logging",
    # Logging - Convenience functions
    "enable_debug_logging",
    "get_driver_logger",
    # Logging - Pre-configured loggers
    "get_frontend_logger",
    "get_ir_logger",
    "get_logger",
    "get_onnx_frontend_logger",
    "get_runtime_logger",
    "get_torch_frontend_logger",
    # GPU utilities
    "gpu_available",
    "set_log_level",
    # Logging - Backward compatibility
    "setup_fhe_logger",
    # Logging - Core functions
    "setup_logging",
]


def gpu_available():
    """Check if GPU is available."""
    return torch.cuda.is_available()


def compute_build_meta_hash(ctx: dict) -> str:
    """Compute a short hash from build context for build_meta.json naming."""
    normalized = ctx.copy()
    normalized["source_mtime"] = os.path.getmtime(ctx["source_file"])
    data = json.dumps(normalized, sort_keys=True, default=str)
    return hashlib.sha256(data.encode()).hexdigest()[:16]
