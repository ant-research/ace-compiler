#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

from dataclasses import dataclass
from typing import Optional

@dataclass
class VECOption:
    """Configuration for the VEC scheme."""
    ms: int                             # Plaintext modulus

@dataclass
class CKKSOption:
    """Configuration for the CKKS scheme."""
    N: int                              # Polynomial ring dimension (must be a power of two)
    scale: Optional[int] = None         # Scaling factor
    precision: Optional[int] = None     # Precision (number of fractional bits)

@dataclass
class Option:
    """
    Container for user-defined FHE configuration.
    Each field corresponds to a specific scheme; None means the scheme is disabled.
    """
    vec: Optional[VECOption] = None
    ckks: Optional[CKKSOption] = None

    def __post_init__(self):
        """Optional: Validate configuration correctness."""
        if self.ckks and self.ckks.N & (self.ckks.N - 1) != 0:
            raise ValueError(f"CKKS N must be a power of two, got {self.ckks.N}")
