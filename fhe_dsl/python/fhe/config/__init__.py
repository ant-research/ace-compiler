# config/__init__.py
"""FHE configuration and options."""

from .compile_options import CompileOptions
from .compute_options import ComputeOptions
from .default_options import clear_env_options, get_compile_options, set_env_options
from .io_config import DataPath, IoConfig, IoExecMode, IoPolicy, IoPolicySettings, IoProfile
from .key_mode import KeyMode
from .profiler import ReLUProfiler
from .spec import FuncSpec, ModelSpec


# Backward compatibility alias
InferenceMode = KeyMode


__all__ = [
    "CompileOptions",
    "ComputeOptions",
    "DataPath",
    "FuncSpec",
    "InferenceMode",  # backward compat alias
    "IoConfig",
    "IoExecMode",
    "IoPolicy",
    "IoPolicySettings",
    "IoProfile",
    "KeyMode",
    "ModelSpec",
    "ReLUProfiler",
    "clear_env_options",
    "get_compile_options",
    "set_env_options",
]
