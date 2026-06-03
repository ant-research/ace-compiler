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

import os
import json
import hashlib
from pathlib import Path

import torch

from .logger import (
    # Core functions
    setup_logging,
    get_logger,
    set_log_level,
    # Convenience functions
    enable_debug_logging,
    disable_debug_logging,
    # Pre-configured loggers
    get_frontend_logger,
    get_ir_logger,
    get_torch_frontend_logger,
    get_onnx_frontend_logger,
    get_driver_logger,
    get_runtime_logger,
    # Backward compatibility
    setup_fhe_logger,
)

__all__ = [
    # Logging - Core functions
    "setup_logging",
    "get_logger",
    "set_log_level",
    # Logging - Convenience functions
    "enable_debug_logging",
    "disable_debug_logging",
    # Logging - Pre-configured loggers
    "get_frontend_logger",
    "get_ir_logger",
    "get_torch_frontend_logger",
    "get_onnx_frontend_logger",
    "get_driver_logger",
    "get_runtime_logger",
    # Logging - Backward compatibility
    "setup_fhe_logger",
    # GPU utilities
    "gpu_available",
    # Hash utilities
    "compute_build_meta_hash",
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