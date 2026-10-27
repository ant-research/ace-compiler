# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
GDS CLI subcommands.

Subcommands:
    check       — Verify GDS/cuFile I/O round-trip (needs GPU + cuFile)
    env-check   — Diagnose GDS environment (no GPU I/O, safe for containers)
"""

from .check import register as register_check
from .env_check import register as register_env_check


def register_parsers(subparsers):
    """Register all gds subcommand parsers.

    Called from main.py to add gds subcommands to the ``gds`` subparser group.
    """
    register_check(subparsers)
    register_env_check(subparsers)


def run(args):
    """Dispatch to the appropriate gds subcommand handler."""
    command = getattr(args, "gds_command", None)

    if command is None:
        from . import __doc__ as pkg_doc

        print(pkg_doc)
        return

    dispatch = {
        "check": _run_check,
        "env-check": _run_env_check,
    }

    handler = dispatch.get(command)
    if handler is None:
        print(f"Unknown gds subcommand: {command}")
        return

    handler(args)


def _run_check(args):
    from .check import run

    run(args)


def _run_env_check(args):
    from .env_check import run

    run(args)
