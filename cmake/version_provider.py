"""Custom version provider for scikit-build-core.

Reads the base version from ``fhe_dsl/python/_version.py`` and appends
a CUDA/CPU variant as a PEP 440 local suffix when ``ACE_LOCAL_VERSION``
is explicitly set (e.g. ``0.2.2+cu128``).

Without ``ACE_LOCAL_VERSION``, returns the base version only (e.g.
``0.2.2``), which is required for PyPI uploads since PyPI rejects local
version identifiers.

Used by pyproject.toml::

    [tool.scikit-build.metadata.version]
    provider = "version_provider"
    provider-path = "cmake"

Environment variables:

    ACE_LOCAL_VERSION
        When set, appended as a PEP 440 local suffix (e.g. ``cu128``,
        ``cpu``). When unset, the base version is returned without a
        local suffix (for PyPI compatibility).
"""

import os
import re
from pathlib import Path


def dynamic_metadata(field, settings):
    if field != "version":
        msg = "Only version is supported"
        raise ValueError(msg)

    version_file = Path(__file__).resolve().parent.parent / "fhe_dsl" / "python" / "_version.py"
    regex = r'(?i)^__version__\s*=\s*[\'"]v?(.+?)[\'"]'
    match = re.search(regex, version_file.read_text(), re.MULTILINE)
    if not match:
        msg = f"Couldn't find version in {version_file}"
        raise RuntimeError(msg)
    base_version = match.group(1)

    local = os.environ.get("ACE_LOCAL_VERSION", "")
    if local:
        return f"{base_version}+{local}"

    return base_version


def get_requires_for_dynamic_metadata(settings):
    return []
