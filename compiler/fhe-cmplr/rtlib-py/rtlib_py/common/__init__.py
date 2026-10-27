"""Common layer of rtlib-py -- provider-agnostic runtime infrastructure.

Mirrors `rtlib/include/common/` (the C rtlib common headers): the frozen
runtime contract (contract.py), the runner (driver.py), the provider registry
(provider.py), and the `.conf` parity parser (conf.py). Each `rt_<provider>/`
subpackage implements the contract declared here.
"""

from .contract import (CONTRACT_NAMES, PUBLIC_API, PUBLIC_TYPES,
                       RuntimeProvider, check_provider)
from .driver import run
from .provider import ALL_PROVIDERS, get_provider, register

__all__ = [
    "CONTRACT_NAMES", "PUBLIC_API", "PUBLIC_TYPES",
    "RuntimeProvider", "check_provider",
    "run",
    "ALL_PROVIDERS", "get_provider", "register",
]