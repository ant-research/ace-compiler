"""
Setup script for ace_bindings package.

This is a minimal setup for the shared C++ bindings package.
The .so files are built separately via CMake and copied here.
"""

from setuptools import setup, find_packages

setup(
    name="ace_bindings",
    version="0.1.0",
    description="Shared C++ bindings for ACE compiler infrastructure",
    packages=find_packages(where=".."),
    package_dir={"": ".."},
    python_requires=">=3.8",
    # Note: The .so files are built via CMake, not setuptools
    # Include them as package data
    package_data={
        "ace_bindings": ["*.so", "*.cpython-*.so"],
    },
    include_package_data=True,
)

