# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Key generation API for FHE encryption and evaluation keys.

This module provides the high-level API for generating FHE key bundles.
The actual cryptographic key generation is performed by the C++ runtime
(via the ckks-gpu library or the ant rtlib). This module handles:

1. Reading CKKS parameters (from user input or .conf files)
2. Creating the key bundle directory structure
3. Writing metadata.json
4. Delegating to the C++ bridge for key binary generation

**Current status**: The C++ bridge for key serialization is not yet
implemented. Key bundle directories and metadata are created, but the
actual key binary files (``.bin``) require the C++ serialization API
to be wired through pybind11. See the "C++ Bridge" section below for
details on what needs to be added.

C++ Bridge Requirements
-----------------------
To complete key generation, the following C++ → Python bindings are needed:

1. **Context creation** from CKKS params:
   ``make_context(poly_degree, primes, special_prime_count) → HostContext``

2. **Key generation**:
   ``KeyGenerator(context) → KeyGenerator``
   ``keygen.secret_key() → SecretKey``
   ``keygen.create_public_key() → PublicKey``
   ``keygen.create_relin_keys() → RelinKeys``
   ``keygen.create_galois_keys(steps) → GaloisKeys``

3. **Key serialization**:
   ``key.save(ostream, compr_mode) → size_t``
   ``KeyType.load(context, istream) → KeyType``

These are available in the ckks-gpu library (frontend namespace) but
not yet exposed via pybind11. Once bound, the ``_generate_key_binaries()``
functions in this module can be updated to call the C++ API.
"""

import logging
from pathlib import Path
from typing import Any, Optional

from .config_reader import CKKSParams, find_conf_file, read_conf
from .key_bundle import (
    KeyBundle,
    KeyBundleMetadata,
    make_encryption_metadata,
    make_evaluation_metadata,
)
from .storage import resolve_output_dir, save_key_bundle


logger = logging.getLogger(__name__)


def generate_encryption_keys(
    ckks_params: dict[str, int],
    output_dir: str | Path,
    key_id: str | None = None,
    description: str = "",
) -> KeyBundle:
    """Generate encryption keys (SK + PK) for client-side data encryption.

    Encryption keys are model-independent — they only depend on CKKS
    parameters (N, q0, sf). The same encryption keys can be used across
    different models that share the same CKKS parameters.

    Creates a key bundle directory with::

        output_dir/
        ├── metadata.json
        └── global/
            ├── secret_key.bin     # (requires C++ bridge)
            └── public_key.bin     # (requires C++ bridge)

    Args:
        ckks_params: CKKS parameters as a dict with keys:
            - ``N`` (int): Polynomial degree (e.g., 65536).
            - ``q0`` (int): First modulus bit size (e.g., 60).
            - ``sf`` (int): Scaling factor modulus bit size (e.g., 56).
            Optional keys:
            - ``hamming_weight`` (int, default 192).
            - ``sec_level`` (int, default 0).
        output_dir: Output directory for the key bundle.
        key_id: Optional key identifier. Defaults to ``enc-<timestamp>``.
        description: Optional human-readable description.

    Returns:
        The created :class:`KeyBundle`.

    Raises:
        ValueError: If required CKKS parameters are missing.
    """
    # Validate and construct CKKSParams
    required = {"N", "q0", "sf"}
    missing = required - set(ckks_params.keys())
    if missing:
        raise ValueError(f"Missing required CKKS parameters: {missing}")

    params = CKKSParams(
        N=ckks_params["N"],
        q0=ckks_params["q0"],
        sf=ckks_params["sf"],
        hamming_weight=ckks_params.get("hamming_weight", 192),
        sec_level=ckks_params.get("sec_level", 0),
    )

    # Resolve output directory and key_id
    output_dir, key_id = resolve_output_dir(output_dir, key_id)

    # Create metadata
    metadata = make_encryption_metadata(
        ckks_params=params,
        key_id=key_id,
        description=description,
    )

    # Create key bundle
    bundle = KeyBundle(path=output_dir, metadata=metadata)

    # Save directory structure and metadata
    save_key_bundle(bundle, create_dirs=True)

    logger.info(
        "Created encryption key bundle: key_id=%s, path=%s",
        key_id,
        output_dir,
    )

    # Generate key binaries (requires C++ bridge)
    _generate_encryption_key_binaries(bundle, params)

    return bundle


def generate_evaluation_keys(
    compiled_dir: str | Path,
    output_dir: str | Path,
    key_id: str | None = None,
    description: str = "",
) -> KeyBundle:
    """Generate evaluation keys (Relin + Galois + Bootstrap) from a compiled model.

    Evaluation keys are model-specific — they depend on the rotation indices
    and multiplication depth from the compiled model's ``.conf`` file.
    The ``.conf`` file is automatically located in ``compiled_dir``.

    Creates a key bundle directory with::

        output_dir/
        ├── metadata.json
        └── model/
            ├── relin_key.bin      # (requires C++ bridge)
            ├── galois_key.bin     # (requires C++ bridge)
            └── bootstrap_key.bin  # (requires C++ bridge, if mul_depth > 0)

    Args:
        compiled_dir: Path to the compiled model directory containing
            a ``.conf`` file.
        output_dir: Output directory for the key bundle.
        key_id: Optional key identifier. Defaults to ``eval-<timestamp>``.
        description: Optional human-readable description.

    Returns:
        The created :class:`KeyBundle`.

    Raises:
        FileNotFoundError: If compiled_dir or .conf file doesn't exist.
        ValueError: If required fields are missing from .conf.
    """
    # Read CKKS parameters from .conf file
    conf_path = find_conf_file(compiled_dir)
    params, raw_conf = read_conf(conf_path)

    # Extract model name from conf
    model_name = raw_conf.get("model", conf_path.stem)

    # Resolve output directory and key_id
    output_dir, key_id = resolve_output_dir(output_dir, key_id)

    # Create metadata
    metadata = make_evaluation_metadata(
        ckks_params=params,
        model_name=model_name,
        conf_source=str(conf_path),
        key_id=key_id,
        description=description,
    )

    # Create key bundle
    bundle = KeyBundle(path=output_dir, metadata=metadata)

    # Save directory structure and metadata
    save_key_bundle(bundle, create_dirs=True)

    logger.info(
        "Created evaluation key bundle: key_id=%s, path=%s, rot_idxs=%d, mul_depth=%d",
        key_id,
        output_dir,
        len(params.rot_idxs),
        params.mul_depth,
    )

    # Generate key binaries (requires C++ bridge)
    _generate_evaluation_key_binaries(bundle, params)

    return bundle


# ----------------------------------------------------------------
# C++ Bridge (placeholder)
# ----------------------------------------------------------------


def _generate_encryption_key_binaries(
    bundle: KeyBundle,
    params: CKKSParams,
) -> None:
    """Generate encryption key binary files (SK + PK).

    This function creates the actual cryptographic key files in the bundle
    directory. Currently a placeholder — requires the C++ key generation
    and serialization API to be exposed via pybind11.

    When the C++ bridge is available, this function will:

    1. Create a CKKS context from the parameters:
       ``context = make_context(params.N, [primes...], num_special_primes)``

    2. Generate keys:
       ``keygen = KeyGenerator(context)``
       ``sk = keygen.secret_key()``
       ``pk = keygen.create_public_key()``

    3. Serialize to disk:
       ``sk.save(bundle.secret_key_path)``
       ``pk.save(bundle.public_key_path)``

    Args:
        bundle: Key bundle with directory structure already created.
        params: CKKS parameters for key generation.
    """
    sk_path = bundle.secret_key_path
    pk_path = bundle.public_key_path

    if not sk_path.parent.exists():
        sk_path.parent.mkdir(parents=True, exist_ok=True)

    # TODO: Implement C++ bridge for key generation and serialization
    # from ace import runtime as _C  (or a new kms C extension)
    #
    # context = _C.make_context(params.N, params.q0, params.sf, ...)
    # keygen = _C.KeyGenerator(context)
    # sk = keygen.secret_key()
    # pk = keygen.create_public_key()
    # sk.save(str(sk_path))
    # pk.save(str(pk_path))

    logger.warning(
        "Key binary generation not yet implemented. "
        "Key bundle structure created at %s but key files are empty placeholders. "
        "C++ serialization bridge required for actual key generation.",
        bundle.path,
    )

    # Write placeholder files to indicate expected key locations
    _write_placeholder(sk_path, "secret_key", params)
    _write_placeholder(pk_path, "public_key", params)


def _generate_evaluation_key_binaries(
    bundle: KeyBundle,
    params: CKKSParams,
) -> None:
    """Generate evaluation key binary files (Relin + Galois + Bootstrap).

    This function creates the actual evaluation key files in the bundle
    directory. Currently a placeholder — requires the C++ key generation
    and serialization API to be exposed via pybind11.

    When the C++ bridge is available, this function will:

    1. Create a CKKS context from the parameters
    2. Generate evaluation keys:
       ``keygen = KeyGenerator(context, secret_key)``  # reuse SK
       ``relin = keygen.create_relin_keys()``
       ``galois = keygen.create_galois_keys(params.rot_idxs)``
    3. Optionally generate bootstrap keys (if mul_depth > 0)
    4. Serialize to disk

    Args:
        bundle: Key bundle with directory structure already created.
        params: CKKS parameters from the compiled model's .conf file.
    """
    model_dir = bundle.model_dir
    if not model_dir.exists():
        model_dir.mkdir(parents=True, exist_ok=True)

    # TODO: Implement C++ bridge for key generation and serialization
    # from ace import runtime as _C  (or a new kms C extension)
    #
    # context = _C.make_context(params.N, params.q0, params.sf, ...)
    # keygen = _C.KeyGenerator(context)
    # relin = keygen.create_relin_keys()
    # galois = keygen.create_galois_keys(params.rot_idxs)
    # relin.save(str(bundle.relin_key_path))
    # galois.save(str(bundle.galois_key_path))
    #
    # if params.mul_depth > 0:
    #     # Bootstrap key generation
    #     bootstrap = keygen.create_bootstrap_keys(...)
    #     bootstrap.save(str(bundle.bootstrap_key_path))

    logger.warning(
        "Key binary generation not yet implemented. "
        "Key bundle structure created at %s but key files are empty placeholders. "
        "C++ serialization bridge required for actual key generation.",
        bundle.path,
    )

    # Write placeholder files
    _write_placeholder(bundle.relin_key_path, "relin_key", params)
    _write_placeholder(bundle.galois_key_path, "galois_key", params)
    if params.mul_depth > 0:
        _write_placeholder(bundle.bootstrap_key_path, "bootstrap_key", params)


def _write_placeholder(path: Path, key_type: str, params: CKKSParams) -> None:
    """Write a placeholder file indicating where a key binary will be.

    The placeholder is a small text file with metadata about the expected
    key. It will be replaced by actual key binary data when the C++
    serialization bridge is implemented.
    """
    header = (
        f"PLACEHOLDER — {key_type}\n"
        f"This file is a placeholder. Actual key binary generation\n"
        f"requires the C++ serialization bridge (pybind11 bindings for\n"
        f"ckks-gpu KeyGenerator and Serialization APIs).\n\n"
        f"CKKS params: N={params.N}, q0={params.q0}, sf={params.sf}\n"
    )
    path.write_text(header, encoding="utf-8")
    logger.debug("Wrote placeholder: %s", path)
