# tests/unit/io_transport/test_io_bench_util.py
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Unit tests for IO benchmark policy-to-transport mapping.

No GPU required — exercises the business logic in policy_to_expected_transport().
"""

import os
import sys

import pytest


# Add benchmark/io to sys.path so we can import _io_bench_util
_bench_io_dir = os.path.join(os.path.dirname(__file__), "..", "..", "..", "benchmark", "io")
if _bench_io_dir not in sys.path:
    sys.path.insert(0, _bench_io_dir)

from _io_bench_util import BenchResult, collect_results, policy_to_expected_transport, write_report  # noqa: E402


class TestPolicyToExpectedTransport:
    """Test policy_to_expected_transport mapping."""

    def test_staged_always_staged(self):
        assert policy_to_expected_transport("staged", gds_available=True) == "staged"
        assert policy_to_expected_transport("staged", gds_available=False) == "staged"

    def test_mapped_always_mapped(self):
        assert policy_to_expected_transport("mapped", gds_available=True) == "mapped"

    def test_gds_with_gds(self):
        assert policy_to_expected_transport("gds", gds_available=True) == "gds"

    def test_gds_without_gds(self):
        assert policy_to_expected_transport("gds", gds_available=False) == "gds(fallback)"

    def test_auto_with_gds(self):
        assert policy_to_expected_transport("auto", gds_available=True) == "gds"

    def test_auto_without_gds(self):
        assert policy_to_expected_transport("auto", gds_available=False) == "staged"

    # Backward compat: deprecated names
    def test_checkpoint_always_staged(self):
        assert policy_to_expected_transport("checkpoint", gds_available=True) == "staged"
        assert policy_to_expected_transport("checkpoint", gds_available=False) == "staged"

    def test_galois_key_load_with_gds(self):
        assert policy_to_expected_transport("galois_key_load", gds_available=True) == "gds"

    def test_galois_key_load_without_gds(self):
        assert policy_to_expected_transport("galois_key_load", gds_available=False) == "staged"

    def test_high_throughput_with_gds(self):
        assert policy_to_expected_transport("high_throughput", gds_available=True) == "gds"

    def test_high_throughput_without_gds(self):
        assert policy_to_expected_transport("high_throughput", gds_available=False) == "staged"


class TestBenchResult:
    """Test BenchResult dataclass defaults and construction."""

    def test_defaults(self):
        br = BenchResult()
        assert br.data_path == ""
        assert br.policy == ""
        assert br.threads == 0
        assert br.disks == 1
        assert br.file_mode == "single"
        assert br.data_size_gib == 0.0
        assert br.write_ms == 0.0
        assert br.write_gib_s == 0.0
        assert br.read_ms == 0.0
        assert br.read_gib_s == 0.0
        assert br.transport_name == ""
        assert br.verified is False
        assert br.gds_available is False

    def test_custom_values(self):
        br = BenchResult(
            policy="gds",
            threads=4,
            disks=2,
            data_size_gib=10.0,
            read_gib_s=20.5,
            transport_name="GdsTransport",
            verified=True,
        )
        assert br.policy == "gds"
        assert br.threads == 4
        assert br.disks == 2
        assert br.data_size_gib == 10.0
        assert br.read_gib_s == 20.5
        assert br.verified is True

    def test_data_path_backward_compat(self):
        """data_path field is kept for backward compat."""
        br = BenchResult(data_path="staged", policy="staged")
        assert br.data_path == "staged"
        assert br.policy == "staged"


class TestCollectResults:
    """Test collect_results() conversion from raw dicts."""

    def test_empty(self):
        assert collect_results([]) == []

    def test_single_result(self):
        raw = [{"policy": "gds", "threads": 4, "disks": 2, "read_gib_s": 15.0, "transport_name": "GdsTransport"}]
        results = collect_results(raw)
        assert len(results) == 1
        assert results[0].policy == "gds"
        assert results[0].threads == 4
        assert results[0].disks == 2
        assert results[0].read_gib_s == 15.0

    def test_policy_fallback_from_data_path(self):
        """When 'policy' is missing, fall back to 'data_path'."""
        raw = [{"data_path": "staged", "threads": 1}]
        results = collect_results(raw)
        assert results[0].policy == "staged"
        assert results[0].data_path == "staged"

    def test_data_path_fallback_from_policy(self):
        """When 'data_path' is missing, fall back to 'policy'."""
        raw = [{"policy": "gds", "threads": 2}]
        results = collect_results(raw)
        assert results[0].data_path == "gds"

    def test_multiple_results(self):
        raw = [
            {"policy": "staged", "threads": 1, "read_gib_s": 5.0},
            {"policy": "gds", "threads": 4, "read_gib_s": 20.0},
        ]
        results = collect_results(raw)
        assert len(results) == 2
        assert results[0].policy == "staged"
        assert results[1].policy == "gds"


class TestWriteReport:
    """Test write_report() markdown generation."""

    def test_empty_results(self, tmp_path):
        """Empty results should produce no file."""
        output = tmp_path / "report.md"
        write_report([], str(output))
        assert not output.exists()

    def test_basic_report(self, tmp_path):
        """Non-empty results should produce a markdown report."""
        results = [
            {
                "policy": "staged",
                "threads": 1,
                "disks": 1,
                "data_size_gib": 10,
                "write_gib_s": 5.0,
                "read_gib_s": 6.0,
                "transport_name": "StagedTransport",
                "verified": True,
                "file_mode": "single",
            },
        ]
        output = tmp_path / "report.md"
        write_report(results, str(output))
        assert output.exists()
        content = output.read_text()
        assert "staged" in content
        assert "StagedTransport" in content
        assert "✓" in content
