#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import concurrent.futures as cf
import logging
import os
import time
from typing import Any, Optional

import torch


logger = logging.getLogger(__name__)

import contextlib  # noqa: E402

from ace import runtime as _C  # noqa: E402

from .executor import KernelExecutor  # noqa: E402
from .loader import ConfigLoader  # noqa: E402
from .results import BatchResult, BatchTiming  # noqa: E402


# Map Python logging levels to C++ spdlog level strings
_PY_TO_SPDLOG_LEVEL = {
    logging.DEBUG: "DEBUG",
    logging.INFO: "INFO",
    logging.WARNING: "WARN",
    logging.ERROR: "ERROR",
    logging.CRITICAL: "CRITICAL",
}


def _sync_log_level():
    """Sync Python logger level to C++ spdlog."""
    rt_logger = logging.getLogger("ace.runtime")
    level = rt_logger.getEffectiveLevel()
    spdlog_level = _PY_TO_SPDLOG_LEVEL.get(level, "INFO")
    with contextlib.suppress(Exception):
        _C.set_log_level(spdlog_level)


class FHERuntime:
    """FHE inference runtime.

    Manages configuration, kernel lifecycle, and provides inference APIs
    for single-image, batch, and dataset execution.

    Note: The C++ FHE runtime uses process-wide state. Only one FHERuntime
    instance should be active at a time. Creating a new FHERuntime will
    automatically finalize the previous one.
    """

    _active_instance = None

    def __init__(
        self,
        package,
        *,
        auto_init: bool = False,
        validate_atol: float | None = None,
        validate_rtol: float | None = None,
    ):
        """
        Args:
            package: Model package — either a dict with keys:
                - model: Model name (used to derive config path)
                - kernel: Path to compiled FHE shared library
                - config_path: (optional) Explicit path to config file
                - key_path: (optional) Path to pre-generated key bundle directory.
                  When set, keys are loaded from this directory instead of being
                  generated at runtime.
                - key_mode: (optional) Key mode string:
                  'online' (default), 'preload', 'preload_full', 'on_demand',
                  'on_demand_full', or backward-compat 'key_offline', 'server_only'.
                  Controls which keys are loaded/generated.  Modes other than
                  'online' require key_path to be set.
                - load_full_keys: (optional, deprecated) When True and mode is
                  'preload'/'on_demand', upgrades to 'preload_full'/'on_demand_full'.
                  Prefer using 'preload_full' directly instead.
              or a CompiledProgram instance (its .package dict is used).
            auto_init: If True, eagerly initialize the FHE context (key
              generation/loading) at construction. Default False preserves the
              lazy behavior — the context is created on first run()/run_batch().
            validate_atol: Absolute error tolerance for validate(). Reserved —
              the C++ Validate_result binding does not yet accept per-call
              thresholds (currently env vars ABS_ERROR/REL_ERROR). Passing a
              non-None value raises NotImplementedError until the binding is
              extended.
            validate_rtol: Relative error tolerance for validate(). Reserved —
              see validate_atol.
        """
        # Finalize previous instance to avoid process-wide state conflicts
        self._cleanup_previous()

        # Accept CompiledProgram objects directly
        if hasattr(package, "package") and isinstance(package.package, dict):
            package = package.package
        self.package = package

        # Validate-threshold config (reserved; C++ binding extension pending).
        self._validate_atol = validate_atol
        self._validate_rtol = validate_rtol
        # Track whether this instance's FHE context has been torn down, so
        # finalize()/__exit__/__del__ and _cleanup_previous() are idempotent.
        self._finalized = False
        self._auto_init = auto_init

        # Load FHE configuration
        config_path = package.get("config_path", f"{package['model']}.conf")
        # Pick up key_path from package, or from the C++ global state if it
        # was set externally (e.g. via _C.set_key_path).  Tracking the
        # effective key_path ensures finalize() always clears it.
        self.key_path = package.get("key_path", None) or (_C.get_key_path() or None)
        self.config_loader = ConfigLoader(config_path)

        if self.key_path:
            _C.set_key_path(str(self.key_path))
            logger.info("Key path set: %s (pre-generated keys will be loaded)", self.key_path)

        # Set key mode (controls which keys are loaded/generated)
        from ..config.key_mode import KeyMode

        key_mode = package.get("key_mode", None)
        load_full_keys = package.get("load_full_keys", None)
        if key_mode is not None:
            key_mode = KeyMode._validate(key_mode)
            # Validate: modes that need key_path
            if key_mode in KeyMode._REQUIRES_KEY_PATH and not self.key_path:
                raise ValueError(f"key_mode='{key_mode}' requires key_path to be set")
            # Backward compat: load_full_keys=True upgrades preload→preload_full
            if load_full_keys is True and key_mode in ("preload", "on_demand"):
                key_mode = key_mode + "_full"
            KeyMode.apply(key_mode)
            self.key_mode = key_mode
            logger.info("Key mode set: %s", self.key_mode)
        elif self.key_path:
            # Key path is set but no explicit mode — default to preload_full
            # for backward compatibility (previous 'key_offline' behavior)
            KeyMode.apply(KeyMode.PRELOAD_FULL)
            self.key_mode = KeyMode.PRELOAD_FULL
            logger.info("Key path set without key_mode; defaulting to preload_full")
        else:
            # No key_mode and no key_path — reset to ONLINE (default)
            KeyMode.apply(KeyMode.ONLINE)
            self.key_mode = KeyMode.ONLINE

        # Register and hold the single kernel executor
        self._executor = KernelExecutor("kernel", package["kernel"])

        # Register as active instance
        FHERuntime._active_instance = self

        _sync_log_level()

        if self._auto_init:
            self.init()

    @classmethod
    def _sync_gpu(cls):
        """Synchronize the GPU before tearing down an FHE context.

        phantom's Finalize_context destroys the live FheContext (PhantomContext
        + GPU keys + evaluator + cuFile); if torch/CUDA async ops are still in
        flight when that runs, the next FheContext can race on recycled GPU
        state and produce numerically exploded results (see KI-1 in
        .claude/known-issues.md). Finalizing only after a device synchronize
        avoids the leak. No-op on CPU / when CUDA is unavailable.
        """
        with contextlib.suppress(Exception):
            if torch.cuda.is_available():
                torch.cuda.synchronize()

    @classmethod
    def _cleanup_previous(cls):
        """Finalize and release the previous active runtime instance."""
        prev = cls._active_instance
        if prev is not None:
            cls._sync_gpu()
            with contextlib.suppress(Exception):
                prev._executor.finalize()
            prev._executor = None
            # Mark as finalized so the displaced instance's __del__ is a no-op
            # and finalize() becomes idempotent.
            prev._finalized = True
            cls._active_instance = None

    def init(self):
        """Initialize FHE context (key generation, etc.).

        Call once before multiple run() calls to avoid per-call
         context setup. run()/run_batch() will auto-call this if not
         already done.
        """
        self._executor.init()

    def finalize(self):
        """Finalize FHE context and release resources.

        Call once after all run()/run_batch() calls are done.  Resets
        global C++ state (key_path, key_mode) to safe defaults so that
        a subsequent FHERuntime starts from a clean state.

        Idempotent: a no-op on already-finalized instances.
        """
        if self._finalized:
            return
        self._finalized = True
        self._sync_gpu()
        self._executor.finalize()
        # Clear key path from C++ runtime when this runtime is done
        if self.key_path:
            _C.clear_key_path()
        # Reset inference mode to ONLINE (default) to prevent stale mode
        # from contaminating subsequent FHERuntime instances that don't
        # explicitly set key_mode.
        from ..config.key_mode import KeyMode

        _C.set_key_mode(KeyMode._to_cpp(KeyMode.ONLINE))

    # ------------------------------------------------------------------
    # Deterministic scoping — RAII-equivalent for the heavy native FHE
    # context. Prefer `with FHERuntime(pkg) as rt:` over relying on
    # __del__ (GC timing is nondeterministic and the C++ Finalize_context
    # must drain the device first).
    # ------------------------------------------------------------------
    def __enter__(self):
        """Enter context scope. Does not auto-init (lazy by default)."""
        return self

    def __exit__(self, exc_type, exc, tb):
        """Release the FHE context on scope exit. Does not suppress exceptions."""
        self.finalize()
        return False

    def __del__(self):
        """Best-effort release fallback. Never the primary path.

        Only releases the executor's native resources; avoids touching the
        `_C` global state (clear_key_path / set_key_mode) since the module
        may already be torn down at interpreter exit and GC ordering is
        nondeterministic. Use `with FHERuntime(...) as rt:` or explicit
        finalize() for guaranteed, complete teardown.
        """
        with contextlib.suppress(Exception):
            if not self._finalized and self._executor is not None:
                self._executor.finalize()
                self._finalized = True

    # ------------------------------------------------------------------
    # KMS key management — requires init()/run() first (active FheContext).
    # Replaces the retired ctypes FHert_* path; see fhe/kms/kms_runtime_api.h.
    # ------------------------------------------------------------------
    def save_keys(self, key_dir: str, eval_only: bool = False) -> int:
        """Save keys to ``key_dir``.

        Args:
            key_dir: Output directory for the key bundle.
            eval_only: If True, save only eval keys (RLK+GK), no SK/PK.

        Returns:
            0 on success, -1 on failure.
        """
        if eval_only:
            return self._executor.save_eval_keys(str(key_dir))
        return self._executor.save_keys(str(key_dir))

    def load_keys(self, key_dir: str, eval_only: bool = False) -> int:
        """Load keys from ``key_dir``.

        Args:
            key_dir: Key bundle directory to load from.
            eval_only: If True, load only eval keys (RLK+GK).

        Returns:
            0 on success, -1 on failure.
        """
        if eval_only:
            return self._executor.load_eval_keys(str(key_dir))
        return self._executor.load_keys(str(key_dir))

    def keys_loaded_from_kms(self) -> int:
        """Return 1 if keys were loaded from KMS, 0 if generated at runtime."""
        return self._executor.keys_loaded_from_kms()

    def get_galois_load_time_seconds(self) -> float:
        """Wall-clock seconds of the most recent galois-key load (-1.0 if none)."""
        return self._executor.get_galois_load_time_seconds()

    # ------------------------------------------------------------------
    # Multi-user Mode B — shared context + N key bundles, switch by index.
    # Requires init() first (an active FheContext).
    # ------------------------------------------------------------------
    def create_session(self, key_dir: str) -> int:
        """Create an extra session sharing this context, loading a full key
        bundle from ``key_dir``. Returns session index (>=1) on success, -1
        on failure. The new session's galois elements must match the primary's.
        """
        return self._executor.create_session(key_dir)

    def switch_session(self, idx: int) -> int:
        """Switch active session by index: 0 = primary, >=1 = extra session
        from create_session(). Returns 0 on success, -1 on failure.
        """
        return self._executor.switch_session(idx)

    def release_session(self, idx: int) -> int:
        """Release all galois-key slots for session idx (free their HBM). The
        session structure (evaluator, GaloisTool, BtsMgr) stays intact; only
        galois key bytes are evicted. A subsequent ``run_session`` re-loads
        keys from disk on demand via Ensure_galois_key. Returns 0 on success,
        -1 on failure. Call this when a user is done reclaiming HBM for
        other users.
        """
        return self._executor.release_session(idx)

    @property
    def num_sessions(self) -> int:
        """Number of sessions currently resident (primary + extra from create_session)."""
        return self._executor.num_sessions()

    def run_session(self, session_idx: int, *input_tensors: torch.Tensor) -> torch.Tensor:
        """Mode B-P: run inference bound to a SPECIFIC session on the calling
        thread. Concurrency-safe: two worker threads calling
        run_session(i, x) / run_session(j, x) execute in parallel.

        Each session has its own evaluator+keys+CUDA stream (phantom) / device
        context (ace); Activate_session binds the calling thread to session_idx
        before kernel execution. Thus unlike run(), no switch_session is needed
        before run_session — and run_session is the primitive for parallel
        multi-user serving.

        Args:
            session_idx: 0 = primary, >=1 = extra session (create_session index).
        """
        with torch.profiler.record_function("fhe::run_session"):
            input_tensors_4d = [self._to_4d(t) for t in input_tensors]
            return self._executor.execute_session(session_idx, *input_tensors_4d)

    def run_cipher(self, session_idx: int, input_bytes: list, output_name: str = "output") -> bytes:
        """Mode B-P + ciphertext-in/out (P2''). Like run_session but accepts
        serialized input ciphertext bytes (client-encrypted) per input, evals
        with the session's keys (no encrypt/decrypt — server eval-only), and
        returns the serialized output ciphertext bytes. Concurrency-safe across
        sessions (per-session Io mutex; GIL released in native).

        ``input_bytes`` is a list of `bytes`, ordered to match the compiled
        input scheme (runtime maps each to get_encode_scheme_c(i)->_name).
        """
        with torch.profiler.record_function("fhe::run_cipher"):
            return self._executor.run_cipher(session_idx, input_bytes, output_name)

    def run(self, *input_tensors: torch.Tensor) -> torch.Tensor:
        """Run a single FHE inference on the primary session (idx=0).

        Delegates to ``run_session(0, *input_tensors)`` — all single-inference
        calls share one implementation. Equivalent to ``run_session(0, ...)``.

        Args:
            *input_tensors: Input tensors (auto-converted to 4D).

        Returns:
            Output tensor from FHE computation.
        """
        return self.run_session(0, *input_tensors)

    def run_batch(
        self,
        batch_inputs: list,
        parallel: bool = False,
        num_threads: int = 0,
        verbose: bool = False,
    ) -> BatchResult:
        """Run FHE inference on a batch of inputs.

        Single-image path: ``parallel=False`` dispatches each image serially
        through the C++ batch runner (backends share an io_mutex per-step).

        Parallel paths by backend:
          - **GPU (phantom/ace)**: ``parallel=True`` auto-creates per-image
            sessions sharing the primary's ``key_dir``, then dispatches via
            ``ThreadPoolExecutor`` → ``run_session(i, inputs_i)``.  Each session
            has its own evaluator + CUDA stream, so there is no scratch-buffer
            race.  CUDA Graph is NOT used here (per-session streams are
            incompatible with capture/replay).
          - **CPU (ant)**: ``parallel=True`` uses the C++ OpenMP batch runner
            (``#pragma omp parallel for`` with critical-section I/O).

        Args:
            batch_inputs: list of input tensor lists, one per image.
                Each element can be a single tensor or a list/tuple of tensors.
            parallel: If True, evaluate images in parallel (auto-selects
                ThreadPool for GPU, OpenMP for CPU).
            num_threads: Max worker threads (0 = auto: min(N, cpu_count)).
            verbose: If True, print per-image progress.

        Returns:
            BatchResult with outputs, timing, and success/failure counts.
        """
        # Ensure each item's tensors are 4D
        prepared = []
        for item in batch_inputs:
            item = [self._to_4d(item)] if isinstance(item, torch.Tensor) else [self._to_4d(t) for t in item]
            prepared.append(item)
        N = len(prepared)
        if N == 0:
            return BatchResult(outputs=[], timing=BatchTiming(0, 0, 0, 0, 0), num_success=0, num_failure=0)

        # ── Detect backend by kernel path: GPU kernels are named for the
        #     phantom/ace targets; CPU antlib for "ant". Do NOT query
        #     self.num_sessions here — before Init() the backend's
        #     Kms_num_sessions → FheContext::Current() asserts on a null
        #     thread-local current context (phantom), so any run_batch call
        #     before init would crash. ──
        kernel_path = self.package.get("kernel", "")
        _gpu = "-phantom-" in kernel_path or "-ace-" in kernel_path

        if parallel and _gpu:
            # ── GPU ThreadPool path (was run_batch_parallel) ──
            with torch.profiler.record_function("fhe::run_batch_parallel"):
                # Auto-create sessions: need at least N sessions (0..N-1).
                current_sessions = self.num_sessions
                key_dir = self.key_path
                if key_dir is None:
                    raise ValueError(
                        "FHERuntime.run_batch(parallel=True) requires key_path to be set "
                        "(key_mode must be preload|preload_full|on_demand|on_demand_full). "
                        "The primary session was created without a key_dir, so we cannot "
                        "create extra sessions sharing the same keys."
                    )
                while current_sessions < N:
                    rc = self.create_session(key_dir)
                    if rc < 0:
                        raise RuntimeError(
                            f"create_session failed for image {current_sessions} "
                            f"(session_idx would be {current_sessions}, key_dir={key_dir})"
                        )
                    current_sessions = self.num_sessions

                results = [None] * N
                timings = [0.0] * N
                exceptions: list[Exception | None] = [None] * N

                def _run_one(i: int, inputs_4d: list[torch.Tensor]):
                    t0 = time.perf_counter()
                    try:
                        return self.run_session(i, *inputs_4d)
                    except Exception as e:
                        exceptions[i] = e
                        return None
                    finally:
                        timings[i] = time.perf_counter() - t0

                max_workers = num_threads or min(N, os.cpu_count() or 4)
                max_workers = max(1, max_workers)
                with cf.ThreadPoolExecutor(max_workers=max_workers) as pool:
                    futures = {pool.submit(_run_one, i, prepared[i]): i for i in range(N)}
                    for fut in cf.as_completed(futures):
                        i = futures[fut]
                        try:
                            results[i] = fut.result()
                        except Exception as e:
                            exceptions[i] = e

                # Collect results
                outputs = []
                num_success = sum(1 for r in results if r is not None)
                num_failure = N - num_success
                for r in results:
                    outputs.append(r if r is not None else torch.zeros(self._get_output_shape()))

                # Build timing
                total_ms = sum(timings) * 1000.0
                valid_times = [t * 1000.0 for t in timings if t > 0]
                max_ms = max(valid_times) if valid_times else 0.0
                timing = BatchTiming(
                    total_ms=total_ms,
                    avg_per_image_ms=total_ms / max(N, 1),
                    min_image_ms=min(valid_times) if valid_times else 0.0,
                    max_image_ms=max_ms,
                    num_images=N,
                )
                if verbose:
                    for i in range(N):
                        tag = f"{'OK' if results[i] is not None else 'FAIL'}"
                        millis = timings[i] * 1000.0
                        logger.info(f"[Batch {i + 1}/{N}] {tag}  time={millis:.1f}ms")
                return BatchResult(outputs=outputs, timing=timing, num_success=num_success, num_failure=num_failure)
        else:
            # ── C++ batch runner: serial or OpenMP (CPU only) ──
            with torch.profiler.record_function("fhe::run_batch"):
                return self._executor.execute_batch(
                    prepared, parallel=parallel, num_threads=num_threads, verbose=verbose
                )

    def _get_output_shape(self) -> list[int]:
        """Derive output tensor shape from the compiled config. Cached per init."""
        # Lazy-load from C++ config accessor.
        from ace import runtime as _C

        try:
            cfg_summary = _C.config_summary()
            # Simple heuristic: try to decode from decode_sch in the JSON
            import json

            data = json.loads(cfg_summary)
            decode_sch = data.get("_decode_sch", [])
            if decode_sch and len(decode_sch) > 0:
                ds = decode_sch[0]
                shape = ds.get("_shape", {})
                return [shape.get("n", 1), shape.get("c", 1), shape.get("h", 1), shape.get("w", 1)]
        except Exception:
            pass
        return [1, 1, 1, 1]

    def validate(self, result: torch.Tensor, expected: torch.Tensor) -> bool:
        """Validate FHE result against expected tensor.

        Compares the FHE inference result with the expected plaintext result
        using absolute and relative error thresholds.

        Args:
            result: FHE inference result tensor
            expected: Expected (plaintext) tensor

        Returns:
            True if validation passes (within error thresholds)

        Note:
            Thresholds are currently env-var driven (ABS_ERROR / REL_ERROR);
            the constructor's ``validate_atol`` / ``validate_rtol`` params are
            reserved until the C++ Validate_result binding is extended.
        """
        return self._executor.validate(result, expected, atol=self._validate_atol, rtol=self._validate_rtol)

    @staticmethod
    def _to_4d(tensor: torch.Tensor) -> torch.Tensor:
        """Ensure tensor is 4D (N, C, H, W)."""
        return tensor if tensor.dim() == 4 else tensor.unsqueeze(1).unsqueeze(2)
