# config/compute_options.py
from dataclasses import dataclass
from typing import Callable, List, Union, Any, Optional

from .compile_options import CompileOptions

@dataclass
class ComputeOptions(CompileOptions):
    """
    Compute-time configuration options (extends CompileOptions).
    
    These options control both compilation and execution behavior.
    """
    # Add compute-specific fields
    # validate_result: bool = True
    # max_retries: int = 3
    validate: bool = True
    server_url: Optional[str] = None
    # ... other execution parameters
