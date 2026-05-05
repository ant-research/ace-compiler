#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Backend strategies for FHE execution engines.
"""

from ..driver.registry import (
    register_library,
    get_library_impl,
    is_library_supported as check_library,
    list_supported_combos,
)

# Export base classes for library implementations
from .base import FheCmplrBackend, CpuBackend, GpuBackend

# Register all built-in libraries
def _register_default_backends():
    from .antlib import AntLIB
    from .seal import SealLIB
    from .phantom import PhantomLIB
    from .hyperfhe import HyperfheLIB
    from .openfhe import OpenFHELIB

    # Register using new library terminology (backward compat maintained)
    # Note: backend_name() returns the library name (e.g., "antlib", "phantom")
    register_library(AntLIB.backend_name(), AntLIB.device_name(), AntLIB)
    register_library(SealLIB.backend_name(), SealLIB.device_name(), SealLIB)
    register_library(PhantomLIB.backend_name(), PhantomLIB.device_name(), PhantomLIB)
    register_library(HyperfheLIB.backend_name(), HyperfheLIB.device_name(), HyperfheLIB)
    register_library(OpenFHELIB.backend_name(), OpenFHELIB.device_name(), OpenFHELIB)


# Execute on import
_register_default_backends()

# Public API
from .antlib import AntLIB

__all__ = [
    # Base classes
    "FheCmplrBackend",
    "CpuBackend",
    "GpuBackend",
    # Library registry
    "register_library",
    "get_library_impl",
    "check_library",
    "list_supported_combos",
    # Built-in libraries
    "AntLIB",
]