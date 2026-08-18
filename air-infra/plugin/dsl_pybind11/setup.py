"""
Provide python-space access to the functions exposed in __init__.pyx
"""

from distutils.core import setup, Extension
from distutils import sysconfig

ext_modules = [
   Extension(
      "air_dsl",
      language="c++",
      sources=["src/air_dsl_bindings.cxx", "src/dsl.cxx"],
      include_dirs = ['include'],
      libraries=["NNutil", "AIRopt", "AIRcg", "AIRopt", "AIRdriver", "AIRbase", "AIRcore"],
   )
]

setup(
      ext_modules = ext_modules
)
