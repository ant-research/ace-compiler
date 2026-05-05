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
    3. Library: FHE execution engines (antlib, phantom, hyperfhe)
    4. Driver: Orchestrates frontend → library compilation
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
    from .runtime import FHERuntime
except ImportError:
    runtime = None
    FHERuntime = None

HAS_RUNTIME = runtime is not None

# ============================================================================
# 6. Spec - Unified Compilation Input Descriptor (Three-layer architecture)
# ============================================================================

from .spec import (
    CompileSpec,
    ModelEntity,
    FuncEntity,
    CompileConfig,
    InputSpec,
    RuntimeConfig,
    DatasetSource,
)

# ============================================================================
# 7. Cache Management
# ============================================================================

from .cache import (
    configure_cache,
    has_cache,
    generate_cache_key,
    generate_scope,
)

# ============================================================================
# 8. Utility Functions
# ============================================================================

from .util import gpu_available

# ============================================================================
# Public API
# ============================================================================

__all__ = [
    # 1. Decorators
    "compile",
    "compute",
    "export",
    # 2. Frontend
    "frontend",
    # 3. Backend
    "backend",
    # 4. Driver
    "driver",
    "Driver",
    # 5. Runtime
    "runtime",
    "FHERuntime",
    "HAS_RUNTIME",
    # 6. Spec
    "CompileSpec",
    "ModelEntity",
    "FuncEntity",
    "CompileConfig",
    "InputSpec",
    "RuntimeConfig",
    "DatasetSource",
    # 7. Cache
    "configure_cache",
    "has_cache",
    "generate_cache_key",
    "generate_scope",
    # 8. Utility functions
    "gpu_available",
]

# ============================================================================
# Extension Availability Checks (after __all__ to use frontend variable)
# ============================================================================

HAS_FRONTEND = frontend is not None

# Re-export HAS_FRONTEND for external use
__all__.append("HAS_FRONTEND")