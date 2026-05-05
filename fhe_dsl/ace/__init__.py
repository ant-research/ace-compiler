"""
ANT-ACE: FHE Compiler & Runtime (Multi-library)

Domain-specific compiler for Fully Homomorphic Encryption.
Transforms ML models into encrypted computation programs.
Supports multiple FHE libraries: antlib, phantom, hyperfhe.
"""

# ============================================================================
# Standard Library
# ============================================================================

import ctypes
import os
import sysconfig

# ============================================================================
# Version
# ============================================================================

from ._version import __version__

# ============================================================================
# Pre-load libFHErt_common.so with RTLD_GLOBAL (required for runtime import)
# ============================================================================


def _preload_rtlib_common():
    """Pre-load libFHErt_common.so with RTLD_GLOBAL to allow undefined symbols.

    libFHErt_common.so has undefined symbols (like Main_graph) that are
    provided by user-compiled kernel libraries at runtime. Loading with
    RTLD_GLOBAL allows these symbols to be resolved later.
    """
    try:
        site_packages = sysconfig.get_path('purelib')
        lib_path = os.path.join(site_packages, "ace", "lib", "libFHErt_common.so")
        if os.path.exists(lib_path):
            ctypes.CDLL(lib_path, mode=ctypes.RTLD_GLOBAL)
    except Exception:
        pass  # If preloading fails, let the normal import handle it


# Pre-load before importing runtime
_preload_rtlib_common()

# ============================================================================
# Core Modules (FHE domain) - Top-down architecture
# ============================================================================

# 1. Decorators
try:
    from .fhe.decorators import compile, compute, export
except ImportError:
    compile = None
    compute = None
    export = None

# 2. Frontend  (C++ extension)
try:
    from .fhe import frontend
except ImportError:
    frontend = None

HAS_FRONTEND = frontend is not None

# 3. Library
try:
    from .fhe import backend
except ImportError:
    backend = None

# 4. Driver
try:
    from .fhe import driver
except ImportError:
    driver = None

# 5. Runtime (C++ extension)
try:
    from . import runtime
except ImportError:
    runtime = None

HAS_RUNTIME = runtime is not None

# 6. Utility Functions
try:
    from .fhe.util import gpu_available
except ImportError:
    gpu_available = None

# ============================================================================
# Optional Dependencies
# ============================================================================

try:
    import torch
    import torch.nn as nn
    TORCH_AVAILABLE = True
except ImportError:
    TORCH_AVAILABLE = False
    torch = None
    nn = None

try:
    import onnx
    ONNX_AVAILABLE = True
except ImportError:
    ONNX_AVAILABLE = False
    onnx = None

# ============================================================================
# Public API
# ============================================================================

__all__ = [
    "__version__",
    # 1. Decorators
    "compile",
    "compute",
    "export",
    # 2. Submodules(Frontend, Backend, Driver, Runtime)
    "fhe",
    "frontend",
    "backend",
    "driver",
    "runtime",
    # Extension availability
    "HAS_FRONTEND",
    "HAS_RUNTIME",
    # 3. Utility functions
    "gpu_available",
    # Optional dependencies (user-facing only)
    "TORCH_AVAILABLE",
    "ONNX_AVAILABLE",
]