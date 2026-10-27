#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Ciphertext-in/out end-to-end verification (P2''.4). Same-origin simulation:
on a single FHERuntime (PRELOAD_FULL/ON_DEMAND_FULL session, which holds SK/PK),
encrypt a CIFAR input → run_cipher (ciphertext in, ciphertext out, server
eval-only path) → decrypt the output, and check the logits match the plaintext
run_session baseline.

This exercises the new server-side Kms_run_cipher C ABI (+ Kms_encrypt_input /
Kms_decrypt_output same-origin helpers) and proves the P1 CiphertextSerializer
round-trips correctly through a real eval.

Usage:
    CUDA_VISIBLE_DEVICES=<idle> python3 examples/11_client_server/test_ciphertext_e2e.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20
"""

import argparse
import ctypes
import json
import os
import sys
import time

import torch

from ace import fhe
from ace.fhe.config import IoPolicy, IoPolicySettings, KeyMode
from ace.fhe.util import gpu_available
from ace.model.dataset import load_cifar10_images


_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(_HERE, "..", "10_multi_user", "resnet20"))
from generate_keys import _compile_once  # noqa: E402


# Input name from resnet20 conf encode_schemes[0]._name; output name "output".
INPUT_NAME = "x"
OUTPUT_NAME = "output"


def _load_helper_abi():
    """Load libFHErt_phantom.so and bind the same-origin helper C ABIs."""
    lib_path = os.path.join(os.path.dirname(fhe.__file__), "lib", "libFHErt_phantom.so")
    if not os.path.exists(lib_path):
        # fall back to the env site-packages path
        import glob

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

    # Diagnostic: encrypt→decrypt (no serialization).
    lib.Kms_encrypt_decrypt_plain.argtypes = [
        ctypes.c_int,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_size_t,
    ]
    lib.Kms_encrypt_decrypt_plain.restype = ctypes.c_long

    # Diagnostic: encrypt→serialize→deserialize, return deserialized ct fields.
    lib.Kms_serialize_fields_diag.argtypes = [
        ctypes.c_int,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_size_t,
    ]
    lib.Kms_serialize_fields_diag.restype = ctypes.c_long

    # Diagnostic: serialize the Io input slot (populated by run_session).
    lib.Kms_serialize_io_input.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t]
    lib.Kms_serialize_io_input.restype = ctypes.c_long
    return lib


def _encrypt_input(lib, session_idx, vec_double):
    """Two-pass: query size, then fill. Returns bytes."""
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
    """Two-pass: query count, then fill. Returns list[float]."""
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


def main():
    ap = argparse.ArgumentParser(description="Ciphertext-in/out e2e (P2''.4)")
    ap.add_argument("--key-root", required=True, help="key root with meta.json")
    ap.add_argument("--io", default="staged", choices=["staged", "gds", "mapped", "auto"])
    ap.add_argument("--key-mode", default="on_demand_full", choices=["preload_full", "on_demand_full"])
    args = ap.parse_args()

    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(
        policy={"staged": IoPolicy.STAGED, "gds": IoPolicy.GDS, "mapped": IoPolicy.MAPPED, "auto": IoPolicy.AUTO}[
            args.io
        ]
    ).apply()
    KeyMode.apply(args.key_mode)
    if not gpu_available():
        print("ERROR: GPU not available")
        sys.exit(1)

    root = os.path.abspath(args.key_root)
    with open(os.path.join(root, "meta.json")) as f:
        meta = json.load(f)
    user_dir = meta["user_key_dirs"][0]["key_dir"]

    print("  compiling resnet20 ...", flush=True)
    package = _compile_once(meta.get("backend", "phantom"))
    pkg = {**package, "key_path": user_dir, "key_mode": args.key_mode}
    runner = fhe.FHERuntime(pkg)
    runner.init()
    print(f"  session 0 init done; backend={meta.get('backend')}  key_mode={KeyMode.get()}", flush=True)

    # Warm run so bootstrap-key prefetch (sync hoist) + lazy setup are done
    # before timing — both the baseline and the cipher path see steady state.
    cifar, _ = load_cifar10_images(1)
    x = (cifar[0:1].float(),)
    print("  warm run ...", flush=True)
    runner.run_session(0, *x)

    # Baseline: plaintext path.
    t0 = time.perf_counter()
    baseline = runner.run_session(0, *x)
    t_base = time.perf_counter() - t0
    base_logits = baseline.float().reshape(-1).tolist()
    print(f"  [baseline] run_session {t_base:.2f}s logits={[round(v, 3) for v in base_logits]}", flush=True)

    # Re-run run_session with the SAME input: CKKS encrypt uses randomness, so
    # two runs differ only by CKKS noise (typically <0.1). If they match
    # closely, then run_cipher's ~9 divergence is a real bug, not noise.
    base2 = runner.run_session(0, *x)
    base2_logits = base2.float().reshape(-1).tolist()
    b_diff = max(abs(a - b) for a, b in zip(base_logits, base2_logits))
    print(
        f"  [baseline2] run_session again {t_base:.2f}s logits={[round(v, 3) for v in base2_logits]}  diff_vs_base1={b_diff:.4g}",
        flush=True,
    )

    # Cipher path: encrypt → run_cipher → decrypt.
    lib = _load_helper_abi()
    plain = x[0].reshape(-1).double().tolist()  # 1*3*32*32 = 3072 doubles

    # DIAG0: helper-internal encrypt→decrypt WITHOUT serialization. Isolates
    # whether the encrypt/decrypt helper itself is correct (if wrong here, the
    # bug is in the helper's chain_index/scale/evaluator choice, not serialize).
    n = len(plain)
    arr = (ctypes.c_double * n)(*plain)
    need0 = lib.Kms_encrypt_decrypt_plain(0, arr, n, None, 0)
    out0 = (ctypes.c_double * need0)()
    got0 = lib.Kms_encrypt_decrypt_plain(0, arr, n, out0, need0)
    plain_cmp = torch.tensor(plain[: len(plain)])
    diag0_t = torch.tensor([float(out0[i]) for i in range(min(n, int(got0)))])
    nc = min(len(plain_cmp), len(diag0_t))
    diag0_ok = torch.allclose(plain_cmp[:nc], diag0_t[:nc], atol=1e-3, rtol=1e-3)
    diag0_diff = (plain_cmp[:nc] - diag0_t[:nc]).abs().max().item() if nc > 0 else float("inf")
    print(
        f"  [diag0] helper encrypt->decrypt (no serialize): "
        f"{'OK ✓' if diag0_ok else f'DIFF max={diag0_diff:.4g}'}  "
        f"plain[:5]={[round(v, 3) for v in plain[:5]]} got[:5]={[round(float(out0[i]), 3) for i in range(5)]}",
        flush=True,
    )
    if not diag0_ok:
        print("  >> helper encrypt/decrypt itself is wrong (chain_index/scale/evaluator).", flush=True)

    # DIAG-FIELDS: encrypt→serialize→deserialize, dump the deserialized ct's
    # fields. The backend also logs (spdlog warn) ORIG vs RNDTR side by side —
    # see server stderr for the [diag] ORIG/RNDTR lines.
    farr = (ctypes.c_double * 8)()
    g = lib.Kms_serialize_fields_diag(0, arr, n, farr, 8)
    if g == 8:
        names = ["size", "N", "L", "chain_index", "scale", "ntt", "nsd", "asym"]
        rndtr = {names[i]: farr[i] for i in range(8)}
        print(f"  [diag-fields] RNDTR ct fields: {rndtr}", flush=True)
        print(
            "  [diag-fields] see [diag] ORIG / [diag] RNDTR in the backend log for the encrypt-time (ORIG) comparison.",
            flush=True,
        )

    # DIAG-RTLIB: capture the rtlib/Prepare_input-encrypted input and feed it
    # through run_cipher, to separate "encrypt path differs" from "eval path
    # differs" (the original intent).
    #
    # WARNING — why this NO LONGER reads the Io slot after run_session:
    # The kernel reads its input via FheContext::Get_input_data (context.h:378-
    # 385), which does `std::move(*data); delete data;` on the Io slot. So after
    # ANY run_session, the "x" input slot in the Io store is a DANGLING POINTER
    # (Io_set_data never nulls it; only Io_fini does, at teardown). An earlier
    # version here did run_session() then Kms_serialize_io_input("x",...), which
    # dereferenced that dangling pointer and broke with "Invalid ciphertext
    # header" (freed GPU memory -> garbage magic bytes).
    #
    # Because Prepare_input (context.cu:536-554) and Kms_encrypt_input
    # (kms_api.h:294-318) are byte-for-byte identical (same chain_index/scale/
    # encoder/encryptor -- confirmed statically; diag0/diag prove the encrypt
    # path is lossless), the "rtlib-encrypted input" this diag wanted is
    # EXACTLY what _encrypt_input already produces. So we feed that through
    # run_cipher here as a second, fully-same-origin eval pass. This isolates
    # eval-vs-encrypt WITHOUT touching the dangling Io slot.
    try:
        rtlib_in = _encrypt_input(lib, 0, plain)  # == Prepare_input ct, serialized
        out_io = runner.run_cipher(0, [rtlib_in], OUTPUT_NAME)
        io_logits = _decrypt_output(lib, 0, out_io)
        io_t = torch.tensor(io_logits[: len(base_logits)])
        io_diff = (
            (io_t - torch.tensor(base_logits)).abs().max().item()
            if len(io_logits) >= len(base_logits)
            else float("inf")
        )
        io_ok = len(io_logits) >= len(base_logits) and torch.allclose(
            io_t, torch.tensor(base_logits), atol=1.0, rtol=0.1
        )
        print(
            f"  [diag-rtlib] rtlib-equiv-encrypt -> run_cipher -> decrypt vs baseline: "
            f"{'OK ✓' if io_ok else f'DIFF max={io_diff:.4g}'}",
            flush=True,
        )
        print(f"              logits={[round(v, 3) for v in io_logits[: len(base_logits)]]}", flush=True)
    except Exception as e:
        import traceback

        print(f"  [diag-rtlib] EXCEPTION: {e}", flush=True)
        traceback.print_exc()

    t0 = time.perf_counter()
    in_bytes = _encrypt_input(lib, 0, plain)
    t_enc = time.perf_counter() - t0

    # DIAG: serialize round-trip WITHOUT eval. Decrypt the freshly-encrypted
    # input bytes directly. If the plaintexts match, the serialize/deserialize
    # layer preserves the ciphertext (noise_scale_deg, scale, chain_index,
    # NTT form, symmetric flag); if they diverge, the bug is in serialization
    # (P1) and run_cipher's eval error is downstream of it.
    diag = _decrypt_output(lib, 0, in_bytes)
    # decrypt returns the full N-slot vector (N=32768); encrypt encoded the
    # 3072-d plain into the first 3072 slots (rest zero). Compare only the
    # encoded span.
    n_plain = len(plain)
    n_cmp = min(n_plain, len(diag))
    diag_t = torch.tensor(diag[:n_cmp])
    plain_t = torch.tensor(plain[:n_cmp])
    diag_diff = (diag_t - plain_t).abs().max().item()
    diag_ok = torch.allclose(diag_t, plain_t, atol=1e-3, rtol=1e-3)
    print(
        f"  [diag] encrypt->serialize->deserialize->decrypt vs original plain "
        f"(first {n_cmp} of {len(diag)} decoded slots): "
        f"{'OK ✓' if diag_ok else f'DIFF max={diag_diff:.4g}'}",
        flush=True,
    )
    print(
        f"        plain[:5]={[round(v, 4) for v in plain[:5]]}  diag[:5]={[round(v, 4) for v in diag[:5]]}", flush=True
    )
    if not diag_ok:
        print("  >> serialization layer loses fidelity; run_cipher eval error is downstream.", flush=True)

    t0 = time.perf_counter()

    t0 = time.perf_counter()
    out_bytes = runner.run_cipher(0, [in_bytes], OUTPUT_NAME)
    t_run = time.perf_counter() - t0

    t0 = time.perf_counter()
    cipher_logits = _decrypt_output(lib, 0, out_bytes)
    t_dec = time.perf_counter() - t0

    print(f"  [cipher]    encrypt={t_enc:.2f}s run_cipher={t_run:.2f}s decrypt={t_dec:.2f}s", flush=True)
    print(f"  [cipher]    logits={[round(v, 3) for v in cipher_logits[: len(base_logits)]]}", flush=True)
    print(f"  bytes: in={len(in_bytes)} out={len(out_bytes)}", flush=True)

    # Compare: cipher path logits should match the plaintext baseline (same
    # model, same input, same keys → same output modulo serialize round-trip).
    n = min(len(base_logits), len(cipher_logits))
    if n == 0:
        print("  !! no logits to compare")
        sys.exit(1)
    base_t = torch.tensor(base_logits[:n])
    cipher_t = torch.tensor(cipher_logits[:n])
    max_diff = (base_t - cipher_t).abs().max().item()
    ok = torch.allclose(base_t, cipher_t, atol=1.0, rtol=0.1)
    print(f"\n  RESULT: {'PASS ✓' if ok else 'FAIL ✗'}  max_abs_diff={max_diff:.4g}  (atol=1.0 rtol=0.1)")
    if not ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
