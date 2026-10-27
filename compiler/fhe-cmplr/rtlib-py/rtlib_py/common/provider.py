"""rtlib-py provider registry -- the Python analog of the C
`ALL_PROVIDERS()` X-macro (`include/fhe/core/lib_provider.h:18-23`).

The C compiler enumerates crypto providers (ANT/SEAL/OPENFHE/PHANTOM/ACE) via
the X-macro; rtlib-py instead enumerates **Python runtime modules**
(`rt_ref`/`rt_openfhe`/...). The key IS the rtlib_py submodule name, so it
matches `POLY2PY_CONFIG::_py_rt_module` (decision #1) one-for-one: the emitted
`from rtlib_py.<name> import *` and the driver's `provider=<name>` refer to the
same module instance (Python caches imports), so state the driver seeds and
state the emitted module reads are consistent.

Providers are loaded LAZILY (`importlib`): a provider's heavy native binding
(`openfhe`/`fhert.openfhe`) is imported only inside its `_seed_ctx`, so listing
it here never crashes a host without the binding. A provider is validated
against the frozen contract (common/contract.check_provider) the first time it
is fetched.
"""

import importlib

from .contract import check_provider

# Known rtlib-py providers (mirrors ALL_PROVIDERS). Add a provider = append a
# name here + ship rtlib_py/<name>/__init__.py. The crypto provider set
# (SEAL/OPENFHE/PHANTOM/ACE + ref) is a separate concern -- `-CKKS2PY:lib=` still
# flows into get_context_params()['provider'] for the runtime to read, but it
# does NOT select a Python module.
#   rt_ref     -- plaintext reference (Phase 2, the contract / golden ref)
#   rt_openfhe -- real CKKS via upstream openfhe-python (Phase 3)
#   rt_fhert   -- real CKKS via fhert's OpenFHE 1.4.2 binding (Plan
#                 doc/rtlib-py-integrate/rt_fhert_python_plan.md, Phase A host)
ALL_PROVIDERS = ("rt_ref", "rt_openfhe", "rt_fhert")

# Cache: name -> loaded module (importlib caches too, but caching here keeps the
# contract check to a single run per name).
_PROVIDERS = {}


def get_provider(name="rt_ref"):
    """Return the rtlib_py submodule `name` as a conforming provider module.
    `name` defaults to `rt_ref` (Phase 2 reference plaintext runtime). Raises
    `ValueError` for an unknown name, or `AttributeError` if a known module is
    missing a frozen contract name."""
    if name not in ALL_PROVIDERS:
        raise ValueError(
            f"unknown rtlib-py provider {name!r}; known: {list(ALL_PROVIDERS)} "
            f"(add it to ALL_PROVIDERS + ship rtlib_py/{name}/)")
    if name not in _PROVIDERS:
        mod = importlib.import_module(f"rtlib_py.{name}")
        check_provider(mod)
        _PROVIDERS[name] = mod
    return _PROVIDERS[name]


def register(name, module):
    """Explicit registration (for tests / out-of-tree providers that ship
    outside rtlib_py/). Validates against the frozen contract."""
    check_provider(module)
    _PROVIDERS[name] = module


__all__ = ["ALL_PROVIDERS", "get_provider", "register"]