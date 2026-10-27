"""rtlib-py -- the pure-Python runtime home for the ckks IR -> Python
(`-CKKS2PY`) backend of fhe-cmplr.

Design (Phase 2.0):
  * PURE PYTHON -- never wraps/binds the C rtlib (no pybind11 / ctypes /
    BUILD_SHARED / ExternalProject). Each provider is a native-Python impl of
    the frozen contract (common/contract.py).
  * Two-track by backend API flavor: a C-API backend uses the existing
    `rtlib/` (`-P2C`); a Python-API backend uses THIS package (`-CKKS2PY`).
  * First provider `rt_ref` = the reference plaintext runtime (promoted from
    the Phase-1 `test/py_verify/fake_rt.py` hack). Real-HE (`rt_openfhe`, via
    upstream openfhe-python) is Phase 3.

Layout (mirrors rtlib's common + rt_<prov>):
  rtlib_py/common/{contract,driver,provider,conf}.py   provider-agnostic
  rtlib_py/rt_ref/             reference plaintext provider (Phase 2)
  rtlib_py/rt_openfhe/         Phase-3 stub (NotImplementedError)

The emitted `-CKKS2PY` module begins with `from rtlib_py.<_py_rt_module> import *`
(default `rt_ref`); run it via `rtlib_py.common.driver.run(py_file, spec)`.
"""

from .common import (ALL_PROVIDERS, CONTRACT_NAMES, PUBLIC_API, PUBLIC_TYPES,
                     RuntimeProvider, check_provider, get_provider, register,
                     run)

__version__ = "0.1.0"

__all__ = [
    "ALL_PROVIDERS", "CONTRACT_NAMES", "PUBLIC_API", "PUBLIC_TYPES",
    "RuntimeProvider", "check_provider", "get_provider", "register",
    "run", "__version__",
]