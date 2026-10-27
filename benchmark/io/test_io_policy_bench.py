# =============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# =============================================================================

"""IO performance benchmark — IoPolicySettings API validation.

Validates that the recommended IoPolicySettings Python API correctly drives
the C++ IO layer. Python sets policy via IoPolicySettings.apply(), then
calls _C.bench_io_roundtrip() which reads the global policy internally.

This tests PURPOSE 1 (calibrate IoPolicy top-level interface) and
PURPOSE 2 (confirm Python-layer abstraction).

Sub-matrix: ~20 representative combinations (not full 108).
"""

import os

import pytest
from _io_bench_util import policy_to_expected_transport
from conftest import GiB, available_disks, skip_if_no_gpu

from ace import runtime as _C
from ace.fhe.config import IoPolicy, IoPolicySettings


# =============================================================================
# Test parameters — representative sub-matrix
# =============================================================================

# (policy, threads_via_expert_config, disks, file_mode, data_size_gib)
POLICY_BENCH_PARAMS = [
    # auto: basic sanity
    ("auto", 1, 1, "single", 10),
    ("auto", 4, 1, "single", 10),
    # gds: primary use case
    ("gds", 1, 1, "single", 10),
    ("gds", 4, 1, "single", 10),
    ("gds", 4, 1, "multi", 10),
    ("gds", 2, 2, "single", 10),
    # staged: always staged
    ("staged", 1, 1, "single", 10),
    ("staged", 1, 1, "multi", 10),
    # mapped: one less copy
    ("mapped", 4, 1, "single", 10),
    ("mapped", 4, 1, "single", 20),
    ("mapped", 2, 4, "multi", 10),
]


def _make_param_id(p):
    policy, threads, disks, mode, size = p
    return f"{policy}_T{threads}_D{disks}_{mode}_{size}G"


# =============================================================================
# Test class
# =============================================================================


@pytest.mark.slow
@skip_if_no_gpu
class TestIoPolicyBench:
    """Benchmark IoPolicySettings API across representative configurations."""

    @pytest.mark.parametrize(
        "policy,threads,disks,file_mode,data_size_gib",
        POLICY_BENCH_PARAMS,
        ids=[_make_param_id(p) for p in POLICY_BENCH_PARAMS],
    )
    def test_policy_roundtrip(self, policy, threads, disks, file_mode, data_size_gib, tmp_bench_dir, bench_collector):
        """Run IO roundtrip via IoPolicySettings and validate results."""
        # Get accessible disks
        try:
            disk_list = available_disks(disks)
        except pytest.skip.Exception:
            pytest.skip(
                f"Only {len([d for d in ['/mnt/nvme0', '/mnt/nvme1', '/mnt/nvme2', '/mnt/nvme3'] if os.path.isdir(d)])} disks available, need {disks}"
            )

        # Step 1: Apply policy via IoPolicySettings (recommended API)
        IoPolicySettings(policy=policy, disk_paths=disk_list).apply()

        # Step 2: Set threads via expert IoConfig (only tuning knob)
        from ace.fhe.config import IoConfig

        IoConfig(threads=threads, disk_paths=disk_list).apply()

        # Step 3: Run benchmark — only path/data_size/num_files
        bench_path = os.path.join(disk_list[0], "bench_policy")
        num_files = 1 if file_mode == "single" else max(4, threads)
        data_size = int(data_size_gib * GiB)

        result = _C.bench_io_roundtrip(bench_path, data_size, num_files)

        # Step 4: Validate
        assert result["verified"], f"Data verification failed for {policy} T{threads} D{disks}"
        assert result["write_gib_s"] > 0, f"Write throughput zero for {policy}"
        assert result["read_gib_s"] > 0, f"Read throughput zero for {policy}"

        # Step 5: Validate policy → transport mapping
        transport = result["transport_name"]
        gds_avail = result["gds_available"]
        expected = policy_to_expected_transport(policy, gds_avail)
        if expected:  # auto has no strict expectation
            assert transport.startswith(expected) or expected in transport, (
                f"Policy '{policy}' expected transport '{expected}', got '{transport}'"
            )

        # Step 6: Collect results
        bench_collector.append(
            {
                "policy": policy,
                "threads": threads,
                "disks": disks,
                "file_mode": file_mode,
                "data_size_gib": data_size_gib,
                "data_path": "",  # set by policy internally
                "write_ms": result["write_ms"],
                "read_ms": result["read_ms"],
                "write_gib_s": result["write_gib_s"],
                "read_gib_s": result["read_gib_s"],
                "transport_name": transport,
                "verified": result["verified"],
                "gds_available": gds_avail,
            }
        )

        # Print summary
        print(
            f"\n  {policy}/T{threads}/D{disks}/{file_mode}/{data_size_gib}GiB: "
            f"write={result['write_gib_s']:.2f}GiB/s "
            f"read={result['read_gib_s']:.2f}GiB/s "
            f"[{transport}] verified={'✓' if result['verified'] else '✗'}"
        )
