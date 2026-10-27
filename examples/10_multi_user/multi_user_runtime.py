#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-User FHE Runtime — simulate N users sharing one FHE inference process.

Goal
----
A single FHE inference server (one GPU, one compiled model) serves many users.
Each user owns an independent key bundle (SK/PK/RLK/GK). At any moment exactly
one user's keys are bound to the evaluator; inference runs under that user's
keys. Switching users means rebinding the evaluator to a different key bundle
WITHOUT recompiling or re-instantiating the (heavy) crypto context.

Why this module exists
-----------------------
The C++ layer already has the multi-user scaffolding: ``FheContext`` holds a
primary ``FheSession`` + an ``_active_session`` pointer and exposes
``switch_session()`` (implemented for phantom and ace backends), which rebinds
relin/galois/encryptor/decryptor to a new session's keys.  BUT none of that is
exposed to Python, and ``FHERuntime`` is a process-wide singleton.  So today
there is no Python way to keep N key bundles resident and switch between them.

This module provides a Python-level abstraction NOW so the multi-user workflow
is runnable and measurable, with a seam left for the native C++ session path:

    ┌──────────────────────────── MultiUserRuntime ────────────────────────────┐
    │  register_user(user_id, key_dir)   switch(user_id)   run(*inputs)        │
    │                   │ (delegates to a pluggable SessionBackend)             │
    │      ┌────────────┴───────────────┐                                    │
    │      ▼                            ▼                                    │
    │  RebuildBackend             NativeSessionBackend (TODO)                │
    │  (now: finalize+reload)     (future: C++ switch_session via pybind)    │
    └─────────────────────────────────────────────────────────────────────────┘

RebuildBackend (current)
    On ``switch(user_id)``: finalizes the active FHERuntime, then constructs a
    new ``FHERuntime`` pointed at ``user_id``'s key_dir in PRELOAD_FULL mode.
    The first ``run()`` after the switch pays the key-load cost (cuFile/SSD
    read of galois keys into HBM).  This is the ONLY correct path in pure
    Python today and gives a measurable upper-bound on the per-switch cost:
    each switch re-reads the user's galois keys from disk.

    ⇒ The headline number this measures:  switch_latency = key-load time.
    ⇒ The architectural gap it exposes:    a native switch_session() would keep
       all sessions' keys resident in HBM and switch by pointer rebind only —
       no disk IO, no context teardown/recreate.  When that lands, swap the
       backend: user scripts stay unchanged.

NativeSessionBackend (future, not implemented)
    Would pybind ``FheContext::create_extra_session(key_dir)`` +
    ``switch_session(session)`` so N key bundles live in GPU memory and a
    switch is just an evaluator key rebind (microseconds).  Requires C++ work:
    a per-session key-load entry on KeyMgr (today key_path is process-global),
    a session container on FheContext, and the pybind surface.  Memory note:
    N×galois_keys must fit in HBM — fine for small cases, infeasible for N
    ResNet-20 users (22 GiB each on an 80 GiB A100).

Memory discipline
------------------
GPU must be idle before benchmarking (cuFile Bar1 mapping fails otherwise).
Use the standard pre-benchmark recipe (see CLAUDE.md) before timing switches.
"""

from __future__ import annotations

import abc
import logging
import time
from typing import Any

import torch

from ace import fhe
from ace import runtime as _C


logger = logging.getLogger(__name__)

# PRELOAD_FULL loads SK+PK+RLK+GK from disk — needed so each user can both
# encrypt inputs and decrypt outputs end-to-end (the multi-user verification
# path). The production compute-only mode (PRELOAD, eval-only) would skip SK/PK
# and could not decrypt here.
_KEY_MODE = "preload_full"


class SessionBackend(abc.ABC):
    """Pluggable strategy for binding/switching a user's keys to the evaluator."""

    @abc.abstractmethod
    def activate(self, user_id: str, key_dir: str, package: dict) -> Any:
        """Make ``user_id``'s keys the active set. Return whatever the backend
        needs to run inference (e.g. a live FHERuntime)."""
        raise NotImplementedError

    @abc.abstractmethod
    def deactivate(self) -> None:
        """Release the currently active session's native resources."""
        raise NotImplementedError

    @abc.abstractmethod
    def run(self, handle: Any, *inputs: torch.Tensor) -> torch.Tensor:
        """Run one inference under the active session's keys."""
        raise NotImplementedError

    @property
    @abc.abstractmethod
    def name(self) -> str:
        raise NotImplementedError


class RebuildBackend(SessionBackend):
    """Rebuild-session backend: finalizes the old FHERuntime and constructs a new
    one pointed at the target user's key_dir.  Correct today; measures the disk
    IO cost that a native switch_session would eliminate.

    Each switch is expensive (tear down PhantomContext + reload galois keys from
    disk), so this backend is for correctness and an upper-bound baseline, not
    for production throughput.
    """

    name = "rebuild"

    def activate(self, user_id: str, key_dir: str, package: dict) -> fhe.FHERuntime:
        # Constructing FHERuntime auto-finalizes the previous active instance
        # (FHERuntime._active_instance singleton). We set key_path/key_mode in
        # the package so the new runtime LOADS this user's keys (PRELOAD_FULL).
        pkg = {**package, "key_path": key_dir, "key_mode": _KEY_MODE}
        logger.info("[rebuild] activate user=%s key_dir=%s", user_id, key_dir)
        runner = fhe.FHERuntime(pkg)
        return runner

    def deactivate(self) -> None:
        # Finalize whatever live runtime exists. Cheap and idempotent.
        prev = fhe.FHERuntime._active_instance
        if prev is not None and not prev._finalized:
            prev.finalize()

    def run(self, handle: fhe.FHERuntime, *inputs: torch.Tensor) -> torch.Tensor:
        # handle.init()/keys load lazily on first run(); the first call after a
        # switch is where the galois-key load time shows up.
        return handle.run(*inputs)


class MultiUserRuntime:
    """Serve N users from one compiled model, one active user at a time.

    Args:
        package: CompiledProgram.package dict (model/kernel/config_path),
                 as produced by ``compiled.fhe_compile(example_inputs)``.
        backend: SessionBackend controlling how keys are bound. Defaults to
                 the correctness-first RebuildBackend.

    Usage::

        mu = MultiUserRuntime(prog.package)
        mu.register_user("alice", "/keys/alice")
        mu.register_user("bob",   "/keys/bob")
        mu.switch("alice"); y_a = mu.run(x)
        mu.switch("bob");   y_b = mu.run(x)
        mu.close()
    """

    def __init__(self, package: dict, backend: SessionBackend | None = None):
        self.package = dict(package)
        self.backend: SessionBackend = backend or RebuildBackend()
        self._users: dict[str, str] = {}  # user_id -> key_dir
        self._active_user: str | None = None
        self._active_handle: Any = None
        # Per-user stats for the switch-cost report.
        self.stats: dict[str, dict] = {}

    # ------------------------------------------------------------------ users
    def register_user(self, user_id: str, key_dir: str) -> None:
        """Register a user and the directory holding their key bundle.

        Does NOT load keys yet — keys are loaded on first switch (lazy), so
        registering many users is cheap."""
        if not key_dir:
            raise ValueError("key_dir is required")
        self._users[user_id] = key_dir
        self.stats.setdefault(user_id, {"switches": 0, "first_switch_s": None, "runs": 0})

    @property
    def active_user(self) -> str | None:
        return self._active_user

    @property
    def user_ids(self) -> list[str]:
        return list(self._users)

    # --------------------------------------------------------------- switching
    def switch(self, user_id: str) -> None:
        """Make ``user_id`` the active user (load their keys into the evaluator)."""
        if user_id not in self._users:
            raise KeyError(f"unknown user '{user_id}'; registered: {list(self._users)}")

        t0 = time.perf_counter()
        handle = self.backend.activate(user_id, self._users[user_id], self.package)
        dt = time.perf_counter() - t0

        self._active_user = user_id
        self._active_handle = handle
        st = self.stats[user_id]
        st["switches"] += 1
        if st["first_switch_s"] is None:
            st["first_switch_s"] = dt
        # The most recent switch latency (includes context teardown + key load).
        st["last_switch_s"] = dt
        logger.info("[multi-user] switch -> %s (%.3fs)", user_id, dt)

    # ----------------------------------------------------------------- running
    def run(self, *inputs: torch.Tensor) -> torch.Tensor:
        """Run inference under the currently active user's keys."""
        if self._active_handle is None:
            raise RuntimeError("no active user — call switch(user_id) first")
        out = self.backend.run(self._active_handle, *inputs)
        self.stats[self._active_user]["runs"] += 1
        return out

    # ------------------------------------------------------------------- timing
    def first_run(self, *inputs: torch.Tensor) -> tuple[torch.Tensor, float]:
        """Run inference and also return wall-clock seconds of this call.

        The FIRST run after a switch is where the key-load cost lands for the
        rebuild backend (lazy init + galois-key read). Use this to measure the
        per-switch overhead, separate from steady-state inference."""
        if self._active_handle is None:
            raise RuntimeError("no active user — call switch(user_id) first")
        t0 = time.perf_counter()
        out = self.backend.run(self._active_handle, *inputs)
        return out, time.perf_counter() - t0

    def get_galois_load_time(self) -> float:
        """Wall-clock seconds of the most recent galois-key load for the active
        runtime, or -1.0 if unavailable. For RebuildBackend this is the dominant
        component of switch latency for large models."""
        if self._active_handle is None:
            return -1.0
        try:
            return float(self._active_handle.get_galois_load_time_seconds())
        except Exception:
            return -1.0

    # ------------------------------------------------------------------ cleanup
    def close(self) -> None:
        """Release the active session's native resources."""
        self.backend.deactivate()
        self._active_handle = None
        self._active_user = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc, tb):
        self.close()
        return False
