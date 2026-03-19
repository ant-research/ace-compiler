"""
Source Location Tracking for AIR Generation
=============================================

Provides utilities to track Python source locations and propagate them
to AIR nodes for better debugging and error messages.

Captures Python source locations and attaches them to AIR nodes for debugging.

Main API:
    find_user_frame() - Find the first stack frame outside ace_edsl library
    register_file(filename) - Register a source file and get its ID
    set_glob_scope(glob) - Set the GlobScope for file registration

Usage in AIRValue:
    from ace_edsl.base_dsl.loc import find_user_frame, register_file
    
    def _set_loc(self):
        user_frame = find_user_frame()
        if user_frame is not None:
            file_id = register_file(user_frame.f_code.co_filename)
            self._container.set_loc(file_id, user_frame.f_lineno, 0)

Library vs User Code:
    Library code (filtered): ace_edsl/base_dsl/, ace_edsl/edsl/
    User code (captured): ace_edsl/tests/, ace_edsl/examples/, external files

In AIR Dump:
    Source locations appear on statement nodes as LINE(line:col:count):
    st "__ret_tmp_0" VAR[...] ID(...) LINE(61:0:0)
"""

import inspect
from functools import wraps
from typing import Any, Optional, Tuple, Dict
from dataclasses import dataclass


@dataclass
class SourceLoc:
    """Python source location information."""
    filename: str
    line: int
    col: int = 0
    func_name: str = ""
    
    def __str__(self) -> str:
        if self.func_name:
            return f"{self.filename}:{self.line} ({self.func_name})"
        return f"{self.filename}:{self.line}"


# Global file ID cache: filename -> file_id
_file_id_cache: Dict[str, int] = {}

# Global glob_scope reference for file registration
_glob_scope: Any = None


def set_glob_scope(glob: Any):
    """Set the global GlobScope for file registration."""
    global _glob_scope
    _glob_scope = glob


def get_glob_scope() -> Any:
    """Get the global GlobScope."""
    return _glob_scope


def register_file(filename: str) -> int:
    """
    Register a source file and return its ID.
    
    File IDs are cached to avoid repeated registration.
    """
    global _file_id_cache, _glob_scope
    
    if filename in _file_id_cache:
        return _file_id_cache[filename]
    
    if _glob_scope is not None and hasattr(_glob_scope, 'register_file'):
        file_id = _glob_scope.register_file(filename)
        _file_id_cache[filename] = file_id
        return file_id
    
    # Fallback: use hash of filename as pseudo-ID
    file_id = hash(filename) & 0x7FFFFFFF  # Keep it positive
    _file_id_cache[filename] = file_id
    return file_id


def get_current_frame(skip: int = 1) -> Optional[Any]:
    """
    Get the current stack frame, skipping internal frames.
    
    Args:
        skip: Number of frames to skip (1 = caller, 2 = caller's caller, etc.)
        
    Returns:
        Frame object or None if not available
    """
    try:
        frame = inspect.currentframe()
        for _ in range(skip + 1):  # +1 to skip this function itself
            if frame is None:
                return None
            frame = frame.f_back
        return frame
    except Exception:
        return None


# Cache the ace_edsl library paths for filtering (internal library code)
_ACE_EDSL_LIB_PATHS: Optional[Tuple[str, ...]] = None


def _get_ace_edsl_lib_paths() -> Tuple[str, ...]:
    """
    Get the ace_edsl internal library paths (cached).
    
    Internal library paths are directories containing DSL implementation code.
    User code in tests/ or examples/ should NOT be filtered.
    """
    global _ACE_EDSL_LIB_PATHS
    if _ACE_EDSL_LIB_PATHS is None:
        import os
        # This file is in ace_edsl/base_dsl/loc.py
        base_dsl_dir = os.path.dirname(os.path.abspath(__file__))  # base_dsl/
        ace_edsl_dir = os.path.dirname(base_dsl_dir)  # ace_edsl/
        
        # Internal library paths to filter (these contain DSL implementation)
        _ACE_EDSL_LIB_PATHS = (
            os.path.join(ace_edsl_dir, "base_dsl"),
            os.path.join(ace_edsl_dir, "edsl"),
        )
    return _ACE_EDSL_LIB_PATHS


def _is_library_frame(filename: str) -> bool:
    """
    Check if a filename belongs to the ace_edsl internal library.
    
    Returns True for internal library code (base_dsl/, edsl/),
    False for user code (tests/, examples/, external files).
    """
    lib_paths = _get_ace_edsl_lib_paths()
    for lib_path in lib_paths:
        if filename.startswith(lib_path):
            return True
    return False


def find_user_frame() -> Optional[Any]:
    """
    Walk up the call stack to find the first frame outside the ace_edsl library.
    
    This is useful for capturing source location from the user's code rather
    than from within the DSL library internals.
    
    Library code includes: ace_edsl/base_dsl/, ace_edsl/edsl/
    User code includes: ace_edsl/tests/, ace_edsl/examples/, any external files
    
    Returns:
        Frame object for the user's code, or None if not found
    """
    try:
        frame = inspect.currentframe()
        
        while frame is not None:
            frame = frame.f_back
            if frame is None:
                break
            
            filename = frame.f_code.co_filename
            # Skip frames from internal ace_edsl library
            if not _is_library_frame(filename):
                return frame
        
        return None
    except Exception:
        return None


def get_source_loc(skip: int = 1) -> Optional[SourceLoc]:
    """
    Get source location from the current call stack.
    
    Args:
        skip: Number of frames to skip (1 = caller, 2 = caller's caller, etc.)
        
    Returns:
        SourceLoc or None if not available
    """
    frame = get_current_frame(skip + 1)  # +1 to skip this function
    if frame is None:
        return None
    
    return SourceLoc(
        filename=frame.f_code.co_filename,
        line=frame.f_lineno,
        col=0,
        func_name=frame.f_code.co_name
    )


def set_current_loc(container: Any, skip: int = 2) -> Optional[SourceLoc]:
    """
    Set the current source location on a Container.
    
    This should be called before creating AIR nodes to attach
    source location information to them.
    
    Args:
        container: AIR Container object
        skip: Number of frames to skip to find the user's code
        
    Returns:
        SourceLoc that was set, or None if failed
    """
    loc = get_source_loc(skip + 1)  # +1 to skip this function
    if loc is None:
        return None
    
    if container is not None and hasattr(container, 'set_loc'):
        file_id = register_file(loc.filename)
        container.set_loc(file_id, loc.line, loc.col)
    
    return loc


def with_source_loc(func):
    """
    Decorator that automatically sets source location before calling a function.
    
    Usage:
        @with_source_loc
        def new_add(container, a, b):
            return container.new_add(a, b)
    
    The decorator expects the first argument to be a container (or an object
    with a container attribute '_container').
    """
    @wraps(func)
    def wrapper(*args, **kwargs):
        # Find container in args
        container = None
        if len(args) > 0:
            arg0 = args[0]
            if hasattr(arg0, 'set_loc'):
                container = arg0
            elif hasattr(arg0, '_container'):
                container = arg0._container
        
        # Set source location from caller
        if container is not None:
            set_current_loc(container, skip=2)
        
        return func(*args, **kwargs)
    
    return wrapper


class LocTracker:
    """
    Context manager and utility for tracking source locations.
    
    Usage:
        tracker = LocTracker(container)
        
        # Automatic tracking in AIRValue operations
        tracker.track()  # Sets loc from caller
        result = container.new_add(a, b)
    """
    
    def __init__(self, container: Any, glob: Any = None):
        self.container = container
        self.glob = glob
        if glob is not None:
            set_glob_scope(glob)
    
    def track(self, skip: int = 1) -> Optional[SourceLoc]:
        """Set source location from the current call stack."""
        return set_current_loc(self.container, skip=skip + 1)
    
    def set(self, filename: str, line: int, col: int = 0):
        """Set a specific source location."""
        if self.container is not None and hasattr(self.container, 'set_loc'):
            file_id = register_file(filename)
            self.container.set_loc(file_id, line, col)
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        return False


def clear_file_cache():
    """Clear the file ID cache (for testing)."""
    global _file_id_cache
    _file_id_cache.clear()

