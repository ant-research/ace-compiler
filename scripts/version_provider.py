"""Custom version provider for scikit-build-core.

Automatically detects CUDA/CPU version from PyTorch and appends
as a local version identifier (e.g., 0.2.0+cu124, 0.2.0+cpu).

Used by pyproject.toml:
    [tool.scikit-build.metadata.version]
    provider = "version_provider"

Environment variables:
    ACE_LOCAL_VERSION  If set, use this as the local tag (e.g. cu124, cpu)
                       instead of auto-detecting from torch.
"""
import os
import re
from pathlib import Path


def dynamic_metadata(field, settings):
    if field != "version":
        msg = "Only version is supported"
        raise ValueError(msg)

    # Read base version from _version.py using regex (don't import, avoid side effects)
    version_file = Path(__file__).resolve().parent.parent / "fhe_dsl" / "ace" / "_version.py"
    regex = r'(?i)^__version__\s*=\s*[\'"]v?(.+?)[\'"]'
    match = re.search(regex, version_file.read_text(), re.MULTILINE)
    if not match:
        msg = f"Couldn't find version in {version_file}"
        raise RuntimeError(msg)
    base_version = match.group(1)

    # Determine local tag from env var or torch detection
    local = os.environ.get("ACE_LOCAL_VERSION", "")
    if not local:
        try:
            import torch
            if torch.version.cuda:
                local = "cu" + torch.version.cuda.replace(".", "")
            else:
                local = "cpu"
        except ImportError:
            local = "cpu"

    return f"{base_version}+{local}"


def get_requires_for_dynamic_metadata(settings):
    return []