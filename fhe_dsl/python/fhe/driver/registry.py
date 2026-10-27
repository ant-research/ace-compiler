#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Registry module for frontend and library backends.

Library vs Backend terminology:
- Library: FHE runtime implementation (e.g., ant, phantom, ace)
- Device: Hardware target (e.g., cpu, cuda)
- Backend: Python class that implements a (library, device) combo

The registry maps (library, device) combos to Backend classes.
"""

from typing import Union

from .base import Backend, Frontend


# type alias for library implementation classes
LibraryImpl = type[Backend]

# =============================================================================
# Frontend Registry
# =============================================================================

_FRONTEND_REGISTRY: dict[str, type[Frontend] | tuple] = {}


def register_frontend(name: str, cls: type[Frontend] | str | tuple) -> None:
    """
    Register a frontend strategy.

    Args:
        name: Frontend name (e.g., "torch", "onnx", "ast")
        cls: Either:
            - Frontend class (eager registration)
            - "module.path.ClassName" string (lazy registration)
            - ("module.path", "ClassName") tuple (lazy registration)
    """
    if name in _FRONTEND_REGISTRY:
        raise ValueError(f"Frontend '{name}' already registered")
    _FRONTEND_REGISTRY[name] = cls


def get_frontend(name: str, **kwargs) -> Frontend:
    """
    Get frontend instance with optional initialization parameters.

    Args:
        name: Registered frontend name
        **kwargs: Parameters passed to frontend constructor

    Returns:
        Frontend instance

    Raises:
        ValueError: If frontend not found
        RuntimeError: If frontend class cannot be imported
    """
    if name not in _FRONTEND_REGISTRY:
        available = list(_FRONTEND_REGISTRY.keys())
        raise ValueError(f"Unknown frontend '{name}'. Available: {available}")

    entry = _FRONTEND_REGISTRY[name]

    # If it's already a class, instantiate it
    if isinstance(entry, type):
        return entry(**kwargs)

    # Lazy import from string path
    try:
        if isinstance(entry, str):
            module_path, class_name = entry.rsplit(".", 1)
        else:  # tuple
            module_path, class_name = entry

        import importlib

        module = importlib.import_module(module_path)
        cls = getattr(module, class_name)
        _FRONTEND_REGISTRY[name] = cls  # Cache the class for next time
        return cls(**kwargs)

    except (ImportError, AttributeError) as e:
        raise RuntimeError(f"Failed to load frontend '{name}' from {entry}: {e}") from e


def list_frontends() -> list[str]:
    """list all registered frontends (without importing them)."""
    return list(_FRONTEND_REGISTRY.keys())


# =============================================================================
# Library Registry (was Backend Registry)
# =============================================================================

# Key: (library, device), Value: Backend subclass (library implementation)
_LIBRARY_REGISTRY: dict[tuple[str, str], LibraryImpl] = {}


def register_library(library: str, device: str, impl_cls: LibraryImpl) -> None:
    """
    Register an implementation class for a (library, device) combo.

    Args:
        library: Library name (e.g., "ant", "phantom", "ace")
        device: Device name (e.g., "cpu", "cuda")
        impl_cls: Backend subclass that implements this library+device combo

    Example:
        register_library("ant", "cpu", AntLIB)
        register_library("phantom", "cuda", PhantomLIB)
    """
    key = (library.lower(), device.lower())
    if key in _LIBRARY_REGISTRY:
        raise ValueError(f"Library combo {key} already registered.")
    _LIBRARY_REGISTRY[key] = impl_cls


def get_library_impl(library: str, device: str = "cpu", **kwargs) -> Backend:
    """
    Get an implementation instance for the specified (library, device) combo.

    Args:
        library: Library name (e.g., "ant", "phantom")
        device: Device name (default: "cpu")
        **kwargs: Arguments passed to backend constructor

    Returns:
        Backend instance (the library implementation)

    Raises:
        ValueError: If library or device not supported
    """
    device = device.lower()
    key = (library.lower(), device)

    if key not in _LIBRARY_REGISTRY:
        _raise_library_error(library, device)

    cls = _LIBRARY_REGISTRY[key]
    return cls(device=device, **kwargs)


def _raise_library_error(library: str, device: str) -> None:
    """Raise helpful error message for unknown library/device combo."""
    library_lower = library.lower()

    known_libraries = sorted({lib for (lib, _) in _LIBRARY_REGISTRY})
    devices_for_library = sorted([dev for (lib, dev) in _LIBRARY_REGISTRY if lib == library_lower])

    if library_lower in known_libraries:
        # Library exists but device not supported
        raise ValueError(
            f"Library '{library}' does not support device '{device}'.\n"
            f"Supported devices: {', '.join(devices_for_library)}"
        )
    else:
        # Unknown library
        raise ValueError(f"Unknown library: '{library}'.\nAvailable libraries: {', '.join(known_libraries)}")


def list_libraries() -> list[str]:
    """Return all registered library names."""
    return sorted({lib for lib, _ in _LIBRARY_REGISTRY})


def list_devices_for_library(library: str) -> list[str]:
    """Return supported devices for a library."""
    return sorted([dev for (lib, dev) in _LIBRARY_REGISTRY if lib == library.lower()])


def list_supported_combos() -> list[tuple[str, str]]:
    """list all registered (library, device) combos."""
    return list(_LIBRARY_REGISTRY.keys())


def get_provider_specs() -> dict[str, dict]:
    """Return provider specs for all registered libraries.

    Returns:
        dict mapping library name to spec dict with keys:
        - device: str (e.g., "cpu", "cuda")
        - implemented: bool (whether compile_to_lib is functional)
    """
    specs = {}
    for (library, device), impl_cls in _LIBRARY_REGISTRY.items():
        specs[library] = {
            "device": device,
            "implemented": getattr(impl_cls, "implemented", True),
        }
    return specs


def is_library_supported(library: str, device: str) -> bool:
    """Check if a library+device combo is supported."""
    return (library.lower(), device.lower()) in _LIBRARY_REGISTRY


def register_impl(impl_cls: LibraryImpl) -> None:
    """
    Auto-register a backend class using cls.library_name() and cls.device_name().

    Priority:
    1. library_name() + device_name() (new standard)
    2. backend_name() + device_name() (backward compat)

    Usage as decorator:
        @register_impl
        class AntLIB(CpuBackend):
            @classmethod
            def library_name(cls) -> str: return "ant"
            @classmethod
            def device_name(cls) -> str: return "cpu"
    """
    # Try library_name() first, fall back to backend_name()
    library_method = getattr(impl_cls, "library_name", None)
    if library_method is None:
        library_method = getattr(impl_cls, "backend_name", None)

    if library_method is None:
        raise ValueError(
            f"Backend class {impl_cls.__name__} must define either library_name() or backend_name() class method"
        )

    library = library_method()
    device = impl_cls.device_name()
    register_library(library, device, impl_cls)


# =============================================================================
# Public API
# =============================================================================

__all__ = [
    "get_frontend",
    "get_library_impl",
    "get_provider_specs",
    "is_library_supported",
    "list_devices_for_library",
    "list_frontends",
    "list_libraries",
    "list_supported_combos",
    # Frontend
    "register_frontend",
    "register_impl",
    # Library
    "register_library",
]
