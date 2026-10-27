# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
GDS check subcommand — verify cuFile I/O round-trip.

Tests the full GDS pipeline: driver open → GPU buffer → register →
write (GPU→file) → read (file→GPU) → data verification.

Usage:
    ace_tool gds check
    ace_tool gds check --size 1024 --dir /data
    ace_tool gds check --keep
"""

import argparse
import sys


def register(subparsers):
    """Register the check subcommand parser."""
    p = subparsers.add_parser(
        "check",
        help="Verify GDS/cuFile I/O round-trip (needs GPU + cuFile)",
        description="Test the full cuFile lifecycle: driver open, GPU buffer "
        "registration, file handle registration, write (GPU→file), "
        "read (file→GPU), and data verification.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument(
        "--size",
        type=int,
        default=4,
        help="Test buffer size in KB (default: 4, rounded up to 4KB for O_DIRECT)",
    )
    p.add_argument(
        "--dir",
        type=str,
        default="/tmp",
        help="Directory for the temporary test file (default: /tmp). "
        "Must be on a disk-backed filesystem (ext4/xfs), not tmpfs.",
    )
    p.add_argument(
        "--keep",
        action="store_true",
        help="Keep the test file after the run (for debugging)",
    )


def run(args):
    """Execute the GDS I/O check."""
    from ace.gds.check_io import run_io_check

    rc = run_io_check(size_kb=args.size, test_dir=args.dir, keep=args.keep)
    sys.exit(rc)
