#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 FHE Inference — Client/Server (HTTP). Two modes (--mode):

  plaintext  (Step 1): float32 in/out, run_session (server holds full keys,
              incl. SK/PK — encrypts/decrypts server-side). POST /predict.
  ciphertext (Step 2): encrypted-in/encrypted-out, run_cipher (server is
              eval-only — loads RLK+GK only, NO SK/PK). POST /predict_cipher.
              Client encrypts with its PK, decrypts with its SK; the server
              never sees plaintext or secret keys.

Server holds ONE FHERuntime (process-wide singleton) with K resident sessions.
Requests route by `user_id % K` to a dedicated single-worker
ThreadPoolExecutor per slot — same-slot requests serialize, K slots run in
parallel (Mode B-P, per-thread Activate_session). The 06_server_parallel_pool
K-pool model wrapped in a FastAPI HTTP layer.

Key separation (ciphertext mode):
  server  --mode ciphertext  → KeyMode 'preload'  (RLK+GK only, no SK/PK)
  client  --mode ciphertext  → KeyMode 'on_demand_full' (SK+PK+RLK only,
           galois NOT loaded — client never evals, so it never needs the 21 GiB
           galois set). Encrypts inputs / decrypts outputs via Kms_encrypt_input
           / Kms_decrypt_output C ABI (no Python binding for those yet).

Warmup:
  plaintext  — server warms each slot via run_session at startup.
  ciphertext — server has no SK, so it cannot run_session. Instead the client
              POSTs a sample ciphertext to /warmup_cipher per slot at startup
              (run_cipher absorbs lazy setup + bootstrap ensure); then the
              timed /predict_cipher request is cold-cache-free.

SLA: inference wrapped in future.result(timeout=INFER_TIMEOUT_S). Timeout→503.

Usage:
    # Step 1 (plaintext)
    CUDA_VISIBLE_DEVICES=<idle> python3 examples/11_client_server/server.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20 --port 8000 --num-users 3 \\
        --io staged --mode plaintext
    python3 examples/11_client_server/client.py --url http://localhost:8000 --user-id 0

    # Step 2 (ciphertext, server eval-only / no SK)
    CUDA_VISIBLE_DEVICES=<idle> python3 examples/11_client_server/server.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20 --port 8000 --num-users 1 \\
        --io staged --mode ciphertext
    python3 examples/11_client_server/client.py --url http://localhost:8000 \\
        --mode ciphertext --key-root /mnt/nvme0/mu_keys_resnet20 --user-id 0
"""

import argparse
import json
import os
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from concurrent.futures import TimeoutError as FuturesTimeout

import torch

from ace import fhe
from ace.fhe.config import IoPolicy, IoPolicySettings, KeyMode
from ace.fhe.util import gpu_available
from ace.model.dataset import load_cifar10_images


# _compile_once lives in examples/10_multi_user/resnet20/generate_keys.py.
_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(_HERE, "..", "10_multi_user", "resnet20"))
from generate_keys import _compile_once  # noqa: E402


# --- Globals populated at startup (single FHERuntime + K slot pools) ---
runner: fhe.FHERuntime = None  # type: ignore[assignment]
slot_pools: list = []  # K ThreadPoolExecutor(max_workers=1)
NUM_SESSIONS = 0
INFER_TIMEOUT_S = 30.0  # 8.5s single / ~16s under 3-way GPU contention (report.md K=3 wall 16.2s) + margin
INPUT_SHAPE = (1, 3, 32, 32)  # resnet20 CIFAR-10 normalized input
OUTPUT_LEN = 10  # resnet20 logits
MODE = "plaintext"  # "plaintext" (Step 1) | "ciphertext" (Step 2, run_cipher eval-only)


def _build_runtime(args) -> None:
    """Compile once, build K sessions, warm them, wire K slot pools.
    Directly follows examples/10_multi_user/resnet20/06_server_parallel_pool.py
    L131-183 (preheat + K-pool). Ensure_bootstrap_keys fires inside C++ KeyMgr
    during Init/create_session (sync-hoist, commit 51da15b) — no explicit call."""
    global runner, slot_pools, NUM_SESSIONS, MODE

    MODE = args.mode
    # Ciphertext mode: server is eval-only — load just RLK+GK (KeyMode::PRELOAD,
    # no SK/PK, fhe_lib/phantom/src/kms/key_mgr.cu:90-118). run_session cannot
    # run here (it encrypts/decrypts → needs SK), so warmup is via run_cipher on
    # a client-supplied sample ciphertext (see /warmup_cipher), not run_session.
    # Plaintext mode: full keys (encrypt/decrypt), warmup via run_session.
    key_mode = "preload" if MODE == "ciphertext" else args.key_mode

    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(
        policy={"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}[
            args.io
        ]
    ).apply()
    KeyMode.apply(key_mode)

    root = os.path.abspath(args.key_root)
    with open(os.path.join(root, "meta.json")) as fp:
        meta = json.load(fp)
    if meta.get("spec") != "resnet20":
        raise RuntimeError(f"meta.json spec='{meta.get('spec')}', expected 'resnet20'")
    all_user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    K = args.num_users
    if len(all_user_dirs) < K:
        raise RuntimeError(f"--num-users {K} exceeds key bundles ({len(all_user_dirs)})")
    user_dirs = all_user_dirs[:K]
    if not gpu_available():
        raise RuntimeError("GPU not available")

    print("=" * 70, flush=True)
    print(f"ResNet-20 Client/Server — K={K} sessions, mode={MODE}, key_mode={KeyMode.get()}", flush=True)
    if MODE == "ciphertext":
        print("  [ciphertext] server is EVAL-ONLY: loaded RLK+GK only (no SK/PK).", flush=True)
    print(f"  routing: user_id -> session (user_id % {K})", flush=True)
    print("=" * 70, flush=True)

    print("  compiling resnet20 ...", flush=True)
    t0 = time.perf_counter()
    package = _compile_once(meta.get("backend", "phantom"))
    print(f"  compiled ({time.perf_counter() - t0:.1f}s); kernel: {package['kernel']}", flush=True)

    pkg = {**package, "key_path": user_dirs[0], "key_mode": key_mode}
    t0 = time.perf_counter()
    runner = fhe.FHERuntime(pkg)
    runner.init()
    print(f"  session 0 init={time.perf_counter() - t0:5.1f}s", flush=True)

    for i in range(1, K):
        t0 = time.perf_counter()
        idx = runner.create_session(user_dirs[i])
        if idx < 0:
            runner.finalize()
            raise RuntimeError(f"create_session for k{i} failed (returned {idx})")
        print(f"  session {idx} (k{i}) create={time.perf_counter() - t0:5.1f}s", flush=True)

    try:
        free, total = torch.cuda.mem_get_info()
        print(f"  [hbm] after K={K} sessions: resident ≈ {(total - free) // (1024 * 1024)} MiB", flush=True)
    except Exception as e:
        print(f"  [hbm] mem_get_info failed: {e}", flush=True)

    # Warm all K sessions (absorb lazy kernel/NTT/encoder per session).
    # Plaintext: run_session warmup (encrypts/decrypts — needs SK/PK).
    # Ciphertext: server has no SK/PK, so run_session would throw; warmup is
    # deferred to /warmup_cipher (client POSTs a sample ciphertext, run_cipher
    # triggers lazy setup + bootstrap ensure). If Ensure_bootstrap_keys fails
    # under PRELOAD, that surfaces on the first /warmup_cipher call.
    if MODE == "plaintext":
        cifar_imgs, _ = load_cifar10_images(1)
        warmup_x = (cifar_imgs[0:1].float(),)
        for s in range(K):
            t0 = time.perf_counter()
            _ = runner.run_session(s, *warmup_x)
            print(f"  warmup session {s}: {time.perf_counter() - t0:.1f}s", flush=True)
    else:
        print("  [ciphertext] warmup deferred — client drives /warmup_cipher per slot.", flush=True)

    slot_pools = [ThreadPoolExecutor(max_workers=1) for _ in range(K)]
    NUM_SESSIONS = K
    print(f"  ready: {K} slot pools online (timeout={INFER_TIMEOUT_S}s)", flush=True)


def _infer(slot: int, x: torch.Tensor) -> torch.Tensor:
    """Submit run_session(slot, x) to that slot's dedicated worker, wait."""
    fut = slot_pools[slot].submit(runner.run_session, slot, x)
    return fut.result(timeout=INFER_TIMEOUT_S)


def _infer_cipher(slot: int, input_bytes: bytes):
    """Submit run_cipher(slot, [input_bytes], 'output') to that slot's worker.

    Server eval-only (no SK/PK): deserializes the client-encrypted input
    ciphertext, evals with the session's RLK+GK, returns serialized output
    ciphertext bytes. Also returns the per-stage run_cipher timing (read on the
    SAME worker thread that ran run_cipher — the timing is thread-local).

    Returns (out_bytes, timing_or_None, runcipher_wall) where timing is
    [deser_sync, launder_sync, kernel, serialize] seconds (None/[] if the
    backend doesn't export the timing getter), and runcipher_wall is the wall
    of runner.run_cipher itself on the worker thread (to localize where the
    server route+threadpool overhead actually lives — between server_wall and
    the RunCipher-internal 4-stage sum).
    """

    def _work():
        t_rc0 = time.perf_counter()
        out = runner.run_cipher(slot, [input_bytes], "output")  # runs on THIS worker thread
        t_rc = time.perf_counter() - t_rc0
        # Thread-local timing — MUST read here on the same worker, not outside.
        timing = runner._executor.last_runcipher_timing()
        return out, (timing if timing else None), t_rc

    fut = slot_pools[slot].submit(_work)
    return fut.result(timeout=INFER_TIMEOUT_S)


def _warmup_cipher(slot: int, input_bytes: bytes) -> None:
    """Run_cipher once on a client-supplied sample ciphertext to absorb lazy
    setup (NTT/encoder/bootstrap ensure) on this slot. Output is discarded.
    First call per slot is the slow one (galois-key/bootstrap residency)."""
    fut = slot_pools[slot].submit(runner.run_cipher, slot, [input_bytes], "output")
    fut.result(timeout=INFER_TIMEOUT_S)


def make_app():
    from fastapi import FastAPI, HTTPException, Query, Request
    from fastapi.concurrency import run_in_threadpool
    from fastapi.responses import Response

    app = FastAPI(title="ace resnet20 fhe infer")

    @app.on_event("startup")
    async def _startup():
        # _build_runtime is invoked from main() before uvicorn.run to surface
        # errors synchronously; this hook is a no-op marker.
        pass

    @app.get("/health")
    async def health():
        return {"ready": NUM_SESSIONS > 0, "num_sessions": NUM_SESSIONS}

    @app.post("/predict")
    async def predict(request: Request, user_id: int = Query(...)):
        # async endpoint so request.body() (a coroutine) works. The blocking
        # run_session (~8.5s) is dispatched to a starlette worker thread via
        # run_in_threadpool — the event loop stays free, so K concurrent
        # requests each block in their own worker thread while the K slot_pools
        # (one ThreadPoolExecutor(1) per session) run run_session in genuine
        # parallel (Mode B-P per-thread Activate_session). Without this, a
        # synchronous block in an async endpoint serializes on the single
        # event loop (observed 9s → 18s → 27s stair-step).
        if NUM_SESSIONS == 0:
            raise HTTPException(status_code=503, detail="server not ready")
        body = await request.body()
        expect = INPUT_SHAPE[0] * INPUT_SHAPE[1] * INPUT_SHAPE[2] * INPUT_SHAPE[3] * 4
        if len(body) != expect:
            raise HTTPException(
                status_code=400,
                detail=f"input bytes {len(body)} != expected {expect} (float32 {INPUT_SHAPE})",
            )
        x = torch.frombuffer(bytearray(body), dtype=torch.float32).reshape(INPUT_SHAPE)

        slot = user_id % NUM_SESSIONS
        t0 = time.perf_counter()
        try:
            out = await run_in_threadpool(_infer, slot, x)
        except FuturesTimeout:
            raise HTTPException(status_code=503, detail=f"inference timeout > {INFER_TIMEOUT_S}s") from None
        dt = time.perf_counter() - t0

        logits = out.float().reshape(-1)[:OUTPUT_LEN].contiguous()
        print(f"  [infer] user_id={user_id} slot={slot} dt={dt:.2f}s", flush=True)
        return Response(content=logits.numpy().tobytes(), media_type="application/octet-stream")

    @app.post("/predict_cipher")
    async def predict_cipher(request: Request, user_id: int = Query(...)):
        # Ciphertext-in/out (Step 2): body is a serialized input ciphertext
        # (client-encrypted with PK); server evals via run_cipher (RLK+GK only,
        # no SK) and returns the serialized output ciphertext. Like /predict,
        # the blocking run_cipher is dispatched to a worker thread via
        # run_in_threadpool so K concurrent requests run in genuine parallel
        # (Mode B-P) instead of serializing on the event loop.
        if NUM_SESSIONS == 0:
            raise HTTPException(status_code=503, detail="server not ready")
        body = await request.body()
        if not body:
            raise HTTPException(status_code=400, detail="empty ciphertext body")

        slot = user_id % NUM_SESSIONS
        t0 = time.perf_counter()
        try:
            out_bytes, timing, rc_wall = await run_in_threadpool(_infer_cipher, slot, bytes(body))
        except FuturesTimeout:
            raise HTTPException(status_code=503, detail=f"inference timeout > {INFER_TIMEOUT_S}s") from None
        dt = time.perf_counter() - t0
        print(
            f"  [cipher] user_id={user_id} slot={slot} dt={dt:.2f}s runcipher={rc_wall:.2f}s "
            f"in={len(body)}B out={len(out_bytes)}B",
            flush=True,
        )
        # Echo the server-side per-stage split back to the client (server & client
        # are separate environments — the client can't read server logs). Headers
        # so the binary ciphertext body stays untouched.
        headers = {"X-RC-Slot": str(slot), "X-RC-Wall": f"{dt:.3f}", "X-RC-RunCipherWall": f"{rc_wall:.3f}"}
        if timing is not None:
            headers["X-RC-Deser"] = f"{timing[0]:.3f}"
            headers["X-RC-Launder"] = f"{timing[1]:.3f}"
            headers["X-RC-Kernel"] = f"{timing[2]:.3f}"
            headers["X-RC-Serialize"] = f"{timing[3]:.3f}"
        return Response(content=out_bytes, media_type="application/octet-stream", headers=headers)

    @app.post("/warmup_cipher")
    async def warmup_cipher(request: Request, user_id: int = Query(...)):
        # Warmup handshake (ciphertext mode only): client POSTs a sample
        # ciphertext; server run_cipher-once on that slot to absorb lazy setup.
        # Output discarded. Lets the client time the real request cold-cache-free.
        if NUM_SESSIONS == 0:
            raise HTTPException(status_code=503, detail="server not ready")
        body = await request.body()
        if not body:
            raise HTTPException(status_code=400, detail="empty warmup ciphertext body")
        slot = user_id % NUM_SESSIONS
        t0 = time.perf_counter()
        try:
            await run_in_threadpool(_warmup_cipher, slot, bytes(body))
        except FuturesTimeout:
            raise HTTPException(status_code=503, detail=f"warmup timeout > {INFER_TIMEOUT_S}s") from None
        print(f"  [warmup-cipher] user_id={user_id} slot={slot} dt={time.perf_counter() - t0:.2f}s", flush=True)
        return {"ok": True, "slot": slot}

    return app


def main():
    ap = argparse.ArgumentParser(description="ResNet-20 FHE Client/Server (HTTP)")
    ap.add_argument("--key-root", required=True, help="Key-root with meta.json (from resnet20/generate_keys.py)")
    ap.add_argument("--port", type=int, default=8000, help="HTTP port (default 8000)")
    ap.add_argument("--num-users", type=int, default=3, help="K: resident sessions / concurrency (default 3)")
    ap.add_argument("--io", choices=["auto", "gds", "staged", "mapped"], default="staged", help="IO policy")
    ap.add_argument(
        "--mode",
        choices=["plaintext", "ciphertext"],
        default="plaintext",
        help="plaintext (Step 1, run_session, full keys) | ciphertext (Step 2, run_cipher, "
        "server eval-only — loads RLK+GK only, no SK/PK). default plaintext",
    )
    ap.add_argument(
        "--key-mode",
        choices=["preload_full", "on_demand_full"],
        default="on_demand_full",
        help="Key residency for PLAINTEXT mode only (default on_demand_full). "
        "ciphertext mode forces 'preload' (eval-only, no SK/PK).",
    )
    ap.add_argument("--host", default="0.0.0.0", help="bind host")
    args = ap.parse_args()

    _build_runtime(args)  # compile + K sessions + warmup (synchronous, before serve)
    app = make_app()
    import uvicorn

    uvicorn.run(app, host=args.host, port=args.port, log_level="info")


if __name__ == "__main__":
    main()
