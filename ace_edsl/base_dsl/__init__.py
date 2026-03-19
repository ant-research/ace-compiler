# Local module imports
from .dsl import *
from .runtime import *
from functools import lru_cache as lru_cache_ir  # Compatibility alias
from .env_manager import get_env_var, detect_gpu_arch
from .utils.numpy import *
from .loc import (
    set_glob_scope, get_source_loc, set_current_loc, 
    SourceLoc, find_user_frame, register_file
)
