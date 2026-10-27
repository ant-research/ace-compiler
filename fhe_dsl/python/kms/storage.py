# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
File-based key bundle storage.

Key bundles are stored as directories containing key files and a
``metadata.json`` file. The default key directory is ``./keys/`` with
each key bundle in a subdirectory named by its key_id.

Layout::

    keys/
    ├── enc-20260605-120000/
    │   ├── metadata.json
    │   └── global/
    │       ├── secret_key.bin
    │       └── public_key.bin
    └── eval-20260605-120100/
        ├── metadata.json
        └── model/
            ├── relin_key.bin
            ├── galois_key.bin
            └── bootstrap_key.bin
"""

import json
import logging
from pathlib import Path
from typing import Any, Optional

from .key_bundle import KeyBundle, KeyBundleMetadata


logger = logging.getLogger(__name__)


def save_key_bundle(
    bundle: KeyBundle,
    create_dirs: bool = True,
) -> Path:
    """Save a key bundle's metadata to disk.

    Creates the bundle directory structure and writes ``metadata.json``.
    Key binary files are not written by this function — they are written
    by the key generation step.

    Args:
        bundle: The key bundle to save.
        create_dirs: If True, create the directory structure including
            ``global/`` and ``model/`` subdirectories.

    Returns:
        Path to the bundle directory.
    """
    bundle_path = bundle.path
    if create_dirs:
        bundle_path.mkdir(parents=True, exist_ok=True)
        # Create key subdirectories
        if bundle.key_type == "encryption":
            bundle.global_dir.mkdir(parents=True, exist_ok=True)
        elif bundle.key_type == "evaluation":
            bundle.model_dir.mkdir(parents=True, exist_ok=True)
        else:
            # Create both for combined bundles
            bundle.global_dir.mkdir(parents=True, exist_ok=True)
            bundle.model_dir.mkdir(parents=True, exist_ok=True)

    # Write metadata.json
    metadata_path = bundle_path / "metadata.json"
    with open(metadata_path, "w", encoding="utf-8") as f:
        json.dump(bundle.metadata.to_dict(), f, indent=2, ensure_ascii=False)

    logger.info("Saved key bundle metadata to %s", metadata_path)
    return bundle_path


def load_key_bundle(bundle_path: str | Path) -> KeyBundle:
    """Load a key bundle from a directory.

    Args:
        bundle_path: Path to the key bundle directory (must contain
            ``metadata.json``).

    Returns:
        The loaded key bundle.

    Raises:
        FileNotFoundError: If the directory or metadata.json doesn't exist.
        ValueError: If metadata.json is invalid.
    """
    bundle_path = Path(bundle_path).resolve()
    metadata_path = bundle_path / "metadata.json"

    if not bundle_path.is_dir():
        raise FileNotFoundError(f"Key bundle directory not found: {bundle_path}")
    if not metadata_path.exists():
        raise FileNotFoundError(f"metadata.json not found in {bundle_path}")

    with open(metadata_path, encoding="utf-8") as f:
        data = json.load(f)

    metadata = KeyBundleMetadata.from_dict(data)
    return KeyBundle(path=bundle_path, metadata=metadata)


def list_keys(key_dir: str | Path = "./keys/") -> list[dict[str, Any]]:
    """List all key bundles in a directory.

    Scans the key directory for subdirectories containing ``metadata.json``
    and returns a summary for each.

    Args:
        key_dir: Directory containing key bundle subdirectories.
            Defaults to ``./keys/``.

    Returns:
        List of metadata dicts, one per key bundle found.
    """
    key_dir = Path(key_dir).resolve()
    if not key_dir.is_dir():
        logger.info("Key directory does not exist: %s", key_dir)
        return []

    results = []
    for entry in sorted(key_dir.iterdir()):
        if not entry.is_dir():
            continue
        metadata_path = entry / "metadata.json"
        if not metadata_path.exists():
            continue
        try:
            with open(metadata_path, encoding="utf-8") as f:
                data = json.load(f)
            metadata = KeyBundleMetadata.from_dict(data)
            results.append(metadata.to_dict())
        except (json.JSONDecodeError, KeyError, ValueError) as e:
            logger.warning("Skipping invalid metadata in %s: %s", entry, e)

    return results


def get_key_info(
    key_id: str,
    key_dir: str | Path = "./keys/",
) -> dict[str, Any]:
    """Get detailed information about a specific key bundle.

    Args:
        key_id: The key identifier to look up.
        key_dir: Directory containing key bundle subdirectories.
            Defaults to ``./keys/``.

    Returns:
        Detailed info dict including key file sizes.

    Raises:
        FileNotFoundError: If the key bundle is not found.
    """
    key_dir = Path(key_dir).resolve()
    bundle_path = key_dir / key_id

    if not bundle_path.is_dir():
        raise FileNotFoundError(f"Key bundle '{key_id}' not found in {key_dir}")

    bundle = load_key_bundle(bundle_path)
    return bundle.summary()


def resolve_output_dir(
    output_dir: str | Path,
    key_id: str | None = None,
) -> tuple[Path, str]:
    """Resolve the output directory and key_id for a key bundle.

    If output_dir already contains a metadata.json, the key_id is read
    from it. Otherwise, a new key_id is generated if not provided.

    Args:
        output_dir: Target output directory.
        key_id: Optional key identifier.

    Returns:
        Tuple of (resolved_path, key_id).
    """
    output_dir = Path(output_dir).resolve()

    if output_dir.exists() and (output_dir / "metadata.json").exists():
        # Existing bundle — read key_id from metadata
        bundle = load_key_bundle(output_dir)
        return output_dir, bundle.key_id

    # New bundle — use provided key_id or derive from directory name
    if key_id is None:
        key_id = output_dir.name

    return output_dir, key_id
