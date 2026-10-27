#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 FHE web client — a small local web UI that drives the ciphertext
client/server pipeline (pairs with server.py --mode ciphertext).

UI (browser): pick a server URL, pick one of a few preset CIFAR-10 test
images, click "Infer" → shows the returned 10 logits + predicted class.

How it runs (the browser can't encrypt — phantom encrypt is GPU/CKKS — so the
local web server acts as the encrypt/decrypt proxy):
  1. browser → POST /api/infer {url, image_idx}
  2. local backend: load image → Kms_encrypt_input (PK) → POST server
     /warmup_cipher then /predict_cipher → Kms_decrypt_output (SK) → logits
  3. browser ← {logits[10], predicted_class, timings}

The encrypt/decrypt machinery is reused from client.py (on_demand_full client
runtime; galois NOT loaded since the client never evals). First request warms
the server slot via /warmup_cipher.

Usage:
    # 1. start the ciphertext server (eval-only, no SK)
    CUDA_VISIBLE_DEVICES=0 python3 examples/11_client_server/server.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20 --port 8000 --num-users 1 \\
        --io staged --mode ciphertext
    # 2. start this web client (separate terminal; it needs a GPU for encrypt)
    CUDA_VISIBLE_DEVICES=0 python3 examples/11_client_server/web_client.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20 --port 8080 \\
        --server-url http://localhost:8000
    # 3. open http://localhost:8080 in a browser
"""

import argparse
import ctypes
import io
import json
import os
import sys
import time

import torch

from ace import fhe
from ace.fhe.config import KeyMode
from ace.model.dataset import CIFAR10_CLASSES, CIFAR10_MEAN, CIFAR10_STD, load_cifar10_images


# Reuse the client encrypt/decrypt machinery from client.py (same directory).
_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _HERE)
sys.path.insert(0, os.path.join(_HERE, "..", "10_multi_user", "resnet20"))
from client import (  # noqa: E402  # noqa: E402
    INPUT_NAME,
    OUTPUT_LEN,
    OUTPUT_NAME,
    _build_client_runtime,
    _decrypt_output,
    _encrypt_input,
    _load_helper_abi,
)


NUM_PRESET_IMAGES = 100  # how many CIFAR-10 test images to offer in the UI

# --- state built once at startup ---
_client_lib = None  # ctypes handle to libFHErt_phantom.so (KMS ABI bound)
_client_sess = 0  # session idx on the client runtime
_warmed_slots: set = set()  # server slots we've POSTed /warmup_cipher to


def _build_client(key_root: str, user_id: int):
    """Build the on_demand_full client runtime once (SK/PK to encrypt/decrypt;
    galois NOT loaded). Mirrors client.py ciphertext mode startup."""
    global _client_lib, _client_sess
    _runner, _client_sess, _user_dir = _build_client_runtime(key_root, user_id)
    _client_lib = _load_helper_abi()
    print(f"  [web-client] client runtime ready (sess={_client_sess})", flush=True)


def _denormalized_to_png_b64(img_chw: torch.Tensor) -> str:
    """Convert a normalized CIFAR image (3,32,32 float32) to a base64 PNG for the
    browser gallery. Reverse the CIFAR10 mean/std normalization."""
    import base64

    import numpy as np

    arr = img_chw.float().numpy() * CIFAR10_STD.reshape(3, 1, 1) + CIFAR10_MEAN.reshape(3, 1, 1)
    arr = (arr.clip(0, 1) * 255).astype("uint8").transpose(1, 2, 0)  # HWC
    buf = io.BytesIO()
    try:
        import PIL.Image

        PIL.Image.fromarray(arr).save(buf, format="PNG")
    except Exception:
        # Fallback: write a minimal PPM if PIL unavailable, then base64 it.
        ppm = b"P6\n32 32\n255\n" + arr.tobytes()
        buf.write(ppm)
    return base64.b64encode(buf.getvalue()).decode()


def make_app(args):
    from fastapi import FastAPI
    from fastapi.responses import HTMLResponse, JSONResponse

    # Preload the preset CIFAR images + labels + thumbnails once.
    imgs, labels = load_cifar10_images(NUM_PRESET_IMAGES)
    thumbs = [_denormalized_to_png_b64(imgs[i]) for i in range(NUM_PRESET_IMAGES)]
    presets = [
        {"idx": i, "label": CIFAR10_CLASSES[labels[i]], "thumb_b64": thumbs[i]} for i in range(NUM_PRESET_IMAGES)
    ]

    app = FastAPI(title="ace resnet20 fhe web client")

    # The UI is a separate index.html (static/index.html); only SERVER_URL is
    # injected at serve time. Image presets + class names are fetched async
    # via /api/presets, so the HTML stays clean (no big inline JSON).
    index_html_path = os.path.join(_HERE, "static", "index.html")
    with open(index_html_path, encoding="utf-8") as fp:
        index_html = fp.read().replace("{{SERVER_URL}}", args.server_url)

    @app.get("/")
    def index():
        return HTMLResponse(index_html)

    @app.get("/api/presets")
    def presets_api():
        return JSONResponse({"presets": presets, "classes": CIFAR10_CLASSES})

    @app.post("/api/infer")
    def infer(req: dict | None = None):
        import requests

        try:
            body = req if isinstance(req, dict) else {}
            url = (body.get("url") or args.server_url).rstrip("/")
            image_idx = int(body.get("image_idx", 0))

            if _client_lib is None:
                _build_client(args.key_root, args.user_id)

            imgs, labels = load_cifar10_images(max(image_idx + 1, 1))
            plain = imgs[image_idx].float().reshape(-1).double().tolist()

            t_total0 = time.perf_counter()

            # Stage: client encrypt (PK). ~0.08s.
            t0 = time.perf_counter()
            ct_bytes = _encrypt_input(_client_lib, _client_sess, plain)
            t_enc = time.perf_counter() - t0

            slot = args.user_id
            # Stage: one-time server warmup (lazy eval setup on the slot). Only
            # the FIRST request for this slot pays this; excluded from the
            # steady-state per-inference total below.
            warmed = slot in _warmed_slots
            t_w = time.perf_counter()
            if not warmed:
                rw = requests.post(f"{url}/warmup_cipher", params={"user_id": slot}, data=ct_bytes, timeout=120)
                if rw.status_code != 200:
                    return JSONResponse({"detail": f"warmup HTTP {rw.status_code}: {rw.text[:200]}"}, status_code=502)
                _warmed_slots.add(slot)
            t_warm = time.perf_counter() - t_w

            # Stage: send ciphertext → server eval (run_cipher) ← ciphertext back.
            # This wall includes network + server route + RunCipher(deser+kernel+serialize).
            t1 = time.perf_counter()
            resp = requests.post(f"{url}/predict_cipher", params={"user_id": slot}, data=ct_bytes, timeout=120)
            t_post = time.perf_counter() - t1
            if resp.status_code != 200:
                return JSONResponse({"detail": f"server HTTP {resp.status_code}: {resp.text[:200]}"}, status_code=502)

            # Server echoes its per-stage split via X-RC-* headers (server & client
            # are separate environments). Use them to show the real eval breakdown.
            def _hdr(name):
                return resp.headers.get(name)

            server_split = None
            if _hdr("X-RC-Kernel") is not None:
                try:
                    server_split = {
                        "deser_sync": float(_hdr("X-RC-Deser")),
                        "launder_sync": float(_hdr("X-RC-Launder")),
                        "kernel": float(_hdr("X-RC-Kernel")),
                        "serialize": float(_hdr("X-RC-Serialize")),
                        "server_wall": float(_hdr("X-RC-Wall")),
                        "runcipher_wall": float(_hdr("X-RC-RunCipherWall")) if _hdr("X-RC-RunCipherWall") else None,
                    }
                except (TypeError, ValueError):
                    server_split = None

            # Stage: client decrypt (SK). ~0.05s.
            t2 = time.perf_counter()
            logits = _decrypt_output(_client_lib, _client_sess, resp.content)
            t_dec = time.perf_counter() - t2

            t_total = time.perf_counter() - t_total0

            logits10 = logits[:OUTPUT_LEN]
            predicted = int(max(range(len(logits10)), key=lambda i: logits10[i]))
            # Steady-state per-inference (excludes one-time warmup): encrypt + round-trip(server eval) + decrypt.
            steady = t_enc + t_post + t_dec
            stages = [
                {
                    "name": "client: encrypt (PK)",
                    "s": round(t_enc, 3),
                    "note": "CKKS encrypt (encode + encrypt with PK)",
                },
                {
                    "name": f"server: warmup (one-time{' — skipped' if warmed else ''})",
                    "s": round(t_warm, 3),
                    "dim": True,
                    "note": "first-call lazy setup; not part of steady-state inference",
                },
            ]
            # server eval breakdown (from X-RC-* headers). Show ONE main row
            # "server: eval" = the RunCipher subtotal (≈ pure inference), then a
            # single dim detail line that splits it into kernel (the GPU eval)
            # + deser/launder/serialize + overhead. Avoids 7 noisy rows.
            if server_split is not None:
                rc_sum = (
                    server_split["deser_sync"]
                    + server_split["launder_sync"]
                    + server_split["kernel"]
                    + server_split["serialize"]
                )
                s_wall = server_split.get("server_wall", 0.0)
                rc_wall = server_split.get("runcipher_wall")
                non_kernel = server_split["deser_sync"] + server_split["launder_sync"] + server_split["serialize"]
                overhead = (
                    max(rc_wall - rc_sum, 0.0) + max(s_wall - (rc_wall or rc_sum), 0.0)
                    if rc_wall is not None
                    else max(s_wall - rc_sum, 0.0)
                )
                network_overhead = max(t_post - s_wall, 0.0)
                stages += [
                    {
                        "name": "server: eval (run_cipher)",
                        "s": round(rc_sum, 3),
                        "note": "server-side inference (ciphertext eval) — breakdown below",
                    },
                    {
                        "name": "network: client ↔ server",
                        "s": round(network_overhead, 3),
                        "note": "send 17.8 MiB ciphertext + recv 1 MiB over HTTP",
                    },
                ]
            else:
                stages += [
                    {
                        "name": "send + server eval + recv (round-trip)",
                        "s": round(t_post, 3),
                        "note": "client wall (server didn't return a per-stage split)",
                    },
                ]
            stages += [
                {
                    "name": "client: decrypt (SK)",
                    "s": round(t_dec, 3),
                    "note": "CKKS decrypt (decrypt with SK + decode)",
                },
            ]
            print(
                f"  [web-infer] idx={image_idx} pred={predicted}({CIFAR10_CLASSES[predicted]}) "
                f"steady={steady:.2f}s total={t_total:.2f}s enc={t_enc:.2f} eval_rt={t_post:.2f} "
                f"dec={t_dec:.2f} warm={t_warm:.2f}{'(skipped)' if warmed else ''} "
                f"ct_in={len(ct_bytes)}B ct_out={len(resp.content)}B",
                flush=True,
            )
            return JSONResponse(
                {
                    "logits": logits10,
                    "predicted": predicted,
                    "true_label": CIFAR10_CLASSES[labels[image_idx]],
                    "stages": stages,
                    "server_split": server_split,  # deser/launder/kernel/serialize from X-RC-* headers (None if server is old)
                    "steady_total_s": round(steady, 3),  # excludes one-time warmup
                    "wall_total_s": round(t_total, 3),  # includes warmup this call
                    "warmed_first_call": not warmed,
                    "ct_in_bytes": len(ct_bytes),
                    "ct_out_bytes": len(resp.content),
                }
            )
        except Exception as e:
            import traceback

            traceback.print_exc()
            return JSONResponse({"detail": str(e)}, status_code=500)

    return app


def main():
    ap = argparse.ArgumentParser(description="resnet20 FHE web client (ciphertext proxy)")
    ap.add_argument("--key-root", required=True, help="key-root with meta.json (client loads SK/PK to encrypt/decrypt)")
    ap.add_argument("--port", type=int, default=8080, help="port for this web UI (default 8080)")
    ap.add_argument("--server-url", default="http://localhost:8000", help="ace ciphertext server URL")
    ap.add_argument("--user-id", type=int, default=0, help="user id (routes to a server session)")
    ap.add_argument("--host", default="0.0.0.0")
    args = ap.parse_args()

    # Build the client encrypt/decrypt runtime up front (so the first click is fast).
    _build_client(args.key_root, args.user_id)

    app = make_app(args)
    print(f"  web client: http://{args.host}:{args.port}  →  server {args.server_url}", flush=True)
    import uvicorn

    uvicorn.run(app, host=args.host, port=args.port, log_level="warning")


if __name__ == "__main__":
    main()
