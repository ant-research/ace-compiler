#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import json
import logging
from pathlib import Path
from typing import Any, Dict

from ._ext import get_module

logger = logging.getLogger(__name__)

# ======================
# Configuration Loader
# ======================

class FHEConfigLoader:
    """Parse JSON configuration and bind to C++ FHE runtime."""

    # Field keys (avoid string literals scattered in code)
    _CKKS_FIELDS = {
        "provider"          : "_provider",
        "poly_degree"       : "_poly_degree",
        "sec_level"         : "_sec_level",
        "mul_depth"         : "_mul_depth",
        "input_level"       : "_input_level",
        "first_mod_size"    : "_first_mod_size",
        "scaling_mod_size"  : "_scaling_mod_size",
        "num_q_parts"       : "_num_q_parts",
        "hamming_weight"    : "_hamming_weight",
        "num_rot_idx"       : "_num_rot_idx",
        "rot_idxs"          : "_rot_idxs",
    }

    def __init__(self, config_path: str):
        self.config_path = Path(config_path).resolve()
        if not self.config_path.exists():
            raise FileNotFoundError(f"Config file not found: {self.config_path}")
        
        self.raw_data = self._load_json()
        get_module().set_fhe_config(self.raw_data)

        # Fast verification
        if not get_module().validate_config():
            logger.error("Config errors:")
            logger.error(get_module().get_validation_errors())
            exit(1)

        # Print configuration summary
        logger.info(get_module().get_config_summary())

    def _load_json(self) -> Dict[str, Any]:
        """Load and validate JSON configuration."""
        try:
            with open(self.config_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
            data["model"] = self.config_path.stem
            return data
        except json.JSONDecodeError as e:
            raise ValueError(f"Invalid JSON in {self.config_path}: {e}") from e
