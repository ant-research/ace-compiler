"""Backward compatibility shim — use ace.fhe.config.KeyMode instead.

This module is deprecated. Import from ace.fhe.config import KeyMode instead.
"""

from .key_mode import KeyMode


# Alias for backward compatibility
InferenceMode = KeyMode

__all__ = ["InferenceMode", "KeyMode"]
