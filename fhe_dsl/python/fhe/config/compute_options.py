# config/compute_options.py
from collections.abc import Callable
from dataclasses import dataclass
from typing import Any, Optional, Union

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
    server_url: str | None = None
    # ... other execution parameters
