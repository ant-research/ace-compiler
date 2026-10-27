# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ace_tool doctor — ACE FHE environment diagnostics.

Checks Python, compiled libraries, GPU/CUDA, backends,
model assets, and dataset readiness.

Usage:
    ace_tool doctor
"""

import sys


def register(subparsers):
    """Register the doctor subcommand parser."""
    subparsers.add_parser(
        "doctor",
        help="Diagnose ACE FHE environment readiness",
        description="Check Python, compiled binaries, GPU/CUDA, "
        "FHE backends, model assets, and dataset availability. "
        "Reports pass/fail for each category with remediation hints. "
        "Exit code 0 if all critical checks pass, 1 otherwise.",
    )


def run(args):
    """Execute the doctor environment check."""
    from ace.fhe.check_env import run_doctor

    rc = run_doctor()
    sys.exit(rc)
