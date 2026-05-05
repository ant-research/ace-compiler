# config/base.py
from dataclasses import dataclass
from typing import Optional, List, Union, Dict, Any

@dataclass
class BaseOption:
    """Base class for all FHE configuration options."""
    # encrypt_inputs: Optional[Union[List[str], List[int]]] = None
    verbose: bool = False
    # Add common fields here if needed

    def to_compiler_options(self) -> Dict[str, Any]:
        """
        Extract compiler options as a dict.

        Returns:
            Dict with non-None compiler option keys (vec, ckks, sihe, p2c, o2a, fhe_scheme, poly)
        """
        result = {}
        for key in ['vec', 'ckks', 'sihe', 'p2c', 'o2a', 'fhe_scheme', 'poly']:
            value = getattr(self, key, None)
            if value is not None:
                result[key] = value
        return result
