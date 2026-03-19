#!/usr/bin/env python3
"""
Setup script for ACE DSL.
"""

from setuptools import setup, find_packages

setup(
    name="ace-dsl",
    version="0.1.0",
    description="Python DSL for ACE (ANT Compiler for Encryption)",
    author="ACE Team",
    python_requires=">=3.8",
    packages=find_packages(),
    install_requires=[],
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "black>=22.0.0",
            "flake8>=5.0.0",
            "mypy>=0.990",
        ],
        "onnx": [
            "onnx>=1.12.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "ace-compile=ace_dsl.frontend.compile:main",
        ],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
)

