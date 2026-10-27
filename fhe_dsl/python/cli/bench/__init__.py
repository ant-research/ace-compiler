# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Benchmark CLI subcommands.

Subcommands:
    latency    Detailed latency benchmark (init vs inference phase breakdown)
"""

from .latency import register as register_latency


def register_parsers(subparsers):
    """Register all bench subcommand parsers.

    Called from main.py to add bench subcommands to the ``bench`` subparser group.
    """
    register_latency(subparsers)


def run(args):
    """Dispatch to the appropriate bench subcommand handler."""
    command = getattr(args, "bench_command", None)

    if command is None:
        from . import __doc__ as pkg_doc

        print(pkg_doc)
        return

    dispatch = {
        "latency": _run_latency,
    }

    handler = dispatch.get(command)
    if handler is None:
        print(f"Unknown bench subcommand: {command}")
        return

    handler(args)


def _run_latency(args):
    from .latency import run

    run(args)
