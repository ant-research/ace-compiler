# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
KMS info subcommand — show key bundle details.

Usage:
    ace_tool kms info --key-id resnet20-v1
    ace_tool kms info --key-id resnet20-v1 --key-dir /path/to/keys/
"""

import json


def register(subparsers):
    """Register the info subcommand parser."""
    p = subparsers.add_parser(
        "info",
        help="Show detailed information about a key bundle",
        description="Display metadata and details for a specific key bundle, "
        "including CKKS parameters, key sizes, and creation time.",
    )
    p.add_argument(
        "--key-id",
        type=str,
        required=True,
        help="Key identifier to inspect",
    )
    p.add_argument(
        "--key-dir",
        type=str,
        default="./keys/",
        help="Directory containing key bundles (default: ./keys/)",
    )


def run(args):
    """Execute the info subcommand."""
    from ace.kms import get_key_info

    info = get_key_info(key_id=args.key_id, key_dir=args.key_dir)
    print(json.dumps(info, indent=2, default=str))
