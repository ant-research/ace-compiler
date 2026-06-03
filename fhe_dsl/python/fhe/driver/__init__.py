#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Function module: FHE compilation infrastructure.
"""

# Expose core interfaces at package level
from .base import Frontend, Backend
from .registry import (
    register_frontend,
    get_frontend,
    list_frontends,
    # Library registry
    register_library,
    get_library_impl,
    list_libraries,
    list_devices_for_library,
    list_supported_combos,
    is_library_supported,
    register_impl,
)

from .driver import Driver

# Import frontends to trigger registration
from .. import frontend as _frontend

__all__ = [
    # Core interfaces
    "Frontend",
    "Backend",
    "Driver",
    # Frontend registry
    "register_frontend",
    "get_frontend",
    "list_frontends",
    # Library registry
    "register_library",
    "get_library_impl",
    "list_libraries",
    "list_devices_for_library",
    "list_supported_combos",
    "is_library_supported",
    "register_impl",
]