# =============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# =============================================================================

"""IO performance benchmark — IoPolicy expert API validation.

Full combo matrix testing IoPolicy x threads x disks x file_mode x data_size.

Validates that the IoPolicy API correctly maps to C++ transport selection
and achieves expected throughput.

This tests PURPOSE 1 (calibrate IoPolicy top-level interface) and
PURPOSE 2 (confirm Python-layer abstraction).
"""

import os

import pytest
from _io_bench_util import policy_to_expected_transport
from conftest import GiB, available_disks, skip_if_no_gds, skip_if_no_gpu

from ace import runtime as _C
from ace.fhe.config import IoPolicy, IoPolicySettings


# =============================================================================
# Test parameters — full combo matrix
# =============================================================================

IO_POLICIES = ["staged", "mapped", "gds"]
THREADS = [1, 2, 4]
DISKS = [1, 2, 4]
FILE_MODES = ["single", "multi"]  # single=单文件, multi=多文件
DATA_SIZES = [10, 20]  # GiB


def _policy_id(p):
    return p


@pytest.mark.slow
@skip_if_no_gpu
class TestIoDataPathBench:
    """Benchmark IoPolicy API across full matrix."""

    @pytest.mark.parametrize("io_policy", IO_POLICIES, ids=_policy_id)
    @pytest.mark.parametrize("threads", THREADS, ids=lambda t: f"T{t}")
    @pytest.mark.parametrize("disks", DISKS, ids=lambda d: f"D{d}")
    @pytest.mark.parametrize("file_mode", FILE_MODES, ids=lambda m: m)
    @pytest.mark.parametrize("data_size_gib", DATA_SIZES, ids=lambda s: f"{s}G")
    def test_policy_roundtrip(
        self, io_policy, threads, disks, file_mode, data_size_gib, tmp_bench_dir, bench_collector
    ):
        """Run IO roundtrip via IoPolicy and validate results."""
        # Skip GDS if cuFile not available
        if io_policy == "gds" and not _C.gds_available():
            pytest.skip("GDS not available")

        # Get accessible disks
        try:
            disk_list = available_disks(disks)
        except pytest.skip.Exception:
            pytest.skip(f"Insufficient disks ({disks} required)")

        # Step 1: Apply IoPolicy via IoPolicySettings
        IoPolicySettings(policy=io_policy, disk_paths=disk_list).apply()

        # Step 2: Set threads via expert IoConfig
        from ace.fhe.config import IoConfig

        IoConfig(
            threads=threads,
            disk_paths=disk_list,
        ).apply()

        # Step 3: Run benchmark
        bench_path = os.path.join(disk_list[0], "bench_dp")
        num_files = 1 if file_mode == "single" else max(4, threads)
        data_size = int(data_size_gib * GiB)

        result = _C.bench_io_roundtrip(bench_path, data_size, num_files)

        # Step 4: Validate basic correctness
        assert result["verified"], f"Data verification failed for {io_policy} T{threads} D{disks}"
        assert result["write_gib_s"] > 0, f"Write throughput zero for {io_policy} T{threads}"
        assert result["read_gib_s"] > 0, f"Read throughput zero for {io_policy} T{threads}"

        # Step 5: Validate policy → transport mapping
        transport = result["transport_name"]
        gds_avail = result["gds_available"]
        expected = policy_to_expected_transport(io_policy, gds_avail)
        if expected:
            assert expected in transport, (
                f"IoPolicy '{io_policy}' expected transport containing '{expected}', got '{transport}'"
            )

        # Step 6: Collect results
        bench_collector.append(
            {
                "data_path": io_policy,  # kept for backward compat in reports
                "policy": io_policy,
                "threads": threads,
                "disks": disks,
                "file_mode": file_mode,
                "data_size_gib": data_size_gib,
                "write_ms": result["write_ms"],
                "read_ms": result["read_ms"],
                "write_gib_s": result["write_gib_s"],
                "read_gib_s": result["read_gib_s"],
                "transport_name": transport,
                "verified": result["verified"],
                "gds_available": result["gds_available"],
            }
        )

        # Print summary
        print(
            f"\n  {io_policy}/T{threads}/D{disks}/{file_mode}/{data_size_gib}GiB: "
            f"write={result['write_gib_s']:.2f}GiB/s "
            f"read={result['read_gib_s']:.2f}GiB/s "
            f"[{transport}] verified={'✓' if result['verified'] else '✗'}"
        )
