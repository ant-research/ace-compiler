#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import logging

import torch

from ace import runtime as _C

from .results import BatchResult


logger = logging.getLogger(__name__)


class KernelExecutor:
    """Manages the lifecycle of a single FHE kernel (.so).

    Wraps the C++ ProviderManager for kernel loading, context initialization,
    single-image and batch inference, and validation.
    """

    def __init__(self, name: str, lib_path: str, use_cuda_graph: bool = False):
        """
        Args:
            name: Identifier for this kernel (e.g. "kernel").
            lib_path: Path to the compiled FHE shared library.
            use_cuda_graph: If True, capture CUDA graph on first Execute()
                and replay on subsequent calls to reduce launch overhead.
                Only effective for GPU backends. Falls back to normal
                execution if graph capture fails.
        """
        self.name = name
        self.lib_path = lib_path

        self._manager = _C.ProviderManager()
        self._manager.register_kernel(name, lib_path, use_cuda_graph)

        self._initialized = False

    def init(self):
        """Initialize FHE context (key generation, etc.).

        Call once before multiple execute() calls to avoid per-inference context setup.
        execute() will auto-call this if not already done (backward compatible).
        """
        if not self._initialized:
            self._manager.init(self.name)
            self._initialized = True

    def finalize(self):
        """Finalize FHE context and release resources.

        Call once after all execute() calls are done.
        After this, init() must be called again before the next execute().
        """
        if self._initialized:
            self._manager.finalize(self.name)
            self._initialized = False

    def capture_graph(self) -> bool:
        """Attempt CUDA Graph capture for the Execute() phase.

        Must be called after init(). Runs Main_graph() once under stream
        capture. If successful, subsequent phase-split Execute() calls
        will replay the captured graph, reducing kernel launch overhead.

        WARNING: Only call this if the kernel's Main_graph() does NOT call
        cudaMalloc/cudaFree internally (illegal during stream capture).
        Phantom backend is NOT compatible; use only with ACE's own kernels.

        Returns:
            True if capture succeeded, False otherwise.
        """
        return self._manager.capture_graph(self.name)

    # ------------------------------------------------------------------
    # KMS key management — requires init() first (active FheContext).
    # Forwards to KERNEL_RUNNER via ProviderManager; replaces the retired
    # ctypes FHert_* path (see fhe/kms/kms_runtime_api.h).
    # ------------------------------------------------------------------
    def save_keys(self, key_dir: str) -> int:
        """Save all keys (SK+PK+RLK+GK) to ``key_dir``; 0 on success, -1 on failure."""
        return self._manager.save_keys(self.name, str(key_dir))

    def save_eval_keys(self, key_dir: str) -> int:
        """Save evaluation keys (RLK+GK) only; 0 on success, -1 on failure."""
        return self._manager.save_eval_keys(self.name, str(key_dir))

    def load_keys(self, key_dir: str) -> int:
        """Load all keys from ``key_dir``; 0 on success, -1 on failure."""
        return self._manager.load_keys(self.name, str(key_dir))

    def load_eval_keys(self, key_dir: str) -> int:
        """Load evaluation keys (RLK+GK) only; 0 on success, -1 on failure."""
        return self._manager.load_eval_keys(self.name, str(key_dir))

    def keys_loaded_from_kms(self) -> int:
        """Return 1 if keys were loaded from KMS, 0 if generated at runtime."""
        return self._manager.keys_loaded_from_kms(self.name)

    def get_galois_load_time_seconds(self) -> float:
        """Wall-clock seconds of the most recent galois-key load (-1.0 if none)."""
        return self._manager.get_galois_load_time_seconds(self.name)

    def create_session(self, key_dir: str) -> int:
        """Mode B (multi-user): create an extra session sharing the active
        context, loading a full key bundle (SK+PK+RLK+GK) from ``key_dir``.

        Requires init() first. Returns the session index (>=1) on success,
        -1 on failure.
        """
        return self._manager.create_session(self.name, str(key_dir))

    def switch_session(self, idx: int) -> int:
        """Mode B: switch the active session by index (0=primary, >=1=extra).
        Returns 0 on success, -1 on failure.
        """
        return self._manager.switch_session(self.name, int(idx))

    def release_session(self, idx: int) -> int:
        """Release all galois-key slots for session idx (free HBM). Session
        structure stays; keys re-load on next use. Returns 0/-1.
        """
        return self._manager.release_session(self.name, int(idx))

    def num_sessions(self) -> int:
        """Return the number of sessions currently resident (primary + extra). -1 if not available."""
        return self._manager.num_sessions(self.name)

    def execute_session(self, session_idx: int, *input_tensors: torch.Tensor) -> torch.Tensor:
        """Mode B-P: run inference bound to a specific session on the calling
        thread. Concurrency-safe across sessions — two worker threads calling
        execute_session(i) / execute_session(j) run in parallel (GIL released in
        native Run_session). Each session uses its own evaluator+keys+CUDA
        stream; the compiled kernel's ops route to the calling thread's active
        session via Activate_session.

        Inputs MUST already be 4D (use FHERuntime.run_session, which converts).
        """
        return self._manager.execute_session(self.name, int(session_idx), [t for t in input_tensors])

    def run_cipher(self, session_idx: int, input_bytes: list, output_name: str = "output") -> bytes:
        """Mode B-P + ciphertext-in/out (P2''). Accept serialized input
        ciphertext bytes (one per input, ordered to match the compiled input
        scheme — runtime maps each to get_encode_scheme_c(i)->_name as the Io
        key). Eval with the session's keys (no encrypt/decrypt). Return the
        serialized output ciphertext bytes. GIL released in native.

        ``input_bytes`` must be a list of `bytes` objects (not torch tensors);
        no _to_4d conversion — these are already ciphertexts.
        """
        return self._manager.run_cipher(self.name, int(session_idx), [bytes(b) for b in input_bytes], output_name)

    def last_runcipher_timing(self) -> list:
        """Per-stage seconds [deser_sync, launder_sync, kernel(eval), serialize]
        of the most recent run_cipher on THIS thread. MUST be called on the SAME
        worker thread that ran run_cipher (the backend timing is thread-local);
        callers running run_cipher in a dedicated worker must read it there.
        Returns [] if the backend doesn't export the symbol (older .so)."""
        try:
            return list(self._manager.last_runcipher_timing(self.name))
        except Exception:
            return []

    def execute(self, *input_tensors: torch.Tensor) -> torch.Tensor:
        """Run single FHE inference.

        Auto-initializes FHE context if not already done.

        Args:
            *input_tensors: Input tensors (will be converted to 4D if needed).

        Returns:
            Output tensor from FHE computation.
        """
        self.init()
        return self._manager.execute(self.name, list(input_tensors))

    def execute_batch(
        self,
        batch_inputs: list,
        parallel: bool = False,
        num_threads: int = 0,
        verbose: bool = False,
    ) -> BatchResult:
        """Run batch FHE inference on multiple inputs.

        Args:
            batch_inputs: List of input tensor lists, one per image.
                Each element is a list/tuple of input tensors for a single
                inference call, e.g. [(tensor1,), (tensor2,), ...].
            parallel: If True, use OpenMP parallel batch execution.
            num_threads: Number of threads for parallel mode (0 = auto).
            verbose: If True, print per-image progress and results.

        Returns:
            BatchResult with outputs, timing, and success/failure counts.
        """
        self.init()

        # Ensure each item is a list of tensors
        prepared = []
        for item in batch_inputs:
            if isinstance(item, torch.Tensor):
                item = [item]
            prepared.append(list(item))

        cxx_result = self._manager.execute_batch(
            self.name, prepared, parallel=parallel, num_threads=num_threads, verbose=verbose
        )

        result = BatchResult.from_cxx(cxx_result)
        return result

    def validate(
        self, result: torch.Tensor, expected: torch.Tensor, *, atol: float | None = None, rtol: float | None = None
    ) -> bool:
        """Validate FHE result against expected tensor.

        Compares the FHE inference result with the expected plaintext result
        using absolute and relative error thresholds.

        Args:
            result: FHE inference result tensor
            expected: Expected (plaintext) tensor
            atol: Absolute error tolerance (reserved — requires a C++ binding
                extension; currently the threshold comes from the ABS_ERROR env
                var, default 1e-4).
            rtol: Relative error tolerance (reserved — requires a C++ binding
                extension; currently the threshold comes from the REL_ERROR env
                var, default 1e-3).

        Returns:
            True if validation passes (within error thresholds)
        """
        if atol is not None or rtol is not None:
            raise NotImplementedError(
                "Per-call validate thresholds (atol/rtol) require extending the "
                "C++ Validate_result binding. Today thresholds are env-var driven "
                "(ABS_ERROR / REL_ERROR). See FHERuntime.__init__ validate_atol/rtol."
            )
        return _C.validate_result(result.to(torch.float64), expected.to(torch.float64))
