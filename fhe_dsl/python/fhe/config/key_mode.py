# =============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# =============================================================================

"""Key mode configuration for FHE runtime.

Controls how keys are obtained at runtime and whether encryption/decryption
is available.

Modes:
    ONLINE (default):        Generate all keys (SK+PK+RLK+GK) on GPU.
                             Full encrypt → eval → decrypt pipeline.

    PRELOAD:                 Load eval keys (RLK+GK) from disk at init time.
                             This is the production default — compute-only,
                             no encrypt/decrypt.  The server receives
                             ciphertexts from clients and returns ciphertexts.

    PRELOAD_FULL:            Load all keys (SK+PK+RLK+GK) from disk at init.
                             Full pipeline with encrypt/decrypt — used for
                             debugging, development, and e2e verification.

    ON_DEMAND:                Load eval keys on-demand at runtime. RLK preload
                             at init; galois keys load lazily into an HBM
                             cache on first use (per session). Implemented for
                             the phantom backend.

    ON_DEMAND_FULL:           Load all keys on-demand at runtime (reserved).

Usage::

    from ace.fhe.config import KeyMode

    # Production: compute-only (default)
    KeyMode.PRELOAD.apply()

    # Debug/verification: full pipeline with encrypt/decrypt
    KeyMode.PRELOAD_FULL.apply()

    # Or pass in the package dict
    runner = fhe.FHERuntime({
        "model": "Add",
        "kernel": kernel_path,
        "key_path": eval_key_dir,
        "key_mode": "preload_full",
    })

Backward compatibility:
    "key_offline"  → "preload_full" (was: preload + load_full_keys=True)
    "server_only"  → "preload"      (was: preload + load_full_keys=False)

Key requirements by mode:

    ┌───────────────┬─────────┬─────────┬─────────┬─────────┬──────────────┐
    │ Mode          │ SK      │ PK      │ RLK     │ GK      │ key_path     │
    ├───────────────┼─────────┼─────────┼─────────┼─────────┼──────────────┤
    │ ONLINE        │ gen     │ gen     │ gen     │ gen     │ not required │
    │ PRELOAD       │ —       │ —       │ load    │ load    │ required     │
    │ PRELOAD_FULL  │ load    │ load    │ load    │ load    │ required     │
    │ ON_DEMAND      │ —       │ —       │ defer   │ defer   │ required     │
    │ ON_DEMAND_FULL │ defer   │ defer   │ defer   │ defer   │ required     │
    └───────────────┴─────────┴─────────┴─────────┴─────────┴──────────────┘
"""

from __future__ import annotations

from typing import ClassVar


__all__ = ["KeyMode"]


class KeyMode:
    """Key mode — controls how keys are obtained at runtime.

    Attributes:
        ONLINE:        Generate all keys on GPU (default, backward compatible)
        PRELOAD:       Load eval keys from disk at init (production default)
        PRELOAD_FULL:  Load all keys from disk at init (debug/verification)
        ON_DEMAND:      Load eval keys on-demand at runtime (galois keys lazy)
        ON_DEMAND_FULL: Load all keys on-demand at runtime (reserved)
    """

    ONLINE = "online"
    PRELOAD = "preload"
    PRELOAD_FULL = "preload_full"
    ON_DEMAND = "on_demand"
    ON_DEMAND_FULL = "on_demand_full"

    # Backward-compatible aliases
    KEY_OFFLINE = "key_offline"  # → preload_full
    SERVER_ONLY = "server_only"  # → preload

    # Internal mapping to C++ enum values
    _CPP_VALUES: ClassVar[dict[str, int]] = {
        "online": 0,
        "preload": 1,
        "on_demand": 2,
        "preload_full": 3,
        "on_demand_full": 4,
        # Aliases map to their canonical values
        "key_offline": 3,  # was: preload + load_full_keys=True
        "server_only": 1,  # was: preload + load_full_keys=False
    }

    # Modes that require key_path
    _REQUIRES_KEY_PATH: ClassVar[set[str]] = {
        "preload",
        "on_demand",
        "preload_full",
        "on_demand_full",
        "key_offline",
        "server_only",
    }

    @classmethod
    def _validate(cls, mode: str) -> str:
        """Validate and normalize a key mode string."""
        mode = mode.lower()
        if mode not in cls._CPP_VALUES:
            raise ValueError(f"Invalid key_mode='{mode}'. Must be one of: {', '.join(sorted(cls._CPP_VALUES.keys()))}")
        return mode

    @classmethod
    def _to_cpp(cls, mode: str) -> int:
        """Convert a Python mode string to the C++ enum value."""
        mode = cls._validate(mode)
        return cls._CPP_VALUES[mode]

    @classmethod
    def _needs_full_keys(cls, mode: str) -> bool:
        """Whether a mode string implies loading full keys (SK+PK).

        'key_offline' (backward compat) and 'preload_full' imply full keys.
        All other modes default to eval-only (no SK/PK).
        """
        mode = mode.lower()
        return mode in ("key_offline", "preload_full", "on_demand_full")

    @classmethod
    def apply(cls, mode: str, load_full_keys: bool | None = None) -> None:
        """Set the global key mode.

        Must be called before FHERuntime.init(). Once set, it affects
        all subsequent FheContext initializations in the current process.

        Args:
            mode: One of 'online', 'preload', 'preload_full', 'on_demand',
                  'on_demand_full', or backward-compat aliases
                  'key_offline', 'server_only'.
            load_full_keys: DEPRECATED.  When True and mode is 'preload'/'on_demand',
                  upgrades the mode to 'preload_full'/'on_demand_full'.
                  When False (default), mode is used as-is.
                  If None, inferred from mode ('key_offline' and 'preload_full'
                  → True, others → False).
        """
        from ace import runtime as _C

        mode = cls._validate(mode)

        # Backward compat: if user passes load_full_keys=True with preload/on_demand,
        # upgrade to preload_full/on_demand_full
        if load_full_keys is True and mode in ("preload", "on_demand"):
            mode = mode + "_full"

        cpp_value = cls._to_cpp(mode)
        _C.set_key_mode(cpp_value)

    @classmethod
    def get(cls) -> str:
        """Get the current global key mode as a string.

        Returns the canonical mode name (never a backward-compat alias).
        For example, after ``apply("key_offline")``, ``get()`` returns
        ``"preload_full"`` because ``key_offline`` is an alias for
        ``preload_full``.

        Returns:
            The current canonical key mode string.
        """
        from ace import runtime as _C

        cpp_value = _C.get_key_mode()
        # Reverse mapping: integer → canonical name.
        # Only return primary names, not aliases.
        _CANONICAL = {0: "online", 1: "preload", 2: "on_demand", 3: "preload_full", 4: "on_demand_full"}
        return _CANONICAL.get(cpp_value, "online")
