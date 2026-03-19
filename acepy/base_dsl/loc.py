"""
Source Location Tracking
========================

Provides source location tracking for error messages and debugging.
"""

from dataclasses import dataclass
from typing import Optional, List
from contextlib import contextmanager
import threading
import sys


@dataclass(frozen=True)
class Loc:
    """Source location for error messages and debugging."""
    line: int
    col: int
    filename: str
    end_line: Optional[int] = None
    end_col: Optional[int] = None
    
    @staticmethod
    def unknown() -> 'Loc':
        """Create an unknown location."""
        return Loc(0, 0, "<unknown>")
    
    def __str__(self) -> str:
        if self.end_line and self.end_line != self.line:
            return f"{self.filename}:{self.line}:{self.col}-{self.end_line}:{self.end_col}"
        return f"{self.filename}:{self.line}:{self.col}"
    
    def format_error(self, message: str, source_lines: List[str] = None) -> str:
        """Format error message with source context."""
        result = f"\n{self}: error: {message}\n"
        
        if source_lines and 0 < self.line <= len(source_lines):
            # Show source context (3 lines before and 2 after)
            start = max(0, self.line - 3)
            end = min(len(source_lines), self.line + 2)
            
            for i in range(start, end):
                prefix = ">>> " if i == self.line - 1 else "    "
                result += f"{prefix}{i+1:4d} | {source_lines[i].rstrip()}\n"
            
            # Show caret pointing to column
            if self.col > 0:
                result += f"         {' ' * self.col}^\n"
        
        return result


# Thread-local storage for current location context
_loc_context = threading.local()


def get_current_loc() -> Loc:
    """Get the current source location from thread-local context."""
    return getattr(_loc_context, 'loc', Loc.unknown())


def set_current_loc(loc: Loc):
    """Set the current source location in thread-local context."""
    _loc_context.loc = loc


def get_caller_loc(depth: int = 1) -> Loc:
    """
    Get source location of the caller.
    
    Args:
        depth: How many frames to go up (1 = direct caller, 2 = caller's caller)
    
    Returns:
        Loc with the caller's source position
    """
    frame = sys._getframe(depth + 1)  # +1 for this function
    
    try:
        filename = frame.f_code.co_filename
        lineno = frame.f_lineno
        
        return Loc(
            line=lineno,
            col=0,  # Column not available from frame
            filename=filename
        )
    finally:
        del frame  # Avoid reference cycles


@contextmanager
def _source_location_context(loc: Loc):
    """
    Context manager to set current source location.
    
    Usage:
        with source_location(loc):
            # All operations here use this location
            result = a + b
    """
    old_loc = getattr(_loc_context, 'loc', None)
    _loc_context.loc = loc
    try:
        yield
    finally:
        _loc_context.loc = old_loc


def source_location(func_or_loc):
    """
    Decorator or context manager for source location tracking.
    
    As a decorator:
        @source_location
        def my_method(self, other):
            ...
    
    As a context manager:
        with source_location(loc):
            ...
    """
    if isinstance(func_or_loc, Loc):
        # Used as context manager with location
        return _source_location_context(func_or_loc)
    elif callable(func_or_loc):
        # Used as decorator
        from functools import wraps
        
        @wraps(func_or_loc)
        def wrapper(*args, **kwargs):
            # Get caller's location
            loc = get_caller_loc(depth=1)
            with _source_location_context(loc):
                return func_or_loc(*args, **kwargs)
        return wrapper
    else:
        raise TypeError(f"source_location expects a callable or Loc, got {type(func_or_loc)}")

