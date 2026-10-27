# config/base.py
from dataclasses import dataclass
from typing import Any, Optional, Union


@dataclass
class BaseOption:
    """Base class for all FHE configuration options."""

    # encrypt_inputs: Optional[Union[list[str], list[int]]] = None
    verbose: bool = False
    # Add common fields here if needed

    def to_compiler_options(self) -> dict[str, Any]:
        """
        Extract compiler options as a dict.

        Returns:
            dict with non-None compiler option keys (vec, ckks, sihe, p2c, o2a, fhe_scheme, poly)
        """
        result = {}
        for key in ["vec", "ckks", "sihe", "p2c", "o2a", "fhe_scheme", "poly"]:
            value = getattr(self, key, None)
            if value is not None:
                result[key] = value
        return result
