#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

from typing import List, Tuple, Any
import logging

logger = logging.getLogger(__name__)

# ======================
# Key Types (for clarity and extensibility)
# ======================

class KeyType:
    PRIVATE_KEY = "PRIVATE_KEY"
    PUBLIC_KEY  = "PUBLIC_KEY"
    RELIN_KEY   = "RELIN_KEY"
    ROTATE_KEY  = "ROTATE_KEY"

# ======================
# Key Manager
# ======================

class KeyManager:
    """
    Manages FHE cryptographic keys and exports them to the data entry list.
    
    Keys are fetched from the C++ context and stored in a shared data registry
    for later serialization or debugging.
    """

    def __init__(self, ctx, data_entry: List[Tuple[str, str, Any, int]], keep: bool = False):
        """
        Initialize the key manager.

        Args:
            ctx: C++ FHE context object (from ace_torch_ext).
            data_entry: Shared list to store key tensors in format (type, name, tensor, q_count).
            keep: Reserved for future debug support (currently unused).
        """
        self.ctx = ctx
        self.data_entry = data_entry

        # Initialize FHE context and export default keys
        self.ctx.init_fhe_context()
        self._export_default_keys()

    def _export_default_keys(self) -> None:
        """Export standard FHE keys required for computation."""
        # Always export these keys (matches original behavior)
        self._add_key(KeyType.PRIVATE_KEY, "pri", self.ctx.get_priv_key())
        self._add_key(KeyType.PUBLIC_KEY, "pub", self.ctx.get_pub_key())
        self._add_key(KeyType.RELIN_KEY, "relin", self.ctx.get_relin_key())

        # Rotation key is optional (may be empty)
        rotate_key = self.ctx.get_rotate_key()
        if rotate_key:
            self._add_key(KeyType.ROTATE_KEY, "rot", rotate_key)

    def _add_key(self, key_type: str, short_name: str, tensor) -> None:
        """
        Append a key to the shared data entry list.

        Format: (key_type, short_name, tensor, q_count=0)
        """
        if tensor is not None:
            self.data_entry.append((key_type, short_name, tensor, 0))
            logger.debug(f"Exported key: {key_type} ({short_name})")
        else:
            logger.warning(f"Skipped empty key: {key_type}")

    # Optional: Public method for custom key export (future-proofing)
    def export_key(self, key_type: str, name: str, getter_func) -> None:
        """
        Export a custom key using a getter function.
        
        Example:
            km.export_key("BOOTSTRAP_KEY", "bsk", ctx.get_bootstrap_key)
        """
        try:
            tensor = getter_func()
            self._add_key(key_type, name, tensor)
        except Exception as e:
            logger.error(f"Failed to export key {key_type}: {e}")
