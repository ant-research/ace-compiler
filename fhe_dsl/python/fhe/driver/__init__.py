#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Function module: FHE compilation infrastructure.
"""

# Expose core interfaces at package level
# Import frontends to trigger registration
from .. import frontend as _frontend
from .base import Backend, Frontend
from .driver import Driver
from .registry import (
    _raise_library_error,
    get_frontend,
    get_library_impl,
    is_library_supported,
    list_devices_for_library,
    list_frontends,
    list_libraries,
    list_supported_combos,
    register_frontend,
    register_impl,
    # Library registry
    register_library,
)


__all__ = [
    "Backend",
    "Driver",
    # Core interfaces
    "Frontend",
    "get_frontend",
    "get_library_impl",
    "is_library_supported",
    "list_devices_for_library",
    "list_frontends",
    "list_libraries",
    "list_supported_combos",
    # Frontend registry
    "register_frontend",
    "register_impl",
    # Library registry
    "register_library",
]
