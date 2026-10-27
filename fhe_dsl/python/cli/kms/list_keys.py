# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
KMS list subcommand — list key bundles.

Usage:
    ace_tool kms list
    ace_tool kms list --key-dir /path/to/keys/
"""


def register(subparsers):
    """Register the list subcommand parser."""
    p = subparsers.add_parser(
        "list",
        help="List key bundles in a directory",
        description="List all key bundles found in the key directory. "
        "Each key bundle contains a metadata.json file with key information.",
    )
    p.add_argument(
        "--key-dir",
        type=str,
        default="./keys/",
        help="Directory containing key bundles (default: ./keys/)",
    )


def run(args):
    """Execute the list subcommand."""
    from ace.kms import list_keys

    keys = list_keys(key_dir=args.key_dir)
    if not keys:
        print(f"No key bundles found in {args.key_dir}")
        return

    print(f"Key bundles in {args.key_dir}:")
    for key_info in keys:
        key_id = key_info.get("key_id", "unknown")
        created = key_info.get("created_at", "unknown")
        print(f"  {key_id}  (created: {created})")
