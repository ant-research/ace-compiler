# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Key bundle data structures for FHE key management.

A key bundle is a directory containing FHE keys and a metadata.json file
that describes the keys. There are two types of key bundles:

    - **Encryption key bundle**: Contains secret_key.bin and public_key.bin.
      Model-independent, depends only on CKKS parameters.

    - **Evaluation key bundle**: Contains relin_key.bin, galois_key.bin,
      and optionally bootstrap_key.bin. Model-specific, depends on rotation
      indices and multiplication depth from the compiled model.

Directory layout::

    key_bundle/
    ├── metadata.json          # Key metadata (creation time, params, etc.)
    ├── global/
    │   ├── secret_key.bin     # SK (confidential)
    │   └── public_key.bin     # PK (shareable)
    └── model/
        ├── relin_key.bin      # Relinearization key
        ├── galois_key.bin     # Galois (rotation) key
        └── bootstrap_key.bin  # Bootstrap key (optional)
"""

import json
import logging
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Optional

from .config_reader import CKKSParams


logger = logging.getLogger(__name__)


@dataclass
class KeyBundleMetadata:
    """Metadata for an FHE key bundle.

    Serialized as ``metadata.json`` in the key bundle directory.

    Attributes:
        key_id: Unique identifier for this key bundle.
        created_at: ISO 8601 timestamp of creation.
        key_type: Type of key bundle — ``"encryption"`` or ``"evaluation"``.
        ckks_params: CKKS cryptographic parameters.
        model_name: Name of the compiled model (evaluation keys only).
        conf_source: Path to the .conf file used (evaluation keys only).
        key_files: Dict mapping key type to relative file path.
        description: Optional human-readable description.
    """

    key_id: str
    created_at: str
    key_type: str  # "encryption" or "evaluation"
    ckks_params: dict[str, Any]
    model_name: str | None = None
    conf_source: str | None = None
    key_files: dict[str, str] = field(default_factory=dict)
    description: str = ""

    def to_dict(self) -> dict[str, Any]:
        """Serialize metadata to a plain dictionary."""
        d: dict[str, Any] = {
            "key_id": self.key_id,
            "created_at": self.created_at,
            "key_type": self.key_type,
            "ckks_params": self.ckks_params,
        }
        if self.model_name is not None:
            d["model_name"] = self.model_name
        if self.conf_source is not None:
            d["conf_source"] = self.conf_source
        if self.key_files:
            d["key_files"] = self.key_files
        if self.description:
            d["description"] = self.description
        return d

    @classmethod
    def from_dict(cls, d: dict[str, Any]) -> "KeyBundleMetadata":
        """Deserialize metadata from a dictionary."""
        return cls(
            key_id=d["key_id"],
            created_at=d["created_at"],
            key_type=d["key_type"],
            ckks_params=d["ckks_params"],
            model_name=d.get("model_name"),
            conf_source=d.get("conf_source"),
            key_files=d.get("key_files", {}),
            description=d.get("description", ""),
        )


@dataclass
class KeyBundle:
    """An FHE key bundle — a directory containing keys and metadata.

    Attributes:
        path: Absolute path to the key bundle directory.
        metadata: Bundle metadata.
    """

    path: Path
    metadata: KeyBundleMetadata

    @property
    def key_id(self) -> str:
        """Key identifier."""
        return self.metadata.key_id

    @property
    def key_type(self) -> str:
        """Key bundle type (``"encryption"`` or ``"evaluation"``)."""
        return self.metadata.key_type

    @property
    def metadata_path(self) -> Path:
        """Path to metadata.json."""
        return self.path / "metadata.json"

    @property
    def global_dir(self) -> Path:
        """Directory for encryption keys (SK + PK)."""
        return self.path / "global"

    @property
    def model_dir(self) -> Path:
        """Directory for evaluation keys (Relin + Galois + Bootstrap)."""
        return self.path / "model"

    @property
    def secret_key_path(self) -> Path:
        """Path to secret_key.bin."""
        return self.global_dir / "secret_key.bin"

    @property
    def public_key_path(self) -> Path:
        """Path to public_key.bin."""
        return self.global_dir / "public_key.bin"

    @property
    def relin_key_path(self) -> Path:
        """Path to relin_key.bin."""
        return self.model_dir / "relin_key.bin"

    @property
    def galois_key_path(self) -> Path:
        """Path to galois_key.bin."""
        return self.model_dir / "galois_key.bin"

    @property
    def bootstrap_key_path(self) -> Path:
        """Path to bootstrap_key.bin."""
        return self.model_dir / "bootstrap_key.bin"

    def key_file_path(self, key_name: str) -> Path:
        """Get the path for a specific key file.

        Args:
            key_name: One of ``"secret_key"``, ``"public_key"``,
                ``"relin_key"``, ``"galois_key"``, ``"bootstrap_key"``.

        Returns:
            Absolute path to the key file.

        Raises:
            ValueError: If key_name is not recognized.
        """
        mapping = {
            "secret_key": self.secret_key_path,
            "public_key": self.public_key_path,
            "relin_key": self.relin_key_path,
            "galois_key": self.galois_key_path,
            "bootstrap_key": self.bootstrap_key_path,
        }
        if key_name not in mapping:
            raise ValueError(f"Unknown key name: {key_name}. Expected one of: {list(mapping.keys())}")
        return mapping[key_name]

    def has_encryption_keys(self) -> bool:
        """Check if encryption keys (SK + PK) exist on disk."""
        return self.secret_key_path.exists() and self.public_key_path.exists()

    def has_evaluation_keys(self) -> bool:
        """Check if evaluation keys exist on disk."""
        return self.relin_key_path.exists() and self.galois_key_path.exists()

    def summary(self) -> dict[str, Any]:
        """Return a summary dict for display (used by ``ace_tool kms info``)."""
        result: dict[str, Any] = {
            "key_id": self.key_id,
            "key_type": self.key_type,
            "created_at": self.metadata.created_at,
            "ckks_params": self.metadata.ckks_params,
        }
        if self.metadata.model_name:
            result["model_name"] = self.metadata.model_name
        if self.metadata.conf_source:
            result["conf_source"] = self.metadata.conf_source

        # Key file status with sizes
        key_status = {}
        for name, rel_path in self.metadata.key_files.items():
            abs_path = self.path / rel_path
            if abs_path.exists():
                key_status[name] = {
                    "path": str(abs_path),
                    "size_bytes": abs_path.stat().st_size,
                    "size_mb": round(abs_path.stat().st_size / (1024 * 1024), 2),
                    "exists": True,
                }
            else:
                key_status[name] = {
                    "path": str(abs_path),
                    "exists": False,
                }
        if key_status:
            result["key_files"] = key_status

        if self.metadata.description:
            result["description"] = self.metadata.description

        return result


def make_encryption_metadata(
    ckks_params: CKKSParams,
    key_id: str | None = None,
    description: str = "",
) -> KeyBundleMetadata:
    """Create metadata for an encryption key bundle.

    Args:
        ckks_params: CKKS parameters used for key generation.
        key_id: Optional key identifier. Defaults to timestamp-based ID.
        description: Optional human-readable description.

    Returns:
        Metadata for the encryption key bundle.
    """
    if key_id is None:
        key_id = f"enc-{datetime.now(timezone.utc).strftime('%Y%m%d-%H%M%S')}"

    return KeyBundleMetadata(
        key_id=key_id,
        created_at=datetime.now(timezone.utc).isoformat(),
        key_type="encryption",
        ckks_params=ckks_params.to_dict(),
        key_files={
            "secret_key": "global/secret_key.bin",
            "public_key": "global/public_key.bin",
        },
        description=description,
    )


def make_evaluation_metadata(
    ckks_params: CKKSParams,
    model_name: str | None = None,
    conf_source: str | None = None,
    key_id: str | None = None,
    description: str = "",
) -> KeyBundleMetadata:
    """Create metadata for an evaluation key bundle.

    Args:
        ckks_params: CKKS parameters from the compiled model's .conf file.
        model_name: Name of the compiled model.
        conf_source: Path to the .conf file used.
        key_id: Optional key identifier. Defaults to timestamp-based ID.
        description: Optional human-readable description.

    Returns:
        Metadata for the evaluation key bundle.
    """
    if key_id is None:
        key_id = f"eval-{datetime.now(timezone.utc).strftime('%Y%m%d-%H%M%S')}"

    key_files: dict[str, str] = {
        "relin_key": "model/relin_key.bin",
        "galois_key": "model/galois_key.bin",
    }
    # Include bootstrap key only if mul_depth > 0 (bootstrap is needed)
    if ckks_params.mul_depth > 0:
        key_files["bootstrap_key"] = "model/bootstrap_key.bin"

    return KeyBundleMetadata(
        key_id=key_id,
        created_at=datetime.now(timezone.utc).isoformat(),
        key_type="evaluation",
        ckks_params=ckks_params.to_dict(),
        model_name=model_name,
        conf_source=conf_source,
        key_files=key_files,
        description=description,
    )
