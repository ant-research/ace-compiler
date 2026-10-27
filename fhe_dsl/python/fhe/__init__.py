#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
FHE Compilation Framework

Architecture (top-down):
    1. Decorators: @compile, @compute, @export - High-level APIs
    2. Frontend: Convert models to AIR IR (Torch, ONNX, AST)
    3. Library: FHE execution engines (ant, phantom, ace)
    4. Driver: Orchestrates frontend -> library compilation
    5. Runtime: Execute compiled FHE programs

Note:
    Custom frontend/library registration:
        from ace.fhe.frontend import register_frontend, get_frontend
        from ace.fhe.driver import register_library, get_library_impl
"""

# ============================================================================
# 1. Decorators - High-level Declarative APIs
# ============================================================================

from .decorators import compile, compute, export


# ============================================================================
# 2. Frontend - Model/Function Converters
# ============================================================================

# Frontend requires IRBuilder (C++ extension) to be available
try:
    from . import frontend
    from .ir import IRBuilder

    # Only consider frontend available if IRBuilder is available
    if IRBuilder and not IRBuilder.is_available():
        frontend = None
except ImportError:
    frontend = None

# ============================================================================
# 3. Library - Library Registration (must be before driver)
# ============================================================================

try:
    from . import backend
except ImportError:
    backend = None

# ============================================================================
# 4. Driver - Compilation Driver and Registry
# ============================================================================

try:
    from . import driver
    from .driver import Driver
except ImportError:
    driver = None
    Driver = None

# ============================================================================
# 5. Runtime - C++ Extension Module
# ============================================================================

try:
    from . import runtime
    from .runtime import CompiledProgram, FHERuntime
except ImportError:
    runtime = None
    FHERuntime = None
    CompiledProgram = None

HAS_RUNTIME = runtime is not None

# ============================================================================
# 6. Spec - Unified Compilation Input Descriptor
# ============================================================================

import warnings as _warnings  # noqa: E402


# Legacy spec system (deprecated) — suppress the warning from the spec module
# itself during import.  We emit our own deprecation warning lazily via
# __getattr__ so that merely importing ace.fhe does not produce noise.
with _warnings.catch_warnings():
    _warnings.filterwarnings("ignore", category=DeprecationWarning, message="ace.fhe.spec is deprecated")
    from .spec import (
        CompileConfig,
        CompileSpec,
        DatasetSource,
        FuncEntity,
        InputSpec,
        ModelEntity,
        RuntimeConfig,
    )

_DEPRECATED_SPEC_NAMES = {
    "CompileSpec",
    "ModelEntity",
    "FuncEntity",
    "CompileConfig",
    "InputSpec",
    "RuntimeConfig",
    "DatasetSource",
}


def __getattr__(name):
    """Emit a deprecation warning only when legacy spec names are accessed."""
    if name in _DEPRECATED_SPEC_NAMES:
        _warnings.warn(
            f"{name} is deprecated. Use ModelSpec/FuncSpec from ace.fhe.config.spec instead.",
            DeprecationWarning,
            stacklevel=2,
        )
        return globals()[name]
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")


# New spec system (preferred)
# ============================================================================
# 7. Cache Management
# ============================================================================
from .cache import (  # noqa: E402
    configure_cache,
    generate_cache_key,
    generate_scope,
    has_cache,
)
from .config.profiler import ReLUProfiler  # noqa: E402
from .config.spec import FuncSpec, ModelSpec  # noqa: E402

# ============================================================================
# 9. Profiling
# ============================================================================
from .profiler import FHEProfiler, ProfileResult, profiler  # noqa: E402

# ============================================================================
# 8. Utility Functions
# ============================================================================
from .util import gpu_available  # noqa: E402


# ============================================================================
# Public API
# ============================================================================

__all__ = [
    "HAS_RUNTIME",
    "CompileConfig",
    # 6. Spec (legacy, deprecated)
    "CompileSpec",
    "CompiledProgram",
    "DatasetSource",
    "Driver",
    "FHEProfiler",
    "FHERuntime",
    "FuncEntity",
    "FuncSpec",
    "InputSpec",
    "ModelEntity",
    # 6b. Spec (new, preferred)
    "ModelSpec",
    "ProfileResult",
    "ReLUProfiler",
    "RuntimeConfig",
    # 3. Backend
    "backend",
    # 1. Decorators
    "compile",
    "compute",
    # 7. Cache
    "configure_cache",
    # 4. Driver
    "driver",
    "export",
    # 2. Frontend
    "frontend",
    "generate_cache_key",
    "generate_scope",
    # 8. Utility functions
    "gpu_available",
    "has_cache",
    # 9. Profiling
    "profiler",
    # 5. Runtime
    "runtime",
]

# ============================================================================
# Extension Availability Checks (after __all__ to use frontend variable)
# ============================================================================

HAS_FRONTEND = frontend is not None

# Re-export HAS_FRONTEND for external use
__all__.append("HAS_FRONTEND")
