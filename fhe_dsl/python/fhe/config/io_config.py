# =============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# =============================================================================

"""I/O transport configuration for FHE key save/load operations.

Provides two levels of abstraction:

  1. **IoPolicy + IoPolicySettings** (recommended) — transport path selection
     that describes *how* data is moved.  The IO implementation translates
     the policy into the optimal transport and tuning parameters.

     >>> from ace.fhe.config import IoPolicySettings
     >>> IoPolicySettings(policy="staged").apply()   # safety-first

  2. **IoConfig** (expert override) — direct control over the
     low-level transport parameters.  Only needed for expert tuning of
     chunk_size, threads, or galois format.

     >>> from ace.fhe.config import IoConfig
     >>> IoConfig(threads=4, chunk_size=16*1024*1024).apply()

IoPolicy values (transport path selection):

  ┌──────────┬────────────────────────────────────────────────────────┐
  │ Policy   │ Behavior                                              │
  ├──────────┼────────────────────────────────────────────────────────┤
  │ auto     │ GDS if cuFile available, else staged (default)       │
  │ gds      │ cuFile DMA (fallback to staged on error)             │
  │ staged   │ Host staged: fread + cudaMemcpy (safest path)        │
  │ mapped   │ Host mapped: mmap + cudaMemcpy (one less copy)       │
  └──────────┴────────────────────────────────────────────────────────┘

Deprecated (backward compatible) — mapped to new values:

  ┌──────────────────┬──────────────┬──────────────────────────────────┐
  │ Old name         │ Maps to      │ Note                             │
  ├──────────────────┼──────────────┼──────────────────────────────────┤
  │ auto_detect      │ auto         │ Same behavior                    │
  │ galois_key_load  │ auto         │ Business preset now via IoProfile│
  │ key_persistence  │ auto         │ Business preset now via IoProfile│
  │ checkpoint       │ staged       │ Always staged                    │
  │ high_throughput  │ auto         │ Business preset now via IoProfile│
  └──────────────────┴──────────────┴──────────────────────────────────┘
"""

from __future__ import annotations

import warnings
from dataclasses import dataclass, field


__all__ = ["DataPath", "IoConfig", "IoExecMode", "IoPolicy", "IoPolicySettings", "IoProfile"]


class IoPolicy:
    """IO transport selection — the primary knob for how data is moved.

    Maps to rtio::IoPolicy C++ enum. Use IoPolicySettings instead of
    IoConfig for normal IO configuration.

    Values:
        AUTO:    Auto-detect: GDS if cuFile available, else staged (default)
        GDS:     GDS (cuFile DMA), fallback to staged on error
        STAGED:  Host staged: fread + cudaMemcpy (always available, safest)
        MAPPED:  Host mapped: mmap + cudaMemcpy (one less copy)
    """

    AUTO = "auto"
    GDS = "gds"
    STAGED = "staged"
    MAPPED = "mapped"


class DataPath:
    """DEPRECATED: Use IoPolicy instead. Kept for backward compatibility.

    Old values are mapped to their IoPolicy equivalents.
    """

    HOST_STAGED = "staged"  # Was "host_staged"
    HOST_MAPPED = "mapped"  # Was "host_mapped"
    GDS = "gds"
    AUTO = "auto"

    # Old values for backward compat (emit deprecation when used)
    _OLD_HOST_STAGED = "host_staged"
    _OLD_HOST_MAPPED = "host_mapped"


class IoExecMode:
    """Execution mode for IO operations.

    Controls how IO operations are dispatched relative to computation.
    Currently only synchronous (blocking) mode is implemented.
    Async and pipeline modes are reserved for future extensions where
    IO and GPU computation can overlap.

    Values:
        sync:  Blocking — call returns when IO completes (current default)
    """

    SYNC = "sync"
    # Future: ASYNC = "async"
    # Future: PIPELINE = "pipeline"


# Mapping of deprecated policy names to their new equivalents
_DEPRECATED_POLICY_MAP = {
    "auto_detect": IoPolicy.AUTO,
    "galois_key_load": IoPolicy.AUTO,
    "key_persistence": IoPolicy.AUTO,
    "checkpoint": IoPolicy.STAGED,
    "high_throughput": IoPolicy.AUTO,
}

# All valid policy names (new + deprecated)
_VALID_POLICIES = {IoPolicy.AUTO, IoPolicy.GDS, IoPolicy.STAGED, IoPolicy.MAPPED}
_VALID_POLICIES.update(_DEPRECATED_POLICY_MAP.keys())


@dataclass
class IoPolicySettings:
    """Primary IO configuration for FHE key save/load.

    Attributes:
        policy: IO transport policy (default: auto). Controls how the IO
            implementation selects transport path and tuning parameters.
        disk_paths: Multi-disk paths for galois key distribution (expert).
            When empty (default), single-disk mode is used. When non-empty,
            key i is placed on disk_paths[i % len(disk_paths)].

    Example::

        from ace.fhe.config import IoPolicySettings

        # Default — auto-detect everything
        IoPolicySettings().apply()

        # Staged mode — safety-first, no GDS
        IoPolicySettings(policy="staged").apply()

        # Auto with multi-disk
        IoPolicySettings(policy="auto",
                         disk_paths=["/mnt/nvme0", "/mnt/nvme1"]).apply()

        # Backward compat (deprecated names still work, emit warning)
        IoPolicySettings(policy="galois_key_load",
                         disk_paths=["/mnt/nvme0"]).apply()
    """

    policy: str = IoPolicy.AUTO
    disk_paths: list[str] = field(default_factory=list)

    def __post_init__(self):
        # Check for deprecated names and emit warnings
        if self.policy in _DEPRECATED_POLICY_MAP:
            new_policy = _DEPRECATED_POLICY_MAP[self.policy]
            warnings.warn(
                f"IoPolicy value '{self.policy}' is deprecated. "
                f"Use '{new_policy}' instead. "
                f"Business presets are now in IoProfile.",
                DeprecationWarning,
                stacklevel=2,
            )
            object.__setattr__(self, "policy", new_policy)
            return

        if self.policy not in _VALID_POLICIES:
            raise ValueError(f"Invalid policy='{self.policy}'. Must be one of: {', '.join(sorted(_VALID_POLICIES))}")

    def apply(self):
        """Apply this policy as the global IO policy.

        Sets the C++ global IoPolicy so that subsequent key save/load
        operations use this policy. If disk_paths is non-empty, also
        configures the global IoConfig disk_paths.
        """
        from ace import runtime as _C

        _POLICY_MAP = {
            # New names (primary)
            IoPolicy.AUTO: _C.IoPolicy.auto,
            IoPolicy.GDS: _C.IoPolicy.gds,
            IoPolicy.STAGED: _C.IoPolicy.staged,
            IoPolicy.MAPPED: _C.IoPolicy.mapped,
        }

        if self.disk_paths:
            _C.set_io_policy_with_disk_paths(_POLICY_MAP[self.policy], self.disk_paths)
        else:
            _C.set_io_policy(_POLICY_MAP[self.policy])


@dataclass
class IoConfig:
    """I/O transport configuration for FHE key save/load.

    .. deprecated::
        Prefer IoPolicySettings for normal IO configuration.
        IoConfig is an expert-level override for direct control over
        transport parameters (chunk_size, threads, galois format).

    Attributes:
        fallback_on_error: If True (default), GDS I/O errors silently fall
            back to staged (POSIX + cudaMemcpy) I/O. If False, errors are
            propagated. Only meaningful when policy is "auto" or "gds".
        chunk_size: I/O request size in bytes. Default is 8 MiB,
            optimal for GdsTransport (matches GDS DMA throughput).
        threads: Number of parallel I/O threads. Default is 4.
        disk_paths: List of disk paths for per-key galois key distribution.
            When empty (default), single-disk mode.
        galois_merged_format: If True (default), save galois keys as one
            merged file with offset-based indexing.
        exec_mode: Execution mode for IO operations. Default "sync".
    """

    # NOTE: data_path field REMOVED — transport selection is now via IoPolicy.
    # If user passes data_path, we convert it to IoPolicy internally (deprecated).
    fallback_on_error: bool = True
    chunk_size: int = 8 * 1024 * 1024  # 8 MiB — optimal for GDS DMA throughput
    threads: int = 4
    num_io_threads: int | None = field(default=None, repr=False)
    disk_paths: list[str] = field(default_factory=list)
    galois_merged_format: bool = True
    exec_mode: str = IoExecMode.SYNC

    def __post_init__(self):
        # Backward compat: num_io_threads → threads
        if self.num_io_threads is not None:
            object.__setattr__(self, "threads", self.num_io_threads)
            object.__setattr__(self, "num_io_threads", None)

    @classmethod
    def _from_data_path(cls, data_path: str, **kwargs) -> IoConfig:
        """Create an IoConfig from a deprecated data_path value.

        This is a backward-compatibility bridge. data_path is mapped to
        the equivalent IoPolicy internally, and a DeprecationWarning is emitted.
        """
        _DATAPATH_TO_POLICIES = {
            "host_staged": "staged",
            "host_mapped": "mapped",
            "staged": "staged",
            "mapped": "mapped",
            "gds": "gds",
            "auto": "auto",
        }
        warnings.warn(
            "IoConfig.data_path is deprecated. Use IoPolicySettings(policy=...) instead.",
            DeprecationWarning,
            stacklevel=3,
        )
        # Just create a normal IoConfig — data_path mapping is informational only
        return cls(**kwargs)

    def _to_cpp(self):
        """Convert to C++ IoConfig object via pybind11 bindings."""
        from ace import runtime as _C

        cfg = _C.IoConfig()
        cfg.fallback_on_error = self.fallback_on_error
        cfg.chunk_size = self.chunk_size
        cfg.num_io_threads = self.threads
        cfg.disk_paths = self.disk_paths
        cfg.galois_merged_format = self.galois_merged_format
        # exec_mode: currently only "sync" is supported
        _EXEC_MODE_MAP = {
            IoExecMode.SYNC: _C.IoExecMode.sync,
        }
        cfg.exec_mode = _EXEC_MODE_MAP.get(self.exec_mode, _C.IoExecMode.sync)
        return cfg

    def apply(self):
        """Apply this configuration as the global I/O config.

        .. deprecated::
            Prefer IoPolicySettings for normal IO configuration.

        Example::

            IoConfig(threads=4, chunk_size=16*1024*1024).apply()
        """
        warnings.warn(
            "IoConfig.apply() is a low-level API. Prefer IoPolicySettings for normal IO configuration.",
            DeprecationWarning,
            stacklevel=2,
        )
        from ace import runtime as _C

        _C.set_io_config(self._to_cpp())


class IoProfile:
    """Business-level IO presets — convenience combinations of IoPolicy + IoConfig.

    These are NOT part of the IO transport layer. They encode domain knowledge
    about how different FHE workloads should configure IO.

    Example::

        from ace.fhe.config import IoProfile, IoPolicySettings, IoConfig

        # High-throughput key load
        settings, config = IoProfile.galois_key_load(disk_paths=["/mnt/nvme0"])
        settings.apply()
        config.apply()  # expert override — will emit DeprecationWarning

        # Or simply use IoPolicySettings with defaults:
        IoPolicySettings(policy="auto",
                         disk_paths=["/mnt/nvme0"]).apply()
    """

    @staticmethod
    def galois_key_load(disk_paths=None):
        """High-throughput merged-file GPU key load."""
        dp = disk_paths or []
        return (
            IoPolicySettings(policy=IoPolicy.AUTO, disk_paths=dp),
            IoConfig(threads=4, galois_merged_format=True, disk_paths=dp),
        )

    @staticmethod
    def key_persistence(disk_paths=None):
        """Per-key save/load (balanced)."""
        dp = disk_paths or []
        return (
            IoPolicySettings(policy=IoPolicy.AUTO, disk_paths=dp),
            IoConfig(threads=2, galois_merged_format=False, disk_paths=dp),
        )

    @staticmethod
    def checkpoint():
        """Safety-first (staged, no GDS)."""
        return (IoPolicySettings(policy=IoPolicy.STAGED), IoConfig(threads=1, galois_merged_format=False))

    @staticmethod
    def high_throughput(disk_paths=None):
        """Maximum bandwidth bulk transfer."""
        return IoProfile.galois_key_load(disk_paths)
