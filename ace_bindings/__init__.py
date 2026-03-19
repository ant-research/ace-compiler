"""
ACE Bindings - Shared C++ binding modules for ACE compiler infrastructure

This package provides Python access to the C++ ACE-compiler infrastructure via pybind11.
It is designed to be shared by multiple Python DSL projects (acepy, pydsl, etc.).

Available Modules:
    - air_builder: Core AIR infrastructure (GlobScope, FuncScope, Container, Node)
    - nn_addon: Neural network operations (nn::core, nn::vector)
    - fhe_cmplr: FHE compilation (fhe::sihe, fhe::ckks, fhe::poly)
    - passmanager: Pass infrastructure (PassManager, Pass)

Build Instructions:
    cd ace-compiler/bindings
    mkdir build && cd build
    cmake .. -DACE_COMPILER_DIR=/path/to/ace-compiler
    make

Usage:
    from ace_bindings import air_builder
    
    glob = air_builder.create_glob_scope()
    func = glob.new_func("my_kernel")
    container = func.container()
    
    a = func.new_param("a", air_builder.Type.make_float(32))
    b = func.new_param("b", air_builder.Type.make_float(32))
    result = container.new_add(a, b)
"""

import os
import sys
from typing import Optional, Any

_bindings_dir = os.path.dirname(os.path.abspath(__file__))


def _try_import(module_name: str) -> Optional[Any]:
    """Try to import a C++ binding module."""
    import glob
    import importlib
    import importlib.util
    
    # First try direct import (if modules are in PYTHONPATH or properly installed)
    try:
        return importlib.import_module(f".{module_name}", package="ace_bindings")
    except ImportError:
        pass
    
    # Try loading from bindings directory with ABI-tagged filenames
    # pybind11 modules have names like: module.cpython-311-x86_64-linux-gnu.so
    try:
        # Look for any .so file matching the module name
        patterns = [
            os.path.join(_bindings_dir, f"{module_name}.so"),
            os.path.join(_bindings_dir, f"{module_name}.*.so"),
            os.path.join(_bindings_dir, f"{module_name}.cpython-*.so"),
        ]
        
        for pattern in patterns:
            matches = glob.glob(pattern)
            if matches:
                so_path = matches[0]  # Take first match
                spec = importlib.util.spec_from_file_location(module_name, so_path)
                if spec and spec.loader:
                    module = importlib.util.module_from_spec(spec)
                    spec.loader.exec_module(module)
                    return module
    except Exception as e:
        # Debug: uncomment to see loading errors
        # print(f"Failed to load {module_name}: {e}")
        pass
    
    return None


# =============================================================================
# AIR Builder - Core IR infrastructure
# =============================================================================
air_builder = _try_import("air_builder")

if air_builder is None:
    raise ImportError(
        "C++ air_builder bindings not available. "
        "Build with: cd ace-compiler/bindings && mkdir build && cd build && cmake .. && make"
    )


# =============================================================================
# NN Addon - Neural network operations
# =============================================================================
nn_addon = _try_import("nn_addon")

if nn_addon is None:
    raise ImportError(
        "C++ nn_addon bindings not available. "
        "Build with: cd ace-compiler/bindings && mkdir build && cd build && cmake .. && make"
    )


# =============================================================================
# FHE Compiler - FHE operations
# =============================================================================
fhe_cmplr = _try_import("fhe_cmplr")

if fhe_cmplr is None:
    raise ImportError(
        "C++ fhe_cmplr bindings not available. "
        "Build with: cd ace-compiler/bindings && mkdir build && cd build && cmake .. && make"
    )


# =============================================================================
# Pass Manager - Compilation passes
# =============================================================================
passmanager = _try_import("passmanager")

if passmanager is None:
    raise ImportError(
        "C++ passmanager bindings not available. "
        "Build with: cd ace-compiler/bindings && mkdir build && cd build && cmake .. && make"
    )


__all__ = [
    'air_builder',
    'nn_addon', 
    'fhe_cmplr',
    'passmanager',
]

