# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
GDS env-check subcommand — diagnose GDS environment prerequisites.

Checks kernel modules, device nodes, libraries, configuration,
filesystem support, and container setup — no GPU I/O required.

Usage:
    ace_tool gds env-check
    ace_tool gds env-check --dir /data
"""

import argparse
import sys


def register(subparsers):
    """Register the env-check subcommand parser."""
    p = subparsers.add_parser(
        "env-check",
        help="Diagnose GDS environment prerequisites (no GPU I/O)",
        description="Check runtime prerequisites for GPUDirect Storage: "
        "kernel modules, device nodes, library availability, "
        "configuration, filesystem O_DIRECT support, and container setup. "
        "Safe to run in any environment — no GPU I/O is performed.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument(
        "--dir",
        type=str,
        default="/tmp",
        help="Directory to check for O_DIRECT support (default: /tmp). Use a disk-backed mount if /tmp is tmpfs.",
    )


def run(args):
    """Execute the GDS environment check."""
    from ace.gds.check_env import run_env_check

    rc = run_env_check(test_dir=args.dir)
    sys.exit(rc)
