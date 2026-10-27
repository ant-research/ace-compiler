# =============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# =============================================================================

"""IO performance benchmark fixtures.

Provides GPU availability checks, disk path fixtures, and temp directory
management for benchmark/io/ tests.
"""

import os
import shutil
import tempfile

import pytest


# Default NVMe disk paths for multi-disk benchmarks
DISK_PATHS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]

# Unit constants
GiB = 1024**3


def _gpu_available():
    """Check if CUDA GPU is available via ace runtime."""
    try:
        import torch

        from ace import runtime as _C

        return torch.cuda.is_available()
    except Exception:
        return False


def _gds_available():
    """Check if GDS/cuFile is available."""
    try:
        from ace import runtime as _C

        return _C.gds_available()
    except Exception:
        return False


# Skip markers
skip_if_no_gpu = pytest.mark.skipif(not _gpu_available(), reason="CUDA GPU not available")

skip_if_no_gds = pytest.mark.skipif(not _gds_available(), reason="GDS/cuFile not available")


def available_disks(n: int) -> list[str]:
    """Return the first n accessible disk paths.

    Raises pytest.skip if fewer than n disks are accessible.
    """
    result = []
    for dp in DISK_PATHS:
        if os.path.isdir(dp) and os.access(dp, os.W_OK | os.R_OK):
            result.append(dp)
        if len(result) >= n:
            break
    if len(result) < n:
        pytest.skip(f"Only {len(result)}/{n} NVMe disks accessible")
    return result


# =============================================================================
# Fixtures
# =============================================================================


@pytest.fixture(params=[1, 2, 4], ids=["1disk", "2disk", "4disk"])
def disk_paths(request):
    """Parametrized fixture: 1, 2, or 4 NVMe disks."""
    return available_disks(request.param)


@pytest.fixture
def tmp_bench_dir():
    """Create a temporary directory for benchmark files, cleaned up after."""
    d = tempfile.mkdtemp(prefix="io_bench_")
    yield d
    shutil.rmtree(d, ignore_errors=True)


@pytest.fixture(scope="session")
def bench_collector():
    """Session-scoped collector for benchmark results.

    After all tests complete, writes results to JSON and markdown.
    """
    results = []

    yield results

    if not results:
        return

    # Write JSON output
    import json

    output_dir = os.path.join(tempfile.gettempdir(), "io_bench_results")
    os.makedirs(output_dir, exist_ok=True)

    json_path = os.path.join(output_dir, "io_bench_results.json")
    with open(json_path, "w") as f:
        json.dump(results, f, indent=2)
    print(f"\nIO benchmark results written to: {json_path}")

    # Write markdown report
    from _io_bench_util import write_report

    md_path = os.path.join(output_dir, "io_bench_report.md")
    write_report(results, md_path)
    print(f"IO benchmark report written to: {md_path}")
