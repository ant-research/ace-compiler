# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ACE KMS — Key Management Service for FHE encryption and evaluation keys.

Provides Python APIs and CLI tools for generating, storing, and inspecting
FHE key bundles used in Client-Server deployment, as well as offline
preprocessing (weight encoding and key saving) for faster inference.

Key types:
    - Encryption keys (SK + PK): model-independent, depend only on CKKS params
    - Evaluation keys (Relin + Galois + Bootstrap): model-specific, depend on
      rotation indices and multiplication depth from the compiled .conf file

Offline preprocessing (DISABLED — pending a standalone encoding tool):
    - OfflinePrep / OfflineEncoder: the offline weight-encoding + key-save
      helper used to pre-encode DE_MSG_F32 weights to DE_PLAINTEXT and save
      generated keys. The encode-only FheContext path (EncPolicy::BYPASS) was
      removed; OfflinePrep.__init__/encode/encode_package/save_keys all raise
      NotImplementedError. Key saving moved to FHERuntime.save_keys (see
      below); weight encoding is pending reimplementation as a standalone tool.

Usage (Python API — key saving):
    # Save keys (SK+PK+RLK+GK) from a live FHERuntime, BEFORE finalize():
    from ace import fhe
    runner = fhe.FHERuntime(pkg)
    runner.run(x)                                       # generate keys in memory
    runner.save_keys("/path/to/keys")
    runner.save_keys("/path/to/keys", eval_only=True)   # eval-only (RLK+GK)
    runner.finalize()

Usage (Python API — keys):
    from ace.kms import generate_encryption_keys, generate_evaluation_keys

    # Generate encryption keys (client-side)
    generate_encryption_keys(
        ckks_params={"N": 65536, "q0": 60, "sf": 56},
        output_dir="./keys/global/",
    )

    # Generate evaluation keys (server-side, from compiled model)
    generate_evaluation_keys(
        compiled_dir="./compiled/resnet20/",
        output_dir="./keys/resnet20-v1/",
    )

Usage (CLI):
    ace_tool kms gen-enc-keys --ckks N=65536,q0=60,sf=56 --output ./keys/global/
    ace_tool kms gen-eval-keys --compiled-dir ./compiled/resnet20/ --output ./keys/resnet20-v1/
    ace_tool kms list
    ace_tool kms info --key-id resnet20-v1
"""

from .config_reader import CKKSParams, find_conf_file, read_conf
from .generator import generate_encryption_keys, generate_evaluation_keys
from .key_bundle import KeyBundle, KeyBundleMetadata
from .offline import OfflinePrep
from .storage import get_key_info, list_keys, load_key_bundle, save_key_bundle


__all__ = [
    "CKKSParams",
    "KeyBundle",
    "KeyBundleMetadata",
    "OfflinePrep",
    "find_conf_file",
    "generate_encryption_keys",
    "generate_evaluation_keys",
    "get_key_info",
    "list_keys",
    "load_key_bundle",
    "read_conf",
    "save_key_bundle",
]
