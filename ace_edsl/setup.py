"""
Setup script for ace_edsl package

ACE Embedded DSL - Python DSL using AST preprocessing and operator overloading
"""

from setuptools import setup, find_packages
import os

# Read version from edsl/__init__.py
def get_version():
    init_path = os.path.join(os.path.dirname(__file__), 'edsl', '__init__.py')
    if os.path.exists(init_path):
        with open(init_path) as f:
            for line in f:
                if line.startswith('__version__'):
                    return line.split('=')[1].strip().strip('"').strip("'")
    return '0.1.0'

setup(
    name='ace_edsl',
    version=get_version(),
    description='ACE Embedded DSL - Python DSL using AST preprocessing and operator overloading',
    author='ACE Compiler Team',
    packages=find_packages(),
    install_requires=[
        'pybind11>=2.10.0',
    ],
    python_requires='>=3.8',
    # Note: C++ bindings are shared via ace_bindings and must be built separately
)
