#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Backend strategies for FHE execution engines.
"""

from ..driver.registry import (
    get_library_impl,
    get_provider_specs,
    list_supported_combos,
    register_library,
)
from ..driver.registry import (
    is_library_supported as check_library,
)

# Export base classes for library implementations
from .base import CpuBackend, FheCmplrBackend, GpuBackend


# Register all built-in libraries
def _register_default_backends():
    from .acelib import AcelibLIB
    from .antlib import AntLIB
    from .openfhe import OpenFHELIB
    from .phantom import PhantomLIB
    from .seal import SealLIB

    # Register using new library terminology (backward compat maintained)
    # Note: backend_name() returns the library name (e.g., "ant", "phantom")
    register_library(AntLIB.backend_name(), AntLIB.device_name(), AntLIB)
    register_library(SealLIB.backend_name(), SealLIB.device_name(), SealLIB)
    register_library(PhantomLIB.backend_name(), PhantomLIB.device_name(), PhantomLIB)
    register_library(AcelibLIB.backend_name(), AcelibLIB.device_name(), AcelibLIB)
    register_library(OpenFHELIB.backend_name(), OpenFHELIB.device_name(), OpenFHELIB)


# Execute on import
_register_default_backends()

# Public API
from .antlib import AntLIB  # noqa: E402


__all__ = [
    # Built-in libraries
    "AntLIB",
    "CpuBackend",
    # Base classes
    "FheCmplrBackend",
    "GpuBackend",
    "check_library",
    "get_library_impl",
    "get_provider_specs",
    "list_supported_combos",
    # Library registry
    "register_library",
]
