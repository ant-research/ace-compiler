#!/usr/bin/env python3
"""Quick validation: 3 IoPolicy x T1/T4 x D1 x single/multi x 1GiB = 12 combos"""

import json
import os
import sys
import time


sys.path.insert(0, "/work/ace-fhe/ace-compiler")
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings


GiB = 1024**3

DP_TO_POLICY = {"host_staged": "staged", "host_mapped": "mapped", "gds": "gds"}

configs = [(dp, t, fm) for dp in ["host_staged", "host_mapped", "gds"] for t in [1, 4] for fm in ["single", "multi"]]

results = []
os.makedirs("/mnt/nvme0/io_bench", exist_ok=True)

for idx, (dp, t, fm) in enumerate(configs, 1):
    label = f"{dp}/T{t}/D1/{fm}/1GiB"
    print(f"[{idx}/{len(configs)}] {label} ... ", end="", flush=True)

    IoPolicySettings(policy=DP_TO_POLICY[dp], disk_paths=["/mnt/nvme0"]).apply()
    IoConfig(threads=t, disk_paths=["/mnt/nvme0"]).apply()

    bench_path = f"/mnt/nvme0/io_bench/val_{dp}_T{t}_{fm}"
    num_files = 1 if fm == "single" else max(4, t)

    try:
        r = _C.bench_io_roundtrip(bench_path, 1 * GiB, num_files, ["/mnt/nvme0"])
        print(
            f"write={r['write_gib_s']:.2f} read={r['read_gib_s']:.2f} [{r['transport_name']}] {'✓' if r['verified'] else '✗'}"
        )
        results.append(
            {
                "data_path": dp,
                "threads": t,
                "file_mode": fm,
                "write_gib_s": r["write_gib_s"],
                "read_gib_s": r["read_gib_s"],
                "transport_name": r["transport_name"],
                "verified": r["verified"],
            }
        )
    except Exception as e:
        print(f"ERROR: {e}")

print(f"\n{len(results)}/{len(configs)} passed")
