#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 FHE Inference — Client (HTTP, pairs with server.py). Two modes:

  plaintext  (Step 1): send normalized CIFAR-10 image as raw float32 bytes
              (1x3x32x32, ~12 KB) via POST /predict, decode returned 1x10 logits.
              No encryption on the client.
  ciphertext (Step 2): encrypt the image with the client's PK (Kms_encrypt_input
              via ctypes — no Python binding yet), POST the ciphertext to
              /predict_cipher, decrypt the returned ciphertext with the client's
              SK (Kms_decrypt_output) → logits. Server is eval-only (RLK+GK,
              no SK/PK). Also POSTs /warmup_cipher per slot at startup so the
              timed request is cold-cache-free.

For ciphertext mode the client needs SK/PK only (to encrypt/decrypt); it does
NOT eval, so it never loads the 21 GiB galois key set. We use ON_DEMAND_FULL,
which preloads SK/PK/RLK and leaves galois as an empty shell — client HBM stays
~1 GiB, not ~22. The server never sees SK or plaintext.

Usage:
    # Step 1 (plaintext)
    python3 examples/11_client_server/client.py --url http://localhost:8000 --user-id 0
    # concurrent throughput test:
    python3 examples/11_client_server/client.py --url http://localhost:8000 --concurrent 3 --requests 6

    # Step 2 (ciphertext)
    python3 examples/11_client_server/client.py --url http://localhost:8000 \\
        --mode ciphertext --key-root /mnt/nvme0/mu_keys_resnet20 --user-id 0
"""

import argparse
import ctypes
import glob
import json
import os
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

import torch

from ace import fhe
from ace.fhe.config import KeyMode
from ace.model.dataset import load_cifar10_images


# Input/output scheme names from the resnet20 conf (encode_schemes[0]._name;
# output name "output") — same as test_ciphertext_e2e.py.
INPUT_NAME = "x"
OUTPUT_NAME = "output"
INPUT_SHAPE = (1, 3, 32, 32)  # resnet20 CIFAR-10 normalized input
OUTPUT_LEN = 10  # resnet20 logits


# ---------------------------------------------------------------------------
# Ciphertext-mode ctypes helpers (mirror test_ciphertext_e2e.py:46-135).
# Kms_encrypt_input / Kms_decrypt_output have no Python binding, so we bind
# the C ABI in libFHErt_phantom.so directly.
# ---------------------------------------------------------------------------


def _load_helper_abi():
    """Load libFHErt_phantom.so and bind Kms_encrypt_input / Kms_decrypt_output."""
    lib_path = os.path.join(os.path.dirname(fhe.__file__), "lib", "libFHErt_phantom.so")
    if not os.path.exists(lib_path):
        cands = glob.glob("/opt/ace-env/lib/python*/site-packages/ace/lib/libFHErt_phantom.so")
        lib_path = cands[0] if cands else lib_path
    lib = ctypes.CDLL(lib_path)

    # long Kms_encrypt_input(int session_idx, const char* input_name,
    #                        const double* plain, size_t n_elems,
    #                        char* out_buf, size_t out_cap)
    lib.Kms_encrypt_input.argtypes = [
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_size_t,
        ctypes.c_char_p,
        ctypes.c_size_t,
    ]
    lib.Kms_encrypt_input.restype = ctypes.c_long

    # long Kms_decrypt_output(int session_idx, const char* output_name,
    #                         const char* in_buf, size_t in_size,
    #                         double* out_doubles, size_t out_cap_doubles)
    lib.Kms_decrypt_output.argtypes = [
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_size_t,
    ]
    lib.Kms_decrypt_output.restype = ctypes.c_long
    return lib


def _encrypt_input(lib, session_idx, vec_double):
    """Two-pass: query size, then fill. Returns the serialized input ciphertext bytes."""
    n = len(vec_double)
    arr = (ctypes.c_double * n)(*vec_double)
    need = lib.Kms_encrypt_input(session_idx, INPUT_NAME.encode(), arr, n, None, 0)
    if need < 0:
        raise RuntimeError(f"Kms_encrypt_input pass1 failed (ret={need})")
    buf = ctypes.create_string_buffer(need)
    got = lib.Kms_encrypt_input(session_idx, INPUT_NAME.encode(), arr, n, buf, need)
    if got < 0:
        raise RuntimeError(f"Kms_encrypt_input pass2 failed (ret={got})")
    return buf.raw[:got]


def _decrypt_output(lib, session_idx, out_bytes):
    """Two-pass: query count, then fill. Returns the decoded output doubles."""
    n = len(out_bytes)
    buf = ctypes.create_string_buffer(out_bytes, n)
    need = lib.Kms_decrypt_output(session_idx, OUTPUT_NAME.encode(), buf, n, None, 0)
    if need < 0:
        raise RuntimeError(f"Kms_decrypt_output pass1 failed (ret={need})")
    out = (ctypes.c_double * need)()
    got = lib.Kms_decrypt_output(session_idx, OUTPUT_NAME.encode(), buf, n, out, need)
    if got < 0:
        raise RuntimeError(f"Kms_decrypt_output pass2 failed (ret={got})")
    return [float(out[i]) for i in range(got)]


def _build_client_runtime(key_root: str, user_id: int):
    """Ciphertext mode: the client only encrypts/decrypts — it NEVER evals, so
    it never needs the galois/bootstrap key set. We use ON_DEMAND_FULL, which
    preloads only SK/PK/RLK and leaves galois as an index-only shell.
    On top, FHERT_SKIP_BOOTSTRAP_HOIST=1 skips the Ensure_bootstrap_keys()
    prefetch that FheContext construction would otherwise trigger (it fetches
    all 67 galois keys = ~21 GiB into HBM — pointless for a client that never
    bootstraps). Server/eval callers must NOT set that env.

    NOTE (future minimal-client path): this still needs the compiled kernel .so
    (Get_context_params() is a code-gen global symbol dlopen'd from it) and a
    small GPU footprint (phantom encode/encrypt run NTT on GPU). A truly
    no-GPU client requires a C++ standalone encrypt-only API (context params
    from a sidecar json, encrypt without the eval session/kernel) — not yet
    implemented. Tracked as a follow-up.

    Returns (runner, session_idx, user_key_dir). Compiles resnet20 once
    (reuses the server's kernel .so; cached — first run compiles, subsequent
    runs hit /tmp/ace-compile-cache and skip compilation).
    """
    _HERE = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, os.path.join(_HERE, "..", "10_multi_user", "resnet20"))
    from generate_keys import _compile_once

    # Client never bootstraps/evals → skip the 21 GiB bootstrap-key prefetch
    # that FheContext construction would otherwise do (context.cu:198 hoist).
    # Must be set before runner.init() (which calls Prepare_context).
    os.environ.setdefault("FHERT_SKIP_BOOTSTRAP_HOIST", "1")

    root = os.path.abspath(key_root)
    with open(os.path.join(root, "meta.json")) as fp:
        meta = json.load(fp)
    user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    if user_id >= len(user_dirs):
        raise RuntimeError(f"--user-id {user_id} exceeds key bundles ({len(user_dirs)})")
    user_dir = os.path.abspath(user_dirs[user_id])

    # Client only encrypts/decrypts — it does NOT eval, so it never touches the
    # galois keys (the 21 GiB bootstrap/rotation key set). Use ON_DEMAND_FULL:
    # it preloads only SK/PK/RLK (Load_sk_pk_relin_only) and leaves galois as an
    # empty shell + index. Because the client never calls Ensure_galois_key /
    # rotation, the 21 GiB is NEVER loaded — client HBM stays ~1 GiB, not ~22.
    # (preload_full would eager-load all 67 galois keys = 21 GiB HBM, pointless
    #  on a client that only does encrypt/decrypt.)
    KeyMode.apply("on_demand_full")
    package = _compile_once(meta.get("backend", "phantom"))
    pkg = {**package, "key_path": user_dir, "key_mode": "on_demand_full"}
    runner = fhe.FHERuntime(pkg)
    runner.init()
    return runner, 0, user_dir


# ---------------------------------------------------------------------------
# Inference drivers
# ---------------------------------------------------------------------------


def _infer_plain(url: str, user_id: int, x_bytes: bytes):
    import requests

    t0 = time.perf_counter()
    resp = requests.post(f"{url}/predict", params={"user_id": user_id}, data=x_bytes, timeout=60)
    dt = time.perf_counter() - t0
    if resp.status_code != 200:
        return user_id, None, dt, f"HTTP {resp.status_code}: {resp.text[:200]}"
    logits = torch.frombuffer(bytearray(resp.content), dtype=torch.float32).reshape(1, OUTPUT_LEN)
    return user_id, logits, dt, None


def _infer_cipher(url: str, user_id: int, ct_bytes: bytes, decrypt_lib, decrypt_session):
    """encrypt is done by the caller (keeps the ciphertext reusable across
    requests); here we POST it, then decrypt the returned output ciphertext."""
    import requests

    t0 = time.perf_counter()
    resp = requests.post(f"{url}/predict_cipher", params={"user_id": user_id}, data=ct_bytes, timeout=120)
    dt = time.perf_counter() - t0
    if resp.status_code != 200:
        return user_id, None, dt, f"HTTP {resp.status_code}: {resp.text[:200]}"
    logits = _decrypt_output(decrypt_lib, decrypt_session, resp.content)
    return user_id, torch.tensor(logits[:OUTPUT_LEN]), dt, None


def _warmup_cipher(url: str, user_id: int, ct_bytes: bytes):
    """POST /warmup_cipher so the server run_cipher-once on this slot absorbs
    lazy setup; the timed request that follows is cold-cache-free."""
    import requests

    t0 = time.perf_counter()
    resp = requests.post(f"{url}/warmup_cipher", params={"user_id": user_id}, data=ct_bytes, timeout=120)
    dt = time.perf_counter() - t0
    if resp.status_code != 200:
        print(f"  [warmup] user_id={user_id} HTTP {resp.status_code}: {resp.text[:200]}", flush=True)
        return dt
    print(f"  [warmup] user_id={user_id} dt={dt:.2f}s", flush=True)
    return dt


def main():
    ap = argparse.ArgumentParser(description="ResNet-20 FHE Client (HTTP)")
    ap.add_argument("--url", default="http://localhost:8000", help="server URL")
    ap.add_argument(
        "--mode",
        choices=["plaintext", "ciphertext"],
        default="plaintext",
        help="plaintext (Step 1) | ciphertext (Step 2, client encrypt/decrypt). default plaintext",
    )
    ap.add_argument("--user-id", type=int, default=0, help="user id (routes to session user_id%K)")
    ap.add_argument("--image", type=int, default=0, help="CIFAR-10 test image index (default 0)")
    ap.add_argument("--concurrent", type=int, default=1, help="concurrent workers (default 1)")
    ap.add_argument("--requests", type=int, default=1, help="total requests to send (default 1)")
    ap.add_argument(
        "--key-root",
        default=None,
        help="required for --mode ciphertext: key-root with meta.json (client loads full keys)",
    )
    ap.add_argument(
        "--no-warmup",
        action="store_true",
        help="ciphertext mode: skip the /warmup_cipher handshake (first request will be cold)",
    )
    args = ap.parse_args()

    n_imgs = max(args.image + 1, args.concurrent)
    cifar_imgs, _ = load_cifar10_images(n_imgs)

    # --- Plaintext mode (Step 1): unchanged original path ---
    if args.mode == "plaintext":
        x_bytes = cifar_imgs[args.image : args.image + 1].float().contiguous().numpy().tobytes()
        print(f"  input: image {args.image}, {len(x_bytes)} bytes (float32 1x3x32x32)")
        _run_requests(args, lambda uid, idx: _infer_plain(args.url, uid, x_bytes))
        return

    # --- Ciphertext mode (Step 2) ---
    if args.key_root is None:
        print("  ERROR: --mode ciphertext requires --key-root", file=sys.stderr)
        sys.exit(1)

    print("  [client] building on_demand_full runtime (SK/PK to encrypt/decrypt; galois NOT loaded) ...", flush=True)
    t0 = time.perf_counter()
    _runner, sess_idx, user_dir = _build_client_runtime(args.key_root, args.user_id)
    print(
        f"  [client] runtime ready in {time.perf_counter() - t0:.1f}s (key={user_dir}, mode={KeyMode.get()})",
        flush=True,
    )

    ciph_lib = _load_helper_abi()

    # Encrypt ONE CIFAR image once; reuse the ciphertext across requests.
    plain = cifar_imgs[args.image].float().reshape(-1).double().tolist()  # 1*3*32*32 doubles
    t0 = time.perf_counter()
    ct_bytes = _encrypt_input(ciph_lib, sess_idx, plain)
    print(f"  [client] encrypted input: {len(ct_bytes)} bytes ({time.perf_counter() - t0:.2f}s)", flush=True)

    # Warmup handshake: drive /warmup_cipher for the target slot(s) so the
    # server absorbs lazy setup before we time the real request.
    if not args.no_warmup:
        if args.user_id != 0 or args.concurrent == 1:
            warm_uids = [args.user_id]
        else:
            # --user-id 0 under concurrency means "cycle across K slots"
            warm_uids = list(range(max(args.concurrent, 1)))
        seen = set()
        for uid in warm_uids:
            if uid in seen:
                continue
            seen.add(uid)
            _warmup_cipher(args.url, uid, ct_bytes)

    def _one_cipher(uid, idx):
        return _infer_cipher(args.url, uid, ct_bytes, ciph_lib, sess_idx)

    _run_requests(args, _one_cipher)


def _run_requests(args, infer_fn):
    """Send requests (single or concurrent) via infer_fn(uid, idx)."""
    if args.concurrent == 1 and args.requests == 1:
        uid, logits, dt, err = infer_fn(args.user_id, 0)
        if err:
            print(f"  ERROR: {err}")
            sys.exit(1)
        print(f"  user_id={uid} wall={dt:.2f}s")
        v = logits.reshape(-1).tolist() if torch.is_tensor(logits) else list(logits)
        print(f"  logits: {[round(x, 4) for x in v[:OUTPUT_LEN]]}")
        return

    print(f"  concurrent={args.concurrent} requests={args.requests}")
    t0 = time.perf_counter()
    results = []
    with ThreadPoolExecutor(max_workers=args.concurrent) as pool:
        futs = []
        for i in range(args.requests):
            uid = (i % max(args.concurrent, 1)) if args.user_id == 0 else args.user_id
            futs.append(pool.submit(infer_fn, uid, i))
        for f in as_completed(futs):
            uid, logits, dt, err = f.result()
            results.append((uid, dt, err is None))
            tag = "ok" if err is None else f"ERR {err}"
            print(f"  user_id={uid} wall={dt:.2f}s {tag}")
    wall = time.perf_counter() - t0
    ok = sum(1 for _, _, ok in results if ok)
    print(f"  total wall={wall:.2f}s  ok={ok}/{args.requests}  throughput={args.requests / wall:.3f} img/s")


if __name__ == "__main__":
    main()
