# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
KMS gen-enc-keys subcommand — generate encryption keys (SK + PK).

Usage:
    ace_tool kms gen-enc-keys --ckks N=65536,q0=60,sf=56 --output ./keys/global/
    ace_tool kms gen-enc-keys --ckks N=65536,q0=60,sf=56 --output ./keys/global/ --key-id my-key
"""

import argparse


def register(subparsers):
    """Register the gen-enc-keys subcommand parser."""
    p = subparsers.add_parser(
        "gen-enc-keys",
        help="Generate encryption keys (secret key + public key)",
        description="Generate CKKS encryption keys (SK + PK) for client-side data encryption. "
        "These keys are model-independent and only depend on CKKS parameters.",
    )
    p.add_argument(
        "--ckks",
        type=str,
        required=True,
        help='CKKS parameters as comma-separated key=value pairs, e.g. "N=65536,q0=60,sf=56"',
    )
    p.add_argument(
        "--output",
        "-o",
        type=str,
        required=True,
        help="Output directory for generated key files",
    )
    p.add_argument(
        "--key-id",
        type=str,
        default=None,
        help="Optional key identifier (defaults to timestamp-based ID)",
    )


def run(args):
    """Execute the gen-enc-keys subcommand."""
    # Parse CKKS parameters
    ckks_params = _parse_ckks(args.ckks)

    # Delegate to KMS Python API
    from ace.kms import generate_encryption_keys

    generate_encryption_keys(
        ckks_params=ckks_params,
        output_dir=args.output,
        key_id=args.key_id,
    )


def _parse_ckks(ckks_str):
    """Parse CKKS parameter string into a dict.

    Accepts formats like:
        N=65536,q0=60,sf=56
        N=65536, q0=60, sf=56
    """
    params = {}
    for pair in ckks_str.split(","):
        pair = pair.strip()
        if "=" not in pair:
            raise argparse.ArgumentTypeError(f"Invalid CKKS parameter: '{pair}'. Expected key=value format.")
        key, value = pair.split("=", 1)
        key = key.strip()
        value = value.strip()
        try:
            params[key] = int(value)
        except ValueError as err:
            raise argparse.ArgumentTypeError(
                f"Invalid CKKS parameter value: '{value}' for key '{key}'. Expected integer."
            ) from err
    return params
