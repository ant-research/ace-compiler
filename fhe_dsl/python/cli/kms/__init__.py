# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
KMS (Key Management Service) CLI subcommands.

Subcommands:
    gen-enc-keys    Generate encryption keys (SK + PK)
    gen-eval-keys   Generate evaluation keys (Relin + Galois + Bootstrap)
    list            List key bundles in a directory
    info            Show detailed information about a key bundle
"""

from .gen_enc_keys import register as register_gen_enc_keys
from .gen_eval_keys import register as register_gen_eval_keys
from .info import register as register_info
from .list_keys import register as register_list


def register_parsers(subparsers):
    """Register all KMS subcommand parsers.

    Called from main.py to add KMS subcommands to the ``kms`` subparser group.
    """
    register_gen_enc_keys(subparsers)
    register_gen_eval_keys(subparsers)
    register_list(subparsers)
    register_info(subparsers)


def run(args):
    """Dispatch to the appropriate KMS subcommand handler."""
    command = getattr(args, "kms_command", None)

    if command is None:
        from . import __doc__ as pkg_doc

        print(pkg_doc)
        return

    dispatch = {
        "gen-enc-keys": _run_gen_enc_keys,
        "gen-eval-keys": _run_gen_eval_keys,
        "list": _run_list,
        "info": _run_info,
    }

    handler = dispatch.get(command)
    if handler is None:
        print(f"Unknown kms subcommand: {command}")
        return

    handler(args)


def _run_gen_enc_keys(args):
    from .gen_enc_keys import run

    run(args)


def _run_gen_eval_keys(args):
    from .gen_eval_keys import run

    run(args)


def _run_list(args):
    from .list_keys import run

    run(args)


def _run_info(args):
    from .info import run

    run(args)
