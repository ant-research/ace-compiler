#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Offline Weight Encoding: Pre-encode weight data to avoid runtime encoding overhead.

This script demonstrates the offline weight encoding workflow:
  1. Compile a simple model (add function) which produces a DE_MSG_F32 weight file
  2. Run inference with on-the-fly encoding (baseline)
  3. Use OfflinePrep.encode_package() to pre-encode weights
  4. Run inference with pre-encoded weights
  5. Verify that outputs match

Benefits of offline encoding:
  - Eliminates per-inference encoding overhead for model weights
  - Reduces GPU compute time during inference startup
  - Enables faster repeated inference with the same model

Usage:
    python3 08_offline_encode.py

Requires: CUDA GPU, phantom backend
"""

import os
import sys
import time

import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.util import gpu_available
from ace.kms import OfflinePrep


def main():
    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 60)
    print("Offline Weight Encoding Demo")
    print("=" * 60)
    print()

    # -- Step 1: Compile the model ---------------------------------
    print("[1/4] Compiling model ...")

    @fhe.compile(frontend="torch", library="phantom", device="cuda", encrypt_inputs=["x"])
    def weighted_add(x):
        w = torch.tensor([1.0, 2.0, 3.0, 4.0])
        return x * w

    x = torch.randn(1, 4, dtype=torch.float32)
    prog = weighted_add.compile([x])
    package = prog.package

    print(f"      Kernel: {package['kernel']}")
    print(f"      Config: {package.get('config_path', 'N/A')}")
    print("      Done.")

    # -- Step 2: Run inference with on-the-fly encoding (baseline) --
    print("[2/4] Running inference with on-the-fly encoding (baseline) ...")
    _C.set_key_path("")  # ensure no pre-set key_path

    runner_baseline = fhe.FHERuntime(package)
    t0 = time.time()
    result_baseline = runner_baseline.run(x)
    t_baseline = time.time() - t0

    expected = x * torch.tensor([1.0, 2.0, 3.0, 4.0])
    print(f"      Result:   {result_baseline.flatten()[:4].tolist()}")
    print(f"      Expected: {expected.flatten()[:4].tolist()}")
    print(f"      Time:     {t_baseline:.3f}s")
    runner_baseline.finalize()

    # -- Step 3: Encode weight file offline -------------------------
    print("[3/4] Encoding weight file offline ...")

    try:
        prep = OfflinePrep(kernel_path=package["kernel"])
        t0 = time.time()
        encoded_package = prep.encode_package(package)
        t_encode = time.time() - t0
        print(f"      Encoded config: {encoded_package['config_path']}")
        print(f"      Encode time:    {t_encode:.3f}s")
        print("      Done.")
    except FileNotFoundError as e:
        print(f"      No weight file found: {e}")
        print("      Offline encoding is not needed for this model.")
        print()
        print("=" * 60)
        print("Demo skipped (no weight data to encode)")
        print("=" * 60)
        return

    # -- Step 4: Run inference with pre-encoded weights -------------
    print("[4/4] Running inference with pre-encoded weights ...")
    _C.set_key_path("")  # ensure no pre-set key_path

    runner_encoded = fhe.FHERuntime(encoded_package)
    t0 = time.time()
    result_encoded = runner_encoded.run(x)
    t_encoded = time.time() - t0

    print(f"      Result:   {result_encoded.flatten()[:4].tolist()}")
    print(f"      Expected: {expected.flatten()[:4].tolist()}")
    print(f"      Time:     {t_encoded:.3f}s")
    runner_encoded.finalize()

    # -- Verify results match --------------------------------------
    print()
    print("=" * 60)
    max_diff = (result_baseline - result_encoded).abs().max().item()
    if max_diff < 1e-3:
        print(f"VERIFICATION PASSED: Results match (max diff = {max_diff:.6e})")
    else:
        print(f"VERIFICATION FAILED: Results differ (max diff = {max_diff:.6e})")
        sys.exit(1)

    print()
    print("Summary:")
    print(f"  Baseline inference: {t_baseline:.3f}s")
    print(f"  Encode time:        {t_encode:.3f}s (one-time cost)")
    print(f"  Encoded inference:  {t_encoded:.3f}s")
    print("=" * 60)


if __name__ == "__main__":
    main()
